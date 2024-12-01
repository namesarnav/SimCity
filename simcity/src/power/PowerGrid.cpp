#include "power/PowerGrid.hpp"
#include <queue>
#include <algorithm>

PowerGrid::PowerGrid(int width, int height)
    : width(width), height(height)
{
    powerMap.resize(height, std::vector<bool>(width, false));
}

void PowerGrid::update(const std::vector<std::vector<std::unique_ptr<Cell>>> &grid)
{
    // Store grid reference
    currentGrid = &grid;

    // Reset power status
    resetPowerMap();

    // Find power plants and add them as sources
    clearPowerSources();
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            const Cell *cell = grid[y][x].get();
            if (cell && cell->getType() == CellType::POWERPLANT)
            {
                addPowerSource(new PowerSource(x, y));
            }
        }
    }

    // Propagate power from each source
    for (const auto *source : powerSources)
    {
        if (source && source->isActive())
        {
            propagatePower(source->getX(), source->getY());
        }
    }
}

void PowerGrid::resetPowerMap()
{
    for (auto &row : powerMap)
    {
        std::fill(row.begin(), row.end(), false);
    }
}

void PowerGrid::propagatePower(int startX, int startY)
{
    if (!isValidPosition(startX, startY))
        return;

    std::queue<std::pair<int, int>> powerQueue;
    powerQueue.push({startX, startY});
    powerMap[startY][startX] = true;

    // Direction arrays for all 8 adjacent cells
    const int dx[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    const int dy[] = {-1, 0, 1, -1, 1, -1, 0, 1};

    while (!powerQueue.empty())
    {
        auto [x, y] = powerQueue.front();
        powerQueue.pop();

        // Check all adjacent cells
        for (int dir = 0; dir < 8; ++dir)
        {
            int newX = x + dx[dir];
            int newY = y + dy[dir];

            if (canPropagateTo(newX, newY) && !powerMap[newY][newX])
            {
                powerMap[newY][newX] = true;
                powerQueue.push({newX, newY});
            }
        }
    }
}

bool PowerGrid::canPropagateTo(int x, int y) const
{
    if (!isValidPosition(x, y))
        return false;

    if (!currentGrid)
        return false;
    const Cell *cell = (*currentGrid)[y][x].get();
    if (!cell)
        return false;

    CellType type = cell->getType();

    // Power can propagate through:
    // 1. Power lines
    // 2. Power lines over roads
    // 3. Power plants
    // 4. Connected powered zones
    switch (type)
    {
    case CellType::POWERLINE:
    case CellType::POWERLINE_ROAD:
    case CellType::POWERPLANT:
        return true;

    case CellType::RESIDENTIAL:
    case CellType::COMMERCIAL:
    case CellType::INDUSTRIAL:
        // Zones can conduct power if they're populated
        return cell->getPopulation() > 0;

    default:
        return false;
    }
}

bool PowerGrid::isValidPosition(int x, int y) const
{
    return x >= 0 && x < width && y >= 0 && y < height;
}

void PowerGrid::addPowerSource(PowerSource *source)
{
    if (source)
    {
        source->activate();
        powerSources.push_back(source);
    }
}

void PowerGrid::clearPowerSources()
{
    for (auto *source : powerSources)
    {
        delete source;
    }
    powerSources.clear();
}

bool PowerGrid::hasPowerAt(int x, int y) const
{
    if (!isValidPosition(x, y))
        return false;
    return powerMap[y][x];
}

bool PowerGrid::isPowered(const Cell *cell) const
{
    if (!cell)
        return false;

    // Get cell position
    auto [x, y] = cell->getPosition();

    // Check if any adjacent cell has power
    const int dx[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    const int dy[] = {-1, 0, 1, -1, 1, -1, 0, 1};

    for (int dir = 0; dir < 8; ++dir)
    {
        int newX = x + dx[dir];
        int newY = y + dy[dir];

        if (hasPowerAt(newX, newY))
        {
            return true;
        }
    }

    return false;
}

int PowerGrid::getTotalPowerSources() const
{
    return powerSources.size();
}

int PowerGrid::getPoweredCellCount() const
{
    int count = 0;
    for (const auto &row : powerMap)
    {
        count += std::count(row.begin(), row.end(), true);
    }
    return count;
}