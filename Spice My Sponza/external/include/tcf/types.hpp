/**
 * types.hpp
 *
 * Common math types.
 *
 * Copyright (c) 2011 Tyrone Davison.
 */

#pragma once
#ifndef __TCF_TYPES__
#define __TCF_TYPES__

namespace tcf {

    struct Vector2 {
        float x, y;
        Vector2() : x(0), y(0) {}
        Vector2(float X, float Y) : x(X), y(Y) {}
    };

    struct Vector3 {
        float x, y, z;
        Vector3() : x(0), y(0), z(0) {}
        Vector3(float X, float Y, float Z) : x(X), y(Y), z(Z) {}
    };

    struct Vector4 {
        float x, y, z, w;
        Vector4() : x(0), y(0), z(0), w(1) {}
        Vector4(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W) {}
    };

    class Matrix {
    public:
        float m00, m10, m20, m30,
              m01, m11, m21, m31,
              m02, m12, m22, m32,
              m03, m13, m23, m33;
    public:
        Matrix() : m00(1), m10(0), m20(0), m30(0),
                   m01(0), m11(1), m21(0), m31(0),
                   m02(0), m12(0), m22(1), m32(0),
                   m03(0), m13(0), m23(0), m33(1) {
        }
        Matrix(float M00, float M01, float M02, float M03,
               float M10, float M11, float M12, float M13,
               float M20, float M21, float M22, float M23,
               float M30, float M31, float M32, float M33)
                    : m00(M00), m10(M10), m20(M20), m30(M30),
                      m01(M01), m11(M11), m21(M21), m31(M31),
                      m02(M02), m12(M12), m22(M22), m32(M32),
                      m03(M03), m13(M13), m23(M23), m33(M33) {
        }
    };

}

#endif
