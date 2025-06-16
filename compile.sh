#!/bin/bash

set -e

glslc src/shaders/vertex.vert -o src/shaders/vert.spv
glslc src/shaders/fragment.frag -o src/shaders/frag.spv
glslc src/shaders/compute.comp -o src/shaders/compute.spv
glslc src/shaders/advect.comp -o src/shaders/advect.spv
glslc src/shaders/diffuse.comp -o src/shaders/diffuse.spv
glslc src/shaders/pressureCalc.comp -o src/shaders/pressureCalc.spv
glslc src/shaders/gradientSub.comp -o src/shaders/gradientSub.spv
