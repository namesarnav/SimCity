#ifndef COMMERCIAL_SYSTEM_H
#define COMMERCIAL_SYSTEM_H

#include <vector>
#include <algorithm>
#include "Cell.h"

class CommercialSystem {
public:
    static void update(std::vector<std::vector<Cell>>& grid, int& availableWorkers, int& availableGoods);
    static int getTotalPopulation(const std::vector<std::vector<Cell>>& grid);

private:
    static bool isPowered(const std::vector<std::vector<Cell>>& grid, int x, int y);
    static int countAdjacentPopulation(const std::vector<std::vector<Cell>>& grid, int x, int y, int minPop);
    static bool canGrow(const std::vector<std::vector<Cell>>& grid, int x, int y);
};

#endif