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
#ifndef __TSL_GEOMETRY__
#define __TSL_GEOMETRY__

#include "math.hpp"
#include <vector>

namespace tsl
{
  /**
   * A mesh constructed from vertices joined into primitives via an array of indices.
   * This is the standard output format of the shape generation methods.
   */
  struct IndexedMesh
  {
    std::vector<Vector3> vertex_array; //< Array giving the position of each vertex.  Length of the array indicates number of vertices in mesh.
    std::vector<Vector3> normal_array; //< Array giving the normal vector of each vertex.  Length should be zero or match vertex_array.
    std::vector<Vector3> binormal_array; //< Array giving the binormal vector of each vertex.  Length should be zero or match vertex_array.
    std::vector<Vector3> tangent_array; //< Array giving the tangent vector of each vertex.  Length should be zero or match vertex_array.
    std::vector<Vector2> uv_array; //< Array giving the uv surface coordinate of each vertex.  Length should be zero or match vertex_array.
    /**
     * Array of indices to map vertices into primitives.
     * The data is only meaningful if you know the primitive type.
     * An index of -1 is used to terminate polygons.
     */
    std::vector<int> index_array;
  };

  /**
   * Compute the number of polygons described by a mesh.
   * Only valid on a mesh which has index data describing polygons.
   * @param mesh A valid mesh of polygons described using index data.
   * @return The number of polygons in the mesh.
   */
  unsigned int ComputeNumPolygons( const IndexedMesh& mesh );

  /**
   * Modify the index array of a polygon mesh to describe a triangle list.
   * Only valid on a mesh which has index data describing polygons.
   * The existing index data will be destroyed.
   * @param mesh Pointer to a valid mesh of polygons described using index data.  Must not be null.
   * @return The number of triangles in the converted mesh.
   */
  int ConvertPolygonsToTriangles( IndexedMesh* mesh );
}

#endif
