#include "power/PowerSystem.hpp"
#include <iostream>

PowerSystem::PowerSystem(int width, int height)
    : grid(std::make_unique<PowerGrid>(width, height)) {
}

void PowerSystem::update(const std::vector<std::vector<std::unique_ptr<Cell>>>& cityGrid) {
    grid->update(cityGrid);
}

bool PowerSystem::hasPower(const Cell* cell) const {
    return grid->isPowered(cell);
}

int PowerSystem::getTotalPowerSources() const {
    return grid->getTotalPowerSources();
}

int PowerSystem::getPoweredCells() const {
    return grid->getPoweredCellCount();
}

void PowerSystem::displayPowerMap() const {
    int width = 0;  // Get from grid
    int height = 0; // Get from grid
    
    std::cout << "Power Distribution Map:\n";
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (grid->hasPowerAt(x, y)) {
                std::cout << "* ";
            } else {
                std::cout << ". ";
            }
        }
        std::cout << '\n';
    }
}