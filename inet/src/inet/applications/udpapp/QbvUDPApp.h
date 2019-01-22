#ifndef __QbvUDPApp_H
#define __QbvUDPApp_H

#include <vector>

#include "inet/common/INETDefs.h"

#include "inet/applications/udpapp/UDPBasicApp.h"
#include "inet/transportlayer/contract/udp/UDPSocket.h"
#include "inet/common/ModuleAccess.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/networklayer/contract/IInterfaceTable.h"

using namespace omnetpp;
/**
 * UDP application. See NED for more info.
 */
namespace inet {
    class INET_API QbvUDPApp : public UDPBasicApp
    {
      protected:
        // chooses random destination address
        virtual void sendPacket();
        virtual void initialize(int stage) override;
        virtual void processSend();

        double sendInterval;

      public:
        void scheduleAtInObject(const SimTime realtime, cMessage* msg);

      private:
        long int TsnVLANId;
    };
}
#endif

