//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package inet.linklayer.ethernet;

import inet.common.queue.DropTailQueue;
import inet.common.queue.IOutputQueue;
import inet.common.queue.PriorityScheduler;


//
// Queue module that gives the PAUSE frames a higher priority,
// and can be parametrized with an ~IOutputQueue for serving the
// other frames.
//
module EtherQbvQueue like IOutputQueue
{
    parameters:
        @display("i=block/queue,#FFFF00");
        int numOfQueues = default(8);
        string dataQueueType = default("DropTailQueue");

    gates:
        input in;
        output out;

    submodules:
        classifier: QbvClassifier {
            parameters:
                @display("p=50,100");
        }
        scheduler: PriorityScheduler {
            @display("p=350,100");
        }
        queue[numOfQueues]: <dataQueueType> like IOutputQueue {
            parameters:
                @display("p=200,100,column");
        }


    connections:
        in --> classifier.in;
        scheduler.out --> out;

        for i=0..numOfQueues-1 {
            classifier.out++ --> queue[i].in;
            queue[i].out --> scheduler.in++;
        }
}