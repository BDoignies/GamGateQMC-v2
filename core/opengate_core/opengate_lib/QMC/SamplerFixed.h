#pragma once

#include <unordered_map>
#include "Sampler.h"

class FixedSampler : public Sampler
{
public:
    inline FixedSampler(const std::unordered_map<DimensionCount, SampleType>& spl) : 
        Sampler("FixedSampler"), samples(spl)
    { }

    inline SampleType Sample(PointCount i, DimensionCount d) override
    {
        auto it = samples.find(d);
        if (it == samples.end()) return Whitenoise();
        return it->second;
    }
private:
    std::unordered_map<DimensionCount, SampleType> samples;
};