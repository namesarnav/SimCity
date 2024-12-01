#include "CommercialManager.hpp"
#include <algorithm>

CommercialManager::CommercialManager()
    : totalPopulation(0), assignedWorkers(0), assignedGoods(0)
{
}

void CommercialManager::addZone(CommercialZone *zone)
{
    if (zone)
    {
        zones.push_back(zone);
    }
}

void CommercialManager::updateZones(const std::vector<std::vector<std::unique_ptr<Cell>>> &grid)
{
    // Release all resources before updating
    releaseResources();

    for (CommercialZone *zone : zones)
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

        // Update the zone with its adjacent cells
        zone->updatePopulation(adjacentCells);
    }

    updatePopulationStats();
}

bool CommercialManager::assignResources(int availableWorkers, int availableGoods)
{
    // Sort zones by priority before assigning resources
    sortZonesByPriority();

    bool resourcesAssigned = false;

    for (CommercialZone *zone : zones)
    {
        if (!zone)
            continue;

        // Try to assign both a worker and a good
        if (availableWorkers > 0 && availableGoods > 0)
        {
            if (zone->assignWorker())
            {
                availableWorkers--;
                assignedWorkers++;
            }
            if (zone->assignGood())
            {
                availableGoods--;
                assignedGoods++;
            }
            resourcesAssigned = true;
        }
        else
        {
            break; // No more resources to assign
        }
    }

    return resourcesAssigned;
}

void CommercialManager::releaseResources()
{
    assignedWorkers = 0;
    assignedGoods = 0;

    for (CommercialZone *zone : zones)
    {
        if (zone)
        {
            zone->releaseWorker();
            zone->releaseGood();
        }
    }
}

void CommercialManager::updatePopulationStats()
{
    totalPopulation = 0;
    for (const CommercialZone *zone : zones)
    {
        if (zone)
        {
            totalPopulation += zone->getPopulation();
        }
    }
}
bool CommercialManager::needsWorkers() const
{
    for (const CommercialZone *zone : zones)
    {
        if (!zone)
            continue;
        // Each commercial zone needs 1 worker per population level
        if (zone->getAssignedWorkers() < zone->getPopulation() + 1)
        {
            return true;
        }
    }
    return false;
}
void CommercialManager::sortZonesByPriority()
{
    // Sort zones based on project requirements:
    // 1. Larger population first
    // 2. Greater total adjacent population
    // 3. Smaller Y coordinate
    // 4. Smaller X coordinate
    std::sort(zones.begin(), zones.end(),
              [](const CommercialZone *a, const CommercialZone *b)
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