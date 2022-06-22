#pragma once

#include <vector>
#include "../DimensionProvider.h" 
#include "../CurrentTrackInformationAction.h"

class PartDimensionProvider : public DimensionProvider
{
public:
    PartDimensionProvider(const std::string& name);

    virtual bool Accept(
        const std::string& processName,
        const std::string& className, 
        const std::string& funcName
    ) = 0;

    virtual unsigned int GetMaxDimension() const = 0;
    // virtual DimensionCount GetCurrentDimension() = 0;

    virtual inline ~PartDimensionProvider() {};
};

class TotalDimensionProvider : public DimensionProvider
{
public:
    TotalDimensionProvider(
        PartDimensionProvider* primary,
        const std::vector<PartDimensionProvider*>& providers
    );

    unsigned int GetDimensionPerBounce() const;
    unsigned int GetNumberOfDimensionAtBounce(unsigned int i) const;

    DimensionCount GetCurrentDimension() override;

    ~TotalDimensionProvider();
private:
    unsigned int primaryDimensionCount;
    unsigned int dimensionPerBounce;

    PartDimensionProvider* primary;
    std::vector<PartDimensionProvider*> providers;
};
