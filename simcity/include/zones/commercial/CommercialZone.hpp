#ifndef COMMERCIAL_ZONE_HPP
#define COMMERCIAL_ZONE_HPP

#include "../Cell.hpp"
#include <vector>
#include <string>

enum class CommerceState {
    INACTIVE,           // No workers/goods assigned
    ACTIVE,            // Workers and goods assigned
    POWER_LOST,        // No power connection
    UNDERSTAFFED,      // Not enough workers
    NO_GOODS           // No goods available
};

class CommercialZone : public Cell {
private:
    int assignedWorkers;
    int consumedGoods;
    CommerceState state;
    bool wasActiveLastTurn;
    
    // Helper functions
    bool canGrowToOne(const std::vector<Cell*>& adjacentCells) const;
    bool canGrowToTwo(const std::vector<Cell*>& adjacentCells) const;
    bool canGrowToThree(const std::vector<Cell*>& adjacentCells) const;
    
    // Population and adjacency checks
    int countAdjacentPopulation(const std::vector<Cell*>& adjacentCells, int minPopulation) const;
    bool hasAdjacentPower(const std::vector<Cell*>& adjacentCells) const;
    
    // State management
    void updateCommerceState();
    void handlePowerLoss();
    void calculateGoodsConsumption();

public:
    CommercialZone(int x = 0, int y = 0);
    virtual ~CommercialZone() = default;
    
    // Required override from Cell
    void updatePopulation(const std::vector<Cell*>& adjacentCells) override;
    
    // Worker and goods management
    bool assignWorker();
    void releaseWorker();
    bool consumeGood();
    
    // Getters
    int getAssignedWorkers() const { return assignedWorkers; }
    int getConsumedGoods() const { return consumedGoods; }
    CommerceState getState() const { return state; }
    bool isActive() const { return state == CommerceState::ACTIVE; }
    bool wasActive() const { return wasActiveLastTurn; }
    
    // Debug/Display
    std::string getStatusReport() const;
};

#endif // COMMERCIAL_ZONE_HPP