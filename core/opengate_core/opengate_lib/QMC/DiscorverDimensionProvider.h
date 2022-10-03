#pragma once

#include <unordered_map>
#include "DimensionProvider.h" 
#include "CurrentTrackInformationAction.h"

struct DimensionID
{
    std::string className = "";
    std::string funcName  = "";
    unsigned int line      = 0;
    unsigned int bounce = 0;

    bool operator==(const DimensionID& other) const
    {
        return className == other.className && 
                funcName  == other.funcName  &&
                line      == other.line      &&
                bounce    == other.bounce;
    }
};

struct DimensionIDHash
{
    inline std::size_t operator()(const DimensionID& key) const
    {
        std::size_t rslt = 
                std::hash<std::string>()(key.className);
        rslt ^= std::hash<std::string>()(key.funcName);
        rslt ^= std::hash<unsigned int>()(key.line);
        rslt ^= std::hash<unsigned int>()(key.bounce);
        return rslt;
    }
};

class DiscoverDimensionProvider : public DimensionProvider
{
public:
    DiscoverDimensionProvider(bool includeBounce = true);

    void loadFromFile(const std::string& filename);

    DimensionCount GetCurrentDimension() override;

    void saveToFile(const std::string& filename);

    inline virtual ~DiscoverDimensionProvider() { }
protected:
    bool includeBounce;
    unsigned int currentMaxDimension;
    std::unordered_map<DimensionID, unsigned int, DimensionIDHash> dimensions;
};