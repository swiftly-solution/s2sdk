#include "public/mathlib/ray.h"

Ray_t::Ray_t() { Init( Vector( 0.0f, 0.0f, 0.0f ) ); }
Ray_t::Ray_t( const Vector& vStartOffset ) { Init( vStartOffset ); }
Ray_t::Ray_t( const Vector& vCenter, float flRadius ) { Init( vCenter, flRadius ); }
Ray_t::Ray_t( const Vector& vMins, const Vector& vMaxs ) { Init( vMins, vMaxs ); }
Ray_t::Ray_t( const Vector& vCenterA, const Vector& vCenterB, float flRadius ) { Init( vCenterA, vCenterB, flRadius ); }
Ray_t::Ray_t( const Vector& vMins, const Vector& vMaxs, const Vector* pVertices, int nNumVertices ) { Init( vMins, vMaxs, pVertices, nNumVertices ); }

void Ray_t::Init( const Vector& vStartOffset )
{
	m_Line.m_vStartOffset = vStartOffset;
	m_Line.m_flRadius = 0.0f;
	m_eType = RAY_TYPE_LINE;
}

void Ray_t::Init( const Vector& vCenter, float flRadius )
{
	if ( flRadius > 0.0f )
	{
		m_Sphere.m_vCenter = vCenter;
		m_Sphere.m_flRadius = flRadius;
		m_eType = RAY_TYPE_SPHERE;
	}
	else
	{
		Init( vCenter );
	}
}

void Ray_t::Init( const Vector& vMins, const Vector& vMaxs )
{
	if ( vMins != vMaxs )
	{
		m_Hull.m_vMins = vMins;
		m_Hull.m_vMaxs = vMaxs;
		m_eType = RAY_TYPE_HULL;
	}
	else
	{
		Init( vMins );
	}
}

void Ray_t::Init( const Vector& vCenterA, const Vector& vCenterB, float flRadius )
{
	if ( vCenterA != vCenterB )
	{
		if ( flRadius > 0.0f )
		{
			m_Capsule.m_vCenter[0] = vCenterA;
			m_Capsule.m_vCenter[1] = vCenterB;
			m_Capsule.m_flRadius = flRadius;
			m_eType = RAY_TYPE_CAPSULE;
		}
		else
		{
			Init( vCenterA, vCenterB );
		}
	}
	else
	{
		Init( vCenterA, flRadius );
	}
}

void Ray_t::Init( const Vector& vMins, const Vector& vMaxs, const Vector* pVertices, int nNumVertices )
{
	m_Mesh.m_vMins = vMins;
	m_Mesh.m_vMaxs = vMaxs;
	m_Mesh.m_pVertices = pVertices;
	m_Mesh.m_nNumVertices = nNumVertices;
	m_eType = RAY_TYPE_MESH;
}
