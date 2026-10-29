#include "public/mathlib/transform.h"

CTransform::CTransform() {}

CTransform::CTransform( const Vector &v, const Quaternion &q ) : m_vPosition(v), m_orientation(q) {}

bool CTransform::IsValid() const
{
	return m_vPosition.IsValid() && m_orientation.IsValid();
}

void CTransform::SetToIdentity()
{
	m_vPosition = vec3_origin;
	m_vPosition.w = 1.0f;
	m_orientation = quat_identity;
}

bool CTransform::operator==(const CTransform& t) const
{
	return t.m_vPosition == m_vPosition && t.m_orientation == m_orientation;
}

bool CTransform::operator!=(const CTransform& t) const
{
	return t.m_vPosition != m_vPosition || t.m_orientation != m_orientation;
}
