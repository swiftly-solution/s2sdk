//========= Copyright Valve Corporation, All rights reserved. ============//
#ifndef ANALYSIS_ANNOTATIONS_H
#define ANALYSIS_ANNOTATIONS_H

#if _MSC_VER >= 1600 // VS 2010 and above.
//-----------------------------------------------------------------------------
// Upgrading important helpful warnings to errors
//-----------------------------------------------------------------------------
#pragma warning(error : 4789 ) // warning C4789: destination of memory copy is too small

// Suppress some code analysis warnings
#ifdef _PREFAST_
// Include the annotation header file.
#include <sal.h>

// Tag all printf style format strings with this
#define PRINTF_FORMAT_STRING _Printf_format_string_
#define SCANF_FORMAT_STRING _Scanf_format_string_impl_
// Various macros for specifying the capacity of the buffer pointed
// to by a function parameter. Variations include in/out/inout,
// CAP (elements) versus BYTECAP (bytes), and null termination or
// not (_Z).
#define IN_Z _In_z_
#define IN_CAP(x) _In_count_(x)
#define IN_BYTECAP(x) _In_bytecount_(x)
#define OUT_Z_CAP(x) _Out_z_cap_(x)
#define OUT_CAP(x) _Out_cap_(x)
#define OUT_CAP_C(x) _Out_cap_c_(x) // Output buffer with specified *constant* capacity in elements
#define OUT_BYTECAP(x) _Out_bytecap_(x)
#define OUT_Z_BYTECAP(x) _Out_z_bytecap_(x)
#define INOUT_BYTECAP(x) _Inout_bytecap_(x)
#define INOUT_Z_CAP(x) _Inout_z_cap_(x)
#define INOUT_Z_BYTECAP(x) _Inout_z_bytecap_(x)
// These macros are use for annotating array reference parameters, typically used in functions
// such as V_strcpy_safe. Because they are array references the capacity is already known.
#if _MSC_VER >= 1700
#define OUT_Z_ARRAY _Post_z_
#define INOUT_Z_ARRAY _Prepost_z_
#else
#define OUT_Z_ARRAY _Deref_post_z_
#define INOUT_Z_ARRAY _Deref_prepost_z_
#endif // _MSC_VER >= 1700
// Used for annotating functions to describe their return types.
#define MUST_CHECK_RETURN _Check_return_
// Use the macros above to annotate string functions that fill buffers as shown here,
// in order to give VS's /analyze more opportunities to find bugs.
// void V_wcsncpy( OUT_Z_BYTECAP(maxLenInBytes) wchar_t *pDest, wchar_t const *pSrc, int maxLenInBytes );
// int V_snwprintf( OUT_Z_CAP(maxLenInCharacters) wchar_t *pDest, int maxLenInCharacters, PRINTF_FORMAT_STRING const wchar_t *pFormat, ... );

#endif // _PREFAST_
#endif // _MSC_VER >= 1600 // VS 2010 and above.

#ifndef PRINTF_FORMAT_STRING
#define PRINTF_FORMAT_STRING
#define SCANF_FORMAT_STRING
#define IN_Z
#define IN_CAP(x)
#define IN_BYTECAP(x)
#define OUT_Z_CAP(x)
#define OUT_CAP(x)
#define OUT_CAP_C(x)
#define OUT_BYTECAP(x)
#define OUT_Z_BYTECAP(x)
#define INOUT_BYTECAP(x)
#define INOUT_Z_CAP(x)
#define INOUT_Z_BYTECAP(x)
#define OUT_Z_ARRAY
#define INOUT_Z_ARRAY
#define MUST_CHECK_RETURN
#endif

#endif // ANALYSIS_ANNOTATIONS_H