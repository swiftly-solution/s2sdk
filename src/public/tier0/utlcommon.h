#ifndef UTLCOMMON_H
#define UTLCOMMON_H

#ifdef _WIN32
#pragma once
#endif

#include "platform_base.h"
#include "generichash.h"
#include "utlstring.h"
#include "murmurhash3.h"
#include "utlstringtoken.h"
#include <type_traits>
#include <utility>

struct undefined_t;

template <int sel, typename A, typename B>
struct CTypeSelect { typedef A type; };

template <typename A, typename B>
struct CTypeSelect<0, A, B> { typedef B type; };

// CTypeEquals<A, B>::value is nonzero if A and B are the same type
template <typename A, typename B, bool bIgnoreConstVolatile = false, bool bIgnoreReference = false>
struct CTypeEquals { enum { value = 0 }; };

template <typename Same>
struct CTypeEquals<Same, Same, false, false> { enum { value = 1 }; };

template <typename A, typename B>
struct CTypeEquals<A, B, true, true> : CTypeEquals< const volatile A&, const volatile B& > {};

template <typename A, typename B>
struct CTypeEquals<A, B, true, false> : CTypeEquals< const volatile A, const volatile B > {};

template <typename A, typename B>
struct CTypeEquals<A, B, false, true> : CTypeEquals< A&, B& > {};

template <typename A>
constexpr bool CTypePOD = std::is_standard_layout_v<A> && std::is_trivial_v<A>;

template <typename K, typename V>
class CUtlKeyValuePair
{
public:
	typedef V ValueReturn_t;
	K m_key;
	V m_value;

	CUtlKeyValuePair() {}

	template < typename KInit >
	explicit CUtlKeyValuePair( const KInit &k ) : m_key( k ) {}

	template < typename KInit, typename VInit >
	CUtlKeyValuePair( const KInit &k, const VInit &v ) : m_key( k ), m_value( v ) {}

	V &GetValue() { return m_value; }
	const V &GetValue() const { return m_value; }
};

template <typename K>
class CUtlKeyValuePair<K, empty_t>
{
public:
	typedef const K ValueReturn_t;
	K m_key;

	CUtlKeyValuePair() {}

	template < typename KInit >
	explicit CUtlKeyValuePair( const KInit &k ) : m_key( k ) {}

	template < typename KInit >
	CUtlKeyValuePair( const KInit &k, empty_t ) : m_key( k ) {}

	CUtlKeyValuePair( const K &k, const empty_t& ) : m_key( k ) {}
	const K &GetValue() const { return m_key; }
};

template <typename T> struct DefaultLessFunctor;
template <typename T> struct DefaultEqualFunctor;
template <typename T> struct DefaultHashFunctor;
template <typename T> struct ArgumentTypeInfo;

struct StringLessFunctor { bool operator()( const char *a, const char *b ) const { return Q_strcmp( a, b ) < 0; } };
struct StringEqualFunctor { bool operator()( const char *a, const char *b ) const { return Q_strcmp( a, b ) == 0; } };
struct CaselessStringLessFunctor { bool operator()( const char *a, const char *b ) const { return Q_strcasecmp( a, b ) < 0; } };
struct CaselessStringEqualFunctor { bool operator()( const char *a, const char *b ) const { return Q_strcasecmp( a, b ) == 0; } };

struct MurmurHash3Mix32HashFunctor { unsigned int operator()( uint32_t n ) const { return MurmurHash3Int( n ); } };
struct MurmurHash3Mix64HashFunctor { unsigned int operator()( uint64_t n ) const { return MurmurHash3Int64( n ); } };
struct StringHashFunctor { unsigned int operator()( const char* s ) const; };
struct CaselessStringHashFunctor { unsigned int operator()( const char* s ) const; };

template <typename T> struct IntegerHashFunctor : CTypeSelect<( sizeof( T ) > sizeof( uint32_t ) ), MurmurHash3Mix64HashFunctor, MurmurHash3Mix32HashFunctor>::type { };

struct PointerLessFunctor { bool operator()( const void *a, const void *b ) const { return a < b; } };
struct PointerEqualFunctor { bool operator()( const void *a, const void *b ) const { return a == b; } };
struct PointerHashFunctor { unsigned int operator()( const void* s ) const { return MurmurHash3Mix64HashFunctor()( (uint64_t)(uintptr_t)s ); } };

template < typename T >
struct DefaultLessFunctor
{
	bool operator()( typename ArgumentTypeInfo< T >::Arg_t a, typename ArgumentTypeInfo< T >::Arg_t b ) const { return a < b; }
	bool operator()( typename ArgumentTypeInfo< T >::Alt_t a, typename ArgumentTypeInfo< T >::Arg_t b ) const { return a < b; }
	bool operator()( typename ArgumentTypeInfo< T >::Arg_t a, typename ArgumentTypeInfo< T >::Alt_t b ) const { return a < b; }
};

template < typename T >
struct DefaultEqualFunctor
{
	bool operator()( typename ArgumentTypeInfo< T >::Arg_t a, typename ArgumentTypeInfo< T >::Arg_t b ) const { return a == b; }
	bool operator()( typename ArgumentTypeInfo< T >::Alt_t a, typename ArgumentTypeInfo< T >::Arg_t b ) const { return a == b; }
	bool operator()( typename ArgumentTypeInfo< T >::Arg_t a, typename ArgumentTypeInfo< T >::Alt_t b ) const { return a == b; }
};

template <> struct DefaultHashFunctor<char> : IntegerHashFunctor<char> { };
template <> struct DefaultHashFunctor<signed char> : IntegerHashFunctor<signed char> { };
template <> struct DefaultHashFunctor<unsigned char> : IntegerHashFunctor<unsigned char> { };
template <> struct DefaultHashFunctor<signed short> : IntegerHashFunctor<signed short> { };
template <> struct DefaultHashFunctor<unsigned short> : IntegerHashFunctor<unsigned short> { };
template <> struct DefaultHashFunctor<signed int> : IntegerHashFunctor<signed int> { };
template <> struct DefaultHashFunctor<unsigned int> : IntegerHashFunctor<unsigned int> { };
template <> struct DefaultHashFunctor<signed long> : IntegerHashFunctor<signed long> { };
template <> struct DefaultHashFunctor<unsigned long> : IntegerHashFunctor<unsigned long> { };
template <> struct DefaultHashFunctor<signed long long> : IntegerHashFunctor<signed long long> { };
template <> struct DefaultHashFunctor<unsigned long long> : IntegerHashFunctor<unsigned long long> { };
template <> struct DefaultHashFunctor<void*> : PointerHashFunctor { };
template <> struct DefaultHashFunctor<const void*> : PointerHashFunctor { };
template <> struct DefaultHashFunctor<CUtlStringToken> { unsigned int operator()( const CUtlStringToken &k ) const { return k.GetHashCode(); } };

#if !defined(_MSC_VER) || defined(_NATIVE_WCHAR_T_DEFINED)
template <> struct DefaultHashFunctor<wchar_t> : IntegerHashFunctor<wchar_t> { };
#endif

// Enums are hashed by their underlying value, other types without a specialization have no default hash
template <typename T, bool = std::is_enum_v<T>> struct EnumHashFunctor;
template <typename T> struct EnumHashFunctor<T, true>
{
	unsigned int operator()( T e ) const { return IntegerHashFunctor<std::underlying_type_t<T>>()( (std::underlying_type_t<T>)e ); }
};
template <typename T> struct DefaultHashFunctor : EnumHashFunctor<T> { };

template <typename A, typename B> struct DefaultHashFunctor<std::pair<A, B>>
{
	unsigned int operator()( const std::pair<A, B> &p ) const { return MurmurHash3Mix64HashFunctor()( ( (uint64_t)DefaultHashFunctor<A>()( p.first ) << 32 ) | DefaultHashFunctor<B>()( p.second ) ); }
};

// String specializations. If you want to operate on raw values, use
// PointerLessFunctor and friends from the "building-block" section above
template <> struct DefaultLessFunctor<char*> : StringLessFunctor { };
template <> struct DefaultLessFunctor<const char*> : StringLessFunctor { };
template <> struct DefaultEqualFunctor<char*> : StringEqualFunctor { };
template <> struct DefaultEqualFunctor<const char*> : StringEqualFunctor { };
template <> struct DefaultHashFunctor<char*> : StringHashFunctor { };
template <> struct DefaultHashFunctor<const char*> : StringHashFunctor { };

// CUtlString/CUtlConstString are specialized here and not in utlstring.h
// because I consider string datatypes to be fundamental, and don't feel
// comfortable making that header file dependent on this one. (henryg)
class CUtlString;
template < typename T > class CUtlConstStringBase;

template <> struct DefaultLessFunctor<CUtlString> : StringLessFunctor { };
template <> struct DefaultHashFunctor<CUtlString> : StringHashFunctor { };
template <> struct MurmurHash3Functor<CUtlString> { uint32_t operator()( const CUtlString &key ) const { return MurmurHash3String( key.Get() ); } };
template < typename T > struct DefaultLessFunctor< CUtlConstStringBase<T> > : StringLessFunctor { };
template < typename T > struct DefaultHashFunctor< CUtlConstStringBase<T> > : StringHashFunctor { };


// Helpers to deduce if a type defines a public AltArgumentType_t typedef:
template < typename T >
struct HasClassAltArgumentType
{
	template < typename X > static long Test( typename X::AltArgumentType_t* );
	template < typename X > static char Test( ... );
	enum { value = ( sizeof( Test< T >( NULL ) ) != sizeof( char ) ) };
};

template < typename T, bool = HasClassAltArgumentType< T >::value >
struct GetClassAltArgumentType { typedef typename T::AltArgumentType_t Result_t; };

template < typename T >
struct GetClassAltArgumentType< T, false > { typedef undefined_t Result_t; };

// Unwrap references; reference types don't have member typedefs.
template < typename T >
struct GetClassAltArgumentType< T&, false > : GetClassAltArgumentType< T > { };

// ArgumentTypeInfoImpl is the base for all ArgumentTypeInfo specializations.
template < typename ArgT, typename AltT = typename GetClassAltArgumentType<ArgT>::Result_t >
struct ArgumentTypeInfoImpl
{
	enum { has_alt = 1 };
	typedef ArgT Arg_t;
	typedef AltT Alt_t;
};

// Handle cases where AltArgumentType_t is typedef'd to undefined_t
template < typename ArgT >
struct ArgumentTypeInfoImpl< ArgT, undefined_t >
{
	enum { has_alt = 0 };
	typedef ArgT Arg_t;
	typedef undefined_t Alt_t;
};

// Handle cases where AltArgumentType_t is typedef'd to the primary type
template < typename ArgT >
struct ArgumentTypeInfoImpl< ArgT, ArgT >
{
	enum { has_alt = 0 };
	typedef ArgT Arg_t;
	typedef undefined_t Alt_t;
};


// By default, everything is passed via const ref and doesn't define an alternate type.
template <typename T> struct ArgumentTypeInfo : ArgumentTypeInfoImpl< const T& > { };

// Small native types are most efficiently passed by value.
template <> struct ArgumentTypeInfo< bool > : ArgumentTypeInfoImpl< bool > { };
template <> struct ArgumentTypeInfo< char > : ArgumentTypeInfoImpl< char > { };
template <> struct ArgumentTypeInfo< signed char > : ArgumentTypeInfoImpl< signed char > { };
template <> struct ArgumentTypeInfo< unsigned char > : ArgumentTypeInfoImpl< unsigned char > { };
template <> struct ArgumentTypeInfo< signed short > : ArgumentTypeInfoImpl< signed short > { };
template <> struct ArgumentTypeInfo< unsigned short > : ArgumentTypeInfoImpl< unsigned short > { };
template <> struct ArgumentTypeInfo< signed int > : ArgumentTypeInfoImpl< signed int > { };
template <> struct ArgumentTypeInfo< unsigned int > : ArgumentTypeInfoImpl< unsigned int > { };
template <> struct ArgumentTypeInfo< signed long > : ArgumentTypeInfoImpl< signed long > { };
template <> struct ArgumentTypeInfo< unsigned long > : ArgumentTypeInfoImpl< unsigned long > { };
template <> struct ArgumentTypeInfo< signed long long > : ArgumentTypeInfoImpl< signed long long > { };
template <> struct ArgumentTypeInfo< unsigned long long > : ArgumentTypeInfoImpl< unsigned long long > { };
template <> struct ArgumentTypeInfo< float > : ArgumentTypeInfoImpl< float > { };
template <> struct ArgumentTypeInfo< double > : ArgumentTypeInfoImpl< double > { };
template <> struct ArgumentTypeInfo< long double > : ArgumentTypeInfoImpl< long double > { };
#if !defined(_MSC_VER) || defined(_NATIVE_WCHAR_T_DEFINED)
template <> struct ArgumentTypeInfo< wchar_t > : ArgumentTypeInfoImpl< wchar_t > { };
#endif

// Pointers are also most efficiently passed by value.
template < typename T > struct ArgumentTypeInfo< T* > : ArgumentTypeInfoImpl< T* > { };


// Specializations to unwrap const-decorated types and references
template <typename T> struct ArgumentTypeInfo<const T> : ArgumentTypeInfo<T> { };
template <typename T> struct ArgumentTypeInfo<volatile T> : ArgumentTypeInfo<T> { };
template <typename T> struct ArgumentTypeInfo<const volatile T> : ArgumentTypeInfo<T> { };
template <typename T> struct ArgumentTypeInfo<T&> : ArgumentTypeInfo<T> { };

template <typename T> struct DefaultLessFunctor<const T> : DefaultLessFunctor<T> { };
template <typename T> struct DefaultLessFunctor<volatile T> : DefaultLessFunctor<T> { };
template <typename T> struct DefaultLessFunctor<const volatile T> : DefaultLessFunctor<T> { };
template <typename T> struct DefaultLessFunctor<T&> : DefaultLessFunctor<T> { };

template <typename T> struct DefaultEqualFunctor<const T> : DefaultEqualFunctor<T> { };
template <typename T> struct DefaultEqualFunctor<volatile T> : DefaultEqualFunctor<T> { };
template <typename T> struct DefaultEqualFunctor<const volatile T> : DefaultEqualFunctor<T> { };
template <typename T> struct DefaultEqualFunctor<T&> : DefaultEqualFunctor<T> { };

template <typename T> struct DefaultHashFunctor<const T> : DefaultHashFunctor<T> { };
template <typename T> struct DefaultHashFunctor<volatile T> : DefaultHashFunctor<T> { };
template <typename T> struct DefaultHashFunctor<const volatile T> : DefaultHashFunctor<T> { };
template <typename T> struct DefaultHashFunctor<T&> : DefaultHashFunctor<T> { };


// Hash all pointer types as raw pointers by default
template <typename T> struct DefaultHashFunctor< T * > : PointerHashFunctor { };


// Here follow the useful implementations.

// Based on the widely-used FNV-1A string hash with a final
// mixing step to improve dispersion for very small and very
// large hash table sizes.
inline unsigned int StringHashFunctor::operator()( const char* s ) const
{
	uint32_t h = 2166136261u;
	for ( ; *s; ++s )
	{
		uint32_t c = (unsigned char) *s;
		h = (h ^ c) * 16777619;
	}
	return (h ^ (h << 17)) + (h >> 21);
}

// Equivalent to StringHashFunctor on lower-case strings.
inline unsigned int CaselessStringHashFunctor::operator()( const char* s ) const
{
	uint32_t h = 2166136261u;
	for ( ; *s; ++s )
	{
		uint32_t c = (unsigned char) *s;
		// Brutally fast branchless ASCII tolower():
		// if ((c >= 'A') && (c <= 'Z')) c += ('a' - 'A');
		c += (((('A'-1) - c) & (c - ('Z'+1))) >> 26) & 32;
		h = (h ^ c) * 16777619;
	}
	return (h ^ (h << 17)) + (h >> 21);
}

#endif