#include "math.h"
#include <cstdio>

std::string t_Vector3::to_string() const
{
	char buf[64];
	snprintf(buf, sizeof(buf), "%.2f, %.2f, %.2f", x, y, z);
	return std::string(buf).c_str();
}

t_Vector3::t_Vector3()
{
	x = 0;
	y = 0;
	z = 0;
}

t_Vector3::t_Vector3(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}
