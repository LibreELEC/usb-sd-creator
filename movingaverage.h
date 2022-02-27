// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2016-present Team LibreELEC (https://libreelec.tv)

#ifndef MOVINGAVERAGE_H
#define MOVINGAVERAGE_H

#include <list>
#include <limits>

class MovingAverage
{
public:
    MovingAverage(const unsigned int numSamples = 10);
    void AddValue(double val);
    double AverageValue();

private:
    std::list<double> samples;
    unsigned int size;
    double total;
};

#endif // MOVINGAVERAGE_H
