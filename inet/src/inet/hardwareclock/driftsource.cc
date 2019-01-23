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

#include <inet/hardwareclock/driftsource.h>
#include <iostream>

using namespace std;

namespace inet {

    Driftsource::~Driftsource()
    {
    }

    double Driftsource::nextValue()
    {
        const double driftLimit = -0.999999;

        double n = next();

        // limit drift to values > -1, so the time can't go back
        if (n < driftLimit)
            return driftLimit;

        return n;
    }

    ConstantDrift::ConstantDrift(double drift)
        : drift(drift)
    {
    }

    double ConstantDrift::next()
    {
        return drift;
    }

    BoundedDrift::BoundedDrift(const cPar& distribution)
        : distribution(distribution)
    {
    }

    double BoundedDrift::next()
    {
        return distribution.doubleValue();
    }

    BoundedDriftVariation::BoundedDriftVariation(const cPar& distribution, double max_drift_variation, double start_value, const simtime_t& tint,const cPar& max_variation)
        : BoundedDrift(distribution)
        , max_drift_change(tint.dbl() * max_drift_variation)
        , last_drift(start_value)
        , max_variation_l(max_variation)
    {
    }

    double BoundedDriftVariation::next()
    {
        //未用上distribution，步进值在范围内均匀分布，而不是之前的直接截断
        double drift = last_drift+distribution.doubleValue();
        //当然大的数据范围还是要满足
        if(drift>0.0001)
            drift=0.0001;
        else if(drift<-0.0001)
            drift=-0.0001;

        last_drift = drift;
        return drift;
    }

}

