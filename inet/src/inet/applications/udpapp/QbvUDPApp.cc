#include <inet/applications/udpapp/QbvUDPApp.h>
#include "inet/common/lifecycle/NodeOperations.h"
#include "inet/transportlayer/contract/udp/UDPControlInfo_m.h"
#include <iostream>
#include <inet/helper/parHelper.h>

using namespace std;

namespace inet {

    Define_Module(QbvUDPApp);

    void QbvUDPApp::initialize(int stage)
    {
        UDPBasicApp::initialize(stage);

        if (stage == 0)
        {
            sendInterval = 1.0/par("TrafficPacketsPerSec").doubleValue();
            startTime = sendInterval;
            TsnVLANId = par("tsn_destVlan").longValue();
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
        if (TsnVLANId > 0)
        {
            addParRecursive(payload, "packet_vlan") = TsnVLANId;
        }

        emit(sentPkSignal, payload);
        socket.sendTo(payload, destAddr, destPort);
        numSent++;
    }

    void QbvUDPApp::processSend()
    {
        sendPacket();

        simtime_t d = simTime() + sendInterval;

        if ((stopTime < SIMTIME_ZERO || d < stopTime) && sendInterval > 0)
        {
            selfMsg->setKind(SEND);
            scheduleAt(d, selfMsg);
        }
        else
        if (stopTime != -1)
        {
            selfMsg->setKind(STOP);
            scheduleAt(stopTime, selfMsg);
        }
    }

    void QbvUDPApp::scheduleAtInObject(const SimTime realtime, cMessage* msg)//在hardwareclock module 使用，取回schedule和msg控制权
    {
        Enter_Method_Silent();
        take(msg);

        scheduleAt(realtime, msg);
    }
}
