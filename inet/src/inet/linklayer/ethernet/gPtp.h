//
// @authors: Enkhtuvshin Janchivnyambuu
//           Henning Puttnies
//           Peter Danielis
//           University of Rostock, Germany
// Modifier: Hathaway Le

#ifndef INET_LINKLAYER_ETHERNET_GPTP_H_
#define INET_LINKLAYER_ETHERNET_GPTP_H_

namespace inet {
    /* Packet size is in byte */
    enum gPtpPacketSize: int {
        SYNC_PACKET_SIZE = 44,
        FOLLOW_UP_PACKET_SIZE = 76,
        PDELAY_REQ_PACKET_SIZE = 54,
        PDELAY_RESP_PACKET_SIZE = 54,
        PDELAY_RESP_FOLLOW_UP_PACKET_SIZE = 54
    };

    /* Below is used to calculate packet transmission time */
    enum gPtpHeader: int {
        MAC_HEADER = 22,
        CRC_CHECKSUM = 4
    };

}
#endif /* INET_LINKLAYER_ETHERNET_GPTP_H_ */
