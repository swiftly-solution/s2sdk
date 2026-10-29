#include "public/mathlib/camera.h"

CFrustum::CFrustum()
{
}

CFrustum::~CFrustum()
{
}

bool CFrustum::BoundingVolumeIntersectsFrustum( AABB_t const &box ) const
{
	Vector vMins = box.m_vMinBounds - m_camera.m_origin;
	Vector vMaxs = box.m_vMaxBounds - m_camera.m_origin;
	return !m_frustum.CullBox( vMins, vMaxs );
}

bool CFrustum::BoundingVolumeIntersectsFrustum( Vector const &mins, Vector const &maxes ) const
{
	Vector vMins = mins - m_camera.m_origin;
	Vector vMaxs = maxes - m_camera.m_origin;
	return !m_frustum.CullBox( vMins, vMaxs );
}

bool CFrustum::BoundingVolumeIntersectsFrustum( AABB_t const &box, Vector &vOriginShift ) const
{
	Vector vMins = box.m_vMinBounds - m_camera.m_origin - vOriginShift;
	Vector vMaxs = box.m_vMaxBounds - m_camera.m_origin - vOriginShift;
	return !m_frustum.CullBox( vMins, vMaxs );
}

bool CFrustum::CheckBoxInline( const VectorAligned &mins, const VectorAligned &maxs )
{
	fltx4 fl4Origin = LoadAlignedSIMD( &m_camera.m_origin );
	fltx4 mins4 = SubSIMD( LoadAlignedSIMD( &mins.x ), fl4Origin );
	fltx4 maxs4 = SubSIMD( LoadAlignedSIMD( &maxs.x ), fl4Origin );

	fltx4 minx = SplatXSIMD(mins4);
	fltx4 miny = SplatYSIMD(mins4);
	fltx4 minz = SplatZSIMD(mins4);
	fltx4 maxx = SplatXSIMD(maxs4);
	fltx4 maxy = SplatYSIMD(maxs4);
	fltx4 maxz = SplatZSIMD(maxs4);

	// compute the dot product of the normal and the farthest corner
	// dotBack0 = DotProduct( normal, normals.x < 0 ? mins.x : maxs.x );
	for ( int i = 0; i < 2; i++ )
	{
		fltx4 xTotalBack = MulSIMD( m_frustum.planes[i].nX, MaskedAssign( m_frustum.planes[i].xSign, minx, maxx ) );
		fltx4 yTotalBack = MulSIMD( m_frustum.planes[i].nY, MaskedAssign( m_frustum.planes[i].ySign, miny, maxy ) );
		fltx4 zTotalBack = MulSIMD( m_frustum.planes[i].nZ, MaskedAssign( m_frustum.planes[i].zSign, minz, maxz ) );
		fltx4 dotBack = AddSIMD( xTotalBack, AddSIMD(yTotalBack, zTotalBack) );
		// if plane of the farthest corner is behind the plane, then the box is completely outside this plane
		fltx4 isOut = CmpLtSIMD( dotBack, m_frustum.planes[i].dist );
		if ( IsAnyNegative(isOut) )
			return false;
	}
	return true;
}

void CFrustum::GetNearAndFarPlanesAroundBox( float *pNear, float *pFar, AABB_t const &inBox, Vector &vOriginShift ) const
{
	AABB_t box = inBox;
	box.m_vMinBounds -= vOriginShift;
	box.m_vMaxBounds -= vOriginShift;

	Vector vCorners[8];
	vCorners[0] = box.m_vMinBounds;
	vCorners[1] = Vector( box.m_vMinBounds.x, box.m_vMinBounds.y, box.m_vMaxBounds.z );
	vCorners[2] = Vector( box.m_vMinBounds.x, box.m_vMaxBounds.y, box.m_vMinBounds.z );
	vCorners[3] = Vector( box.m_vMinBounds.x, box.m_vMaxBounds.y, box.m_vMaxBounds.z );

	vCorners[4] = Vector( box.m_vMaxBounds.x, box.m_vMinBounds.y, box.m_vMinBounds.z );
	vCorners[5] = Vector( box.m_vMaxBounds.x, box.m_vMinBounds.y, box.m_vMaxBounds.z );
	vCorners[6] = Vector( box.m_vMaxBounds.x, box.m_vMaxBounds.y, box.m_vMinBounds.z );
	vCorners[7] = box.m_vMaxBounds;

	float flNear = FLT_MAX;//m_camera.m_flZNear;
	float flFar = -FLT_MAX;//m_camera.m_flZFar;
	for ( int i=0; i<8; ++i )
	{
		Vector vDelta = vCorners[i] - m_camera.m_origin;
		float flDist = DotProduct( m_forward, vDelta );
		flNear = MIN( flNear, flDist );
		flFar = MAX( flFar, flDist );
	}

	*pNear = flNear;
	*pFar = flFar;
}

void CFrustum::UpdateFrustumFromCamera()
{
	ComputeViewMatrix( &m_worldToView, &m_cameraToWorld, m_camera );
	ComputeProjectionMatrix( &m_projection, m_camera.m_flZNear, m_camera.m_flZFar, m_camera.m_flFOVX, m_flAspect );

	m_viewProj = (m_projection * VMatrix(m_worldToView)).Transpose();
	MatrixGetColumn( m_cameraToWorld, 0, m_forward );
	MatrixGetColumn( m_cameraToWorld, 1, m_left );
	MatrixGetColumn( m_cameraToWorld, 2, m_up );

	// NOTE: Don't pass camera origin here.  Compute it locally so that voxels can be translated here
	// and the plane constants retain precision over large world coordinate spaces
	// Also the voxel renderer needs the camera information anyway to compute LOD so this is
	// convenient
	GeneratePerspectiveFrustum( vec3_origin, m_camera.m_angles, m_camera.m_flZNear, m_camera.m_flZFar, m_camera.m_flFOVX, m_flAspect, m_frustum );
}

VMatrix CFrustum::ViewMatrixRH( Vector &vEye, Vector &vAt, Vector &vUp )
{
	Vector xAxis, yAxis;
	Vector zAxis = vEye - vAt;
	xAxis = CrossProduct( vUp, zAxis );
	yAxis = CrossProduct( zAxis, xAxis );
	xAxis.NormalizeInPlace();
	yAxis.NormalizeInPlace();
	zAxis.NormalizeInPlace();
	float flDotX = -DotProduct( xAxis, vEye );
	float flDotY = -DotProduct( yAxis, vEye );
	float flDotZ = -DotProduct( zAxis, vEye );
	VMatrix mRet(
		xAxis.x, yAxis.x, zAxis.x, 0,
		xAxis.y, yAxis.y, zAxis.y, 0,
		xAxis.z, yAxis.z, zAxis.z, 0,
		flDotX, flDotY, flDotZ, 1 );
	return mRet.Transpose();
}

VMatrix CFrustum::OrthoMatrixRH( float flWidth, float flHeight, float flNear, float flFar )
{
	float flDelta = flNear - flFar;

	VMatrix mRet(
		2.0f / flWidth, 0, 0, 0,
		0, 2.0f / flHeight, 0, 0,
		0, 0, 1.0f / flDelta, 0,
		0, 0, flNear / flDelta, 1 );
	return mRet.Transpose();
}


const Vector &CFrustum::Forward() const { return m_forward; }
const Vector &CFrustum::Left() const { return m_left; }
const Vector &CFrustum::Up() const { return m_up; }

void CFrustum::SetView( VMatrix &mView ) { m_worldToView = mView.As3x4(); }
void CFrustum::SetProj( VMatrix &mProj ) { m_projection = mProj; }
void CFrustum::CalcViewProj( ) { m_viewProj = (m_projection * VMatrix(m_worldToView)).Transpose(); }
void CFrustum::SetFrustum( const Frustum_t &frustum ) { m_frustum = frustum; }
void CFrustum::SetForward( Vector &forward ) { m_forward = forward; }
void CFrustum::SetNearFarPlanes( float flNear, float flFar ) { m_camera.m_flZNear = flNear; m_camera.m_flZFar = flFar; }
void CFrustum::SetCameraPosition( const Vector &origin ) { m_camera.m_origin = origin; }
void CFrustum::SetCameraAngles( const QAngle &angles ) { m_camera.m_angles = angles; }
void CFrustum::SetAspect( float flAspect ) { m_flAspect = flAspect; }
void CFrustum::SetFOV( float flFOV ) { m_camera.m_flFOVX = flFOV; }

const Vector &CFrustum::GetCameraPosition() const { return m_camera.m_origin; }
const QAngle &CFrustum::GetCameraAngles() const { return m_camera.m_angles; }
const matrix3x4_t &CFrustum::GetView() const { return m_worldToView; }
const VMatrix &CFrustum::GetProj() const { return m_projection; }
const VMatrix &CFrustum::GetViewProj() const { return m_viewProj; }
float CFrustum::GetAspect() const { return m_flAspect; }
float CFrustum::GetNearPlane() const { return m_camera.m_flZNear; }
float CFrustum::GetFarPlane() const { return m_camera.m_flZFar; }
float CFrustum::GetFOV() const { return m_camera.m_flFOVX; }

void CFrustum::InitCamera( const Vector &origin, const QAngle &angles, float flNear, float flFar, float flFOV, float flAspect )
{
	m_camera.m_origin = origin;
	m_camera.m_angles = angles;
	m_camera.m_flZNear = flNear;
	m_camera.m_flZFar = flFar;
	m_camera.m_flFOVX = flFOV;
	m_flAspect = flAspect;
}
