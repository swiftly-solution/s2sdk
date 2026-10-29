#ifndef COMMON_H
#define COMMON_H

#ifdef _WIN32
#pragma once
#endif

#include <cstdint>

#include "public/tier0/commonmacros.h"

uint32_t& FloatBits( float& f );
uint32_t const FloatBits( const float &f );
float BitsToFloat( uint32_t i );
bool IsFinite( const float &f );
uint32_t FloatAbsBits( float f );
float FloatMakeNegative( float f );
float FloatMakePositive( float f );
float fpmin( float a, float b );
float fpmax( float a, float b );

#define PAD_NUMBER(number, boundary) \
	( ((number) + ((boundary)-1)) / (boundary) ) * (boundary)

#define FLOAT32_NAN_BITS     (uint32)0x7FC00000
#define FLOAT32_NAN          BitsToFloat( FLOAT32_NAN_BITS )

#endif