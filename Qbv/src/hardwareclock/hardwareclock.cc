/*
    Clock Synchronization Simulation in OMNet++
    Copyright (C) 2012 Jens Steinhauser (https://github.com/JenSte/omnet-ptp)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

    Modifier: Hathaway Le
*/

#include "hardwareclock.h"
#include "storagewindow.h"
#include "EtherGPTP.h"
#include "driftsource.h"
#include <exception>

#include <iostream>

using namespace std;

Define_Module(HardwareClock);

HardwareClock::Properties::Properties()
	: _u(0)
	, _s(0)
{
}

void HardwareClock::Properties::set(const simtime_t& tint, size_t u)
{
	// minimum values
	simtime_t tint_min = SimTime::parse("1ms");////把1ms转换为仿真时间
	size_t u_min = 10;

	if (tint < tint_min) {
		EV << "warning: 'interval' value too small\n";
		_tint = tint_min;
	} else
		_tint = tint;

	if (u < u_min) {
		EV << "warning: 'update' value too small\n";
		_u = u_min;
	} else
		_u = u;

	_s = 2 * _u;//默认2倍
}

HardwareClock::HardwareClock()
	: storageWindow(NULL)
	, selfMsg(NULL)
{
}

HardwareClock::~HardwareClock()
{
	cancelAndDelete(selfMsg);

	cleanup();

}

void HardwareClock::nextUpdate(cMessage* msg)
{
	scheduleAt(simTime() + properties.updateInterval(), msg);
}

void HardwareClock::cleanup()
{
	if (storageWindow) {
		delete storageWindow;
		storageWindow = NULL;
	}

	// delete all remaining messages,
	// clear queue
	while (queue.size() > 0) {
		const QueuedMessage& q = queue.top();

		delete q.msg;

		queue.pop();
	}

	// NOTE: selfMsg isn't deleted
}

void HardwareClock::initialize()
{
	// if needed, clean up stuff from the last run
	cleanup();

	properties.set(par("interval"), par("update"));//默认1s，100个单位,ned定义的变量都要用par

	EV << "update interval: " << properties.updateInterval() << "s\n";

	Driftsource* d = NULL;

	if (hasPar("drift_distribution")) {
		if (hasPar("max_drift_variation"))
			d = new BoundedDriftVariation(par("drift_distribution"),
					par("max_drift_variation"),
					par("start_value"),
					properties.tint(),
					par("max_variation"));
		else
			d = new BoundedDrift(par("drift_distribution"));
	} else
		d = new ConstantDrift(par("constant_drift"));

	storageWindow = new StorageWindow(properties, d);
	updateDisplay();

	cMessage *msg = new cMessage("storage window update");
	nextUpdate(msg);
}

void HardwareClock::handleMessage(cMessage *msg)
{
	if (msg->isSelfMessage()) {//只会被窗口更新selfmsg触发
		// the only self message is to update the storage window

		storageWindow->update();
		updateDisplay();//显示当前（更新窗口）haretime-simtime

		// check if queued messages can be scheduled now
		while (queue.size() > 0) {
			const QueuedMessage& q = queue.top();
			simtime_t real;

			if (!HWtoSimTime(q.time, real)) {
				// requested timestamp is still not in
				// the storage window (and the timestamps
				// of the following messages also not)
				break;
			}

		    q.self->scheduleAtInObject(real, q.msg);

			queue.pop();
		}

		nextUpdate(msg);
	}
}

void HardwareClock::finish()
{
	if (storageWindow)
		storageWindow->finish();
}

void HardwareClock::updateDisplay()
{
	if (!hasGUI()) {
		// skip this if GUI is not running or in express mode
		return;
	}

	simtime_t real = simTime();
	const simtime_t& hard = storageWindow->hardwareTimeBegin();

	simtime_t diff = hard - real;
	double d = fabs(diff.dbl());

	const char  sign = diff < 0 ? '-' : '+';
	const char* unit;
	int val;

	       if (d > 1) {
		val = d;        unit =  "s";
	} else if (d > 1e-3) {
		val = d * 1e3;  unit = "ms";
	} else if (d > 1e-6) {
		val = d * 1e6;  unit = "us";
	} else if (d > 1e-9) {
		val = d * 1e9;  unit = "ns";
	} else if (d > 1e-12) {
		val = d * 1e12; unit = "ps";
	} else {
		getDisplayString().setTagArg("t", 0, "delta t: ca. 0s");
		return;
	}

	char buf[64];
	snprintf(buf, sizeof(buf), "delta t: ca. %c%i%s", sign, val, unit);

	getDisplayString().setTagArg("t", 0, buf);
}

simtime_t HardwareClock::getHWtime(simtime_t of) const
{
	simtime_t now = simTime();

	int k = storageWindow->indexOf(now);

	const StorageWindow::HoldPoint& hp = storageWindow->at(k);

	simtime_t t = now - hp.realTime;

	return hp.hardwareTime + t * (1 + hp.drift) + of;
}

bool HardwareClock::HWtoSimTime(const simtime_t& hwtime, simtime_t& realtime) const
{
	if (hwtime < storageWindow->at(0).hardwareTime ||
	    hwtime > storageWindow->hardwareTimeEnd()) {
		// outside of storage window, can't translate timestamp
		return false;
	}

	// the current interval is the lower limit for the
	// interval the hardware time is in
	size_t k = storageWindow->indexOf(simTime());

	// find the correct interval
	while (k != properties.s() - 1 && storageWindow->at(k + 1).hardwareTime < hwtime)
		k++;

	const StorageWindow::HoldPoint& hp = storageWindow->at(k);

	realtime = hp.realTime + (hwtime - hp.hardwareTime) / (1 + hp.drift);

	return true;
}

void HardwareClock::scheduleAtHWtime(const simtime_t time, cMessage* msg, EtherGPTP* self,simtime_t of)
{
	Enter_Method_Silent();
	take(msg);//your contain class has to take ownership of the inserted messages, and release them when they are removed from the message

	simtime_t nowHW = getHWtime(of);//这是最大的修改，其他地方其实等于没改

	if (time <= nowHW) {
		// message is in the past
		return;
	}

	simtime_t real;

	if (HWtoSimTime(time, real))
	{
	    //判断出是否在窗口内
		// hardware timestamp in storage window
	    if(real<=simTime())
	    {
//	        std::cout<<"real "<<real.dbl()<<" simTime "<<simTime().dbl()<<endl;
	        self->scheduleAtInObject(simTime(),msg);
	        //在允许误差的情况下，让所有消息都发送出去,这样可能会导致本来应该间隔发送的2个msg，同时发送，使用外部队列时，内部队列失效，报错
	    }
	    else
	    {
	        self->scheduleAtInObject(real,msg);
	    }

	} else {//hwtime大于窗口的被放入队列，小于的已经在前面return了
		// hardware timestamp not yet in storage
		// window, keep message for later
	    //不在窗内的time无法计算realtime

        if(time < storageWindow->at(0).hardwareTime )
        {
            self->scheduleAtInObject(simTime(),msg);//在允许误差的情况下，让所有消息都发送出去
        }
        else
        {
            queue.push(QueuedMessage(time, msg,self));//大于窗最大值的话放入优先级队列，QueuedMessage的operator<重载了
        }
	}
}

std::vector<HardwareClock*> HardwareClock::findClocks(const cModule* parent)
{
	std::vector<HardwareClock*> result;

	for (cModule::SubmoduleIterator i(parent); !i.end(); i++) {
		cModule* child = *i;

		if (0 == strcmp("HardwareClock", child->getClassName()))
			result.push_back(check_and_cast<HardwareClock*>(child));
	}

	return result;
}

HardwareClock* HardwareClock::findFirstClock(const cModule* parent)
{
	std::vector<HardwareClock*> clocks = findClocks(parent);

	if (0 == clocks.size())
		throw std::logic_error("no HardwareClock found");

	return clocks[0];
}

