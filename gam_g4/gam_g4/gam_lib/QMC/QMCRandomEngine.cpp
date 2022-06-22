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

    sampler = params.sampler;
    dimProvider = params.dimProvider;
    idProvider = params.idProvider;
}

double QMCRandomEngine::flat(const std::source_location location)
{
    CurrentTrackInformation::SetLocation(location);
    
    if (profiler) profiler->AddCall(1);
    if (statistics) statistics->AddCall(1);

    DimensionCount d = dimProvider->GetCurrentDimension();
    PointCount i = idProvider->GetCurrentPointID(d);
    SampleType s = sampler->Sample(i, d);
    return s;
}

void QMCRandomEngine::flatArray(const int size, double* vect, const std::source_location location)
{
    CurrentTrackInformation::SetLocation(location);

    if (profiler) profiler->AddCall(size);
    if (statistics) statistics->AddCall(size);
 
    // Do not call flat() for stat computation   
    for (int k = 0; k < size; k++)
    {
        DimensionCount d = dimProvider->GetCurrentDimension();
        PointCount i = idProvider->GetCurrentPointID(d);
        SampleType s = sampler->Sample(i, d);

        vect[k] = s;
    }
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