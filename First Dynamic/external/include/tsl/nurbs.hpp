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
#ifndef __TSL_NURBS__
#define __TSL_NURBS__

#include "geometry.hpp"

namespace tsl
{
  /**
   * Compute an indexed mesh for a NURBS surface.
   * @param ucount Number of control points in the u parametric direction. Must be greater than one.
   * @param uknot Array of knot values for the u parametric direction.
   * @param umin Starting u value to generate the surface.  Currently ignored.
   * @param umax Ending u value to generate the surface.  Currently ignored.
   * @param vcount Number of control points in the v parametric direction. Must be greater than one.
   * @param vknot Array of knot values for the v parametric direction.
   * @param vmin Starting v value to generate the surface.  Currently ignored.
   * @param vmax Ending v value to generate the surface.  Currently ignored.
   * @param control_points Array of homogeneous points (w is the NURBS weight) describing the surface.  The array order must be U-major and its size match ucount*vcount.
   * @param subdivisions_u Control the number of divisions in the u parametric direction for the generated mesh.
   * @param subdivisions_v Control the number of divisions in the v parametric direction for the generated mesh.
   * @param out Pointer to an empty indexed mesh to receive the generated output.  Must not be null.
   */
  void CreateNurbsSurface( unsigned int ucount, const std::vector<float>& uknot, float umin, float umax, unsigned int vcount, const std::vector<float>& vknot, float vmin, float vmax, const std::vector<Vector4>& control_points, unsigned int subdivisions_u, unsigned int subdivisions_v, IndexedMesh* out );
  /**
   * Compute an array of sample points for a NURBS curve.
   * @param knots Array of knot values.
   * @param umin Starting value to generate the curve.
   * @param umax Ending value to generate the curve.
   * @param control_points Array of homogeneous points (w is the NURBS weight) describing the curve.
   * @param subdivisions Control the number of sample points in the output.
   * @param out Pointer to an empty array to receive the generated output.  Must not be null.
   */
  void CreateNurbsCurve( const std::vector<float>& knots, float umin, float umax, const std::vector<Vector4>& control_points, unsigned int subdivisions, std::vector<Vector3>* out );
}

#endif
