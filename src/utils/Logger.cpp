/**
 * @file Logger.cpp
 * @brief Implementation of the comprehensive logging system
 */

#include "Logger.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <map>
#include <string>
#include <vector>
#include <GLFW/glfw3.h>

Logger::Logger()
    : m_currentLevel(Level::INFO)
    , m_consoleOutput(true)
    , m_fileOutput(false)
    , m_logFilename("black_hole_simulation.log") {
}

Logger::~Logger() {
    if (m_logFile.is_open()) {
        log(Level::INFO, "Logger shutting down");
        m_logFile.close();
    }
}

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::setLevel(Level level) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_currentLevel = level;
}

void Logger::setConsoleOutput(bool enabled) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_consoleOutput = enabled;
}

void Logger::setFileOutput(bool enabled, const std::string& filename) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_logFile.is_open()) {
        m_logFile.close();
    }
    
    m_fileOutput = enabled;
    
    if (!filename.empty()) {
        m_logFilename = filename;
    }
    
    if (m_fileOutput) {
        m_logFile.open(m_logFilename, std::ios::app);
        if (!m_logFile.is_open()) {
            std::cerr << "Failed to open log file: " << m_logFilename << std::endl;
            m_fileOutput = false;
        } else {
            // Write header to log file
            m_logFile << "\n" << std::string(80, '=') << "\n";
            m_logFile << "Black Hole Simulation Log - " << getCurrentTimestamp() << "\n";
            m_logFile << std::string(80, '=') << "\n";
        }
    }
}

void Logger::log(Level level, const std::string& message) {
    if (level < m_currentLevel) return;
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::string timestamp = getCurrentTimestamp();
    
    if (m_consoleOutput) {
        writeToConsole(level, timestamp, message);
    }
    
    if (m_fileOutput && m_logFile.is_open()) {
        writeToFile(level, timestamp, message);
    }
}

void Logger::debug(const std::string& message) {
    log(Level::DEBUG, message);
}

void Logger::info(const std::string& message) {
    log(Level::INFO, message);
}

void Logger::warning(const std::string& message) {
    log(Level::WARNING, message);
}

void Logger::error(const std::string& message) {
    log(Level::ERROR, message);
}

void Logger::critical(const std::string& message) {
    log(Level::CRITICAL, message);
}

void Logger::startTimer(const std::string& name) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_timers[name] = std::chrono::high_resolution_clock::now();
    debug("Timer '" + name + "' started");
}

void Logger::stopTimer(const std::string& name) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_timers.find(name);
    if (it != m_timers.end()) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - it->second);
        
        double milliseconds = duration.count() / 1000.0;
        
        std::stringstream ss;
        ss << std::fixed << std::setprecision(3);
        ss << "Timer '" << name << "' elapsed: " << milliseconds << "ms";
        
        info(ss.str());
        m_timers.erase(it);
    } else {
        warning("Attempted to stop non-existent timer: " + name);
    }
}

void Logger::flush() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_consoleOutput) {
        std::cout.flush();
        std::cerr.flush();
    }
    
    if (m_fileOutput && m_logFile.is_open()) {
        m_logFile.flush();
    }
}

std::string Logger::levelToString(Level level) const {
    switch (level) {
        case Level::DEBUG:    return "DEBUG";
        case Level::INFO:     return "INFO ";
        case Level::WARNING:  return "WARN ";
        case Level::ERROR:    return "ERROR";
        case Level::CRITICAL: return "CRIT ";
        default:              return "UNKN ";
    }
}

std::string Logger::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    
    return ss.str();
}

void Logger::writeToConsole(Level level, const std::string& timestamp, const std::string& message) {
    std::ostream* stream = &std::cout;
    const char* colorCode = "\033[0m";  // Default (reset)
    
    // Choose output stream and color based on log level
    switch (level) {
        case Level::DEBUG:
            colorCode = "\033[36m";  // Cyan
            break;
        case Level::INFO:
            colorCode = "\033[32m";  // Green
            break;
        case Level::WARNING:
            colorCode = "\033[33m";  // Yellow
            stream = &std::cerr;
            break;
        case Level::ERROR:
            colorCode = "\033[31m";  // Red
            stream = &std::cerr;
            break;
        case Level::CRITICAL:
            colorCode = "\033[35m";  // Magenta
            stream = &std::cerr;
            break;
    }
    
    *stream << colorCode << "[" << timestamp << "] " 
            << levelToString(level) << ": " << message 
            << "\033[0m" << std::endl;
}

void Logger::writeToFile(Level level, const std::string& timestamp, const std::string& message) {
    m_logFile << "[" << timestamp << "] " 
              << levelToString(level) << ": " << message << std::endl;
}