#ifndef COLOR_H
#define COLOR_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"

class ALIGN4 Color
{
public:
	// constructors
	Color();
	Color(int _r,int _g,int _b);
	Color(int _r,int _g,int _b,int _a);
	
	// set the color
	// r - red component (0-255)
	// g - green component (0-255)
	// b - blue component (0-255)
	// a - alpha component, controls transparency (0 - transparent, 255 - opaque);
	void SetColor(int _r, int _g, int _b, int _a = 0);

	void GetColor(int &_r, int &_g, int &_b, int &_a) const;

	void SetRawColor( int color32 );

	int GetRawColor() const;

	int r() const;
	int g() const;
	int b() const;
	int a() const;
	
	unsigned char &operator[](int index);

	const unsigned char &operator[](int index) const;

	bool operator == (const Color &rhs) const;

	bool operator != (const Color &rhs) const;

	Color &operator=( const Color &rhs );

private:
	unsigned char _color[4];
} ALIGN4_POST;

#endif