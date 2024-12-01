#include "errors/ErrorHandler.hpp"
#include <ctime>
#include <sstream>
#include <iomanip>

std::vector<std::string> ErrorHandler::errorLog;
std::ofstream ErrorHandler::logFile;
const std::string ErrorHandler::LOG_FILENAME = "simcity_error.log";

void ErrorHandler::initialize() {
    openLogFile();
    errorLog.clear();
}

void ErrorHandler::cleanup() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

void ErrorHandler::logError(const std::string& error) {
    errorLog.push_back(error);
    writeToLogFile(error);
    std::cerr << "Error: " << error << std::endl;
}

void ErrorHandler::logError(const SimCityException& e) {
    logError(e.what());
}

void ErrorHandler::displayErrors() {
    if (errorLog.empty()) {
        std::cout << "No errors recorded.\n";
        return;
    }
    
    std::cout << "\nRecorded Errors:\n";
    for (const auto& error : errorLog) {
        std::cout << "- " << error << '\n';
    }
}

void ErrorHandler::clearErrors() {
    errorLog.clear();
}

bool ErrorHandler::hasErrors() {
    return !errorLog.empty();
}

const std::vector<std::string>& ErrorHandler::getErrorLog() {
    return errorLog;
}

void ErrorHandler::openLogFile() {
    logFile.open(LOG_FILENAME, std::ios::app);
    if (!logFile.is_open()) {
        throw FileException("Unable to open error log file: " + LOG_FILENAME);
    }
}

void ErrorHandler::writeToLogFile(const std::string& message) {
    if (!logFile.is_open()) {
        return;
    }
    
    // Get current time
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    
    std::ostringstream timestamp;
    timestamp << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    
    logFile << "[" << timestamp.str() << "] " << message << std::endl;
}
