#include "zones/commercial/CommercialZone.hpp"
#include <sstream>

CommercialZone::CommercialZone(int x, int y)
    : Cell(CellType::COMMERCIAL, x, y),
      assignedWorkers(0),
      consumedGoods(0),
      state(CommerceState::INACTIVE),
      wasActiveLastTurn(false)
{
}

void CommercialZone::updatePopulation(const std::vector<Cell *> &adjacentCells)
{
    // First check power status
    bool hasPower = hasAdjacentPower(adjacentCells);
    if (!hasPower)
    {
        handlePowerLoss();
        return;
    }

    // Update commerce state
    updateCommerceState();

    wasActiveLastTurn = false;

    // Check growth conditions based on current population
    if (population == 0 && canGrowToOne(adjacentCells))
    {
        population = 1;
        wasActiveLastTurn = true;
    }
    else if (population == 1 && canGrowToTwo(adjacentCells))
    {
        population = 2;
        wasActiveLastTurn = true;
    }
    else if (population == 2 && canGrowToThree(adjacentCells))
    {
        population = 3;
        wasActiveLastTurn = true;
    }

    // Calculate goods consumption for this turn
    calculateGoodsConsumption();
}

bool CommercialZone::canGrowToOne(const std::vector<Cell *> &adjacentCells) const
{
    // Need power, 1 worker, and 1 good
    return hasAdjacentPower(adjacentCells) &&
           assignedWorkers >= 1 &&
           consumedGoods >= 1;
}

bool CommercialZone::canGrowToTwo(const std::vector<Cell *> &adjacentCells) const
{
    // Need 2+ populated neighbors, 1 worker, and 1 good
    return countAdjacentPopulation(adjacentCells, 1) >= 2 &&
           assignedWorkers >= 1 &&
           consumedGoods >= 1;
}

bool CommercialZone::canGrowToThree(const std::vector<Cell *> &adjacentCells) const
{
    // Need 4+ neighbors with pop >= 2, 1 worker, and 1 good
    return countAdjacentPopulation(adjacentCells, 2) >= 4 &&
           assignedWorkers >= 1 &&
           consumedGoods >= 1;
}

int CommercialZone::countAdjacentPopulation(const std::vector<Cell *> &adjacentCells, int minPopulation) const
{
    int count = 0;
    for (const Cell *cell : adjacentCells)
    {
        if (cell && cell->getPopulation() >= minPopulation)
        {
            count++;
        }
    }
    return count;
}

bool CommercialZone::hasAdjacentPower(const std::vector<Cell *> &adjacentCells) const
{
    for (const Cell *cell : adjacentCells)
    {
        if (!cell)
            continue;

        CellType type = cell->getType();
        if (type == CellType::POWERLINE ||
            type == CellType::POWERLINE_ROAD ||
            type == CellType::POWERPLANT)
        {
            return true;
        }
    }
    return false;
}

void CommercialZone::updateCommerceState()
{
    if (!hasPower)
    {
        state = CommerceState::POWER_LOST;
    }
    else if (assignedWorkers < 1)
    {
        state = CommerceState::UNDERSTAFFED;
    }
    else if (consumedGoods < 1)
    {
        state = CommerceState::NO_GOODS;
    }
    else
    {
        state = CommerceState::ACTIVE;
    }
}

void CommercialZone::handlePowerLoss()
{
    state = CommerceState::POWER_LOST;
    consumedGoods = 0;
    wasActiveLastTurn = false;
}

void CommercialZone::calculateGoodsConsumption()
{
    if (state == CommerceState::ACTIVE)
    {
        // Reset consumed goods counter for next turn
        consumedGoods = 0;
    }
}

bool CommercialZone::assignWorker()
{
    if (assignedWorkers < population + 1)
    {
        assignedWorkers++;
        updateCommerceState();
        return true;
    }
    return false;
}

void CommercialZone::releaseWorker()
{
    if (assignedWorkers > 0)
    {
        assignedWorkers--;
        updateCommerceState();
    }
}

bool CommercialZone::consumeGood()
{
    if (state != CommerceState::POWER_LOST && assignedWorkers > 0)
    {
        consumedGoods++;
        updateCommerceState();
        return true;
    }
    return false;
}

std::string CommercialZone::getStatusReport() const
{
    std::stringstream report;
    report << "Commercial Zone at (" << x << "," << y << ")\n"
           << "Population: " << population << "\n"
           << "Workers Assigned: " << assignedWorkers << "\n"
           << "Goods Consumed: " << consumedGoods << "\n"
           << "State: ";

    switch (state)
    {
    case CommerceState::INACTIVE:
        report << "Inactive";
        break;
    case CommerceState::ACTIVE:
        report << "Active Commerce";
        break;
    case CommerceState::POWER_LOST:
        report << "Power Lost";
        break;
    case CommerceState::UNDERSTAFFED:
        report << "Understaffed";
        break;
    case CommerceState::NO_GOODS:
        report << "No Goods Available";
        break;
    }

    report << "\nActive Last Turn: " << (wasActiveLastTurn ? "Yes" : "No");

    return report.str();
}