//Modifier: Hathaway Le
#ifndef __Qbv_CLASSIFIER_H_
#define __Qbv_CLASSIFIER_H_

#include <omnetpp.h>
#include "inet/networklayer/contract/ipv4/IPv4ControlInfo.h"
#include "inet/common/INETDefs.h"
#include "inet/networklayer/ipv4/IPv4Datagram.h"
#include "inet/transportlayer/udp/UDPPacket_m.h"
#include <inet/linklayer/ethernet/Dot1QFrame.h>

using namespace omnetpp;
namespace inet {
		class QbvClassifier : public cSimpleModule
		{
			private:
				cGate* prio[8];
				int prioClasses;
                int defaultQBVQueue;
                int defaultNonQBVQueue;
			protected:
				virtual void initialize();
				virtual void handleMessage(cMessage *msg);
		};
}
#endif

