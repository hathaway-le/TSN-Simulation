//
// @authors: Enkhtuvshin Janchivnyambuu
//           Henning Puttnies
//           Peter Danielis
//           University of Rostock, Germany
// Modifier: Hathaway Le

#ifndef __IEEE8021AS_TABLEGPTP_H_
#define __IEEE8021AS_TABLEGPTP_H_

#include <omnetpp.h>
#include <vector>
#include <string>
#include <inet/hardwareclock/hardwareclock.h>

using namespace omnetpp;
namespace inet {
    enum gate_window : int
    {
        w_gptp = 0,
        w_pro = 1,
        w_unpro = 2,
        w_guard = 3
    };
    struct gate_state
    {
        std::vector<int> gate_state_this;
    };
    class TableGPTP : public cSimpleModule
    {
        SimTime correctionField;
        SimTime rateRatio;
        SimTime originTimestamp;
        SimTime peerDelay;
        SimTime offset;
        SimTime diff;

        std::vector<gate_state> all_gate_state;
        int cur_state;//用来当cur_gate_state的index

        int numberOfGates;
        bool qbvstart = false;

        cMessage* selfQbv;
        HardwareClock* clockGptp;
      protected:
        virtual void initialize(int stage);
        virtual void handleMessage(cMessage *msg);
        virtual int numInitStages() const override { return 2; }

      public:
        TableGPTP();
        ~TableGPTP();
        void setCorrectionField(SimTime cf);
        void setRateRatio(SimTime cf);
        void setPeerDelay(SimTime cf);
        void setOriginTimestamp(SimTime cf);
        void setOffset(SimTime cf);
        void setdiff(SimTime cf);

        SimTime getCorrectionField();
        SimTime getRateRatio();
        SimTime getPeerDelay();
        SimTime getOriginTimestamp();
        SimTime getOffset();
        SimTime getdiff();
        std::vector<int> getgatestate();

        void scheduleAtInObject(const SimTime realtime, cMessage* msg);
    };
}
#endif
