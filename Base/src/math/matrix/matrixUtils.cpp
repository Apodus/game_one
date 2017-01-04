
#include "matrixUtils.hpp"
#include "math/matrix/matrix4.hpp"
#include "util/logging.hpp"

#ifdef near
#undef near
#undef far
#endif

float identity[16] = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
    };


void sa::MatrixUtils::multiplyMM(Matrix4& result, const Matrix4& lhs, const Matrix4& rhs) {
	float m[16];
    for(int i = 0; i < 4; ++i) {
        for(int j = 0; j < 4; ++j) {
            float sum = 0.0f;
            for(int k = 0; k < 4; ++k) {
                sum += lhs[4 * k + i] * rhs[4 * j + k];
            }
            m[j * 4 + i] = sum;
        }
    }

	memcpy(result.data, m, sizeof(m));
}


void sa::MatrixUtils::orthoM(Matrix4& result,
            float left, float right,
            float bottom, float top,
            float near, float far)
{
	ASSERT(left != right, "illegal arguments");
	ASSERT(bottom != top, "illegal arguments");
	ASSERT(near != far, "illegal arguments");

    float r_width = 1.0f / (right - left);
    float r_height = 1.0f / (top - bottom);
    float r_depth = 1.0f / (far - near);
    result[0] = 2.0f * (r_width);
    result[5] = 2.0f * (r_height);
    result[10] = -2.0f * (r_depth);
    result[12] = -(right + left) * r_width;
    result[13] = -(top + bottom) * r_height;
    result[14] = -(far + near) * r_depth;
    result[15] = 1.0f;
    result[1] = 0.0f;
    result[2] = 0.0f;
    result[3] = 0.0f;
    result[4] = 0.0f;
    result[6] = 0.0f;
    result[7] = 0.0f;
    result[8] = 0.0f;
    result[9] = 0.0f;
    result[11] = 0.0f;
}


void sa::MatrixUtils::frustumM(Matrix4& result,
                float left, float right,
                float bottom, float top,
                float near, float far)
{
	ASSERT(left != right, "illegal arguments");
	ASSERT(bottom != top, "illegal arguments");
	ASSERT(near != far, "illegal arguments");
	ASSERT(near > 0, "illegal arguments");
	ASSERT(far > 0, "illegal arguments");
        
	float r_width = 1.0f / (right - left);
	float r_height = 1.0f / (top - bottom);
	float r_depth = 1.0f / (near - far);
	float x = 2.0f * (near * r_width);
	float y = 2.0f * (near * r_height);
	// float A = 2.0f * ((right + left) * r_width);
	float A = (right + left) * r_width;
	float B = (top + bottom) * r_height;
	float C = (far + near) * r_depth;
	float D = 2.0f * (far * near * r_depth);
	result[0] = x;
	result[5] = y;
	result[8] = A;
	result[9] = B;
	result[10] = C;
	result[14] = D;
	result[11] = -1.0f;
	result[1] = 0.0f;
	result[2] = 0.0f;
	result[3] = 0.0f;
	result[4] = 0.0f;
	result[6] = 0.0f;
	result[7] = 0.0f;
	result[12] = 0.0f;
	result[13] = 0.0f;
	result[15] = 0.0f;
}

void sa::MatrixUtils::setIdentityM(sa::Matrix4& result) {
    memcpy(result.data, identity, sizeof(identity));
}


void sa::MatrixUtils::translateM(sa::Matrix4& result, float x, float y, float z) {
    for (int i = 0; i < 4; ++i) {
        result[i + 12] += result[i] * x + result[i + 4] * y + result[i + 8] * z;
    }
}

void sa::MatrixUtils::rotateM(sa::Matrix4& result, float radians, float x, float y, float z) {
	sa::Matrix4 temp1;
	sa::Matrix4 temp2;
    setRotateM(temp1, radians, x, y, z);
    multiplyMM(temp2, result, temp1);
	memcpy(result.data, temp2.data, sizeof(temp2.data));
}


void sa::MatrixUtils::setRotateM(sa::Matrix4& result, float angle, float x, float y, float z) {
    result[3] = 0;
    result[7] = 0;
    result[11] = 0;
    result[12] = 0;
    result[13] = 0;
    result[14] = 0;
    result[15] = 1;
    float s = sa::math::sin(angle);
    float c = sa::math::cos(angle);
    if (1.0f == x && 0.0f == y && 0.0f == z) {
        result[5] = c;
        result[10] = c;
        result[6] = s;
        result[9] = -s;
        result[1] = 0;
        result[2] = 0;
        result[4] = 0;
        result[8] = 0;
        result[0] = 1;
    } else if (0.0f == x && 1.0f == y && 0.0f == z) {
        result[0] = c;
        result[10] = c;
        result[8] = s;
        result[2] = -s;
        result[1] = 0;
        result[4] = 0;
        result[6] = 0;
        result[9] = 0;
        result[5] = 1;
    } else if (0.0f == x && 0.0f == y && 1.0f == z) {
        result[0] = c;
        result[5] = c;
        result[1] = s;
        result[4] = -s;
        result[2] = 0;
        result[6] = 0;
        result[8] = 0;
        result[9] = 0;
        result[10] = 1;
    } else {
        float len = length(x, y, z);
        if (1.0f != len) {
            float recipLen = 1.0f / len;
            x *= recipLen;
            y *= recipLen;
            z *= recipLen;
        }
        float nc = 1.0f - c;
        float xy = x * y;
        float yz = y * z;
        float zx = z * x;
        float xs = x * s;
        float ys = y * s;
        float zs = z * s;
        result[0] = x * x * nc + c;
        result[4] = xy * nc - zs;
        result[8] = zx * nc + ys;
        result[1] = xy * nc + zs;
        result[5] = y * y * nc + c;
        result[9] = yz * nc - xs;
        result[2] = zx * nc - ys;
        result[6] = yz * nc + xs;
        result[10] = z * z * nc + c;
    }
}



void sa::MatrixUtils::setLookAtM(sa::Matrix4& result,
                float    eyeX, float    eyeY, float    eyeZ,
                float centerX, float centerY, float centerZ,
                float     upX, float     upY, float     upZ)
{
    float fx = centerX - eyeX;
    float fy = centerY - eyeY;
    float fz = centerZ - eyeZ;

    // Normalize f
    float rlf = 1.0f / length(fx, fy, fz);
    fx *= rlf;
    fy *= rlf;
    fz *= rlf;

    // compute s = f x up (x means "cross product")
    float sx = fy * upZ - fz * upY;
    float sy = fz * upX - fx * upZ;
    float sz = fx * upY - fy * upX;

    // and normalize s
    float rls = 1.0f / length(sx, sy, sz);
    sx *= rls;
    sy *= rls;
    sz *= rls;

    // compute u = s x f
    float ux = sy * fz - sz * fy;
    float uy = sz * fx - sx * fz;
    float uz = sx * fy - sy * fx;

    result[0] = sx;
    result[1] = ux;
    result[2] = -fx;
    result[3] = 0.0f;

    result[4] = sy;
    result[5] = uy;
    result[6] = -fy;
    result[7] = 0.0f;

    result[8] = sz;
    result[9] = uz;
    result[10] = -fz;
    result[11] = 0.0f;

    result[12] = 0.0f;
    result[13] = 0.0f;
    result[14] = 0.0f;
    result[15] = 1.0f;

    translateM(result, -eyeX, -eyeY, -eyeZ);
}

void sa::MatrixUtils::makeScaleMatrix(sa::Matrix4& result, float x, float y, float z) {
	setIdentityM(result);
	result[0] = x;
	result[5] = y;
	result[10] = z;
}

void sa::MatrixUtils::scaleM(sa::Matrix4& result, float x, float y, float z) {
	sa::Matrix4 temp1;
    makeScaleMatrix(temp1, x, y, z);
    multiplyMM(result, result, temp1);
}

void sa::MatrixUtils::makeTranslationMatrix(sa::Matrix4& data, float x, float y, float z) {
    setIdentityM(data);
    data[12] = x;
    data[13] = y;
    data[14] = z;
}

float sa::MatrixUtils::length(float x, float y, float z) {
    return sa::math::sqrt(x * x + y * y + z * z);
}
