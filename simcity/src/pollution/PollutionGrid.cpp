#include "PollutionGrid.hpp"
#include <algorithm>
#include <cmath>

PollutionGrid::PollutionGrid(int width, int height)
    : width(width), height(height) {
    initializeGrid(width, height);
}

void PollutionGrid::initializeGrid(int width, int height) {
    pollutionLevels.resize(height);
    for (auto& row : pollutionLevels) {
        row.resize(width, 0);
    }
}

bool PollutionGrid::isValidPosition(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height;
}

void PollutionGrid::addPollution(int x, int y, int amount) {
    if (!isValidPosition(x, y) || amount <= 0) return;
    pollutionLevels[y][x] += amount;
}

void PollutionGrid::spreadPollution() {
    std::vector<std::vector<int>> newLevels = pollutionLevels;
    
    // Spread patterns for all 8 directions
    const int dx[] = {-1, -1, -1,  0, 0,  1, 1, 1};
    const int dy[] = {-1,  0,  1, -1, 1, -1, 0, 1};
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (pollutionLevels[y][x] <= 0) continue;
            
            // Calculate spread amount (decreases by 1 per cell distance)
            int sourceAmount = pollutionLevels[y][x];
            
            // Spread to adjacent cells
            for (int dir = 0; dir < 8; ++dir) {
                int newX = x + dx[dir];
                int newY = y + dy[dir];
                
                if (isValidPosition(newX, newY)) {
                    // Calculate pollution spread based on distance
                    int spreadAmount = std::max(0, sourceAmount - 1);
                    newLevels[newY][newX] += spreadAmount;
                }
            }
        }
    }
    
    pollutionLevels = std::move(newLevels);
    normalizeValues();
}

void PollutionGrid::normalizeValues() {
    for (auto& row : pollutionLevels) {
        for (int& value : row) {
            value = std::max(0, value);
        }
    }
}

void PollutionGrid::reset() {
    for (auto& row : pollutionLevels) {
        std::fill(row.begin(), row.end(), 0);
    }
}

int PollutionGrid::getPollutionAt(int x, int y) const {
    if (!isValidPosition(x, y)) return 0;
    return pollutionLevels[y][x];
}

int PollutionGrid::getTotalPollution() const {
    int total = 0;
    for (const auto& row : pollutionLevels) {
        for (int value : row) {
            total += value;
        }
    }
    return total;
}

int PollutionGrid::getAreaPollution(int x1, int y1, int x2, int y2) const {
    // Ensure coordinates are in correct order
    if (x1 > x2) std::swap(x1, x2);
    if (y1 > y2) std::swap(y1, y2);
    
    // Bound checking
    x1 = std::max(0, x1);
    y1 = std::max(0, y1);
    x2 = std::min(width - 1, x2);
    y2 = std::min(height - 1, y2);
    
    int total = 0;
    for (int y = y1; y <= y2; ++y) {
        for (int x = x1; x <= x2; ++x) {
            total += pollutionLevels[y][x];
        }
    }
    return total;
}

std::pair<int, int> PollutionGrid::findHighestPollution() const {
    int maxPollution = -1;
    std::pair<int, int> location = {-1, -1};
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (pollutionLevels[y][x] > maxPollution) {
                maxPollution = pollutionLevels[y][x];
                location = {x, y};
            }
        }
    }
    
    return location;
}