#pragma once

#include "PointIDProvider.h"
#include "QMC_utils.h"

#include <algorithm>
#include <vector>

class GrouppedIDPointIDProvider : public PointIDProvider
{
public:
    inline GrouppedIDPointIDProvider(std::vector<std::vector<DimensionCount>> group) : 
        PointIDProvider("GrouppedIDPointIDProvider")
    { 
        unsigned int maxDim = 0;
        for (unsigned int i = 0; i < group.size(); i++)
        {
            for (unsigned int j = 0; j < group[i].size(); j++)
            {
                if (maxDim < group[i][j]) maxDim = group[i][j];
            }
        }

        dimensionToGroup = std::vector<unsigned int>(maxDim + 1, group.size());
        
        for (unsigned int i = 0; i < group.size(); i++)
        {
            for (unsigned int j = 0; j < group[i].size(); j++)
            {
                dimensionToGroup[group[i][j]] = i;
            }
        }

        lastDimensions.resize(group.size(), std::vector<DimensionCount>());
        samples.resize(group.size(), 0);
    }

    inline PointCount GetCurrentPointID(DimensionCount d)
    {
        if (d >= UNKNOWN_DIMENSION) return UNKNOWN_POINTID;
        if (d >= dimensionToGroup.size()) return UNKNOWN_POINTID;

        const unsigned int group = dimensionToGroup[d];
        if (group >= lastDimensions.size()) return UNKNOWN_POINTID;
        
        auto it = std::find(lastDimensions[group].begin(), lastDimensions[group].end(), d);
        if (it != lastDimensions[group].end())
        {
            samples[group] ++;
            lastDimensions[group].clear();
        }
        
        lastDimensions[group].push_back(d);
        return samples[group];
    }
private:
    std::vector<PointCount> samples;
    std::vector<std::vector<DimensionCount>> lastDimensions;
    std::vector<std::vector<DimensionCount>> retained_groups;

    std::vector<unsigned int> dimensionToGroup;
};