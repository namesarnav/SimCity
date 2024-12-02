// Cell.cpp
#include "Cell.h"

Cell::Cell() : type('-'), population(0), pollution(0) {}

char Cell::getType() const { 
    return type; 
}

int Cell::getPopulation() const { 
    return population; 
}

int Cell::getPollution() const { 
    return pollution; 
}

void Cell::setType(char t) { 
    type = t; 
}

void Cell::setPopulation(int pop) { 
    population = (pop >= 0) ? pop : 0; 
}

void Cell::setPollution(int pol) { 
    pollution = (pol >= 0) ? pol : 0; 
}