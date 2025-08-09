/**
 * @file Logger.h
 * @brief Comprehensive logging system for the black hole simulation
 */

#pragma once

#include <string>
#include <fstream>
#include <mutex>
#include <chrono>
#include <vector>
#include <map>
#include <GLFW/glfw3.h>

/**
 * @brief Thread-safe singleton logging system
 * 
 * Provides different log levels, file output, and performance tracking.
 * Supports both console and file logging with timestamps and thread safety.
 */
class Logger {
public:
    /**
     * @brief Log message severity levels
     */
    enum class Level {
        DEBUG = 0,      ///< Detailed debug information
        INFO = 1,       ///< General information
        WARNING = 2,    ///< Warning messages
        ERROR = 3,      ///< Error conditions
        CRITICAL = 4    ///< Critical errors
    };
    
    /**
     * @brief Get the singleton instance
     * @return Reference to the logger instance
     */
    static Logger& getInstance();
    
    /**
     * @brief Set the minimum log level
     * @param level Minimum level to log
     */
    void setLevel(Level level);
    
    /**
     * @brief Enable or disable console output
     * @param enabled True to enable console output
     */
    void setConsoleOutput(bool enabled);
    
    /**
     * @brief Enable or disable file output
     * @param enabled True to enable file output
     * @param filename Optional custom log filename
     */
    void setFileOutput(bool enabled, const std::string& filename = "");
    
    /**
     * @brief Log a message
     * @param level Message severity level
     * @param message Message text
     */
    void log(Level level, const std::string& message);
    
    /**
     * @brief Log a debug message (convenience method)
     * @param message Debug message
     */
    void debug(const std::string& message);
    
    /**
     * @brief Log an info message (convenience method)
     * @param message Info message
     */
    void info(const std::string& message);
    
    /**
     * @brief Log a warning message (convenience method)
     * @param message Warning message
     */
    void warning(const std::string& message);
    
    /**
     * @brief Log an error message (convenience method)
     * @param message Error message
     */
    void error(const std::string& message);
    
    /**
     * @brief Log a critical message (convenience method)
     * @param message Critical message
     */
    void critical(const std::string& message);
    
    /**
     * @brief Start a performance timer
     * @param name Timer name
     */
    void startTimer(const std::string& name);
    
    /**
     * @brief Stop a performance timer and log the duration
     * @param name Timer name
     */
    void stopTimer(const std::string& name);
    
    /**
     * @brief Flush all pending log messages
     */
    void flush();
    
    // Delete copy constructor and assignment operator (singleton)
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

private:
    Level m_currentLevel;           ///< Current minimum log level
    bool m_consoleOutput;           ///< Enable console output
    bool m_fileOutput;              ///< Enable file output
    std::string m_logFilename;      ///< Log file name
    std::ofstream m_logFile;        ///< Log file stream
    std::mutex m_mutex;             ///< Thread safety mutex
    
    // Performance timing
    std::map<std::string, std::chrono::high_resolution_clock::time_point> m_timers;
    
    /**
     * @brief Private constructor (singleton)
     */
    Logger();
    
    /**
     * @brief Destructor
     */
    ~Logger();
    
    /**
     * @brief Convert log level to string
     * @param level Log level
     * @return Level as string
     */
    std::string levelToString(Level level) const;
    
    /**
     * @brief Get current timestamp as string
     * @return Formatted timestamp
     */
    std::string getCurrentTimestamp() const;
    
    /**
     * @brief Write message to console
     * @param level Message level
     * @param timestamp Formatted timestamp
     * @param message Message text
     */
    void writeToConsole(Level level, const std::string& timestamp, const std::string& message);
    
    /**
     * @brief Write message to file
     * @param level Message level
     * @param timestamp Formatted timestamp
     * @param message Message text
     */
    void writeToFile(Level level, const std::string& timestamp, const std::string& message);
};