//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
//===========================================================================//

#ifndef IINPUTSYSTEM_H
#define IINPUTSYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "public/tier0/platform.h"
#include "public/tier0/platwindow.h"
#include "public/appframework/iappsystem.h"
#include "inputenums.h"
#include "buttoncode.h"
#include "analogcode.h"
#include "public/tier0/utlvector.h"


///-----------------------------------------------------------------------------
/// A handle to a cursor icon
///-----------------------------------------------------------------------------
DECLARE_POINTER_HANDLE( InputCursorHandle_t );
#define INPUT_CURSOR_HANDLE_INVALID ( (InputCursorHandle_t)0 )


///-----------------------------------------------------------------------------
/// An enumeration describing well-known cursor icons
///-----------------------------------------------------------------------------
enum InputStandardCursor_t
{
	INPUT_CURSOR_NONE	= 0,
	INPUT_CURSOR_ARROW,
	INPUT_CURSOR_IBEAM,   
	INPUT_CURSOR_HOURGLASS,
	INPUT_CURSOR_CROSSHAIR,
	INPUT_CURSOR_WAITARROW,
	INPUT_CURSOR_UP,
	INPUT_CURSOR_SIZE_NW_SE,
	INPUT_CURSOR_SIZE_NE_SW,
	INPUT_CURSOR_SIZE_W_E,
	INPUT_CURSOR_SIZE_N_S,
	INPUT_CURSOR_SIZE_ALL,
	INPUT_CURSOR_NO,
	INPUT_CURSOR_HAND,

	INPUT_CURSOR_COUNT
};


///-----------------------------------------------------------------------------
/// Main interface for input. This is a low-level interface, creating an
/// OS-independent queue of low-level input events which were sampled since
/// the last call to PollInputState. It also contains facilities for cursor
/// control and creation.
///-----------------------------------------------------------------------------
abstract_class IInputSystem : public IAppSystem
{
public:
	/// Attach, detach input system from a particular window
	/// This window should be the root window for the application
	virtual void AttachToWindow( void* hWnd ) = 0;
	virtual void DetachFromWindow( void* hWnd ) = 0;

	/// Enables/disables input. PollInputState will not update current 
	/// button/analog states when it is called if the system is disabled.
	virtual void EnableInput( bool bEnable ) = 0;

	/// Enables/disables the windows message pump. PollInputState will not
	/// Peek/Dispatch messages if this is disabled
	virtual void EnableMessagePump( bool bEnable ) = 0;

	/// Polls the current input state
	// AMNOTE: bPumpEvents pumps and processes the pending SDL events first
	virtual void PollInputState( bool bPumpEvents ) = 0;

	/// Gets the time of the last polling in ms
	virtual int GetPollTick() const = 0;

	// AMNOTE: Plat_FloatTime() of the last SampleDevices call, copied into every new InputEvent_t
	virtual double unk101() const = 0;
	// AMNOTE: Returns true if any button is down
	virtual bool unk102() const = 0;

	/// Is a button down? "Buttons" are binary-state input devices (mouse buttons, keyboard keys)
	virtual bool IsButtonDown( ButtonCode_t code ) const = 0;

	// AMNOTE: Was the button pressed, double clicked or released since the last poll
	virtual bool unk201( ButtonCode_t code ) const = 0;
	virtual bool unk202( ButtonCode_t code ) const = 0;
	virtual bool unk203( ButtonCode_t code ) const = 0;

	/// Returns the tick at which the button was pressed and released
	virtual int GetButtonPressedTick( ButtonCode_t code ) const = 0;
	virtual int GetButtonReleasedTick( ButtonCode_t code ) const = 0;

	/// Gets the value of an analog input device this frame
	/// Includes joysticks, mousewheel, mouse
	virtual float GetAnalogValue( AnalogCode_t code ) const = 0;

	/// Gets the change in a particular analog input device this frame
	/// Includes joysticks, mousewheel, mouse
	virtual float GetAnalogDelta( AnalogCode_t code ) const = 0;

	/// Returns the input events since the last poll
	virtual int GetEventCount() const = 0;
	virtual const InputEvent_t* GetEventData( ) const = 0;
	virtual const CUtlVector< InputEvent_t > &GetEvents() const = 0;

	/// Posts a user-defined event into the event queue; this is expected
	/// to be called in overridden wndprocs connected to the root panel.
	virtual void PostUserEvent( const InputEvent_t &event ) = 0;

	/// Returns the number of joysticks
	virtual int GetJoystickCount() const = 0;

	/// Enable/disable joystick, it has perf costs
	virtual void EnableJoystickInput( int nJoystick, bool bEnable ) = 0;

	/// Enable/disable diagonal joystick POV (simultaneous POV buttons down)
	virtual void EnableJoystickDiagonalPOV( int nJoystick, bool bEnable ) = 0;

	/// Sample the joystick and append events to the input queue
	virtual void SampleDevices( void ) = 0;

	virtual void SetRumble( float fLeftMotor, float fRightMotor, int userId = INVALID_USER_ID ) = 0;
	virtual void StopRumble( int userId = INVALID_USER_ID ) = 0;

	/// Resets the input state
	virtual void ResetInputState() = 0;

	// AMNOTE: Requests an input state reset that is applied on the next PollInputState
	virtual void unk301() = 0;

	/// Convert back + forth between ButtonCode/AnalogCode + strings
	virtual const char *CodeToString( ButtonCode_t code ) const = 0;
	virtual const char *CodeToString( AnalogCode_t code ) const = 0;
	virtual ButtonCode_t StringToButtonCode( const char *pString ) const = 0;
	virtual AnalogCode_t StringToAnalogCode( const char *pString ) const = 0;

	/// Sleeps until input happens. Pass a negative number to sleep infinitely
	virtual void SleepUntilInput( int nMaxSleepTimeMS = -1 ) = 0;

	/// Convert back + forth between virtual codes + button codes
	virtual ButtonCode_t VirtualKeyToButtonCode( int nVirtualKey ) const = 0;
	virtual int ButtonCodeToVirtualKey( ButtonCode_t code ) const = 0;

	/// How many times have we called PollInputState?
	virtual int GetPollCount() const = 0;

	/// Gets and sets the cursor position, relative to hWnd if it's set
	virtual void GetCursorPosition( float *pX, float *pY, PlatWindow_t hWnd ) = 0;
	virtual void SetCursorPosition( float x, float y, PlatWindow_t hWnd ) = 0;

	virtual bool IsWindowAttached( PlatWindow_t hWnd ) const = 0;

	// AMNOTE: Returns the cursor loaded for id, which SetStandardCursor doesn't change
	virtual InputCursorHandle_t unk401( InputStandardCursor_t id ) = 0;

	/// Gets and overrides the cursor used for one of the well-known cursor icons
	virtual InputCursorHandle_t GetStandardCursor( InputStandardCursor_t id ) = 0;
	virtual void SetStandardCursor( InputStandardCursor_t id, InputCursorHandle_t hCursor ) = 0;

	// AMNOTE: Cursor size scaling, see cl_cursor_scale and cl_auto_cursor_scale
	virtual bool unk501() const = 0;
	virtual bool unk502() const = 0;
	virtual void unk503( bool bEnable ) = 0;
	virtual void unk504( float flScale ) = 0;

	/// Loads a cursor defined in a file
	virtual InputCursorHandle_t LoadCursorFromFile( const char *pFileName, const char *pPathID = NULL ) = 0;

	// AMNOTE: Standard cursor overrides set to hCursor fall back to the loaded standard cursors
	virtual void unk601( InputCursorHandle_t hCursor ) = 0;

	/// Sets the cursor icon
	virtual void SetCursorIcon( InputCursorHandle_t hCursor, bool bForce ) = 0;

	// AMNOTE: Returns the cursor set by SetCursorIcon
	virtual InputCursorHandle_t unk701() const = 0;

	virtual void SetMouseCursorVisible( bool bVisible ) = 0;

	// AMNOTE: Returns the value set by SetMouseCursorVisible
	virtual bool unk801() const = 0;

	/// Gets and sets the cursor coordinate bias and scale used for hWnd
	virtual bool GetCoordinateTransform( PlatWindow_t *pWnd, float *pBiasX, float *pBiasY, float *pScaleX, float *pScaleY ) const = 0;
	virtual void SetCoordinateTransform( PlatWindow_t hWnd, float flBiasX, float flBiasY, float flScaleX, float flScaleY ) = 0;

	/// Mouse capture
	virtual void EnableMouseCapture( PlatWindow_t hWnd ) = 0;
	virtual void DisableMouseCapture() = 0;

	// AMNOTE: Returns true while a window has mouse capture
	virtual bool unk901() const = 0;

	virtual bool HasMouseFocus() const = 0;
	virtual const char *GetInputEventName( InputEventType_t nEventType ) const = 0;
	virtual bool IsAppActive() const = 0;
	virtual bool IsXBoxControllerConnected() const = 0;
	virtual void StartDraggingWindow( void* hWnd ) = 0;
	virtual void SetCursorClip( PlatWindow_t hWnd ) = 0;
	virtual void EnablePowerManagement( bool bEnable ) = 0;
	virtual void EnableSystemCommands( uint32 nFlags, bool bEnable ) = 0;
	virtual void SetRelativeMouseMode( bool bEnable ) = 0;

	// AMNOTE: Returns the value set by SetRelativeMouseMode
	virtual bool unk1001() const = 0;

	virtual void DebugSpew() = 0;
	virtual char GetASCIICharacterForButtonPressed( const InputEvent_t &event ) const = 0;
	virtual uint32 ButtonCodeToSDLKey( ButtonCode_t code ) const = 0;
	virtual ButtonCode_t SDLKeyToButtonCode( uint32 nSDLKey ) const = 0;

	// AMNOTE: SDL scancode helpers: scancode count, ButtonCode_t to and from scancode, SDL_GetScancodeName, SDL_GetScancodeFromName
	virtual int unk1101() const = 0;
	virtual int unk1102( ButtonCode_t code ) const = 0;
	virtual ButtonCode_t unk1103( int nScancode ) const = 0;
	virtual const char *unk1104( int nScancode ) const = 0;
	virtual int unk1105( const char *pName ) const = 0;

	virtual const char *CodeToLocalKeyNameUTF8( ButtonCode_t code ) const = 0;

	// AMNOTE: StringToButtonCode that also accepts the names from CodeToLocalKeyNameUTF8
	virtual ButtonCode_t unk1201( const char *pString ) const = 0;
	// AMNOTE: Sets the SDL scancode names to the localized Valve_ButtonCodeName_ strings
	virtual void unk1202() = 0;

	virtual bool GetButtonCodeIsScanCode() const = 0;
	virtual void SetButtonCodeIsScanCode( bool bIsScanCode ) = 0;

	// AMNOTE: Recomputes the automatic cursor scale
	virtual void unk1301() = 0;

	virtual bool IsIMEAllowed() const = 0;
	virtual void SetIMEAllowed( bool bAllowed ) = 0;
	virtual void SetIMETextLocation( int x, int y, int nWidth, int nHeight ) = 0;
	virtual void DismissIME() = 0;

	virtual void unk1401() = 0;

	/// Posts a button press/release event to the queue and updates the button state
	virtual void PostButtonPressedEvent( PlatWindow_t hWnd, InputEventType_t nType, int nTick, ButtonCode_t code, int nModifiers, bool bDoubleClick, uint64 nSDLTimestamp ) = 0;
	virtual void PostButtonReleasedEvent( PlatWindow_t hWnd, InputEventType_t nType, int nTick, ButtonCode_t code, int nModifiers, uint64 nSDLTimestamp ) = 0;

#ifdef PLATFORM_WINDOWS
	// AMNOTE: Window message hooks, called before the input system handles a message; returning true consumes it
	virtual void unk1501( bool ( *pfnHook )( void *hWnd, uint32 nMsg, uintp wParam, intp lParam ) ) = 0;
	virtual void unk1502( bool ( *pfnHook )( void *hWnd, uint32 nMsg, uintp wParam, intp lParam ) ) = 0;
#endif

	// AMNOTE: SDL event hooks, called before the input system handles an event; returning true consumes it
	virtual void unk1503( bool ( *pfnHook )( void *pSDLEvent ) ) = 0;
	virtual void unk1504( bool ( *pfnHook )( void *pSDLEvent ) ) = 0;

	// AMNOTE: Converts an SDL event timestamp in nanoseconds to Plat_FloatTime() time
	virtual double unk1505( uint64 nSDLTimestamp ) const = 0;
};

DECLARE_TIER2_INTERFACE( IInputSystem, g_pInputSystem );


#endif // IINPUTSYSTEM_H
