
#include "matrix4.hpp"

#include <cmath>
#include <cstdio>
#include <cstring>
#include <iomanip>

using namespace std;

sa::Matrix4& sa::Matrix4::operator*=(const sa::Matrix4 &v)
{
	float tmp[16] = { };

	for(int i = 0; i < 4; ++i)
	{
		for(int j = 0; j < 4; ++j)
		{
			float sum = 0.0;
			for(int k = 0; k < 4; ++k)
			{
				sum += data[i * 4 + k] * v[k * 4 + j];
			}
			tmp[i * 4 + j] = sum;
		}
	}
	memcpy(data, tmp, sizeof(float) * 16);
	return *this;
}

sa::Matrix4 sa::Matrix4::operator*(const sa::Matrix4 &v) const
{
	return sa::Matrix4(*this) *= v;
}

sa::vec3<float> sa::Matrix4::operator*(const sa::vec3<float>& v) const
{
	float tmp[16] = { };

	for(int i = 0; i < 3; ++i)
	{
		float sum = 0.0f;
		for(int k = 0; k < 4; ++k)
		{
			sum += data[i*4 + k] * v[k];
		}
		tmp[i] = sum;
	}
	return sa::vec3<float>(tmp[0], tmp[1], tmp[2]);
}

std::ostream& operator<<(std::ostream& out, const sa::Matrix4& m)
{
	out << "[ ";
	for(int i = 0; i < 4; ++i)
	{
		for(int j = 0; j < 4; ++j)
		{
			out << fixed << setprecision(2) << setw(5) << m[i * 4 + j] << " ";
		}
		out << "\n  ";
	}
	out << "]";
	return out;
}



