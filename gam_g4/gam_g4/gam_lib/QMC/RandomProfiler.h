#pragma once

#include <source_location>
#include <fstream>
#include <vector>
#include <string>

#include "G4Track.hh"

struct CallEntry
{
    CallEntry(const std::source_location& location,  unsigned int id_);
    
    std::string  className;
    std::string  funcName;
    unsigned int     line;
    unsigned int      col;
    unsigned int       id;
};

struct TrackEntry
{
public:
    int trackID;
    int parentID;
    std::string particleName;
    std::vector<CallEntry> calls;
};

class RandomProfiler
{
public:
    RandomProfiler(const std::string& outFilename);

    void AddCall(const G4Track* track, const std::source_location& location);

    ~RandomProfiler();
private:
    std::ofstream outFile;

    bool isNewPrimaryTrack;
    std::vector<std::vector<TrackEntry>> primaryTracks;
};