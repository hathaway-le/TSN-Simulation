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

#include <inet/linklayer/ethernet/Dot1QEncap.h>

namespace inet {

		Define_Module(Dot1QEncap);

		void Dot1QEncap::initialize() {
			EtherEncap::initialize();
			Dot1QVlanID = (uint32_t) par("dot1Q_NullStreamVlan").longValue();
		}
/*
		void Dot1QEncap::processPacketFromHigherLayer(cPacket *msg) {

			if (msg->getByteLength() > MAX_ETHERNET_DATA_BYTES)
				throw cRuntimeError("packet from higher layer (%d bytes) exceeds maximum Ethernet payload length (%d)", (int) msg->getByteLength(),
				MAX_ETHERNET_DATA_BYTES);

			totalFromHigherLayer++;
			emit(encapPkSignal, msg);

			// Creates MAC header information and encapsulates received higher layer data
			// with this information and transmits resultant frame to lower layer

			// create Ethernet frame, fill it in from Ieee802Ctrl and encapsulate msg in it
			EV_DETAIL << "Encapsulating higher layer packet `" << msg->getName() << "' for MAC\n";

			Ieee802Ctrl *etherctrl = check_and_cast<Ieee802Ctrl*>(msg->removeControlInfo());
			Dot1QFrame *dot1qFrame = new Dot1QFrame(msg->getName(), etherctrl);

			// Set 802.1Q
			dot1qFrame->setTPID(DEFAULT_TPID);
			dot1qFrame->setPCP(DEFAULT_PCP);
			dot1qFrame->setDEI(DEFAULT_DEI);

			if (cPacket *match = hasParRecursive(msg, "packet_vlan"))//在UDPPacketGen会添加ini文件的中的配置，vlan值在那时候就已经确定
				dot1qFrame->setVID(match->par("packet_vlan").longValue());
			else
				dot1qFrame->setVID(Dot1QVlanID);

			delete etherctrl;

			dot1qFrame->encapsulate(msg);
			if (dot1qFrame->getByteLength() < MIN_ETHERNET_FRAME_BYTES) {
				dot1qFrame->setByteLength(MIN_ETHERNET_FRAME_BYTES);  // "padding"
			}

			EV_INFO << "Sending " << dot1qFrame << " to lower layer.\n";
			send(dot1qFrame, "lowerLayerOut");

		}
*/

}
