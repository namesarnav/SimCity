#ifndef RESIDENTIAL_ZONE_HPP
#define RESIDENTIAL_ZONE_HPP

#include "../Cell.hpp"
#include <vector>
#include <string>

class ResidentialZone : public Cell {
private:
    bool wasActiveLastTurn;
    
    // Growth condition checkers
    bool canGrowToOne(const std::vector<Cell*>& adjacentCells) const;
    bool canGrowToTwo(const std::vector<Cell*>& adjacentCells) const;
    bool canGrowToThree(const std::vector<Cell*>& adjacentCells) const;
    bool canGrowToFour(const std::vector<Cell*>& adjacentCells) const;
    bool canGrowToFive(const std::vector<Cell*>& adjacentCells) const;
    
    // Helper functions
    int countAdjacentPopulation(const std::vector<Cell*>& adjacentCells, int minPopulation) const;
    bool hasAdjacentPower(const std::vector<Cell*>& adjacentCells) const;

public:
    ResidentialZone(int x = 0, int y = 0);
    virtual ~ResidentialZone() = default;
    
    // Required override from Cell
    void updatePopulation(const std::vector<Cell*>& adjacentCells) override;
    
    // Status check
    bool wasActive() const { return wasActiveLastTurn; }
    
    // Debug/Display
    std::string getStatusReport() const;
};

#endif // RESIDENTIAL_ZONE_HPP