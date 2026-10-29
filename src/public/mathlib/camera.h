//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose:
//
// $NoKeywords: $
//
//=============================================================================//

#ifndef CAMERA_H
#define CAMERA_H

#ifdef _WIN32
#pragma once
#endif

#include <math.h>
#include <float.h>

// For vec_t, put this somewhere else?
#include "public/tier0/platform.h"
#include "public/mathlib/vector.h"
#include "public/mathlib/vectoraligned.h"

#include "public/mathlib/vector2d.h"
#include "public/mathlib/math_pfns.h"
#include "public/mathlib/vmatrix.h"
#include "public/mathlib/ssemath.h"
#include "public/mathlib/aabb.h"

#include "public/tier0/memalloc.h"
// declarations for camera and frustum

struct Camera_t
{
	Vector m_origin;
	QAngle m_angles;
	float m_flFOVX;		// FOV for X/width
	float m_flZNear;
	float m_flZFar;
};


//-----------------------------------------------------------------------------
// accessors for generated matrices
//-----------------------------------------------------------------------------
void ComputeViewMatrix( VMatrix *pWorldToView, const Camera_t& camera );
void ComputeViewMatrix( matrix3x4_t *pWorldToView, const Camera_t& camera );
void ComputeViewMatrix( matrix3x4_t *pWorldToView, matrix3x4_t *pWorldToCamera, const Camera_t &camera );
void ComputeProjectionMatrix( VMatrix *pCameraToProjection, const Camera_t& camera, int width, int height );
void ComputeProjectionMatrix( VMatrix *pCameraToProjection, float flZNear, float flZFar, float flFOVX, float flAspectRatio );


//-----------------------------------------------------------------------------
// Computes the screen space position given a screen size
//-----------------------------------------------------------------------------
void ComputeScreenSpacePosition( Vector2D *pScreenPosition, const Vector &vecWorldPosition,
	const Camera_t &camera, int width, int height );


class CFrustum
{
public:
	CFrustum();

	~CFrustum();

	bool BoundingVolumeIntersectsFrustum( AABB_t const &box ) const;

	bool BoundingVolumeIntersectsFrustum( Vector const &mins, Vector const &maxes ) const;

	bool BoundingVolumeIntersectsFrustum( AABB_t const &box, Vector &vOriginShift ) const;

	bool CheckBoxInline( const VectorAligned &mins, const VectorAligned &maxs );

	void GetNearAndFarPlanesAroundBox( float *pNear, float *pFar, AABB_t const &inBox, Vector &vOriginShift ) const;

	void UpdateFrustumFromCamera();



	// Just cram this here since until I figure out how to convert from direction to angles
	static VMatrix ViewMatrixRH( Vector &vEye, Vector &vAt, Vector &vUp );

	VMatrix OrthoMatrixRH( float flWidth, float flHeight, float flNear, float flFar );


	const Vector &Forward() const;
	const Vector &Left() const;
	const Vector &Up() const;

	void SetView( VMatrix &mView );
	void SetProj( VMatrix &mProj );
	void CalcViewProj( );
	void SetFrustum( const Frustum_t &frustum );
	void SetForward( Vector &forward );
	void SetNearFarPlanes( float flNear, float flFar );
	void SetCameraPosition( const Vector &origin );
	void SetCameraAngles( const QAngle &angles );
	void SetAspect( float flAspect );
	void SetFOV( float flFOV );

	const Vector &GetCameraPosition() const;
	const QAngle &GetCameraAngles() const;
	const matrix3x4_t &GetView() const;
	const VMatrix &GetProj() const;
	const VMatrix &GetViewProj() const;
	float GetAspect() const;
	float GetNearPlane() const;
	float GetFarPlane() const;
	float GetFOV() const;

	void InitCamera( const Vector &origin, const QAngle &angles, float flNear, float flFar, float flFOV, float flAspect );

protected:
	ALIGN16 Frustum_t	m_frustum;
	ALIGN16 Camera_t	m_camera;
	float		m_flAspect;
	Vector		m_forward;
	Vector		m_left;
	Vector		m_up;

	matrix3x4_t	m_cameraToWorld;
	matrix3x4_t	m_worldToView;
	VMatrix	m_viewToWorld;
	VMatrix m_projection;
	VMatrix m_viewProj;
};

#endif // CAMERA_H
