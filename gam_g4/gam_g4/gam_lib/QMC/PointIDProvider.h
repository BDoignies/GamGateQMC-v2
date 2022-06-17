#pragma once

#include <vector>
#include "QMC_utils.h"

class PointIDProvider
{
public:
    PointIDProvider(const std::string& name_) : name(name_) 
    { }

    inline std::string GetName() const { return name; }

    virtual PointCount GetCurrentPointID(DimensionCount d) = 0; 
    virtual ~PointIDProvider() {}
private:
    std::string name;
};

class DefaultPointIDProvider : public PointIDProvider
{
public:
    DefaultPointIDProvider() : PointIDProvider("DefaultPointIDProvider"), sample(0)
    { }

    inline PointCount GetCurrentPointID(DimensionCount d)
    {
        if (d == UNKNOWN_DIMENSION) return UNKNOWN_POINTID;

        auto it = std::find(lastDimensions.begin(), lastDimensions.end(), d);
        if (it != lastDimensions.end())
        {
            sample ++;
            lastDimensions.clear();
        }
        
        lastDimensions.push_back(d);
        return sample;
    }
protected:
    std::size_t sample;
    std::vector<DimensionCount> lastDimensions;
};