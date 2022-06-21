#include "PartDimensionProvider.h"

PartDimensionProvider::PartDimensionProvider(const std::string& n) : 
    DimensionProvider(n)
{  }

TotalDimensionProvider::TotalDimensionProvider(
    PartDimensionProvider* prim,
    const std::vector<PartDimensionProvider*>& p
) : 
    DimensionProvider("TotalDimProvider"),
    primary(prim),
    providers(p)
{ }

unsigned int TotalDimensionProvider::GetNumberOfDimensionAtBounce(unsigned int i) const
{
    unsigned int count = primary->GetMaxDimension();
    for (const auto& prov : providers) count += prov->GetMaxDimension() * i;

    return count;
}

DimensionCount TotalDimensionProvider::GetCurrentDimension()
{
    // const std::source_location& location = CurrentTrackInformation::currentLocation;
    const std::string& className = CurrentTrackInformation::currentClassName;
    const std::string& funcName  = CurrentTrackInformation::currentFuncName;
    unsigned int count = 0;

    if (primary->Accept("", className, funcName)) 
        return primary->GetCurrentDimension();
    count += primary->GetMaxDimension();

    for (auto& prov : providers)
    {
        if (prov->Accept("", className, funcName)) return count + prov->GetCurrentDimension();
        count += prov->GetMaxDimension();
    }    
    return UNKNOWN_DIMENSION; 
}

inline TotalDimensionProvider::~TotalDimensionProvider()
{

}