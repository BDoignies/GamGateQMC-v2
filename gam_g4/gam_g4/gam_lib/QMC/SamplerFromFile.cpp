#include "SamplerFromFile.h"

SamplerFromFile::SamplerFromFile(
    const std::string& filename, 
    PointCount n_, DimensionCount d_):
    Sampler("SamplerFromFile"), n(n_), d(d_)
{
    data = new SampleType[n * d];
    Read(filename);
}

double SamplerFromFile::Sample(PointCount i, DimensionCount d_)
{
    if (i >= n || d_ >= d)
    {
        std::string msg = "[SamplerFromFile]: Can not sample point (" + std::to_string(i) + ", " + std::to_string(d_) + ")";
        G4Exception(
            __FILE__, __PRETTY_FUNCTION__, 
            G4ExceptionSeverity::FatalErrorInArgument, msg.c_str()
        );
    }
    
    return data[i * d + d_];
}

void SamplerFromFile::Read(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::string msg = "[SamplerFromFile]: Can not open file " + filename + "";
        G4Exception(
            __FILE__, __PRETTY_FUNCTION__, 
            G4ExceptionSeverity::FatalErrorInArgument, msg.c_str()
        );
    }

    PointCount i = 0;
    static constexpr unsigned long int lineSize = 1000;
    char line[lineSize];
            
    while (!file.eof())
    {
        file.getline(line, lineSize);
        if (line[0] == '#' || line[0] == '\0') break;

        std::stringstream ss;
        ss.str(line);

        for (DimensionCount d_ = 0; d_ < d; d_++)
        {
            ss >> data[i * d + d_];        
        }

        i += 1;
    }

    // In case fewer point are read
    // n = i;
}

SamplerFromFile::~SamplerFromFile()
{
    delete[] data;
}