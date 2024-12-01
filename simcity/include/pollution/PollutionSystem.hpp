#ifndef POLLUTION_SYSTEM_HPP
#define POLLUTION_SYSTEM_HPP
#endif
#include "PollutionGrid.hpp"
#include "../zones/industrial/IndustrialZone.hpp"
#include <unordered_map>
#include <utility>
#include <functional> 

class PollutionSystem
{
private:
    // Hash function for std::pair
    struct PairHash
    {
        template <class T1, class T2>
        std::size_t operator()(const std::pair<T1, T2> &p) const
        {
            return std::hash<T1>{}(p.first) ^ (std::hash<T2>{}(p.second) << 1);
        }
    };

    PollutionGrid *grid;
    std::unordered_map<std::pair<int, int>, int, PairHash> pollutionSources;

public:
    PollutionSystem(int width, int height);
    ~PollutionSystem();

    // Core functionality
    void update(const std::vector<std::vector<std::unique_ptr<Cell>>> &grid);
    void addPollutionSource(int x, int y, int amount);
    void calculateSpread();

    // Analysis
    int getTotalPollution() const;
    int getPollutionAt(int x, int y) const;
    int getAreaPollution(int x1, int y1, int x2, int y2) const;
};