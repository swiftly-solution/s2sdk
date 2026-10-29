#include "public/mathlib/aabb.h"

Vector AABB_t::GetCenter() const { return ( m_vMaxBounds + m_vMinBounds ) / 2.0f; }

float AABB_t::GetMinDistToPoint( const Vector &vPoint ) const
{
	return CalcDistanceToAABB( m_vMinBounds, m_vMaxBounds, vPoint );
}

void AABB_t::CreatePlanesFrom( Vector4D *pPlanes ) const
{
	// X
	pPlanes[0] = Vector4D( 1, 0, 0, -m_vMaxBounds.x  );
	pPlanes[1] = Vector4D( -1, 0, 0, m_vMinBounds.x );

	// Y
	pPlanes[2] = Vector4D( 0, 1, 0, -m_vMaxBounds.y );
	pPlanes[3] = Vector4D( 0, -1, 0, m_vMinBounds.y );

	// Z
	pPlanes[4] = Vector4D( 0, 0, 1, -m_vMaxBounds.z );
	pPlanes[5] = Vector4D( 0, 0, -1, m_vMinBounds.z );
}

void AABB_t::MakeInvalid( void )
{
	m_vMinBounds.Init( FLT_MAX, FLT_MAX, FLT_MAX );
	m_vMaxBounds.Init( -FLT_MAX, -FLT_MAX, -FLT_MAX );
}
