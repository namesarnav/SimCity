#ifndef INDUSTRIAL_MANAGER_HPP
#define INDUSTRIAL_MANAGER_HPP
#endif
#include "IndustrialZone.hpp"
#include <vector>
#include <memory>

class IndustrialManager
{
private:
    std::vector<IndustrialZone *> zones;
    int totalPopulation;
    int totalWorkers;
    int totalGoods;
    int totalPollution;

public:
    IndustrialManager();
    ~IndustrialManager() = default;

    // Zone management
    void addZone(IndustrialZone *zone);
    void updateZones(const std::vector<std::vector<std::unique_ptr<Cell>>> &grid);

    // Worker management
    bool assignAvailableWorkers(int availableWorkers);
    void releaseAllWorkers();

    // Goods management
    int collectAllGoods(); // Returns total goods produced this turn

    // Pollution management
    void updatePollution();
    int getTotalPollution() const { return totalPollution; }

    // Statistics
    int getTotalPopulation() const { return totalPopulation; }
    int getTotalWorkers() const { return totalWorkers; }
    int getTotalGoods() const { return totalGoods; }

public:
    bool needsWorkers() const;

private:
    void updateStatistics();
    void sortZonesByPriority();
};