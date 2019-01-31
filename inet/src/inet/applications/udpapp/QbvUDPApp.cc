// author: Hathaway Le
#include <inet/applications/udpapp/QbvUDPApp.h>
#include <iostream>
#include <inet/helper/parHelper.h>

#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/lifecycle/NodeOperations.h"
#include "inet/transportlayer/contract/udp/UDPControlInfo_m.h"

using namespace std;

namespace inet {

    Define_Module(QbvUDPApp);

    simsignal_t QbvUDPApp::sentPkSignal = registerSignal("sentPk");
    simsignal_t QbvUDPApp::rcvdPkSignal = registerSignal("rcvdPk");

    QbvUDPApp::~QbvUDPApp()
    {
        cancelAndDelete(selfMsg);
    }

    void QbvUDPApp::initialize(int stage)
    {
        ApplicationBase::initialize(stage);

        if (stage == 0)
        {
            numSent = 0;
            numReceived = 0;
            WATCH(numSent);
            WATCH(numReceived);

            localPort = par("localPort");
            destPort = par("destPort");
            startTime = par("startTime").doubleValue();
            stopTime = par("stopTime").doubleValue();
            packetName = par("packetName");
            if (stopTime >= SIMTIME_ZERO && stopTime < startTime)
                throw cRuntimeError("Invalid startTime/stopTime parameters");
            selfMsg = new cMessage("sendTimer");

            sendInterval = 1.0/par("TrafficPacketsPerSec").doubleValue();
            TsnVLANId = par("tsn_destVlan").longValue();
            Tsnpriority =par("tsn_destpriority").longValue();
        }
        else if (stage == 1)
        {
            cModule* gPtpNode=getParentModule();
            if(gPtpNode->getSubmodule("hwClock")!=nullptr)
             {
                clockGptp = check_and_cast<HardwareClock *>(gPtpNode->getSubmodule("hwClock"));
             }
        }
    }

    void QbvUDPApp::sendPacket()
    {
        std::ostringstream str;
        str << packetName << "-" << numSent;
        cPacket *payload = new cPacket(str.str().c_str());
        payload->setByteLength(par("messageLength").longValue());

        L3Address destAddr = chooseDestAddr();
         //比原来版本多了添加par
        if (TsnVLANId > -1)
        {
            addParRecursive(payload, "packet_vlan") = TsnVLANId;
        }
        if(Tsnpriority > -1)
        {
            addParRecursive(payload, "packet_priority") = Tsnpriority;
        }

        emit(sentPkSignal, payload);
        socket.sendTo(payload, destAddr, destPort);
        numSent++;
    }

    void QbvUDPApp::processStart()
    {
        socket.setOutputGate(gate("udpOut"));
        const char *localAddress = par("localAddress");
        socket.bind(*localAddress ? L3AddressResolver().resolve(localAddress) : L3Address(), localPort);
        setSocketOptions();

        const char *destAddrs = par("destAddresses");
        cStringTokenizer tokenizer(destAddrs);
        const char *token;

        while ((token = tokenizer.nextToken()) != nullptr) {
            L3Address result;
            L3AddressResolver().tryResolve(token, result);
            if (result.isUnspecified())
                EV_ERROR << "cannot resolve destination address: " << token << endl;
            else
                destAddresses.push_back(result);
        }

        if (!destAddresses.empty()) {
            selfMsg->setKind(SEND);
            processSend();
        }
        else {
            if (stopTime >= SIMTIME_ZERO) {
                selfMsg->setKind(STOP);
                if(clockGptp)
                {
                    clockGptp->app_scheduleAtHWtime(stopTime,selfMsg,this);
                }
                else
                {
                    scheduleAt(stopTime, selfMsg);
                }
            }
        }
    }

    void QbvUDPApp::processSend()
    {
        sendPacket();

        simtime_t d = simTime() + sendInterval;

        if ((stopTime < SIMTIME_ZERO || d < stopTime) && sendInterval > 0)
        {
            selfMsg->setKind(SEND);
            if(clockGptp)
            {
                clockGptp->app_scheduleAtHWtime(clockGptp->getHWtime()+sendInterval,selfMsg,this);
            }
            else
            {
                scheduleAt(d, selfMsg);
            }
        }
        else if (stopTime != -1)
        {
            selfMsg->setKind(STOP);
            if(clockGptp)
            {
                clockGptp->app_scheduleAtHWtime(stopTime,selfMsg,this);
            }
            else
            {
                scheduleAt(stopTime, selfMsg);
            }
        }
    }

    bool QbvUDPApp::handleNodeStart(IDoneCallback *doneCallback)
    {
        simtime_t start = std::max(startTime, simTime());
        if ((stopTime < SIMTIME_ZERO) || (start < stopTime) || (start == stopTime && startTime == stopTime)) {
            selfMsg->setKind(START);
            if(clockGptp)
            {
                clockGptp->app_scheduleAtHWtime(startTime,selfMsg,this);
            }
            else
            {
                scheduleAt(startTime, selfMsg);
            }
        }
        return true;
    }

    void QbvUDPApp::scheduleAtInObject(const SimTime realtime, cMessage* msg)//在hardwareclock module 使用，取回schedule和msg控制权
    {
        Enter_Method_Silent();
        take(msg);

        scheduleAt(realtime, msg);
    }


    void QbvUDPApp::finish()
    {
        recordScalar("packets sent", numSent);
        recordScalar("packets received", numReceived);
        delete clockGptp;
        //不能放在析构里，我哪知道为啥,因为不是共享指针，只要在这里删除了，EtherGPTP那边就不用了，当然它本来也没删除，居然没出问题，rebuild后出错的问题也没有了
        //原版程序EtherGPTP也没有delete，但是一且正常，照理说clock模块会自己析构，没有看过omnet底层代码，无解
        ApplicationBase::finish();
    }

    void QbvUDPApp::setSocketOptions()
    {
        int timeToLive = par("timeToLive");
        if (timeToLive != -1)
            socket.setTimeToLive(timeToLive);

        int typeOfService = par("typeOfService");
        if (typeOfService != -1)
            socket.setTypeOfService(typeOfService);

        const char *multicastInterface = par("multicastInterface");
        if (multicastInterface[0]) {
            IInterfaceTable *ift = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);
            InterfaceEntry *ie = ift->getInterfaceByName(multicastInterface);
            if (!ie)
                throw cRuntimeError("Wrong multicastInterface setting: no interface named \"%s\"", multicastInterface);
            socket.setMulticastOutputInterface(ie->getInterfaceId());
        }

        bool receiveBroadcast = par("receiveBroadcast");
        if (receiveBroadcast)
            socket.setBroadcast(true);

        bool joinLocalMulticastGroups = par("joinLocalMulticastGroups");
        if (joinLocalMulticastGroups) {
            MulticastGroupList mgl = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this)->collectMulticastGroups();
            socket.joinLocalMulticastGroups(mgl);
        }
    }

    L3Address QbvUDPApp::chooseDestAddr()
    {
        int k = intrand(destAddresses.size());
        if (destAddresses[k].isLinkLocal()) {    // KLUDGE for IPv6
            const char *destAddrs = par("destAddresses");
            cStringTokenizer tokenizer(destAddrs);
            const char *token = nullptr;

            for (int i = 0; i <= k; ++i)
                token = tokenizer.nextToken();
            destAddresses[k] = L3AddressResolver().resolve(token);
        }
        return destAddresses[k];
    }

    void QbvUDPApp::processStop()
    {
        socket.close();
    }

    void QbvUDPApp::handleMessageWhenUp(cMessage *msg)
    {
        if (msg->isSelfMessage()) {
            ASSERT(msg == selfMsg);
            switch (selfMsg->getKind()) {
                case START:
                    processStart();
                    break;

                case SEND:
                    processSend();
                    break;

                case STOP:
                    processStop();
                    break;

                default:
                    throw cRuntimeError("Invalid kind %d in self message", (int)selfMsg->getKind());
            }
        }
        else if (msg->getKind() == UDP_I_DATA) {
            // process incoming packet
            processPacket(PK(msg));
        }
        else if (msg->getKind() == UDP_I_ERROR) {
            EV_WARN << "Ignoring UDP error report\n";
            delete msg;
        }
        else {
            throw cRuntimeError("Unrecognized message (%s)%s", msg->getClassName(), msg->getName());
        }

        if (hasGUI()) {
            char buf[40];
            sprintf(buf, "rcvd: %d pks\nsent: %d pks", numReceived, numSent);
            getDisplayString().setTagArg("t", 0, buf);
        }
    }

    void QbvUDPApp::processPacket(cPacket *pk)
    {
        emit(rcvdPkSignal, pk);
        EV_INFO << "Received packet: " << UDPSocket::getReceivedPacketInfo(pk) << endl;
        delete pk;
        numReceived++;
    }

    bool QbvUDPApp::handleNodeShutdown(IDoneCallback *doneCallback)
    {
        if (selfMsg)
            cancelEvent(selfMsg);
        //TODO if(socket.isOpened()) socket.close();
        return true;
    }

    void QbvUDPApp::handleNodeCrash()
    {
        if (selfMsg)
            cancelEvent(selfMsg);
    }

}
