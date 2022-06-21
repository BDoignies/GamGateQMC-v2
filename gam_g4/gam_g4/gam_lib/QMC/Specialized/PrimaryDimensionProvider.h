#pragma once

#include "PartDimensionProvider.h"
#include "../CurrentTrackInformationAction.h"

class PrimaryDimensionProvider : public PartDimensionProvider
{
public:
    PrimaryDimensionProvider(unsigned int count);

    bool Accept(
        const std::string& processName, 
        const std::string& className, 
        const std::string& funcName
    ) override;

    unsigned int GetMaxDimension() const override;

    DimensionCount GetCurrentDimension() override;
private:    
    mutable int previousEvent;

    unsigned int counter;
    const unsigned int count;
};