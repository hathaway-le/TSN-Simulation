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


network NetworkDaisyChain
{
    @display("bgb=534,441");
    types:
        channel Ethernet1000 extends ned.DatarateChannel
        {
            datarate = 1Gbps;	//提高波特率，效果很棒，误差不再有大范围的奇异值
            delay = 25ns;		// Length of connection is 5 meters
        }
    submodules:
        gPtpBridge0: GPtpBridge {
            @display("p=264,162");
        }
        gPtpBridge1: GPtpBridge {
            @display("p=264,258");
        }
        gPtpBridge2: GPtpBridge {
            @display("p=264,349");
        }
        gPtpMaster: GPtpMaster {
            @display("p=264,55");
        }
        gPtpSlave0: GPtpSlave {
            @display("p=408,94");
        }
        gPtpSlave1: GPtpSlave {
            @display("p=408,182");
        }
        gPtpSlave2: GPtpSlave {
            @display("p=408,259");
        }
        gPtpSlave3: GPtpSlave {
            @display("p=408,375");
        }
        gPtpSlave4: GPtpSlave {
            @display("p=124,375");
        }
        gPtpSlave5: GPtpSlave {
            @display("p=124,298");
        }
        gPtpSlave6: GPtpSlave {
            @display("p=124,205");
        }
        gPtpSlave7: GPtpSlave {
            @display("p=124,94");
        }
        configurator: IPv4NetworkConfigurator {
            @display("p=51,33");
        }
    connections:
        gPtpMaster.ethg++ <--> Ethernet1000 <--> gPtpBridge0.ethg++;
        gPtpBridge0.ethg++ <--> Ethernet1000 <--> gPtpSlave0.ethg++;
        gPtpBridge0.ethg++ <--> Ethernet1000 <--> gPtpSlave1.ethg++;
        gPtpBridge0.ethg++ <--> Ethernet1000 <--> gPtpSlave7.ethg++;
        gPtpBridge0.ethg++ <--> Ethernet1000 <--> gPtpBridge1.ethg++;
        gPtpBridge1.ethg++ <--> Ethernet1000 <--> gPtpSlave2.ethg++;
        gPtpBridge1.ethg++ <--> Ethernet1000 <--> gPtpSlave6.ethg++;
        gPtpBridge1.ethg++ <--> Ethernet1000 <--> gPtpSlave5.ethg++;
        gPtpBridge1.ethg++ <--> Ethernet1000 <--> gPtpBridge2.ethg++;
        gPtpBridge2.ethg++ <--> Ethernet1000 <--> gPtpSlave3.ethg++;
        gPtpBridge2.ethg++ <--> Ethernet1000 <--> gPtpSlave4.ethg++;
}
