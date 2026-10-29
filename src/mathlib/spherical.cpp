#include "public/mathlib/spherical_geometry.h"
#include <memory.h>
#include "public/mathlib/mathlib.h"

float s_flFactorials[]={
	1.,
	1.,
	2.,
	6.,
	24.,
	120.,
	720.,
	5040.,
	40320.,
	362880.,
	3628800.,
	39916800.,
	479001600.,
	6227020800.,
	87178291200.,
	1307674368000.,
	20922789888000.,
	355687428096000.,
	6402373705728000.,
	121645100408832000.,
	2432902008176640000.,
	51090942171709440000.,
	1124000727777607680000.,
	25852016738884976640000.,
	620448401733239439360000.,
	15511210043330985984000000.,
	403291461126605635584000000.,
	10888869450418352160768000000.,
	304888344611713860501504000000.,
	8841761993739701954543616000000.,
	265252859812191058636308480000000.,
	8222838654177922817725562880000000.,
	263130836933693530167218012160000000.,
	8683317618811886495518194401280000000.
};

float UnitSphereLineSegmentLength( Vector const &a, Vector const &b )
{
	// check unit length
	Assert( fabs( VectorLength( a ) - 1.0 ) < 1.0e-3 );
	Assert( fabs( VectorLength( b ) - 1.0 ) < 1.0e-3 );
	return acos( DotProduct( a, b ) );
}

float UnitSphereTriangleArea( Vector const &a, Vector const &b , Vector const &c )
{
	float flLengthA = UnitSphereLineSegmentLength( b, c );
	float flLengthB = UnitSphereLineSegmentLength( c, a );
	float flLengthC = UnitSphereLineSegmentLength( a, b );

	if ( ( flLengthA == 0. ) || ( flLengthB == 0. ) || ( flLengthC == 0. ) )
		return 0.;											// zero area triangle

	// now, find the 3 incribed angles for the triangle
	float flHalfSumLens = 0.5 * ( flLengthA + flLengthB + flLengthC );
	float flSinSums = sin( flHalfSumLens );
	float flSinSMinusA= sin( flHalfSumLens - flLengthA );
	float flSinSMinusB= sin( flHalfSumLens - flLengthB );
	float flSinSMinusC= sin( flHalfSumLens - flLengthC );

	float flTanAOver2 = sqrt ( ( flSinSMinusB * flSinSMinusC ) / ( flSinSums * flSinSMinusA ) );
	float flTanBOver2 = sqrt ( ( flSinSMinusA * flSinSMinusC ) / ( flSinSums * flSinSMinusB ) );
	float flTanCOver2 = sqrt ( ( flSinSMinusA * flSinSMinusB ) / ( flSinSums * flSinSMinusC ) );

	// Girards formula : area = sum of angles - pi.
	return 2.0 * ( atan( flTanAOver2 ) + atan( flTanBOver2 ) + atan( flTanCOver2 ) ) - M_PI;
}

float AssociatedLegendrePolynomial( int nL, int nM, float flX )
{
	// evaluate associated legendre polynomial at flX, using recurrence relation
	float flPmm = 1.;
	if ( nM > 0 )
	{
		float flSomX2 = sqrt( ( 1 - flX ) * ( 1 + flX ) );
		float flFact = 1.;
		for( int i = 0 ; i < nM; i++ )
		{
			flPmm *= -flFact * flSomX2;
			flFact += 2.0;
		}
	}
	if ( nL == nM )
		return flPmm;
	float flPmmp1 = flX * ( 2.0 * nM + 1.0 ) * flPmm;
	if ( nL == nM + 1 )
		return flPmmp1;
	float flPll = 0.;
	for( int nLL = nM + 2 ; nLL <= nL; nLL++ )
	{
		flPll = ( ( 2.0 * nLL - 1.0 ) * flX * flPmmp1 - ( nLL + nM - 1.0 ) * flPmm ) * ( 1.0 / ( nLL - nM ) );
		flPmm = flPmmp1;
		flPmmp1 = flPll;
	}
	return flPll;
}

static float SHNormalizationFactor( int nL, int nM )
{
	double flTemp = ( ( 2. * nL + 1.0 ) * s_flFactorials[ nL - nM ] )/ ( 4. * M_PI * s_flFactorials[ nL + nM ] );
	return sqrt( flTemp );
}

#define SQRT_2 1.414213562373095

static float SphericalHarmonic( int nL, int nM, float flTheta, float flPhi, float flCosTheta )
{
	if ( nM == 0 )
		return SHNormalizationFactor( nL, 0 ) * AssociatedLegendrePolynomial( nL, nM, flCosTheta );

	if ( nM > 0 )
		return SQRT_2 * SHNormalizationFactor( nL, nM ) * cos ( nM * flPhi ) *
			AssociatedLegendrePolynomial( nL, nM, flCosTheta );

	return
		SQRT_2 * SHNormalizationFactor( nL, -nM ) * sin( -nM * flPhi ) * AssociatedLegendrePolynomial( nL, -nM, flCosTheta );

}

float SphericalHarmonic( int nL, int nM, float flTheta, float flPhi )
{
	return SphericalHarmonic( nL, nM, flTheta, flPhi, cos( flTheta ) );
}

float SphericalHarmonic( int nL, int nM, Vector const &vecDirection )
{
	Assert( fabs( VectorLength( vecDirection ) - 1.0 ) < 0.0001 );
	float flPhi = acos( vecDirection.z );
	float flTheta = 0;
	float S = Square( vecDirection.x ) + Square( vecDirection.y );
	if ( S > 0 )
	{
		flTheta = atan2( vecDirection.y, vecDirection.x );
	}
	return SphericalHarmonic( nL, nM, flTheta, flPhi, cos( flTheta ) );
}
