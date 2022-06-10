#pragma once

#include <source_location>

#include <CLHEP/Random/MixMaxRng.h>
#include <CLHEP/Random/RandomEngine.h>

#include "CurrentTrackInformationAction.h"

class QMCRandomEngine : public CLHEP::HepRandomEngine
{
public:
    QMCRandomEngine();

    double flat(const std::source_location location = std::source_location::current()) override;
    void flatArray(const int size, double* vect, const std::source_location location = std::source_location::current()) override;

    void setSeed(long seed, int n) override;
    void setSeeds(const long* seeds, int n) override;
    void saveStatus(const char filename[] = "Config.conf") const override;
    void restoreStatus(const char filename[] = "Config.conf") override;
    void showStatus() const override;

    std::string name() const;
private:
    CLHEP::MixMaxRng tmpEngine;
};