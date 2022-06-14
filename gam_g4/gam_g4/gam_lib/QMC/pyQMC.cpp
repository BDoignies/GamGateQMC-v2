#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <memory>

namespace py = pybind11;

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
        .def(py::init<std::string>(),
             py::arg("profileFilename") = std::string("")
        );
}