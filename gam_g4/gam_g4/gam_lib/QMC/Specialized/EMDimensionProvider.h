#pragma once

#include "PartDimensionProvider.h"
#include "../CurrentTrackInformationAction.h"

class EMDimensionProvider : public PartDimensionProvider
{
public:
    EMDimensionProvider(unsigned int count);

    bool Accept(
        const std::string& processName, 
        const std::string& className, 
        const std::string& funcName
    );

    unsigned int GetMaxDimension() const override;

    DimensionCount GetCurrentDimension() override;
private:
    int previousEvent;
    
    unsigned int counter;
    const unsigned int count;
};