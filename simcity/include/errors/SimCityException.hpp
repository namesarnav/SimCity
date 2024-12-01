#ifndef SIMCITY_EXCEPTION_HPP
#define SIMCITY_EXCEPTION_HPP

#include <stdexcept>
#include <string>

// Base exception class for all SimCity exceptions
class SimCityException : public std::runtime_error {
public:
    explicit SimCityException(const std::string& message) 
        : std::runtime_error(message) {}
};

// File-related exceptions
class FileException : public SimCityException {
public:
    explicit FileException(const std::string& message) 
        : SimCityException("File Error: " + message) {}
};

// Configuration exceptions
class ConfigException : public SimCityException {
public:
    explicit ConfigException(const std::string& message) 
        : SimCityException("Configuration Error: " + message) {}
};

// Grid-related exceptions
class GridException : public SimCityException {
public:
    explicit GridException(const std::string& message) 
        : SimCityException("Grid Error: " + message) {}
};

// Zone-related exceptions
class ZoneException : public SimCityException {
public:
    explicit ZoneException(const std::string& message) 
        : SimCityException("Zone Error: " + message) {}
};

// Power system exceptions
class PowerException : public SimCityException {
public:
    explicit PowerException(const std::string& message) 
        : SimCityException("Power System Error: " + message) {}
};

// Pollution system exceptions
class PollutionException : public SimCityException {
public:
    explicit PollutionException(const std::string& message) 
        : SimCityException("Pollution System Error: " + message) {}
};

#endif // SIMCITY_EXCEPTION_HPP