#include "public/mathlib/transformws.h"

CTransformWS::CTransformWS() {}

CTransformWS::CTransformWS( const VectorWS &v, const QuaternionWS &q ) : m_vPosition(v), m_orientation(q) {}

bool CTransformWS::IsValid() const
{
	return m_vPosition.IsValid() && m_orientation.IsValid();
}

void CTransformWS::SetToIdentity()
{
	m_vPosition = vec3ws_origin;
	m_orientation = quatws_identity;
}

bool CTransformWS::operator==(const CTransformWS& t) const
{
	return t.m_vPosition == m_vPosition && t.m_orientation == m_orientation;
}

bool CTransformWS::operator!=(const CTransformWS& t) const
{
	return t.m_vPosition != m_vPosition || t.m_orientation != m_orientation;
}
