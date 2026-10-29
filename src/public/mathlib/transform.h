#ifndef TRANSFORM_H
#define TRANSFORM_H

#ifdef _WIN32
#pragma once
#endif

#include "public/mathlib/vector.h"
#include "public/mathlib/vectoraligned.h"
#include "public/mathlib/quaternionaligned.h"
#include "public/mathlib/mathlib.h"

class ALIGN16 CTransform
{
public:
	CTransform();
	CTransform( const Vector &v, const Quaternion &q );

	bool IsValid() const;

	bool operator==(const CTransform& v) const;
	bool operator!=(const CTransform& v) const;

	void SetToIdentity();

public:
	VectorAligned m_vPosition;
	QuaternionAligned m_orientation;

} ALIGN16_POST;

#endif // TRANSFORM_H
