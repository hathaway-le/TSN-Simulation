//
// Copyright (C) 2012 Opensim Ltd.
// Author: Tamas Borbely
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.

// authors: Hathaway Le

#include "inet/common/queue/QbvScheduler.h"

namespace inet {

Define_Module(QbvScheduler);

void QbvScheduler::initialize()
{
    SchedulerBase::initialize();
    cModule* gPtpNode=getParentModule()->getParentModule()->getParentModule();
    if(gPtpNode->getSubmodule("tableGPTP")!=nullptr)
    {
        tableGptp = check_and_cast<TableGPTP *>(gPtpNode->getSubmodule("tableGPTP"));
    }
}

bool QbvScheduler::schedulePacket()
{
    control_gate = tableGptp->getgatestate();
    for(unsigned int i = 0 ; i<control_gate.size(); i++)
    {
        EV<<control_gate[i];
    }
    EV<<endl;
    EV<<tableGptp->getdiff().dbl()<<endl;
    for(unsigned int i = 0 ; i<inputQueues.size(); i++)
    {
        if(control_gate[i] == 1)
        {
            if (!inputQueues[i]->isEmpty())//Returns true if the queue is empty.
            {
                inputQueues[i]->requestPacket();
                return true;
            }
        }
    }
    return false;
}

} // namespace inet

