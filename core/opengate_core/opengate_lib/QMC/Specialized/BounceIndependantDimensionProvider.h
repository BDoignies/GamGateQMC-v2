#pragma once

#include "PartDimensionProvider.h"

class BounceIndependantDimensionProvider : public TotalDimensionProvider
{
public:
    inline BounceIndependantDimensionProvider(
        PartDimensionProvider* pr,
        const std::vector<PartDimensionProvider*>& p) : 
        TotalDimensionProvider(pr, p)
    { }

    inline unsigned int GetNumberOfDimensionAtBounce(unsigned int i) const override
    {
        // Ignore bounce number !
        return primaryDimensionCount;
    }

private:
    /* data */
};
