//
// Copyright (C) 2006 Levente Meszaros
// Copyright (C) 2011 Zoltan Bojthe
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
//
// Modifier: Hathaway Le

#include "inet/linklayer/ethernet/EtherTSN.h"

#include "inet/common/queue/IPassiveQueue.h"
#include "inet/common/NotifierConsts.h"
#include "inet/common/RawPacket.h"
#include "inet/common/serializer/SerializerBase.h"
#include "inet/common/serializer/headerserializers/ethernet/EthernetSerializer.h"
#include "inet/common/ModuleAccess.h"
#include "inet/linklayer/ethernet/EtherFrame.h"
#include "inet/networklayer/common/InterfaceEntry.h"

namespace inet {

// TODO: refactor using a statemachine that is present in a single function
// TODO: this helps understanding what interactions are there and how they affect the state

Define_Module(EtherTSN);

EtherTSN::EtherTSN()
{
}

void EtherTSN::initialize(int stage)
{
    EtherMACBase::initialize(stage);// EtherMACBase::initialize 里面 initializeQueueModule

    if (stage == INITSTAGE_LOCAL) {
        if (!par("duplexMode").boolValue())
            throw cRuntimeError("Half duplex operation is not supported by EtherTSN, use the EtherMAC module for that! (Please enable csmacdSupport on EthernetInterface)");
    }
    else if (stage == INITSTAGE_LINK_LAYER) {
        beginSendFrames();    //FIXME choose an another stage for it
    }
}

void EtherTSN::initializeQueueModule()
{
    if (par("queueModule").stringValue()[0]) {
        cModule *module = getModuleFromPar<cModule>(par("queueModule"), this);
        IPassiveQueue *queueModule;
        if (module->isSimple())
            queueModule = check_and_cast<IPassiveQueue *>(module);
        else {
            cGate *queueOut = module->gate("out")->getPathStartGate();
            queueModule = check_and_cast<IPassiveQueue *>(queueOut->getOwnerModule());
        }

        EV_DETAIL << "Requesting first frame from queue module\n";
        txQueue.setExternalQueue(queueModule);

        if (txQueue.extQueue->getNumPendingRequests() == 0)
            txQueue.extQueue->requestPacket();
    }
    txQueue.innerQueue = new InnerQueue("InQueue", par("txQueueLimit").longValue());
}

void EtherTSN::initializeStatistics()
{
    EtherMACBase::initializeStatistics();

    // initialize statistics
    totalSuccessfulRxTime = 0.0;
}

void EtherTSN::initializeFlags()
{
    EtherMACBase::initializeFlags();

    duplexMode = true;
    physInGate->setDeliverOnReceptionStart(false);
}

void EtherTSN::handleMessage(cMessage *msg)
{
    if (!isOperational) {
        handleMessageWhenDown(msg);
        return;
    }

    if (channelsDiffer)
        readChannelParameters(true);

    if (msg->isSelfMessage())
        handleSelfMessage(msg);
    else if (msg->getArrivalGate() == upperLayerInGate)
        processFrameFromUpperLayer(check_and_cast<EtherFrame *>(msg));
    else if (msg->getArrivalGate() == physInGate)
        processMsgFromNetwork(PK(msg));
    else
        throw cRuntimeError("Message received from unknown gate!");

    if (hasGUI())
        updateDisplayString();
}

void EtherTSN::handleSelfMessage(cMessage *msg)
{
    EV_TRACE << "Self-message " << msg << " received\n";

    if (msg == endTxMsg)
        handleEndTxPeriod();
    else if (msg == endIFGMsg)
        handleEndIFGPeriod();
    else if (msg == endPauseMsg)
        handleEndPausePeriod();
    else
        throw cRuntimeError("Unknown self message received!");
}

void EtherTSN::startFrameTransmission()
{
    ASSERT(curTxFrame);
    EV_DETAIL << "Transmitting a copy of frame " << curTxFrame << endl;

    EtherFrame *frame = curTxFrame->dup();    // note: we need to duplicate the frame because we emit a signal with it in endTxPeriod()

    if (frame->getSrc().isUnspecified())
        frame->setSrc(address);

    if (frame->getByteLength() < curEtherDescr->frameMinBytes)
        frame->setByteLength(curEtherDescr->frameMinBytes);

    // add preamble and SFD (Starting Frame Delimiter), then send out
    frame->addByteLength(PREAMBLE_BYTES + SFD_BYTES);

    // send
    EV_INFO << "Transmission of " << frame << " started.\n";
    send(frame, physOutGate);

    scheduleAt(transmissionChannel->getTransmissionFinishTime(), endTxMsg);
    transmitState = TRANSMITTING_STATE;
    emit(transmitStateSignal, TRANSMITTING_STATE);
}

void EtherTSN::processFrameFromUpperLayer(EtherFrame *frame)
{
    if (frame->getByteLength() < MIN_ETHERNET_FRAME_BYTES)
        frame->setByteLength(MIN_ETHERNET_FRAME_BYTES); // "padding"

    frame->setFrameByteLength(frame->getByteLength());

    EV_INFO << "Received " << frame << " from upper layer." << endl;

    emit(packetReceivedFromUpperSignal, frame);

    if (frame->getDest().equals(address)) {
        throw cRuntimeError("logic error: frame %s from higher layer has local MAC address as dest (%s)",
                frame->getFullName(), frame->getDest().str().c_str());
    }

    if (frame->getByteLength() > MAX_ETHERNET_FRAME_BYTES) {
        throw cRuntimeError("packet from higher layer (%d bytes) exceeds maximum Ethernet frame size (%d)",
                (int)(frame->getByteLength()), MAX_ETHERNET_FRAME_BYTES);
    }

    if (!connected || disabled) {
        EV_WARN << (!connected ? "Interface is not connected" : "MAC is disabled") << " -- dropping packet " << frame << endl;
        emit(dropPkFromHLIfaceDownSignal, frame);
        numDroppedPkFromHLIfaceDown++;
        delete frame;

        requestNextFrameFromExtQueue();
        return;
    }

    // fill in src address if not set
    if (frame->getSrc().isUnspecified())
        frame->setSrc(address);

    bool isPauseFrame = (dynamic_cast<EtherPauseFrame *>(frame) != nullptr);

    if (!isPauseFrame) {
        numFramesFromHL++;
        emit(rxPkFromHLSignal, frame);
    }

/*    if (txQueue.extQueue) {
        if(curTxFrame!=NULL)
        {
            EV<<curTxFrame->getName()<<endl;
            EV<<frame->getName()<<endl;
        }
        ASSERT(curTxFrame == nullptr);
        ASSERT(transmitState == TX_IDLE_STATE || transmitState == PAUSE_STATE);
        curTxFrame = frame;
    }
    else {*/
        if (txQueue.innerQueue->isFull())
            throw cRuntimeError("txQueue length exceeds %d -- this is probably due to "
                                "a bogus app model generating excessive traffic "
                                "(or if this is normal, increase txQueueLimit!)",
                    txQueue.innerQueue->getQueueLimit());
        // store frame and possibly begin transmitting
        EV_DETAIL << "Frame " << frame << " arrived from higher layers, enqueueing\n";
        txQueue.innerQueue->insertFrame(frame);

//        if (!curTxFrame && !txQueue.innerQueue->isEmpty() && transmitState == TX_IDLE_STATE)
//            curTxFrame = (EtherFrame *)txQueue.innerQueue->pop();
//    }

    if (transmitState == TX_IDLE_STATE)
    {
        getNextFrameFromQueue();
        startFrameTransmission();
    }
}

void EtherTSN::processMsgFromNetwork(cPacket *pk)
{
    if (dynamic_cast<RawPacket *>(pk)) {
        using namespace serializer;
        RawPacket *rp = static_cast<RawPacket *>(pk);
        Buffer b(rp->getByteArray().getDataPtr(), rp->getByteArray().getDataArraySize());
        Context c;
        cPacket *deserializedPk = SerializerBase::lookupAndDeserialize(b, c, LINKTYPE, LINKTYPE_ETHERNET);
        if (deserializedPk) {
            delete rp;
            pk = deserializedPk;
        }
    }
    EtherTraffic *msg = check_and_cast<EtherTraffic *>(pk);

    EV_INFO << "Reception of " << msg << " started." << endl;

    if (!connected || disabled) {
        EV_WARN << (!connected ? "Interface is not connected" : "MAC is disabled") << " -- dropping msg " << msg << endl;
        if (dynamic_cast<EtherFrame *>(msg)) {    // do not count JAM and IFG packets
            emit(dropPkIfaceDownSignal, msg);
            numDroppedIfaceDown++;
        }
        delete msg;

        return;
    }

    EtherFrame *frame = dynamic_cast<EtherFrame *>(msg);
    if (!frame) {
        if (dynamic_cast<EtherIFG *>(msg))
            throw cRuntimeError("There is no burst mode in full-duplex operation: EtherIFG is unexpected");
        check_and_cast<EtherFrame *>(msg);
    }

    totalSuccessfulRxTime += frame->getDuration();

    // bit errors
    if (frame->hasBitError()) {
        numDroppedBitError++;
        emit(dropPkBitErrorSignal, frame);
        delete frame;
        return;
    }

    if (!dropFrameNotForUs(frame)) {
        if (dynamic_cast<EtherPauseFrame *>(frame) != nullptr) {
            int pauseUnits = ((EtherPauseFrame *)frame)->getPauseTime();
            delete frame;
            numPauseFramesRcvd++;
            emit(rxPausePkUnitsSignal, pauseUnits);
            processPauseCommand(pauseUnits);
        }
        else {
            EV_INFO << "Reception of " << frame << " successfully completed." << endl;
            processReceivedDataFrame((EtherFrame *)frame);
        }
    }
}

void EtherTSN::handleEndIFGPeriod()
{
    ASSERT(nullptr == curTxFrame);
    if (transmitState != WAIT_IFG_STATE)
        throw cRuntimeError("Not in WAIT_IFG_STATE at the end of IFG period");

    // End of IFG period, okay to transmit
    EV_DETAIL << "IFG elapsed" << endl;

    getNextFrameFromQueue();
    beginSendFrames();
}

void EtherTSN::handleEndTxPeriod()
{
    // we only get here if transmission has finished successfully
    if (transmitState != TRANSMITTING_STATE)
        throw cRuntimeError("End of transmission, and incorrect state detected");

    if (nullptr == curTxFrame)
        throw cRuntimeError("Frame under transmission cannot be found");

    emit(packetSentToLowerSignal, curTxFrame);    //consider: emit with start time of frame

    if (dynamic_cast<EtherPauseFrame *>(curTxFrame) != nullptr) {
        numPauseFramesSent++;
        emit(txPausePkUnitsSignal, ((EtherPauseFrame *)curTxFrame)->getPauseTime());
    }
    else {
        unsigned long curBytes = curTxFrame->getFrameByteLength();
        numFramesSent++;
        numBytesSent += curBytes;
        emit(txPkSignal, curTxFrame);
    }

    EV_INFO << "Transmission of " << curTxFrame << " successfully completed.\n";
    delete curTxFrame;
    curTxFrame = nullptr;
    lastTxFinishTime = simTime();


    if (pauseUnitsRequested > 0) {
        // if we received a PAUSE frame recently, go into PAUSE state
        EV_DETAIL << "Going to PAUSE mode for " << pauseUnitsRequested << " time units\n";

        scheduleEndPausePeriod(pauseUnitsRequested);
        pauseUnitsRequested = 0;
    }
    else {
        EV_DETAIL << "Start IFG period\n";
        scheduleEndIFGPeriod();
    }
}

void EtherTSN::finish()
{
    EtherMACBase::finish();

    simtime_t t = simTime();
    simtime_t totalRxChannelIdleTime = t - totalSuccessfulRxTime;
    recordScalar("rx channel idle (%)", 100 * (totalRxChannelIdleTime / t));
    recordScalar("rx channel utilization (%)", 100 * (totalSuccessfulRxTime / t));
}

void EtherTSN::handleEndPausePeriod()
{
    ASSERT(nullptr == curTxFrame);
    if (transmitState != PAUSE_STATE)
        throw cRuntimeError("End of PAUSE event occurred when not in PAUSE_STATE!");

    EV_DETAIL << "Pause finished, resuming transmissions\n";
    getNextFrameFromQueue();
    beginSendFrames();
}

void EtherTSN::processReceivedDataFrame(EtherFrame *frame)
{
    emit(packetReceivedFromLowerSignal, frame);

    // strip physical layer overhead (preamble, SFD) from frame
    frame->setByteLength(frame->getFrameByteLength());

    // statistics
    unsigned long curBytes = frame->getByteLength();
    numFramesReceivedOK++;
    numBytesReceivedOK += curBytes;
    emit(rxPkOkSignal, frame);

    numFramesPassedToHL++;
    emit(packetSentToUpperSignal, frame);
    // pass up to upper layer
    EV_INFO << "Sending " << frame << " to upper layer.\n";
    send(frame, "upperLayerOut");
}

void EtherTSN::processPauseCommand(int pauseUnits)
{
    if (transmitState == TX_IDLE_STATE) {
        EV_DETAIL << "PAUSE frame received, pausing for " << pauseUnitsRequested << " time units\n";
        if (pauseUnits > 0)
            scheduleEndPausePeriod(pauseUnits);
    }
    else if (transmitState == PAUSE_STATE) {
        EV_DETAIL << "PAUSE frame received, pausing for " << pauseUnitsRequested
                  << " more time units from now\n";
        cancelEvent(endPauseMsg);

        if (pauseUnits > 0)
            scheduleEndPausePeriod(pauseUnits);
    }
    else {
        // transmitter busy -- wait until it finishes with current frame (endTx)
        // and then it'll go to PAUSE state
        EV_DETAIL << "PAUSE frame received, storing pause request\n";
        pauseUnitsRequested = pauseUnits;
    }
}

void EtherTSN::scheduleEndIFGPeriod()
{
    ASSERT(nullptr == curTxFrame);
    transmitState = WAIT_IFG_STATE;
    emit(transmitStateSignal, WAIT_IFG_STATE);
    simtime_t endIFGTime = simTime() + (INTERFRAME_GAP_BITS / curEtherDescr->txrate);
    scheduleAt(endIFGTime, endIFGMsg);
}

void EtherTSN::scheduleEndPausePeriod(int pauseUnits)
{
    ASSERT(nullptr == curTxFrame);
    // length is interpreted as 512-bit-time units
    simtime_t pausePeriod = ((pauseUnits * PAUSE_UNIT_BITS) / curEtherDescr->txrate);
    scheduleAt(simTime() + pausePeriod, endPauseMsg);
    transmitState = PAUSE_STATE;
    emit(transmitStateSignal, PAUSE_STATE);
}

void EtherTSN::beginSendFrames()
{
    if (curTxFrame) {
        // Other frames are queued, transmit next frame
        EV_DETAIL << "Transmit next frame in output queue\n";
        startFrameTransmission();
    }
    else {
        // No more frames set transmitter to idle
        transmitState = TX_IDLE_STATE;
        emit(transmitStateSignal, TX_IDLE_STATE);
        if (!txQueue.extQueue) {
            // Output only for internal queue (we cannot be shure that there
            //are no other frames in external queue)
            EV_DETAIL << "No more frames to send, transmitter set to idle\n";
        }
    }
}
void EtherTSN::getNextFrameFromQueue()//在endIFG处理函数使用，并没有判断curTxFrame是否为空
{
    if (!txQueue.innerQueue->isEmpty())
    {
        curTxFrame = (EtherFrame *)txQueue.innerQueue->pop();
    }
    else
    {
        if (txQueue.extQueue)
        {
            if (txQueue.extQueue->getNumPendingRequests() == 0)
            {
                txQueue.extQueue->requestPacket();
            }
        }
    }
}

} // namespace inet

