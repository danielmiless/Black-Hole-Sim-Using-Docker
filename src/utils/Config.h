/**
 * @file Config.h
 * @brief Configuration management system using JSON
 */

#pragma once

#include <string>
#include <map>
#include <vector>
#include <nlohmann/json.hpp>
#include <GLFW/glfw3.h>

/**
 * @brief Configuration management system
 * 
 * Loads and manages configuration parameters from JSON files.
 * Provides type-safe access to configuration values with default fallbacks.
 */
class Config {
public:
    /**
     * @brief Default constructor with empty configuration
     */
    Config();
    
    /**
     * @brief Constructor that loads from file
     * @param filename Path to JSON configuration file
     */
    explicit Config(const std::string& filename);
    
    /**
     * @brief Load configuration from JSON file
     * @param filename Path to JSON file
     * @return True if loaded successfully, false otherwise
     */
    bool loadFromFile(const std::string& filename);
    
    /**
     * @brief Save current configuration to JSON file
     * @param filename Path to output file
     * @return True if saved successfully, false otherwise
     */
    bool saveToFile(const std::string& filename) const;
    
    /**
     * @brief Load configuration from JSON string
     * @param jsonString JSON configuration as string
     * @return True if parsed successfully, false otherwise
     */
    bool loadFromString(const std::string& jsonString);
    
    /**
     * @brief Get string value
     * @param key Configuration key (supports dot notation like "window.title")
     * @param defaultValue Default value if key not found
     * @return Configuration value or default
     */
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    
    /**
     * @brief Get integer value
     * @param key Configuration key
     * @param defaultValue Default value if key not found
     * @return Configuration value or default
     */
    int getInt(const std::string& key, int defaultValue = 0) const;
    
    /**
     * @brief Get float value
     * @param key Configuration key
     * @param defaultValue Default value if key not found
     * @return Configuration value or default
     */
    float getFloat(const std::string& key, float defaultValue = 0.0f) const;
    
    /**
     * @brief Get double value
     * @param key Configuration key
     * @param defaultValue Default value if key not found
     * @return Configuration value or default
     */
    double getDouble(const std::string& key, double defaultValue = 0.0) const;
    
    /**
     * @brief Get boolean value
     * @param key Configuration key
     * @param defaultValue Default value if key not found
     * @return Configuration value or default
     */
    bool getBool(const std::string& key, bool defaultValue = false) const;
    
    /**
     * @brief Get array of integers
     * @param key Configuration key
     * @param defaultValue Default value if key not found
     * @return Configuration array or default
     */
    std::vector<int> getIntArray(const std::string& key, const std::vector<int>& defaultValue = {}) const;
    
    /**
     * @brief Get array of floats
     * @param key Configuration key
     * @param defaultValue Default value if key not found
     * @return Configuration array or default
     */
    std::vector<float> getFloatArray(const std::string& key, const std::vector<float>& defaultValue = {}) const;
    
    /**
     * @brief Get array of doubles
     * @param key Configuration key
     * @param defaultValue Default value if key not found
     * @return Configuration array or default
     */
    std::vector<double> getDoubleArray(const std::string& key, const std::vector<double>& defaultValue = {}) const;
    
    /**
     * @brief Set string value
     * @param key Configuration key
     * @param value Value to set
     */
    void setString(const std::string& key, const std::string& value);
    
    /**
     * @brief Set integer value
     * @param key Configuration key
     * @param value Value to set
     */
    void setInt(const std::string& key, int value);
    
    /**
     * @brief Set float value
     * @param key Configuration key
     * @param value Value to set
     */
    void setFloat(const std::string& key, float value);
    
    /**
     * @brief Set boolean value
     * @param key Configuration key
     * @param value Value to set
     */
    void setBool(const std::string& key, bool value);
    
    /**
     * @brief Check if a key exists in the configuration
     * @param key Configuration key to check
     * @return True if key exists, false otherwise
     */
    bool hasKey(const std::string& key) const;
    
    /**
     * @brief Get all keys in the configuration
     * @return Vector of all configuration keys
     */
    std::vector<std::string> getAllKeys() const;
    
    /**
     * @brief Get the raw JSON object (for advanced usage)
     * @return Const reference to the JSON object
     */
    const nlohmann::json& getJson() const { return m_json; }
    
    /**
     * @brief Clear all configuration data
     */
    void clear();
    
    /**
     * @brief Get configuration as formatted string
     * @return Pretty-printed JSON string
     */
    std::string toString() const;

private:
    nlohmann::json m_json;          ///< JSON configuration data
    std::string m_filename;         ///< Source filename (if loaded from file)
    
    /**
     * @brief Navigate to nested JSON value using dot notation
     * @param key Key with dot notation (e.g., "window.width")
     * @return Pointer to JSON value or nullptr if not found
     */
    const nlohmann::json* getNestedValue(const std::string& key) const;
    
    /**
     * @brief Set nested JSON value using dot notation
     * @param key Key with dot notation
     * @param value JSON value to set
     */
    void setNestedValue(const std::string& key, const nlohmann::json& value);
    
    /**
     * @brief Split dot-notation key into components
     * @param key Key string
     * @return Vector of key components
     */
    std::vector<std::string> splitKey(const std::string& key) const;
};