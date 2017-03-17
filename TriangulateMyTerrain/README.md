# Triangulate My Terrain

Result : 74%

You are required to write a C++ application to draw a terrain populated with given objects as
detailed below. The application must be written using the given framework available via Blackboard.
You must use OpenGL 3.3 and GLSL shaders. You should:
* Use the provided cubic Bezier patches as the basic shape for the terrain geometry;
* Apply the provided texture map as a displacement map to the basic terrain above;
* Apply a noise-like effect to the geometry to make the terrain appear more natural;
* Make the terrain geometry high-resolution using interpolation / extrapolation as necessary;
* Generate smoothed vertex normal vectors for the terrain to improve the given shading;
* Draw the static objects given in the framework initial data at the given locations;
* Make use of appropriate data structures and algorithms (e.g. regular tiling and/or quadtrees) for
visibility determination to limit unnecessary drawing;
* Add performance measurements and output metrics to report upon the success of the above
algorithms and data structures.
