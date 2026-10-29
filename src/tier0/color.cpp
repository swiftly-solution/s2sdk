#include "public/tier0/color.h"

Color::Color()
{
	*((int *)this) = 0;
}

Color::Color(int _r,int _g,int _b)
{
	SetColor(_r, _g, _b, 0);
}

Color::Color(int _r,int _g,int _b,int _a)
{
	SetColor(_r, _g, _b, _a);
}

void Color::SetColor(int _r, int _g, int _b, int _a)
{
	_color[0] = (unsigned char)_r;
	_color[1] = (unsigned char)_g;
	_color[2] = (unsigned char)_b;
	_color[3] = (unsigned char)_a;
}

void Color::GetColor(int &_r, int &_g, int &_b, int &_a) const
{
	_r = _color[0];
	_g = _color[1];
	_b = _color[2];
	_a = _color[3];
}

void Color::SetRawColor( int color32 )
{
	*((int *)this) = color32;
}

int Color::GetRawColor() const
{
	return *((int *)this);
}

int Color::r() const
{ return _color[0]; }

int Color::g() const
{ return _color[1]; }

int Color::b() const
{ return _color[2]; }

int Color::a() const
{ return _color[3]; }

unsigned char & Color::operator[](int index)
{
	return _color[index];
}

const unsigned char & Color::operator[](int index) const
{
	return _color[index];
}

bool Color::operator == (const Color &rhs) const
{
	return ( *((int *)this) == *((int *)&rhs) );
}

bool Color::operator != (const Color &rhs) const
{
	return !(operator==(rhs));
}

Color & Color::operator=( const Color &rhs )
{
	SetRawColor( rhs.GetRawColor() );
	return *this;
}
