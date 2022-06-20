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

#include "RandomProfiler.h"
#include "RandomStatistics.h"

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

void init_Sampling(py::module& qmc)
{  
    py::class_<Sampler, PySampler>(qmc, "Sampler")
        .def(py::init<std::string>(), py::arg("name"))
        .def("SetSeed", &Sampler::SetSeed)
        .def("Whitenoise", &Sampler::Whitenoise)
        .def("GetName", &Sampler::GetName)
        .def("Sample", &Sampler::Sample);

    // DimensionProvider
    py::class_<DimensionProvider, PyDimensionProvider>(qmc, "DimensionProvider")
        .def(py::init<std::string>(), py::arg("name"))
        .def("GetName", &DimensionProvider::GetName)
        .def("GetCurrentDimension", &DimensionProvider::GetCurrentDimension);


    // PointIDProvider
    py::class_<PointIDProvider, PyPointIDProvider>(qmc, "PointIDProvider")
        .def(py::init<std::string>(), py::arg("name"))
        .def("GetName", &PointIDProvider::GetName)
        .def("GetCurrentPointID", &PointIDProvider::GetCurrentPointID);    

    // Samplers

    py::class_<WhiteNoiseSampler, Sampler>(qmc, "WhitenoiseSampler")
        .def(py::init());

    py::class_<SamplerFromFile, Sampler>(qmc, "SamplerFromFile")
        .def(py::init<const std::string&, PointCount, DimensionCount>());

    // Point ID Providers

    py::class_<DefaultPointIDProvider, PointIDProvider, PyDefaultPointIDProvider>(qmc, "DefaultPointIDProvider")
        .def(py::init<>())
        .def("GetName", &PointIDProvider::GetName)
        .def("GetCurrentPointID", &DefaultPointIDProvider::GetCurrentPointID);

    // Dimension Providers

    py::class_<WhitenoiseDimensionProvider, DimensionProvider>(qmc, "WhitenoiseDimensionProvider")
        .def(py::init<>());
}

void init_QMC(py::module& m)
{
    auto qmc = m.def_submodule("qmc");
    
    // Name of random engine class
    const char* EngineName = "QMCRandomEngine";
    qmc.attr("QMCEngine") = py::str(EngineName);


    // Global class

    py::class_<
        CurrentStepInformationAction,
        G4UserSteppingAction,
        std::unique_ptr<CurrentStepInformationAction, py::nodelete>
    >(qmc, "CurrentStepInformationAction")
        .def(py::init());


    init_Sampling(qmc);

    // Base class    

    py::class_<RandomStatistics>(qmc, "Statistics")
        .def(py::init<const std::string&>(),
            py::arg("file")
        );

    py::class_<RandomProfiler>(qmc, "Profiler")
        .def(py::init<const std::string&, bool>(),
            py::arg("file"), py::arg("readable") = false
        );

    py::class_<QMCRandomEngineParameters, std::unique_ptr<QMCRandomEngineParameters, py::nodelete>>(qmc, "Parameters")
        .def(py::init<>())
        .def_readwrite("profilerOutput"  , &QMCRandomEngineParameters::profilerOutput)
        .def_readwrite("profilerReadable", &QMCRandomEngineParameters::profilerReadable)
        .def_readwrite("statsOutput", &QMCRandomEngineParameters::statsOutput)
        .def_readwrite("sampler", &QMCRandomEngineParameters::sampler)
        .def_readwrite("dimProvider", &QMCRandomEngineParameters::dimProvider)
        .def_readwrite("idProvider", &QMCRandomEngineParameters::idProvider);

    py::class_<
        QMCRandomEngine, 
        CLHEP::HepRandomEngine
        // , std::unique_ptr<QMCRandomEngine, py::nodelete>
    >(qmc, EngineName)
        .def(py::init<QMCRandomEngineParameters>(),
             py::arg("params")
        );
}