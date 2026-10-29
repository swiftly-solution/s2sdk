#ifndef RAY_H
#define RAY_H
#ifdef _WIN32
#pragma once
#endif

#include "public/mathlib/vector.h"

enum RayType_t : uint8
{
	RAY_TYPE_LINE = 0,
	RAY_TYPE_SPHERE,
	RAY_TYPE_HULL,
	RAY_TYPE_CAPSULE,
	RAY_TYPE_MESH,
};

//-----------------------------------------------------------------------------
// A ray...
//-----------------------------------------------------------------------------
struct Ray_t
{
	Ray_t();
	Ray_t( const Vector& vStartOffset );
	Ray_t( const Vector& vCenter, float flRadius );
	Ray_t( const Vector& vMins, const Vector& vMaxs );
	Ray_t( const Vector& vCenterA, const Vector& vCenterB, float flRadius );
	Ray_t( const Vector& vMins, const Vector& vMaxs, const Vector* pVertices, int nNumVertices );

	void Init( const Vector& vStartOffset );

	void Init( const Vector& vCenter, float flRadius );

	void Init( const Vector& vMins, const Vector& vMaxs );

	void Init( const Vector& vCenterA, const Vector& vCenterB, float flRadius );

	void Init( const Vector& vMins, const Vector& vMaxs, const Vector* pVertices, int nNumVertices );

	struct Line_t
	{
		Vector m_vStartOffset;
		float m_flRadius;
	};

	struct Sphere_t
	{
		Vector m_vCenter;
		float m_flRadius;
	};

	struct Hull_t
	{
		Vector m_vMins;
		Vector m_vMaxs;
	};

	struct Capsule_t
	{
		Vector m_vCenter[2];
		float m_flRadius;
	};

	struct Mesh_t
	{
		Vector m_vMins;
		Vector m_vMaxs;
		const Vector* m_pVertices;
		int m_nNumVertices;
	};

	union
	{
		Line_t 		m_Line;
		Sphere_t 	m_Sphere;
		Hull_t 		m_Hull;
		Capsule_t 	m_Capsule;
		Mesh_t 		m_Mesh;
	};

	RayType_t m_eType;
};


#endif // RAY_H
