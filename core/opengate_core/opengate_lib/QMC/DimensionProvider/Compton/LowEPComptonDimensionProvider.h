#pragma once

#include "../PartDimensionProvider.h"
#include "../../CurrentTrackInformationAction.h"

class LowEPComptonDimensionProvider : public PartDimensionProvider
{
public:
    LowEPComptonDimensionProvider(
        unsigned int max_bounce, unsigned int max_photon_loop, unsigned int max_electron_loop
    );
    
    bool Accept(
        const std::string& processName,
        const std::string& className, 
        const std::string& funcName
    ) override;

    unsigned int GetMaxDimension() const override;
    DimensionCount GetCurrentDimension() override;
private: 
    unsigned int previous_bounce;
    unsigned int current_loop_counter;

    
    unsigned int max_bounce;
    unsigned int max_photon_loop;
    unsigned int max_electron_loop;
};