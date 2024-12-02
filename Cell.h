// Cell.h
#ifndef CELL_H
#define CELL_H

class Cell {
private:
    char type;        // R, I, C, -, T, #, or P
    int population;   // Current population of the cell
    int pollution;    // Current pollution level

public:
    Cell();
    
    // Getters and setters
    char getType() const;
    int getPopulation() const;
    int getPollution() const;
    
    void setType(char t);
    void setPopulation(int pop);
    void setPollution(int pol);
};

#endif

