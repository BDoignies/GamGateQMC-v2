#pragma once

#include <list>

#include "G4UserTrackingAction.hh"
#include "G4UserSteppingAction.hh"
#include "G4Step.hh"

#include "G4Event.hh"
#include "G4Track.hh"

// Structure to store global informations
struct CurrentTrackInformation
{
    // Information about steps
    struct StepInformation
    {
        // Can't be const because of copy needed
        unsigned int stepNumber;
        unsigned int interactionNumber;
    };

    // Informations about secondaries 
    struct NewSecondaryInformation
    {
        const G4Track* track;
        const StepInformation stepInfos;   
    };

    // Current track
    static const G4Track* track;

    // Global steps for current track
    static StepInformation globalStepInformations;

public:
    static std::vector<CurrentTrackInformation::NewSecondaryInformation>    secondaries;
    
    // Store steps information as vectors whose index is the track ID
    // and values are the steps required.
    // It is not posisble to keep pointers through track as they are 
    // reused...
    static std::vector<StepInformation> stepsInformation;
};

class CurrentTrackInformationAction : public G4UserTrackingAction
{
public:
    CurrentTrackInformationAction();

    void PreUserTrackingAction(const G4Track* track) override;
    void PostUserTrackingAction(const G4Track* track) override;
private:
};

class CurrentStepInformationAction : public G4UserSteppingAction
{
public:
    CurrentStepInformationAction();
    void UserSteppingAction(const G4Step* step) override;
private:
};