# 3D Graphics Programming

Spice My Sponza - ICA1

ICA Requirements:
* Create geometry meshes from the Sponza scene data (which contains indexed triangle lists);
* Draw instances of meshes from the Sponza scene using camera data it contains;
* Implement the full transformation pipeline (model, view and projection transforms);
* Shade the scene using the Phong reflection model with the point light data (including distance attenuation) and per-instance material data from the Sponza scene data (including diffuse and specular coefficients from texture maps);
* Implement a key press feature toggle that additionally draws the surface normal vector at each vertex with emissive shading based upon their world space value (but not affected by the virtual lighting).

First Dynamic

This tutorial aims to extend your existing experience of drawing with OpenGL. You will experiment with dynamic meshes and using OpenGL queries to time the duration of your draws by drawing an ocean using a given simulation class. This is not an ICA
* Draw an animated patch via glBufferSubData
* Time the drawing using a GL query 
* Make the query asynchronous
* Draw many patches
* Experiment with GL usage hints
