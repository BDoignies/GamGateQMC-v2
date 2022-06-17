#pragma once

#include "QMC_utils.h"

class DimensionProvider
{
public:
    inline DimensionProvider(const std::string& name_) : name(name_)
    { }

    inline std::string GetName() const
    {
        return name;
    }

    virtual DimensionCount GetCurrentDimension() = 0;

    virtual ~DimensionProvider() {} 
private:
    std::string name;
};

/* Default proxy */
class WhitenoiseDimensionProvider : public DimensionProvider
{
public:
    inline WhitenoiseDimensionProvider() : DimensionProvider("WhitenoiseDimensionProvider")
    { }

    inline DimensionCount GetCurrentDimension() 
    {
        return 0;
    }
};