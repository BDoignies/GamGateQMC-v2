#pragma once

#include "G4UserTrackingAction.hh"
#include "G4Step.hh"

#include "G4Event.hh"
#include "G4Track.hh"

struct CurrentTrackInformation
{
    static const G4Track* track;
};

class CurrentTrackInformationAction : public G4UserTrackingAction
{
public:
    inline CurrentTrackInformationAction() { }

    inline void PreUserTrackingAction(const G4Track* track) override
    {
        CurrentTrackInformation::track = track;
    }

    inline void PostUserTrackingAction(const G4Track* track) override
    {
        ((void) track);
        CurrentTrackInformation::track = nullptr;
    }
private:
};