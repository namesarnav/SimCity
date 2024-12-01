#include "PollutionSystem.hpp"

PollutionSystem::PollutionSystem(int width, int height)
    : grid(new PollutionGrid(width, height)) {
}

PollutionSystem::~PollutionSystem() {
    delete grid;
}

void PollutionSystem::update(const std::vector<std::vector<std::unique_ptr<Cell>>>& gameGrid) {
    // Reset pollution grid for new calculation
    grid->reset();
    pollutionSources.clear();
    
    // Collect pollution from industrial zones
    for (size_t y = 0; y < gameGrid.size(); ++y) {
        for (size_t x = 0; x < gameGrid[y].size(); ++x) {
            const Cell* cell = gameGrid[y][x].get();
            if (!cell) continue;
            
            if (cell->getType() == CellType::INDUSTRIAL) {
                const IndustrialZone* industrialZone = static_cast<const IndustrialZone*>(cell);
                int pollutionAmount = industrialZone->generatePollution();
                if (pollutionAmount > 0) {
                    addPollutionSource(x, y, pollutionAmount);
                }
            }
        }
    }
    
    // Calculate pollution spread
    calculateSpread();
}

void PollutionSystem::addPollutionSource(int x, int y, int amount) {
    pollutionSources[{x, y}] = amount;
    grid->addPollution(x, y, amount);
}

void PollutionSystem::calculateSpread() {
    grid->spreadPollution();
}

int PollutionSystem::getTotalPollution() const {
    return grid->getTotalPollution();
}

int PollutionSystem::getPollutionAt(int x, int y) const {
    return grid->getPollutionAt(x, y);
}

int PollutionSystem::getAreaPollution(int x1, int y1, int x2, int y2) const {
    return grid->getAreaPollution(x1, y1, x2, y2);
}