//
// @authors: Enkhtuvshin Janchivnyambuu
//           Henning Puttnies
//           Peter Danielis
//           University of Rostock, Germany
// Modifier: Hathaway Le  

package qbv;

//
// this will be used to store a value of correctionField and rateRatio
//
simple TableGPTP
{
    parameters:
        string nodeType = default("SlaveNode"); // MasterNode, BridgeNode and SlaveNode
        double correctionField = default(0);
        double rateRatio = default(1);
    gates:
        input gptpLayerIn[];
        output gptpLayerOut[];
}
