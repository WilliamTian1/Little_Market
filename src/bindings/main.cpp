#include <pybind11/pybind11.h>

namespace py = pybind11;

int add(int i, int j) {
    return i + j;
}

PYBIND11_MODULE(polysim, m) {
    m.doc() = "PolySim: Market Microstructure Simulator"; // optional module docstring

    m.def("add", &add, "A function that adds two numbers");
}
