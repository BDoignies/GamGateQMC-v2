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
{ 
    primaryDimensionCount = primary->GetMaxDimension();
    dimensionPerBounce = 0;
    for (const auto& prov : providers) dimensionPerBounce += prov->GetMaxDimension();
}

unsigned int TotalDimensionProvider::GetNumberOfDimensionAtBounce(unsigned int i) const
{
    return primaryDimensionCount + dimensionPerBounce * i;
}

unsigned int TotalDimensionProvider::GetDimensionPerBounce() const
{
    return dimensionPerBounce;
}   

DimensionCount TotalDimensionProvider::GetCurrentDimension()
{
    // const std::source_location& location = CurrentTrackInformation::currentLocation;
    const std::string& className = CurrentTrackInformation::currentClassName;
    const std::string& funcName  = CurrentTrackInformation::currentFuncName;
    const unsigned int intNumber = CurrentTrackInformation::globalStepInformations.interactionNumber;

    if (primary->Accept("", className, funcName)) 
        return primary->GetCurrentDimension();

    unsigned int count = GetNumberOfDimensionAtBounce(intNumber);
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