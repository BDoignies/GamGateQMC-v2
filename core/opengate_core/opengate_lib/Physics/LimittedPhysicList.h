#pragma once

#include <string>

#include "G4VPhysicsConstructor.hh"
#include "globals.hh"
#include "G4VMscModel.hh"

#include "ProcessLimits.h"

/**
 * @brief Physics List with the ability to choose at runtime processes
 */
class LimittedPhysicList : public G4VPhysicsConstructor
{
public:
    /**
     * @brief Ctor
     *
     * @param ver Verbosity level
     * @param name Name of the process (not used, maintain for compatibility)
     * @param maxSteps Maximum number of steps for a particle
     * @param processes List of processes. Key is the name, value is the way the process is limitted
     */
    explicit LimittedPhysicList(
        G4int ver=1, const G4String& name="",
        G4int maxSteps = 0,                                      // Total max steps
        std::map<std::string, ProcessLimits> processes = {}      // Processes
    );
    
    ~LimittedPhysicList() override;

    /**
     * @brief Construct particles types
     */
    void ConstructParticle() override;
    
    /**
     * @brief Construct processes
     */
    void ConstructProcess() override;
private:
    /**
     * @brief Build compton process
     * 
     * @param limits The limitation for the process
     * @param maxSteps Global maximum number of steps
     */
    void BuildCompton(const ProcessLimits& limits, int maxSteps); 
    void BuildPhotoElectric(const ProcessLimits& limits, int maxSteps);
    void BuildGammaConversion(const ProcessLimits& limits, int maxSteps);
    void BuildRayleighScattering(const ProcessLimits& limits, int maxSteps);
    
    // Electron processes
    void BuildElectronMSC(const ProcessLimits& limits, int maxSteps);
    void BuildElectronIonisation(const ProcessLimits& limits, int maxSteps);
    void BuildElectronBremsstrahlung(const ProcessLimits& limits, int maxSteps);
    void BuildElectronCoulombScattering(const ProcessLimits& limits, int maxSteps);

    void BuildPairProduction(const ProcessLimits& limits, int maxSteps);

    void ConstructElectronMscProcessWrapper(
        G4VMscModel* msc1, G4VMscModel* msc2,
        G4ParticleDefinition* particle,
        const ProcessLimits& limits, int maxSteps
    );

    G4int                  maxGlobalSteps;            //< Maximum number of steps
    std::map<std::string, ProcessLimits> processes;   //< Processes and the way they are limitted
};
