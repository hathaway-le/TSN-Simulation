//author: Hathaway Le
#ifndef __QbvUDPApp_H
#define __QbvUDPApp_H

#include <vector>

#include "inet/common/INETDefs.h"

#include "inet/applications/base/ApplicationBase.h"
#include "inet/transportlayer/contract/udp/UDPSocket.h"
#include "inet/hardwareclock/hardwareclock.h"

using namespace omnetpp;
/**
 * UDP application. See NED for more info.
 */
namespace inet {
    class INET_API QbvUDPApp : public ApplicationBase
    {
        protected:
          enum SelfMsgKinds { START = 1, SEND, STOP };

          // parameters
          std::vector<L3Address> destAddresses;
          int localPort = -1, destPort = -1;
          simtime_t startTime;
          simtime_t stopTime;
          const char *packetName = nullptr;

          // state
          UDPSocket socket;
          cMessage *selfMsg = nullptr;

          // statistics
          int numSent = 0;
          int numReceived = 0;

          static simsignal_t sentPkSignal;
          static simsignal_t rcvdPkSignal;

          virtual int numInitStages() const override { return NUM_INIT_STAGES; }
          virtual void initialize(int stage) override;
          virtual void handleMessageWhenUp(cMessage *msg) override;
          virtual void finish() override;

          // chooses random destination address
          virtual L3Address chooseDestAddr();
          virtual void sendPacket();
          virtual void processPacket(cPacket *msg);
          virtual void setSocketOptions();

          virtual void processStart();
          virtual void processSend();
          virtual void processStop();

          virtual bool handleNodeStart(IDoneCallback *doneCallback) override;
          virtual bool handleNodeShutdown(IDoneCallback *doneCallback) override;
          virtual void handleNodeCrash() override;

        public:
          void scheduleAtInObject(const SimTime realtime, cMessage* msg);
          ~QbvUDPApp();
        private:
          long int TsnVLANId;
          int Tsnpriority;
          HardwareClock* clockGptp = nullptr;
          double sendInterval;
    };
}
#endif

