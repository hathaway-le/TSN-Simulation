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
//  TSimNet Simulation Framework
//      2016, Peter Heise <peter.heise@airbus.com>
//

#include <inet/helper/parHelper.h>
namespace inet {
		cMsgPar& addParRecursive(cPacket *msg, const char *name) {

			cPacket *head = msg;

			while (head->getEncapsulatedPacket() != NULL) {
				head = head->getEncapsulatedPacket();
			}

			return head->addPar(name);//在最里层的packet中添加属性

		}

		cMsgPar& parRecursive(const cPacket *msg, const char *name) {

			cPacket *head = const_cast<cPacket *>(msg);

			while (head != NULL) {
				if (head->hasPar(name)) {
					return head->par(name);
				}
				head = head->getEncapsulatedPacket();
			}

			cPacket *tail = const_cast<cPacket *>(msg);
			return tail->par(name);

		}

		cPacket* hasParRecursive(cPacket *msg, const char *name) {

			cPacket *head = msg;

			while (head != NULL) {
				if (head->hasPar(name)) {
					return head;
				}
				head = head->getEncapsulatedPacket();
			}

			return NULL;

		}
}
