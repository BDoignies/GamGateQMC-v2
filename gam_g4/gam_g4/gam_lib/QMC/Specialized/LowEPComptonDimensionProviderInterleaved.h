#pragma once

#include "PartDimensionProvider.h"
#include "../CurrentTrackInformationAction.h"

// Same as LowEPComptonDimensionProvider, but inner loop are not considered 
class LowEPComptonDimensionProviderInterleaved : public PartDimensionProvider
{
public:
    LowEPComptonDimensionProviderInterleaved();
    
    bool Accept(
        const std::string& processName,
        const std::string& className, 
        const std::string& funcName
    ) override;

    unsigned int GetMaxDimension() const override;
    DimensionCount GetCurrentDimension() override;
private: 
    unsigned int photonloop = 0;
};