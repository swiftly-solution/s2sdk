#ifndef QANGLE_H
#define QANGLE_H

#if _WIN32
#pragma once
#endif

struct QAngle
{
    // in order: pitch, yaw, roll
    float data[3];
    
    float& operator[](int index)
	{
		return data[index];
	}

    const float& operator[](int index) const
	{
		return data[index];
	}
};

#endif