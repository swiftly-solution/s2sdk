//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: This is input priority system, allowing various clients to
// cause input messages / cursor control to be routed to them as opposed to
// other clients.
//
//===========================================================================//

#ifndef IINPUTCLIENTSTACK_H
#define IINPUTCLIENTSTACK_H
#ifdef _WIN32
#pragma once
#endif

#include "public/appframework/iappsystem.h"
#include "iinputsystem.h"


///-----------------------------------------------------------------------------
/// A handle to an input context. These are arranged in a priority-based
/// stack; the top context on the stack which is also enabled wins.
///-----------------------------------------------------------------------------
DECLARE_POINTER_HANDLE( InputContextHandle_t );
#define INPUT_CONTEXT_HANDLE_INVALID ( (InputContextHandle_t)0 )


///-----------------------------------------------------------------------------
/// Purpose: This is input priority system, allowing various clients to
/// cause input messages / cursor control to be routed to them as opposed to
/// other clients.
///
/// NOTE: For Source1, it would be a huge change to move all input (like 
/// the code in engine/keys.cpp for example) to go through this interface. 
/// Therefore, I'm going to stick with only dealing with cursor control, 
/// which is necessary for Jen's new gameUI system to interoperate with VGui.
///-----------------------------------------------------------------------------
abstract_class IInputStackSystem : public IAppSystem
{
public:
	/// Allocates an input context, pushing it on top of the input stack,
	/// thereby giving it top priority
	// AMNOTE: nStateFlags are the states this context controls:
	// 1 cursor icon and visibility, 2 mouse capture, 4 cursor clip,
	// 8 relative mouse mode, 0x10 standard cursors,
	// 0x20 own standard cursors without applying them, 0x40 IME
	virtual InputContextHandle_t PushInputContext( const char *pName, uint32 nStateFlags ) = 0;

	/// Pops the top input context off the input stack, and destroys it.
	virtual void PopInputContext( ) = 0;

	// AMNOTE: Removes the given context from anywhere in the stack and destroys it
	virtual bool unk101( InputContextHandle_t hContext ) = 0;
	// AMNOTE: Moves hContext right above hOtherContext in the stack
	virtual bool unk102( InputContextHandle_t hContext, InputContextHandle_t hOtherContext ) = 0;
	// AMNOTE: Moves hContext right below hOtherContext in the stack
	virtual bool unk103( InputContextHandle_t hContext, InputContextHandle_t hOtherContext ) = 0;

	/// Enables/disables an input context, allowing something lower on the
	/// stack to have control of input. Disabling an input context which
	/// owns mouse capture
	virtual void EnableInputContext( InputContextHandle_t hContext, bool bEnable ) = 0;

	/// Allows a context to make the cursor visible;
	/// the topmost enabled context wins
	virtual void SetCursorVisible( InputContextHandle_t hContext, bool bVisible ) = 0;

	/// Allows a context to set the cursor icon;
	/// the topmost enabled context wins
	// AMNOTE: A visible context with the cursor handle 1 leaves the cursor to the contexts below it
	virtual void SetCursorIcon( InputContextHandle_t hContext, InputCursorHandle_t hCursor ) = 0;

	// AMNOTE: Sets the cursor icon of every context that controls the cursor
	virtual void unk201( InputCursorHandle_t hCursor ) = 0;

	/// Allows a context to enable mouse capture. Disabling an input context
	/// deactivates mouse capture. Capture will occur if it happens on the
	/// topmost enabled context
	virtual void SetMouseCapture( InputContextHandle_t hContext, PlatWindow_t hWnd ) = 0;

	// AMNOTE: Returns true if hContext is enabled and at or above the topmost enabled context controlling any of nStateFlags
	virtual bool IsTopmostEnabledContext( InputContextHandle_t hContext, uint32 nStateFlags ) const = 0;

	virtual void SetCursorClip( InputContextHandle_t hContext, PlatWindow_t hWnd ) = 0;

	virtual void SetRelativeMouseMode( InputContextHandle_t hContext, bool bEnable ) = 0;
	virtual bool GetRelativeMouseMode( InputContextHandle_t hContext ) const = 0;

	virtual InputCursorHandle_t GetStandardCursor( InputContextHandle_t hContext, InputStandardCursor_t id ) = 0;
	virtual void SetStandardCursor( InputContextHandle_t hContext, InputStandardCursor_t id, InputCursorHandle_t hCursor ) = 0;

	virtual void SetIMEAllowed( InputContextHandle_t hContext, bool bAllowed ) = 0;

	// AMNOTE: Reapplies nStateFlags from the topmost enabled contexts controlling them
	virtual void unk301( uint32 nStateFlags ) = 0;

	virtual void DebugSpew() = 0;
};

DECLARE_TIER2_INTERFACE( IInputStackSystem, g_pInputStackSystem );


#endif // IINPUTCLIENTSTACK_H
