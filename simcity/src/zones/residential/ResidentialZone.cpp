#include "zones/residential/ResidentialZone.hpp"
#include <sstream>

ResidentialZone::ResidentialZone(int x, int y)
    : Cell(CellType::RESIDENTIAL, x, y), wasActiveLastTurn(false) {
}

void ResidentialZone::updatePopulation(const std::vector<Cell*>& adjacentCells) {
    wasActiveLastTurn = false;
    
    // Check power first - if no power and no populated neighbors, can't grow
    if (!hasAdjacentPower(adjacentCells) && countAdjacentPopulation(adjacentCells, 1) == 0) {
        return;
    }
    
    // Check growth conditions in order from highest to lowest population requirement
    if (population == 4 && canGrowToFive(adjacentCells)) {
        population = 5;
        wasActiveLastTurn = true;
    }
    else if (population == 3 && canGrowToFour(adjacentCells)) {
        population = 4;
        wasActiveLastTurn = true;
    }
    else if (population == 2 && canGrowToThree(adjacentCells)) {
        population = 3;
        wasActiveLastTurn = true;
    }
    else if (population == 1 && canGrowToTwo(adjacentCells)) {
        population = 2;
        wasActiveLastTurn = true;
    }
    else if (population == 0 && canGrowToOne(adjacentCells)) {
        population = 1;
        wasActiveLastTurn = true;
    }
}

bool ResidentialZone::canGrowToOne(const std::vector<Cell*>& adjacentCells) const {
    // Need either power or one populated neighbor
    return hasAdjacentPower(adjacentCells) || 
           countAdjacentPopulation(adjacentCells, 1) >= 1;
}

bool ResidentialZone::canGrowToTwo(const std::vector<Cell*>& adjacentCells) const {
    // Need 2+ populated neighbors
    return countAdjacentPopulation(adjacentCells, 1) >= 2;
}

bool ResidentialZone::canGrowToThree(const std::vector<Cell*>& adjacentCells) const {
    // Need 4+ neighbors with population >= 2
    return countAdjacentPopulation(adjacentCells, 2) >= 4;
}

bool ResidentialZone::canGrowToFour(const std::vector<Cell*>& adjacentCells) const {
    // Need 6+ neighbors with population >= 3
    return countAdjacentPopulation(adjacentCells, 3) >= 6;
}

bool ResidentialZone::canGrowToFive(const std::vector<Cell*>& adjacentCells) const {
    // Need 8+ neighbors with population >= 4
    return countAdjacentPopulation(adjacentCells, 4) >= 8;
}

int ResidentialZone::countAdjacentPopulation(const std::vector<Cell*>& adjacentCells, int minPopulation) const {
    int count = 0;
    for (const Cell* cell : adjacentCells) {
        if (cell && cell->getPopulation() >= minPopulation) {
            count++;
        }
    }
    return count;
}

bool ResidentialZone::hasAdjacentPower(const std::vector<Cell*>& adjacentCells) const {
    for (const Cell* cell : adjacentCells) {
        if (!cell) continue;
        
        CellType type = cell->getType();
        if (type == CellType::POWERLINE || 
            type == CellType::POWERLINE_ROAD || 
            type == CellType::POWERPLANT) {
            return true;
        }
    }
    return false;
}

std::string ResidentialZone::getStatusReport() const {
    std::stringstream report;
    report << "Residential Zone at (" << x << "," << y << ")\n"
           << "Population: " << population << "\n"
           << "Power Status: " << (hasPower ? "Connected" : "No Power") << "\n"
           << "Growth Status: " << (wasActiveLastTurn ? "Growing" : "Stable") << "\n";
    return report.str();
}