
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
            @display("p=230,179");
        }
        gPtpMaster: GPtpMaster {
            @display("p=91,180");
        }
        gPtpSlave: GPtpSlave {
            @display("p=384,180");
        }
        standardHost0: StandardHost {
            @display("p=230,61");
        }
        configurator: IPv4NetworkConfigurator {
            @display("p=83,38");
        }
    connections:
        gPtpMaster.ethg++ <--> {  datarate = 1Gbps; delay = 250ns; } <--> gPtpBridge0.ethg++;
        standardHost0.ethg++ <--> {  datarate = 1Gbps; delay = 250ns; } <--> gPtpBridge0.ethg++;
        gPtpBridge0.ethg++ <--> {  datarate = 1Gbps; delay = 250ns; } <--> gPtpSlave.ethg++;
}
