# Roller Coaster

lopment Environment: MSVC

Development Duration: About 3 months


## Overview:

Developed a roller coaster simulation using OpenGL.

Users can edit track paths by adjusting control points, with the system interpolating the track using arc-length parameterization for smooth motion.

A slider allows users to control the speed of the roller coaster.

A menu system enables users to switch between different maps, water modes, and toggle tunnels.

## Key Features:

Terrain Generation: The Beituen map was generated using a height map.

Water Simulation: Implemented two techniques:

Sinusoidal waves for simple oscillations.

Moving height maps for dynamic wave patterns.

Shading Techniques:

Phong shading for terrain.

Toon shading for flying spheres, eagles, and cars.

Reflection & Refraction: Water surfaces utilize Frame Buffer Objects (FBOs) to pre-render images and apply them as textures.

Skybox Rendering: Implemented using cubemaps 
