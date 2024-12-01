#ifndef CONFIG_MANAGER_HPP
#define CONFIG_MANAGER_HPP

#include <string>
#include <vector>
#include <memory>
#include "zones/Cell.hpp"

class ConfigManager {
private:
    std::string regionFile;
    int maxTimeSteps;
    int refreshRate;
    std::vector<std::vector<CellType>> initialLayout;

public:
    ConfigManager() = default;
    
    // Read and parse configuration file
    bool readConfigFile(const std::string& filename);
    
    // Load region layout from file
    bool loadRegionLayout();
    
    // Getters
    std::string getRegionFile() const { return regionFile; }
    int getMaxTimeSteps() const { return maxTimeSteps; }
    int getRefreshRate() const { return refreshRate; }
    const std::vector<std::vector<CellType>>& getInitialLayout() const { return initialLayout; }

private:
    // Helper functions
    bool parseConfigLine(const std::string& line);
    CellType charToCell(char c);
    bool validateConfig() const;
};

#endif // CONFIG_MANAGER_HPP