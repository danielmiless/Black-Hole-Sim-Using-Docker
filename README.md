# Enhanced 3D Black Hole Simulation

[![Docker](https://img.shields.io/badge/Docker-Ready-blue.svg)](https://docker.com)
[![OpenGL](https://img.shields.io/badge/OpenGL-4.3-green.svg)](https://opengl.org)
[![C++](https://img.shields.io/badge/C++-17-orange.svg)](https://isocpp.org)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

A comprehensive 3D black hole simulation featuring real-time gravitational lensing, GPU-accelerated ray tracing, and interactive visualization. This enhanced version builds upon the original work by [kavan010](https://github.com/kavan010/black_hole) with significant improvements in code organization, performance, and cross-platform compatibility.

## Features

### Core Simulation
- **Real-time 3D black hole visualization** with Schwarzschild metric implementation
- **GPU-accelerated geodesic ray tracing** using OpenGL compute shaders
- **Interactive accretion disk** with temperature-based coloring
- **Gravitational lensing effects** showing light bending around massive objects
- **Multi-object physics simulation** with N-body gravitational interactions

### Enhanced Graphics
- **Adaptive quality rendering** - lower resolution during camera movement for smooth performance
- **Spacetime curvature visualization** with dynamic grid warping
- **Realistic lighting and shading** for celestial objects
- **Anti-aliased rendering** with configurable multisampling

### User Experience
- **Intuitive orbital camera controls** - mouse to orbit, scroll to zoom
- **Real-time parameter adjustment** via keyboard shortcuts
- **Cross-platform Docker support** for Windows, macOS, and Linux
- **Comprehensive logging system** with performance metrics
- **JSON-based configuration** for easy customization

## Quick Start with Docker

### Prerequisites
- **Docker Desktop** installed and running
- **8GB+ RAM** recommended for smooth operation
- **Graphics drivers** updated (for better performance)

### macOS (Apple Silicon/Intel)
```bash
# Clone the repository
git clone <your-repo-url> black-hole-sim
cd black-hole-sim

# Make the run script executable
chmod +x scripts/run-macos.sh

# Install XQuartz (required for X11 forwarding)
brew install --cask xquartz

# Start XQuartz and enable "Allow connections from network clients"
# Then run the simulation
./scripts/run-macos.sh
```

### Windows
```batch
# Clone the repository
git clone <your-repo-url> black-hole-sim
cd black-hole-sim

# Install VcXsrv or Xming for X11 support
# Download from: https://sourceforge.net/projects/vcxsrv/

# Run the simulation
scripts\run-windows.bat
```

### VNC Access (Most Reliable)
For the most reliable cross-platform experience, use VNC:

```bash
# Start the container
docker-compose up -d

# Connect with VNC viewer to localhost:5900
# No password required
```

## Controls

| Input | Action |
|-------|--------|
| **Left Mouse + Drag** | Orbit camera around black hole |
| **Mouse Wheel** | Zoom in/out |
| **G Key** | Toggle gravity simulation on/off |
| **ESC Key** | Exit simulation |
| **R Key** | Reset camera to default position |

## Project Structure

```
black-hole-sim/
├── CMakeLists.txt              # Build configuration
├── Dockerfile                  # Multi-stage Docker build
├── docker-compose.yml          # Easy container management
├── scripts/                    # Platform-specific run scripts
│   ├── run-macos.sh           # macOS with X11 forwarding
│   ├── run-windows.bat        # Windows with VcXsrv
│   └── build.sh               # Local build script
├── src/                        # Modularized source code
│   ├── main.cpp               # Application entry point
│   ├── engine/                # Core engine systems
│   │   ├── Engine.h/.cpp      # Main application engine
│   │   ├── Camera.h/.cpp      # Orbital camera system
│   │   └── Renderer.h/.cpp    # OpenGL rendering
│   ├── physics/               # Physics simulation
│   │   ├── BlackHole.h/.cpp   # Black hole implementation
│   │   └── Physics.h/.cpp     # N-body physics
│   ├── objects/               # Celestial objects
│   │   └── Object.h/.cpp      # Generic space objects
│   └── utils/                 # Utility systems
│       ├── Logger.h/.cpp      # Logging system
│       └── Config.h/.cpp      # Configuration management
├── shaders/                    # OpenGL shaders
│   ├── vertex.vert           # Vertex shader
│   ├── fragment.frag         # Fragment shader
│   ├── grid.vert/.frag       # Spacetime grid rendering
│   └── geodesic.comp         # GPU ray tracing compute shader
└── config/
    └── simulation.json        # Simulation parameters
```

## Configuration

The simulation is highly configurable via `config/simulation.json`. Key settings include:

```json
{
  "blackHole": {
    "mass": 8.54e36,                    // Sagittarius A* mass
    "position": [0.0, 0.0, 0.0]
  },
  "camera": {
    "initialRadius": 6.34194e10,        // Starting distance
    "orbitSpeed": 0.015,                // Mouse sensitivity
    "zoomSpeed": 25e9                   // Scroll sensitivity
  },
  "rendering": {
    "adaptiveQuality": true,            // Lower quality when moving
    "maxGeodesicSteps": 50000,          // Ray tracing precision
    "enableGrid": true                  // Spacetime visualization
  }
}
```

## Scientific Accuracy

This simulation implements:
- **Schwarzschild metric** for spacetime curvature around non-rotating black holes
- **Geodesic equations** integrated using Runge-Kutta methods
- **Event horizon** visualization at r = 2GM/c²
- **Accretion disk physics** with temperature-based emission
- **Gravitational time dilation** effects (planned enhancement)

## Development

### Local Building (Linux/macOS)
```bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get update
sudo apt-get install build-essential cmake libglfw3-dev libglew-dev nlohmann-json3-dev

# Build the project
./scripts/build.sh Release

# Run locally
./build/black_hole_3d
```

### Adding New Features
1. **New physics**: Extend `src/physics/` classes
2. **Rendering enhancements**: Modify shaders in `shaders/`
3. **UI improvements**: Update `src/engine/` systems
4. **Configuration**: Add parameters to `config/simulation.json`

## Troubleshooting

### Docker Issues
- **Container won't start**: Ensure Docker Desktop is running
- **No graphics**: Install X11 forwarding (XQuartz/VcXsrv)
- **Poor performance**: Try VNC mode instead of X11 forwarding

### Graphics Issues
- **Black screen**: Update graphics drivers
- **Slow rendering**: Lower resolution in config or use adaptive quality
- **Shader errors**: Check OpenGL 4.3 support

### Platform-Specific
- **macOS**: Ensure XQuartz "Allow connections" is enabled
- **Windows**: Use VcXsrv with "Disable access control" checked
- **Linux**: May need `xhost +local:docker` for X11 forwarding

## Performance

Typical performance on modern systems:
- **4K rendering**: 30-60 FPS (GPU dependent)
- **1080p rendering**: 60+ FPS on most dedicated GPUs
- **Adaptive quality**: Maintains smooth interaction during camera movement
- **Memory usage**: ~500MB typical, ~1GB maximum

## Contributing

We welcome contributions! Areas of particular interest:
- **Kerr metric** implementation for rotating black holes
- **Relativistic effects** visualization
- **Multi-black hole** systems and mergers
- **Performance optimizations**
- **Cross-platform** improvements

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Original implementation by [kavan010](https://github.com/kavan010/black_hole)
- Schwarzschild metric mathematics from general relativity literature
- OpenGL compute shader techniques from modern graphics programming
- Docker containerization best practices

## References

1. Misner, C. W., Thorne, K. S., & Wheeler, J. A. (1973). *Gravitation*
2. [Event Horizon Telescope Collaboration](https://eventhorizontelescope.org/)
3. [OpenGL Programming Guide](https://opengl.org/documentation/)
4. [Docker Documentation](https://docs.docker.com/)

---

**Experience the warping of spacetime around one of the universe's most extreme objects!**