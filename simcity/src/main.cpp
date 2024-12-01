// main.cpp
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "config/ConfigManager.hpp"
#include "Region.hpp"

class SimCity
{
private:
    std::unique_ptr<ConfigManager> configManager;
    std::unique_ptr<Region> region;
    bool isRunning;

    bool readConfig()
    {
        std::string configFile;
        std::cout << "Enter configuration file path: ";
        std::getline(std::cin, configFile);

        configManager = std::make_unique<ConfigManager>();
        if (!configManager->readConfigFile(configFile))
        {
            std::cerr << "Error: Failed to read configuration file." << std::endl;
            return false;
        }
        return true;
    }

    bool initializeSimulation()
    {
        try
        {
            auto layout = configManager->loadRegionLayout();
            region = std::make_unique<Region>(layout);
            isRunning = true;
            return true;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error initializing simulation: " << e.what() << std::endl;
            return false;
        }
    }

    void displaySimulationState()
    {
        std::cout << "\nTime Step: " << region->getTimeStep() << std::endl;
        std::cout << "Available Workers: " << region->getAvailableWorkers() << std::endl;
        std::cout << "Available Goods: " << region->getAvailableGoods() << std::endl;
        std::cout << "\nCurrent Region State:" << std::endl;
        region->displayState();
    }

    void handleAreaAnalysis()
    {
        int x1, y1, x2, y2;
        bool validInput = false;

        while (!validInput)
        {
            std::cout << "\nEnter coordinates for area analysis (x1 y1 x2 y2): ";
            if (!(std::cin >> x1 >> y1 >> x2 >> y2))
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid input. Please enter four numbers." << std::endl;
                continue;
            }

            if (x1 < 0 || x1 >= region->getWidth() ||
                y1 < 0 || y1 >= region->getHeight() ||
                x2 < 0 || x2 >= region->getWidth() ||
                y2 < 0 || y2 >= region->getHeight())
            {
                std::cout << "Coordinates out of bounds. Please try again." << std::endl;
                continue;
            }

            validInput = true;
        }

        region->analyzeArea(x1, y1, x2, y2);
    }

public:
    SimCity() : isRunning(false) {}

    bool initialize()
    {
        return readConfig() && initializeSimulation();
    }

    void run()
    {
        if (!isRunning)
        {
            std::cerr << "Error: Simulation not properly initialized." << std::endl;
            return;
        }

        int currentStep = 0;
        int maxSteps = configManager->getMaxTimeSteps();
        int refreshRate = configManager->getRefreshRate();
        bool changed = true;

        // Display initial state
        std::cout << "\nInitial Region State:" << std::endl;
        region->displayState();

        // Main simulation loop
        while (currentStep < maxSteps && changed)
        {
            region->updateState();
            currentStep++;

            if (currentStep % refreshRate == 0)
            {
                displaySimulationState();
            }

            changed = region->hasChanged();
        }

        // Display final state and statistics
        std::cout << "\nFinal Region State:" << std::endl;
        displaySimulationState();

        std::cout << "\nFinal Statistics:" << std::endl;
        std::cout << "Total Residential Population: " << region->getTotalResidentialPopulation() << std::endl;
        std::cout << "Total Industrial Population: " << region->getTotalIndustrialPopulation() << std::endl;
        std::cout << "Total Commercial Population: " << region->getTotalCommercialPopulation() << std::endl;
        std::cout << "Total Pollution: " << region->getTotalPollution() << std::endl;

        // Display pollution map
        std::cout << "\nPollution Map:" << std::endl;
        region->displayPollution();

        // Handle area analysis
        handleAreaAnalysis();
    }
};

int main()
{
    try
    {
        SimCity simulation;

        std::cout << "SimCity Simulation" << std::endl;
        std::cout << "=================" << std::endl;

        if (!simulation.initialize())
        {
            std::cerr << "Failed to initialize simulation. Exiting." << std::endl;
            return 1;
        }

        simulation.run();

        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}