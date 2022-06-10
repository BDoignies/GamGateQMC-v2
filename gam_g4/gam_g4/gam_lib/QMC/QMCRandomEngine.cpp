#include "QMCRandomEngine.h"

QMCRandomEngine::QMCRandomEngine()
{ }

double QMCRandomEngine::flat(const std::source_location location)
{
    std::cout << CurrentTrackInformation::track  << std::endl;
    std::cout << location.file_name() << "(" << location.line() << "," << location.column() << ") " << location.function_name() << std::endl;
    return tmpEngine.flat();
}

void QMCRandomEngine::flatArray(const int size, double* vect, const std::source_location location)
{
    std::cout << CurrentTrackInformation::track << std::endl;
    std::cout << location.file_name() << "(" << location.line() << "," << location.column() << ") " << location.function_name() << std::endl;
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
}

void QMCRandomEngine::restoreStatus(const char filename[])
{

}

void QMCRandomEngine::showStatus() const
{

}

std::string QMCRandomEngine::name() const
{
    return "QMCRandomEngine";
}