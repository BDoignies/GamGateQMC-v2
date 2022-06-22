#include "LowEPComptonDimensionProvider.h"

LowEPComptonDimensionProvider::LowEPComptonDimensionProvider(
    unsigned int mb, unsigned int mp, unsigned int me
) : 
    PartDimensionProvider("LowEPCompton"),
    previous_bounce(0), current_loop_counter(0),
    max_bounce(mb), max_photon_loop(mp), max_electron_loop(me)
{ }

unsigned int LowEPComptonDimensionProvider::GetMaxDimension() const
{
    unsigned int count = 0;

    count ++; // EmProcess::PostStepGetPhysicalInteractionLength(605)
    count ++; // EmElementSelector::SelectRandomAtom(112)

    count += (
        1 + // LowEPComptonModel::SampleSecondaries(358)
        1 + // LowEPComptonModel::SampleSecondaries(360)
        1 + // LowEPComptonModel::SampleSecondaries(365)
        1   // LowEPComptonModel::SampleSecondaries(375)
    ) * max_photon_loop;
    
    count ++; // LowEPComptonModel::SampleSecondaries(379)

    count += (
        1 + // ShellData::SelectRandomShell(352)
        1 + // EMDataSet::RandomSelect(540)
        1 + // LowEPComptonModel::SampleSecondaries(440)
        1 + // LowEPComptonModel::SampleSecondaries(441)
        1   // LowEPComptonModel::SampleSecondaries(537)
    ) * max_electron_loop;

    return count;
}

DimensionCount LowEPComptonDimensionProvider::GetCurrentDimension()
{    
    const std::source_location& location = CurrentTrackInformation::currentLocation;
    const std::string& className = CurrentTrackInformation::currentClassName;
    const std::string& funcName  = CurrentTrackInformation::currentFuncName;
    const auto line = location.line();
    const auto currentBounce = CurrentTrackInformation::globalStepInformations.interactionNumber;

    // Too much bounces
    // if (currentBounce > max_bounce) return UNKNOWN_DIMENSION;
    //  
    // // Reset loop counter
    // if (currentBounce != previous_bounce)
    // {
    //     previous_bounce = currentBounce;
    //     current_loop_counter = 0;
    // }

    // Everything is uniquely identified by line number, so compare against it
    // instead of string names !

    // EmProcess::PostStepGetPhysicalInteractionLength(605)
    if (line == 605) { current_loop_counter = 0; return 0; }
    // EmElementSelector::SelectRandomAtom(112)
    if (line == 112) { return 1; };

    // Too much loop, reject sample to whitenoise!
    const int reject_ph_wn = (current_loop_counter >= max_photon_loop);

    // 4 because at most 4 numbers in the loop 
    // reject_wn allow to rejecting number to whitenoise while leaving the 
    // opportunity to go to electron part 
    const DimensionCount photon_start = 2 + 4 * current_loop_counter + reject_ph_wn * UNKNOWN_DIMENSION;
    if (line == 358) { return 0 + photon_start; };                              // LowEPComptonModel::SampleSecondaries(358)
    if (line == 360) { return 1 + photon_start; };                              // LowEPComptonModel::SampleSecondaries(360)
    if (line == 365) { return 2 + photon_start; };                              // LowEPComptonModel::SampleSecondaries(365)
    if (line == 375) { current_loop_counter ++; return 3 + photon_start; }; // LowEPComptonModel::SampleSecondaries(375)
    
    const DimensionCount photon_end = 2 + 4 * max_photon_loop;
    if (line == 379) { current_loop_counter = 0; return photon_end; };        // LowEPComptonModel::SampleSecondaries(379)

    // Too much loop, reject sample to whitenoise!
    const int reject_el_wn = (current_loop_counter >= max_electron_loop);

    const DimensionCount electron_start = 1 + photon_end + 5 * current_loop_counter + reject_el_wn * UNKNOWN_DIMENSION;
    if (line == 352) return 0 + electron_start;                             // ShellData::SelectRandomShell(352)
    if (line == 540) return 1 + electron_start;                             // EMDataSet::RandomSelect(540)
    if (line == 440) return 2 + electron_start;                             // LowEPComptonModel::SampleSecondaries(440)
    if (line == 441) return 3 + electron_start;                             // LowEPComptonModel::SampleSecondaries(441)
    if (line == 537) { current_loop_counter++; return 4 + electron_start; }; // LowEPComptonModel::SampleSecondaries(537)
    
    return UNKNOWN_DIMENSION;
}

bool LowEPComptonDimensionProvider::Accept(
            const std::string& processName,
            const std::string& className, 
            const std::string& funcName
)
{
    // std::cout << "LowEPComptonModel: " << className << std::endl;
    const auto currentBounce = CurrentTrackInformation::globalStepInformations.interactionNumber;

    // Reject on too many bounces
    if (currentBounce >= max_bounce) return false;

    if (className == "G4VEmProcess") return true;
    if (className == "G4LowEPComptonModel") return true;
    if (className == "G4EmElementSelector") return true;
    if (className == "G4ShellData") return true;
    if (className == "G4EMDataSet") return true;

    // std::cout << "Rejected: " << className << std::endl;
    return false;
}