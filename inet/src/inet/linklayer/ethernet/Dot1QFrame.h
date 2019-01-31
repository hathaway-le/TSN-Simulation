#ifndef Dot1QFrame_H_
#define Dot1QFrame_H_

#include "inet/linklayer/ethernet/Dot1QFrame_m.h"

#include <inet/linklayer/common/Ieee802Ctrl.h>
#include "inet/helper/parHelper.h"

#define DEFAULT_TPID 0x8100
#define DEFAULT_PCP 0
#define DEFAULT_DEI 0
#define DEFAULT_VID 0

namespace inet {
		class Dot1QFrame : public Dot1QFrame_Base
		{

			public:
				/**
				 * @brief Copy Constructor,除abstract的成员，其他的都会被构造
				 */
				Dot1QFrame(const Dot1QFrame& other) :
						Dot1QFrame_Base(other) {
				}

				/**
				 * @brief Constructor
				 *
				 * @param name message name
				 * @param kind message type
				 */
				Dot1QFrame(const char *name = nullptr, short kind = 0) :
						Dot1QFrame_Base(name, kind) {
				}

				/**
				 * @brief Constructor
				 *
				 * @param name message name
				 * @param kind message type
				 */
				Dot1QFrame(const char *name, Ieee802Ctrl *etherctrl) :
						Dot1QFrame(name) {

					// Copy old control info
					setDest(etherctrl->getDest());
					setSrc(etherctrl->getSrc());
					setEtherType(etherctrl->getEtherType());
				}

				/**
				 * @brief Assignment operator
				 */
				Dot1QFrame& operator=(const Dot1QFrame& other) {
					Dot1QFrame_Base::operator=(other);
					return *this;
				}

				virtual Dot1QFrame* dup() const {
					Dot1QFrame* dup = new Dot1QFrame(*this);
/*
					dup->setTCI_Size(this->getTCI_Size());//下面五行是否是多余的
					dup->setTPID(this->getTPID());
					dup->setPCP(this->getPCP());
					dup->setDEI(this->getDEI());
					dup->setVID(this->getVID());
*/
					return dup;
				}

				uint8_t getPCP() const {
					return (getTCI_Size() & 0xe000) >> 13;
				}

				void setPCP(uint8_t PCP) {
					setTCI_Size((getTCI_Size() & 0x0fff) | ((PCP & 0x07) << 13));
				}

				uint8_t getDEI() const {
					return (getTCI_Size() & 0x1000) >> 12;
				}

				void setDEI(uint8_t DEI) {
					setTCI_Size((getTCI_Size() & 0x0fff) | (DEI & 0x0001) << 12);
				}

				uint16_t getVID() const {
					return (getTCI_Size() & 0x0fff);
				}

				void setVID(uint16_t VID) {
					setTCI_Size((getTCI_Size() & 0xf000) | (VID & 0x0fff));
				}
		};
		Register_Class(Dot1QFrame);
}
#endif /* Dot1QFrame_H_ */
