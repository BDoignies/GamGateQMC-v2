#include "SamplerFixedDimensions.h"

// Function to allow fixed_sample to be const
std::unordered_map<unsigned int, SampleType> build_map(
    const std::vector<unsigned int>& dims, 
    const std::vector<SampleType>& vals)
{
    const unsigned int min = std::min(dims.size(), vals.size());

    std::unordered_map<unsigned int, SampleType> map;
    for (unsigned int i = 0; i < min; i++)
        map[dims[i]] = vals[i];

    return map;
}

FixedDimensionsSampler::FixedDimensionsSampler(
    const std::vector<unsigned int>& dimensions, 
    const std::vector<SampleType>& values
) : Sampler("FixedDimensionsSampler"),
    fixedValues(build_map(dimensions, values))
{
    if (dimensions.size() != values.size())
    {
        std::string msg = "[FixedDimensionsSampler]: " +
            std::string("Dimension mismatch (") + 
            std::to_string(dimensions.size()) +
            std::string(" and ") +
            std::to_string(values.size()) +
            std::string(").")    
        ;
        G4Exception(
            __FILE__, __PRETTY_FUNCTION__, 
            G4ExceptionSeverity::FatalErrorInArgument, msg.c_str()
        );
    }
}

SampleType FixedDimensionsSampler::Sample(PointCount i, DimensionCount d_)
{
    auto it = fixedValues.find(d_);
    if (it != fixedValues.end()) return it->second;
    return Whitenoise();
}

FixedDimensionsSampler::~FixedDimensionsSampler()
{ }