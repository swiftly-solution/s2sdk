#ifndef TIER0_STDLIB_H
#define TIER0_STDLIB_H

#ifdef _WIN32
#pragma once
#endif

#include "platform_base.h"
#include "annotations.h"

#include <stdio.h>
#include <setjmp.h>

PLATFORM_INTERFACE double V_acosd( double x );
PLATFORM_INTERFACE float V_acosf( float x );
PLATFORM_INTERFACE double V_asind( double x );
PLATFORM_INTERFACE float V_asinf( float x );
PLATFORM_INTERFACE double V_atand( double x );
PLATFORM_INTERFACE float V_atanf( float x );
PLATFORM_INTERFACE double V_atan2d( double y, double x );
PLATFORM_INTERFACE float V_atan2f( float y, float x );
PLATFORM_INTERFACE double V_cosd( double x );
PLATFORM_INTERFACE float V_cosf( float x );
PLATFORM_INTERFACE double V_sind( double x );
PLATFORM_INTERFACE float V_sinf( float x );
PLATFORM_INTERFACE void V_sincosd( double x, double *pSin, double *pCos );
PLATFORM_INTERFACE void V_sincosf( float x, float *pSin, float *pCos );
PLATFORM_INTERFACE double V_tand( double x );
PLATFORM_INTERFACE float V_tanf( float x );

// Hyperbolic functions
PLATFORM_INTERFACE double V_coshd( double x );
PLATFORM_INTERFACE float V_coshf( float x );
PLATFORM_INTERFACE double V_sinhd( double x );
PLATFORM_INTERFACE float V_sinhf( float x );
PLATFORM_INTERFACE double V_tanhd( double x );
PLATFORM_INTERFACE float V_tanhf( float x );

// Exponential and logarithmic functions
PLATFORM_INTERFACE double V_expd( double x );
PLATFORM_INTERFACE float V_expf( float x );
PLATFORM_INTERFACE double V_exp2d( double x );
PLATFORM_INTERFACE float V_exp2f( float x );
PLATFORM_INTERFACE double V_logd( double x );
PLATFORM_INTERFACE float V_logf( float x );
PLATFORM_INTERFACE double V_log2d( double x );
PLATFORM_INTERFACE float V_log2f( float x );
// Returns floor(log2(x)), and 0 for 0
PLATFORM_INTERFACE uint32_t V_log2_uint( uint32_t x );
PLATFORM_INTERFACE double V_log10d( double x );
PLATFORM_INTERFACE float V_log10f( float x );

// Power functions
PLATFORM_INTERFACE double V_powd( double x, double y );
PLATFORM_INTERFACE float V_powf( float x, float y );
PLATFORM_INTERFACE double V_tier0_sqrtd( double x );
PLATFORM_INTERFACE float V_tier0_sqrtf( float x );
PLATFORM_INTERFACE double V_hypotd( double x, double y );
PLATFORM_INTERFACE float V_hypotf( float x, float y );

// Rounding and remainder functions
PLATFORM_INTERFACE double V_tier0_ceild( double x );
PLATFORM_INTERFACE float V_tier0_ceilf( float x );
PLATFORM_INTERFACE double V_tier0_floord( double x );
PLATFORM_INTERFACE float V_tier0_floorf( float x );
PLATFORM_INTERFACE double V_roundd( double x );
PLATFORM_INTERFACE float V_roundf( float x );
PLATFORM_INTERFACE double V_fmodd( double x, double y );
PLATFORM_INTERFACE float V_fmodf( float x, float y );
PLATFORM_INTERFACE double V_remainderd( double x, double y );
PLATFORM_INTERFACE float V_remainderf( float x, float y );

// Floating-point manipulation functions
PLATFORM_INTERFACE double V_modfd( double x, double *pIntPart );
PLATFORM_INTERFACE float V_modff( float x, float *pIntPart );
PLATFORM_INTERFACE double V_frexpd( double x, int *pExponent );
PLATFORM_INTERFACE float V_frexpf( float x, int *pExponent );
PLATFORM_INTERFACE double V_ldexpd( double x, int nExponent );
PLATFORM_INTERFACE float V_ldexpf( float x, int nExponent );
PLATFORM_INTERFACE double V_tier0_fabsd( double x );
PLATFORM_INTERFACE float V_tier0_fabsf( float x );

// Classification functions
// Returns the FP_* value of the platform's C runtime, which differ between Windows and Linux
PLATFORM_INTERFACE int V_fpclassifyd( double x );
PLATFORM_INTERFACE int V_isfinited( double x );
PLATFORM_INTERFACE int V_isinfd( double x );
PLATFORM_INTERFACE int V_isnand( double x );

// String functions
PLATFORM_INTERFACE int64_t		V_atoi( const char *str );
PLATFORM_INTERFACE int			V_snprintf( OUT_Z_CAP( destLen ) char *pDest, int destLen, PRINTF_FORMAT_STRING const char *pFormat, ... ) FMTFUNCTION( 3, 4 );
PLATFORM_INTERFACE char *		V_strtok( const char *str, const char *delim );
PLATFORM_INTERFACE int			V_sscanf( const char *buffer, const char *pFormat, ... );
PLATFORM_INTERFACE int			V_swscanf( const wchar_t *buffer, const wchar_t *pFormat, ... );
#ifdef _WIN32
PLATFORM_INTERFACE int			V_sscanf_s( const char *buffer, const char *pFormat, ... );
#endif

// File functions
PLATFORM_INTERFACE FILE *V_Get_stdin();
PLATFORM_INTERFACE FILE *V_Get_stdout();
PLATFORM_INTERFACE FILE *V_Get_stderr();

PLATFORM_INTERFACE FILE *V_fopen( const char *filename, const char *mode );
// Takes a UTF-8 file name on every platform
PLATFORM_INTERFACE FILE *V_fopenUTF8( const char *filename, const char *mode );
PLATFORM_INTERFACE FILE *V_freopen( const char *_Filename, const char *_Mode, FILE *_File );
PLATFORM_INTERFACE FILE *V_fdopen( int nFileDescriptor, const char *mode );
PLATFORM_INTERFACE int V_fclose( FILE *stream );

PLATFORM_INTERFACE size_t V_fread( void *_DstBuf, size_t _ElementSize, size_t _Count, FILE *_File );
PLATFORM_INTERFACE size_t V_fwrite( const void *_Str, size_t _Size, size_t _Count, FILE *_File );
PLATFORM_INTERFACE int V_fgetc( FILE *stream );
PLATFORM_INTERFACE char *V_fgets( char *str, int n, FILE *stream );
PLATFORM_INTERFACE int V_ungetc( int c, FILE *stream );
PLATFORM_INTERFACE int V_fputc( int c, FILE *stream );
PLATFORM_INTERFACE int V_fputs( const char *str, FILE *stream );
PLATFORM_INTERFACE int V_fscanf( FILE *_File, const char *_Format, ... );

// The offsets of V_fseek and V_ftell are 32-bit on Windows, use V_fseeki64 and V_ftelli64 for large files
PLATFORM_INTERFACE int V_fseek( FILE *_File, long _Offset, int _Origin );
PLATFORM_INTERFACE int V_fseeki64( FILE *_File, int64_t _Offset, int _Origin );
PLATFORM_INTERFACE long V_ftell( FILE *_File );
PLATFORM_INTERFACE int64_t V_ftelli64( FILE *_File );
PLATFORM_INTERFACE void V_rewind( FILE *stream );

PLATFORM_INTERFACE int V_fflush( FILE *_File );
PLATFORM_INTERFACE int V_feof( FILE *_File );
PLATFORM_INTERFACE int V_ferror( FILE *_File );
PLATFORM_INTERFACE void V_clearerr( FILE *_File );
PLATFORM_INTERFACE int V_fileno( FILE *_File );
PLATFORM_INTERFACE int V_setvbuf( FILE *stream, char *buffer, int mode, size_t size );

// File descriptor functions
PLATFORM_INTERFACE int V_open( const char *filename, int oflag, int pmode );
// Takes a UTF-8 file name on every platform
PLATFORM_INTERFACE int V_openUTF8( const char *filename, int oflag, int pmode );
PLATFORM_INTERFACE int V_close( int fd );
PLATFORM_INTERFACE int V_dup( int fd );
PLATFORM_INTERFACE int V_fstat( int fd, struct stat *buffer );
PLATFORM_INTERFACE int V_mkstemps( char *szTemplate, int suffixlen );
PLATFORM_INTERFACE int V_mkostemps( char *szTemplate, int suffixlen, int flags );
#ifdef _WIN32
PLATFORM_INTERFACE int V_stat( const char *path, struct stat *buffer );
PLATFORM_INTERFACE int V_setmode( int fd, int mode );
PLATFORM_INTERFACE intptr_t V_get_osfhandle( int fd );
PLATFORM_INTERFACE int V_open_osfhandle( intptr_t osfhandle, int flags );
PLATFORM_INTERFACE char *V_tmpnam( char *str );
#endif

// Other functions
PLATFORM_INTERFACE int V_GetErrno();
PLATFORM_INTERFACE void V_SetErrno( int err );
PLATFORM_INTERFACE void V_abort();
PLATFORM_INTERFACE struct lconv *V_localeconv();

typedef int (*QSortCompareFunc_t)( const void *pLeft, const void *pRight );
typedef int (*QSortContextCompareFunc_t)( void *pContext, const void *pLeft, const void *pRight );

PLATFORM_INTERFACE void V_qsort( void *base, size_t num, size_t width, QSortCompareFunc_t compare );
PLATFORM_INTERFACE void V_qsort_s( void *base, size_t num, size_t width, QSortContextCompareFunc_t compare, void *context );

typedef struct _jmp_buf_holder
{
	jmp_buf buf;
} jmp_buf_holder;

PLATFORM_INTERFACE int V_setjmp( jmp_buf_holder *env );
PLATFORM_INTERFACE void V_longjmp( jmp_buf_holder *env, int value );
PLATFORM_INTERFACE uint32_t V_tier0_crc32( uint32_t nStartCrc, const void *pBuf, size_t nBufLen );

#endif