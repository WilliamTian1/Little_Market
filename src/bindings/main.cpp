#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "OrderBook.h"
#include "Agent.h"
#include "Engine.h"

namespace py = pybind11;
using namespace polysim;

// Trampoline class for Agent to allow overriding virtual methods in Python
class PyAgent : public Agent {
public:
    using Agent::Agent; // Inherit constructors

    void on_tick(const std::map<double, double>& market_snapshot) override {
        PYBIND11_OVERRIDE_PURE(
            void,               // Return type
            Agent,              // Parent class
            on_tick,            // Name of function in C++ (must match Python name)
            market_snapshot     // Argument(s)
        );
    }

    void on_trade(const Trade& trade) override {
        PYBIND11_OVERRIDE_PURE(
            void,
            Agent,
            on_trade,
            trade
        );
    }
};

// Helper function to test callback
void test_agent_callback(Agent& a) {
    std::map<double, double> snapshot = {{100.0, 10.0}, {101.0, 5.0}};
    a.on_tick(snapshot);
}

PYBIND11_MODULE(polysim, m) {
    m.doc() = "PolySim: Market Microstructure Simulator";

    // Bind Side Enum
    py::enum_<Side>(m, "Side")
        .value("BUY", Side::BUY)
        .value("SELL", Side::SELL)
        .export_values();

    // Bind Order Struct
    py::class_<Order>(m, "Order")
        .def(py::init<uint64_t, uint64_t, Side, double, double>())
        .def_readwrite("id", &Order::id)
        .def_readwrite("agent_id", &Order::agent_id)
        .def_readwrite("side", &Order::side)
        .def_readwrite("price", &Order::price)
        .def_readwrite("quantity", &Order::quantity);

    // Bind Trade Struct
    py::class_<Trade>(m, "Trade")
        .def_readwrite("buyer_id", &Trade::buyer_id)
        .def_readwrite("seller_id", &Trade::seller_id)
        .def_readwrite("price", &Trade::price)
        .def_readwrite("quantity", &Trade::quantity);

    // Bind OrderBook Class
    py::class_<OrderBook>(m, "OrderBook")
        .def(py::init<>())
        .def("add_order", &OrderBook::AddOrder)
        .def("get_snapshot", &OrderBook::get_snapshot);

    // Bind Agent Class with Trampoline
    py::class_<Agent, PyAgent, std::shared_ptr<Agent>>(m, "Agent")
        .def(py::init<uint64_t, double, double>())
        .def("on_tick", &Agent::on_tick)
        .def("on_trade", &Agent::on_trade)
        .def("log", &Agent::log)
        .def("place_limit_order", &Agent::place_limit_order)
        .def_readwrite("agent_id", &Agent::agent_id)
        .def_readwrite("cash", &Agent::cash)
        .def_readwrite("inventory", &Agent::inventory);

    // Bind Engine Class
    py::class_<Engine>(m, "Engine")
        .def(py::init<>())
        .def("add_agent", &Engine::add_agent)
        .def("run", &Engine::run);

    // Bind Helper Function
    m.def("test_agent_callback", &test_agent_callback);
}
