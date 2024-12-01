#include "zones/residential/ResidentialManager.hpp"
#include <algorithm>

ResidentialManager::ResidentialManager()
    : totalPopulation(0), availableWorkers(0), hasChanged(false) {
}

void ResidentialManager::addZone(ResidentialZone* zone) {
    if (zone) {
        zones.push_back(zone);
    }
}

void ResidentialManager::updateZones(const std::vector<std::vector<std::unique_ptr<Cell>>>& grid) {
    int oldPopulation = totalPopulation;
    hasChanged = false;

    for (ResidentialZone* zone : zones) {
        if (!zone) continue;

        // Get adjacent cells for this zone
        auto [x, y] = zone->getPosition();
        std::vector<Cell*> adjacentCells;

        // Check all 8 adjacent cells
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx == 0 && dy == 0) continue;

                int newX = x + dx;
                int newY = y + dy;

                if (newY >= 0 && newY < grid.size() &&
                    newX >= 0 && newX < grid[newY].size()) {
                    adjacentCells.push_back(grid[newY][newX].get());
                }
            }
        }

        // Update zone population
        zone->updatePopulation(adjacentCells);
    }

    // Update statistics
    updatePopulationStats();
    
    // Check if population changed
    hasChanged = (oldPopulation != totalPopulation);
}

void ResidentialManager::updatePopulationStats() {
    totalPopulation = 0;
    for (const ResidentialZone* zone : zones) {
        if (zone) {
            totalPopulation += zone->getPopulation();
        }
    }
    availableWorkers = totalPopulation;  // Each resident can be a worker
}

int ResidentialManager::getPopulationInArea(int x1, int y1, int x2, int y2) const {
    // Ensure coordinates are in correct order
    if (x1 > x2) std::swap(x1, x2);
    if (y1 > y2) std::swap(y1, y2);
    
    int areaPopulation = 0;
    for (const ResidentialZone* zone : zones) {
        if (!zone) continue;
        
        auto [x, y] = zone->getPosition();
        if (x >= x1 && x <= x2 && y >= y1 && y <= y2) {
            areaPopulation += zone->getPopulation();
        }
    }
    
    return areaPopulation;
}

std::vector<ResidentialZone*> ResidentialManager::getActiveZones() const {
    std::vector<ResidentialZone*> activeZones;
    for (ResidentialZone* zone : zones) {
        if (zone && zone->wasActive()) {
            activeZones.push_back(zone);
        }
    }
    return activeZones;
}