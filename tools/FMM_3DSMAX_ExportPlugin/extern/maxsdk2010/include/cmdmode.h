/**********************************************************************
 *<
	FILE: cmdmode.h

	DESCRIPTION: Command mode class definition

	CREATED BY:	Rolf Berteig

	HISTORY: Created 13 January 1995

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#ifndef __CMDMODE__
#define __CMDMODE__
#include "maxheap.h"

// This file can be included in plug-in modules so
// it shouldn't reference/include private classes or functions.

class MouseCallBack;
class ChangeForegroundCallback;
class HitByNameDlgCallback;
class PickModeCallback;
class PickNodeCallback;

/*! \sa  Class MouseCallBack,
Class ChangeForegroundCallback, <a href="ms-its:3dsmaxsdk.chm::/vports_foreground_background_planes.html">Foreground / Background Planes</a>.\n\n
\par Description:
This base class allows the developer to create a command mode that handles
processing user interaction using the mouse in the viewports.\n\n
\sa  The Advanced Topics section
<a href="ms-its:3dsmaxsdk.chm::/ui_command_modes.html">Command Modes and Mouse
Procs</a>. There are methods in 3ds Max's
Interface class to set and get the current
command mode.  */
class CommandMode: public MaxHeapOperators {
	public:
		/*! \brief Destructor. */
		virtual ~CommandMode() {} 
		/*! \remarks Returns the Class of the command mode. The class describes the type of
		command mode this is. If the developer is defining a command mode to be
		used as part of the sub-object manipulation (Move, Rotate, and Scale)
		then one of the following pre-defined values should be used:\n\n
		<b>MOVE_COMMAND</b>\n\n
		<b>ROTATE_COMMAND</b>\n\n
		<b>SCALE_COMMAND</b>\n\n
		<b>USCALE_COMMAND</b>\n\n
		<b>SQUASH_COMMAND</b>\n\n
		If one of these other values is appropriate it may be used. If not, the
		developer is free to define their own (as an <b>int</b>).\n\n
		<b>VIEWPORT_COMMAND</b>\n\n
		<b>SELECT_COMMAND</b>\n\n
		<b>HIERARCHY_COMMAND</b>\n\n
		<b>CREATE_COMMAND</b>\n\n
		<b>MODIFY_COMMAND</b>\n\n
		<b>MOTION_COMMAND</b>\n\n
		<b>ANIMATION_COMMAND</b>\n\n
		<b>CAMERA_COMMAND</b>\n\n
		<b>NULL_COMMAND</b>\n\n
		<b>DISPLAY_COMMAND</b>\n\n
		<b>SPOTLIGHT_COMMAND</b>\n\n
		<b>PICK_COMMAND</b>
		\return  The Class of the command mode. */
		virtual int Class()=0;
		/*! \remarks This method can be ignored. The default implementation should be used.
		\par Default Implementation:
		<b>{ return 0; }</b> */
		virtual int SuperClass() { return 0; }
		/*! \remarks Returns the ID of the command mode. This value should be the constant
		<b>CID_USER</b> plus some random value chosen by the developer.\n\n
		As an example, this method could be implemented as:\n\n
		<b>{ CID_USER+0x1423; }</b>\n\n
		In the special case of the developer implementing a custom command mode
		to be used as part of sub-object manipulation (Move, Rotate or Scale)
		the value for ID() should be one of the following values:\n\n
		<b>CID_SUBOBJMOVE</b>\n\n
		<b>CID_SUBOBJROTATE</b>\n\n
		<b>CID_SUBOBJSCALE</b>\n\n
		<b>CID_SUBOBJUSCALE</b>\n\n
		<b>CID_SUBOBJSQUASH</b>\n\n
		Note: if two command modes conflict in this ID value, it is <b>not</b>
		a problem, so the uniqueness is not strictly required. However, this
		<b>ID()</b> method is often used to check which mode is active, so
		unless the value for your command mode is identifiable via this ID, you
		may not be able to recognize if your mode is indeed the active one. */
		virtual int ID()=0;
		/*! \remarks This method establishes the number of points required by the command
		mode and returns a pointer to the mouse callback object that is used to
		process the user input.
		\param numPoints This is where to store the number of points used by the CommandMode. If
		the plug-in needs to use an undetermined number of points it can
		specify a large number for this value. When the mouse proc has finished
		processing points it returns FALSE to stop the point processing before
		the number of points specified here have been entered.
		\return  A pointer to the mouse callback object that is used to process the user input.
		\sa  Class MouseCallBack. */
		virtual MouseCallBack *MouseProc(int *numPoints)=0;
		/*! \remarks Returns a pointer to a callback procedure that flags nodes that belong
		in the foreground plane. Plug-ins typically use a standard callback
		object provided by the system that flags all nodes dependent on the
		plug-in object. This ensures that when the plug-in object changes, any
		nodes that change as a result will be in the foreground plane, making
		redraw time faster.\n\n
		These constants may be specified to use one of the standard callback
		objects instead of an actual FG proc. For example <b>{return
		CHANGE_FG_SELECTED;}</b>\n\n
		<b>CHANGE_FG_SELECTED</b>\n\n
		Selected nodes are flagged.\n\n
		<b>CHANGE_FG_ANIMATED</b>\n\n
		Nodes that are animated are flagged.
		\return  A pointer to a callback procedure that flags nodes that belong
		in the foreground plane.\n\n
		\sa  For additional information see the Advanced Topics section
		<a href="ms-its:3dsmaxsdk.chm::/vports_foreground_background_planes.html">Foreground
		/ Background Planes</a>. */
		virtual ChangeForegroundCallback *ChangeFGProc()=0;
		/*! \remarks This method returns TRUE if the command mode needs to change the
		foreground proc (using <b>ChangeFGProc()</b>) and FALSE if it does not.
		A command mode that does not involve any redrawing of the viewports can
		just return FALSE.
		\param oldMode This is the command mode that is currently in place. This may be used
		for comparison with a potential new mode. See the sample code below.
		\par Sample Code:
		The sample code below checks to see if the command mode is already
		<b>CHANGE_FG_SELECTED</b>. If it is there is no reason to change to
		foreground proc to this mode so the method returns FALSE. If a
		different mode is in place TRUE is returned.
		\code
		BOL ChangeFG( CommandMode *oldMode ) { return (oldMode->ChangeFGProc() != CHANGE_FG_SELECTED); }
		ChangeForegroundCallback *ChangeFGProc() { return CHANGE_FG_SELECTED; }
		\endcode  */
		virtual BOOL ChangeFG( CommandMode *oldMode )=0;
		/*! \remarks This method is called when a command mode becomes active. Usually a
		developer responds by changing the state of a control to indicate to
		the user that they have entered the mode. Typically this means pushing
		in a tool button. When the mode is finished the button should be
		returned to normal (see <b>ExitMode()</b> below).\n\n
		Note: A developer should use the standard color <b>GREEN_WASH</b> for
		check buttons that instigate a command mode. While the command mode is
		active the button should be displayed in <b>GREEN_WASH</b>. See
		Class ICustButton (specifically
		the method <b>SetHighlightColor()</b>) for more details.
		\par Sample Code:
		<b>iPick-\>SetHighlightColor(GREEN_WASH);</b>\n\n
		<b>iPick-\>SetCheck(TRUE);</b> */
		virtual void EnterMode()=0;
		/*! \remarks This method is called when the active command mode is replaced by a
		different mode. Typically a developer implements this method to set the
		state of the control that invoked the command mode to the 'out' state.
		See Class ICustButton
		(specifically the method <b>SetCheck()</b>).
		\par Sample Code:
		<b>iPick-\>SetCheck(FALSE);</b> */
		virtual void ExitMode()=0;		
	};

// This is just a collection of modes that make up the xform modes.
// Plug-ins can specify these for thier sub-object types.
/*! \sa  <a href="ms-its:listsandfunctions.chm::/idx_R_list_of_standard_sub_object_modes.html">List of
Standard Sub-Object Modes</a>.\n\n
\par Description:
This class contains a set of six command mode pointers that make up the XForm
modes. Plug-in developers can specify these for their sub-object types. See the
method <b>ActivateSubobjSel</b> in 
Class BaseObject or 
class Control for more details.
\par Data Members:
<b>CommandMode *move;</b>\n\n
Standard command mode to process Move.\n\n
<b>CommandMode *rotate;</b>\n\n
Standard command mode to process Rotate.\n\n
<b>CommandMode *scale;</b>\n\n
Standard command mode to process Non-Uniform Scale.\n\n
<b>CommandMode *uscale;</b>\n\n
Standard command mode to process Uniform Scale.\n\n
<b>CommandMode *squash;</b>\n\n
Standard command mode to process Squash.\n\n
<b>CommandMode *select;</b>\n\n
Standard command mode to process Selection.  */
class XFormModes: public MaxHeapOperators {
	public:
		CommandMode *move;
		CommandMode *rotate;
		CommandMode *scale;
		CommandMode *uscale;
		CommandMode *squash;
		CommandMode *select;
		/*! \remarks Constructor. The data members are set to the command
		modes passed. */
		XFormModes( 
			CommandMode *move_,
			CommandMode *rotate_,
			CommandMode *scale_,
			CommandMode *uscale_,
			CommandMode *squash_,
			CommandMode *select_ )
			{
			this->move   = move_;
			this->rotate = rotate_;
			this->scale  = scale_;
			this->uscale = uscale_;
			this->squash = squash_;
			this->select = select_;
			}
		/*! \remarks Constructor. All the data members are set to NULL. */
		XFormModes() { move = rotate = scale = uscale = squash = select = NULL; } 
	};


// These can be returned from ChangeFGProc() instead of an actual FG proc
// to use predefined FG sets.
#define CHANGE_FG_SELECTED		((ChangeForegroundCallback *)1)
#define CHANGE_FG_ANIMATED		((ChangeForegroundCallback *)2)

// command super classes:
#define TRANSFORM_CMD_SUPER 	1

// command classes
#define VIEWPORT_COMMAND		1
#define MOVE_COMMAND				2
#define ROTATE_COMMAND			3
#define SCALE_COMMAND				4
#define USCALE_COMMAND			5
#define SQUASH_COMMAND			6
#define SELECT_COMMAND			7
#define HIERARCHY_COMMAND		8
#define CREATE_COMMAND			9
#define MODIFY_COMMAND			10
#define MOTION_COMMAND			11
#define ANIMATION_COMMAND		12
#define CAMERA_COMMAND			13
#define NULL_COMMAND				14
#define DISPLAY_COMMAND			15
#define SPOTLIGHT_COMMAND		16
#define PICK_COMMAND				17
#define MANIPULATE_COMMAND	18
#define PICK_EX_COMMAND			19 // Extended Pick Command Mode

// command IDs
#define CID_USER				0x0000ffff


/*! \defgroup stdCommandModes Standard Command Modes
These are the ID's to the standard Max-defined command modes.
\sa Class CommandMode, Interface::SetStdCommandMode */
//@{

#define CID_OBJMOVE				1		//!< \brief Max Transform Move Modes
#define CID_OBJROTATE			2		//!< \brief Max Transform Rotate Modes
#define CID_OBJSCALE			3		//!< \brief Max Transform Scale Modes
#define CID_OBJUSCALE			4		//!< \brief Max Transform Uniform Scale Modes
#define CID_OBJSQUASH			5		//!< \brief Max Transform Squash Modes
#define CID_OBJSELECT			6		//!< \brief Max Transform Select Modes


//! \brief The subobject command IDs are used internally and 
//! are not valid arguments to Interface::SetStdCommandMode
#define CID_SUBOBJMOVE			7		//!< This is not to be used externally
#define CID_SUBOBJROTATE		8		//!< This is not to be used externally
#define CID_SUBOBJSCALE			9		//!< This is not to be used externally
#define CID_SUBOBJUSCALE		10	//!< This is not to be used externally
#define CID_SUBOBJSQUASH		11	//!< This is not to be used externally
#define CID_SUBOBJSELECT		12	//!< This is not to be used externally


//! \brief display branch command modes - These are also
//! used internally and are not valid to SetStdCommandMode
#define CID_UNFREEZE			13		//!< This is not to be used externally
#define CID_UNHIDE				14		//!< This is not to be used externally

//! \brief Hierarchy Commands
#define CID_LINK				100
#define CID_BINDWSM				110		// I guess this is a heirarchy command... sort of

//! \brief Viewport Commands
#define CID_ZOOMVIEW			200
#define CID_ZOOMREGION			201
#define CID_PANVIEW				202
#define CID_ROTATEVIEW			203
#define CID_ZOOMALL				204
#define CID_RNDREGION			205

//! \brief Camera Commands
#define CID_CAMFOV				210
#define CID_CAMDOLLY			211
#define CID_CAMPERSP			212
#define CID_CAMTRUCK			213
#define CID_CAMROTATE			214
#define CID_CAMROLL				215

//ANIMATION_COMMAND
#define CID_PLAYANIMATION		300

//@}

//CREATE_COMMAND		
#define CID_SIMPLECREATE		400

//MODIFIY_COMMAND
#define CID_MODIFYPARAM			500
//! Command Mode Id for the Edit Soft Selection Command Mode
//! \see EditSSMode
#define CID_EDITSOFTSELECTION	525

//MOTION_COMMAND	

#define CID_NULL				600

// Pick modes
#define CID_STDPICK				710
#define CID_PICKAXISOBJECT		700

// ATTACH To GROUP COMMAND
#define CID_GRP_ATTACH			800
#define CID_ASSEMBLY_ATTACH	810

// Manipulate Command Mode
#define CID_MANIPULATE          900

// Special Command IDs used internally by the transform gizmo
// These are not to be used by third party developers
#define CID_FREE_AXIS_ROTATE	-INT_MAX
#define CID_SCREEN_SPACE_ROTATE -INT_MAX+1

//-----------------------------------------------------------------------------
// Derive your command mode class from this one if you wish to implement your 
// own pick command mode and want to hook it into the select by name mechanism
class PickCommandMode : public CommandMode
{
	public:
		virtual ~PickCommandMode() { };
		virtual int Class() { return PICK_EX_COMMAND; }
		virtual HitByNameDlgCallback* GetHitByNameDlgCallback() = 0;
		virtual PickModeCallback* GetPickModeCallback() = 0;
		virtual PickNodeCallback* GetPickNodeCallback() = 0;
};

#endif // __CMDMODE

