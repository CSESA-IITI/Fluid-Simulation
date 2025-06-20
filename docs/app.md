# App Module Documentation

## Overview

The `app` module contains the application entry point and is responsible for initializing the renderer, setting up the fluid simulation, and running the main loop.

---

## Main Flow (`main.cpp`)

1. **Renderer Initialization:**  
   Creates a `Triton::Luna` renderer instance.

2. **Simulation Setup:**  
   Instantiates `Core::FluidSim` with the renderer's Vulkan context.

3. **Scene Configuration:**  
   Adds the simulation's quad object to the renderer.

4. **Pipeline Initialization:**  
   Prepares descriptor image infos and initializes the rendering pipeline.

5. **Main Loop:**  
   Starts the render loop, calling `sim.drawFrame(currentFrame)` each frame to advance the simulation and render.

---

## See Also

- [Core Module Documentation](core.md)
