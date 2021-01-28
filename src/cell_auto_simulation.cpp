// ╔═╗╦╔═╔═╗╦  ╔═╗╔╦╗╔═╗╔╗╔╔═╗╦
// ╚═╗╠╩╗║╣ ║  ║╣  ║ ║ ║║║║║ ╦║
// ╚═╝╩ ╩╚═╝╩═╝╚═╝ ╩ ╚═╝╝╚╝╚═╝╩═╝
// ─┐ ┬┌─┐┌┐┌┌─┐┌┐ ┬ ┬┌┬┐┌─┐ ─┐ ┬┬ ┬┌─┐
// ┌┴┬┘├┤ ││││ │├┴┐└┬┘ │ ├┤  ┌┴┬┘└┬┘┌─┘
// ┴ └─└─┘┘└┘└─┘└─┘ ┴  ┴ └─┘o┴ └─ ┴ └─┘
// Author:  SENEX @ XENOBYTE.XYZ
// License: MIT License
// Website: https://xenobyte.xyz/projects/?nav=cas-sgl


#include "cell_auto_simulation.hpp"


CellAutoSimulation::CellAutoSimulation() : pSimulationStarted(false)
{
    resetSimulation();
}

void CellAutoSimulation::update()
{
    // Copy the current state of the simulation
    pSimulationDelta = pSimulation;
    // Reset render vector
    //pCellsToRender.clear();

    auto getCellStatus = [&](std::uint16_t x, std::uint16_t y)
                    {
                        return pSimulationDelta[(y * SIMULATION_WIDTH) + x];
                    };

    // BLACK MAGIK
    for (std::uint16_t x = 1; x < SIMULATION_WIDTH - 1; ++x)
    {
        for (std::uint16_t y = 1; y < SIMULATION_HEIGHT - 1; ++y)
        {
            // Add the cells surrounding slots
            std::uint8_t neighbors =
                getCellStatus(x-1, y-1) + getCellStatus(x, y-1) + getCellStatus(x+1, y-1) +
                getCellStatus(x-1, y) + 0/*YOU ARE HERE*/ + getCellStatus(x+1, y) +
                getCellStatus(x-1, y+1) + getCellStatus(x, y+1) + getCellStatus(x+1, y+1);

            if (getCellStatus(x, y) == 1) // Cell is active
            {
                //SGL_Log("Active CELL X:" + std::to_string(x) + " Y: " + std::to_string(y));
                std::pair<std::uint16_t, std::uint16_t>pos;
                pos.first = x;
                pos.second = y;
                pCellsToRender.push_back(pos);
                pSimulation[(y * SIMULATION_WIDTH) + x] = (neighbors == 2 || neighbors == 3);
            }
            else // Activate the cell if at least three neighboring cells are active
                pSimulation[(y * SIMULATION_WIDTH) + x] = (neighbors == 3);
        }
    }
    pIterations++;
}

void CellAutoSimulation::setCell(std::uint16_t x, std::uint16_t y, std::string str)
{
    // Check for oob requests

    // Process the input string at the requested location
    std::uint64_t counter = 0;
    for (std::string::const_iterator byteChar = str.begin(); byteChar != str.end(); byteChar++)
    {
        // By multiplying the Y value by the width of the screen it simulates the limitations of a 2D array
        // without actually decalring it so
        pSimulation[(y * SIMULATION_WIDTH) + x + counter] = ((*byteChar) == '#' ? 1 : 0);
        counter++;
    }

}

std::vector<std::pair<std::uint16_t, std::uint16_t>> CellAutoSimulation::cellsToRender()
{
    return pCellsToRender;
    pCellsToRender.clear();
}


std::pair<std::uint16_t, std::uint16_t> CellAutoSimulation::getActiveCell(std::uint16_t cellID)
{
    if (cellID < 0 || cellID >= pCellsToRender.size())
        return std::pair<std::uint16_t, std::uint16_t>{0,0};

    return pCellsToRender[cellID];
}

std::uint16_t CellAutoSimulation::getTotalActiveCells()
{
    return pCellsToRender.size();
}

void CellAutoSimulation::clearActiveCells()
{
    pCellsToRender.clear();
}

void CellAutoSimulation::resetSimulation()
{
    pSimulation.fill(0);
    pSimulationDelta.fill(0);
    pCellsToRender.clear();
    pIterations = 0;
}

std::uint64_t CellAutoSimulation::getIterations()
{
    return pIterations;
}

std::array<std::uint16_t, SIMULATION_WIDTH*SIMULATION_HEIGHT> CellAutoSimulation::getArrayValues()
{
    return pSimulation;
}
