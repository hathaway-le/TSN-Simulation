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

#ifndef __INET_QBVSCHEDULER_H
#define __INET_QBVSCHEDULER_H

#include "inet/common/INETDefs.h"
#include "inet/common/queue/SchedulerBase.h"
#include <inet/linklayer/ethernet/tableGPTP.h>
#include <vector>

namespace inet {

/**
 * Schedule packets in strict priority order.
 *
 * Packets arrived at the 0th gate has the higher priority.
 */
class INET_API QbvScheduler : public SchedulerBase
{
  protected:
    virtual bool schedulePacket() override;
    virtual void initialize() override;
  private:
    std::vector<int> control_gate;
    TableGPTP* tableGptp;
};

} // namespace inet

#endif // ifndef __INET_PRIORITYSCHEDULER_H

