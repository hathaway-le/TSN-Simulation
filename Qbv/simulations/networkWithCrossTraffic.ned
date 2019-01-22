//
// @authors: Enkhtuvshin Janchivnyambuu
//           Henning Puttnies
//           Peter Danielis
//           University of Rostock, Germany
// Modifier: Hathaway Le 

package qbv.simulations;

import qbv.GPtpBridge;
import qbv.GPtpMaster;
import qbv.GPtpSlave;
import inet.networklayer.configurator.ipv4.IPv4NetworkConfigurator;
import inet.node.inet.StandardHost;


network NetworkWithCrossTraffic
{
    @display("bgb=734,360");
    submodules:
        gPtpBridge0: GPtpBridge {
            @display("p=230,179");
        }
        gPtpBridge1: GPtpBridge {
            @display("p=370,179");
        }
        gPtpMaster: GPtpMaster {
            @display("p=91,180");
        }
        gPtpSlave: GPtpSlave {
            @display("p=526.325,179.235");
        }
        standardHost0: StandardHost {
            @display("p=230,61");
        }
        standardHost1: StandardHost {
            @display("p=230,305");
        }
        standardHost2: StandardHost {
            @display("p=370,61");
        }
        standardHost3: StandardHost {
            @display("p=370,305");
        }
        configurator: IPv4NetworkConfigurator {
            @display("p=83,38");
        }
    connections:
        gPtpMaster.ethg++ <--> {  datarate = 1Gbps; delay = 250ns; } <--> gPtpBridge0.ethg++;
        standardHost0.ethg++ <--> {  datarate = 1Gbps; delay = 250ns; } <--> gPtpBridge0.ethg++;
        standardHost1.ethg++ <--> {  datarate = 1Gbps; delay = 250ns; } <--> gPtpBridge0.ethg++;
        gPtpBridge0.ethg++ <--> {  datarate = 1Gbps; delay = 250ns; } <--> gPtpBridge1.ethg++;
        standardHost2.ethg++ <--> {  datarate = 1Gbps; delay = 250ns; } <--> gPtpBridge1.ethg++;
        standardHost3.ethg++ <--> {  datarate = 1Gbps; delay = 250ns; } <--> gPtpBridge1.ethg++;
        gPtpBridge1.ethg++ <--> {  datarate = 1Gbps; delay = 250ns; } <--> gPtpSlave.ethg++;        
}
