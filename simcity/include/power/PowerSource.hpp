#ifndef POWER_SOURCE_HPP
#define POWER_SOURCE_HPP

#include <utility>

class PowerSource
{
private:
    int x;
    int y;
    bool active;

public:
    PowerSource(int x, int y);
    ~PowerSource() = default;

    // Getters
    int getX() const { return x; }
    int getY() const { return y; }
    bool isActive() const { return active; }
    std::pair<int, int> getPosition() const { return {x, y}; }

    // Status management
    void activate() { active = true; }
    void deactivate() { active = false; }
};

#endif // POWER_SOURCE_HPP