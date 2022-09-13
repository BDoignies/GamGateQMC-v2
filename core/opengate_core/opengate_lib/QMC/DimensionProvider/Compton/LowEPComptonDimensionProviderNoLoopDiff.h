#pragma once

#include "../PartDimensionProvider.h"
#include "../../CurrentTrackInformationAction.h"

// Same as LowEPComptonDimensionProvider, but inner loop are not considered 
class LowEPComptonDimensionProviderNoLoop : public PartDimensionProvider
{
public:
    LowEPComptonDimensionProviderNoLoop();
    
    bool Accept(
        const std::string& processName,
        const std::string& className, 
        const std::string& funcName
    ) override;

    unsigned int GetMaxDimension() const override;
    DimensionCount GetCurrentDimension() override;
private: 
};