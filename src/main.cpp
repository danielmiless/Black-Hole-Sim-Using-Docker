/**
 * @file main.cpp
 * @brief Enhanced 3D Black Hole Simulation - Main Entry Point
 * 
 * This is an improved version of the original black hole simulation with
 * proper modularization, error handling, and Docker compatibility.
 * 
 * Features:
 * - Real-time 3D black hole visualization with gravitational lensing
 * - Interactive camera controls
 * - GPU-accelerated geodesic computation
 * - Configurable simulation parameters
 * - Cross-platform Docker support
 * 
 * @author Enhanced by AI (based on kavan010's original work)
 * @version 2.0.0
 * @date 2025
 */

#include <iostream>
#include <memory>
#include <chrono>
#include <exception>

#include "engine/Engine.h"
#include "utils/Logger.h"
#include "utils/Config.h"

using namespace std;
using namespace std::chrono;

/**
 * @brief Main application entry point
 * 
 * Initializes all systems and runs the main simulation loop.
 * Includes comprehensive error handling and logging.
 */
int main() {
    try {
        // Initialize logging system
        Logger::getInstance().setLevel(Logger::Level::INFO);
        Logger::getInstance().log(Logger::Level::INFO, "🚀 Starting 3D Black Hole Simulation v2.0.0");
        
        // Load configuration
        Config config;
        if (!config.loadFromFile("config/simulation.json")) {
            Logger::getInstance().log(Logger::Level::WARNING, 
                "⚠️  Could not load config file, using defaults");
        }
        
        // Create and initialize the engine
        auto engine = make_unique<Engine>(config);
        Logger::getInstance().log(Logger::Level::INFO, "✅ Engine initialized successfully");
        
        // Performance tracking
        auto startTime = high_resolution_clock::now();
        auto lastFrameTime = startTime;
        size_t frameCount = 0;
        const auto logInterval = seconds(5);
        
        Logger::getInstance().log(Logger::Level::INFO, "🎮 Starting main simulation loop");
        Logger::getInstance().log(Logger::Level::INFO, "🔧 Controls:");
        Logger::getInstance().log(Logger::Level::INFO, "   - Left Mouse: Orbit camera");
        Logger::getInstance().log(Logger::Level::INFO, "   - Scroll: Zoom in/out");
        Logger::getInstance().log(Logger::Level::INFO, "   - G key: Toggle gravity simulation");
        Logger::getInstance().log(Logger::Level::INFO, "   - ESC: Exit simulation");
        
        // Main simulation loop
        while (engine->isRunning()) {
            auto currentTime = high_resolution_clock::now();
            auto deltaTime = duration_cast<microseconds>(currentTime - lastFrameTime).count() / 1000000.0;
            lastFrameTime = currentTime;
            
            // Update simulation
            engine->update(static_cast<float>(deltaTime));
            
            // Render frame
            engine->render();
            
            // Performance logging
            frameCount++;
            auto elapsed = duration_cast<seconds>(currentTime - startTime);
            if (elapsed >= logInterval) {
                double fps = static_cast<double>(frameCount) / elapsed.count();
                Logger::getInstance().log(Logger::Level::INFO, 
                    "📊 Performance: " + to_string(fps) + " FPS, Frame time: " + 
                    to_string(deltaTime * 1000.0) + "ms");
                
                startTime = currentTime;
                frameCount = 0;
            }
        }
        
        Logger::getInstance().log(Logger::Level::INFO, "✅ Simulation ended gracefully");
        
    } catch (const runtime_error& e) {
        Logger::getInstance().log(Logger::Level::ERROR, 
            "💥 Runtime error: " + string(e.what()));
        return EXIT_FAILURE;
        
    } catch (const exception& e) {
        Logger::getInstance().log(Logger::Level::ERROR, 
            "💥 Unexpected error: " + string(e.what()));
        return EXIT_FAILURE;
        
    } catch (...) {
        Logger::getInstance().log(Logger::Level::ERROR, 
            "💥 Unknown error occurred");
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
