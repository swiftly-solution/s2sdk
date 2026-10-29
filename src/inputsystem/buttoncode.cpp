#include "public/inputsystem/buttoncode.h"

bool  IsAlpha( ButtonCode_t code )
{
	return ( code >= KEY_A ) && ( code <= KEY_Z );
}

bool  IsAlphaNumeric( ButtonCode_t code )
{
	return ( code >= KEY_0 ) && ( code <= KEY_Z );
}

bool  IsSpace( ButtonCode_t code )
{
	return ( code == KEY_ENTER ) || ( code == KEY_TAB ) || ( code == KEY_SPACE );
}

bool  IsKeypad( ButtonCode_t code )
{
	return ( code >= KEY_PAD_0 ) && ( code <= KEY_PAD_DECIMAL );
}

bool  IsPunctuation( ButtonCode_t code )
{
	return ( code >= KEY_0 ) && ( code <= KEY_SPACE ) && !IsAlphaNumeric( code ) && !IsSpace( code ) && !IsKeypad( code );
}

bool  IsKeyCode( ButtonCode_t code )
{
	return ( code >= KEY_FIRST ) && ( code <= KEY_LAST );
}

bool  IsMouseCode( ButtonCode_t code )
{
	return ( code >= MOUSE_FIRST ) && ( code <= MOUSE_LAST );
}

bool  IsJoystickCode( ButtonCode_t code )
{
	return ( code >= JOYSTICK_FIRST ) && ( code <= JOYSTICK_LAST );
}

bool  IsJoystickButtonCode( ButtonCode_t code )
{
	return ( code >= JOYSTICK_FIRST_BUTTON ) && ( code <= JOYSTICK_LAST_BUTTON );
}

bool  IsJoystickPOVCode( ButtonCode_t code )
{
	return ( code >= JOYSTICK_FIRST_POV_BUTTON ) && ( code <= JOYSTICK_LAST_POV_BUTTON );
}

bool  IsJoystickAxisCode( ButtonCode_t code )
{
	return ( code >= JOYSTICK_FIRST_AXIS_BUTTON ) && ( code <= JOYSTICK_LAST_AXIS_BUTTON );
}

ButtonCode_t  GetBaseButtonCode( ButtonCode_t code )
{
	if ( IsJoystickButtonCode( code ) )
	{
		int offset = ( code - JOYSTICK_FIRST_BUTTON ) % JOYSTICK_MAX_BUTTON_COUNT;
		return (ButtonCode_t)( JOYSTICK_FIRST_BUTTON + offset );
	}

	if ( IsJoystickPOVCode( code ) )
	{
		int offset = ( code - JOYSTICK_FIRST_POV_BUTTON ) % JOYSTICK_POV_BUTTON_COUNT;
		return (ButtonCode_t)( JOYSTICK_FIRST_POV_BUTTON + offset );
	}

	if ( IsJoystickAxisCode( code ) )
	{
		int offset = ( code - JOYSTICK_FIRST_AXIS_BUTTON ) % JOYSTICK_AXIS_BUTTON_COUNT;
		return (ButtonCode_t)( JOYSTICK_FIRST_AXIS_BUTTON + offset );
	}

	return code;
}

int  GetJoystickForCode( ButtonCode_t code )
{
	if ( !IsJoystickCode( code ) )
		return 0;

	if ( IsJoystickButtonCode( code ) )
	{
		int offset = ( code - JOYSTICK_FIRST_BUTTON ) / JOYSTICK_MAX_BUTTON_COUNT;
		return offset;
	}
	if ( IsJoystickPOVCode( code ) )
	{
		int offset = ( code - JOYSTICK_FIRST_POV_BUTTON ) / JOYSTICK_POV_BUTTON_COUNT;
		return offset;
	}
	if ( IsJoystickAxisCode( code ) )
	{
		int offset = ( code - JOYSTICK_FIRST_AXIS_BUTTON ) / JOYSTICK_AXIS_BUTTON_COUNT;
		return offset;
	}

	return 0;
}

ButtonCode_t  ButtonCodeToJoystickButtonCode( ButtonCode_t code, int nDesiredJoystick )
{
	if ( !IsJoystickCode( code ) || nDesiredJoystick == 0 )
		return code;

	nDesiredJoystick = clamp( nDesiredJoystick, 0, MAX_JOYSTICKS - 1 );

	code = GetBaseButtonCode( code );

	// Now upsample it
	if ( IsJoystickButtonCode( code ) )
	{
		int nOffset = code - JOYSTICK_FIRST_BUTTON;
		return JOYSTICK_BUTTON( nDesiredJoystick, nOffset );
	}

	if ( IsJoystickPOVCode( code ) )
	{
		int nOffset = code - JOYSTICK_FIRST_POV_BUTTON;
		return JOYSTICK_POV_BUTTON( nDesiredJoystick, nOffset );
	}

	if ( IsJoystickAxisCode( code ) )
	{
		int nOffset = code - JOYSTICK_FIRST_AXIS_BUTTON;
		return JOYSTICK_AXIS_BUTTON( nDesiredJoystick, nOffset );
	}

	return code;
}
