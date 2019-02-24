//
// @authors: Enkhtuvshin Janchivnyambuu
//           Henning Puttnies
//           Peter Danielis
//           University of Rostock, Germany
// Modifier: Hathaway Led

#include <inet/linklayer/ethernet/tableGPTP.h>

namespace inet {
    Define_Module(TableGPTP);
    TableGPTP::TableGPTP()
    {
        selfQbv = NULL;
    }

    TableGPTP::~TableGPTP()
    {
        cancelAndDelete(selfQbv);
    }

    void TableGPTP::initialize(int stage)
    {
        if(stage == 0)//stage == 0 的时候，时钟还未初始化好
        {
            correctionField = par("correctionField");
            rateRatio = par("rateRatio");
            peerDelay = 0;
            numberOfGates = gateSize("gptpLayerIn");
            originTimestamp=0;
            offset=0;
            cModule* gPtpNode=getParentModule();//爸爸的爸爸，host or bridge 不是endstation(nodebase)
            nodeType = gPtpNode->par("gPtpNodeType");
            if(selfQbv==NULL)
                selfQbv = new cMessage("selfQbv");
            cStringTokenizer tokenizer0(par("control_gate_table").stringValue(), ";");
            if (tokenizer0.hasMoreTokens()) {
//                cStringTokenizer tokenizer1(tokenizer0.nextToken(), ",");
//                std::vector<std::string> vector1 = tokenizer1.asVector();
//                EV << "gate " << vector1[0] << " duration " << vector1[1] << endl;
                std::vector<std::string> vector1 = tokenizer0.asVector();
                for(unsigned int j=0;j<vector1.size();j++)
                {
                    EV << "gate " << vector1[j] << endl;
                    gate_state temp;
                    for(unsigned int i=0;i<vector1[j].size();i++)
                    {
                        temp.gate_state_this.push_back(vector1[j][i]-'0');//用atoi并不方便，因为取vector1[0][i]的地址的话，会一直算到‘\0’才停
                    }
                    all_gate_state.push_back(temp);
                }
            }
        }
        else if(stage == 1)
        {
            cModule* gPtpNode=getParentModule();
            if(gPtpNode->getSubmodule("hwClock")!=nullptr)
             {
                clockGptp = check_and_cast<HardwareClock *>(gPtpNode->getSubmodule("hwClock"));
             }
        }
    }

    void TableGPTP::handleMessage(cMessage *msg)
    {
        if(msg->arrivedOn("gptpLayerIn"))
        {
            if(nodeType == BRIDGE_NODE)
            {
                if(qbvstart == false)//从接受到第一个followup为开始
                {
                    qbvstart = true;
                    //每次只需改变cur_state

    //                SimTime time_now = clockGptp->getHWtime();
    //                SimTime time_int = all_gate_state[cur_state].duration;
    //                SimTime time_next = time_now+time_int;
    //                scheduleAt(simTime()+0.000010,selfQbv);
                }
                cur_state = w_gptp;
                EV<<"gptp start "<<simTime()<<endl;
                clockGptp->qbv_scheduleAtHWtime(clockGptp->getHWtime() +0.000005,selfQbv,this);
            }
            for (int i = 0; i < numberOfGates; i++)
            {
                send(msg->dup(), "gptpLayerOut", i);
            }
            delete msg;
        }
        else if(msg->isSelfMessage())
        {
            if(msg == selfQbv)
            {
                switch(cur_state)
                {
                    case w_gptp:
                        cur_state = w_pro;
                        EV<<"pro start "<<simTime()<<endl;
                        clockGptp->qbv_scheduleAtHWtime(clockGptp->getHWtime()+0.000015,selfQbv,this);
//                        scheduleAt(simTime()+0.000010,selfQbv);
                        break;
                    case w_pro:
                        cur_state = w_unpro;
                        EV<<"unpro start "<<simTime()<<endl;
                        clockGptp->qbv_scheduleAtHWtime(clockGptp->getHWtime()+0.000965,selfQbv,this);
//                        clockGptp->qbv_scheduleAtHWtime(clockGptp->getHWtime()+0.009965,selfQbv,this);
//                        scheduleAt(simTime()+0.000015,selfQbv);
                        break;
                    case w_unpro:
                        cur_state = w_guard;
                        EV<<"guard start "<<simTime()<<endl;//guard的时间取决于下一个同步followup什么时候来，按照注释里的方法，无法控制累加误差
//                        clockGptp->qbv_scheduleAtHWtime(clockGptp->getHWtime()+0.000015,selfQbv,this);
//                        scheduleAt(simTime()+0.009960,selfQbv);
                        break;
//                    case w_guard:
//                        cur_state = w_gptp;
//                        EV<<"gptp start "<<simTime()<<endl;
//                        clockGptp->qbv_scheduleAtHWtime(clockGptp->getHWtime()+0.000015,selfQbv,this);
//                        scheduleAt(simTime()+0.000015,selfQbv);
//                        break;
                    default:break;
                }
//                scheduleAt(simTime()+all_gate_state[cur_state].duration,selfQbv);
//                clockGptp->qbv_scheduleAtHWtime(clockGptp->getHWtime()+all_gate_state[cur_state].duration,selfQbv,this);
            }
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

    void TableGPTP::setdiff(SimTime cf)
    {
        diff = cf;
    }

    SimTime TableGPTP::getdiff()
    {
        return diff;
    }

    std::vector<int> TableGPTP::getgatestate()
    {
        if(qbvstart == false)
        {
           std::vector<int> temp;
           for(unsigned i=0;i<8;i++)
           {
               temp.push_back(1);
           }
           return temp;
        }
        else
        {
            return all_gate_state[cur_state].gate_state_this;
        }
    }

    void TableGPTP::scheduleAtInObject(const SimTime realtime, cMessage* msg)//在hardwareclock module 使用，取回schedule和msg控制权
    {
        Enter_Method_Silent();
        take(msg);

        scheduleAt(realtime, msg);
    }
}
