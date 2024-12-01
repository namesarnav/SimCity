#ifndef ERROR_HANDLER_HPP
#define ERROR_HANDLER_HPP

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "SimCityException.hpp"

class ErrorHandler {
private:
    static std::vector<std::string> errorLog;
    static std::ofstream logFile;
    static const std::string LOG_FILENAME;

public:
    static void initialize();
    static void cleanup();
    
    static void logError(const std::string& error);
    static void logError(const SimCityException& e);
    static void displayErrors();
    static void clearErrors();
    
    static bool hasErrors();
    static const std::vector<std::string>& getErrorLog();
    
private:
    static void openLogFile();
    static void writeToLogFile(const std::string& message);
};

#endif // ERROR_HANDLER_HPP
