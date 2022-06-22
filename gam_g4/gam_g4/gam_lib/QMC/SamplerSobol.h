#pragma once

#include <vector>
#include <fstream>

#include "G4Exception.hh"

inline unsigned int CountStartingOnes(unsigned int i)
{
	if (i == 0) return 1;

	unsigned int count = 1;
	while (i & 1) { i >>= 1; count++; }
	return count;
}

#include "Sampler.h"

class SobolSampler : public Sampler
{
public:
	SobolSampler(const std::string& dirName, DimensionCount D_, PointCount N_, PointCount L_);

	SampleType Sample(PointCount i, DimensionCount d) override;
private:
	void LoadDirections(const std::string& dirName);
	void LoadPoint(PointCount i);
private:
	const unsigned int D;
	const unsigned int L;
	const unsigned int N;

	const double inv32;

	unsigned int pointID;
	std::vector<double> point;
	std::vector<unsigned int> index;

	std::vector<std::vector<unsigned int>> Vs;
};