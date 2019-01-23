//
// @authors: Enkhtuvshin Janchivnyambuu
//           Henning Puttnies
//           Peter Danielis
//           University of Rostock, Germany
// Modifier: Hathaway Le 

cplusplus {{
//#include "simtime.h"
#include "inet/linklayer/ethernet/gPtp.h"
#include "omnetpp.h"
}};
namespace inet;

class noncobject omnetpp::SimTime;

packet gPtp_Sync
{
	omnetpp::SimTime sentTime;
}

packet gPtp_FollowUp
{
   	omnetpp::SimTime originTimestamp;	
    omnetpp::SimTime ratio;//ratio也用SimTime是因为double的精度不够高
	omnetpp::SimTime correctionField;
}

packet gPtp_PdelayReq
{
}

packet gPtp_PdelayResp
{
    omnetpp::SimTime sentTime;
    omnetpp::SimTime requestReceiptTimestamp;
}

packet gPtp_PdelayRespFollowUp
{
}
