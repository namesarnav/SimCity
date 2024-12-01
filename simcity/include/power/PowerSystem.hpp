#ifndef POWER_SYSTEM_HPP
#define POWER_SYSTEM_HPP

#include <memory>
#include <vector>
#include "../zones/Cell.hpp"
#include "PowerGrid.hpp"

class PowerSystem {
private:
    std::unique_ptr<PowerGrid> grid;

public:
    PowerSystem(int width, int height);
    ~PowerSystem() = default;

    // Core functionality
    void update(const std::vector<std::vector<std::unique_ptr<Cell>>>& cityGrid);
    bool hasPower(const Cell* cell) const;
    
    // Analysis
    int getTotalPowerSources() const;
    int getPoweredCells() const;
    void displayPowerMap() const;
};

#endif // POWER_SYSTEM_HPP