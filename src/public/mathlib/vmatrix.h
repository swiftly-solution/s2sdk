//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
//
// VMatrix always postmultiply vectors as in Ax = b.
// Given a set of basis vectors ((F)orward, (L)eft, (U)p), and a (T)ranslation, 
// a matrix to transform a vector into that space looks like this:
// Fx Lx Ux Tx
// Fy Ly Uy Ty
// Fz Lz Uz Tz
// 0   0  0  1

// Note that concatenating matrices needs to multiply them in reverse order.
// ie: if I want to apply matrix A, B, then C, the equation needs to look like this:
// C * B * A * v
// ie:
// v = A * v;
// v = B * v;
// v = C * v;
//=============================================================================

#ifndef VMATRIX_H
#define VMATRIX_H

#ifdef _WIN32
#pragma once
#endif

#include <string.h>
#include "public/mathlib/vector.h"
#include "public/mathlib/vplane.h"
#include "public/mathlib/vector4d.h"
#include "public/mathlib/mathlib.h"

struct cplane_t;


class VMatrix
{
public:

	VMatrix();
	VMatrix(
		vec_t m00, vec_t m01, vec_t m02, vec_t m03,
		vec_t m10, vec_t m11, vec_t m12, vec_t m13,
		vec_t m20, vec_t m21, vec_t m22, vec_t m23,
		vec_t m30, vec_t m31, vec_t m32, vec_t m33
		);

	// Creates a matrix where the X axis = forward
	// the Y axis = left, and the Z axis = up
	VMatrix( const Vector& forward, const Vector& left, const Vector& up );
	
	// Construct from a 3x4 matrix
	VMatrix( const matrix3x4_t& matrix3x4 );

	// Set the values in the matrix.
	void		Init( 
		vec_t m00, vec_t m01, vec_t m02, vec_t m03,
		vec_t m10, vec_t m11, vec_t m12, vec_t m13,
		vec_t m20, vec_t m21, vec_t m22, vec_t m23,
		vec_t m30, vec_t m31, vec_t m32, vec_t m33 
		);


	// Initialize from a 3x4
	void		Init( const matrix3x4_t& matrix3x4 );

	// array access
	float* operator[](int i);

	const float* operator[](int i) const;

	// Get a pointer to m[0][0]
	float *Base();

	const float *Base() const;

	void		SetLeft(const Vector &vLeft);
	void		SetUp(const Vector &vUp);
	void		SetForward(const Vector &vForward);

	void		GetBasisVectors(Vector &vForward, Vector &vLeft, Vector &vUp) const;
	void		SetBasisVectors(const Vector &vForward, const Vector &vLeft, const Vector &vUp);

	// Get/set the translation.
	Vector &	GetTranslation( Vector &vTrans ) const;
	void		SetTranslation(const Vector &vTrans);

	void		PreTranslate(const Vector &vTrans);
	void		PostTranslate(const Vector &vTrans);

	matrix3x4_t& As3x4();
	const matrix3x4_t& As3x4() const;
	void		CopyFrom3x4( const matrix3x4_t &m3x4 );
	void		Set3x4( matrix3x4_t& matrix3x4 ) const;

	bool		operator==( const VMatrix& src ) const;
	bool		operator!=( const VMatrix& src ) const;

#ifndef VECTOR_NO_SLOW_OPERATIONS
	// Access the basis vectors.
	Vector		GetLeft() const;
	Vector		GetUp() const;
	Vector		GetForward() const;
	Vector		GetTranslation() const;
#endif


// Matrix->vector operations.
public:
	// Multiply by a 3D vector (same as operator*).
	void		V3Mul(const Vector &vIn, Vector &vOut) const;

	// Multiply by a 4D vector.
	void		V4Mul(const Vector4D &vIn, Vector4D &vOut) const;

#ifndef VECTOR_NO_SLOW_OPERATIONS
	// Applies the rotation (ignores translation in the matrix). (This just calls VMul3x3).
	Vector		ApplyRotation(const Vector &vVec) const;

	// Multiply by a vector (divides by w, assumes input w is 1).
	Vector		operator*(const Vector &vVec) const;

	// Multiply by the upper 3x3 part of the matrix (ie: only apply rotation).
	Vector		VMul3x3(const Vector &vVec) const;

	// Apply the inverse (transposed) rotation (only works on pure rotation matrix)
	Vector		VMul3x3Transpose(const Vector &vVec) const;

	// Multiply by the upper 3 rows.
	Vector		VMul4x3(const Vector &vVec) const;

	// Apply the inverse (transposed) transformation (only works on pure rotation/translation)
	Vector		VMul4x3Transpose(const Vector &vVec) const;
#endif


// Matrix->plane operations.
public:
	// Transform the plane. The matrix can only contain translation and rotation.
	void		TransformPlane( const VPlane &inPlane, VPlane &outPlane ) const;

#ifndef VECTOR_NO_SLOW_OPERATIONS
	// Just calls TransformPlane and returns the result.
	VPlane		operator*(const VPlane &thePlane) const;
#endif

// Matrix->matrix operations.
public:

	VMatrix&	operator=(const VMatrix &mOther);
	
	// Multiply two matrices (out = this * vm).
	void		MatrixMul( const VMatrix &vm, VMatrix &out ) const;

	// Add two matrices.
	const VMatrix& operator+=(const VMatrix &other);

#ifndef VECTOR_NO_SLOW_OPERATIONS
	// Just calls MatrixMul and returns the result.	
	VMatrix		operator*(const VMatrix &mOther) const;

	// Add/Subtract two matrices.
	VMatrix		operator+(const VMatrix &other) const;
	VMatrix		operator-(const VMatrix &other) const;

	// Negation.
	VMatrix		operator-() const;

	// Return inverse matrix. Be careful because the results are undefined 
	// if the matrix doesn't have an inverse (ie: InverseGeneral returns false).
	VMatrix		operator~() const;
#endif

// Matrix operations.
public:
	// Set to identity.
	void		Identity();

	bool		IsIdentity() const;

	// Setup a matrix for origin and angles.
	void		SetupMatrixOrgAngles( const Vector &origin, const QAngle &vAngles );
	
	// General inverse. This may fail so check the return!
	bool		InverseGeneral(VMatrix &vInverse) const;
	
	// Does a fast inverse, assuming the matrix only contains translation and rotation.
	void		InverseTR( VMatrix &mRet ) const;

	// Usually used for debug checks. Returns true if the upper 3x3 contains
	// unit vectors and they are all orthogonal.
	bool		IsRotationMatrix() const;
	
#ifndef VECTOR_NO_SLOW_OPERATIONS
	// This calls the other InverseTR and returns the result.
	VMatrix		InverseTR() const;

	// Get the scale of the matrix's basis vectors.
	Vector		GetScale() const;

	// (Fast) multiply by a scaling matrix setup from vScale.
	VMatrix		Scale(const Vector &vScale);	

	// Normalize the basis vectors.
	VMatrix		NormalizeBasisVectors() const;

	// Transpose.
	VMatrix		Transpose() const;

	// Transpose upper-left 3x3.
	VMatrix		Transpose3x3() const;
#endif

public:
	// The matrix.
	vec_t		m[4][4];
};



//-----------------------------------------------------------------------------
// Helper functions.
//-----------------------------------------------------------------------------

#ifndef VECTOR_NO_SLOW_OPERATIONS

// Setup an identity matrix.
VMatrix		SetupMatrixIdentity();

// Setup as a scaling matrix.
VMatrix		SetupMatrixScale(const Vector &vScale);

// Setup a translation matrix.
VMatrix		SetupMatrixTranslation(const Vector &vTranslation);

// Setup a matrix to reflect around the plane.
VMatrix		SetupMatrixReflection(const VPlane &thePlane);

// Setup a matrix to project from vOrigin onto thePlane.
VMatrix		SetupMatrixProjection(const Vector &vOrigin, const VPlane &thePlane);

// Setup a matrix to rotate the specified amount around the specified axis.
VMatrix		SetupMatrixAxisRot(const Vector &vAxis, vec_t fDegrees);

// Setup a matrix to rotate one axis onto another. Input vectors must be normalized.
VMatrix		SetupMatrixAxisToAxisRot(const Vector &vFromAxis, const Vector &vToAxis);

// Setup a matrix from euler angles. Just sets identity and calls MatrixAngles.
VMatrix		SetupMatrixAngles(const QAngle &vAngles);

// Setup a matrix for origin and angles.
VMatrix		SetupMatrixOrgAngles(const Vector &origin, const QAngle &vAngles);

#endif

#define VMatToString(mat)	(static_cast<const char *>(CFmtStr("[ (%f, %f, %f), (%f, %f, %f), (%f, %f, %f), (%f, %f, %f) ]", mat.m[0][0], mat.m[0][1], mat.m[0][2], mat.m[0][3], mat.m[1][0], mat.m[1][1], mat.m[1][2], mat.m[1][3], mat.m[2][0], mat.m[2][1], mat.m[2][2], mat.m[2][3], mat.m[3][0], mat.m[3][1], mat.m[3][2], mat.m[3][3] ))) // ** Note: this generates a temporary, don't hold reference!

//-----------------------------------------------------------------------------
// Returns the point at the intersection on the 3 planes.
// Returns false if it can't be solved (2 or more planes are parallel).
//-----------------------------------------------------------------------------
bool PlaneIntersection( const VPlane &vp1, const VPlane &vp2, const VPlane &vp3, Vector &vOut );


//-----------------------------------------------------------------------------
// These methods are faster. Use them if you want faster code
//-----------------------------------------------------------------------------
void MatrixSetIdentity( VMatrix &dst );
void MatrixTranspose( const VMatrix& src, VMatrix& dst );
void MatrixCopy( const VMatrix& src, VMatrix& dst );
void MatrixMultiply( const VMatrix& src1, const VMatrix& src2, VMatrix& dst );

// Accessors
void MatrixGetColumn( const VMatrix &src, int nCol, Vector *pColumn );
void MatrixSetColumn( VMatrix &src, int nCol, const Vector &column );
void MatrixGetRow( const VMatrix &src, int nCol, Vector *pColumn );
void MatrixSetRow( VMatrix &src, int nCol, const Vector &column );

// Vector3DMultiply treats src2 as if it's a direction vector
void Vector3DMultiply( const VMatrix& src1, const Vector& src2, Vector& dst );

// Vector3DMultiplyPosition treats src2 as if it's a point (adds the translation)
void Vector3DMultiplyPosition( const VMatrix& src1, const VectorByValue src2, Vector& dst );

// Vector3DMultiplyPositionProjective treats src2 as if it's a point 
// and does the perspective divide at the end
void Vector3DMultiplyPositionProjective( const VMatrix& src1, const Vector &src2, Vector& dst );

// Vector3DMultiplyPosition treats src2 as if it's a direction 
// and does the perspective divide at the end
// NOTE: src1 had better be an inverse transpose to use this correctly
void Vector3DMultiplyProjective( const VMatrix& src1, const Vector &src2, Vector& dst );

void Vector4DMultiply( const VMatrix& src1, const Vector4D& src2, Vector4D& dst );

// Same as Vector4DMultiply except that src2 has an implicit W of 1
void Vector4DMultiplyPosition( const VMatrix& src1, const Vector &src2, Vector4D& dst );

// Multiplies the vector by the transpose of the matrix
void Vector3DMultiplyTranspose( const VMatrix& src1, const Vector& src2, Vector& dst );
void Vector4DMultiplyTranspose( const VMatrix& src1, const Vector4D& src2, Vector4D& dst );

// Transform a plane
void MatrixTransformPlane( const VMatrix &src, const cplane_t &inPlane, cplane_t &outPlane );

// Transform a plane that has an axis-aligned normal
void MatrixTransformAxisAlignedPlane( const VMatrix &src, int nDim, float flSign, float flDist, cplane_t &outPlane );

void MatrixBuildTranslation( VMatrix& dst, float x, float y, float z );
void MatrixBuildTranslation( VMatrix& dst, const Vector &translation );


void MatrixTranslate( VMatrix& dst, const Vector &translation );


void MatrixBuildRotationAboutAxis( VMatrix& dst, const Vector& vAxisOfRot, float angleDegrees );
void MatrixBuildRotateZ( VMatrix& dst, float angleDegrees );

void MatrixRotate( VMatrix& dst, const Vector& vAxisOfRot, float angleDegrees );

// Builds a rotation matrix that rotates one direction vector into another
void MatrixBuildRotation( VMatrix &dst, const Vector& initialDirection, const Vector& finalDirection );

// Builds a scale matrix
void MatrixBuildScale( VMatrix &dst, float x, float y, float z );
void MatrixBuildScale( VMatrix &dst, const Vector& scale );

// Build a perspective matrix.
// zNear and zFar are assumed to be positive.
// You end up looking down positive Z, X is to the right, Y is up.
// X range: [0..1]
// Y range: [0..1]
// Z range: [0..1]
void MatrixBuildPerspective( VMatrix &dst, float fovX, float fovY, float zNear, float zFar );

//-----------------------------------------------------------------------------
// Given a projection matrix, take the extremes of the space in transformed into world space and
// get a bounding box.
//-----------------------------------------------------------------------------
void CalculateAABBFromProjectionMatrix( const VMatrix &worldToVolume, Vector *pMins, Vector *pMaxs );

//-----------------------------------------------------------------------------
// Given a projection matrix, take the extremes of the space in transformed into world space and
// get a bounding sphere.
//-----------------------------------------------------------------------------
void CalculateSphereFromProjectionMatrix( const VMatrix &worldToVolume, Vector *pCenter, float *pflRadius );

//-----------------------------------------------------------------------------
// Given an inverse projection matrix, take the extremes of the space in transformed into world space and
// get a bounding box.
//-----------------------------------------------------------------------------
void CalculateAABBFromProjectionMatrixInverse( const VMatrix &volumeToWorld, Vector *pMins, Vector *pMaxs );

//-----------------------------------------------------------------------------
// Given an inverse projection matrix, take the extremes of the space in transformed into world space and
// get a bounding sphere.
//-----------------------------------------------------------------------------
void CalculateSphereFromProjectionMatrixInverse( const VMatrix &volumeToWorld, Vector *pCenter, float *pflRadius );

//-----------------------------------------------------------------------------
// Calculate frustum planes given a clip->world space transform.
//-----------------------------------------------------------------------------
void FrustumPlanesFromMatrix( const VMatrix &clipToWorld, Frustum_t &frustum );

//-----------------------------------------------------------------------------
// Setup a matrix from euler angles. 
//-----------------------------------------------------------------------------
void MatrixFromAngles( const QAngle& vAngles, VMatrix& dst );

//-----------------------------------------------------------------------------
// Creates euler angles from a matrix 
//-----------------------------------------------------------------------------
void MatrixToAngles( const VMatrix& src, QAngle& vAngles );

//-----------------------------------------------------------------------------
// Does a fast inverse, assuming the matrix only contains translation and rotation.
//-----------------------------------------------------------------------------
void MatrixInverseTR( const VMatrix& src, VMatrix &dst );

//-----------------------------------------------------------------------------
// Inverts any matrix at all
//-----------------------------------------------------------------------------
bool MatrixInverseGeneral(const VMatrix& src, VMatrix& dst);

//-----------------------------------------------------------------------------
// Computes the inverse transpose
//-----------------------------------------------------------------------------
void MatrixInverseTranspose( const VMatrix& src, VMatrix& dst );

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void MatrixBuildOrtho( VMatrix& dst, double left, double top, double right, double bottom, double zNear, double zFar );
void MatrixBuildPerspectiveX( VMatrix& dst, double flFovX, double flAspect, double flZNear, double flZFar );
void MatrixBuildPerspectiveOffCenterX( VMatrix& dst, double flFovX, double flAspect, double flZNear, double flZFar, double bottom, double top, double left, double right );

bool MatricesAreEqual( const VMatrix &src1, const VMatrix &src2, float flTolerance );
void MatrixOrtho( VMatrix& dst, double left, double top, double right, double bottom, double zNear, double zFar );
void MatrixPerspectiveX( VMatrix& dst, double flFovX, double flAspect, double flZNear, double flZFar );
void MatrixPerspectiveOffCenterX( VMatrix& dst, double flFovX, double flAspect, double flZNear, double flZFar, double bottom, double top, double left, double right );

#endif
