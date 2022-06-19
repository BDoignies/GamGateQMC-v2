#include "QMCRandomEngine.h"

RandomProfiler* QMCRandomEngineParameters::profiler() const
{
    if (profilerOutput.size() != 0) return new RandomProfiler(profilerOutput, profilerReadable);
    return nullptr;
}

RandomStatistics* QMCRandomEngineParameters::statistics() const
{
    if (statsOutput.size() != 0) return new RandomStatistics(statsOutput);
    return nullptr;
}

QMCRandomEngine::QMCRandomEngine(const QMCRandomEngineParameters& params)
{ 
    profiler = params.profiler();
    statistics = params.statistics();
}

double QMCRandomEngine::flat(const std::source_location location)
{
    if (profiler) profiler->AddCall(CurrentTrackInformation::track, location);
    if (statistics) statistics->AddCall(location, 1);

    return tmpEngine.flat();
}

void QMCRandomEngine::flatArray(const int size, double* vect, const std::source_location location)
{
    if (profiler) profiler->AddCall(CurrentTrackInformation::track, location);
    if (statistics) statistics->AddCall(location, size);
    
    return tmpEngine.flatArray(size, vect);
}

void QMCRandomEngine::setSeed(long seed, int n)
{
    tmpEngine.setSeed(seed, n);
}

void QMCRandomEngine::setSeeds(const long* seeds, int n)
{
    tmpEngine.setSeeds(seeds, n);
}

void QMCRandomEngine::saveStatus(const char filename[]) const
{
    ((void) filename);
}

void QMCRandomEngine::restoreStatus(const char filename[])
{
    ((void) filename);
}

void QMCRandomEngine::showStatus() const
{

}

std::string QMCRandomEngine::name() const
{
    return "QMCRandomEngine";
}

QMCRandomEngine::~QMCRandomEngine()
{
    if (profiler) delete profiler;
    if (statistics) delete statistics;
}