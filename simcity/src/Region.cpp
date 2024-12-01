#include "Region.hpp"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <queue>
#include <set>

// Constructor
Region::Region(const std::vector<std::vector<CellType>> &layout)
    : timeStep(0), availableWorkers(0), availableGoods(0)
{

    validateLayout(layout);

    // Initialize managers before grid
    powerSystem = std::make_unique<PowerSystem>(layout[0].size(), layout.size());
    residentialManager = std::make_unique<ResidentialManager>();
    commercialManager = std::make_unique<CommercialManager>();
    industrialManager = std::make_unique<IndustrialManager>();
    pollutionSystem = std::make_unique<PollutionSystem>(layout[0].size(), layout.size());

    initializeGrid(layout);
    takeStateSnapshot();
}

void Region::validateLayout(const std::vector<std::vector<CellType>> &layout) const
{
    if (layout.empty() || layout[0].empty())
    {
        throw std::invalid_argument("Invalid layout: empty grid");
    }

    size_t firstRowWidth = layout[0].size();
    for (const auto &row : layout)
    {
        if (row.size() != firstRowWidth)
        {
            throw std::invalid_argument("Invalid layout: inconsistent row lengths");
        }
    }

    // Verify at least one power plant exists
    bool hasPowerPlant = false;
    for (const auto &row : layout)
    {
        for (const auto &cell : row)
        {
            if (cell == CellType::POWERPLANT)
            {
                hasPowerPlant = true;
                break;
            }
        }
        if (hasPowerPlant)
            break;
    }
    if (!hasPowerPlant)
    {
        throw std::invalid_argument("Invalid layout: no power plant found");
    }
}

void Region::initializeGrid(const std::vector<std::vector<CellType>> &layout)
{
    height = layout.size();
    width = layout[0].size();

    grid.resize(height);
    for (int y = 0; y < height; ++y)
    {
        grid[y].resize(width);
        for (int x = 0; x < width; ++x)
        {
            grid[y][x] = std::unique_ptr<Cell>(createCell(layout[y][x], x, y));

            // Add cells to appropriate managers
            if (auto *cell = grid[y][x].get())
            {
                switch (cell->getType())
                {
                case CellType::RESIDENTIAL:
                    residentialManager->addZone(static_cast<ResidentialZone *>(cell));
                    break;
                case CellType::COMMERCIAL:
                    commercialManager->addZone(static_cast<CommercialZone *>(cell));
                    break;
                case CellType::INDUSTRIAL:
                    industrialManager->addZone(static_cast<IndustrialZone *>(cell));
                    break;
                default:
                    break;
                }
            }
        }
    }
}

Cell *Region::createCell(CellType type, int x, int y)
{
    switch (type)
    {
    case CellType::RESIDENTIAL:
        return new ResidentialZone(x, y);
    case CellType::COMMERCIAL:
        return new CommercialZone(x, y);
    case CellType::INDUSTRIAL:
        return new IndustrialZone(x, y);
    case CellType::ROAD:
    case CellType::POWERLINE:
    case CellType::POWERLINE_ROAD:
    case CellType::POWERPLANT:
    case CellType::EMPTY:
        return new Cell(type, x, y);
    default:
        return new Cell(CellType::EMPTY, x, y);
    }
}

// State Management
void Region::updateState()
{
    takeStateSnapshot();
    updatePowerState();
    updateZones();
    updatePollution();
    calculateResources();
    updateHistoricalStates();
    calculateStateChange();
    timeStep++;
}

void Region::takeStateSnapshot()
{
    lastState = {
        getTotalResidentialPopulation(),
        getTotalCommercialPopulation(),
        getTotalIndustrialPopulation(),
        getTotalPollution(),
        getWorkerUtilization(),
        getGoodsUtilization(),
        false // hasChanged will be calculated later
    };
}

void Region::updateHistoricalStates()
{
    historicalStates[timeStep] = lastState;
}

void Region::calculateStateChange()
{
    if (timeStep == 0)
    {
        lastState.hasChanged = true;
        return;
    }

    auto &currentState = lastState;
    auto &previousState = historicalStates[timeStep - 1];

    lastState.hasChanged =
        currentState.totalResidential != previousState.totalResidential ||
        currentState.totalCommercial != previousState.totalCommercial ||
        currentState.totalIndustrial != previousState.totalIndustrial ||
        currentState.totalPollution != previousState.totalPollution ||
        std::abs(currentState.workerUtilization - previousState.workerUtilization) > 0.01 ||
        std::abs(currentState.goodsUtilization - previousState.goodsUtilization) > 0.01;
}

bool Region::hasChanged() const
{
    return lastState.hasChanged;
}

// Resource Management
void Region::calculateResources()
{
    // Calculate available workers
    availableWorkers = residentialManager->getTotalPopulation() -
                       industrialManager->getTotalWorkers() -
                       commercialManager->getTotalWorkers();

    // Calculate available goods
    availableGoods = industrialManager->getTotalGoods() -
                     commercialManager->getTotalGoods();
}

void Region::redistributeResources()
{
    // First, release all workers and goods
    industrialManager->releaseAllWorkers();
    commercialManager->releaseAllWorkers();

    // Recalculate available resources
    availableWorkers = residentialManager->getTotalPopulation();
    availableGoods = industrialManager->collectAllGoods();

    // Redistribute workers and goods according to priority rules
    while (availableWorkers > 0 && (industrialManager->needsWorkers() || commercialManager->needsWorkers()))
    {
        // Commercial zones have priority over industrial
        if (commercialManager->needsWorkers() && availableGoods > 0)
        {
            if (commercialManager->assignWorker())
            {
                availableWorkers--;
                availableGoods--;
            }
        }

        // Then industrial zones
        if (availableWorkers >= 2 && industrialManager->needsWorkers())
        {
            if (industrialManager->assignWorkers(2))
            {
                availableWorkers -= 2;
            }
        }

        // If neither zone could accept workers, break
        if (!commercialManager->needsWorkers() && !industrialManager->needsWorkers())
        {
            break;
        }
    }
}

// Zone Management
void Region::updateZones()
{
    // Update residential zones first
    residentialManager->updateZones(grid);

    // Update industrial zones to produce goods
    industrialManager->updateZones(grid);

    // Update commercial zones last
    commercialManager->updateZones(grid);

    // Redistribute resources efficiently
    redistributeResources();
}

// Power Management
void Region::updatePowerState()
{
    powerSystem->update(grid);
}

bool Region::hasPowerAt(int x, int y) const
{
    return powerSystem->hasPower(getCellAt(x, y));
}

int Region::getTotalPoweredCells() const
{
    return powerSystem->getPoweredCells();
}

double Region::getPowerCoverage() const
{
    int totalZones = countZoneType(CellType::RESIDENTIAL) +
                     countZoneType(CellType::COMMERCIAL) +
                     countZoneType(CellType::INDUSTRIAL);
    return totalZones > 0 ? static_cast<double>(getTotalPoweredCells()) / totalZones : 0.0;
}

// Pollution Management
void Region::updatePollution()
{
    pollutionSystem->update(grid);
}

// Population and Growth Methods
int Region::getTotalResidentialPopulation() const
{
    return residentialManager->getTotalPopulation();
}

int Region::getTotalIndustrialPopulation() const
{
    return industrialManager->getTotalPopulation();
}

int Region::getTotalCommercialPopulation() const
{
    return commercialManager->getTotalPopulation();
}

int Region::getTotalPopulation() const
{
    return getTotalResidentialPopulation() +
           getTotalIndustrialPopulation() +
           getTotalCommercialPopulation();
}

// Display Methods
void Region::displayState() const
{
    std::cout << std::string(width * 4 + 1, '-') << std::endl;
    for (int y = 0; y < height; ++y)
    {
        std::cout << "|";
        for (int x = 0; x < width; ++x)
        {
            const Cell *cell = getCellAt(x, y);
            int pop = cell->getPopulation();

            if (pop > 0)
            {
                std::cout << std::setw(3) << pop << "|";
            }
            else
            {
                char symbol;
                switch (cell->getType())
                {
                case CellType::RESIDENTIAL:
                    symbol = 'R';
                    break;
                case CellType::COMMERCIAL:
                    symbol = 'C';
                    break;
                case CellType::INDUSTRIAL:
                    symbol = 'I';
                    break;
                case CellType::ROAD:
                    symbol = '-';
                    break;
                case CellType::POWERLINE:
                    symbol = 'T';
                    break;
                case CellType::POWERLINE_ROAD:
                    symbol = '#';
                    break;
                case CellType::POWERPLANT:
                    symbol = 'P';
                    break;
                default:
                    symbol = ' ';
                    break;
                }
                std::cout << " " << symbol << " |";
            }
        }
        std::cout << std::endl
                  << std::string(width * 4 + 1, '-') << std::endl;
    }
}

void Region::displayGrowthStats() const
{
    std::cout << "\nGrowth Statistics:" << std::endl;
    std::cout << "==================" << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Residential Growth Rate: " << getResidentialGrowthRate() << "%" << std::endl;
    std::cout << "Industrial Growth Rate: " << getIndustrialGrowthRate() << "%" << std::endl;
    std::cout << "Commercial Growth Rate: " << getCommercialGrowthRate() << "%" << std::endl;
    std::cout << "Overall Growth Rate: " << getOverallGrowthRate() << "%" << std::endl;
    std::cout << "Population Density: " << getPopulationDensity() << " per cell" << std::endl;
}

// Analysis Methods
void Region::analyzeArea(int x1, int y1, int x2, int y2) const
{
    if (x1 > x2)
        std::swap(x1, x2);
    if (y1 > y2)
        std::swap(y1, y2);

    // Validate coordinates
    x1 = std::max(0, std::min(x1, width - 1));
    y1 = std::max(0, std::min(y1, height - 1));
    x2 = std::max(0, std::min(x2, width - 1));
    y2 = std::max(0, std::min(y2, height - 1));

    int resPop = 0, indPop = 0, comPop = 0;
    int pollution = 0;
    int poweredCells = 0;
    int totalCells = (x2 - x1 + 1) * (y2 - y1 + 1);

    for (int y = y1; y <= y2; ++y)
    {
        for (int x = x1; x <= x2; ++x)
        {
            const Cell *cell = getCellAt(x, y);
            if (!cell)
                continue;

            // Population counts
            switch (cell->getType())
            {
            case CellType::RESIDENTIAL:
                resPop += cell->getPopulation();
                break;
            case CellType::INDUSTRIAL:
                indPop += cell->getPopulation();
                break;
            case CellType::COMMERCIAL:
                comPop += cell->getPopulation();
                break;
            default:
                break;
            }

            // Other metrics
            pollution += cell->getPollution();
            if (hasPowerAt(x, y))
                poweredCells++;
        }
    }

    // Display analysis results
    std::cout << "\nArea Analysis (" << x1 << "," << y1 << ") to (" << x2 << "," << y2 << "):" << std::endl;
    std::cout << "===========================================" << std::endl;
    std::cout << "Populations:" << std::endl;
    std::cout << "  Residential: " << resPop << std::endl;
    std::cout << "  Industrial: " << indPop << std::endl;
    std::cout << "  Commercial: " << comPop << std::endl;
    std::cout << "Total Population: " << (resPop + indPop + comPop) << std::endl;
    std::cout << "\nPollution:" << std::endl;
    std::cout << "  Total: " << pollution << std::endl;
    std::cout << "  Average: " << (static_cast<double>(pollution) / totalCells) << " per cell" << std::endl;
    std::cout << "\nPower Coverage: " << (static_cast<double>(poweredCells) / totalCells * 100) << "%" << std::endl;
}

// Utility Methods
bool Region::isValidPosition(int x, int y) const
{
    return x >= 0 && x < width && y >= 0 && y < height;
}

Cell *Region::getCellAt(int x, int y)
{
    if (!isValidPosition(x, y))
        return nullptr;
    return grid[y][x].get();
}

const Cell *Region::getCellAt(int x, int y) const
{
    if (!isValidPosition(x, y))
        return nullptr;
    return grid[y][x].get();
}

int Region::countZoneType(CellType type) const
{
    int count = 0;
    for (const auto &row : grid)
    {
        for (const auto &cell : row)
        {
            if (cell && cell->getType() == type)
            {
                count++;
            }
        }
    }
    return count;
}

bool Region::validateState() const
{
    return validateResources() && validateConnectivity();
}

bool Region::validateResources() const
{
    // Ensure worker count is consistent
    int totalWorkers = getTotalResidentialPopulation();
    int assignedWorkers = industrialManager->getTotalWorkers() +
                          commercialManager->getTotalWorkers();

    if (totalWorkers < assignedWorkers)
    {
        return false;
    }

    // Ensure goods count is consistent
    int totalGoods = industrialManager->getTotalGoods();
    int usedGoods = commercialManager->getTotal;
    int usedGoods = commercialManager->getTotalGoods();

    if (totalGoods < usedGoods)
    {
        return false;
    }

    return true;
}

bool Region::validateConnectivity() const
{
    // Check if all populated zones have power access
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            const Cell *cell = getCellAt(x, y);
            if (!cell)
                continue;

            if (cell->getPopulation() > 0 && !isPowerConnected(x, y))
            {
                return false;
            }
        }
    }
    return true;
}

bool Region::isPowerConnected(int x, int y) const
{
    return powerSystem->hasPower(getCellAt(x, y));
}

std::vector<Cell *> Region::getAdjacentCells(int x, int y)
{
    std::vector<Cell *> adjacent;

    // Check all 8 directions
    for (int dy = -1; dy <= 1; ++dy)
    {
        for (int dx = -1; dx <= 1; ++dx)
        {
            if (dx == 0 && dy == 0)
                continue;

            int newX = x + dx;
            int newY = y + dy;

            if (isValidPosition(newX, newY))
            {
                adjacent.push_back(grid[newY][newX].get());
            }
        }
    }

    return adjacent;
}

double Region::calculateGrowthRate(int currentValue, int pastValue) const
{
    if (pastValue == 0)
        return 0.0;
    return ((static_cast<double>(currentValue) - pastValue) / pastValue) * 100.0;
}

double Region::getResidentialGrowthRate(int steps) const
{
    if (timeStep < steps || steps <= 0)
        return 0.0;
    auto it = historicalStates.find(timeStep - steps);
    if (it == historicalStates.end())
        return 0.0;
    return calculateGrowthRate(getTotalResidentialPopulation(), it->second.totalResidential);
}

double Region::getIndustrialGrowthRate(int steps) const
{
    if (timeStep < steps || steps <= 0)
        return 0.0;
    auto it = historicalStates.find(timeStep - steps);
    if (it == historicalStates.end())
        return 0.0;
    return calculateGrowthRate(getTotalIndustrialPopulation(), it->second.totalIndustrial);
}

double Region::getCommercialGrowthRate(int steps) const
{
    if (timeStep < steps || steps <= 0)
        return 0.0;
    auto it = historicalStates.find(timeStep - steps);
    if (it == historicalStates.end())
        return 0.0;
    return calculateGrowthRate(getTotalCommercialPopulation(), it->second.totalCommercial);
}

double Region::getOverallGrowthRate(int steps) const
{
    if (timeStep < steps || steps <= 0)
        return 0.0;
    auto it = historicalStates.find(timeStep - steps);
    if (it == historicalStates.end())
        return 0.0;

    int currentTotal = getTotalPopulation();
    int pastTotal = it->second.totalResidential +
                    it->second.totalCommercial +
                    it->second.totalIndustrial;

    return calculateGrowthRate(currentTotal, pastTotal);
}

void Region::displayResourceUtilization() const
{
    std::cout << "\nResource Utilization:" << std::endl;
    std::cout << "====================" << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Workers:" << std::endl;
    std::cout << "  Total Available: " << getTotalResidentialPopulation() << std::endl;
    std::cout << "  Currently Working: " << (getTotalResidentialPopulation() - availableWorkers) << std::endl;
    std::cout << "  Utilization: " << (getWorkerUtilization() * 100) << "%" << std::endl;

    std::cout << "\nGoods:" << std::endl;
    std::cout << "  Total Produced: " << industrialManager->getTotalGoods() << std::endl;
    std::cout << "  Currently Used: " << (industrialManager->getTotalGoods() - availableGoods) << std::endl;
    std::cout << "  Utilization: " << (getGoodsUtilization() * 100) << "%" << std::endl;
}

void Region::displayZoneDistribution() const
{
    std::cout << "\nZone Distribution:" << std::endl;
    std::cout << "=================" << std::endl;
    std::cout << std::fixed << std::setprecision(2);

    int totalZones = countZoneType(CellType::RESIDENTIAL) +
                     countZoneType(CellType::COMMERCIAL) +
                     countZoneType(CellType::INDUSTRIAL);

    std::cout << "Residential: " << (getResidentialRatio() * 100) << "% ("
              << countZoneType(CellType::RESIDENTIAL) << " zones)" << std::endl;
    std::cout << "Commercial: " << (getCommercialRatio() * 100) << "% ("
              << countZoneType(CellType::COMMERCIAL) << " zones)" << std::endl;
    std::cout << "Industrial: " << (getIndustrialRatio() * 100) << "% ("
              << countZoneType(CellType::INDUSTRIAL) << " zones)" << std::endl;

    std::cout << "\nZone Balance Status: " << (isBalanced() ? "Balanced" : "Unbalanced") << std::endl;
}

void Region::analyzePollutionImpact(int x1, int y1, int x2, int y2) const
{
    if (x1 > x2)
        std::swap(x1, x2);
    if (y1 > y2)
        std::swap(y1, y2);

    x1 = std::max(0, std::min(x1, width - 1));
    y1 = std::max(0, std::min(y1, height - 1));
    x2 = std::max(0, std::min(x2, width - 1));
    y2 = std::max(0, std::min(y2, height - 1));

    int totalPollution = 0;
    int maxPollution = 0;
    std::pair<int, int> maxPollutionLoc = {-1, -1};
    int affectedCells = 0;
    const int HIGH_POLLUTION_THRESHOLD = 3;

    // Count affected residential zones
    std::vector<std::pair<int, int>> affectedResidential;

    for (int y = y1; y <= y2; ++y)
    {
        for (int x = x1; x <= x2; ++x)
        {
            const Cell *cell = getCellAt(x, y);
            if (!cell)
                continue;

            int pollution = pollutionSystem->getPollutionAt(x, y);
            totalPollution += pollution;

            if (pollution > 0)
            {
                affectedCells++;
                if (pollution > maxPollution)
                {
                    maxPollution = pollution;
                    maxPollutionLoc = {x, y};
                }

                if (cell->getType() == CellType::RESIDENTIAL &&
                    pollution >= HIGH_POLLUTION_THRESHOLD)
                {
                    affectedResidential.push_back({x, y});
                }
            }
        }
    }

    int area = (x2 - x1 + 1) * (y2 - y1 + 1);
    double avgPollution = area > 0 ? static_cast<double>(totalPollution) / area : 0.0;
    double affectedRatio = area > 0 ? static_cast<double>(affectedCells) / area : 0.0;

    // Display analysis
    std::cout << "\nPollution Impact Analysis:" << std::endl;
    std::cout << "========================" << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Average Pollution: " << avgPollution << " per cell" << std::endl;
    std::cout << "Peak Pollution: " << maxPollution;
    if (maxPollutionLoc.first != -1)
    {
        std::cout << " at (" << maxPollutionLoc.first << ","
                  << maxPollutionLoc.second << ")";
    }
    std::cout << std::endl;
    std::cout << "Area Affected: " << (affectedRatio * 100) << "% of selected region" << std::endl;

    if (!affectedResidential.empty())
    {
        std::cout << "\nWarning: " << affectedResidential.size()
                  << " residential zone(s) affected by high pollution:" << std::endl;
        for (const auto &[x, y] : affectedResidential)
        {
            std::cout << "  - Location (" << x << "," << y << "): "
                      << pollutionSystem->getPollutionAt(x, y) << " pollution units" << std::endl;
        }
    }
}

double Region::getWorkerUtilization() const
{
    int totalWorkers = getTotalResidentialPopulation();
    if (totalWorkers == 0)
        return 0.0;
    return static_cast<double>(totalWorkers - availableWorkers) / totalWorkers;
}

double Region::getGoodsUtilization() const
{
    int totalGoods = industrialManager->getTotalGoods();
    if (totalGoods == 0)
        return 0.0;
    return static_cast<double>(totalGoods - availableGoods) / totalGoods;
}

StateSnapshot Region::getHistoricalState(int pastTimeStep) const
{
    auto it = historicalStates.find(pastTimeStep);
    if (it == historicalStates.end())
    {
        throw std::out_of_range("No historical state available for the specified time step");
    }
    return it->second;
}

void Region::reset()
{
    timeStep = 0;
    availableWorkers = 0;
    availableGoods = 0;
    historicalStates.clear();

    // Reset all zones
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            if (auto *cell = getCellAt(x, y))
            {
                cell->updatePollution(0); // Clear pollution
                // Reset population if it's a zone
                if (cell->getType() == CellType::RESIDENTIAL ||
                    cell->getType() == CellType::COMMERCIAL ||
                    cell->getType() == CellType::INDUSTRIAL)
                {
                    // Reset to unpopulated state
                    while (cell->getPopulation() > 0)
                    {
                        cell->updatePopulation(std::vector<Cell *>());
                    }
                }
            }
        }
    }

    // Take fresh snapshot
    takeStateSnapshot();
}