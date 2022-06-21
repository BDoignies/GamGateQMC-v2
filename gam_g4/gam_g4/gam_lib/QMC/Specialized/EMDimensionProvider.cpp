#include "EMDimensionProvider.h"

EMDimensionProvider::EMDimensionProvider(unsigned int c) : 
    PartDimensionProvider("EMDimensionProvider"),
    previousEvent(0), counter(0), count(c)
{ }

bool EMDimensionProvider::Accept(
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

    if (className == "G4VEmProcess") return true;
    // std::cout << "[EMDimensionProvider] Rejected: " << className << "::" << funcName << std::endl;
    return false;
}

unsigned int EMDimensionProvider::GetMaxDimension() const
{
    return count;
}

DimensionCount EMDimensionProvider::GetCurrentDimension()
{
    return counter ++;
}