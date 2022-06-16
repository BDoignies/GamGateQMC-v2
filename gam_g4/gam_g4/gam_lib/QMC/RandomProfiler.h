#pragma once

#include <source_location>
#include <fstream>
#include <vector>
#include <string>

#include "CurrentTrackInformationAction.h"
#include "G4Track.hh"
#include "G4VProcess.hh"

struct CallEntry
{
    CallEntry(const std::source_location& location);
    
    std::string  className;
    std::string  funcName;
    unsigned int     line;
    unsigned int      col;
};

struct StepEntry
{
    StepEntry(const G4Step* step);
    
    double energy;
    unsigned int globalStepNumber;
    unsigned int localStepNumber;
    unsigned int interactionNumber;
    
    // std::string modelName;
    // std::string creatorProcessName;
    // std::string preStepProcessName;
    // std::string postStepProcessName;
};

struct TrackEntry
{
public:
    int trackID;
    int parentID;

    std::string particleName;
    std::vector<CallEntry> calls;
    std::vector<StepEntry> steps;
};

class RandomProfiler
{
public:
    RandomProfiler(const std::string& outFilename, bool readable = false);

    void AddCall(const G4Track* track, const std::source_location& location);

    ~RandomProfiler();
private:
    void WriteProfiler();
private:
    bool readable;
    std::ofstream outFile;

    bool isNewPrimaryTrack;
    std::vector<std::vector<TrackEntry>> primaryTracks;
};