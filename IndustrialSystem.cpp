#include "IndustrialSystem.h"
struct GrowthCell
{
    int x, y;
    int population;
    int adjacentPop;
};

bool IndustrialSystem::isPowered(const std::vector<std::vector<Cell>> &grid, int x, int y)
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

int IndustrialSystem::countAdjacentPopulation(const std::vector<std::vector<Cell>> &grid, int x, int y, int minPop)
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
                if (cell.getType() == 'I' && cell.getPopulation() >= minPop)
                {
                    count++;
                }
            }
        }
    }
    return count;
}

bool IndustrialSystem::canGrow(const std::vector<std::vector<Cell>> &grid, int x, int y)
{
    const Cell &cell = grid[y][x];
    int pop = cell.getPopulation();

    switch (pop)
    {
    case 0:
        return isPowered(grid, x, y) || countAdjacentPopulation(grid, x, y, 1) >= 1;
    case 1:
        return countAdjacentPopulation(grid, x, y, 1) >= 2;
    case 2:
        return countAdjacentPopulation(grid, x, y, 2) >= 4;
    default:
        return false;
    }
}
void IndustrialSystem::update(std::vector<std::vector<Cell>> &grid, int &availableWorkers, int &availableGoods)
{
    // Only process after Commercial (priority enforced by Region class)
    std::vector<GrowthCell> growthCells;

    for (size_t y = 0; y < grid.size(); y++)
    {
        for (size_t x = 0; x < grid[0].size(); x++)
        {
            if (grid[y][x].getType() == 'I' && canGrow(grid, x, y))
            {
                growthCells.push_back({static_cast<int>(x),
                                       static_cast<int>(y),
                                       grid[y][x].getPopulation(),
                                       countAdjacentPopulation(grid, x, y, 1)});
            }
        }
    }

    // Same priority sorting as Commercial
    std::sort(growthCells.begin(), growthCells.end(),
              [](const GrowthCell &a, const GrowthCell &b)
              {
                  if (a.population != b.population)
                      return a.population > b.population;
                  if (a.adjacentPop != b.adjacentPop)
                      return a.adjacentPop > b.adjacentPop;
                  if (a.y != b.y)
                      return a.y < b.y;
                  return a.x < b.x;
              });

    // Apply growth to sorted cells
    for (const auto &cell : growthCells)
    {
        if (availableWorkers >= 2)
        { // Industrial needs 2 workers
            grid[cell.y][cell.x].setPopulation(
                grid[cell.y][cell.x].getPopulation() + 1);
            availableWorkers -= 2;
            availableGoods++; // Produces 1 good
        }
    }
}

void IndustrialSystem::updatePollution(std::vector<std::vector<Cell>> &grid)
{
    std::vector<std::vector<int>> newPollution(grid.size(), std::vector<int>(grid[0].size(), 0));

    // Calculate pollution from industrial zones and power plants
    for (size_t y = 0; y < grid.size(); y++)
    {
        for (size_t x = 0; x < grid[0].size(); x++)
        {
            char type = grid[y][x].getType();
            int pollutionSource = 0;

            if (type == 'I')
            {
                pollutionSource = grid[y][x].getPopulation();
            }
            else if (type == 'P')
            {
                pollutionSource = 4; // Power plants produce constant pollution
            }

            if (pollutionSource > 0)
            {
                // Spread pollution to surrounding cells
                for (int dy = -3; dy <= 3; dy++)
                {
                    for (int dx = -3; dx <= 3; dx++)
                    {
                        int newX = static_cast<int>(x) + dx;
                        int newY = static_cast<int>(y) + dy;

                        if (newX >= 0 && newX < static_cast<int>(grid[0].size()) &&
                            newY >= 0 && newY < static_cast<int>(grid.size()))
                        {
                            int distance = std::max(std::abs(dx), std::abs(dy));
                            int spreadPollution = std::max(0, pollutionSource - distance);
                            newPollution[newY][newX] += spreadPollution;
                        }
                    }
                }
            }
        }
    }

    // Update pollution values in grid
    for (size_t y = 0; y < grid.size(); y++)
    {
        for (size_t x = 0; x < grid[0].size(); x++)
        {
            grid[y][x].setPollution(newPollution[y][x]);
        }
    }
}

int IndustrialSystem::getTotalPopulation(const std::vector<std::vector<Cell>> &grid)
{
    int total = 0;
    for (const auto &row : grid)
    {
        for (const auto &cell : row)
        {
            if (cell.getType() == 'I')
            {
                total += cell.getPopulation();
            }
        }
    }
    return total;
}
