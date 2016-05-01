#ifndef MATRIX4_H
#define MATRIX4_H

#include <ostream>

#include "util/vec3.hpp"
#include "math/matrix/matrixUtils.hpp"

#include <cstring>

/**
 * A 4x4 matrix, in a column major order:
 *
 *  0  4  8 12
 *  1  5  9 13
 *  2  6 10 14
 *  3  7 11 15
 */

#ifdef near
#undef near
#endif

#ifdef far
#undef far
#endif

namespace sa {

class Matrix4
{
public:
	float data[16];

    Matrix4() {
        makeIdentityMatrix();
    }

    Matrix4(const Matrix4& other) {
        memcpy(data, other.data, sizeof(data));
    }

    void makeIdentityMatrix() {
		MatrixUtils::setIdentityM(*this);
    }

    void makeLookAtMatrix(float eyeX, float eyeY, float eyeZ,
                          float centerX, float centerY, float centerZ,
                          float upX, float upY, float upZ)
	{
        MatrixUtils::setLookAtM(*this, eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ);
    }

    void makeFrustumMatrix(float left, float right, float bottom, float top, float near, float far) {
        MatrixUtils::frustumM(*this, left, right, bottom, top, near, far);
    }

    void makeOrthoMatrix(float left, float right, float bottom, float top, float near, float far) {
        MatrixUtils::orthoM(*this, left, right, bottom, top, near, far);
    }

    void makeRotationMatrix(float degrees, float x, float y, float z) {
        MatrixUtils::setRotateM(*this, degrees, x, y, z);
    }

	Matrix4& operator = (const Matrix4& other) {
		memcpy(data, other.data, sizeof(data));
		return *this;
	}

    void storeMultiplication(Matrix4 lhs, Matrix4 rhs) {
        MatrixUtils::multiplyMM(*this, lhs, rhs);
    }

    void translate(float dx, float dy, float dz) {
        MatrixUtils::translateM(*this, dx, dy, dz);
    }

    void scale(float dx, float dy, float dz) {
        MatrixUtils::scaleM(*this, dx, dy, dz);
    }

	void scale(const vec3<float>& v) {
		scale(v.x, v.y, v.z);
    }

    void makeScaleMatrix(float x, float y, float z) {
        MatrixUtils::makeScaleMatrix(*this, x, y, z);
    }

    void makeTranslationMatrix(float x, float y, float z) {
        MatrixUtils::makeTranslationMatrix(*this, x, y, z);
    }

	template<typename T>
	void makeTranslationMatrix(const vec3<T>& v) {
		makeTranslationMatrix(v.x, v.y, v.z);
	}

    void rotate(float radians, float x, float y, float z) {
        MatrixUtils::rotateM(*this, radians, x, y, z);
    }

	Matrix4& operator*=(const Matrix4& v);
	Matrix4 operator *(const Matrix4& v) const;

	const float& operator [] (int index) const {
		return data[index];
	}

	float& operator [] (int index) {
		return data[index];
	}

	vec3<float> operator *(const vec3<float>& v) const;

};

}

std::ostream& operator<<(std::ostream& out, const sa::Matrix4& m);

#endif

