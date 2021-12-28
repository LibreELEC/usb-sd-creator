////////////////////////////////////////////////////////////////////////////////
//      This file is part of LibreELEC - http://www.libreelec.tv
//      Copyright (C) 2016 Team LibreELEC
//
//  LibreELEC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 2 of the License, or
//  (at your option) any later version.
//
//  LibreELEC is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with LibreELEC.  If not, see <http://www.gnu.org/licenses/>.
////////////////////////////////////////////////////////////////////////////////

#include "movingaverage.h"

// idea from http://www.codeproject.com/Articles/17860/A-Simple-Moving-Average-Algorithm

// initialize the sample size to the specified number
MovingAverage::MovingAverage(const unsigned int numSamples)
{
    size = numSamples;
    total = 0;
}

// add sample to a list
void MovingAverage::AddValue(double val)
{
    if (samples.size() == size) {
        // substract the oldest value and remove it from the list
        total -= samples.front();
        samples.pop_front();
    }

    samples.emplace_back(val);  // add new value to the list
    total += val;
}

// get the average value
double MovingAverage::AverageValue()
{
    //if (samples.size() < size / 10)
    //    return std::numeric_limits<double>::max();

    return total / samples.size();
}
