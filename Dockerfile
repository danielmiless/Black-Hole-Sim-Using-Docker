# Multi-stage build for optimized container
FROM ubuntu:22.04 AS builder

# Prevent interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    wget \
    pkg-config \
    # OpenGL and graphics libraries
    mesa-common-dev \
    libgl1-mesa-dev \
    libgl1-mesa-dri \
    libglu1-mesa-dev \
    libegl1-mesa \
    libegl1-mesa-dev \
    libgbm1 \
    libgbm-dev \
    # GLFW dependencies
    libglfw3-dev \
    libglfw3 \
    # GLEW dependencies
    libglew-dev \
    libglew2.2 \
    # GLM dependencies
    libglm-dev \
    # JSON parsing library
    nlohmann-json3-dev \
    # Additional utilities
    nano \
    && rm -rf /var/lib/apt/lists/*

# Set up working directory
WORKDIR /app

# Copy source files
COPY . .

# Create build directory and build the application
RUN mkdir -p build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make -j$(nproc)

# Runtime stage - smaller final image
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# Install only runtime dependencies
RUN apt-get update && apt-get install -y \
    mesa-common-dev \
    libgl1-mesa-dev \
    libgl1-mesa-dri \
    libglu1-mesa-dev \
    libegl1-mesa \
    libgbm1 \
    libglfw3 \
    libglew2.2 \
    # X11 and display dependencies
    xvfb \
    x11vnc \
    fluxbox \
    nano \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy built application and assets from builder stage
COPY --from=builder /app/build/black_hole_3d /app/
COPY --from=builder /app/shaders/ /app/shaders/
COPY --from=builder /app/config/ /app/config/

# Set up X11 display
ENV DISPLAY=:1

# Create startup script with improved graphics handling
RUN echo '#!/bin/bash \n\
# Set up software rendering for compatibility\n\
export LIBGL_ALWAYS_SOFTWARE=1\n\
export GALLIUM_DRIVER=llvmpipe\n\
\n\
# Start virtual display with GLX support\n\
Xvfb :1 -screen 0 1920x1080x24 -ac +extension GLX +render -noreset &\n\
sleep 3\n\
\n\
# Start window manager\n\
fluxbox -display :1 &\n\
sleep 2\n\
\n\
# Start VNC server for remote viewing\n\
x11vnc -display :1 -nopw -listen 0.0.0.0 -xkb -forever &\n\
sleep 1\n\
\n\
# Run the black hole simulation\n\
echo "Starting 3D Black Hole Simulation..."\n\
echo "Connect via VNC to localhost:5900 to view"\n\
cd /app && ./black_hole_3d\n\
' > /app/start.sh && chmod +x /app/start.sh

EXPOSE 5900

CMD ["/app/start.sh"]
