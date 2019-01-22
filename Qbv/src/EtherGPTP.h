//
// @authors: Enkhtuvshin Janchivnyambuu
//           Henning Puttnies
//           Peter Danielis
//           University of Rostock, Germany
// Modifier: Hathaway Le

#ifndef __IEEE8021AS_ETHERGPTP_H_
#define __IEEE8021AS_ETHERGPTP_H_

#include "gPtp.h"
#include "gPtpPacket_m.h"
#include "tableGPTP.h"
#include "hardwareclock.h"
#include <omnetpp.h>
#include <string>
#include "inet/linklayer/ethernet/EtherFrame_m.h"

#include "inet/linklayer/ethernet/EtherMACBase.h"
#include "inet/linklayer/ethernet/EtherFrame_m.h"
#include "inet/linklayer/common/MACAddress.h"
#include "inet/linklayer/base/MACBase.h"

using namespace omnetpp;
using namespace inet;
enum gPtpNodeType {
    MASTER_NODE = 11,
    BRIDGE_NODE = 12,
    SLAVE_NODE  = 13
};

enum gPtpPortType {
    MASTER_PORT  = 2,
    SLAVE_PORT   = 1,
    PASSIVE_PORT = 0
};

enum gPtpMessageType {
    SYNC    = 1010,
    FOLLOW_UP = 1011,
    PDELAY_REQ = 1014,
    PDELAY_RESP = 1012,
    PDELAY_RESP_FOLLOW_UP = 1013
};

class EtherGPTP : public cSimpleModule
{
    TableGPTP* tableGptp;
    HardwareClock* clockGptp;
    int portType;
    int nodeType;

    SimTime rateRatio;

    // errorTime is time difference between MAC transmition
    // or receiving time and etherGPTP time
    cMessage* requestMsg;

    SimTime syncInterval;

    cMessage* selfMsgSync;
    cMessage* selfMsgFollowUp;
    cMessage* selfMsgDelayResp;
    cMessage* selfMsgRespFollow;

    /* Slave port - Variables is used for Peer Delay Measurement */

    SimTime receivedTimeResponder;
    SimTime receivedTimeRequester;
    SimTime transmittedTimeResponder;
    SimTime transmittedTimeRequester;
    double PDelayRespInterval;
    double FollowUpInterval;
    double RespFollowInterval;
    SimTime sentTimeSync_self;

    /* Slave port - Variables is used for Rate Ratio. All times are drifted based on constant drift */

    SimTime receivedTimeSync;
    SimTime lastreceivedTimeSync;
    SimTime lastTimeSync;
    SimTime currentTimeSync;

    SimTime lastCascadeRatio;


    /* Statistics information */
    cOutVector vTimeDifferenceGMafterSync;
    cOutVector vTimeDifferenceGMbeforeSync;

  protected:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    int numInitStages() const {return 2;}

  public:
    EtherGPTP();

    void masterPort(cMessage *msg);
    void sendSync();
    void sendFollowUp();
    void processPdelayReq(gPtp_PdelayReq* gptp);
    void sendPdelayResp();
    void sendPdelayRespFollowUp();

    void slavePort(cMessage *msg);
    void sendPdelayReq();
    void processSync(gPtp_Sync* gptp);
    void processFollowUp(gPtp_FollowUp* gptp);
    void processPdelayResp(gPtp_PdelayResp* gptp);
    void processPdelayRespFollowUp(gPtp_PdelayRespFollowUp* gptp);

    void scheduleAtInObject(const SimTime realtime, cMessage* msg);
};

#endif
