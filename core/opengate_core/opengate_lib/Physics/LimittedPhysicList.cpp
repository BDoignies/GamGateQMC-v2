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

    if (auto it = processes.find("PhotoElectric");
        it != processes.end())
    {   
        BuildPhotoElectric(it->second, maxGlobalSteps);
    }

    if (auto it = processes.find("GammaConversion");
        it != processes.end())
    {   
        BuildGammaConversion(it->second, maxGlobalSteps);
    }

    if (auto it = processes.find("RayleighScattering");
        it != processes.end())
    {   
        BuildRayleighScattering(it->second, maxGlobalSteps);
    }

    if (auto it = processes.find("ElectronMSC");
        it != processes.end())
    {
        BuildElectronMSC(it->second, maxGlobalSteps);   
    }
    
    if (auto it = processes.find("ElectronIonisation");
        it != processes.end())
    {
        BuildElectronIonisation(it->second, maxGlobalSteps);   
    }

    if (auto it = processes.find("ElectronBremsstrahlung");
        it != processes.end())
    {
        BuildElectronBremsstrahlung(it->second, maxGlobalSteps);   
    }

    if (auto it = processes.find("ElectronCoulombScattering");
        it != processes.end())
    {
        BuildElectronCoulombScattering(it->second, maxGlobalSteps);   
    }
    
    if (auto it = processes.find("PairProduction");
        it != processes.end())
    {
        BuildPairProduction(it->second, maxGlobalSteps);   
    }
    // G4EmBuilder::ConstructCharged(hmsc, pnuc);
    // G4EmModelActivator mact(GetPhysicsName());
}

void LimittedPhysicList::BuildCompton(const ProcessLimits& limits, int maxSteps)
{
    // Copy of OPT4 physics
    G4ParticleDefinition* particle = G4Gamma::Gamma();

    G4EmParameters* param = G4EmParameters::Instance();
    G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();
    G4bool polar = param->EnablePolarisation();

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

void LimittedPhysicList::BuildPhotoElectric(const ProcessLimits& limits, int maxSteps)
{
    G4ParticleDefinition* particle = G4Gamma::Gamma();

    G4EmParameters* param = G4EmParameters::Instance();
    G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();
    G4bool polar = param->EnablePolarisation();

    G4PhotoElectricEffect* pe = new G4PhotoElectricEffect();
    G4VEmModel* peModel = new G4LivermorePhotoElectricModel();
    pe->SetEmModel(peModel);
    if(polar) {
        peModel->SetAngularDistribution(new G4PhotoElectricAngularGeneratorPolarized());
    }

    StepsCountLimittedProcess* process = new StepsCountLimittedProcess(limits, maxSteps, pe);
    ph->RegisterProcess(process, particle);
}

void LimittedPhysicList::BuildGammaConversion(const ProcessLimits& limits, int maxSteps)
{
    G4ParticleDefinition* particle = G4Gamma::Gamma();

    G4EmParameters* param = G4EmParameters::Instance();
    G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();

    G4GammaConversion* gc = new G4GammaConversion();
    G4VEmModel* conv = new G4BetheHeitler5DModel();
    gc->SetEmModel(conv);

    StepsCountLimittedProcess* process = new StepsCountLimittedProcess(limits, maxSteps, gc);
    ph->RegisterProcess(process, particle);
}

void LimittedPhysicList::BuildRayleighScattering(const ProcessLimits& limits, int maxSteps)
{
    G4ParticleDefinition* particle = G4Gamma::Gamma();

    G4EmParameters* param = G4EmParameters::Instance();
    G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();
    G4bool polar = param->EnablePolarisation();

    G4RayleighScattering* rl = new G4RayleighScattering();
    if(polar) {
        rl->SetEmModel(new G4LivermorePolarizedRayleighModel());
    }

    StepsCountLimittedProcess* process = new StepsCountLimittedProcess(limits, maxSteps, rl);
    ph->RegisterProcess(process, particle);
}

void LimittedPhysicList::BuildElectronMSC(const ProcessLimits& limits, int maxSteps)
{
    const G4EmParameters* param = G4EmParameters::Instance();
    const G4double highEnergyLimit = param->MscEnergyLimit();

    G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();

    G4ParticleDefinition* particle = G4Electron::Electron();
    
    G4eMultipleScattering* msc = new G4eMultipleScattering();
    G4GoudsmitSaundersonMscModel* msc1 = new G4GoudsmitSaundersonMscModel();
    G4WentzelVIModel* msc2 = new G4WentzelVIModel();
    msc1->SetHighEnergyLimit(highEnergyLimit);
    msc2->SetLowEnergyLimit(highEnergyLimit);
    msc->SetEmModel(msc1);
    msc->SetEmModel(msc2);

    StepsCountLimittedProcess* process = new StepsCountLimittedProcess(limits, maxSteps, msc);
    ph->RegisterProcess(process, particle);
}

void LimittedPhysicList::BuildElectronIonisation(const ProcessLimits& limits, int maxSteps)
{
    const G4EmParameters* param = G4EmParameters::Instance();
    const G4double highEnergyLimit = param->MscEnergyLimit();

    G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();

    G4ParticleDefinition* particle = G4Electron::Electron();
    
    G4eIonisation* eioni = new G4eIonisation();
    G4VEmModel* theIoniLiv = new G4LivermoreIonisationModel();
    theIoniLiv->SetHighEnergyLimit(0.1*CLHEP::MeV); 
    eioni->AddEmModel(0, theIoniLiv, new G4UniversalFluctuation() );
    
    StepsCountLimittedProcess* process = new StepsCountLimittedProcess(limits, maxSteps, eioni);
    ph->RegisterProcess(process, particle);
}

void LimittedPhysicList::BuildElectronBremsstrahlung(const ProcessLimits& limits, int maxSteps)
{
    const G4EmParameters* param = G4EmParameters::Instance();
    const G4double highEnergyLimit = param->MscEnergyLimit();

    G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();

    G4ParticleDefinition* particle = G4Electron::Electron();
    
    G4eBremsstrahlung* brem = new G4eBremsstrahlung();
    G4SeltzerBergerModel* br1 = new G4SeltzerBergerModel();
    G4eBremsstrahlungRelModel* br2 = new G4eBremsstrahlungRelModel();
    br1->SetAngularDistribution(new G4Generator2BS());
    br2->SetAngularDistribution(new G4Generator2BS());
    brem->SetEmModel(br1);
    brem->SetEmModel(br2);
    br1->SetHighEnergyLimit(CLHEP::GeV);
    
    StepsCountLimittedProcess* process = new StepsCountLimittedProcess(limits, maxSteps, brem);
    ph->RegisterProcess(process, particle);
}

void LimittedPhysicList::BuildElectronCoulombScattering(const ProcessLimits& limits, int maxSteps)
{
    const G4EmParameters* param = G4EmParameters::Instance();
    const G4double highEnergyLimit = param->MscEnergyLimit();

    G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();

    G4ParticleDefinition* particle = G4Electron::Electron();
        
    G4eCoulombScatteringModel* ssm = new G4eCoulombScatteringModel(); 
    G4CoulombScattering* ss = new G4CoulombScattering();
    ss->SetEmModel(ssm); 
    ss->SetMinKinEnergy(highEnergyLimit);
    ssm->SetLowEnergyLimit(highEnergyLimit);
    ssm->SetActivationLowEnergyLimit(highEnergyLimit);
    
    StepsCountLimittedProcess* process = new StepsCountLimittedProcess(limits, maxSteps, ss);
    ph->RegisterProcess(process, particle);
}

void LimittedPhysicList::BuildPairProduction(const ProcessLimits& limits, int maxSteps)
{
    const G4EmParameters* param = G4EmParameters::Instance();
    const G4double highEnergyLimit = param->MscEnergyLimit();

    G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();

    G4ParticleDefinition* particle = G4Electron::Electron();
        
    G4ePairProduction* ee = new G4ePairProduction();

    StepsCountLimittedProcess* process = new StepsCountLimittedProcess(limits, maxSteps, ee);
    ph->RegisterProcess(process, particle);
}