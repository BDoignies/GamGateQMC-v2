#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include <unordered_map>

#include "Sampler.h"
#include "G4Exception.hh"

class FixedDimensionsSampler : public Sampler
{
public:
    FixedDimensionsSampler(
        const std::vector<unsigned int>& dimensions, 
        const std::vector<SampleType>& values
    );

    SampleType Sample(PointCount i, DimensionCount d);

    ~FixedDimensionsSampler();
private:    
    const std::unordered_map<unsigned int, SampleType> fixedValues;
};