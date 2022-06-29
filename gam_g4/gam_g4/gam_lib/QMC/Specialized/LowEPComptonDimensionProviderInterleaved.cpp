#include "LowEPComptonDimensionProviderInterleaved.h"

LowEPComptonDimensionProviderInterleaved::LowEPComptonDimensionProviderInterleaved() : 
    PartDimensionProvider("LowEPCompton")
{ }

unsigned int LowEPComptonDimensionProviderInterleaved::GetMaxDimension() const
{
    unsigned int count = 0;

    count ++; // EmProcess::PostStepGetPhysicalInteractionLength(605)
    count ++; // EmElementSelector::SelectRandomAtom(112)

    count += (
        1 + // LowEPComptonModel::SampleSecondaries(358)
        1 + // LowEPComptonModel::SampleSecondaries(360)
        // 1 + // LowEPComptonModel::SampleSecondaries(365)
        1   // LowEPComptonModel::SampleSecondaries(375)
    ) * 1;
    
    count ++; // LowEPComptonModel::SampleSecondaries(379)

    count += (
        1 + // ShellData::SelectRandomShell(352)
        1 + // EMDataSet::RandomSelect(540)
        1 + // LowEPComptonModel::SampleSecondaries(440)
        1 + // LowEPComptonModel::SampleSecondaries(441)
        1   // LowEPComptonModel::SampleSecondaries(537)
    ) * 1;

    return 10;
}

DimensionCount LowEPComptonDimensionProviderInterleaved::GetCurrentDimension()
{    
    static unsigned int lastBounce = 0;
    static int lastEvent = 0;
    const std::source_location& location = CurrentTrackInformation::currentLocation;
    const auto line = location.line();
    
    const auto currentBounce = CurrentTrackInformation::globalStepInformations.interactionNumber;
    // const std::string& className = CurrentTrackInformation::currentClassName;
    // const std::string& funcName  = CurrentTrackInformation::currentFuncName;

    // Reordering : 112 = 5 <=> 379 = 1. Breaks (4, 5) patters of sobol ???
    if (line == 605) 
    {
        if (CurrentTrackInformation::GetEventID() != lastEvent)
        {    lastEvent = CurrentTrackInformation::GetEventID(); lastBounce = 0; }

        return lastBounce++;
    }  // EmProcess::PostStepGetPhysicalInteractionLength(605)
    
    // if (currentBounce >= 1) return UNKNOWN_DIMENSION;
    // if (line == 112) { return  2; }; // EmElementSelector::SelectRandomAtom(112)
    //     
    // if (photonloop == 0)
    // {
    //     if (line == 358) {                return  3; }; // LowEPComptonModel::SampleSecondaries(358)
    //     if (line == 360) {                return  4; }; // LowEPComptonModel::SampleSecondaries(360)
    //     if (line == 365) {                return  4; }; // LowEPComptonModel::SampleSecondaries(365)
    //     if (line == 375) { photonloop ++; return  5; }; // LowEPComptonModel::SampleSecondaries(375)
    // }                                                                                               
    // else if (photonloop == 1)                                                                       
    // {                                                                                               
    //     if (line == 358) {                return 12; }; // LowEPComptonModel::SampleSecondaries(358)
    //     if (line == 360) {                return 13; }; // LowEPComptonModel::SampleSecondaries(360)
    //     if (line == 365) {                return 13; }; // LowEPComptonModel::SampleSecondaries(365)
    //     if (line == 375) { photonloop ++; return 14; }; // LowEPComptonModel::SampleSecondaries(375)
    // }
    // else 
    // {
    //     return UNKNOWN_DIMENSION;        
    // }
    // 
    // if (line == 379) { return  6; }; // LowEPComptonModel::SampleSecondaries(379)
    // if (line == 352) { return  7; }; // ShellData::SelectRandomShell(352)
    // if (line == 540) { return  8; }; // EMDataSet::RandomSelect(540)
    // if (line == 440) { return  9; }; // LowEPComptonModel::SampleSecondaries(440)
    // if (line == 441) { return 10; }; // LowEPComptonModel::SampleSecondaries(441)
    // if (line == 537) { return 11; }; // LowEPComptonModel::SampleSecondaries(537)
    // 
    // 
    return UNKNOWN_DIMENSION;
}

bool LowEPComptonDimensionProviderInterleaved::Accept(
            const std::string& processName,
            const std::string& className, 
            const std::string& funcName
)
{
    const auto currentBounce = CurrentTrackInformation::globalStepInformations.interactionNumber;
    if (currentBounce >= 10) return false;
    
    if (className == "G4LowEPComptonModel") return true;
    if (className == "G4VEmProcess")        return true;   
    if (className == "G4EmElementSelector") return true;
    if (className == "G4ShellData")         return true;
    if (className == "G4EMDataSet")         return true;

    // std::cout << "Rejected: " << className << std::endl;
    return false;
}
