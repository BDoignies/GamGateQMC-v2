#include "RandomProfiler.h"

#include "G4Exception.hh"

#include <cmath>
double nonan(double current)
{
    if (std::isnan(current) || current < 1e-10)
        return 0.0;
    return current;
}

#include "QMC_utils.h"

CallEntry::CallEntry(unsigned int c)
{
    const std::source_location& location = CurrentTrackInformation::currentLocation;

    className = CurrentTrackInformation::currentClassName;
    funcName = CurrentTrackInformation::currentFuncName;
    line = location.line();
    col = location.column();
    count = c;
}

StepEntry::StepEntry(const G4Step* step)
{
    // ((void) step);
    if (step != nullptr)
    {
        globalStepNumber  = CurrentTrackInformation::globalStepInformations.stepNumber;
        interactionNumber = CurrentTrackInformation::globalStepInformations.interactionNumber;
        localStepNumber   = step->GetTrack()->GetCurrentStepNumber();
        energy            = step->GetTrack()->GetTotalEnergy();
        
        // if (step->GetTrack()->GetCreatorProcess() != nullptr)
        //     creatorProcessName = step->GetTrack()->GetCreatorProcess()->GetProcessName();
        //
        // modelName = step->GetTrack()->GetCreatorModelName();
        //
        // if (step->GetPreStepPoint()->GetProcessDefinedStep() != nullptr)
        //     preStepProcessName = step->GetPreStepPoint()->GetProcessDefinedStep()->GetProcessName();
        //
        // if (step->GetPostStepPoint()->GetProcessDefinedStep() != nullptr)
        //     postStepProcessName = step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
    }
}

RandomProfiler::RandomProfiler(const std::string& outFilename, bool r)
{
    readable = r;
    isNewPrimaryTrack = true;

    outFile.open(outFilename);
    if (!outFile.is_open())
    {
        std::string msg = "[RandomProfiler]: Can not open file " + outFilename + ", the profiler will not output its results";
        G4Exception(
            __FILE__, __PRETTY_FUNCTION__, 
            G4ExceptionSeverity::FatalErrorInArgument, msg.c_str()
        );
    }
}



void RandomProfiler::AddCall(unsigned int count)
{
    const G4Track* track = CurrentTrackInformation::track;
    if (track != nullptr) 
    {
        // Case where the primary particles do not require sampling
        // Use isNewPrimaryTrack + globalStepNumber
        //     - A new track is when the global Step Number went above 1 and is back to 1
        //     - isNewPrimaryTrack required because multiple sampling can be done at step 1
        if ((isNewPrimaryTrack && CurrentTrackInformation::globalStepInformations.stepNumber == 1) || primaryTracks.size() == 0)
        {
            isNewPrimaryTrack = false;
            primaryTracks.push_back({{
                track->GetTrackID(),
                track->GetParentID(),
                track->GetParticleDefinition()->GetParticleName(),
                { CallEntry(count) },
                { StepEntry(track->GetStep()) }
            }});
        }
        else
        {   
            isNewPrimaryTrack = (CurrentTrackInformation::globalStepInformations.stepNumber != 1);
            
            auto& lastEntry = primaryTracks.back().back();
            if (lastEntry.trackID == track->GetTrackID())
            {
                lastEntry.calls.push_back(CallEntry(count));
                lastEntry.steps.push_back(StepEntry(track->GetStep()));
            }
            else
            {
                primaryTracks.back().push_back({
                    track->GetTrackID(),
                    track->GetParentID(),
                    track->GetParticleDefinition()->GetParticleName(),
                    { CallEntry(count) },
                    { StepEntry(track->GetStep()) }
                });
            }
        }
    }
    else
    {
        if (isNewPrimaryTrack)
        {
            primaryTracks.push_back({{
                0,
                -1,
                "",
                { CallEntry(count) },
                { StepEntry(nullptr) }
            }});
            isNewPrimaryTrack = false;
        }
        else
        {
            auto& lastEntry = primaryTracks.back().back();
            lastEntry.calls.push_back(CallEntry(count));
            lastEntry.steps.push_back(StepEntry(nullptr));
        }
    }
}

void RandomProfiler::WriteProfiler()
{
    outFile << "{"; 
    if (readable) outFile << "\n";
    if (readable) outFile << "\t"; 
        outFile << "\"PrimaryTracks\":"; 
    if (readable) outFile << "\n";
    
    if (readable) outFile << "\t"; 
        outFile << "["; 
    if(readable) outFile << "\n";

    std::size_t i = 0;
    for (const auto& pTrack : primaryTracks)
    {   
        if (readable) outFile << "\t\t"; 
        outFile << "["; 
        if (readable) outFile << "\n";
        
        std::size_t j = 0;
        for (const auto& track : pTrack)
        {
                if (readable) outFile << "\t\t\t";
                outFile << "{"; 
                if (readable) outFile << "\n";

                    if (readable) outFile << "\t\t\t\t"; 
                    outFile << "\"TrackID\": "  << track.trackID << ","; 
                    if (readable) outFile << "\n";
                    
                    if (readable) outFile << "\t\t\t\t"; 
                    outFile << "\"ParentID\": " << track.parentID << ",";  
                    if (readable) outFile << "\n";
                    
                    if (readable) outFile << "\t\t\t\t"; 
                    outFile << "\"Particle\": \""  << track.particleName << "\",";  
                    if (readable) outFile << "\n";

                    if (readable) outFile << "\t\t\t\t"; 
                    outFile << "\"Calls\":"; 
                    if (readable) outFile << "\n";
                    
                    if (readable) outFile << "\t\t\t\t"; 
                    outFile << "["; 
                    if (readable) outFile << "\n";

                        std::size_t k = 0;
                        for (const auto& call : track.calls)
                        {
                            if (readable) outFile << "\t\t\t\t\t"; 
                            outFile << "{"; 
                            if (readable) outFile << "\n";

                                if (readable) outFile << "\t\t\t\t\t\t"; 
                                outFile << "\"ClassName\": \"" << call.className << "\","; 
                                if (readable) outFile << "\n";

                                if (readable) outFile << "\t\t\t\t\t\t"; 
                                outFile << "\"FuncName\": \""  << call.funcName  << "\","; 
                                if (readable) outFile << "\n";

                                if (readable) outFile << "\t\t\t\t\t\t"; 
                                outFile << "\"Line\": "        << call.line      <<   ","; 
                                if (readable) outFile << "\n";

                                if (readable) outFile << "\t\t\t\t\t\t"; 
                                outFile << "\"Column\": "      << call.col       <<   ","; 
                                if (readable) outFile << "\n";
                                
                                if (readable) outFile << "\t\t\t\t\t\t"; 
                                outFile << "\"Count\": "      << call.count      <<   ""; 
                                if (readable) outFile << "\n";

                            if (readable) outFile << "\t\t\t\t\t"; 
                                outFile << "}";
                            
                            if (++k != track.calls.size()) outFile << ",";
                            if (readable) outFile << "\n";
                        }

                    if (readable) outFile << "\t\t\t\t"; 
                    outFile << "],"; 
                    if (readable) outFile << "\n";

                    if (readable) outFile << "\t\t\t\t"; 
                    outFile << "\"Steps\":"; 
                    if (readable) outFile << "\n";
                    
                    if (readable) outFile << "\t\t\t\t"; 
                    outFile << "["; 
                    if (readable) outFile << "\n";

                        k = 0;
                        for (const auto& step : track.steps)
                        {
                            if (readable) outFile << "\t\t\t\t\t"; 
                            outFile << "{"; 
                            if (readable) outFile << "\n";

                            if (readable) outFile << "\t\t\t\t\t\t"; 
                            outFile << "\"GlobalStepNum\": " << step.globalStepNumber << ",";
                            if (readable) outFile << "\n";

                            if (readable) outFile << "\t\t\t\t\t\t"; 
                            outFile << "\"LocalStepNum\": " << step.localStepNumber << ",";
                            if (readable) outFile << "\n";
                            
                            if (readable) outFile << "\t\t\t\t\t\t"; 
                            outFile << "\"InteractionNumber\": " << step.interactionNumber << ",";
                            if (readable) outFile << "\n";

                            if (readable) outFile << "\t\t\t\t\t\t"; 
                            outFile << "\"TotalEnergy\": " << nonan(step.energy);
                            if (readable) outFile << "\n";

                            
                            // if (readable) outFile << "\t\t\t\t\t\t"; 
                            // outFile << "\"CreatorProcess\": \"" << step.creatorProcessName << "\",";
                            // if (readable) outFile << "\n";
                            // 
                            // if (readable) outFile << "\t\t\t\t\t\t"; 
                            // outFile << "\"Model\": \"" << step.modelName << "\",";
                            // if (readable) outFile << "\n";
                            // 
                            // if (readable) outFile << "\t\t\t\t\t\t"; 
                            // outFile << "\"PreProcess\": \"" << step.preStepProcessName << "\",";
                            // if (readable) outFile << "\n";
                            // 
                            // if (readable) outFile << "\t\t\t\t\t\t"; 
                            // outFile << "\"PostProcess\": \"" << step.postStepProcessName << "\",";
                            // if (readable) outFile << "\n";
                            

                            if (readable) outFile << "\t\t\t\t\t"; 
                            outFile << "}";
                            
                            if (++k != track.steps.size()) outFile << ",";
                            if (readable) outFile << "\n";
                        }

                    if (readable) outFile << "\t\t\t\t"; 
                    outFile << "]"; 
                    if (readable) outFile << "\n";
                
                if (readable) outFile << "\t\t\t"; 
                outFile << "}";
                
                if (++j != pTrack.size()) outFile << ",";
                if (readable) outFile << "\n";
        }

        if (readable) outFile << "\t\t"; 
        outFile << "]";
        
        if (++i != primaryTracks.size()) outFile << ",";
        if (readable) outFile << "\n";
    }

    if (readable) outFile << "\t"; 
    outFile << "]"; 
    if (readable) outFile << "\n";
    outFile << "}";
}

RandomProfiler::~RandomProfiler()
{
    WriteProfiler();
}