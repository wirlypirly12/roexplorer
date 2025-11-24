#pragma once
#include <string>

struct t_Vector3
{
	float x, y, z;

	bool operator==(const t_Vector3& other) const
	{
		return other.x == x && other.y == y && other.z == z;
	}

	std::string to_string() const;

	t_Vector3();
	t_Vector3(float x, float y, float z);
};
