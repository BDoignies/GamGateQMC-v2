#include "SamplerSobol.h"
#include <random>

static uint32_t hash3( uint32_t x ) 
{
    // finalizer from murmurhash3
    x ^= x >> 16;
    x *= 0x85ebca6bu;
    x ^= x >> 13;
    x *= 0xc2b2ae35u;
    x ^= x >> 16;
    return x;
}

uint64_t owenScramble(uint64_t i, int seed, int m, int base)
{
    std::minstd_rand gen(hash3(seed));
    std::uniform_int_distribution<int> unif(0,base-1);
    std::uniform_int_distribution<int> nextSeed;
    uint64_t res = 0;
    //We start from strong digits
    uint64_t current = uint64_t(round(pow(base, m - 1)));

    for (int pos = 0; pos < m; ++pos){
        int permut = unif(gen);
        int digit = (i / current) % base;
        //Apply permutation
        res += uint64_t((digit + permut) % base) * current;
        //Move to next node in Owen tree
        gen.seed(hash3(nextSeed(gen) + digit));
        //Move to next digit
        i -= digit * current;
        current /= uint64_t(base);
    }
    return res;
}

SobolSampler::SobolSampler(const std::string& dirName, DimensionCount D_, PointCount N_, PointCount L_) :
	Sampler("Sobol"), seed(1),
	D(D_), L(L_), N(N_),
	inv32(1.0 / static_cast<double>(static_cast<unsigned long int>(1) << 32))
{
	LoadDirections(dirName);
}

void SobolSampler::SetSeed(long int s)
{
	Sampler::SetSeed(s);
	seed = static_cast<uint64_t>(s);

	pointID = 0;
	for (uint64_t j = 0; j < D; j++)
	{
		index[j] = 0;
		point[j] = (double) Scramble(index[j], j) * inv32;
	}
}

SampleType SobolSampler::Sample(PointCount i, DimensionCount d)
{
	if (d >= UNKNOWN_DIMENSION) return Whitenoise();
	if (i >= UNKNOWN_POINTID) return Whitenoise();
	if (i != pointID) LoadPoint(i);	

	return point[d];
}

uint64_t SobolSampler::Scramble(uint64_t value, DimensionCount d)
{
	if (seed == 1) return value;
	return owenScramble(value, seed + d, 32, 2);
}

void SobolSampler::LoadDirections(const std::string& dirName)
{
	constexpr static uint64_t BUFF_SIZE = 1000;
	std::ifstream infile(dirName);
	if (!infile)
	{
		std::string msg = "[Sobol]: Can not open file " + dirName;
        G4Exception(
            __FILE__, __PRETTY_FUNCTION__, 
            G4ExceptionSeverity::FatalErrorInArgument, msg.c_str()
        );
	}

	char buffer[BUFF_SIZE];
	infile.getline(buffer, BUFF_SIZE, '\n');

	Vs.resize(D);
	Vs[0].resize(L + 1);
	for (uint64_t i = 1; i <= L; i++) Vs[0][i] = 1ULL << (32 - i);

	// Load file 
	std::vector<uint64_t> m;		
	for (uint64_t j = 1; j < D; j++)
	{
		uint64_t d, s, a;
		infile >> d >> s >> a;

		m.resize(s + 1);
		for (uint64_t i = 1; i <= s; i++) infile >> m[i];


		Vs[j].resize(L + 1);
		if (L <= s)
		{
			for (uint64_t i = 1; i <= L; i++) Vs[j][i] = m[i] << (32 - i);
		}
		else 
		{
			for (uint64_t i = 1; i <= s; i++) Vs[j][i] = m[i] << (32 - i);
			for (uint64_t i = s + 1; i <= L; i++)
			{
				Vs[j][i] = Vs[j][i - s] ^ (Vs[j][i - s] >> s);
				for (uint64_t k = 1; k <= s; k++)
				{
					Vs[j][i] ^= (((a >> (s - 1 - k)) & 1) * Vs[j][i - k]);
				}
			}
		}
	}

	// Load first point
	pointID = 0;

	index = std::vector<uint64_t>(D, 0);
	point = std::vector<double>(D, 0.0);
}

void SobolSampler::LoadPoint(PointCount i)
{
	pointID = i;

	const uint64_t C = CountStartingOnes(i - 1);
	for (uint64_t j = 0; j < D; j++)
	{
		index[j] = index[j] ^ Vs[j][C];
		point[j] = (double) index[j] * inv32;
	}
	return;
	
	/*
	if (i == pointID + 1)
	{
		pointID = i;

		const uint64_t C = CountStartingOnes(i - 1);
		for (uint64_t j = 0; j < D; j++)
		{
			index[j] = index[j] ^ Vs[j][C];
			point[j] = (double) Scramble(index[j], j) * inv32;
		}
	}
	else
	{
		// Skip some points
		if (i > pointID + 1)
		{
			for (uint64_t k = pointID + 1; k < i + 1; k++)
			{
				const uint64_t C = CountStartingOnes(k - 1);
				for (uint64_t j = 0; j < D; j++)
				{
					index[j] = index[j] ^ Vs[j][C];
					point[j] = (double) Scramble(index[j], j) * inv32;
				}
			}
			pointID = i;
		}
		// Go back to 0 to build up to desired number
		else
		{
			SetSeed(seed); // Reset point

			for (uint64_t k = 0; k < i + 1; k++)
			{
				const uint64_t C = CountStartingOnes(k - 1);
				for (uint64_t j = 0; j < D; j++)
				{
					index[j] = index[j] ^ Vs[j][C];
					point[j] = (double) Scramble(index[j], j) * inv32;
				}
			}
			pointID = i;
		}
	}
	*/
}