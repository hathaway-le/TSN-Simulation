//
// @authors: Enkhtuvshin Janchivnyambuu
//           Henning Puttnies
//           Peter Danielis
//           University of Rostock, Germany
// Modifier: Hathaway Le

#include "tableGPTP.h"

Define_Module(TableGPTP);

void TableGPTP::initialize(int stage)
{
    correctionField = par("correctionField");
    rateRatio = par("rateRatio");
    peerDelay = 0;
    numberOfGates = gateSize("gptpLayerIn");
    originTimestamp=0;
    offset=0;
}

void TableGPTP::handleMessage(cMessage *msg)
{
    if(msg->arrivedOn("gptpLayerIn"))
    {
        for (int i = 0; i < numberOfGates; i++)
        {
            send(msg->dup(), "gptpLayerOut", i);
        }
        delete msg;
    }
}

void TableGPTP::setCorrectionField(SimTime cf)
{
    correctionField = cf;
}

SimTime TableGPTP::getCorrectionField()
{
    return correctionField;
}

void TableGPTP::setRateRatio(SimTime cf)
{
    rateRatio = cf;
}

SimTime TableGPTP::getRateRatio()
{
    return rateRatio;
}

void TableGPTP::setPeerDelay(SimTime cf)
{
    peerDelay = cf;
}

SimTime TableGPTP::getPeerDelay()
{
    return peerDelay;
}

void TableGPTP::setOriginTimestamp(SimTime cf)
{
    originTimestamp = cf;
}

SimTime TableGPTP::getOriginTimestamp()
{
    return originTimestamp;
}

void TableGPTP::setOffset(SimTime cf)
{
    offset = cf;
}

SimTime TableGPTP::getOffset()
{
    return offset;
}

