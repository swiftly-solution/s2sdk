#ifndef VECTOR4_H
#define VECTOR4_H

#if _WIN32
#pragma once
#endif

struct Vector4D
{
    float data[4];
    float &operator[](int index)
	{
		return data[index];
	}
    const float &operator[](int index) const
	{
		return data[index];
	}
};

#endif