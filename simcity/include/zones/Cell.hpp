#ifndef CELL_HPP
#define CELL_HPP

#include <vector>

enum class CellType {
    RESIDENTIAL,
    COMMERCIAL,
    INDUSTRIAL,
    ROAD,
    POWERLINE,
    POWERLINE_ROAD,
    POWERPLANT,
    EMPTY
};

class Cell {
protected:
    CellType type;
    int population;
    int pollution;
    bool hasPower;
    int x;
    int y;

public:
    Cell(CellType type = CellType::EMPTY, int x = 0, int y = 0);
    virtual ~Cell() = default;

    // Basic getters and setters
    CellType getType() const { return type; }
    int getPopulation() const { return population; }
    int getPollution() const { return pollution; }
    bool getPowerStatus() const { return hasPower; }
    
    // Virtual functions for cell behavior
    virtual void updatePopulation(const std::vector<Cell*>& adjacentCells) = 0;
    virtual void updatePollution(int newPollution) { pollution = newPollution; }
    virtual void setPowerStatus(bool status) { hasPower = status; }

    // Utility functions
    bool isAdjacent(const Cell& other) const;
    std::pair<int, int> getPosition() const { return {x, y}; }
};

#endif // CELL_HPP