#include "RandomProfiler.h"

#include "G4Exception.hh"

CallEntry::CallEntry(const std::source_location& location, unsigned int id_)
{
    bool isSpace = false;
    bool isScope = false;

    std::size_t beginClassName = 0;
    std::size_t endClassName = 0;
    std::size_t beginFuncName = 0;
    std::size_t endFuncName = 0;

    const std::string functionName = location.function_name();
    for (std::size_t i = 0; i < functionName.size(); i++)
    {
        if (functionName[i] == ' ')
        {
            isSpace = true;
            beginClassName = i + 1;
        }
        else if (functionName[i] == ':' && isSpace)
        {
            isScope = true;
            beginFuncName = i + 2;
            endClassName = i;
            i++;
        }
        else if (functionName[i] == '(')
        {
            endFuncName = i;
            break;
        }
    }

    if (!isScope)
    {
        beginFuncName = beginClassName;
        endClassName  = beginClassName;
    }

    className = std::string(functionName, beginClassName, endClassName - beginClassName);
    funcName  = std::string(functionName, beginFuncName , endFuncName - beginFuncName);
    line = location.line();
    col = location.column();
    id = id_;
}

RandomProfiler::RandomProfiler(const std::string& outFilename)
{
    isNewPrimaryTrack = true;
    outFile.open(outFilename);

    if (!outFile.is_open())
    {
        std::string msg = "[RandomProfiler]: Can not open file " + outFilename;
        G4Exception(
            __FILE__, __PRETTY_FUNCTION__, 
            G4ExceptionSeverity::FatalErrorInArgument, msg.c_str()
        );
    }
}


void RandomProfiler::AddCall(const G4Track* track, const std::source_location& location)
{
    if (track != nullptr) // Ignore primaries
    {
        if (isNewPrimaryTrack)
        {
            primaryTracks.push_back({{
                track->GetTrackID(),
                track->GetParentID(),
                track->GetParticleDefinition()->GetParticleName(),
                { CallEntry(location, track->GetCurrentStepNumber()) }
            }});
        }
        else
        {
            auto& lastEntry = primaryTracks.back().back();
            if (lastEntry.trackID == track->GetTrackID())
            {
                lastEntry.calls.push_back(CallEntry(location, track->GetCurrentStepNumber()));
            }
            else
            {
                primaryTracks.back().push_back({
                    track->GetTrackID(),
                    track->GetParentID(),
                    track->GetParticleDefinition()->GetParticleName(),
                    { CallEntry(location, track->GetCurrentStepNumber()) }
                });
            }
        }
        
        isNewPrimaryTrack = false;
    }
    else
    {
        isNewPrimaryTrack = true;
    }
}

RandomProfiler::~RandomProfiler()
{
    bool compressed = false;

    outFile << "{"; if (compressed) outFile << "\n";
    if (compressed) outFile << "\t"; outFile << "\"PrimaryTracks\":"; if (compressed) outFile << "\n";
    if (compressed) outFile << "\t"; outFile << "["; if(compressed) outFile << "\n";

    std::size_t i = 0;
    for (const auto& pTrack : primaryTracks)
    {   
        if (compressed) outFile << "\t\t"; outFile << "["; if (compressed) outFile << "\n";
        std::size_t j = 0;
        for (const auto& track : pTrack)
        {
                if (compressed) outFile << "\t\t\t"; outFile << "{"; if (compressed) outFile << "\n";

                    if (compressed) outFile << "\t\t\t\t"; outFile << "\"TrackID\": "  << track.trackID << ","; if (compressed) outFile << "\n";
                    if (compressed) outFile << "\t\t\t\t"; outFile << "\"ParentID\": " << track.parentID << ",";  if (compressed) outFile << "\n";
                    if (compressed) outFile << "\t\t\t\t"; outFile << "\"Paricle\": \""  << track.particleName << "\",";  if (compressed) outFile << "\n";
                    if (compressed) outFile << "\t\t\t\t"; outFile << "\"Calls\":"; if (compressed) outFile << "\n";
                    if (compressed) outFile << "\t\t\t\t"; outFile << "["; if (compressed) outFile << "\n";

                        std::size_t k = 0;
                        for (const auto& call : track.calls)
                        {
                            if (compressed) outFile << "\t\t\t\t\t"; outFile << "{"; if (compressed) outFile << "\n";

                                if (compressed) outFile << "\t\t\t\t\t\t"; outFile << "\"ClassName\": \"" << call.className << "\","; if (compressed) outFile << "\n";
                                if (compressed) outFile << "\t\t\t\t\t\t"; outFile << "\"FuncName\": \""  << call.funcName  << "\","; if (compressed) outFile << "\n";
                                if (compressed) outFile << "\t\t\t\t\t\t"; outFile << "\"Line\": "        << call.line      <<   ","; if (compressed) outFile << "\n";
                                if (compressed) outFile << "\t\t\t\t\t\t"; outFile << "\"Column\": "      << call.col       <<   ","; if (compressed) outFile << "\n";
                                if (compressed) outFile << "\t\t\t\t\t\t"; outFile << "\"StepNum\": "     << call.id        <<    ""; if (compressed) outFile << "\n";
                                
                            if (compressed) outFile << "\t\t\t\t\t"; outFile << "}";
                            if (++k != track.calls.size()) outFile << ",";
                            if (compressed) outFile << "\n";
                        }

                    if (compressed) outFile << "\t\t\t\t"; outFile << "]"; if (compressed) outFile << "\n";
                
                if (compressed) outFile << "\t\t\t"; outFile << "}";
                if (++j != pTrack.size()) outFile << ",";
                if (compressed) outFile << "\n";
        }

        if (compressed) outFile << "\t\t"; outFile << "]";
        if (++i != primaryTracks.size()) outFile << ",";
        if (compressed) outFile << "\n";
    }

    if (compressed) outFile << "\t"; outFile << "]"; if (compressed) outFile << "\n";
    outFile << "}";
}