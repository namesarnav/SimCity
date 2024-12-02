// ResidentialSystem.h
// Complete implementation for the residential zone management system
#ifndef RESIDENTIAL_SYSTEM_H
#define RESIDENTIAL_SYSTEM_H

#include <vector>
#include "Cell.h"

class ResidentialSystem
{
public:
    // Core functions for residential zone management
    static void update(std::vector<std::vector<Cell>> &grid);
    static int getTotalPopulation(const std::vector<std::vector<Cell>> &grid);
    static int getAvailableWorkers(const std::vector<std::vector<Cell>> &grid);

    // Growth condition checking
    static bool canGrow(const std::vector<std::vector<Cell>> &grid, int x, int y);

    // Utility functions for zone management
    static int countAdjacentPopulation(const std::vector<std::vector<Cell>> &grid, int x, int y, int minPop);
    static bool isPowered(const std::vector<std::vector<Cell>> &grid, int x, int y);

    // Constants for growth rules
    static const int MAX_POPULATION = 5;
    static const int POWER_RADIUS = 1;
};

#endif // RESIDENTIAL_SYSTEM_H