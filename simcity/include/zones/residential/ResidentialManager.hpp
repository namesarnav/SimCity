#ifndef RESIDENTIAL_MANAGER_HPP
#define RESIDENTIAL_MANAGER_HPP

#include "ResidentialZone.hpp"
#include <vector>
#include <memory>

class ResidentialManager {
private:
    std::vector<ResidentialZone*> zones;
    int totalPopulation;
    int availableWorkers;
    bool hasChanged;

    void updatePopulationStats();

public:
    ResidentialManager();
    ~ResidentialManager() = default;

    // Zone management
    void addZone(ResidentialZone* zone);
    void updateZones(const std::vector<std::vector<std::unique_ptr<Cell>>>& grid);

    // Statistics
    int getTotalPopulation() const { return totalPopulation; }
    int getAvailableWorkers() const { return availableWorkers; }
    bool hasPopulationChanged() const { return hasChanged; }

    // Analysis
    int getPopulationInArea(int x1, int y1, int x2, int y2) const;
    std::vector<ResidentialZone*> getActiveZones() const;
};

#endif // RESIDENTIAL_MANAGER_HPP