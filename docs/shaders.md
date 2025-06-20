# Shader Module Documentation

## Overview

This project uses several GLSL shaders (compiled to SPIR-V) for both compute and rendering tasks. The compute shaders implement the core steps of the fluid simulation, while the vertex and fragment shaders handle visualization.

---

## Shader Files

- [`advect.comp`](../src/shaders/advect.comp): Advection step (moves fluid quantities along the velocity field)
- [`diffuse.comp`](../src/shaders/diffuse.comp): Diffusion step (simulates viscosity)
- [`pressureCalc.comp`](../src/shaders/pressureCalc.comp): Pressure solve (enforces incompressibility)
- [`gradientSub.comp`](../src/shaders/gradientSub.comp): Gradient subtraction (projects velocity field to be divergence-free)
- [`compute.comp`](../src/shaders/compute.comp): General-purpose compute shader (may be used for initialization, clearing, or utility operations)
- [`vertex.vert`](../src/shaders/vertex.vert): Vertex shader for rendering the simulation quad
- [`fragment.frag`](../src/shaders/fragment.frag): Fragment shader for visualizing the simulation output

> **Note:**  
> `.spv` files are compiled SPIR-V binaries generated from the corresponding GLSL source files.

---

## Compute Shaders

### advect.comp

- **Purpose:** Moves fluid properties (velocity, etc.) along the flow using semi-Lagrangian advection.
- **Inputs:**  
  - Uniform buffer: simulation parameters (dimensions, density, viscosity, timestep, mouse input)
  - Images: previous frame, current frame, temporary output
- **Key Operations:**  
  - Computes backtraced positions using velocity field
  - Bilinear interpolation for sampling
  - Handles boundary conditions

### diffuse.comp

- **Purpose:** Simulates viscosity by diffusing the velocity field.
- **Inputs:**  
  - Uniform buffer: simulation parameters
  - Images: previous frame, current frame, temporary output
- **Key Operations:**  
  - Jacobi iteration for diffusion
  - Handles boundary conditions

### pressureCalc.comp

- **Purpose:** Solves for pressure to enforce incompressibility (divergence-free velocity).
- **Inputs:**  
  - Uniform buffer: simulation parameters
  - Images: previous frame, current frame, temporary output
- **Key Operations:**  
  - Computes divergence of velocity
  - Iteratively solves for pressure using Jacobi iteration
  - Handles boundary conditions

### gradientSub.comp

- **Purpose:** Subtracts the pressure gradient from the velocity field to make it divergence-free.
- **Inputs:**  
  - Uniform buffer: simulation parameters
  - Images: previous frame, current frame, temporary output
- **Key Operations:**  
  - Computes pressure gradients
  - Updates velocity field accordingly
  - Handles boundary conditions

### compute.comp

- **Purpose:** General-purpose compute shader.  
  - May be used for initialization, clearing buffers, or other utility operations required by the simulation.
- **Inputs/Outputs:**  
  - Typically operates on images or buffers as needed for setup or maintenance steps.

---

## Rendering Shaders

### vertex.vert

- **Purpose:** Transforms quad vertices for rendering.
- **Inputs:**  
  - Vertex attributes: position, color, normal, tangent, texcoord
  - Uniforms: model matrix, push constants (view/proj matrices, lighting, view mode)
- **Outputs:**  
  - Varyings for fragment shader (color, texcoord, normal, etc.)

### fragment.frag

- **Purpose:** Visualizes the simulation output.
- **Inputs:**  
  - Texture sampler: simulation result
  - Varyings from vertex shader
  - View mode (controls visualization style)
- **Key Operations:**  
  - Converts velocity vectors to color using HSV mapping
  - Supports multiple visualization modes (e.g., velocity, pressure)
  - Applies gamma correction and lighting

---

## See Also

- [Core Module Documentation](core.md)
- [App Module Documenation](app.md)
