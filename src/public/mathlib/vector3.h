#ifndef VECTOR3_H
#define VECTOR3_H

#if _WIN32
#pragma once
#endif

struct Vector
{
    float data[3];
    float &operator[](int index)
	{
		return data[index];
	}
    const float& operator[](int index) const
	{
		return data[index];
	}
};

#endif