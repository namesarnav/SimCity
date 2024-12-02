#include "ResidentialSystem.h"
#include <algorithm>

// Check if a cell has power access (within POWER_RADIUS of power infrastructure)
bool ResidentialSystem::isPowered(const std::vector<std::vector<Cell>> &grid, int x, int y)
{
    int height = grid.size();
    int width = grid[0].size();

    for (int dy = -POWER_RADIUS; dy <= POWER_RADIUS; dy++)
    {
        for (int dx = -POWER_RADIUS; dx <= POWER_RADIUS; dx++)
        {
            if (dx == 0 && dy == 0)
                continue;

            int newX = x + dx;
            int newY = y + dy;

            if (newX >= 0 && newX < width && newY >= 0 && newY < height)
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

// Count adjacent cells with population >= minPop
int ResidentialSystem::countAdjacentPopulation(const std::vector<std::vector<Cell>> &grid, int x, int y, int minPop)
{
    int count = 0;
    int height = grid.size();
    int width = grid[0].size();

    for (int dy = -1; dy <= 1; dy++)
    {
        for (int dx = -1; dx <= 1; dx++)
        {
            if (dx == 0 && dy == 0)
                continue;

            int newX = x + dx;
            int newY = y + dy;

            if (newX >= 0 && newX < width && newY >= 0 && newY < height)
            {
                const Cell &cell = grid[newY][newX];
                if (cell.getType() == 'R' && cell.getPopulation() >= minPop)
                {
                    count++;
                }
            }
        }
    }
    return count;
}

// Check if a residential cell can grow based on its current population
bool ResidentialSystem::canGrow(const std::vector<std::vector<Cell>> &grid, int x, int y)
{
    const Cell &cell = grid[y][x];
    int pop = cell.getPopulation();

    // Don't grow beyond maximum population
    if (pop >= MAX_POPULATION - 1)
    {
        return false;
    }

    // Apply growth rules based on current population
    switch (pop)
    {
    case 0:
        // Population 0->1: Need power OR 1 adjacent populated cell
        return isPowered(grid, x, y) || countAdjacentPopulation(grid, x, y, 1) >= 1;
    case 1:
        // Population 1->2: Need 2 adjacent cells with pop >= 1
        return countAdjacentPopulation(grid, x, y, 1) >= 2;
    case 2:
        // Population 2->3: Need 4 adjacent cells with pop >= 2
        return countAdjacentPopulation(grid, x, y, 2) >= 4;
    case 3:
        // Population 3->4: Need 6 adjacent cells with pop >= 3
        return countAdjacentPopulation(grid, x, y, 3) >= 6;
    case 4:
        // Population 4->5: Need 8 adjacent cells with pop >= 4
        return countAdjacentPopulation(grid, x, y, 4) >= 8;
    default:
        return false;
    }
}

// Update all residential zones in the grid
void ResidentialSystem::update(std::vector<std::vector<Cell>> &grid)
{
    std::vector<std::pair<int, int>> growthCells;

    // First pass: identify all cells that can grow
    for (size_t y = 0; y < grid.size(); y++)
    {
        for (size_t x = 0; x < grid[0].size(); x++)
        {
            if (grid[y][x].getType() == 'R' && canGrow(grid, x, y))
            {
                growthCells.push_back({x, y});
            }
        }
    }

    // Second pass: grow all identified cells
    for (const auto &pos : growthCells)
    {
        Cell &cell = grid[pos.second][pos.first];
        cell.setPopulation(cell.getPopulation() + 1);
    }
}

// Get total population of all residential zones
int ResidentialSystem::getTotalPopulation(const std::vector<std::vector<Cell>> &grid)
{
    int total = 0;
    for (const auto &row : grid)
    {
        for (const auto &cell : row)
        {
            if (cell.getType() == 'R')
            {
                total += cell.getPopulation();
            }
        }
    }
    return total;
}

// Get number of available workers (same as total population for residential)
int ResidentialSystem::getAvailableWorkers(const std::vector<std::vector<Cell>> &grid)
{
    return getTotalPopulation(grid);
}