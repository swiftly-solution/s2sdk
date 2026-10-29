#include "public/tier0/utlsoacontainer.h"

float & CSOAContainer::FloatValue( int nAttrIdx, int nX, int nY, int nZ ) const
{
	AssertDataType( nAttrIdx, ATTRDATATYPE_FLOAT );
	return RowPtr<float>( nAttrIdx, nY, nZ )[nX];
}

void CSOAContainer:: MulAttr( int nDestAttr, float flScale )
{
	ApplyBinaryFunctionToAttr<MulSIMD>( nDestAttr, ReplicateX4( flScale ) );
}

void CSOAContainer:: AddToAttr( int nDestAttr, float flAddend )
{
	ApplyBinaryFunctionToAttr<AddSIMD>( nDestAttr, ReplicateX4( flAddend ) );
}

void CSOAContainer:: MaxAttr( int nDestAttr, float flMinValue )
{
	ApplyBinaryFunctionToAttr<MaxSIMD>( nDestAttr, ReplicateX4( flMinValue ) );
}

void CSOAContainer:: MinAttr( int nDestAttr, float flMaxValue )
{
	ApplyBinaryFunctionToAttr<MinSIMD>( nDestAttr, ReplicateX4( flMaxValue ) );
}

void CSOAContainer:: ClampAttr( int nDestAttr, float flMinValue, float flMaxValue )
{
	ApplyTwoComposedBinaryFunctionsToAttr<MinSIMD, MaxSIMD>( nDestAttr, ReplicateX4( flMaxValue ), ReplicateX4( flMinValue ) );
}

void CSOAContainer:: Init( void )
{
	memset( m_nDataType, 0xff, sizeof( m_nDataType ) );
	memset( m_pSeparateDataMemory, 0, sizeof( m_pSeparateDataMemory ) );
	
#ifdef _DEBUG
	memset( m_pAttributePtrs, 0xFF, sizeof( m_pAttributePtrs ) );
	memset( m_nStrideInBytes, 0xFF, sizeof( m_nStrideInBytes ) );
	memset( m_nRowStrideInBytes, 0xFF, sizeof( m_nRowStrideInBytes ) );
	memset( m_nSliceStrideInBytes, 0xFF, sizeof( m_nSliceStrideInBytes ) );
#endif

	m_pConstantDataMemory = NULL;
	m_pDataMemory = 0;
	m_nNumQuadsPerRow = 0;
	m_nColumns = m_nPaddedColumns = m_nRows = m_nSlices = 0;
	m_nFieldPresentMask = 0;
	m_eThreadMode = SOATHREADMODE_NONE;
}

void CSOAAttributeReference::operator *=( float flScale ) const
{
	m_pContainer->MulAttr( m_nAttributeID, flScale );
}

void CSOAAttributeReference::operator +=( float flAddend ) const
{
	m_pContainer->AddToAttr( m_nAttributeID, flAddend );
}

void CSOAAttributeReference::operator -=( float flAddend ) const
{
	m_pContainer->AddToAttr( m_nAttributeID, -flAddend );
}

void CSOAAttributeReference::operator =( float flValue ) const
{
	m_pContainer->FillAttr( m_nAttributeID, flValue );
}

void CSOAAttributeReference::operator =( CSOAAttributeReference const &other ) const
{
	m_pContainer->CopyAttrFrom( *other.m_pContainer, m_nAttributeID, other.m_nAttributeID );
}

void CSOAAttributeReference::CopyTo( CSOAAttributeReference &other ) const
{
	other.m_pContainer = m_pContainer;
	other.m_nAttributeID = m_nAttributeID;
}

CSOAAttributeReference CSOAContainer::operator[]( int nAttrIdx )
{
	CSOAAttributeReference ret;
	ret.m_pContainer = this;
	ret.m_nAttributeID = nAttrIdx;
	return ret;
}

CSOAAttributeReference CSOAContainer::Attr( int nAttrIdx )
{
	return (*this)[nAttrIdx];
}

float KMeansQuantizedValue::operator()( int n )
{
	return SubFloat( m_fl4Values[n], 0 );
}

fltx4 const & KMeansSampleDescriptor::operator()( int nField ) const
{
	return *m_pInputValues[nField];
}

void IKMeansErrorMetric:: PostAdjustQuantizedValue( KMeansQuantizedValue &valueToAdjust )
{
}

void IKMeansErrorMetric:: PostStep( int const *pFieldIndices, int nNumFields,
					   KMeansQuantizedValue *pValues, int nNumQuantizedValues,
					   int nIndexField, CSOAContainer &data )
{
}

CSOAContainer::CSOAContainer( void )
{
	Init();
}

bool CSOAContainer::HasAllocatedMemory( int nAttrIdx ) const
{
	return ( m_nFieldPresentMask & ( 1 << nAttrIdx ) ) != 0;
}

EAttributeDataType CSOAContainer::GetAttributeType( int nAttrIdx ) const
{
	Assert( ( nAttrIdx >= 0 ) && ( nAttrIdx < MAX_SOA_FIELDS ) );
	return m_nDataType[nAttrIdx];
}

void CSOAContainer::EnsureDataType( int nAttrIdx, EAttributeDataType nDataType )
{
	if ( !HasAllocatedMemory( nAttrIdx ) )
	{
		SetAttributeType( nAttrIdx, nDataType );
	}
}

int CSOAContainer::NumRows( void ) const
{
	return m_nRows;
}

int CSOAContainer::NumCols( void ) const
{
	return m_nColumns;
}

int CSOAContainer::NumSlices( void ) const
{
	return m_nSlices;
}

void CSOAContainer::AssertDataType( int nAttrIdx, EAttributeDataType nDataType ) const
{
	Assert( nAttrIdx >= 0 );
	Assert( nAttrIdx < MAX_SOA_FIELDS );
	Assert( m_nDataType[ nAttrIdx ] == nDataType );
}

int CSOAContainer::NumQuadsPerRow( void ) const
{
	return m_nNumQuadsPerRow;
}

int CSOAContainer::Count( void ) const
{
	return NumCols();
}

int CSOAContainer::NumElements( void ) const
{
	return NumCols() * NumRows() * NumSlices();
}

size_t CSOAContainer::RowToRowStep( int nAttrIdx ) const
{
	return 0;
}

void const * CSOAContainer::ConstRowPtr( int nAttributeIdx, int nRowNumber, int nSliceNumber ) const
{
	Assert( nRowNumber < m_nRows );
	Assert( nAttributeIdx < MAX_SOA_FIELDS );
	Assert( m_nDataType[nAttributeIdx] != ATTRDATATYPE_NONE );
	return m_pAttributePtrs[nAttributeIdx] 
	+ nRowNumber * m_nRowStrideInBytes[nAttributeIdx]
	+ nSliceNumber * m_nSliceStrideInBytes[nAttributeIdx];
}

FourVectors * CSOAContainer::ElementPointer4V( int nAttributeIdx, int nX, int nY, int nZ ) const
{
	Assert( nAttributeIdx < MAX_SOA_FIELDS );
	Assert( nX < m_nColumns );
	Assert( nY < m_nRows );
	Assert( nZ < m_nSlices );
	Assert( m_nDataType[nAttributeIdx] == ATTRDATATYPE_4V );
	int nXIdx = nX / 4;
	uint8 *pRet =  m_pAttributePtrs[nAttributeIdx] 
		+ nXIdx * 4 * m_nStrideInBytes[nAttributeIdx]
		+ nY * m_nRowStrideInBytes[nAttributeIdx]
		+ nZ * m_nSliceStrideInBytes[nAttributeIdx];
	pRet += 4 * ( nX & 3 );
	return reinterpret_cast<FourVectors *>( pRet );
}

size_t CSOAContainer::ItemByteStride( int nAttributeIdx ) const
{
	Assert( nAttributeIdx < MAX_SOA_FIELDS );
	Assert( m_nDataType[nAttributeIdx] != ATTRDATATYPE_NONE );
	return m_nStrideInBytes[ nAttributeIdx ];
}

void CSOAContainer::MoveDataFrom( CSOAContainer other )
{
	(*this) = other;
	other.Init();
}

FourVectors  CompressSIMD( FourVectors const &a, FourVectors const &b )
{
	FourVectors ret;
	ret.x = CompressSIMD( a.x, b.x );
	ret.y = CompressSIMD( a.y, b.y );
	ret.z = CompressSIMD( a.z, b.z );
	return ret;
}

FourVectors  Compress4SIMD( FourVectors const &a, FourVectors const &b,
									   FourVectors const &c, FourVectors const &d )
{
	FourVectors ret;
	ret.x = Compress4SIMD( a.x, b.x, c.x, d.x );
	ret.y = Compress4SIMD( a.y, b.y, c.y, d.y );
	ret.z = Compress4SIMD( a.z, b.z, c.z, d.z );
	return ret;
}
