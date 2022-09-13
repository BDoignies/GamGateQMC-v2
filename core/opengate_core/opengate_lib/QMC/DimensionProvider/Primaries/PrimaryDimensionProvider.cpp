#include "PrimaryDimensionProvider.h"

PrimaryDimensionProvider::PrimaryDimensionProvider(unsigned int c) : 
    PartDimensionProvider("PrimaryDimensionProvider"),
    previousEvent(0), counter(0), count(c)
{ }

bool PrimaryDimensionProvider::Accept(
    const std::string& processName, 
    const std::string& className, 
    const std::string& funcName
)
{
    int currentEvent = CurrentTrackInformation::GetEventID();
    if (currentEvent != previousEvent)
    {
        previousEvent = currentEvent;
        counter = 0;
    }

    if (counter >= count)
        return false;

    if (className == "G4SPSRandomGenerator")        return true;
    if (className == "GamSPSVoxelsPosDistribution") return true;
    
    // std::cout << "[EMDimensionProvider] Rejected: " << className << "::" << funcName << std::endl;
    return false;
}

unsigned int PrimaryDimensionProvider::GetMaxDimension() const
{
    return count;
}

DimensionCount PrimaryDimensionProvider::GetCurrentDimension()
{
    return counter ++;
}