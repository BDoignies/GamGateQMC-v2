#include "StepsCountLimittedProcess.h"
#include <fstream>

#include "G4VEmProcess.hh"

// #define DEBUG(X) X
#define DEBUG(X)

StepsCountLimittedProcess::StepsCountLimittedProcess(const ProcessLimits& l, G4int maxG, G4VProcess* process) : 
    G4WrapperProcess("StepsCountLimittedProcess[" + process->GetProcessName() + "]"),
    maxGlobalSteps(maxG),
    limits(l)
{
    RegisterProcess(process);
    SetProcessType(process->GetProcessType());
    SetProcessSubType(process->GetProcessSubType());
}

G4VParticleChange* StepsCountLimittedProcess::PostStepDoIt(const G4Track& track, const G4Step& stepData)
{
    DEBUG(std::cout << "PostStepDoIt: " << GetProcessName() << std::endl);
    // Sample of secondary particles context is left to underlying implementation
    return KillTrack(stepData) ? &change : pRegProcess->PostStepDoIt(track, stepData);
}

G4VParticleChange* StepsCountLimittedProcess::AlongStepDoIt(const G4Track& track, const G4Step& stepData)
{
    DEBUG(std::cout << "AlongStepDoIt: " << GetProcessName() << std::endl);
    // Sample of secondary particles context is left to underlying implementation
    return KillTrack(stepData) ? &change : pRegProcess->AlongStepDoIt(track, stepData);
}

G4VParticleChange* StepsCountLimittedProcess::AtRestDoIt(const G4Track& track, const G4Step& stepData)
{
    DEBUG(std::cout << "AtRestDoIt: " << GetProcessName() << std::endl);

    // Sample of secondary particles context is left to underlying implementation
    return KillTrack(stepData) ? &change : pRegProcess->AtRestDoIt(track, stepData);
}

G4double StepsCountLimittedProcess::AlongStepGetPhysicalInteractionLength (const G4Track &track, G4double previousStepSize, G4double currentMinimumStep, G4double &proposedSafety, G4GPILSelection *selection)
{
    DEBUG(std::cout << "AlongStepGetPhysicalInteractionLength: " << GetProcessName() << std::endl);

    auto it = processSteps.find(track.GetTrackID());
    if (it != processSteps.end() && it->second > limits.maxInteractions) 
    { 
        *selection = NotCandidateForSelection; 
        return limits.behaviorOnLimit(ALONG); 
    }
    
    G4double s = pRegProcess->AlongStepGetPhysicalInteractionLength(track, previousStepSize, currentMinimumStep, proposedSafety, selection);    
    return s;
}

G4double StepsCountLimittedProcess::AtRestGetPhysicalInteractionLength (const G4Track &track, G4ForceCondition *condition)
{
    DEBUG(std::cout << "AtRestGetPhysicalInteractionLength: " << GetProcessName() << std::endl);

    auto it = processSteps.find(track.GetTrackID());
    if (it != processSteps.end() && it->second > limits.maxInteractions) 
    { 
        *condition = InActivated; 
        return limits.behaviorOnLimit(ALONG);
    }
    
    G4double s = pRegProcess->AtRestGetPhysicalInteractionLength(track, condition);
    return s;
}

G4double StepsCountLimittedProcess::PostStepGetPhysicalInteractionLength (const G4Track &track, G4double previousStepSize, G4ForceCondition *condition)
{
    DEBUG(std::cout << "PostStepGetPhysicalInteractionLength: " << GetProcessName() << std::endl);

    auto it = processSteps.find(track.GetTrackID());
    if (it != processSteps.end() && it->second > limits.maxInteractions) 
    { 
        *condition = InActivated; 
        return limits.behaviorOnLimit(ALONG); 
    }
    G4double s = pRegProcess->PostStepGetPhysicalInteractionLength(track, previousStepSize, condition);
    return s;
}

void StepsCountLimittedProcess::StartTracking(G4Track* track)
{
    DEBUG(std::cout << "StartTracking: " << GetProcessName() << std::endl);

    // Reset the count when a new track is created. Track do not
    // embed the event number, so no way to differentiate between 
    // two track with same Id.
    processSteps[track->GetTrackID()] = 0;
    pRegProcess->StartTracking(track);
}

bool StepsCountLimittedProcess::KillTrack(const G4Step& step)
{
    DEBUG(std::cout << "KillTrack: " << GetProcessName() << std::endl);
    
    G4Track& track = *step.GetTrack();
    // Kill only on global number of step 
    // Otherwise, reaching limit of a process would kill the 
    // track and leave no chance for other processes to occur 
    if ((track.GetCurrentStepNumber() >= maxGlobalSteps))
    {
        // Initializing G4ParticleChange with track = do not change anything
        track.SetTrackStatus(fStopAndKill);
        change.Initialize(track);
        return true;
    }
    
    if ((processSteps[track.GetTrackID()]++ >= limits.maxInteractions))
    {
        // Initializing G4ParticleChange with track = do not change anything
        change.Initialize(track);
        return true;
    }

    return false;
}
