# 3D Graphics Programming

## Spice My Sponza - ICA1

Result : 90%

ICA Requirements:
* Create geometry meshes from the Sponza scene data (which contains indexed triangle lists);
* Draw instances of meshes from the Sponza scene using camera data it contains;
* Implement the full transformation pipeline (model, view and projection transforms);
* Shade the scene using the Phong reflection model with the point light data (including distance attenuation) and per-instance material data from the Sponza scene data (including diffuse and specular coefficients from texture maps);
* Implement a key press feature toggle that additionally draws the surface normal vector at each vertex with emissive shading based upon their world space value (but not affected by the virtual lighting).

## Triangulate My Terrain

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
