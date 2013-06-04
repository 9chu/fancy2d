/**********************************************************************
 *<
	FILE: mouseman.h

	DESCRIPTION:  Manages mouse input

	CREATED BY: Rolf Berteig
	
	HISTORY: created 18 October 1994

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#ifndef __MOUSEMAN__
#define __MOUSEMAN__

#include "coreexp.h"

/* To create a mouse call back, derive a sub class of this
 * class and redefine 'proc'
 */

class MouseManager;

/*! \sa  Class CommandMode.\n\n
\par Description:
This class is used to allow a developer to capture the mouse events entered by
the user and process them. To create a mouse call back, derive a sub class of
this class and implement the <b>proc()</b> method.  */
class MouseCallBack : public BaseInterfaceServer {
		MouseManager * mouseMan;
	public:
		virtual ~MouseCallBack() {}

		/*! \remarks This method is called when a mouse event takes place to handles its
		processing.
		\par Parameters:
		<b>HWND hwnd</b>\n\n
		The window handle of the window the user clicked in. This is one of the
		viewports. An interface to the viewport can be obtained from the
		system, given this window handle. See
		Class Interface and review the
		methods below:\n\n
		<b>ViewExp *GetViewport( HWND hwnd )</b>\n\n
		This method gets a viewport interface given the window handle.\n\n
		<b>void ReleaseViewport( ViewExp *vpt )</b>\n\n
		When the developer is done with the viewport interface they should call
		this method to release it.\n\n
		<b>int msg</b>\n\n
		This message describes the type of event that occurred. See
		<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_mouse_callback_messages.html">List of Mouse
		Callback Messages</a>.\n\n
		<b>int point</b>\n\n
		The point number. this is 0 for the first click, 1 for the second,
		etc.\n\n
		<b>int flags</b>\n\n
		These flags describe the state of the mouse buttons. See
		<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_mouse_callback_flags.html">List of Mouse
		Callback Flags</a>.\n\n
		<b>IPoint2 m</b>\n\n
		The 2D screen point that the user clicked on. Methods in the viewport
		interface allow this point to be converted into a world space ray or a
		3D view space point. A world space ray can be intersected with the
		active construction plane which results in a point on the active
		construction plane. See Class ViewExp.
		\return  TRUE indicates the proc should continue to process points;
		FALSE means stop processing points. If a plug-in supplied a large
		number of points in a command mode that uses this mouse proc, FALSE can
		be returned to abort the processing before all the points have been
		entered. */
		CoreExport virtual int proc( 
			HWND hwnd, 
			int msg, 
			int point, 
			int flags, 
			IPoint2 m );
		virtual void pan(IPoint2 offset) {}
		/*! \remarks This method is used to override the default drag mode. Most plug-in
		will not need to replace the default implementation of this method.
		What this does is change the way the messages are sent relative to the
		mouse clicking.\n\n
		Normally the messages are sent as follows: When the user clicks down
		this generates a <b>MOUSE_POINT</b> message. Then the user drags the
		mouse with the button down and a series of <b>MOUSE_MOVE</b> messages
		are sent. When they let up on the mouse button a <b>MOUSE_POINT</b>
		messages is generated. Then as the mouse is moved a series of
		<b>MOUSE_MOVE</b> messages are sent. Then they click down on the mouse
		again, but this time a point message is not generated until the button
		is released. All future points are then only sent after the mouse
		button has been pressed and released.
		\par Parameters:
		<b>int mode</b>\n\n
		The current drag mode. See below.
		\return  One of the following drag modes should be returned:\n\n
		<b>CLICK_MODE_DEFAULT</b>\n\n
		Returned to indicate the use of the system mouse mode.\n\n
		<b>CLICK_DRAG_CLICK</b>\n\n
		This is the default behavior as described above.\n\n
		<b>CLICK_MOVE_CLICK</b>\n\n
		In this mode, the first point is entered by clicking the mouse button
		down and then letting it up. This generates point 0. In other words, a
		<b>MOUSE_POINT</b> message is only generated after the user has pressed
		and released the mouse button.\n\n
		<b>CLICK_DOWN_POINT</b>\n\n
		In this mode, point messages are sent on mouse-down only.
		\par Default Implementation:
		<b>{ return mode; }</b>
		\par Sample Code:
		A sample program that uses the override method is
		<b>/MAXSDK/SAMPLES/OBJECTS/SPLINE.CPP.</b>It uses
		<b>CLICK_DOWN_POINT</b>. */
		virtual int override(int mode) { return mode; }		// Return given mouse mode by default
		/*! \remarks This method is used internally. */
		void setMouseManager(MouseManager *mm)  { mouseMan = mm; }
		/*! \remarks This method is used internally. */
		MouseManager *getMouseManager()  { return mouseMan; }

		// Transform Gizmo Interface
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
				This method returns TRUE if the mouse proc supports a transform gizmo;
		otherwise FALSE.\n\n
		This method, and <b>DeactivateTransformGizmo()</b> below, are normally
		implemented by the selection processor and the existing mouse procs. In
		case a special implementation is done, the following describes what
		needs to be done:\n\n
		Since the transform gizmo is dependent on the Command Mode, the
		MouseCallback itself decides if it supports the use of the transform
		gizmo. When the node axis (or transform gizmo) is redrawn, the system
		will ask the command mode's mouse proc if it supports transform gizmos,
		and if it does, it will draw a gizmo, instead of the regular node
		axis.\n\n
		The same things happens with the main selection processor in 3ds Max.
		When the mouse is moved, the selection processor itself asks if the
		MouseCallback supports transform gizmos, and if it does, it will hit
		test the gizmo in a <b>MOUSE_FREEMOVE</b> or <b>MOUSE_POINT</b>
		message. If any of the transform gizmos hit test flags are passed into
		the mouse procs hit tester, the transform gizmo should be hit tested as
		well (using <b>Interface::HitTestTransformGizmo()</b>)\n\n
		When hit testing the gizmo, different flags will be passed in:
		<b>HIT_TRANSFORMGIZMO</b> is passed in on a <b>MOUSE_FREEMOVE</b>
		message so that the axis is hit tested and it hightlights if it is hit,
		but it doesn't actually switch the transform mode.\n\n
		In case of a <b>MOUSE_POINT</b>, the flag will be
		<b>HIT_SWITCH_GIZMO</b>, and if the axis is hit, the 'hit' transform
		mode will be pushed on the transform mode stack.\n\n
		When the mouse is released (<b>MOUSE_POINT</b> (<b>pt==1</b>), or
		<b>MOUSE_ABORT</b>, then the axis constraint should pop back to the
		existing one, and <b>DeactivateTransformGizmo()</b> is called. Inside
		<b>DeactivateTransformGizmo()</b> the normal implementation is to pop
		the axis mode back. It should also maintain a flag (set it if
		<b>HitTestTransformGizmo()</b> returns true and the
		<b>HIT_SWITCH_GIZMO</b> is set, and clear it in
		<b>DeactivateTransformGizmo()</b>. The flag is needed because you
		should not call <b>Interface::PopAxisMode()</b> unless the axis mode
		was previously popped.
		\par Default Implementation:
		<b>{ return FALSE; }</b> */
		virtual BOOL SupportTransformGizmo() { return FALSE; }
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
				This method is called to deactivate the transform gizmo. See the note
		above in <b>SupportTransformGizmo()</b>.
		\par Default Implementation:
		<b>{}</b> */
		virtual void DeactivateTransformGizmo() {}
		// End of Transform Gizmo Interface

		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Mouse procs wishing to utilize the AutoGrid feature should override
		this method by returning TRUE and then make the appropriate calls to
		the <b>ViewExp</b> methods <b>TrackImpliciGrid()</b>,
		<b>CommitImplicitGrid()</b> and <b>ReleaseImplicitGrid()</b> from the
		body of their classes <b>proc()</b> method. For sample code see
		<b>/MAXSDK/SAMPLES/MODIFIERS/SURFWRAP/SURFWRAP.CPP</b>.
		\par Default Implementation:
		<b>{return FALSE;}</b> */
		virtual BOOL SupportAutoGrid(){return FALSE;}
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
				This method returns TRUE if Ortho Mode makes sense for this creation;
		FALSE if it doesn't. In general this is only TRUE for splines and such.
		\par Default Implementation:
		<b>{return FALSE; }</b> */
		virtual BOOL TolerateOrthoMode(){return FALSE; }
	};


// Messages to a mouse procedure and mouseModes.
//
#define MOUSE_ABORT 		0
#define MOUSE_IDLE			0
#define MOUSE_POINT			1
#define MOUSE_MOVE			2
#define MOUSE_DBLCLICK		3
#define MOUSE_INIT			4
#define MOUSE_UNINIT		5
#define MOUSE_FREEMOVE		6
#define MOUSE_KEYBOARD		7
#define MOUSE_PROPCLICK		8
#define MOUSE_SNAPCLICK		9

// Drag modes.
#define CLICK_MODE_DEFAULT	0	// Returned by CreateMouseCallBack to indicate use of system mouse mode
#define CLICK_DRAG_CLICK	1
#define CLICK_MOVE_CLICK	2
#define CLICK_DOWN_POINT	3	// Point messages on mouse-down only

// Buttons
#define LEFT_BUTTON			0
#define MIDDLE_BUTTON		1
#define RIGHT_BUTTON		2

// Flags to mouse callback.
#define MOUSE_SHIFT			(1<<0)
#define MOUSE_CTRL			(1<<1)
#define MOUSE_ALT			(1<<2)
#define MOUSE_LBUTTON		(1<<3)	// Left button is down
#define MOUSE_MBUTTON		(1<<4)	// Middle button is down
#define MOUSE_RBUTTON		(1<<5)	// Right button is down

class MouseOverride;
// aszabo | Nov.14.2000
// Although this class is not supposed to be instanciated directly
// by 3rd party code, there's code in the Modifier samples that 
// uses it directly rather then through MouseManager* Interface::GetMouseManager()
//
/*! \sa  Class BaseInterfaceServer,  Class MouseCallBack,  Class Interface\n\n
\par Description:
This class represents the interface to the mouse manager and handling system
and is used internally only.\n\n
This class should not be instanced directly into plugin code. To retrieve a
pointer to the mouse manager you can use the method <b>MouseManager*
Interface::GetMouseManager()</b>. */
class MouseManager : public BaseInterfaceServer {
	private:
		// This is shared by all instances.
		static int clickDragMode;
		
		// These are local to each instance.
		int 			mouseMode;
		int 			curPoint;
		int         	curButton;
		MouseCallBack 	*TheMouseProc[3];
		int 			numPoints[3];
		int				buttonState[3];
		int				mouseProcReplaced;
		int 			inMouseProc;
#ifdef _OSNAP
		UINT			m_msg;
#endif
		HWND			captureWnd;

    // This wonds a callback that plug-ins can register to get the raw
    // windows messages for the mouse and keyboard for a viewport.
        WNDPROC mpMouseWindProc;

	public:

		friend class MouseManagerStateInterface;
		friend class MouseOverride; //internal
		// Constructor/Destructor
		CoreExport MouseManager();
		CoreExport ~MouseManager();

		CoreExport int SetMouseProc( MouseCallBack *mproc, int button, int numPoints=2 );
		CoreExport int SetDragMode( int mode );
		CoreExport int GetDragMode( );
		CoreExport int SetNumPoints( int numPoints, int button );
		CoreExport int ButtonFlags();
		CoreExport void Pan(IPoint2 p);
		CoreExport LRESULT CALLBACK MouseWinProc( 
			HWND hwnd, 
			UINT message, 
			WPARAM wParam, 
			LPARAM lParam );
		
		// RB 4-3-96: Resets back to the MOUSE_IDLE state
		CoreExport void Reset();
		int GetMouseMode() {return mouseMode;}
#ifdef _OSNAP
		UINT GetMouseMsg() {return m_msg;}
		int GetMousePoint() {return curPoint;}
#endif

		CoreExport void SetCapture(HWND hWnd);
		CoreExport HWND HasCapture();
		CoreExport void ReleaseCapture();
		CoreExport void RestoreCapture();

        CoreExport void SetMouseWindProcCallback(WNDPROC pMouseWindProc)
        { mpMouseWindProc = pMouseWindProc; }
// The following member been added
// in 3ds max 4.2.  If your plugin utilizes this new
// mechanism, be sure that your clients are aware that they
// must run your plugin with 3ds max version 4.2 or higher.
		WNDPROC GetMouseWindProcCallback() const
        { return mpMouseWindProc; }
// End of 3ds max 4.2 Extension
	};


#define WM_MOUSEABORT	(WM_USER + 7834)

// Indicates if ANY mouse proc is currently in the process of
// aborting a mouse proc.
/*! \remarks This global function is available in release 2.0 and later
only.\n\n
This function is not part of this class but is availalbe for use. It returns
TRUE if <b>any</b> mouse proc is currently in the process of aborting a mouse
proc; otherwise FALSE.\n\n
For instance, a developer may be using the
<b>Animatable::MouseCycleCompleted()</b> method to put up a dialog box, but
need to not put it up if the mouse proc was aborting. This method provides a
way to know if indeed the mouse proc is aborting so the dialog won't be
displayed inside <b>MouseCycleCompleted()</b>.
\return  TRUE if aborting; otherwise FALSE. */
CoreExport BOOL GetInMouseAbort();


#endif // __MOUSEMAN__
