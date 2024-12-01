#ifndef COMMERCIAL_MANAGER_HPP
#define COMMERCIAL_MANAGER_HPP

#include "CommercialZone.hpp"
#include <vector>
#include <memory>

class CommercialManager
{
private:
    std::vector<CommercialZone *> zones;
    int totalPopulation;
    int totalWorkers;
    int totalGoods;
    bool hasChanged;

    void updateStatistics();
    void sortZonesByPriority();

public:
    CommercialManager();
    ~CommercialManager() = default;

    // Zone management
    void addZone(CommercialZone *zone);
    void updateZones(const std::vector<std::vector<std::unique_ptr<Cell>>> &grid);

    // Worker management
    bool assignWorkers(int availableWorkers);
    void releaseAllWorkers();
    bool needsWorkers() const;

    // Goods management
    bool consumeGoods(int availableGoods);
    int getGoodsNeeded() const;

    // Statistics
    int getTotalPopulation() const { return totalPopulation; }
    int getTotalWorkers() const { return totalWorkers; }
    int getTotalGoods() const { return totalGoods; }
    bool hasPopulationChanged() const { return hasChanged; }

    // Analysis
    int getPopulationInArea(int x1, int y1, int x2, int y2) const;
    std::vector<CommercialZone *> getActiveZones() const;
};

#endif // COMMERCIAL_MANAGER_HPP