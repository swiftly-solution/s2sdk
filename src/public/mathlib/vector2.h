#ifndef VECTOR2_H
#define VECTOR2_H

#if _WIN32
#pragma once
#endif

struct Vector2D
{
    float data[2];
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