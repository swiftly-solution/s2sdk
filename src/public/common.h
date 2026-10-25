#ifndef COMMONMACROS_H
#define COMMONMACROS_H

#ifdef _WIN32
#pragma once
#endif

template <typename T>
inline T CalcNewDoublingCount( T old_count, T requested_count, T min_count, T max_count )
{
	T new_count = old_count;

	while(new_count < requested_count)
	{
		if(new_count < max_count / 2)
		{
			new_count *= 2;
			if(new_count < min_count)
				new_count = min_count;
		}
		else
		{
			new_count = max_count;
			break;
		}
	}

	return new_count;
}

#endif