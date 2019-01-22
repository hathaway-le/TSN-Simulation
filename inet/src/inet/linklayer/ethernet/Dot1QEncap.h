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
//	TSN Non-Timebased Simulation Framework
//		2016, Peter Heise <peter.heise@airbus.com>
//

#ifndef TSN_DOT1QENCAP_H_
#define TSN_DOT1QENCAP_H_

#include <inet/linklayer/ethernet/Dot1QFrame.h>
#include <omnetpp.h>
#include <inet/linklayer/ethernet/EtherEncap.h>
#include <inet/linklayer/common/Ieee802Ctrl.h>
//#include "src/helper/parHelper.h"

using namespace omnetpp;

namespace inet {
		class Dot1QEncap : public EtherEncap
		{
			private:
				uint16_t Dot1QVlanID;

			protected:
				virtual void initialize();
//				virtual void processPacketFromHigherLayer(cPacket *msg);

		};
}
#endif
