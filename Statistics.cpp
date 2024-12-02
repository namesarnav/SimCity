
// Statistics.cpp
#include "Statistics.h"

bool Statistics::isValidCoordinates(const std::vector<std::vector<Cell>> &grid,
                                    int x1, int y1, int x2, int y2)
{
    if (grid.empty() || grid[0].empty())
        return false;
    int height = grid.size();
    int width = grid[0].size();

    return x1 >= 0 && x1 < width && y1 >= 0 && y1 < height &&
           x2 >= 0 && x2 < width && y2 >= 0 && y2 < height;
}

int Statistics::getTotalPopulation(const std::vector<std::vector<Cell>> &grid, char type)
{
    int total = 0;
    for (const auto &row : grid)
    {
        for (const auto &cell : row)
        {
            if (cell.getType() == type)
            {
                total += cell.getPopulation();
            }
        }
    }
    return total;
}

int Statistics::getAreaPopulation(const std::vector<std::vector<Cell>> &grid,
                                  int x1, int y1, int x2, int y2, char type)
{
    if (!isValidCoordinates(grid, x1, y1, x2, y2))
        return 0;

    int total = 0;
    for (int y = y1; y <= y2; y++)
    {
        for (int x = x1; x <= x2; x++)
        {
            if (grid[y][x].getType() == type)
            {
                total += grid[y][x].getPopulation();
            }
        }
    }
    return total;
}

int Statistics::getTotalPollution(const std::vector<std::vector<Cell>> &grid)
{
    int total = 0;
    for (const auto &row : grid)
    {
        for (const auto &cell : row)
        {
            total += cell.getPollution();
        }
    }
    return total;
}

int Statistics::getAreaPollution(const std::vector<std::vector<Cell>> &grid,
                                 int x1, int y1, int x2, int y2)
{
    if (!isValidCoordinates(grid, x1, y1, x2, y2))
        return 0;

    int total = 0;
    for (int y = y1; y <= y2; y++)
    {
        for (int x = x1; x <= x2; x++)
        {
            total += grid[y][x].getPollution();
        }
    }
    return total;
}
