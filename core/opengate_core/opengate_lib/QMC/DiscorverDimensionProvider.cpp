#include "DiscorverDimensionProvider.h"

#include <sstream>
#include <fstream>

DiscoverDimensionProvider::DiscoverDimensionProvider(bool iB) : 
    DimensionProvider("Discorver"),
    includeBounce(iB)
{ }

void DiscoverDimensionProvider::loadFromFile(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cout << "File " << filename << " does not exist. Nothing is loaded !!!" << std::endl;
        return;
    }

    std::string tmp;
    while (std::getline(file, tmp))
    {
        std::stringstream line; line << tmp;

        DimensionID id;
        unsigned int dim;
        line >> id.className;
        line >> id.funcName;
        line >> id.line;
        line >> id.bounce;
        line >> dim;

        if (id.className == "?") id.className = "";

        currentMaxDimension = std::max(currentMaxDimension, dim);
        dimensions[id] = dim;
    }

    std::cout << "Loaded: " << currentMaxDimension << "dimensions !" << std::endl;
}

void DiscoverDimensionProvider::saveToFile(const std::string& filename)
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        std::cout << "File " << filename << " does not exist. Nothing is loaded !!!" << std::endl;
        return;
    }

    for (auto it : dimensions)
    {
        const auto cName = it.first.className.empty() ? "?" : it.first.className; 
        file << cName              << " "
             << it.first.funcName  << " "
             << it.first.line      << " "
             << it.first.bounce    << " "
             << it.second          << " "
             << "\n";
    }
}


DimensionCount DiscoverDimensionProvider::GetCurrentDimension()
{
    const DimensionID currentDimension =
    {
        CurrentTrackInformation::currentClassName,
        CurrentTrackInformation::currentFuncName,
        CurrentTrackInformation::currentLocation.line(),
        includeBounce * CurrentTrackInformation::globalStepInformations.interactionNumber
    };

    auto it = dimensions.find(currentDimension);
    if (it != dimensions.end()) return it->second;

    const auto newDimension = currentMaxDimension++;
    dimensions[currentDimension] = newDimension;
    return newDimension; 
}