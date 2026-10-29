#ifndef TRANSFORM_WORLD_SPACE_H
#define TRANSFORM_WORLD_SPACE_H

#ifdef _WIN32
#pragma once
#endif

#include "public/tier0/platform.h"
#include "public/mathlib/vectorws.h"
#include "public/mathlib/mathlib.h"

class ALIGN16 CTransformWS
{
public:
	CTransformWS();
	CTransformWS( const VectorWS &v, const QuaternionWS &q );

	bool IsValid() const;

	bool operator==(const CTransformWS& v) const;
	bool operator!=(const CTransformWS& v) const;

	void SetToIdentity();

public:
	VectorWS m_vPosition;
	QuaternionWS m_orientation;

} ALIGN16_POST;

#endif // TRANSFORM_WORLD_SPACE_H
