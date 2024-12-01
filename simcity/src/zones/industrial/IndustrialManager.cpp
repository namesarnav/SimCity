#include "IndustrialManager.hpp"
#include <algorithm>

IndustrialManager::IndustrialManager()
    : totalPopulation(0), totalWorkers(0), totalGoods(0), totalPollution(0)
{
}

void IndustrialManager::addZone(IndustrialZone *zone)
{
    if (zone)
    {
        zones.push_back(zone);
    }
}
bool IndustrialManager::needsWorkers() const
{
    for (const IndustrialZone *zone : zones)
    {
        if (!zone)
            continue;
        // Each population level needs 2 workers
        int workersNeeded = zone->getPopulation() * 2;
        if (zone->getAssignedWorkers() < workersNeeded)
        {
            return true;
        }
    }
    return false;
}

void IndustrialManager::updateZones(const std::vector<std::vector<std::unique_ptr<Cell>>> &grid)
{
    for (IndustrialZone *zone : zones)
    {
        if (!zone)
            continue;

        auto [x, y] = zone->getPosition();
        std::vector<Cell *> adjacentCells;

        // Get adjacent cells
        for (int dy = -1; dy <= 1; ++dy)
        {
            for (int dx = -1; dx <= 1; ++dx)
            {
                if (dx == 0 && dy == 0)
                    continue;

                int newX = x + dx;
                int newY = y + dy;

                if (newY >= 0 && newY < grid.size() &&
                    newX >= 0 && newX < grid[newY].size())
                {
                    adjacentCells.push_back(grid[newY][newX].get());
                }
            }
        }

        zone->updatePopulation(adjacentCells);
    }

    updateStatistics();
    updatePollution();
}

bool IndustrialManager::assignAvailableWorkers(int availableWorkers)
{
    if (availableWorkers <= 0)
        return false;

    sortZonesByPriority();
    bool workersAssigned = false;

    for (IndustrialZone *zone : zones)
    {
        if (!zone || availableWorkers <= 0)
            break;

        // Try to assign workers to this zone
        int workersNeeded = zone->getPopulation() * 2 - zone->getAssignedWorkers();
        if (workersNeeded > 0)
        {
            int workersToAssign = std::min(availableWorkers, workersNeeded);
            if (zone->assignWorkers(workersToAssign))
            {
                availableWorkers -= workersToAssign;
                workersAssigned = true;
            }
        }
    }

    updateStatistics();
    return workersAssigned;
}

void IndustrialManager::releaseAllWorkers()
{
    for (IndustrialZone *zone : zones)
    {
        if (zone)
        {
            zone->releaseWorkers();
        }
    }
    totalWorkers = 0;
}

int IndustrialManager::collectAllGoods()
{
    totalGoods = 0;
    for (IndustrialZone *zone : zones)
    {
        if (zone)
        {
            totalGoods += zone->collectGoods();
        }
    }
    return totalGoods;
}

void IndustrialManager::updatePollution()
{
    totalPollution = 0;
    for (IndustrialZone *zone : zones)
    {
        if (zone)
        {
            totalPollution += zone->generatePollution();
        }
    }
}

void IndustrialManager::updateStatistics()
{
    totalPopulation = 0;
    totalWorkers = 0;

    for (const IndustrialZone *zone : zones)
    {
        if (zone)
        {
            totalPopulation += zone->getPopulation();
            totalWorkers += zone->getAssignedWorkers();
        }
    }
}

void IndustrialManager::sortZonesByPriority()
{
    // Sort zones based on project requirements:
    // 1. Larger population first
    // 2. Greater total adjacent population
    // 3. Smaller Y coordinate
    // 4. Smaller X coordinate
    std::sort(zones.begin(), zones.end(),
              [](const IndustrialZone *a, const IndustrialZone *b)
              {
                  if (!a || !b)
                      return false;

                  // Compare populations
                  if (a->getPopulation() != b->getPopulation())
                  {
                      return a->getPopulation() > b->getPopulation();
                  }

                  // Compare positions (Y coordinate first, then X)
                  auto [ax, ay] = a->getPosition();
                  auto [bx, by] = b->getPosition();

                  if (ay != by)
                  {
                      return ay < by; // Smaller Y has priority
                  }

                  return ax < bx; // Smaller X has priority
              });
}