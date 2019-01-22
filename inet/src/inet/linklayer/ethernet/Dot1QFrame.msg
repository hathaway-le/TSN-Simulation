cplusplus {{
    //INET
    #include "inet/linklayer/ethernet/EtherFrame_m.h"
}}

class inet::EthernetIIFrame;
namespace inet;

packet Dot1QFrame extends EthernetIIFrame {
    
    @customize(true);
    
    // 802.1Q
    uint16_t TPID; // 0x8100 for 802.1Q (VLAN) Tag
    uint16_t TCI_Size; // TCI size (includes PCP, DEI, VID) 
    abstract uint8_t PCP; // 3bits
    abstract uint8_t DEI; // 1bit
    abstract uint16_t VID; // 12 bits
    
}
