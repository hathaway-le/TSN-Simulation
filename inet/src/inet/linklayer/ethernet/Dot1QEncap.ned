package inet.linklayer.ethernet;

simple Dot1QEncap like IEtherEncap
{
    parameters:
        // TSN Scenario Configuration
        int dot1Q_NullStreamVlan = default(1);

        bool useSNAP = default(false);  // create EtherFrameWithSNAP frames instead of EthernetIIFrame
        @display("i=block/square");
        @signal[encapPk](type=cPacket);
        @signal[decapPk](type=cPacket);
        @signal[pauseSent](type=long); // pause units
        @statistic[encapPk](title="packets encapsulated";source=encapPk; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[decapPk](title="packets decapsulated";source=decapPk; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[pauseSent](title="pause sent";record=count,sum,vector);

    gates:
        input upperLayerIn @labels(Ieee802Ctrl/down); // higher layer
        output upperLayerOut @labels(Ieee802Ctrl/up); // higher layer
        input lowerLayerIn @labels(EtherFrame);   // to Ethernet MAC
        output lowerLayerOut @labels(EtherFrame); // to Ethernet MAC
}