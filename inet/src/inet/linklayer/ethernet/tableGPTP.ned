//
// @authors: Enkhtuvshin Janchivnyambuu
//           Henning Puttnies
//           Peter Danielis
//           University of Rostock, Germany
// Modifier: Hathaway Le  

package inet.linklayer.ethernet;

//
// this will be used to store a value of correctionField and rateRatio
//
simple TableGPTP
{
    parameters:
        string nodeType = default("SlaveNode"); // MasterNode, BridgeNode and SlaveNode
        double correctionField = default(0);
        double rateRatio = default(1);
        string control_gate_table = default("");
    gates:
        input gptpLayerIn[];
        output gptpLayerOut[];
}
