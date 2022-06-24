#pragma once

#include <vector>
#include <fstream>
#include <random>

#include "Sampler.h"
#include "SamplerSobol.h"
#include "G4Exception.hh"

class SplittedSobol : public Sampler
{
public:
	inline SplittedSobol(
		const std::string& dirName, 
		std::vector<std::vector<DimensionCount>> group, 
		PointCount L_
	) : Sampler("SplittedSobol")
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
			sobols.push_back(new SobolSampler(dirName, group[i].size(), 0, L_));
			minDim.push_back(group[i][0]);
		    for (unsigned int j = 0; j < group[i].size(); j++)
            {
                dimensionToGroup[group[i][j]] = i;
            }
        }
	}

	inline SampleType Sample(PointCount i, DimensionCount d) override
	{
		if (d >= UNKNOWN_DIMENSION) return Whitenoise();

		const unsigned int group = dimensionToGroup[d];
		return sobols[group]->Sample(i, d - minDim[group]);
	}

	inline void SetSeed(long seed) override
	{
		Sampler::SetSeed(seed);

		std::minstd_rand gen(seed);
		std::uniform_int_distribution<int> unif(0, 2147483646);
		
		for (unsigned int i = 0; i < sobols.size(); i++)
			sobols[i]->SetSeed(unif(gen));
	}
private:
	std::vector<SobolSampler*> sobols;

    std::vector<unsigned int> minDim;
    std::vector<unsigned int> dimensionToGroup;
};