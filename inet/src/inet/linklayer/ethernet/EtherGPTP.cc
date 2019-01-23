//
// @authors: Enkhtuvshin Janchivnyambuu
//           Henning Puttnies
//           Peter Danielis
//           University of Rostock, Germany
// Modifier: Hathaway Le

#include <inet/linklayer/ethernet/EtherGPTP.h>
#include <inet/linklayer/ethernet/gPtp.h>
#include <inet/linklayer/ethernet/gPtpPacket_m.h>
#include "inet/linklayer/common/Ieee802Ctrl_m.h"
#include "inet/transportlayer/contract/udp/UDPControlInfo_m.h"
#include <omnetpp.h>
#include <inet/linklayer/ethernet/tableGPTP.h>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <list>
#include <utility>

#include "inet/linklayer/ethernet/EtherMACBase.h"
#include "inet/linklayer/ethernet/EtherFrame_m.h"
#include "inet/linklayer/common/MACAddress.h"
#include "inet/linklayer/base/MACBase.h"
using namespace omnetpp;
namespace inet {
    Define_Module(EtherGPTP);

    EtherGPTP::EtherGPTP()
    {
        selfMsgSync = NULL;
        selfMsgDelayResp = NULL;
        selfMsgFollowUp = NULL;
        selfMsgRespFollow = NULL;
    }

    void EtherGPTP::initialize(int stage)
    {
        if(stage == 1)//stage == 0 的时候，时钟还未初始化好
        {
            cModule* gPtpNode=getParentModule()->getParentModule();//爸爸的爸爸，host or bridge 不是endstation(nodebase)
            if(gPtpNode->getSubmodule("tableGPTP")!=nullptr)
            {
                tableGptp = check_and_cast<TableGPTP *>(gPtpNode->getSubmodule("tableGPTP"));
            }
    //        clockGptp= HardwareClock::findFirstClock(gPtpNode);
            if(gPtpNode->getSubmodule("hwClock")!=nullptr)
             {
                clockGptp = check_and_cast<HardwareClock *>(gPtpNode->getSubmodule("hwClock"));
             }

            portType = par("portType");
            nodeType = gPtpNode->par("gPtpNodeType");
            syncInterval = par("syncInterval");

            rateRatio = 1;

            /* following parameters are used to schedule follow_up and pdelay_resp messages.
             * These numbers must be enough large to prevent creating queue in MAC layer.
             * It means it should be large than transmission time of message sent before */
            PDelayRespInterval = 0.0000008;//因为GPTP模块后面是没有队列的，要控制好发送间隔,但也可以加队列，严格安排好时间
            FollowUpInterval = 0.0000007;//发送完sync需要0.000000656（算上IFG）
            RespFollowInterval = 0.0000008;
            /* Only grandmaster in the domain can initialize the synchronization message periodically
             * so below condition checks whether it is grandmaster and then schedule first sync message */
            if(portType == MASTER_PORT && nodeType == MASTER_NODE)//一个小型时间aware网络应该是只有一个grandmaster
            {
                // Schedule Sync message to be sent
                if (NULL == selfMsgSync)
                    selfMsgSync = new cMessage("selfMsgSync");

                //master先发送,前syncIntervalp*2时间用于初始化
                tableGptp->setOriginTimestamp(syncInterval + syncInterval);//最初始的发送时间
                scheduleAt(syncInterval + syncInterval, selfMsgSync);//这个是grandmaster，用理想时间发送倒无所谓
            }
            else if(portType == SLAVE_PORT)
            {
                vTimeDifferenceGMafterSync.setName("Clock difference to GM after Sync");
                vTimeDifferenceGMbeforeSync.setName("Clock difference to GM before Sync");

                requestMsg = new cMessage("requestToSendSync");
             }
        }
    }

    void EtherGPTP::handleMessage(cMessage *msg)
    {
        if(portType == MASTER_PORT)
        {
            masterPort(msg);
        }
        else if(portType == SLAVE_PORT)
        {
            slavePort(msg);
        }
        else
        {
            // Forward message to upperLayerOut gate since packet is not gPtp
            if(msg->arrivedOn("lowerLayerIn"))
            {
                EV_INFO << "EtherGPTP: Received " << msg << " from LOWER LAYER." << endl;
                send(msg, "upperLayerOut");
            }
            else if(msg->arrivedOn("gptpLayerIn"))//table群发的请求，PASSIVE端口无需理睬
            {
                delete msg;
            }
            else
            {
                EV_INFO << "EtherGPTP: Received " << msg << " from UPPER LAYER." << endl;
                send(msg, "lowerLayerOut");//如果端口是passive，gptpLayerIn的帧（requestToSendSync）错误地从这边发送了，他们并不是以太网帧
            }
        }
    }

    /*********************************/
    /* Master port related functions */
    /*********************************/

    void EtherGPTP::masterPort(cMessage *msg)
    {
        if (msg->isSelfMessage())
        {
            if(selfMsgSync == msg)//只有master才主动定时发送sync
            {
                sendSync();
                clockGptp->scheduleAtHWtime(clockGptp->getHWtime()+syncInterval,msg,this);
            }
            else if(selfMsgFollowUp == msg)
            {
                sendFollowUp();
            }
            else if(selfMsgDelayResp == msg)
            {
                sendPdelayResp();
            }
            else if(selfMsgRespFollow == msg)
            {
                sendPdelayRespFollowUp();
            }
        }
        else if (msg->arrivedOn("gptpLayerIn"))
        {
            // Only sync message is sent if its node is bridge
            if(nodeType == BRIDGE_NODE)
            {
                sendSync();
            }
            delete msg;
        }
        else if(msg->arrivedOn("lowerLayerIn"))
        {
            if (inet::EthernetIIFrame* etherFrame = dynamic_cast<inet::EthernetIIFrame*>(msg))
            {
                if(msg->getKind() == PDELAY_REQ)
                {
                    gPtp_PdelayReq* gptp = dynamic_cast<gPtp_PdelayReq*>(etherFrame->decapsulate());
                    processPdelayReq(gptp);
                    delete gptp;
                    delete etherFrame;
                }
                else
                {
                    // Forward message to upperLayerOut gate since packet is not gPtp but eth
                    send(msg, "upperLayerOut");
                }
            }
            else
            {
                // 并不是以太网帧，但是如果不是以太网帧，mac那边是不会传上来的，起码3.2.4这个版本不会
                //send(msg, "upperLayerOut");
            }
        }
        else if (msg->arrivedOn("upperLayerIn"))
        {
            // Forward message to lowerLayerOut gate because the message from upper layer doesn't need to be touched
            // and we are interested in only message from lowerLayerIn
            send(msg, "lowerLayerOut");
        }
    }

    void EtherGPTP::sendSync()
    {
        // Create EtherFrame to be used to encapsulate gPtpPacket because lower layer MAC only supports EtherFrame
        inet::EthernetIIFrame* frame=new inet::EthernetIIFrame("Sync");
        frame->setDest(inet::MACAddress::BROADCAST_ADDRESS);
        frame->setEtherType(inet::ETHERTYPE_IPv4);  // So far INET doesn't support gPTP (etherType = 0x88f7)
        frame->setKind(SYNC);//因为omnet没有field的按字节识别功能，所以message内带kind成员变量
        frame->setByteLength(18);
        gPtp_Sync* gptp =new gPtp_Sync("Sync", inet::IEEE802CTRL_DATA);
        gptp->setName("Sync");

        /* Sync message length 44 byte */
        gptp->setByteLength(SYNC_PACKET_SIZE);
        /* OriginTimestamp always get Sync departure time from grand master */
        if (nodeType == MASTER_NODE)
        {
            tableGptp->setOriginTimestamp(clockGptp->getHWtime());

        }
        else if(nodeType == BRIDGE_NODE)
        {

        }

        sentTimeSync_self = clockGptp->getHWtime();
        gptp->setSentTime(sentTimeSync_self);
        frame->encapsulate(gptp);

        send(frame, "lowerLayerOut");

        if (NULL == selfMsgFollowUp)
            selfMsgFollowUp = new cMessage("selfMsgFollowUp");
        clockGptp->scheduleAtHWtime(clockGptp->getHWtime()+SimTime(FollowUpInterval),selfMsgFollowUp,this);
    //    scheduleAt(simTime() + clockGptp->HwtoSw_interval(SimTime(FollowUpInterval)), selfMsgFollowUp);//followup的间隔应该不会直接影响计算
    }

    void EtherGPTP::sendFollowUp()
    {
        // Create EtherFrame to be used to encapsulate gPtpPacket because lower layer MAC only supports EtherFrame
        inet::EthernetIIFrame* frame=new inet::EthernetIIFrame("sync-follow");
        frame->setDest(inet::MACAddress::BROADCAST_ADDRESS);
        frame->setEtherType(inet::ETHERTYPE_IPv4);  // So far INET doesn't support gPTP (etherType = 0x88f7)
        frame->setKind(FOLLOW_UP);
        frame->setByteLength(18);
        gPtp_FollowUp* gptp = new gPtp_FollowUp("Follow_Up", inet::IEEE802CTRL_DATA);
        gptp->setName("Follow_Up");

        /* Follow_Up message length 76 byte */
        gptp->setByteLength(FOLLOW_UP_PACKET_SIZE);
        SimTime residenceTime;

        if (nodeType == MASTER_NODE)
        {
            gptp->setCorrectionField(FollowUpInterval);
            gptp->setRatio(tableGptp->getRateRatio());
            gptp->setOriginTimestamp(tableGptp->getOriginTimestamp());
        }
        else if (nodeType == BRIDGE_NODE)
        {
            double bits_follow=(MAC_HEADER + FOLLOW_UP_PACKET_SIZE + CRC_CHECKSUM)*8;
            SimTime packetTransmissionTime_follow = (SimTime)bits_follow/1000000000;
            residenceTime=tableGptp->getRateRatio().dbl()*(clockGptp->getHWtime()-sentTimeSync_self);
            gptp->setCorrectionField(tableGptp->getCorrectionField()+packetTransmissionTime_follow+ residenceTime+tableGptp->getPeerDelay());
            gptp->setRatio(tableGptp->getRateRatio());
            gptp->setOriginTimestamp(tableGptp->getOriginTimestamp());
        }
        frame->encapsulate(gptp);

        send(frame, "lowerLayerOut");
    }

    void EtherGPTP::processPdelayReq(gPtp_PdelayReq* gptp)
    {
        receivedTimeResponder = clockGptp->getHWtime(); // t2

        if (NULL == selfMsgDelayResp)
            selfMsgDelayResp = new cMessage("selfMsgPdelayResp");

        clockGptp->scheduleAtHWtime(clockGptp->getHWtime()+(SimTime)PDelayRespInterval,selfMsgDelayResp,this);
    }

    void EtherGPTP::sendPdelayResp()
    {
        // Create EtherFrame to be used to encapsulate gPtpPacket because lower layer MAC only supports EtherFrame
        inet::EthernetIIFrame* frame=new inet::EthernetIIFrame("resp");
        frame->setDest(inet::MACAddress::BROADCAST_ADDRESS);
        frame->setEtherType(inet::ETHERTYPE_IPv4);  // So far INET doesn't support gPTP (etherType = 0x88f7)
        frame->setKind(PDELAY_RESP);
        frame->setByteLength(18);

        gPtp_PdelayResp* gptp = new gPtp_PdelayResp("Pdelay_Resp", inet::IEEE802CTRL_DATA);
        gptp->setName("Pdelay_Resp");

        /* Pdelay_Resp message length 54 byte */
        gptp->setByteLength(PDELAY_RESP_PACKET_SIZE);
        gptp->setSentTime(clockGptp->getHWtime());//t3的真实数据，本来应该放在follow里发送的，当然确实也发送了，但是实际计算的时候还是从这个帧里取的数
        gptp->setRequestReceiptTimestamp(receivedTimeResponder);//t2
        frame->encapsulate(gptp);

        send(frame, "lowerLayerOut");
        if(selfMsgRespFollow == NULL)
            selfMsgRespFollow = new cMessage("selfMsgRespFollow");
        clockGptp->scheduleAtHWtime(clockGptp->getHWtime()+(SimTime)RespFollowInterval,selfMsgRespFollow,this);
    }

    void EtherGPTP::sendPdelayRespFollowUp()
    {
        inet::EthernetIIFrame* frame=new inet::EthernetIIFrame("resp-follow");
        frame->setDest(inet::MACAddress::BROADCAST_ADDRESS);
        frame->setEtherType(inet::ETHERTYPE_IPv4);  // So far INET doesn't support gPTP (etherType = 0x88f7)
        frame->setKind(PDELAY_RESP_FOLLOW_UP);
        frame->setByteLength(18);

        gPtp_PdelayRespFollowUp* gptp = new gPtp_PdelayRespFollowUp("Pdelay_Resp_Follow_Up", inet::IEEE802CTRL_DATA);
        gptp->setName("Pdelay_Resp_Follow_Up");

        /* Pdelay_Resp_Follow_Up message length 54 byte */
        gptp->setByteLength(PDELAY_RESP_FOLLOW_UP_PACKET_SIZE);
        frame->encapsulate(gptp);

        send(frame, "lowerLayerOut");
    }

    /********************************/
    /* Slave port related functions */
    /********************************/

    void EtherGPTP::slavePort(cMessage *msg)
    {
        if (msg->isSelfMessage())
        {
    //        if(selfMsgDelayReq == msg)
    //        {
    //            sendPdelayReq();

    //            schedulePdelay = clockGptp->getHWtime()+ pdelayInterval;
    //            clockGptp->scheduleAtHWtime(schedulePdelay,msg,this);
    //            schedulePdelay = simTime() + clockGptp->HwtoSw_interval(pdelayInterval);
    //            scheduleAt(schedulePdelay, msg);
    //        }
        }
        else if(msg->arrivedOn("lowerLayerIn"))
        {
            if (inet::EthernetIIFrame* etherFrame = dynamic_cast<inet::EthernetIIFrame*>(msg))
            {
                cPacket* ppp;
                switch(msg->getKind())
                {
                case SYNC:
                    ppp=etherFrame->decapsulate();
                    if(gPtp_Sync* gptp = dynamic_cast<gPtp_Sync*>(ppp))
                    {
                        processSync(gptp);
                    }
                    delete ppp;
                    delete etherFrame;
                    break;
                case FOLLOW_UP:
                    ppp=etherFrame->decapsulate();
                    if(gPtp_FollowUp* gptp = dynamic_cast<gPtp_FollowUp*>(ppp))
                    {
                        processFollowUp(gptp);

                        // Send a request to send Sync message
                        // through other gPtp Ethernet interfaces
                        send(requestMsg->dup(), "gptpLayerOut");
                    }
                    delete ppp;
                    delete etherFrame;
                    break;
                case PDELAY_RESP:
                    ppp=etherFrame->decapsulate();
                    if(gPtp_PdelayResp* gptp = dynamic_cast<gPtp_PdelayResp*>(ppp))
                    {
                        processPdelayResp(gptp);
                    }
                    delete ppp;
                    delete etherFrame;
                    break;
                case PDELAY_RESP_FOLLOW_UP:
                    ppp=etherFrame->decapsulate();
                    if(gPtp_PdelayRespFollowUp* gptp = dynamic_cast<gPtp_PdelayRespFollowUp*>(ppp))
                    {
                        processPdelayRespFollowUp(gptp);
                    }
                    delete ppp;
                    delete etherFrame;
                    break;
                default:
                    send(msg, "upperLayerOut");// Forward message to upperLayerOut gate since packet is not gPtp but eth
                    break;
                }
            }
            else
            {
                //send(msg, "upperLayerOut");// 并不是以太网帧，但如果是不是以太网帧，mac那边是不会传上来的，起码3.2.4这个版本不会
            }
        }
        else if(msg->arrivedOn("gptpLayerIn"))//table群发的请求，
        {
            sendPdelayReq();//slave端口处理玩followup后，立即发送PdelayReq，不再按照周期发送
            delete msg;
        }
        else if (msg->arrivedOn("upperLayerIn"))
        {
            // Forward message to lowerLayerOut gate because the message from upper layer doesn't need to be touched
            // and we are interested in only message from lowerLayerIn
            send(msg, "lowerLayerOut");
        }//如果是gptpLayerIn来的，没有处理，要不要删除？
    }

    void EtherGPTP::sendPdelayReq()
    {
        // Create EtherFrame to be used to encapsulate gPtpPacket because lower layer MAC only supports EtherFrame
        inet::EthernetIIFrame* frame=new inet::EthernetIIFrame("req");
        frame->setDest(inet::MACAddress::BROADCAST_ADDRESS);
        frame->setEtherType(inet::ETHERTYPE_IPv4);  // So far INET doesn't support gPTP (etherType = 0x88f7)
        frame->setKind(PDELAY_REQ);
        frame->setByteLength(18);

        gPtp_PdelayReq* gptp = new gPtp_PdelayReq("Pdelay_Req", inet::IEEE802CTRL_DATA);
        gptp->setName("Pdelay_Req");

        /* Pdelay_Req message length 54 byte */
        gptp->setByteLength(PDELAY_REQ_PACKET_SIZE);
        frame->encapsulate(gptp);

        send(frame, "lowerLayerOut");
        transmittedTimeRequester = clockGptp->getHWtime();//t1
    }

    void EtherGPTP::processSync(gPtp_Sync* gptp)
    {
        lastTimeSync=currentTimeSync;
        currentTimeSync = gptp->getSentTime();
        lastreceivedTimeSync=receivedTimeSync;
        receivedTimeSync = clockGptp->getHWtime();

        rateRatio = lastCascadeRatio.dbl()*(currentTimeSync - lastTimeSync)/(receivedTimeSync-lastreceivedTimeSync);

        tableGptp->setRateRatio(rateRatio);//实时更新，但是其实用不到
    }

    void EtherGPTP::processFollowUp(gPtp_FollowUp* gptp)
    {
        SimTime TimeSync_orign = gptp->getOriginTimestamp();
        tableGptp->setCorrectionField(gptp->getCorrectionField());

        lastCascadeRatio=gptp->getRatio();
        rateRatio = lastCascadeRatio.dbl()*(currentTimeSync - lastTimeSync)/(receivedTimeSync-lastreceivedTimeSync);
        tableGptp->setRateRatio(rateRatio);

        tableGptp->setOriginTimestamp(TimeSync_orign);//原始

        SimTime receivedTimeSyncBeforeSync = clockGptp->getHWtime();

        double bits_follow=(MAC_HEADER + FOLLOW_UP_PACKET_SIZE + CRC_CHECKSUM)*8;
        SimTime packetTransmissionTime_follow = (SimTime)bits_follow/1000000000;
        tableGptp->setOffset(TimeSync_orign + tableGptp->getCorrectionField()+packetTransmissionTime_follow+tableGptp->getPeerDelay() - (clockGptp->getHWtime()-tableGptp->getOffset()));
        //计算偏移的时候，是和分段线性函数上的硬件时间值相减，getHWtime返回带offset的值，需要处理
        clockGptp->setOffset_self(tableGptp->getOffset());
        vTimeDifferenceGMafterSync.record(clockGptp->getHWtime()-simTime());
        vTimeDifferenceGMbeforeSync.record(receivedTimeSyncBeforeSync - simTime());
    }

    void EtherGPTP::processPdelayResp(gPtp_PdelayResp* gptp)
    {
        receivedTimeRequester = clockGptp->getHWtime(); //t4
        receivedTimeResponder = gptp->getRequestReceiptTimestamp();
        transmittedTimeResponder = gptp->getSentTime();
    }

    void EtherGPTP::processPdelayRespFollowUp(gPtp_PdelayRespFollowUp* gptp)
    {
        double bits = (MAC_HEADER + PDELAY_RESP_PACKET_SIZE + CRC_CHECKSUM)*8;
        SimTime packetTransmissionTime = (SimTime)bits/1000000000;
        SimTime peerDelay= (tableGptp->getRateRatio().dbl()*(receivedTimeRequester - transmittedTimeRequester).dbl() - transmittedTimeResponder.dbl() + receivedTimeResponder.dbl())/2 - packetTransmissionTime;

        tableGptp->setPeerDelay(peerDelay);
    }

    void EtherGPTP::scheduleAtInObject(const SimTime realtime, cMessage* msg)//在hardwareclock module 使用，取回schedule和msg控制权
    {
        Enter_Method_Silent();
        take(msg);

        scheduleAt(realtime, msg);
    }
}
