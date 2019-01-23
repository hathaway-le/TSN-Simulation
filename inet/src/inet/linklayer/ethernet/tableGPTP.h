//
// @authors: Enkhtuvshin Janchivnyambuu
//           Henning Puttnies
//           Peter Danielis
//           University of Rostock, Germany
// Modifier: Hathaway Le

#ifndef __IEEE8021AS_TABLEGPTP_H_
#define __IEEE8021AS_TABLEGPTP_H_

#include <omnetpp.h>

using namespace omnetpp;
namespace inet {
    class TableGPTP : public cSimpleModule
    {
        SimTime correctionField;
        SimTime rateRatio;
        SimTime originTimestamp;
        SimTime peerDelay;
        SimTime offset;

        int numberOfGates;

      protected:
        virtual void initialize(int stage);
        virtual void handleMessage(cMessage *msg);

      public:
        void setCorrectionField(SimTime cf);
        void setRateRatio(SimTime cf);
        void setPeerDelay(SimTime cf);
        void setOriginTimestamp(SimTime cf);
        void setOffset(SimTime cf);

        SimTime getCorrectionField();
        SimTime getRateRatio();
        SimTime getPeerDelay();
        SimTime getOriginTimestamp();
        SimTime getOffset();
    };
}
#endif
