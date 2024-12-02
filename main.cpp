// SimCity Project - Team 8
/*
Arnav Verma     - av0967
Donovan Smith   -
Rishi Parekh    -
Aditya Moturi   -

*/

#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include "Region.h"

void clearInputBuffer()
{
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

bool verifyConfigFile(const std::string &filename,
                      std::string &regionFile,
                      int &maxSteps,
                      int &refreshRate)
{
    std::ifstream file(filename);
    if (!file)
    {
        std::cerr << "Error: Cannot open configuration file '" << filename << "'" << std::endl;
        return false;
    }

    // Read and verify region filename
    std::getline(file, regionFile);
    if (regionFile.empty())
    {
        std::cerr << "Error: Region filename cannot be empty" << std::endl;
        return false;
    }

    // Read and verify max steps and refresh rate
    if (!(file >> maxSteps >> refreshRate))
    {
        std::cerr << "Error: Invalid format for max steps or refresh rate" << std::endl;
        return false;
    }

    if (maxSteps <= 0)
    {
        std::cerr << "Error: Maximum time steps must be positive" << std::endl;
        return false;
    }

    if (refreshRate <= 0)
    {
        std::cerr << "Error: Refresh rate must be positive" << std::endl;
        return false;
    }

    if (refreshRate > maxSteps)
    {
        std::cerr << "Error: Refresh rate cannot be larger than maximum time steps" << std::endl;
        return false;
    }

    file.close();
    return true;
}

void runSimulation()
{
    std::string configFilename;
    bool validConfig = false;
    Region region;

    while (!validConfig)
    {
        std::cout << "\nEnter configuration file name (or 'quit' to return to menu): ";
        std::getline(std::cin, configFilename);

        if (configFilename == "quit")
        {
            return;
        }

        std::string regionFilename;
        int maxTimeSteps, refreshRate;

        if (verifyConfigFile(configFilename, regionFilename, maxTimeSteps, refreshRate))
        {
            if (region.loadFromFile(regionFilename))
            {
                // Run simulation
                region.simulate(maxTimeSteps, refreshRate);

                // Area analysis
                bool validArea = false;
                while (!validArea)
                {
                    std::cout << "\nEnter coordinates for area analysis (x1 y1 x2 y2): ";
                    int x1, y1, x2, y2;

                    if (std::cin >> x1 >> y1 >> x2 >> y2)
                    {
                        region.analyzeArea(x1, y1, x2, y2);
                        validArea = true;
                    }
                    else
                    {
                        std::cerr << "Error: Please enter four numbers separated by spaces" << std::endl;
                        clearInputBuffer();
                    }
                }
                clearInputBuffer();
                validConfig = true;
            }
        }

        if (!validConfig)
        {
            std::cout << "Would you like to try again? (y/n): ";
            std::string response;
            std::getline(std::cin, response);
            if (response != "y" && response != "Y")
            {
                return;
            }
        }
    }
}

void displayBanner()
{
    std::cout << "\n";
    std::cout << "+--------------------------------------------+" << std::endl;
    std::cout << "|              SimCity - Group 8             |" << std::endl;
    std::cout << "|                                            |" << std::endl;
    std::cout << "|          City Simulation Project           |" << std::endl;
    std::cout << "|               CSCE 2110.501                |" << std::endl;
    std::cout << "|              ----------------              |" << std::endl;
    std::cout << "|          Arnav Verma, Rishi Parekh         |" << std::endl;
    std::cout << "|         Donovan Smith, Aditya Moturi       | " << std::endl;
    std::cout << "+--------------------------------------------+" << std::endl;
    std::cout << "\n";
}

void displayMenu()
{
    std::cout << "\n=== Main Menu ===" << std::endl;
    std::cout << "1. Run new simulation" << std::endl;
    std::cout << "2. Exit program" << std::endl;
    std::cout << "\nEnter your choice (1-2): ";
}

int main()
{
    displayBanner(); // Show the banner at startup

    std::string input;
    bool running = true;

    while (running)
    {
        displayMenu();
        std::getline(std::cin, input);

        if (input == "1")
        {
            runSimulation();
        }
        else if (input == "2")
        {
            std::cout << "\n+--------------------------------------------+" << std::endl;
            std::cout << "|        Thank you for using SimCity!         |" << std::endl;
            std::cout << "|              See you again!                 |" << std::endl;
            std::cout << "+---------------------------------------------+\n"
                      << std::endl;
            running = false;
        }
        else
        {
            std::cout << "Invalid choice. Please enter 1 or 2." << std::endl;
        }
    }

    return 0;
}