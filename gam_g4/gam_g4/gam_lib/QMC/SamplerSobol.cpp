#include "SamplerSobol.h"

SobolSampler::SobolSampler(const std::string& dirName, DimensionCount D_, PointCount N_, PointCount L_) :
	Sampler("Sobol"),
	D(D_), L(L_), N(N_),
	inv32(1.0 / static_cast<double>(static_cast<unsigned long int>(1) << 32))
{
	LoadDirections(dirName);
}

SampleType SobolSampler::Sample(PointCount i, DimensionCount d)
{
	if (i != pointID) LoadPoint(i);	
	return point[d];
}

void SobolSampler::LoadDirections(const std::string& dirName)
{
	constexpr static unsigned int BUFF_SIZE = 1000;
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
	for (unsigned i = 1; i <= L; i++) Vs[0][i] = 1 << (32 - i);

	// Load file 
	std::vector<unsigned int> m;		
	for (unsigned int j = 1; j < D; j++)
	{
		unsigned int d, s, a;
		infile >> d >> s >> a;

		m.resize(s + 1);
		for (unsigned int i = 1; i <= s; i++) infile >> m[i];


		Vs[j].resize(L + 1);
		if (L <= s)
		{
			for (unsigned int i = 1; i <= L; i++) Vs[j][i] = m[i] << (32 - i);
		}
		else 
		{
			for (unsigned int i = 1; i <= s; i++) Vs[j][i] = m[i] << (32 - i);
			for (unsigned int i = s + 1; i <= L; i++)
			{
				Vs[j][i] = Vs[j][i - s] ^ (Vs[j][i - s] >> s);
				for (unsigned int k = 1; k <= s; k++)
				{
					Vs[j][i] ^= (((a >> (s - 1 - k)) & 1) * Vs[j][i - k]);
				}
			}
		}
	}

	// Load first point
	pointID = 0;
	index = std::vector<unsigned int>(D, 0);
	point = std::vector<double>(D, 0.0);
}

void SobolSampler::LoadPoint(PointCount i)
{
	if (i == pointID + 1)
	{
		pointID = i;

		const unsigned int C = CountStartingOnes(i - 1);
		for (unsigned int j = 0; j < D; j++)
		{
			index[j] = index[j] ^ Vs[j][C];
			point[j] = (double) index[j] * inv32;
		}
	}
	else
	{
		// Skip some points
		if (i > pointID + 1)
		{
			for (unsigned int k = pointID + 1; k < i + 1; k++)
			{
				const unsigned int C = CountStartingOnes(k - 1);
				for (unsigned int j = 0; j < D; j++)
				{
					index[j] = index[j] ^ Vs[j][C];
					point[j] = (double) index[j] * inv32;
				}
			}
			pointID = i;
		}
		// Go back to 0 to build up to desired number
		else
		{
			index = std::vector<unsigned int>(D, 0);
			point = std::vector<double>(D, 0.0);
			for (unsigned int k = 0; k < i + 1; k++)
			{
				const unsigned int C = CountStartingOnes(k - 1);
				for (unsigned int j = 0; j < D; j++)
				{
					index[j] = index[j] ^ Vs[j][C];
					point[j] = (double) index[j] * inv32;
				}
			}
			pointID = i;
		}
	}
}