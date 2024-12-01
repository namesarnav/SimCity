#ifndef REGION_HPP
#define REGION_HPP

#include <vector>
#include <memory>
#include <string>
#include <map>
#include <stdexcept>
#include "zones/Cell.hpp"
#include "power/PowerSystem.hpp"
#include "zones/residential/ResidentialManager.hpp"
#include "zones/commercial/CommercialManager.hpp"
#include "zones/industrial/IndustrialManager.hpp"
#include "pollution/PollutionSystem.hpp"

class Region {
private:
    // Grid and basic properties
    std::vector<std::vector<std::unique_ptr<Cell>>> grid;
    int width;
    int height;
    int timeStep;
    int availableWorkers;
    int availableGoods;

    // State tracking
    struct StateSnapshot {
        int totalResidential;
        int totalCommercial;
        int totalIndustrial;
        int totalPollution;
        double workerUtilization;
        double goodsUtilization;
        bool hasChanged;
    };
    StateSnapshot lastState;
    std::map<int, StateSnapshot> historicalStates;

    // System managers
    std::unique_ptr<PowerSystem> powerSystem;
    std::unique_ptr<ResidentialManager> residentialManager;
    std::unique_ptr<CommercialManager> commercialManager;
    std::unique_ptr<IndustrialManager> industrialManager;
    std::unique_ptr<PollutionSystem> pollutionSystem;

public:
    // Constructor & Destructor
    Region(const std::vector<std::vector<CellType>>& layout);
    ~Region() = default;

    // Grid management
    Cell* getCellAt(int x, int y);
    const Cell* getCellAt(int x, int y) const;
    std::vector<Cell*> getAdjacentCells(int x, int y);

    // Simulation control
    void updateState();
    bool hasChanged() const;
    void calculateResources();
    void reset();
    
    // Display methods
    void displayState() const;
    void displayPollution() const;
    void displayPowerMap() const;
    void displayGrowthStats() const;
    void displayZoneDistribution() const;
    void displayResourceUtilization() const;

    // Population statistics
    int getTotalResidentialPopulation() const;
    int getTotalIndustrialPopulation() const;
    int getTotalCommercialPopulation() const;
    int getTotalPopulation() const;
    
    // Population density
    double getPopulationDensity() const;
    double getResidentialDensity() const;
    double getIndustrialDensity() const;
    double getCommercialDensity() const;
    
    // Growth rates
    double getResidentialGrowthRate(int steps = 1) const;
    double getIndustrialGrowthRate(int steps = 1) const;
    double getCommercialGrowthRate(int steps = 1) const;
    double getOverallGrowthRate(int steps = 1) const;
    
    // Zone ratios and balance
    double getResidentialRatio() const;
    double getIndustrialRatio() const;
    double getCommercialRatio() const;
    bool isBalanced() const;

    // Resource management
    int getAvailableWorkers() const { return availableWorkers; }
    int getAvailableGoods() const { return availableGoods; }
    double getWorkerUtilization() const;
    double getGoodsUtilization() const;
    void redistributeResources();

    // Pollution management
    int getTotalPollution() const;
    double getAveragePollution() const;
    int getPollutionAt(int x, int y) const;
    std::pair<int, int> getHighestPollutionLocation() const;
    double getPollutionImpact() const;

    // Power management
    bool hasPowerAt(int x, int y) const;
    int getTotalPoweredCells() const;
    double getPowerCoverage() const;

    // Area analysis
    void analyzeArea(int x1, int y1, int x2, int y2) const;
    void analyzeAreaGrowth(int x1, int y1, int x2, int y2) const;
    void analyzePollutionImpact(int x1, int y1, int x2, int y2) const;
    void analyzeResourceDistribution(int x1, int y1, int x2, int y2) const;

    // State inspection
    int getTimeStep() const { return timeStep; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    bool isValidPosition(int x, int y) const;
    StateSnapshot getCurrentState() const { return lastState; }
    StateSnapshot getHistoricalState(int pastTimeStep) const;

    // Validation
    bool validateState() const;
    bool validateResources() const;
    bool validateConnectivity() const;

private:
    // Internal helper methods
    void initializeGrid(const std::vector<std::vector<CellType>>& layout);
    Cell* createCell(CellType type, int x, int y);
    void updatePowerState();
    void updateZones();
    void updatePollution();
    void takeStateSnapshot();
    double calculateGrowthRate(int currentValue, int pastValue) const;
    int countZoneType(CellType type) const;
    void validateLayout(const std::vector<std::vector<CellType>>& layout) const;
    bool checkResourceBalance() const;
    void updateHistoricalStates();
    void calculateStateChange();
    bool isPowerConnected(int x, int y) const;
};

#endif // REGION_HPP