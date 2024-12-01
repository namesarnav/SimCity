#ifndef POWER_GRID_HPP
#define POWER_GRID_HPP

#include <vector>
#include <queue>
#include <memory>
#include "../zones/Cell.hpp"
#include "PowerSource.hpp"

class PowerGrid {
private:
    std::vector<std::vector<bool>> powerMap;
    std::vector<PowerSource*> powerSources;
    const std::vector<std::vector<std::unique_ptr<Cell>>>* currentGrid = nullptr;
    int width;
    int height;

    // Helper functions for power propagation
    void resetPowerMap();
    void propagatePower(int x, int y);
    bool canPropagateTo(int x, int y) const;
    bool isValidPosition(int x, int y) const;

public:
    PowerGrid(int width, int height);
    ~PowerGrid() = default;

    // Core functionality
    void update(const std::vector<std::vector<std::unique_ptr<Cell>>>& grid);
    void addPowerSource(PowerSource* source);
    void clearPowerSources();
    
    // Power status checking
    bool hasPowerAt(int x, int y) const;
    bool isPowered(const Cell* cell) const;
    
    // Analysis
    int getTotalPowerSources() const;
    int getPoweredCellCount() const;
};

#endif // POWER_GRID_HPP