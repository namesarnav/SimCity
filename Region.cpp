#include "Region.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

Region::Region() : width(0), height(0), availableWorkers(0), availableGoods(0), changed(false) {}

bool Region::loadFromFile(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file)
    {
        std::cerr << "Error: Cannot open region file: " << filename << std::endl;
        return false;
    }

    try
    {
        std::string line;
        // Read dimensions
        if (!std::getline(file, line))
        {
            throw std::runtime_error("Cannot read dimensions");
        }

        std::stringstream ss(line);
        char comma;
        ss >> height >> comma >> width;

        if (ss.fail() || comma != ',' || height <= 0 || width <= 0)
        {
            throw std::runtime_error("Invalid dimensions format");
        }

        // Initialize grid
        grid.resize(height, std::vector<Cell>(width));

        // Read each line of the grid
        for (int y = 0; y < height; y++)
        {
            if (!std::getline(file, line))
            {
                throw std::runtime_error("Unexpected end of file at line " + std::to_string(y));
            }

            std::stringstream lineStream(line);
            std::string cell;
            int x = 0;

            while (std::getline(lineStream, cell, ','))
            {
                if (x >= width)
                {
                    throw std::runtime_error("Too many columns in row " + std::to_string(y));
                }

                if (cell.empty())
                {
                    throw std::runtime_error("Empty cell at position " + std::to_string(x) + "," + std::to_string(y));
                }

                char type = cell[0];
                if (type != 'R' && type != 'I' && type != 'C' &&
                    type != '-' && type != 'T' && type != '#' && type != 'P')
                {
                    throw std::runtime_error("Invalid cell type '" + std::string(1, type) +
                                             "' at position " + std::to_string(x) + "," + std::to_string(y));
                }

                grid[y][x].setType(type);
                grid[y][x].setPopulation(0);
                grid[y][x].setPollution(0);
                x++;
            }

            if (x < width)
            {
                throw std::runtime_error("Not enough columns in row " + std::to_string(y));
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error loading region file: " << e.what() << std::endl;
        return false;
    }

    return true;
}

void Region::displayState() const
{
    std::cout << "\nRegion State:" << std::endl;
    std::cout << "  ";
    // Column numbers
    for (int x = 0; x < width; x++)
    {
        std::cout << x % 10 << " ";
    }
    std::cout << "\n";

    for (int y = 0; y < height; y++)
    {
        std::cout << y % 10 << " "; // Row numbers
        for (int x = 0; x < width; x++)
        {
            const Cell &cell = grid[y][x];
            if ((cell.getType() == 'R' || cell.getType() == 'I' || cell.getType() == 'C') && cell.getPopulation() > 0)
            {
                std::cout << cell.getPopulation() << " ";
            }
            else
            {
                std::cout << cell.getType() << " ";
            }
        }
        std::cout << std::endl;
    }

    // Display resources
    std::cout << "\nResources:";
    std::cout << "\n- Available Workers: " << availableWorkers;
    std::cout << "\n- Available Goods: " << availableGoods << std::endl;

    // Display totals
    int resPop = ResidentialSystem::getTotalPopulation(grid);
    int indPop = IndustrialSystem::getTotalPopulation(grid);
    int comPop = CommercialSystem::getTotalPopulation(grid);

    std::cout << "\nPopulation:";
    std::cout << "\n- Residential: " << resPop;
    std::cout << "\n- Industrial: " << indPop;
    std::cout << "\n- Commercial: " << comPop;
    std::cout << "\n- Total: " << (resPop + indPop + comPop) << std::endl;
}


void Region::updateResources()
{
    availableWorkers = ResidentialSystem::getTotalPopulation(grid);
    availableGoods = IndustrialSystem::getTotalPopulation(grid);
}

bool Region::hasChanges(const std::vector<std::vector<Cell>> &oldGrid) const
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (grid[y][x].getPopulation() != oldGrid[y][x].getPopulation() ||
                grid[y][x].getPollution() != oldGrid[y][x].getPollution())
            {
                return true;
            }
        }
    }
    return false;
}

void Region::performTimeStep()
{
    auto previousState = grid;
    updateResources();

    // Update in priority order according to project requirements
    CommercialSystem::update(grid, availableWorkers, availableGoods);
    IndustrialSystem::update(grid, availableWorkers, availableGoods);
    ResidentialSystem::update(grid);

    // Update pollution last
    IndustrialSystem::updatePollution(grid);

    changed = hasChanges(previousState);
}

void Region::simulate(int maxTimeSteps, int refreshRate)
{
    std::cout << "\nInitial state:" << std::endl;
    displayState();

    int timeStep = 0;
    bool hasChanged = true;

    while (timeStep < maxTimeSteps && hasChanged)
    {
        auto previousState = grid;
        updateResources();

        // Update zones in priority order
        CommercialSystem::update(grid, availableWorkers, availableGoods);
        IndustrialSystem::update(grid, availableWorkers, availableGoods);
        ResidentialSystem::update(grid);
        IndustrialSystem::updatePollution(grid);

        // Check for changes
        hasChanged = false;
        for (size_t y = 0; y < grid.size() && !hasChanged; y++)
        {
            for (size_t x = 0; x < grid[0].size() && !hasChanged; x++)
            {
                if (grid[y][x].getPopulation() != previousState[y][x].getPopulation() ||
                    grid[y][x].getPollution() != previousState[y][x].getPollution())
                {
                    hasChanged = true;
                }
            }
        }

        if (timeStep % refreshRate == 0 || !hasChanged)
        {
            std::cout << "\nTime step: " << timeStep << std::endl;
            displayState();
        }

        timeStep++;
    }

    std::cout << "\nSimulation ended after " << timeStep << " steps";
    if (!hasChanged)
        std::cout << " (no further changes possible)";
    std::cout << std::endl;
    displayFinalStats();
}

void Region::analyzeArea(int x1, int y1, int x2, int y2)
{
    while (true)
    {
        // Bounds checking
        if (x1 < 0 || x2 >= width || y1 < 0 || y2 >= height)
        {
            std::cout << "Coordinates must be within (0,0) to (" << (width - 1) << "," << (height - 1) << ")\n";
            std::cout << "Enter new coordinates (x1 y1 x2 y2): ";
            if (!(std::cin >> x1 >> y1 >> x2 >> y2))
            {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                continue;
            }
            std::cin.ignore(10000, '\n');
            continue;
        }

        // Swap if coordinates are reversed
        if (x1 > x2)
            std::swap(x1, x2);
        if (y1 > y2)
            std::swap(y1, y2);

        // Calculate area statistics
        int resPop = 0, indPop = 0, comPop = 0, pollution = 0;
        for (int y = y1; y <= y2; y++)
        {
            for (int x = x1; x <= x2; x++)
            {
                const Cell &cell = grid[y][x];
                switch (cell.getType())
                {
                case 'R':
                    resPop += cell.getPopulation();
                    break;
                case 'I':
                    indPop += cell.getPopulation();
                    break;
                case 'C':
                    comPop += cell.getPopulation();
                    break;
                }
                pollution += cell.getPollution();
            }
        }

        // Display results
        std::cout << "\nArea Analysis (" << x1 << "," << y1 << ") to (" << x2 << "," << y2 << "):\n";
        std::cout << "Area size: " << (x2 - x1 + 1) << "x" << (y2 - y1 + 1) << std::endl;
        std::cout << "Residential Population: " << resPop << std::endl;
        std::cout << "Industrial Population: " << indPop << std::endl;
        std::cout << "Commercial Population: " << comPop << std::endl;
        std::cout << "Total Population: " << (resPop + indPop + comPop) << std::endl;
        std::cout << "Total Pollution: " << pollution << std::endl;
        return;
    }
}

void Region::displayFinalStats() const
{
    int resPop = ResidentialSystem::getTotalPopulation(grid);
    int indPop = IndustrialSystem::getTotalPopulation(grid);
    int comPop = CommercialSystem::getTotalPopulation(grid);
    int totalPollution = Statistics::getTotalPollution(grid);

    std::cout << "\nFinal Statistics:" << std::endl;
    std::cout << "Residential Population: " << resPop << std::endl;
    std::cout << "Industrial Population: " << indPop << std::endl;
    std::cout << "Commercial Population: " << comPop << std::endl;
    std::cout << "Total Population: " << (resPop + indPop + comPop) << std::endl;
    std::cout << "Total Pollution: " << totalPollution << std::endl;
}