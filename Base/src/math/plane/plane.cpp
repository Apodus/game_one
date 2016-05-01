
#include "plane.hpp"

#include <cstdio>


sa::Plane::Plane(const sa::vec3<float>& v1, const sa::vec3<float>& v2, const sa::vec3<float>& v3)
{
	set3Points(v1,v2,v3);
}


sa::Plane::Plane()
{
}


void sa::Plane::set3Points(const sa::vec3<float>& v1, const sa::vec3<float>& v2, const sa::vec3<float>& v3)
{
	vec3<float> aux1 = v1 - v2;
	vec3<float> aux2 = v3 - v2;

	normal = aux2 * aux1;

	normal.normalize();
	point = v2;
	d = -(normal.dotProduct(point));
}

void sa::Plane::setNormalAndPoint(const sa::vec3<float>& normal, const sa::vec3<float>& point)
{
	this->normal = normal;
	this->normal.normalize();
	d = -(this->normal.dotProduct(point));
}

void sa::Plane::setCoefficients(float a, float b, float c, float d)
{
	// set the normal vector
	normal.set(a,b,c);
	//compute the lenght of the vector
	float l = normal.length();
	// normalize the vector
	normal.set(a/l,b/l,c/l);
	// and divide d by th length as well
	this->d = d/l;
}


float sa::Plane::distance(const sa::vec3<float>& p) const
{
	return (d + normal.dotProduct(p));
}

