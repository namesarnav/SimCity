#ifndef REGION_H
#define REGION_H

#include <vector>
#include <string>
#include <memory>
#include "Cell.h"
#include "ResidentialSystem.h"
#include "CommercialSystem.h"
#include "IndustrialSystem.h"
#include "Statistics.h"

class Region
{
private:
    std::vector<std::vector<Cell>> grid;
    int width, height;
    int availableWorkers;
    int availableGoods;
    bool changed; // Track if the region changed during last update

    // Helper functions
    void updateResources();
    bool hasChanges(const std::vector<std::vector<Cell>> &oldGrid) const;
    void performTimeStep();

public:
    Region();
    bool loadFromFile(const std::string &filename);
    void displayState() const;
    void simulate(int maxTimeSteps, int refreshRate);
    void analyzeArea(int x1, int y1, int x2, int y2);
    void displayFinalStats() const;

    // Getters for testing/verification
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getAvailableWorkers() const { return availableWorkers; }
    int getAvailableGoods() const { return availableGoods; }
};

#endif