//Modifier: Hathaway Le
#include <inet/linklayer/ethernet/QbvClassifier.h>

namespace inet {
		Define_Module(QbvClassifier);

		void QbvClassifier::initialize()
		{
		    prioClasses = gateSize("out");
		    defaultQBVQueue = prioClasses -1;
		    defaultNonQBVQueue = prioClasses -1;

            for (int var = 0; var < prioClasses; var++)
            {
                prio[var] = gate("out", var);//绑定每个out门
            }
		}

		void QbvClassifier::handleMessage(cMessage *msg)
		{
            if (msg->isSelfMessage())
            {
                delete msg;
                return;
            }
            else
            {
                Dot1QFrame* dot1qFrame = dynamic_cast<Dot1QFrame*>(msg);
                if(dot1qFrame==NULL)
                {
                    send(msg,prio[defaultNonQBVQueue]);
                }
                else
                {
/*                  if(hasParRecursive(dot1qFrame, "packet_priority"))
                    {
                        send(msg,prio[parRecursive(dot1qFrame,"packet_priority").longValue()]);
                    }
                    else
                    {
                        send(msg,prio[defaultQBVQueue]);
                    }
*/                  if(dot1qFrame->getPCP()<prioClasses)
                    {
                        send(msg,prio[dot1qFrame->getPCP()]);
                    }
                    else
                    {
                        send(msg,prio[defaultQBVQueue]);
                    }
                }
            }
		}

}
