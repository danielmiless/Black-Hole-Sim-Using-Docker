/**
 * @file Config.cpp
 * @brief Implementation of the JSON-based configuration system
 */

#include "Config.h"
#include "Logger.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <GLFW/glfw3.h>

Config::Config() {
    // Initialize with empty JSON object
    m_json = nlohmann::json::object();
}

Config::Config(const std::string& filename) : Config() {
    loadFromFile(filename);
}

bool Config::loadFromFile(const std::string& filename) {
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            Logger::getInstance().log(Logger::Level::WARNING, 
                "Could not open config file: " + filename);
            return false;
        }
        
        file >> m_json;
        m_filename = filename;
        
        Logger::getInstance().log(Logger::Level::INFO, 
            "Configuration loaded from: " + filename);
        return true;
        
    } catch (const nlohmann::json::exception& e) {
        Logger::getInstance().log(Logger::Level::ERROR, 
            "JSON parsing error in " + filename + ": " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        Logger::getInstance().log(Logger::Level::ERROR, 
            "Error loading config from " + filename + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToFile(const std::string& filename) const {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            Logger::getInstance().log(Logger::Level::ERROR, 
                "Could not open file for writing: " + filename);
            return false;
        }
        
        file << std::setw(4) << m_json << std::endl;
        
        Logger::getInstance().log(Logger::Level::INFO, 
            "Configuration saved to: " + filename);
        return true;
        
    } catch (const std::exception& e) {
        Logger::getInstance().log(Logger::Level::ERROR, 
            "Error saving config to " + filename + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromString(const std::string& jsonString) {
    try {
        m_json = nlohmann::json::parse(jsonString);
        Logger::getInstance().log(Logger::Level::DEBUG, 
            "Configuration loaded from string");
        return true;
        
    } catch (const nlohmann::json::exception& e) {
        Logger::getInstance().log(Logger::Level::ERROR, 
            "JSON parsing error: " + std::string(e.what()));
        return false;
    }
}

std::string Config::getString(const std::string& key, const std::string& defaultValue) const {
    const nlohmann::json* value = getNestedValue(key);
    
    if (value && value->is_string()) {
        return value->get<std::string>();
    }
    
    return defaultValue;
}

int Config::getInt(const std::string& key, int defaultValue) const {
    const nlohmann::json* value = getNestedValue(key);
    
    if (value && value->is_number_integer()) {
        return value->get<int>();
    }
    
    return defaultValue;
}

float Config::getFloat(const std::string& key, float defaultValue) const {
    const nlohmann::json* value = getNestedValue(key);
    
    if (value && value->is_number()) {
        return value->get<float>();
    }
    
    return defaultValue;
}

double Config::getDouble(const std::string& key, double defaultValue) const {
    const nlohmann::json* value = getNestedValue(key);
    
    if (value && value->is_number()) {
        return value->get<double>();
    }
    
    return defaultValue;
}

bool Config::getBool(const std::string& key, bool defaultValue) const {
    const nlohmann::json* value = getNestedValue(key);
    
    if (value && value->is_boolean()) {
        return value->get<bool>();
    }
    
    return defaultValue;
}

std::vector<int> Config::getIntArray(const std::string& key, const std::vector<int>& defaultValue) const {
    const nlohmann::json* value = getNestedValue(key);
    
    if (value && value->is_array()) {
        std::vector<int> result;
        for (const auto& item : *value) {
            if (item.is_number_integer()) {
                result.push_back(item.get<int>());
            }
        }
        return result;
    }
    
    return defaultValue;
}

std::vector<float> Config::getFloatArray(const std::string& key, const std::vector<float>& defaultValue) const {
    const nlohmann::json* value = getNestedValue(key);
    
    if (value && value->is_array()) {
        std::vector<float> result;
        for (const auto& item : *value) {
            if (item.is_number()) {
                result.push_back(item.get<float>());
            }
        }
        return result;
    }
    
    return defaultValue;
}

std::vector<double> Config::getDoubleArray(const std::string& key, const std::vector<double>& defaultValue) const {
    const nlohmann::json* value = getNestedValue(key);
    
    if (value && value->is_array()) {
        std::vector<double> result;
        for (const auto& item : *value) {
            if (item.is_number()) {
                result.push_back(item.get<double>());
            }
        }
        return result;
    }
    
    return defaultValue;
}

void Config::setString(const std::string& key, const std::string& value) {
    setNestedValue(key, nlohmann::json(value));
}

void Config::setInt(const std::string& key, int value) {
    setNestedValue(key, nlohmann::json(value));
}

void Config::setFloat(const std::string& key, float value) {
    setNestedValue(key, nlohmann::json(value));
}

void Config::setBool(const std::string& key, bool value) {
    setNestedValue(key, nlohmann::json(value));
}

bool Config::hasKey(const std::string& key) const {
    return getNestedValue(key) != nullptr;
}

std::vector<std::string> Config::getAllKeys() const {
    std::vector<std::string> keys;
    
    // This is a simplified implementation
    // A full implementation would recursively traverse the JSON structure
    for (auto it = m_json.begin(); it != m_json.end(); ++it) {
        keys.push_back(it.key());
    }
    
    return keys;
}

void Config::clear() {
    m_json = nlohmann::json::object();
    m_filename.clear();
    Logger::getInstance().log(Logger::Level::DEBUG, "Configuration cleared");
}

std::string Config::toString() const {
    return m_json.dump(4);  // Pretty print with 4-space indentation
}

const nlohmann::json* Config::getNestedValue(const std::string& key) const {
    std::vector<std::string> keyParts = splitKey(key);
    const nlohmann::json* current = &m_json;
    
    for (const std::string& part : keyParts) {
        if (!current->is_object() || !current->contains(part)) {
            return nullptr;
        }
        current = &(*current)[part];
    }
    
    return current;
}

void Config::setNestedValue(const std::string& key, const nlohmann::json& value) {
    std::vector<std::string> keyParts = splitKey(key);
    nlohmann::json* current = &m_json;
    
    // Navigate to the parent of the target key
    for (size_t i = 0; i < keyParts.size() - 1; ++i) {
        const std::string& part = keyParts[i];
        
        if (!current->is_object()) {
            *current = nlohmann::json::object();
        }
        
        if (!current->contains(part)) {
            (*current)[part] = nlohmann::json::object();
        }
        
        current = &(*current)[part];
    }
    
    // Set the final value
    if (!current->is_object()) {
        *current = nlohmann::json::object();
    }
    
    (*current)[keyParts.back()] = value;
    
    Logger::getInstance().log(Logger::Level::DEBUG, 
        "Configuration key '" + key + "' set");
}

std::vector<std::string> Config::splitKey(const std::string& key) const {
    std::vector<std::string> parts;
    std::stringstream ss(key);
    std::string part;
    
    while (std::getline(ss, part, '.')) {
        if (!part.empty()) {
            parts.push_back(part);
        }
    }
    
    // If no dots found, return the entire key as a single part
    if (parts.empty()) {
        parts.push_back(key);
    }
    
    return parts;
}