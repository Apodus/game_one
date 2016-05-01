
#pragma once

#include "math/2d/math.hpp"

namespace sa {
	class Matrix4;
}

namespace sa {
namespace MatrixUtils {
	
    /**
     * Multiplies two matrices together, and stores the result in the third
     * matrix, that is, result = lhs x rhs.
     *
     * @param result The result of the multiplication.
     * @param lhs The left-hand-side multiplicand.
     * @param rhs The right-hand-side multiplicand.
     */
    void multiplyMM(Matrix4& result, const Matrix4& lhs, const Matrix4& rhs);

    /**
     * Computes an orthographic projection matrix.
     *
     * @param result returns the result
     * @param left The left plane
     * @param right The right plane
     * @param bottom The bottom plane
     * @param top The top plane
     * @param near The near plane
     * @param far The far plane
     */
    void orthoM(Matrix4& result,
                float left, float right,
                float bottom, float top,
                float near, float far);

    /**
     * Define a projection matrix in terms of six clip planes
     * @param result the float array that holds the perspective matrix
     * @param left The left plane
     * @param right The right plane
     * @param bottom The bottom plane
     * @param top The top plane
     * @param near The near plane
     * @param far The far plane
     */
    void frustumM(Matrix4& result,
                    float left, float right,
                    float bottom, float top,
                    float near, float far);

    /**
     * Computes the length of a vector,
     *
     * @param x x-coordinate of the vector
     * @param y y-coordinate of the vector
     * @param z z-coordinate of the vector
     * @return the length of the vector
     */
    float length(float x, float y, float z);

    /**
     * Sets matrix to an identity matrix.
     * @param result returns the identity matrix,
     */
    void setIdentityM(Matrix4& result);
    
	/**
     * Translates matrix result by x, y, and z in place.
     * @param result matrix
     * @param x translation factor x
     * @param y translation factor y
     * @param z translation factor z
     */
    void translateM(Matrix4& result, float x, float y, float z);

    /**
     * Rotates a matrix in place by an angle around a vector (x, y, z),
     * @param result the rotated matrix
     * @param degrees an angle to rotate in degrees
     * @param x vector x-coordinate
     * @param y vector y-coordinate
     * @param z vector z-coordinate
     */
    void rotateM(Matrix4& result, float degrees, float x, float y, float z);

    /**
     * Creates a rotation matrix around a vector (x, y, z),
     * @param result returns the result
     * @param angle an angle to rotate in degrees
     * @param x vector x-coordinate
     * @param y vector y-coordinate
     * @param z vector z-coordinate
     */
    void setRotateM(Matrix4& result, float angle, float x, float y, float z);

    /**
     * Implementation of gluLookAt(). Creates a view matrix based on the location, eye direction, and up vector
     * of a camera,
     *
     * @param result returns the result
     * @param eyeX eye point X
     * @param eyeY eye point Y
     * @param eyeZ eye point Z
     * @param centerX camera X
     * @param centerY camera Y
     * @param centerZ camera Z
     * @param upX up vector X
     * @param upY up vector Y
     * @param upZ up vector Z
     */
    
	void setLookAtM(Matrix4& result,
                    float    eyeX, float    eyeY, float    eyeZ,
                    float centerX, float centerY, float centerZ,
                    float     upX, float     upY, float     upZ);

    void makeScaleMatrix(Matrix4& result, float x, float y, float z);
    void scaleM(Matrix4& result, float x, float y, float z);
    void makeTranslationMatrix(Matrix4& data, float x, float y, float z);
}
}