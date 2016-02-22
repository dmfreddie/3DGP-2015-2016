/*
  Copyright (c) 2011 Tyrone Davison

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#pragma once
#ifndef __TSL_PRIMITIVES__
#define __TSL_PRIMITIVES__

#include "geometry.hpp"

namespace tsl
{
  /**
   * Generate an indexed mesh of polygons that describes a box.
   * @param X Size of the box in the x axis.  Must be positive.
   * @param Y Size of the box in the y axis.  Must be positive.
   * @param Z Size of the box in the z axis.  Must be positive.
   * @param subdivisions Control the tesselation of the box faces.  Must be greater than zero.
   * @param out Pointer to an empty indexed mesh to receive the generated output.  Must not be null.
   */
  void CreateBox( float X, float Y, float Z, unsigned int subdivisions, IndexedMesh* out );
  /**
   * Generate an indexed mesh of polygons that describes a cube.
   * @param size Dimension of the cube in each of the x, y, z axes.  Must be positive.
   * @param subdivisions Control the tesselation of the cube faces.  Must be greater than zero.
   * @param out Pointer to an empty indexed mesh to receive the generated output.  Must not be null.
   */
  void CreateCube( float size, unsigned int subdivisions, IndexedMesh* out );
  /**
   * Generate an indexed mesh of polygons that describes a sphere.
   * @param radius Radius of the sphere.  Must be positive.
   * @param subdivisions Control the tesselation of the generated output.  Must be greater than zero.
   * @param out Pointer to an empty indexed mesh to receive the generated output.  Must not be null.
   */
  void CreateSphere( float radius, unsigned int subdivisions, IndexedMesh* out );
  /**
   * Generate an indexed mesh of polygons that describes a cylinder.
   * @param radius Radius of the cylinder in the xy plane.  Must be positive.
   * @param height Height of the cylinder in the z axis.  Must be positive.
   * @param subdivisions Control the tesselation of the generated output.  Must be greater than zero.
   * @param out Pointer to an empty indexed mesh to receive the generated output.  Must not be null.
   */
  void CreateCylinder( float radius, float height, unsigned int subdivisions, IndexedMesh* out );
  /**
   * Generate an indexed mesh of polygons that describes a cone.
   * @param radius Radius of the cone in the xy plane.  Must be positive.
   * @param height Height of the cone in the z axis.  Must be positive.
   * @param subdivisions Control the tesselation of the generated output.  Must be greater than zero.
   * @param out Pointer to an empty indexed mesh to receive the generated output.  Must not be null.
   */
  void CreateCone( float radius, float height, unsigned int subdivisions, IndexedMesh* out );
  /**
   * Generate an indexed mesh of polygons that describes a disc.
   * @param radius Radius of the disc in the xy plane.  Must be positive.
   * @param height Height of the disc in the z axis.  The sign of the height dicates the direction of the disc normals.
   * @param subdivisions Control the tesselation of the generated output.  Must be greater than zero.
   * @param out Pointer to an empty indexed mesh to receive the generated output.  Must not be null.
   */
  void CreateDisk( float radius, float height, unsigned int subdivisions, IndexedMesh* out );
}

#endif
