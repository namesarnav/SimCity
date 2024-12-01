#include "zones/Cell.hpp"
#include <cmath>

Cell::Cell(CellType type, int x, int y)
    : type(type), population(0), pollution(0), hasPower(false), x(x), y(y) {}

bool Cell::isAdjacent(const Cell& other) const {
    int dx = std::abs(x - other.x);
    int dy = std::abs(y - other.y);
    
    // Cells are adjacent if they share an edge or corner
    return (dx <= 1 && dy <= 1) && !(dx == 0 && dy == 0);
}