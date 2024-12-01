#ifndef POLLUTION_GRID_HPP
#define POLLUTION_GRID_HPP
#endif
#include <vector>
#include <memory>
#include "../zones/Cell.hpp"

class PollutionGrid
{
private:
    std::vector<std::vector<int>> pollutionLevels;
    int width;
    int height;

    // Helper functions
    bool isValidPosition(int x, int y) const;
    void initializeGrid(int width, int height);
    void normalizeValues();

public:
    PollutionGrid(int width, int height);
    ~PollutionGrid() = default;

    // Core functionality
    void addPollution(int x, int y, int amount);
    void spreadPollution();
    void reset();

    // Getters
    int getPollutionAt(int x, int y) const;
    int getTotalPollution() const;

    // Analysis
    int getAreaPollution(int x1, int y1, int x2, int y2) const;
    std::pair<int, int> findHighestPollution() const;
};
