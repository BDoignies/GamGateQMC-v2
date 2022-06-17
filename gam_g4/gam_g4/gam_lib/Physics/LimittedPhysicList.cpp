#include "G4SystemOfUnits.hh"
#include "G4ParticleDefinition.hh"
#include "G4LossTableManager.hh"
#include "G4EmParameters.hh"
#include "G4EmBuilder.hh"

#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4PhotoElectricEffect.hh"
#include "G4RayleighScattering.hh"
#include "G4PEEffectFluoModel.hh"
#include "G4KleinNishinaModel.hh"
#include "G4BetheHeitler5DModel.hh"
#include "G4LivermorePhotoElectricModel.hh"
#include "G4LivermorePolarizedRayleighModel.hh"
#include "G4PhotoElectricAngularGeneratorPolarized.hh"
#include "G4LowEPComptonModel.hh"
#include "G4LowEPPolarizedComptonModel.hh"

#include "G4eMultipleScattering.hh"
#include "G4hMultipleScattering.hh"
#include "G4MscStepLimitType.hh"
#include "G4UrbanMscModel.hh"
#include "G4GoudsmitSaundersonMscModel.hh"
#include "G4DummyModel.hh"
#include "G4WentzelVIModel.hh"
#include "G4CoulombScattering.hh"
#include "G4eCoulombScatteringModel.hh"

#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4Generator2BS.hh"
#include "G4Generator2BN.hh"
#include "G4SeltzerBergerModel.hh"
#include "G4ePairProduction.hh"
#include "G4LivermoreIonisationModel.hh"
#include "G4PenelopeIonisationModel.hh"
#include "G4UniversalFluctuation.hh"

#include "G4eplusAnnihilation.hh"

#include "G4hIonisation.hh"
#include "G4ionIonisation.hh"
#include "G4IonParametrisedLossModel.hh"
#include "G4NuclearStopping.hh"

#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4GenericIon.hh"

#include "G4PhysicsListHelper.hh"
#include "G4BuilderType.hh"
#include "G4EmModelActivator.hh"
#include "G4GammaGeneralProcess.hh"

// factory
#include "G4PhysicsConstructorFactory.hh"

#include "LimittedPhysicList.h"
#include "StepsCountLimittedProcess.h"

// Register the physics list as available
G4_DECLARE_PHYSCONSTR_FACTORY(LimittedPhysicList);

LimittedPhysicList::LimittedPhysicList(
    G4int ver, const G4String& name, 
    G4int mgs,
    std::map<std::string, ProcessLimits> pcs  
)
  : G4VPhysicsConstructor("LimittedPhysicList"),
    maxGlobalSteps(mgs),
    processes(pcs)
{
    ((void)name);
    // Copy of Opt4 parameters
    SetVerboseLevel(ver);
    G4EmParameters* param = G4EmParameters::Instance();
    param->SetDefaults();
    param->SetVerbose(ver);
    param->SetMinEnergy(100*CLHEP::eV);
    param->SetLowestElectronEnergy(100*CLHEP::eV);
    param->SetNumberOfBinsPerDecade(20);
    param->ActivateAngularGeneratorForIonisation(true);
    param->SetStepFunction(0.2, 10*CLHEP::um);
    param->SetStepFunctionMuHad(0.1, 50*CLHEP::um);
    param->SetStepFunctionLightIons(0.1, 20*CLHEP::um);
    param->SetStepFunctionIons(0.1, 1*CLHEP::um);
    param->SetUseMottCorrection(true); // use Mott-correction for e-/e+ msc gs
    param->SetMscStepLimitType(fUseSafetyPlus); // for e-/e+ msc gs
    param->SetMscSkin(3);              // error-free stepping for e-/e+ msc gs
    param->SetMscRangeFactor(0.08);    // error-free stepping for e-/e+ msc gs
    param->SetMuHadLateralDisplacement(true);
    param->SetFluo(true);
    param->SetUseICRU90Data(true);
    param->SetMaxNIELEnergy(1*CLHEP::MeV);
    SetPhysicsType(bElectromagnetic);
}

LimittedPhysicList::~LimittedPhysicList()
{}

void LimittedPhysicList::ConstructParticle()
{
    G4EmBuilder::ConstructMinimalEmSet();
}

void LimittedPhysicList::ConstructProcess()
{
    
    // G4EmBuilder::PrepareEMPhysics();
    
    // Gamma processes :
    
    // C++17 only ! 
    if (auto it = processes.find("Compton");
        it != processes.end())
    {   
        BuildCompton(it->second, maxGlobalSteps);
    }
    // G4EmBuilder::ConstructCharged(hmsc, pnuc);
    // G4EmModelActivator mact(GetPhysicsName());
}

void LimittedPhysicList::BuildCompton(const ProcessLimits& limits, int maxSteps)
{
    // Copy of OPT4 physics
    G4EmParameters* param = G4EmParameters::Instance();
    G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();
    G4bool polar = param->EnablePolarisation();

    G4ParticleDefinition* particle = G4Gamma::Gamma();

    G4ComptonScattering* cs = new G4ComptonScattering;
    cs->SetEmModel(new G4KleinNishinaModel());
    G4VEmModel* cModel = nullptr;
    if (polar) cModel = new G4LowEPPolarizedComptonModel(); 
    else cModel = new G4LowEPComptonModel();
    
    cModel->SetHighEnergyLimit(20*CLHEP::MeV);
    cs->AddEmModel(0, cModel);

    // Register processes
    // FIXME : Append only StepsCountLimiteedProcesses but Gamma processes
    // requires a G4EmProcess which is not compatible with G4WrapperProcess ...
    // Commented : it is buggy for the above reason, and deactivated by default...
    // if(param->GeneralProcessActive()) 
    // {
    //     ContextualizedGammaGeneralProcess* sp = new ContextualizedGammaGeneralProcess();
    //     sp->AddEmProcess(cs);
    //     G4LossTableManager::Instance()->SetGammaGeneralProcess(sp);
    //     
    //     StepsCountLimittedProcess* process = new StepsCountLimittedProcess(limits, maxSteps, sp);
    //     ph->RegisterProcess(process, particle);
    // } 
    // else 
    {
        StepsCountLimittedProcess* process = new StepsCountLimittedProcess(limits, maxSteps, cs);
        ph->RegisterProcess(process, particle);
    }
}
