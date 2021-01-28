// ╔═╗╦╔═╔═╗╦  ╔═╗╔╦╗╔═╗╔╗╔╔═╗╦
// ╚═╗╠╩╗║╣ ║  ║╣  ║ ║ ║║║║║ ╦║
// ╚═╝╩ ╩╚═╝╩═╝╚═╝ ╩ ╚═╝╝╚╝╚═╝╩═╝
// ─┐ ┬┌─┐┌┐┌┌─┐┌┐ ┬ ┬┌┬┐┌─┐ ─┐ ┬┬ ┬┌─┐
// ┌┴┬┘├┤ ││││ │├┴┐└┬┘ │ ├┤  ┌┴┬┘└┬┘┌─┘
// ┴ └─└─┘┘└┘└─┘└─┘ ┴  ┴ └─┘o┴ └─ ┴ └─┘
// Author:  SENEX @ XENOBYTE.XYZ
// License: MIT License
// Website: https://xenobyte.xyz/projects/?nav=cas-sgl

#ifndef CELL_AUTO_SIMULATION_HPP
#define CELL_AUTO_SIMULATION_HPP

#include <iostream>
#include <vector>
#include <thread>
#include <cstdint>
#include "skeletonGL/skeletonGL.hpp"
enum class SIMULATIONS : std::uint8_t
{
    SIM_1 = 0,
    SIM_2 = 1,
    SIM_3 = 2,
    END_ENUM = 3
};

const std::uint16_t SIMULATION_WIDTH = 100;
const std::uint16_t SIMULATION_HEIGHT = 100;


class CellAutoSimulation
{
private:
    std::vector<std::pair<std::uint16_t, std::uint16_t>> pCellsToRender, pInitialValues;
    std::array<std::uint16_t, SIMULATION_WIDTH*SIMULATION_HEIGHT> pSimulation, pSimulationDelta;
    bool pSimulationStarted;
    std::uint64_t pIterations;
public:
    CellAutoSimulation();

    void update();
    void setCell(std::uint16_t x, std::uint16_t y, std::string str);
    //std::uint8_t getCellStatus(std::uint16_t x, std::uint16_t y);

    // Get the active cells positions, this COPIES the internal vector
    // and thus the process can be notoriously slow depending on the
    // dimensions of the simulation
    std::vector<std::pair<std::uint16_t, std::uint16_t>> cellsToRender();

    // Returns a copy of the array populated by setCell(), call right after
    // generating the array to get the simulation's inital values or "seed"
    std::array<std::uint16_t, SIMULATION_WIDTH*SIMULATION_HEIGHT> getArrayValues();
    std::pair<std::uint16_t, std::uint16_t> getActiveCell(std::uint16_t cellID);
    std::uint16_t getTotalActiveCells();
    std::uint64_t getIterations();
    void clearActiveCells();

    void resetSimulation();

};





#endif
