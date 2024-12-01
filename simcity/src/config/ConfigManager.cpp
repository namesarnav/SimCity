#include "config/ConfigManager.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>

bool ConfigManager::readConfigFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    // Read region file path
    if (!std::getline(file, regionFile)) {
        return false;
    }

    // Read max time steps
    if (!std::getline(file, line)) {
        return false;
    }
    try {
        maxTimeSteps = std::stoi(line);
    } catch (const std::invalid_argument&) {
        return false;
    }

    // Read refresh rate
    if (!std::getline(file, line)) {
        return false;
    }
    try {
        refreshRate = std::stoi(line);
    } catch (const std::invalid_argument&) {
        return false;
    }

    return validateConfig();
}

bool ConfigManager::loadRegionLayout() {
    std::ifstream file(regionFile);
    if (!file.is_open()) {
        return false;
    }

    initialLayout.clear();
    std::string line;
    while (std::getline(file, line)) {
        std::vector<CellType> row;
        std::stringstream ss(line);
        char cell;
        while (ss >> cell) {
            if (cell != ',') {
                row.push_back(charToCell(cell));
            }
        }
        if (!row.empty()) {
            initialLayout.push_back(row);
        }
    }

    return !initialLayout.empty();
}

CellType ConfigManager::charToCell(char c) {
    switch (c) {
        case 'R': return CellType::RESIDENTIAL;
        case 'C': return CellType::COMMERCIAL;
        case 'I': return CellType::INDUSTRIAL;
        case '-': return CellType::ROAD;
        case 'T': return CellType::POWERLINE;
        case '#': return CellType::POWERLINE_ROAD;
        case 'P': return CellType::POWERPLANT;
        default: return CellType::EMPTY;
    }
}

bool ConfigManager::validateConfig() const {
    return !regionFile.empty() && maxTimeSteps > 0 && refreshRate > 0;
}