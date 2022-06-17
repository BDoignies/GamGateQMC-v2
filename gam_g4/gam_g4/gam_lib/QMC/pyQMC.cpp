#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <memory>

namespace py = pybind11;

#include "CurrentTrackInformationAction.h"
#include "QMCRandomEngine.h"

#include "DimensionProvider.h"
#include "PointIDProvider.h"
#include "Sampler.h"

#include "SamplerFromFile.h"

class PySampler : public Sampler
{
public:
    using Sampler::Sampler;

    SampleType Sample(PointCount i, DimensionCount d) override
    {
        PYBIND11_OVERRIDE_PURE(
            SampleType,
            Sampler,
            Sample,
            i, d
        );
    }

    void SetSeed(long seed_) override
    {
        PYBIND11_OVERLOAD(
            void, 
            Sampler, 
            SetSeed,
            seed_
        );
    }
};

class PyDimensionProvider: public DimensionProvider
{
public:
    using DimensionProvider::DimensionProvider;

    DimensionCount GetCurrentDimension() override
    {
        PYBIND11_OVERRIDE_PURE(
            DimensionCount, 
            DimensionProvider,
            GetCurrentDimension
        );
    }
};

class PyPointIDProvider : public PointIDProvider
{
public:
    using PointIDProvider::PointIDProvider;

    PointCount GetCurrentPointID(DimensionCount d) override
    {
        PYBIND11_OVERRIDE_PURE(
            PointCount,
            PyPointIDProvider,
            GetCurrentPointID,
            d
        );
    }
};

class PyDefaultPointIDProvider : public DefaultPointIDProvider
{
public:
    using DefaultPointIDProvider::DefaultPointIDProvider;

    PointCount GetCurrentPointID(DimensionCount d) override
    {
        PYBIND11_OVERRIDE(
            PointCount,
            DefaultPointIDProvider,
            GetCurrentPointID,
            d
        );
    }
};

void init_QMC(py::module& m)
{
    auto qmc = m.def_submodule("qmc");
    
    // Name of random engine class
    const char* EngineName = "QMCRandomEngine";
    qmc.attr("QMCEngine") = py::str(EngineName);

    py::class_<
        QMCRandomEngine, 
        CLHEP::HepRandomEngine 
        // std::unique_ptr<QMCRandomEngine, py::nodelete>
    >(qmc, EngineName)
        .def(py::init<std::string, bool>(),
             py::arg("profilerFilename") = std::string(""),
             py::arg("profilerReadable") = false
        );

    py::class_<
        CurrentStepInformationAction,
        G4UserSteppingAction,
        std::unique_ptr<CurrentStepInformationAction, py::nodelete>
    >(qmc, "CurrentStepInformationAction")
        .def(py::init());

    // Samplers

    py::class_<WhiteNoiseSampler, Sampler>(qmc, "WhitenoiseSampler")
        .def(py::init());

    // Point ID Providers

    py::class_<DefaultPointIDProvider, PointIDProvider, PyDefaultPointIDProvider>(qmc, "DefaultPointIDProvider")
        .def(py::init<>())
        .def("GetName", &PointIDProvider::GetName)
        .def("GetCurrentPointID", &DefaultPointIDProvider::GetCurrentPointID);

    // Dimension Providers

    py::class_<WhitenoiseDimensionProvider, DimensionProvider>(qmc, "WhitenoiseDimensionProvider")
        .def(py::init<>());
}