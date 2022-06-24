#pragma once

#include <vector>
#include <fstream>

#include "G4Exception.hh"

inline uint64_t CountStartingOnes(uint64_t i)
{
	if (i == 0) return 1;

	uint64_t count = 1;
	while (i & 1) { i >>= 1; count++; }
	return count;
}

#include "Sampler.h"

class SobolSampler : public Sampler
{
public:
	SobolSampler(const std::string& dirName, DimensionCount D_, PointCount N_, PointCount L_);

	SampleType Sample(PointCount i, DimensionCount d) override;
	void SetSeed(long seed) override;
private:
	uint64_t Scramble(uint64_t value, DimensionCount d);
	void LoadDirections(const std::string& dirName);
	void LoadPoint(PointCount i);
private:
	      uint64_t seed;
	const uint64_t D;
	const uint64_t L;
	const uint64_t N;

	const double inv32;

	uint64_t pointID;
	std::vector<double> point;
	std::vector<uint64_t> index;

	std::vector<std::vector<uint64_t>> Vs;
};