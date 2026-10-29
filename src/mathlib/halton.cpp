//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=====================================================================================//

#include "public/mathlib/halton.h"
#include "public/mathlib/mathlib.h"

HaltonSequenceGenerator_t::HaltonSequenceGenerator_t(int b)
{
	base=b;
	fbase=(float) b;
	seed=1;

}

float HaltonSequenceGenerator_t::GetElement(int elem)
{
	int tmpseed=seed;
	float ret=0.0;
	float base_inv=1.0/fbase;
	while(tmpseed)
	{
		int dig=tmpseed % base;
		ret+=((float) dig)*base_inv;
		base_inv/=fbase;
		tmpseed/=base;
	}
	return ret;
}

float HaltonSequenceGenerator_t::NextValue(void)
{
	return GetElement(seed++);
}

DirectionalSampler_t::DirectionalSampler_t(void)
	: zdot(2),vrot(3)
{
}

Vector DirectionalSampler_t::NextValue(void)
{
	float zvalue=zdot.NextValue();
	zvalue=2*zvalue-1.0;								// map from 0..1 to -1..1
	float phi=acos(zvalue);
	// now, generate a random rotation angle for x/y
	float theta=2.0*M_PI*vrot.NextValue();
	float sin_p=sin(phi);
	return Vector(cos(theta)*sin_p,
				  sin(theta)*sin_p,
				  zvalue);

}
