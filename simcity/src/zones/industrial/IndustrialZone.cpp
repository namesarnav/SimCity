#include "IndustrialZone.hpp"
#include <sstream>

IndustrialZone::IndustrialZone(int x, int y)
    : Cell(CellType::INDUSTRIAL, x, y),
      assignedWorkers(0),
      goodsProduced(0),
      state(ProductionState::INACTIVE),
      wasProducingLastTurn(false)
{
}

void IndustrialZone::updatePopulation(const std::vector<Cell *> &adjacentCells)
{
    // First check power status
    bool hasPower = hasAdjacentPower(adjacentCells);
    if (!hasPower)
    {
        handlePowerLoss();
        return;
    }

    // Update production state
    updateProductionState();

    // Check growth conditions based on current population
    if (population == 0 && canGrowToOne(adjacentCells))
    {
        population = 1;
        wasProducingLastTurn = true;
    }
    else if (population == 1 && canGrowToTwo(adjacentCells))
    {
        population = 2;
        wasProducingLastTurn = true;
    }
    else if (population == 2 && canGrowToThree(adjacentCells))
    {
        population = 3;
        wasProducingLastTurn = true;
    }
    else
    {
        wasProducingLastTurn = false;
    }

    // Calculate goods production for this turn
    calculateGoodsProduction();

    // Update pollution levels
    updatePollution();
}

bool IndustrialZone::canGrowToOne(const std::vector<Cell *> &adjacentCells) const
{
    // Need power or adjacent population and 2 workers
    return (hasAdjacentPower(adjacentCells) ||
            countAdjacentPopulation(adjacentCells, 1) >= 1) &&
           assignedWorkers >= 2;
}

bool IndustrialZone::canGrowToTwo(const std::vector<Cell *> &adjacentCells) const
{
    // Need 2+ populated neighbors and 2 workers
    return countAdjacentPopulation(adjacentCells, 1) >= 2 &&
           assignedWorkers >= 2;
}

bool IndustrialZone::canGrowToThree(const std::vector<Cell *> &adjacentCells) const
{
    // Need 4+ neighbors with pop >= 2 and 2 workers
    return countAdjacentPopulation(adjacentCells, 2) >= 4 &&
           assignedWorkers >= 2;
}

int IndustrialZone::countAdjacentPopulation(const std::vector<Cell *> &adjacentCells, int minPopulation) const
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

bool IndustrialZone::hasAdjacentPower(const std::vector<Cell *> &adjacentCells) const
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

void IndustrialZone::updateProductionState()
{
    if (!hasAdjacentPower(std::vector<Cell *>()))
    {
        state = ProductionState::POWER_LOST;
    }
    else if (assignedWorkers < population * 2)
    {
        state = ProductionState::UNDERSTAFFED;
    }
    else if (assignedWorkers > 0)
    {
        state = ProductionState::ACTIVE;
    }
    else
    {
        state = ProductionState::INACTIVE;
    }
}

void IndustrialZone::handlePowerLoss()
{
    state = ProductionState::POWER_LOST;
    goodsProduced = 0;
    wasProducingLastTurn = false;
}

void IndustrialZone::calculateGoodsProduction()
{
    if (state == ProductionState::ACTIVE)
    {
        // Each population level produces goods equal to its population
        goodsProduced = population;
    }
    else
    {
        goodsProduced = 0;
    }
}

void IndustrialZone::updatePollution()
{
    // Pollution equals population
    pollution = population;
}

bool IndustrialZone::assignWorkers(int count)
{
    if (count <= 0)
        return false;

    // Each population level requires 2 workers
    int maxNeededWorkers = population * 2;
    int actualAssignment = std::min(count, maxNeededWorkers - assignedWorkers);

    if (actualAssignment > 0)
    {
        assignedWorkers += actualAssignment;
        updateProductionState();
        return true;
    }
    return false;
}

void IndustrialZone::releaseWorkers()
{
    assignedWorkers = 0;
    updateProductionState();
}

int IndustrialZone::collectGoods()
{
    int goods = goodsProduced;
    goodsProduced = 0;
    return goods;
}

int IndustrialZone::generatePollution() const
{
    return population; // Pollution equals population
}

std::string IndustrialZone::getStatusReport() const
{
    std::stringstream report;
    report << "Industrial Zone at (" << x << "," << y << ")\n"
           << "Population: " << population << "\n"
           << "Workers Assigned: " << assignedWorkers << "\n"
           << "Goods Produced: " << goodsProduced << "\n"
           << "Pollution Level: " << pollution << "\n"
           << "State: ";

    switch (state)
    {
    case ProductionState::INACTIVE:
        report << "Inactive";
        break;
    case ProductionState::ACTIVE:
        report << "Active Production";
        break;
    case ProductionState::POWER_LOST:
        report << "Power Lost";
        break;
    case ProductionState::UNDERSTAFFED:
        report << "Understaffed";
        break;
    }

    report << "\nProducing Last Turn: " << (wasProducingLastTurn ? "Yes" : "No");

    return report.str();
}
