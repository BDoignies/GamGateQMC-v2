#pragma once

#include "QMC_utils.h"
#include <CLHEP/Random/MixMaxRng.h>

class Sampler
{
public:
    inline Sampler(const std::string& name_) : name(name_) 
    { }

    inline std::string GetName() const  { return name; }

    virtual SampleType Sample(PointCount i, DimensionCount d) = 0;

    virtual inline void SetSeed(long seed)  { whitenoise.setSeed(seed); }

    inline SampleType Whitenoise()  { return whitenoise.flat(); }

    virtual ~Sampler() 
    { }
private:
    std::string name;
    CLHEP::MixMaxRng whitenoise;
};

/* Default sampler */
class WhiteNoiseSampler : public Sampler
{
public:
    inline WhiteNoiseSampler() : Sampler("Whitenoise") { }

    SampleType Sample(PointCount i, DimensionCount d) override
    {
        ((void) i); ((void) d);
        return Whitenoise();
    }
};