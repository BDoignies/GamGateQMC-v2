#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "ProcessLimits.h"
#include "LimittedPhysicList.h"

/**
 * @brief Python binding of custom physics list module
 */
void init_CustomPhysicsList(py::module &m) 
{
    auto phlist = m.def_submodule("phlist");

    // Expose the name of custom physics list 
    phlist.attr("LimittedPhlist") = py::str("LimittedPhysicList");
    
    // Expose enum for disabled behavior
    py::enum_<DisabledBehavior>(phlist, "DisabledBehavior")
        .value("AT_REST",  DisabledBehavior::AT_REST)
        .value("INFINITE", DisabledBehavior::INFINITE);

    // Expose processlimit class
    py::class_<ProcessLimits>(phlist, "ProcessLimits")
        .def(
            py::init<G4int, DisabledBehavior>(),
            py::arg("MaxInteraction"), py::arg("DisabledBehavior")
        );
        
    // Expose limitted physics list to python side
    py::class_<LimittedPhysicList, G4VPhysicsConstructor, std::unique_ptr<LimittedPhysicList, py::nodelete>>(phlist, "LimittedPhysicList")
      .def(
        py::init<G4int, G4String, G4int, std::map<std::string, ProcessLimits>>(), 
        py::arg("log") = 1, py::arg("name") = "", py::arg("maxSteps") = 32168,
        py::arg("processes") = std::map<std::string, ProcessLimits>{}
      );
}