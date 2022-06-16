#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <memory>

namespace py = pybind11;

#include "CurrentTrackInformationAction.h"
#include "QMCRandomEngine.h"

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
}