// CommercialSystem.cpp
#include "CommercialSystem.h"
#include <algorithm>

struct GrowthCell
{
    int x, y;
    int population;
    int adjacentPop;
};

bool CommercialSystem::isPowered(const std::vector<std::vector<Cell>> &grid, int x, int y)
{
    for (int dy = -1; dy <= 1; dy++)
    {
        for (int dx = -1; dx <= 1; dx++)
        {
            if (dx == 0 && dy == 0)
                continue;

            int newX = x + dx;
            int newY = y + dy;

            if (newX >= 0 && newX < grid[0].size() && newY >= 0 && newY < grid.size())
            {
                char type = grid[newY][newX].getType();
                if (type == 'T' || type == '#' || type == 'P')
                {
                    return true;
                }
            }
        }
    }
    return false;
}

int CommercialSystem::countAdjacentPopulation(const std::vector<std::vector<Cell>> &grid, int x, int y, int minPop)
{
    int count = 0;
    for (int dy = -1; dy <= 1; dy++)
    {
        for (int dx = -1; dx <= 1; dx++)
        {
            if (dx == 0 && dy == 0)
                continue;

            int newX = x + dx;
            int newY = y + dy;

            if (newX >= 0 && newX < grid[0].size() && newY >= 0 && newY < grid.size())
            {
                const Cell &cell = grid[newY][newX];
                if (cell.getType() == 'C' && cell.getPopulation() >= minPop)
                {
                    count++;
                }
            }
        }
    }
    return count;
}

bool CommercialSystem::canGrow(const std::vector<std::vector<Cell>> &grid, int x, int y)
{
    const Cell &cell = grid[y][x];
    int pop = cell.getPopulation();

    switch (pop)
    {
    case 0:
        return isPowered(grid, x, y) || countAdjacentPopulation(grid, x, y, 1) >= 1;
    case 1:
        return countAdjacentPopulation(grid, x, y, 1) >= 2;
    default:
        return false;
    }
}
void CommercialSystem::update(std::vector<std::vector<Cell>> &grid, int &availableWorkers, int &availableGoods)
{
    std::vector<GrowthCell> growthCells;

    // First: Identify all potential growth cells
    for (size_t y = 0; y < grid.size(); y++)
    {
        for (size_t x = 0; x < grid[0].size(); x++)
        {
            if (grid[y][x].getType() == 'C' && canGrow(grid, x, y))
            {
                growthCells.push_back({static_cast<int>(x),
                                       static_cast<int>(y),
                                       grid[y][x].getPopulation(),
                                       countAdjacentPopulation(grid, x, y, 1)});
            }
        }
    }

    // Sort by priority rules
    std::sort(growthCells.begin(), growthCells.end(),
              [](const GrowthCell &a, const GrowthCell &b)
              {
                  // 1. Population size
                  if (a.population != b.population)
                      return a.population > b.population;
                  // 2. Adjacent population
                  if (a.adjacentPop != b.adjacentPop)
                      return a.adjacentPop > b.adjacentPop;
                  // 3. Y coordinate (smaller first)
                  if (a.y != b.y)
                      return a.y < b.y;
                  // 4. X coordinate (smaller first)
                  return a.x < b.x;
              });

    // Apply growth to sorted cells
    for (const auto &cell : growthCells)
    {
        if (availableWorkers >= 1 && availableGoods >= 1)
        {
            grid[cell.y][cell.x].setPopulation(
                grid[cell.y][cell.x].getPopulation() + 1);
            availableWorkers--;
            availableGoods--;
        }
    }
}

int CommercialSystem::getTotalPopulation(const std::vector<std::vector<Cell>> &grid)
{
    int total = 0;
    for (const auto &row : grid)
    {
        for (const auto &cell : row)
        {
            if (cell.getType() == 'C')
            {
                total += cell.getPopulation();
            }
        }
    }
    return total;
}