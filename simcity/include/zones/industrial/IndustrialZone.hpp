#ifndef INDUSTRIAL_ZONE_HPP
#define INDUSTRIAL_ZONE_HPP
#endif
#include "../Cell.hpp"
#include <vector>
#include <string>

enum class ProductionState {
    INACTIVE,           // No workers assigned
    ACTIVE,            // Workers assigned and producing
    POWER_LOST,        // Workers assigned but no power
    UNDERSTAFFED       // Not enough workers for current level
};

class IndustrialZone : public Cell {
private:
    int assignedWorkers;
    int goodsProduced;
    ProductionState state;
    bool wasProducingLastTurn;
    
    // Helper functions for checking growth conditions
    bool canGrowToOne(const std::vector<Cell*>& adjacentCells) const;
    bool canGrowToTwo(const std::vector<Cell*>& adjacentCells) const;
    bool canGrowToThree(const std::vector<Cell*>& adjacentCells) const;
    
    // Population and adjacency checks
    int countAdjacentPopulation(const std::vector<Cell*>& adjacentCells, int minPopulation) const;
    bool hasAdjacentPower(const std::vector<Cell*>& adjacentCells) const;
    
    // Production management
    void updateProductionState();
    void handlePowerLoss();
    void calculateGoodsProduction();
    void updatePollution();

public:
    IndustrialZone(int x = 0, int y = 0);
    
    // Override base class virtual functions
    void updatePopulation(const std::vector<Cell*>& adjacentCells) override;
    
    // Worker management
    bool assignWorkers(int count);
    void releaseWorkers();
    
    // Goods and pollution
    int collectGoods();  // Returns and resets goods produced
    int generatePollution() const;  // Calculate pollution based on population
    
    // Getters
    int getAssignedWorkers() const { return assignedWorkers; }
    int getGoodsProduced() const { return goodsProduced; }
    ProductionState getState() const { return state; }
    bool isProducing() const { return state == ProductionState::ACTIVE; }
    
    // Status reporting
    std::string getStatusReport() const;
};
