#pragma once

#include <vector>
#include <fstream>
#include <random>

#include "Sampler.h"

#include "Cascaded/SobolGenerator1D.h"
#include "Cascaded/Random.h"
#include "Cascaded/OwenScrambling.h"

template< typename T >
inline void getCascadedSobol( T *values,
                const std::vector<SobolGenerator1D >& sobols,
                const uint32_t *seeds,
                const int nDims,
                const uint32_t n,
                const uint32_t nbits,
                const bool owen_permut_flag = true,
                const uint32_t owen_tree_depth = 32 )
 {
    uint32_t index = n;		// dim 0: take n-th point as index -> into 32-bit integer
    for(int idim = 0; idim < nDims; idim++)
    {
        index = sobols[idim].getSobolInt(index);	// radix-inversion + sobol
        uint32_t result = index;
        if(owen_permut_flag)						// we need to apply OwenScrambling only when this flag is set
            result = OwenScrambling(result, seeds[idim], owen_tree_depth);
        values[idim] = T(result) / T(std::numeric_limits<uint32_t>::max());	// final value (double) for this dimension
        index = index >> (32-nbits);				// this will be used as new index for the next dimension
    }
}   

class CascadedSampler : public Sampler
{
public:
	inline CascadedSampler(const std::string& file, unsigned int bits_, DimensionCount dims_) : 
		Sampler("Cascaded"),
		currentPoint(0), owen(true), bits(bits_), dims(dims_)
	{
    	std::ifstream tableFile(file);
		
		load_init_table(tableFile, d, s, a, m, dims);
    	init_sobols(sobols, d, s, a, m);
	}

	void SetSeed(long seed) override
	{
		Sampler::SetSeed(seed);

		std::uniform_int_distribution<uint32_t> unif32bits(0);
   		std::mt19937_64 gen(seed);

   		realSeeds.resize(dims);
   		for (unsigned int iDim = 0; iDim < dims; ++iDim) {
            realSeeds[iDim] = unif32bits(gen);
        }

        owen = (seed != 1);

        currentPoint = 0;
    	getCascadedSobol<double>(
    		point.data(), sobols, realSeeds.data(),
    		dims, 0, bits, owen
    	);     
	}

	SampleType Sample(PointCount i, DimensionCount dim)
	{
		if (i != currentPoint)
		{
			currentPoint = i;
			getCascadedSobol<double>(
	    		point.data(), sobols, realSeeds.data(),
	    		dims, i, bits, owen
	    	);   
		}

		return point[dim];
	}

private:
	std::vector<SobolGenerator1D> sobols;
    std::vector<uint32_t> d;
    std::vector<uint32_t> s;
    std::vector<uint32_t> a;
    std::vector<std::vector<uint32_t>> m;

    std::vector<uint32_t> realSeeds;

    unsigned int currentPoint;
    std::vector<double> point;

    bool owen;
    unsigned int bits;
    unsigned int dims;
};