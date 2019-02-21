// authors: Hathaway Le 

package qbv.simulations;

import qbv.GPtpBridge;
import qbv.GPtpMaster;
import qbv.GPtpSlave;
import inet.networklayer.configurator.ipv4.IPv4NetworkConfigurator;
import inet.node.inet.StandardHost;


network Qbv
{
    @display("bgb=550,360");
    submodules:
        gPtpBridge0: GPtpBridge {
            @display("p=209,179");
        }
        gPtpBridge1: GPtpBridge {
            @display("p=314,179");
        }
        gPtpMaster: GPtpMaster {
            @display("p=97,120");
        }
        gPtpSlave0: GPtpSlave {
            @display("p=436,120");
        }
        gPtpSlave1: GPtpSlave {
            @display("p=436,253");
        }
        standardHost0: StandardHost {
            @display("p=97,253");
        }
        configurator: IPv4NetworkConfigurator {
            @display("p=83,38");
        }
    connections:
        gPtpMaster.ethg++ <--> {  datarate = 1Gbps; delay = 250ns; } <--> gPtpBridge0.ethg++;
        standardHost0.ethg++ <--> {  datarate = 1Gbps; delay = 250ns; } <--> gPtpBridge0.ethg++;
        gPtpBridge0.ethg++ <--> {  datarate = 1Gbps; delay = 250ns; } <--> gPtpBridge1.ethg++;
        gPtpBridge1.ethg++ <--> {  datarate = 1Gbps; delay = 250ns; } <--> gPtpSlave0.ethg++;
        gPtpBridge1.ethg++ <--> {  datarate = 1Gbps; delay = 250ns; } <--> gPtpSlave1.ethg++;        
}
