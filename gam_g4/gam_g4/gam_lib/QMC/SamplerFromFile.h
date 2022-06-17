#pragma once

#include <fstream>
#include <iostream>
#include <sstream>

#include "Sampler.h"
#include "G4Exception.hh"

class SamplerFromFile : public Sampler
{
public:
    SamplerFromFile(const std::string& filename, PointCount n, DimensionCount d);

    SampleType Sample(PointCount i, DimensionCount d);

    void Read(const std::string& filename);

    ~SamplerFromFile();
private:    
    const PointCount n;
    const DimensionCount d;

    SampleType* data;
};