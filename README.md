# SimCity Simulation Project

## Overview
This project implements a city simulation system that models the growth and development of residential, commercial, and industrial zones over time. The simulation takes into account various factors such as power infrastructure, population dynamics, worker availability, goods production, and pollution spread.

### Key Features
- Dynamic city growth simulation
- Three zone types: Residential (R), Commercial (C), and Industrial (I)
- Infrastructure elements: Roads (-), Power lines (T), Power plants (P)
- Population growth based on adjacency rules and resource availability
- Pollution system that spreads from industrial zones and power plants
- Detailed analysis of city regions and statistics

### Zone Behaviors
- **Residential Zones (R)**: Provide workers for commercial and industrial zones
- **Industrial Zones (I)**: Require workers and produce goods, generate pollution
- **Commercial Zones (C)**: Require both workers and goods to grow
- **Power Infrastructure**: Required for initial zone development

## Project Structure
- `main.cpp` - Program entry point and menu system
- `Region.cpp/h` - Core region management and simulation logic
- `Cell.cpp/h` - Individual cell representation and state
- `ResidentialSystem.cpp/h` - Residential zone growth rules
- `CommercialSystem.cpp/h` - Commercial zone growth rules
- `IndustrialSystem.cpp/h` - Industrial zone management and pollution
- `Statistics.cpp/h` - Analysis and statistics calculation

## Installation

### Prerequisites
- C++ compiler (g++ recommended)
- Make (optional but recommended)

### Building the Project

1. Clone the repository:
```bash
git clone https://github.com/yourusername/simcity-simulation.git
cd simcity-simulation
```

2. Compile using g++:
```bash
g++ -o simcity main.cpp Region.cpp Cell.cpp ResidentialSystem.cpp CommercialSystem.cpp IndustrialSystem.cpp Statistics.cpp
```

Or if you have make installed:
```bash
make
```

## Running the Simulation

1. Prepare your configuration file (e.g., `config.txt`) with the following format:
```
region.csv
50
5
```
Where:
- First line: Region layout file name
- Second line: Maximum time steps
- Third line: Refresh rate

2. Create your region layout file (e.g., `region.csv`) with the city layout:
```
10,10
R,R,R,-,P,-,I,I,C,C
R,R,R,-,T,-,I,I,C,C
...
```

3. Run the program:
```bash
./simcity
```

4. When prompted, enter the configuration file name (e.g., `config.txt`)

5. The simulation will run and display:
- Initial region state
- Development progress at specified refresh rate
- Final statistics and analysis

### Cell Types
- `R` - Residential Zone
- `I` - Industrial Zone
- `C` - Commercial Zone
- `-` - Road
- `T` - Power Line
- `#` - Power Line over Road
- `P` - Power Plant

## Technical Details

### Growth Rules
The simulation follows specific rules for each zone type:

#### Residential Growth
- Population 0→1: Requires power OR 1 adjacent populated cell
- Population 1→2: Requires 2 adjacent cells with pop ≥ 1
- Population 2→3: Requires 4 adjacent cells with pop ≥ 2
- Population 3→4: Requires 6 adjacent cells with pop ≥ 3
- Population 4→5: Requires 8 adjacent cells with pop ≥ 4

#### Industrial Growth
- Requires 2 workers per population level
- Produces goods equal to population
- Generates pollution based on population

#### Commercial Growth
- Requires 1 worker and 1 good per population level
- Prioritized over industrial zones for resource allocation

### Priority System
The simulation implements a priority system for growth:
1. Commercial zones before industrial zones
2. Higher population cells before lower population cells
3. Greater adjacent population before lesser
4. Lower Y coordinate before higher
5. Lower X coordinate before higher

## Contributing
Feel free to submit issues and enhancement requests.

## License
This project is licensed under the MIT License - see the LICENSE file for details.
