#include "CurrentTrackInformationAction.h"
#include "G4SteppingManager.hh"

const G4Track* CurrentTrackInformation::track = nullptr;

std::source_location CurrentTrackInformation::currentLocation;
std::string CurrentTrackInformation::currentClassName;
std::string CurrentTrackInformation::currentFuncName;

std::vector<CurrentTrackInformation::NewSecondaryInformation>    CurrentTrackInformation::secondaries;
std::vector<CurrentTrackInformation::StepInformation>      CurrentTrackInformation::stepsInformation;

CurrentTrackInformation::StepInformation CurrentTrackInformation::globalStepInformations = {0, 0};

void CurrentTrackInformation::SetLocation(const std::source_location& location)
{
    CurrentTrackInformation::currentLocation = location;
    ParseSourceLocation(location, 
        CurrentTrackInformation::currentClassName,
        CurrentTrackInformation::currentFuncName
    );
}

#include "Randomize.hh"

DyingParticleProcess::DyingParticleProcess()
{
    die();
}

void DyingParticleProcess::die()
{
    CurrentTrackInformation::globalStepInformations.interactionNumber ++;
    volatile double ignored = G4UniformRand();
}

CurrentTrackInformationAction::CurrentTrackInformationAction() 
{ 
    CurrentTrackInformation::secondaries.reserve(10);
    CurrentTrackInformation::stepsInformation.reserve(10);
}

void CurrentTrackInformationAction::PreUserTrackingAction(const G4Track* track)
{
    CurrentTrackInformation::track = track;
    CurrentTrackInformation::globalStepInformations = {1, 0};
    if (track == nullptr) return;

    if (track->GetParentID() == 0)
    {
        CurrentTrackInformation::secondaries.clear();
        CurrentTrackInformation::stepsInformation.clear();
        
        // Push primary particle track, idx 0
        CurrentTrackInformation::stepsInformation.push_back({
            0, 0
        });

        // Push Current particle informations, idx 1 (as its track ID)
        CurrentTrackInformation::stepsInformation.push_back({
            1, 1
        });
    }
    else
    {           
        for (const auto& ns : CurrentTrackInformation::secondaries)
        {
            // Make sure the vector contain enough elements
            if (static_cast<long int>(CurrentTrackInformation::stepsInformation.size()) <= ns.track->GetTrackID())
            {
                CurrentTrackInformation::stepsInformation.resize(ns.track->GetTrackID() + 1); 
            }

            CurrentTrackInformation::stepsInformation[ns.track->GetTrackID()] = ns.stepInfos;
        }
        CurrentTrackInformation::secondaries.clear();

        CurrentTrackInformation::globalStepInformations = CurrentTrackInformation::stepsInformation[track->GetTrackID()];
    }
}

void CurrentTrackInformationAction::PostUserTrackingAction(const G4Track* track)
{
    ((void) track);
    DyingParticleProcess();
    CurrentTrackInformation::track = nullptr;
}

CurrentStepInformationAction::CurrentStepInformationAction() 
{ }

void CurrentStepInformationAction::UserSteppingAction(const G4Step* step)
{
    if (step == nullptr) return;
    if (step->GetTrack() == nullptr) return;

    // This does not work because the value is updated too late for some sampled numbers
    // const G4VProcess* currentProcess = fpSteppingManager->GetfCurrentProcess();
    // const G4Track* track = step->GetTrack();

    // A bounce is defined as a changement in track momentum (either direction and/or momentum)
    if (step->GetPreStepPoint()->GetMomentum() != step->GetPostStepPoint()->GetMomentum())
    {
        CurrentTrackInformation::globalStepInformations.interactionNumber += 1;
    }

    CurrentTrackInformation::globalStepInformations.stepNumber += 1;
    
    // New particles were created
    for (std::size_t i = 0; i < step->GetNumberOfSecondariesInCurrentStep(); i++)
    {
        CurrentTrackInformation::secondaries.push_back(
            { step->GetSecondaryInCurrentStep()->at(i), CurrentTrackInformation::globalStepInformations }
        );
    }
}