#pragma once

#include <source_location>

#include <CLHEP/Random/MixMaxRng.h>
#include <CLHEP/Random/RandomEngine.h>

#include "CurrentTrackInformationAction.h"

#include "RandomStatistics.h"
#include "RandomProfiler.h"

struct QMCRandomEngineParameters
{
    std::string profilerOutput = "";
    bool profilerReadable = false;

    std::string statsOutput = "";

    RandomProfiler* profiler() const;
    RandomStatistics* statistics() const;
};

class QMCRandomEngine : public CLHEP::HepRandomEngine
{
public:
    QMCRandomEngine(const QMCRandomEngineParameters& params);

    double flat(const std::source_location location = std::source_location::current()) override;
    void flatArray(const int size, double* vect, const std::source_location location = std::source_location::current()) override;

    void setSeed(long seed, int n) override;
    void setSeeds(const long* seeds, int n) override;
    void saveStatus(const char filename[] = "Config.conf") const override;
    void restoreStatus(const char filename[] = "Config.conf") override;
    void showStatus() const override;

    std::string name() const;

    ~QMCRandomEngine();
private:
    RandomProfiler* profiler;
    RandomStatistics* statistics;

    CLHEP::MixMaxRng tmpEngine;
};