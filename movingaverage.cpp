// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2016-present Team LibreELEC (https://libreelec.tv)

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
