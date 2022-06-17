#pragma once

#include <fstream>
#include "ProcessLimits.h"
#include "G4WrapperProcess.hh"

/**
 * @brief Wrapper process for step limitation
 * 
 * This process wraps another process and keep track
 * of the number of times this process was choosen. 
 * 
 * At a user defined maximum, it performs an operation
 * depending on the requested behavior
 */
class StepsCountLimittedProcess : public G4WrapperProcess
{
public:
    /**
     * @brief Ctor
     * 
     * @param limits Information about the limits
     * @param maxG Maximum global number of step
     * @param process Process to wrap
     */
    StepsCountLimittedProcess(const ProcessLimits& limits, G4int maxG, G4VProcess* process);

    // Reimplementation of G4Process main functions
    virtual G4VParticleChange* PostStepDoIt(const G4Track& track, const G4Step& stepData) override; 
    virtual G4VParticleChange* AlongStepDoIt(const G4Track& track, const G4Step& stepData) override;
    virtual G4VParticleChange* AtRestDoIt(const G4Track& track, const G4Step& stepData) override;
    
    virtual G4double AlongStepGetPhysicalInteractionLength(
        const G4Track &track, G4double previousStepSize, G4double currentMinimumStep, 
        G4double &proposedSafety, G4GPILSelection *selection
    ) override;
    virtual G4double AtRestGetPhysicalInteractionLength   (const G4Track &track, G4ForceCondition *condition) override;
    virtual G4double PostStepGetPhysicalInteractionLength (const G4Track &track, G4double previousStepSize, G4ForceCondition *condition)override;

    virtual void StartTracking(G4Track* track) override;

    /**
     * @brief Check wether the track should be killed or not
     * 
     * @param step The step to check for killing or not
     * 
     * @return True if the track should be killed, false otherwise
     */
    bool KillTrack(const G4Step& step);

    bool hasReachedMaxSteps(const G4Track& track) const;
private:
    std::map<int, std::size_t> processSteps; //< Count number of interactions per particles
    
    // TODO : Best place ?
    G4int maxGlobalSteps; //< Maximum number of steps
    ProcessLimits limits; //< Limittation applied to this process 
    G4VParticleChange change; //< Store "no changes" !
};
