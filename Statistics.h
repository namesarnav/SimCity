// Statistics.h


#ifndef STATISTICS_H
#define STATISTICS_H

#include <vector>
#include "Cell.h"

class Statistics
{
public:
    static int getTotalPopulation(const std::vector<std::vector<Cell>> &grid, char type);
    static int getAreaPopulation(const std::vector<std::vector<Cell>> &grid,
                                 int x1, int y1, int x2, int y2, char type);
    static int getTotalPollution(const std::vector<std::vector<Cell>> &grid);
    static int getAreaPollution(const std::vector<std::vector<Cell>> &grid,
                                int x1, int y1, int x2, int y2);

private:
    static bool isValidCoordinates(const std::vector<std::vector<Cell>> &grid,
                                   int x1, int y1, int x2, int y2);
};
#endif