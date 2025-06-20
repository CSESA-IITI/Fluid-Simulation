# Fluid Simulation

*CSESA* Task Of Graphics Programming And Simulations Division.

Fluid Simulation written in C++ using Compute Shaders for running simulation on GPU using Vulkan Compute API.

---

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Requirements](#requirements)
- [Build Instructions](#build-instructions)
- [Usage](#usage)
- [Code Structure](#code-structure)
- [How It Works](#how-it-works)
- [License](#license)

---

## Overview

This project implements a real-time fluid simulation using Vulkan compute shaders. The simulation leverages GPU acceleration for high performance and interactive visualization.

## Features

- GPU-accelerated fluid simulation using Vulkan Compute API
- Modular C++ codebase
- Interactive controls via mouse input
- Customizable simulation parameters

## Requirements

- C++17 compatible compiler
- Vulkan SDK (`vulkan-sdk` package or [LunarG Vulkan SDK](https://vulkan.lunarg.com/sdk/home))
- GLFW
- GLM
- CMake (recommended)
- Linux (tested), but should be portable

## Build Instructions

1. **Install dependencies:**
   ```bash
   sudo apt update
   sudo apt install vulkan-sdk libglfw3-dev libglm-dev cmake build-essential
   ```

2. **Clone the repository:**
   ```bash
   git clone https://github.com/CSESA-IITI/Fluid-Simulation.git
   cd Fluid-Simulation
   ```

3. **Compile shaders:**
   ```bash
   cd Triton
   ./compile.sh
   cd ..
   ```

4. **Build the project:**
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

## Usage

Run the compiled binary from the `build` directory:

```bash
./FluidSim
```

## Code Structure

- `src/core/core.cpp`  
  Main simulation logic, Vulkan resource management, and compute pipeline setup.
- `src/shaders/`  
  GLSL shader files for vertex, fragment, and compute stages.
- `Triton/compile.sh`  
  Script to compile GLSL shaders to SPIR-V.
- `README.md`  
  Project documentation.

## How It Works

### Initialization

- Vulkan context and device are initialized.
- Compute textures and synchronization objects are created for each frame in flight.
- Compute pipelines and descriptor sets are set up for simulation steps.

### Simulation Loop

- Uniform buffers are updated each frame with simulation parameters and user input (mouse position/force).
- Compute shaders are dispatched in sequence to perform advection, diffusion, pressure calculation, and gradient subtraction.
- Results are written to GPU textures and visualized.

### User Interaction

- Mouse input is captured to apply forces to the fluid.
- Simulation parameters (e.g., viscosity, density) can be adjusted in code.

## License

[MIT](LICENSE)
