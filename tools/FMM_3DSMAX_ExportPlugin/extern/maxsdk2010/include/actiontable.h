//**************************************************************************/
// Copyright (c) 1998-2006 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
// FILE:        ActionTable.h
// DESCRIPTION: Action Table definitions
// AUTHOR:      Scott Morrison
// HISTORY:     Created 8 February, 2000, Based on KbdAction.h.
//**************************************************************************/

// The ActionTable class is used by plug-ins (and core) to export
// tables of items that can be used by the UI to attach to keyboard
// shorcuts, assign to toolbar buttons, and add to menus.
#ifndef _ACTIONTABLE_H_
#define _ACTIONTABLE_H_

#include "maxheap.h"
#include "stack.h"
#include "iFnPub.h"
#include "strbasic.h"            // MCHAR

class MaxIcon;
class MacroEntry;

typedef DWORD ActionTableId;
typedef DWORD ActionContextId;

// ActionTableIds used by the system
const ActionTableId kActionMainUI           = 0;
const ActionTableId kActionTrackView        = 1;
const ActionTableId kActionMaterialEditor   = 2;
const ActionTableId kActionVideoPost        = 3;
const ActionTableId kActionSchematicView    = 5;
const ActionTableId kActionCommonIReshade   = 6;
const ActionTableId kActionScanlineIReshade = 7;

class ActionTable;
class IMenu;

// Context IDs used by the system.  Several tables may share the same context id.
const ActionContextId kActionMainUIContext         = 0;
const ActionContextId kActionTrackViewContext      = 1;
const ActionContextId kActionMaterialEditorContext = 2;
const ActionContextId kActionVideoPostContext      = 3;
const ActionContextId kActionSchematicViewContext  = 5;
const ActionContextId kActionIReshadeContext       = 6;

// Description of a command for building action tables from static data

/*! \sa  Class ActionTable, Class ActionItem, Class ActionCallback.
\par Description:
This structure is available in release 4.0 and later only. \n\n
This is a helper structure used for building ActionTables. A static array 
of these descriptors is passed to the ActionTable constructor.
*/
struct ActionDescription {

	/*! A unique identifier for the command (must be unique per table).
	When an action is executed this is the command ID passed to ActionCallback::ExecuteAction().
	*/
	int mCmdID;

	/*! A string resource id that describes the command. */
	int mDescriptionResourceID;

    /*! A string resource ID for a short name for the action. This name
    appears in the list of Actions in the Customize User Interface dialog. */
    int mShortNameResourceID;

    /*! A string resource ID for the category of an operation. This name appears in
    the Category drop down list in the Customize User Interface dialog. */
    int mCategoryResourceID;

};

#define AO_DEFAULT 0x0001 //default option command to execute
#define AO_CLOSEDIALOG 0x0002 //Execute closeDialog option

#define ACTION_OPTION_INTERFACE Interface_ID(0x3c0276f5, 0x190964f5)
#define ACTION_OPTION_INTERFACE_OPT2 Interface_ID(0x0011dcdc, 0x0012dcdc)
// Implement if the an action item supports an alternate options command,
// overwrite ActionItem::GetInterface(ACTION_OPTION_INTERFACE), to return an instance of this class
class IActionOptions : public BaseInterface
{

public:
	virtual BOOL ExecuteOptions(DWORD options = AO_DEFAULT) = 0;
};

/*! \brief The class ActionItem is used to represent the operation that live in ActionTables. 
	\remarks ActionItem is an abstract class with operations to support various UI operations. 
	The system provides a default implementation of this class that works when the table is 
	build with the ActionTable::BuildActionTable() method. However, developers may want to
	specialize this class for more special-purpose applications. For example, MAXScript does 
	this to export macroScripts to an ActionTable. Methods that are marked as internal should 
	not be used. This class is available in release 4.0 and later only.
	\sa  Class ActionTable, Class ActionCallback, Structure ActionDescription, Class ActionContext, 
	Class IActionManager, Class DynamicMenu, Class DynamicMenuCallback, Class MAXIcon, Class Interface.*/
class ActionItem : public InterfaceServer
{
public:
	//! \brief Constructor
	CoreExport ActionItem();
	//! \brief virtual Destructor
	CoreExport virtual ~ActionItem();

    /*! \remarks This method retrieves the unique identifier for the action.
    This action must be unique in the table, but not does not have to be unique
    between tables. */
    virtual int    GetId() = 0;

    /*! \remarks Calling ExecuteAction causes the item to be run. This returns
    a BOOL that indicates if the action was actually executed. If the item is
    disabled, or if the table that owns it is not activated, then it won't
    execute, and returns FALSE.
    \return  TRUE if the action is executed; otherwise FALSE. */
    virtual BOOL ExecuteAction() = 0;

	//! \brief Override this if you wish to customize macroRecorder output for this action
	CoreExport virtual void EmitMacro();

	//! \brief Handles macroRecording, etc. and call ExecuteAction()
	//! \remarks Internal only.
	CoreExport BOOL Execute(); 

    /*! \remarks This method retrieves the text that will be used when the
	ActionItem is on a text button. The text is stored into the buttonText parameter.
	\param buttonText - Storage for the retrieved text. */
    virtual void GetButtonText(MSTR& buttonText) = 0;

    /*! \remarks This method retrieves the text to use when the item is on a
    menu (either Quad menu or main menu bar). This can be different from the
    button text. This method is called just before the menu is displayed, so it
    can update the text at that time. For example, the "Undo" menu item in 3ds
    Max's main menu adds the name of the command that will be undone.
    \param menuText - Storage for the retrieved text. */
    virtual void GetMenuText(MSTR& menuText) = 0;

    /*! \remarks This method gets the text that will be used for tool tips and
    menu help. This is also the string that is displayed for the operation in
    all the lists in the customization dialogs.
    \param descText - Storage for the retrieved text. */
    virtual void GetDescriptionText(MSTR& descText) = 0;

    /*! \remarks This method retrieves the text for the category of the
    operation. This is used in the customization dialog to fill the "category"
    drop-down list.
    \param catText - Storage for the retrieved text. */
    virtual void GetCategoryText(MSTR& catText) = 0;

    // check to see if menu items should be checked, or button pressed
    /*! \remarks This method determines if the action is in a "Checked" state.
    For menus, this means that this function will return TRUE if a check mark 
	appears next to the item. If the item is on a button, this is used to determine of
    the button is in the "Pressed" state. Note that button states are
    automatically updated on selection change and command mode changes. If your
    plug-in performs an operation that requires the CUI buttons to be redrawn,
    you need to call the method CUIFrameMgr::SetMacroButtonStates(TRUE).
	\return TRUE if menu item is checked, or if a button is 'pressed'. FALSE otherwise*/
    virtual BOOL IsChecked() = 0;

    /*! \remarks This method determines if an item is visible on a context menu. If it
    returns FALSE, then the item is not included in the menu. This can be used
    to create items that a context sensitive. For example, you may want an item
    to appear on a menu only when the selected object is of a particular type.
    To do this, you have this method check the class id of the current
    selection. */
    virtual BOOL IsItemVisible() = 0;

    // Check to see if menu item should be enabled in a menu
    /*! \remarks This method determines if the operation is currently
    available. If this method returns FALSE, and the ActionItem it is on a 
	menu, or button, the item is grayed out. If it assigned to a keyboard 
	shortcut, then it will not execute the operation if invoked. If your plugin 
	performs an operation that requires the CUI buttons to be redrawn, you need 
	to call the method CUIFrameMgr::SetMacroButtonStates(TRUE)
    \return  TRUE for enabled; FALSE for disabled. */
    virtual BOOL IsEnabled() = 0;
    
    /*! \remarks If an icon is available for this operation, then return it
    with this method. If no icon is available, this returns NULL. The icon is
    used on CUI buttons, and in the list of operations in the customization
    dialogs. */
    virtual MaxIcon* GetIcon() = 0;

    /*! \remarks Called to delete the ActionItem. This normally happens when
    the table that owns it is deleted. */
    virtual void DeleteThis() = 0;

    /*! \remarks This returns a pointer to the table that owns the ActionItem.
    An item can only be owned by a single table.
    Default Implementation
	\code
		{ return mpTable; }
	\endcode */
    CoreExport ActionTable* GetTable();

    /*! \remarks Sets the table that owns the item. Used internally. May be
    used if you implement a custom sub-class of ActionItem.
    \param pTable - Points to the table to set.
    Default Implementation:
    \code 
		{ mpTable = pTable; }
	\endcode */
    CoreExport void SetTable(ActionTable* pTable);

    /*! \remarks Returns the string that describes all the keyboard shortcuts
    associated with the item. This will look something like "Alt+A" or "C,
    Shift+Alt+Q". 
	\return NULL if no keyboard shortcut is associated with the item. */
    CoreExport MCHAR* GetShortcutString();

	/*! \remarks Returns the representation of the macroScript for the item,
	if it's implemented by a macroScript, it returns NULL otherwise.
	\return Default Implementation
	\code 
	{ return NULL; }
	\endcode */
	CoreExport virtual MacroEntry* GetMacroScript();

    /*! \remarks Determines if a menu is created or if an action takes place.
    If this method returns TRUE, then the ActionItem creates a menu. If it
    returns FALSE then an action is performed.
    \return Default Implementation
    \code 
	{ return FALSE; }
	\endocode */
    CoreExport virtual BOOL IsDynamicMenu();

    /*! \remarks This method may be called after an action item is created to
    tell the system that it is a dynamic menu action. 
	\Note Dynamic menus may be added to the quad menus procedurally (via 
	the IMenuManager API) or 'manually'. */
    CoreExport virtual void SetIsDynamicMenu();

    /*! \remarks If the ActionItem does produce a menu, this method is called
    to return a pointer to the menu. See class DynamicMenu for an easy way to
    produce these menus.
    \param HWND hwnd -  If the menu is requested by a right-click quad menu, 
		then this hwnd is the handle of the window where the click occurred. 
		If the item is used from a menu bar, this hwnd will be NULL.
    \param m -  If the menu is requested by a right-click quad menu, then this parameter is
		the point in the window where the user clicked.
    \return  A pointer to the menu. Default Implementation:
    \code 
		{ return NULL; }
	\endcode*/
    CoreExport virtual IMenu* GetDynamicMenu(HWND hwnd, IPoint2& m);

    //! \remarks ActionItems that are deleted after they execute should return TRUE.
    CoreExport virtual BOOL IsDynamicAction();

protected:
	//! \brief Points to the table that owns the action. 
	//! \note This does NOT 'own' the memory for the ActionTable pointer
    ActionTable* mpTable;
};

//! \brief Deprecated. For internal use only. To be removed from the SDK.
#define ACTIONITEM_STANDIN_INTERFACE Interface_ID(0x108e1314, 0x5aff3138)

//! \brief Deprecated. For internal use only. To be removed from the SDK.
class IActionItemStandin : public BaseInterface
{
public:
	virtual void          SetPersistentActionId(MSTR idString) = 0;
	virtual MSTR&         GetPersistentActionId() = 0;
	virtual void          SetActionTableId( ActionTableId id ) = 0;
	virtual ActionTableId GetActionTableId() = 0;
	virtual MSTR&         GetPrefixString() = 0;
};

//! \brief Deprecated. For internal use only. To be removed from the SDK.
inline IActionItemStandin* GetIActionItemStandin(ActionItem* a) { return (IActionItemStandin*)a->GetInterface(ACTIONITEM_STANDIN_INTERFACE); }

//! \brief Deprecated. For internal use only. To be removed from the SDK.
class ActionItemStandin: public ActionItem, public IActionItemStandin
{
public:
	CoreExport ActionItemStandin(int cmdId );
	CoreExport virtual ~ActionItemStandin();

	CoreExport virtual int  GetId() { return mCmdId; }
	CoreExport virtual void SetId(int id) { mCmdId = id; }

	CoreExport virtual MCHAR*  GetDescription() { return mName.data();}
	CoreExport virtual void    SetDescription(MCHAR* pDesc) { mName = pDesc; }

	CoreExport virtual MCHAR*  GetShortName() { return mName.data();}
	CoreExport virtual void    SetShortName(MCHAR* pShortName) { mName = pShortName; }

	CoreExport virtual MCHAR*  GetCategory() { return mName.data();}
	CoreExport virtual void    SetCategory(MCHAR* pCategory) { mName = pCategory; }

	CoreExport virtual MaxIcon* GetIcon() { return NULL; };

	// return true if action executed, and FALSE otherwise
	CoreExport virtual BOOL ExecuteAction() { return FALSE; };

	// Get the text to put on a button
	CoreExport virtual void GetButtonText(MSTR& buttonText) { buttonText = mName; };
	// Get the text to use in a menu
	CoreExport virtual void GetMenuText(MSTR& menuText) { menuText = mName; };
	// Get the long description text for tool tips etc.
	CoreExport virtual void GetDescriptionText(MSTR& descText) { descText = mName; };
	// Get the string describing the category of the item
	CoreExport virtual void GetCategoryText(MSTR& catText) { catText = mName; };

	// check to see if menu items should be checked, or button pressed
	CoreExport virtual BOOL IsChecked() { return FALSE; };
	// Check to see if menu item should show up in context menu
	CoreExport virtual BOOL IsItemVisible() { return TRUE; };
	// Check to see if menu item should be enabled in a menu
	CoreExport virtual BOOL IsEnabled() { return FALSE; };

	CoreExport virtual BOOL IsDynamicMenu() { return FALSE; }
	CoreExport virtual void SetIsDynamicMenu() { };
	CoreExport virtual IMenu* GetDynamicMenu(HWND hwnd, IPoint2& m) { return NULL; };

	CoreExport void DeleteThis();

	CoreExport virtual BaseInterface* GetInterface(Interface_ID id);

	// from IActionItemStandin
	CoreExport virtual void SetPersistentActionId(MSTR idString);
	CoreExport virtual MSTR& GetPersistentActionId() { return mPersistentActionId; };
	CoreExport virtual void SetActionTableId( ActionTableId id ) { mId = id; };
	CoreExport virtual ActionTableId GetActionTableId() { return mId; };
	CoreExport virtual MSTR& GetPrefixString() { return mPrefix; };

protected:
	int   mCmdId;
	MSTR  mName;
	MSTR  mPersistentActionId;
	MSTR  mPrefix;
	ActionTableId mId;
};

class ActionCallback;

// A table of actions that can be tied to UI elements (buttons, menus, RC menu,
// keyboard shortcuts)

/*! \sa  Class BaseInterfaceServer,  Class ClassDesc, Structure ActionDescription, Class ActionItem, Class ActionCallback, Class ActionContext, Class IActionManager, Class DynamicMenu, Class DynamicMenuCallback, Class Interface.\n\n
\par Description:
This class is available in release 4.0 and later only.\n\n
This is the class used to create Action Tables. An ActionTable holds a set of
ActionItems, which are operations that can be tied to various UI elements, such
as keyboard shortcuts, CUI buttons, the main menu and the Quad menu. 3ds Max's
core code exports several ActionTables for built-in operations in 3ds Max.
Plug-ins can also export their own action tables via methods available in
ClassDesc.\n\n
All methods of this class are implemented by the system. Note however that many
methods are virtual and may be customized by the plug-in developer as this
class may be sub-classed if required. See the Advanced Topics section
<a href="ms-its:3dsMaxSDK.chm::/ui_customization_root.html">UI
Customization</a> for details on sub-classing this class and ActionItem. For
details on implementing an ActionTable please refer to
<b>/MAXSDK/SAMPLES/MODIFIERS/FFD</b>  */
class ActionTable : public BaseInterfaceServer {

public:
    /*! \remarks Constructor. This constructor builds the action table using
    an array of descriptors. It takes the ID of the table, the context id, a
    name for the table, a windows accelerator table that gives default keyboard
    assignments for the operations, the number of items, the table of operation
    descriptions, and the instance of the module where the string resources in
    the table are stored.\n\n
    At the same time the action table is built developers need to register the
    action context ID with the system. This is done using the
    <b>IActionManager::RegisterActionContext()</b> method.
    \par Parameters:
    <b>ActionTableId id</b>\n\n
    The unique ID for the ActionTable. Every ActionTable has a unique 32-bit
    integer ID. For new tables exported by plug-ins, the developer should
    choose a random 32-bit integer. You can use the Class_ID program to
    generate this identifier: See Class
    Class_ID for more details. Simply use one of the two DWORDs that
    comprise the Class_ID for the ActionTableId.\n\n
    <b>ActionContextId contextId</b>\n\n
    The ActionContextID associated with this table. Several tables may share
    the same ActionContextID.\n\n
    <b>MSTR\& name</b>\n\n
    The name for the ActionTable.\n\n
    <b>HACCEL hDefaults</b>\n\n
    The handle of the a windows accelerator table that gives default keyboard
    assignments for the operations.\n\n
    <b>int numIds</b>\n\n
    The number of items in the description array below.\n\n
    <b>ActionDescription* pOps</b>\n\n
    Points to the array of the operator descriptors.\n\n
    <b>HINSTANCE hInst</b>\n\n
    The handle to the instance of the module where the string resources in the
    array of operator descriptors are stored. */
    CoreExport ActionTable(ActionTableId id,
                           ActionContextId contextId,
                           MSTR& name,
                           HACCEL hDefaults,
                           int numIds,
                           ActionDescription* pOps,
                           HINSTANCE hInst);
    /*! \remarks Constructor. This constructor build a new empty action table
    with the given ID, context ID and name. You then need to add ActionItems to
    the table separately using the <b>AppendOperation()</b> method described
    below.
    \par Parameters:
    <b>ActionTableId id</b>\n\n
    The unique ID for the ActionTable.\n\n
    <b>ActionContextId contextId</b>\n\n
    The ActionContextID associated with this table. Several tables may share
    the same ActionContextID.\n\n
    <b>MSTR\& name</b>\n\n
    The name for the ActionTable. */
    CoreExport ActionTable(ActionTableId id,
                           ActionContextId contextId,
                           MSTR& name);

    /*! \remarks Destructor. Deletes all the operations maintained by the
    table and deletes the keyboard accelerator table if in use. */
    CoreExport virtual ~ActionTable();

    // Get/Set the current keyboard accelerators for the table
    /*! \remarks Returns the handle of the current keyboard accelerator for
    the table. */
    CoreExport HACCEL GetHAccel();
    /*! \remarks Sets the current keyboard accelerator for the table. */
    CoreExport void SetHAccel(HACCEL hAccel);
    // Get the default keyboard accelerator table.  This is used when
    // the user has not assigned any accelerators.
    /*! \remarks Get the default keyboard accelerator table. This is used when
    the user has not assigned any accelerators. */
    CoreExport HACCEL GetDefaultHAccel();
    CoreExport void SetDefaultHAccel(HACCEL accel);

    CoreExport MSTR& GetName();
    CoreExport ActionTableId GetId();
    /*! \remarks Returns the ActionContextId for this ActionTable. */
    CoreExport ActionContextId GetContextId();

    // Get the current callback assocuated with this table.
    // returns NULL if the table is not active.
    /*! \remarks Get the current callback associated with this table. Returns
    NULL if the table is not active. */
    CoreExport ActionCallback* GetCallback();
    /*! \remarks Sets the callback object used by this ActionTable.
    \par Parameters:
    <b>ActionCallback* pCallback</b>\n\n
    Points to the callback to set. */
    CoreExport void SetCallback(ActionCallback* pCallback);

    // Methods to iterate over the actions in the table
    /*! \remarks Returns the number of ActionItems in the table. */
    CoreExport int Count();
    /*! \remarks This operator returns a pointer to the 'i-th' ActionItem.
    \par Parameters:
    <b>int i</b>\n\n
    The zero based index in the list of ActionItems. */
    CoreExport ActionItem* operator[](int i);

    // Get an action by its command id.
    /*! \remarks Returns a pointer to the ActionItem associated with the
    command ID passed.
    \par Parameters:
    <b>int cmdId</b>\n\n
    The command ID. */
    CoreExport ActionItem* GetAction(int cmdId);

    // Add an operation to the table
    /*! \remarks This method adds an operation to the table.
    \par Parameters:
    <b>ActionItem* pAction</b>\n\n
    Points to the ActionItem to append. */
    CoreExport void AppendOperation(ActionItem* pAction);
    // Remove an operation from the table
    /*! \remarks Remove an operation from the table
    \par Parameters:
    <b>ActionItem* pAction</b>\n\n
    Points to the ActionItem to delete.
    \return  TRUE if the operation was deleted; FALSE if it could not be found
    and wasn't. */
    CoreExport BOOL DeleteOperation(ActionItem* pAction);

    /*! \remarks Deletes this ActionItem.
    \par Default Implementation:
    <b>{ delete this; }</b> */
    CoreExport virtual void DeleteThis(); 

    // Get the text to put on a button
    /*! \remarks This method retrieves the text that will be used when the
    ActionItem is on a text button.
    \par Parameters:
    <b>int cmdId</b>\n\n
    The unique ID of the command whose button text is retrieved.\n\n
    <b>MSTR\& buttonText</b>\n\n
    Storage for the text.
    \return  TRUE if the command is in the table; otherwise FALSE. */
    CoreExport virtual BOOL GetButtonText(int cmdId, MSTR& buttonText);
    // Get the text to use in a menu
    /*! \remarks This method retrieves the text to use when the item is on a
    menu (either Quad menu or main menu bar). This can be different from the
    button text.
    \par Parameters:
    <b>int cmdId</b>\n\n
    The unique ID of the command whose menu text is retrieved.\n\n
    <b>MSTR\& menuText</b>\n\n
    Storage for the text.
    \return  TRUE if the command is in the table; otherwise FALSE.
    \par Default Implementation:
    <b>{ return GetButtonText(cmdId, menuText); }</b> */
    CoreExport virtual BOOL GetMenuText(int cmdId, MSTR& menuText);

    // Get the long description text for tool tips etc.
    /*! \remarks This method gets the text that will be used for tool tips and
    menu help. This is also the string that is displayed for the operation in
    all the lists in the customization dialogs.
    \par Parameters:
    <b>int cmdId</b>\n\n
    The unique ID of the command whose description text is retrieved.\n\n
    <b>MSTR\& descText</b>\n\n
    Storage for the text.
    \return  TRUE if the command is in the table; otherwise FALSE. */
    CoreExport virtual BOOL GetDescriptionText(int cmdId, MSTR& descText);

    // check to see if menu items should be checked, or button pressed
    /*! \remarks Returns TRUE if the menu item should be checked or a CUI
    button should be in the pressed state.
    \par Parameters:
    <b>int cmdId</b>\n\n
    The unique ID of the command.
    \par Default Implementation:
    <b>{ return FALSE; }</b> */
    CoreExport virtual BOOL IsChecked(int cmdId);
    // Check to see if menu item should show up in context menu
    /*! \remarks This method determines if an item is to be visible on a menu.
    Returns TRUE if visible; FALSE if not.
    \par Parameters:
    <b>int cmdId</b>\n\n
    The unique ID of the command.
    \par Default Implementation:
    <b>{ return TRUE; }</b> */
    CoreExport virtual BOOL IsItemVisible(int cmdId);
    // Check to see if menu item should be enabled in a menu
    /*! \remarks This method determines if the operation is currently enabled
    and available. Returns TRUE if enabled; FALSE if disabled.
    \par Parameters:
    <b>int cmdId</b>\n\n
    The unique ID of the command.
    \par Default Implementation:
    <b>{ return TRUE; }</b> */
    CoreExport virtual BOOL IsEnabled(int cmdId);

    // Write an action identifier to a CUI file or KBD file
    // Default implementation is to write the integer ID.
    // This is over-riden when command IDs are not persistent
    /*! \remarks This method will write an action identifier to a *.CUI file
    or *.KBD file. It's default implementation is to write the integer ID but
    will be over-riden when command IDs are not persistent.
    \par Parameters:
    <b>int cmdId</b>\n\n
    The unique ID of the command.\n\n
    <b>MSTR\& idString</b>\n\n
    The action ID placed in the string. */
    CoreExport virtual void WritePersistentActionId(int cmdId, MSTR& idString);
    // Read an action identifier from a CUI file or KBD file
    // Default implementation is to read the integer ID.
    // This is over-riden when command IDs are not persistent
    // Returns -1 if the command is not found in the table
    /*! \remarks This method will read an action identifier from a *.CUI file
    or *.KBD file. It's default implementation is to read the integer ID but
    will be over-riden when command IDs are not persistent.
    \par Parameters:
    <b>MSTR\& idString</b>\n\n
    The action ID string.
    \return  This method returns -1 if the command was not found in the table.
    */
    CoreExport virtual int ReadPersistentActionId(MSTR& idString);

    // return an optional icon for the command
    /*! \remarks Returns an optional icon for the command, or NULL if there is
    none.
    \par Parameters:
    <b>int cmdID</b>\n\n
    The unique ID of the command. */
    CoreExport virtual MaxIcon* GetIcon(int cmdId);

    // Fill the action table with the given action descriptions
    /*! \remarks This method will fill the action table with the given action
    descriptions.
    \par Parameters:
    <b>HACCEL hDefaults</b>\n\n
    The handle of the a windows accelerator table that provides keyboard
    assignments for the operations.\n\n
    <b>int numIds</b>\n\n
    The number of ID's to add to the action table.\n\n
    <b>ActionDescription* pOps</b>\n\n
    The array of action descriptions to build the table from.\n\n
    <b>HINSTANCE hInst</b>\n\n
    The handle to the instance of the module. */
    CoreExport void BuildActionTable(HACCEL hDefaults,
                                int numIds,
                                ActionDescription* pOps,
                                HINSTANCE hInst);

    // Get the action assigned to the given accelerator, if any
    /*! \remarks Get the action assigned to the given accelerator, if any.
    \par Parameters:
    <b>ACCEL accel</b>\n\n
    The accelerator key you wish to check the assignment for. */
    CoreExport ActionItem* GetCurrentAssignment(ACCEL accel);
    // Assign the command to th given accelerator.  Also removes any
    // previous assignment to that accelerator
    /*! \remarks Assign the command to the given accelerator. Also removes any
    previous assignment to that accelerator.
    \par Parameters:
    <b>int cmdId</b>\n\n
    The command ID.\n\n
    <b>ACCEL accel</b>\n\n
    The accelerator key you wish to assign. */
    CoreExport void AssignKey(int cmdId, ACCEL accel);
    // removes the given assignment from the shortcut table
    /*! \remarks removes the given assignment from the shortcut table
    \par Parameters:
    <b>ACCEL accel</b>\n\n
    The accelerator key you wish to remove from the shortcut table. */
    void RemoveShortcutFromTable(ACCEL accel);

private:
    // These values are set by the plug-in to describe a action table

    // Unique identifier of table (like a class id)
    ActionTableId  mId;

    // An identifier to group tables use the same context.  Tables with the
    // same context cannot have overlapping keyboard shortcuts.
    ActionContextId mContextId;

    // Name to use in preference dlg drop-down
    MSTR mName;

    // Descriptors of all operations that can have Actions
    Tab<ActionItem*>  mOps; 

    // The windows accelerator table in use when no keyboard shortcuts saved
    HACCEL mhDefaultAccel;
    // The windows accelerator table in use
    HACCEL mhAccel;

    // The currently active callback
    ActionCallback* mpCallback;
};


/*! \sa  Class BaseInterfaceServer,  Class ActionTable,  Class ActionItem, Class ActionContext, Class IActionManager, Class DynamicMenu, Class DynamicMenuCallback, Class Interface.\n\n
\par Description:
This class is available in release 4.0 and later only.\n\n
An important part of implementing an ActionTable is creating a sub-class of
this class. This is an abstract class with a virtual method called
ExecuteAction(int id). Developers need to sub-class this class and pass an
instance of it to the system when they activate an ActionTable. Then when the
system wants to execute an action the ExecuteAction() method is called.\n\n
All methods of this class are virtual. */
class ActionCallback : public BaseInterfaceServer {
public:
	CoreExport ActionCallback();
	CoreExport virtual ~ActionCallback();
    /*! \remarks This method is called to actually execute the action.
    \par Parameters:
    <b>int id</b>\n\n
    The ID of the item to execute.
    \return  This returns a BOOL that indicates if the action was actually
    executed. If the item is disabled, or if the table that owns it is not
    activated, then it won't execute, and returns FALSE. If it does execute
    then TRUE is returned.
    \par Default Implementation:
    <b>{ return FALSE; }</b> */
    CoreExport virtual BOOL ExecuteAction(int id);
    // called when an action item says it is a dynamic menu
    /*! \remarks This method is called when an action item says it is a
    dynamic menu. This returns a pointer to the menu itself.
    \par Parameters:
    <b>int id</b>\n\n
    The item ID which is passed to the
    <b>DynamicMenuCallback::MenuItemSelected()</b>\n\n
    <b>HWND hwnd</b>\n\n
    If the menu is requested by a right-click quad menu, then hwnd is the
    window where the click occurred. If the item is used from a menu bar, then
    hwnd will be NULL.\n\n
    <b>IPoint2\& m</b>\n\n
    If the menu is requested by a right-click quad menu, then this will be the
    point in the window where the user clicked.
    \par Default Implementation:
    <b>{ return NULL; }</b> */
    CoreExport virtual IMenu* GetDynamicMenu(int id, HWND hwnd, IPoint2& m);

    // Access to the table the callback uses
    /*! \remarks Returns a pointer to the ActionTable the callback uses.
    \par Default Implementation:
    <b>{ return mpTable; }</b> */
    CoreExport ActionTable* GetTable();
    /*! \remarks Sets the ActionTable the callback uses.
    \par Parameters:
    <b>ActionTable* pTable</b>\n\n
    Points to the ActionTable the callback uses.
    \par Default Implementation:
    <b>{ mpTable = pTable; }</b> */
    CoreExport void SetTable(ActionTable* pTable);

private:
	//! \brief Points to the table that uses this ActionCallback.
    ActionTable *mpTable;
};

// An ActionContext is an identifer of a group of keyboard shortcuts.
// Examples are Main UI, Tack View, and Editable Mesh.  They are
// registered using Interface::RegisterActionContext().
//
/*! \sa  Class ActionTable, Class ActionItem, Class ActionCallback, Class IActionManager, Class DynamicMenu, Class DynamicMenuCallback, Class Interface.\n\n
\par Description:
This class is available in release 4.0 and later only.\n\n
Every ActionTable also has an ActionContextId associated with it. This
ActionContextId can be shared with other tables.\n\n
When assigning keyboard shortcuts to items in an ActionTable, tables that share
a unique context id are forced to have unique shortcuts. Tables with different
context ids can have overlapping keyboard shortcut assignments.\n\n
An ActionContext is an identifer of a group of keyboard shortcuts. Examples are
the Main 3ds Max UI, Track %View, and the Editable Mesh. They are registered
using <b>IActionManager::RegisterActionContext()</b>.\n\n
Note: <b>typedef DWORD ActionContextId;</b>  */
class ActionContext: public MaxHeapOperators {
public:
    /*! \remarks Constructor. The context ID and the name are initialized from
    the data passed.
    \par Parameters:
    <b>ActionContextId contextId</b>\n\n
    The ID for the ActionContext.\n\n
    <b>MCHAR *pName</b>\n\n
    The name for the ActionContext. */
    ActionContext(ActionContextId contextId, MCHAR *pName)
        { mContextId = contextId; mName = pName; mActive = true; }

    /*! \remarks Returns the name of this ActionContext. */
    MCHAR* GetName() { return mName.data(); }
    /*! \remarks Returns the ID of this ActionContext. */
    ActionContextId GetContextId() { return mContextId; }

    /*! \remarks Returns true if this ActionContext is active; otherwise
    false. An active ActionContext means that it uses its keyboard
    accelerators. This corresponds to the "Active" checkbox in the keyboard
    customization UI. */
    bool IsActive() { return mActive; }
    /*! \remarks Sets the active state of this ActionContext.
    \par Parameters:
    <b>bool active</b>\n\n
    Pass true for active; false for inactive. */
    void SetActive(bool active) { mActive = active; }
    
private:
    ActionContextId  mContextId;
    MSTR             mName;
    bool             mActive;
};

// The ActionManager manages a set of ActionTables, callbacks and ActionContexts.
// The manager handles the keyboard accelerator tables for each ActionTable
// as well.  You get a pointer to this class using Interface::GetActionManager().

#define ACTION_MGR_INTERFACE  Interface_ID(0x4bb71a79, 0x4e531e4f)

/*! \sa  Class ActionTable, Class ClassDesc, Class ActionItem, Class ActionCallback, Class ActionContext, Class DynamicMenu, Class DynamicMenuCallback, Class Interface.\n\n
\par Description:
This class is available in release 4.0 and later only.\n\n
The ActionManager manages a set of ActionTables, callbacks and ActionContexts.
The manager handles the keyboard accelerator tables for each ActionTable as
well. You get a pointer to this class using
<b>Interface::GetActionManager()</b>.  */
class IActionManager : public FPStaticInterface  {

public:
    // Register an action table with the manager.
    // Note that most plug-ins will not need this method.  Instead,
    // plug-ins export action table with the methods in ClassDesc.
    /*! \remarks Register an action table with the manager. Note that most
    plug-ins will not need this method. Instead, plug-ins export action table
    with the methods in ClassDesc. See
    <a href="class_class_desc.html#A_GM_cldesc_actiontable">ClassDesc Action
    Table Methods</a>.
    \par Parameters:
    <b>ActionTable* pTable</b>\n\n
    Points to the Action Table to register. */
    virtual void RegisterActionTable(ActionTable* pTable) = 0;

    // Methods to iterate over the action table
    /*! \remarks Returns the number of ActionTables. */
    virtual int NumActionTables() = 0;
    /*! \remarks Returns a pointer to the 'i-th' ActionTable.
    \par Parameters:
    <b>int i</b>\n\n
    The zero based index of the table. */
    virtual ActionTable* GetTable(int i) = 0;

    // These methods are used to turn a table on and off.
    /*! \remarks This method is called to activate the action table. Some
    plug-ins (for instance Modifiers or Geometric Objects) may only want to
    activate the table when they are being edited in the command panel (between
    BeginEditParams() and EndEditParams()). Others, for instance Global Utility
    Plug-ins, may wish to do so when they are initially loaded so the actions
    are always available.\n\n
    Note that if this method is called multiple times, only the callback from
    the last call will be used.
    \par Parameters:
    <b>ActionCallback* pCallback</b>\n\n
    Points to the callback object which is responsible for executing the
    action.\n\n
    <b>ActionTableId id</b>\n\n
    This is the ID of the table to activate.
    \return  TRUE if the action table was activated. FALSE if the table is
    already active or doesn't exist. */
    virtual int ActivateActionTable(ActionCallback* pCallback, ActionTableId id) = 0;
    /*! \remarks This method is called to deactivate the action table. After
    the table is deactivated (for example in EndEditParams()) the callback
    object can be deleted. Tables are initially active, please do not call this
    method without a preceding call to <b>ActivateActionTable()</b>.
    \par Parameters:
    <b>ActionCallback* pCallback</b>\n\n
    Points to the callback object responsible for executing the action. Pass
    the same callback that was originally passed to
    <b>ActivateActionTable()</b> and do not set this to NULL.\n\n
    <b>ActionTableId id</b>\n\n
    The ID of the table to deactivate.
    \return  TRUE if the action table was deactivated. FALSE if the table was
    already deactivated or doesn't exist. */
    virtual int DeactivateActionTable(ActionCallback* pCallback, ActionTableId id) = 0;

    // Find a table based on its id.
    /*! \remarks This method returns a pointer to the action table as
    specified by it's ID.
    \par Parameters:
    <b>ActionTableId id</b>\n\n
    The ID of the table to find. */
    virtual ActionTable* FindTable(ActionTableId id) = 0;

    // Get the string that describes the keyboard shortcut for the operation
    /*! \remarks Retrieves the string that describes the keyboard shortcut for
    the operation.
    \par Parameters:
    <b>ActionTableId tableId</b>\n\n
    The ID of the action table.\n\n
    <b>int commandId</b>\n\n
    The ID of the command for the action.\n\n
    <b>MCHAR* buf</b>\n\n
    Points to storage for the string.
    \return  TRUE if found; FALSE if not found. */
    virtual BOOL GetShortcutString(ActionTableId tableId, int commandId, MCHAR* buf) = 0;
    // Get A string the descibes an operation
    /*! \remarks Retrieves a string that descibes the specified operation from
    the action table whose ID is passed.
    \par Parameters:
    <b>ActionTableId tableId</b>\n\n
    The ID of the action table.\n\n
    <b>int commandId</b>\n\n
    The ID of the command.\n\n
    <b>MCHAR* buf</b>\n\n
    Points to storage for the description string.
    \return  TRUE if the string was returned; FALSE if not. */
    virtual BOOL GetActionDescription(ActionTableId tableId, int commandId, MCHAR* buf) = 0;

    // Register an action context.  This is called when you create the
    // action table that uses this context.
    /*! \remarks Register the specified action context with the system. This
    is called when you create the action table that uses this context.
    \par Parameters:
    <b>ActionContextId contextId</b>\n\n
    The context ID.\n\n
    <b>MCHAR* pName</b>\n\n
    The name for the action context.
    \return  If the specified action context is already registered FALSE is
    returned; otherwise TRUE is returned. */
    virtual BOOL RegisterActionContext(ActionContextId contextId, MCHAR* pName) = 0;
    // Methods to iterate over the action contexts
    /*! \remarks Returns the number of ActionContexts. */
    virtual int NumActionContexts() = 0;
    /*! \remarks Returns a pointer to the 'i-th' ActionContext.
    \par Parameters:
    <b>int i</b>\n\n
    The zero based index of the ActionContext. */
    virtual ActionContext* GetActionContext(int i) = 0;
    // Find a context based on it's ID.
    /*! \remarks Returns a pointer to the ActionContext based on it's ID. If
    not found NULL is returned.
    \par Parameters:
    <b>ActionContextId contextId</b>\n\n
    The ID whose context is found. */
    virtual ActionContext* FindContext(ActionContextId contextId) = 0;

    // Query whether a context is active.
    /*! \remarks Returns TRUE if the specified context is active; otherwise
    FALSE.
    \par Parameters:
    <b>ActionContextId contextId</b>\n\n
    Specifies the context to check. */
    virtual BOOL IsContextActive(ActionContextId contextId) = 0;

    // Internal methods used by the keyboard shotcut UI
    virtual MCHAR* GetShortcutFile() = 0;
    virtual MCHAR* GetShortcutDir() = 0;
    virtual int IdToIndex(ActionTableId id) = 0;
    virtual void SaveAllContextsToINI() = 0;

    virtual int MakeActionSetCurrent(MCHAR* pDir, MCHAR* pFile) = 0;
    virtual int LoadAccelConfig(LPACCEL *accel, int *cts, ActionTableId tableId = -1,
                                BOOL forceDefault = FALSE) = 0;
    virtual int SaveAccelConfig(LPACCEL *accel, int *cts) = 0;
    virtual int GetCurrentActionSet(MCHAR *buf) = 0;

    virtual BOOL SaveKeyboardFile(MCHAR* pFileName) = 0;
    virtual BOOL LoadKeyboardFile(MCHAR* pFileName) = 0;
    virtual MCHAR* GetKeyboardFile() = 0;

	// Function IDs
    enum {
        executeAction,
#ifndef NO_CUI	// russom - 02/12/02
        saveKeyboardFile,
        loadKeyboardFile,
        getKeyboardFile,
#endif // NO_CUI
    };
};

//!Class IActionManager

/*!
	A mixin-interface extension to IActionManager which allows a client to dispatch a Message back to the application
	if it is not handled by the focused control.
	Can be accessed by calling
	\code
	Interface10* ip = GetCOREInterface10();
	IActionManager* actionMgr = ip-> GetActionManager();
	IActionManagerExt* ext = 
		static_cast<IActionManagerExt*>(actionMgr->GetInterface(IActionManagerExt::kActionMgr10InterfaceID));
	\endcode
	\remarks This interface is NOT intended for extension by 3rd-party developers.
	\see IActionManager
*/
class IActionManager10 : public BaseInterface, public MaxSDK::Util::Noncopyable {

public:
	//! \brief The ID for this interface.  See class description for usage.
	CoreExport static const Interface_ID kActionMgr10InterfaceID;

	//! \brief Dispatches a windows Message structure to the application

	/*!	Takes a MSG structure and dispatches it to the main application proc directly.
		This function can be used to dispatch WM_KEYDOWN events to the application for processing.
		If a control or dialog is capturing input messages, then this function is useful to delegate
		message handling to the main application window proc in the case where the message is not handled
		by the focused control hierarchy or the activated (modeless) dialog.
		\param[in, out] message The message to dispatch to the application.
		\return true if the message is handled by the application
	*/
	virtual bool DispatchMessageToApplication(MSG* message) = 0;

	//! \brief Returns the interface ID
	CoreExport virtual Interface_ID GetID(); 
protected:
	//! \brief The constructor / destructor are defined in Core.dll, but bear in mind that this interface is NOT
	//! intended for extension.
	CoreExport IActionManager10();
	//! \brief The constructor / destructor are defined in Core.dll, but bear in mind that this interface is NOT
	//! intended for extension.
	CoreExport virtual ~IActionManager10();

};

// The DynamicMenu class provides a way for plugins to produce
// the menu needed in the ActionItem::GetDynamicMenu() method.

/*! \sa  Class DynamicMenu, Class IMenu, Class ActionTable, Class ActionItem, Class ActionCallback, Class ActionContext, Class IActionManager, Class Interface.\n\n
\par Description:
This class is available in release 4.0 and later only.\n\n
This is the callback object for a dynamic menu. When a user makes a selection
from a dynamic menu the <b>MenuItemSelected()</b> method of this class is
called to process that selection.  */
class DynamicMenuCallback: public MaxHeapOperators {
public:
    /*! \remarks This method is called to process the user's menu selection.
    \par Parameters:
    <b>int itemId</b>\n\n
    The ID of the item selected. */
    virtual void MenuItemSelected(int itemId) = 0;
};

/*! \sa  Class DynamicMenuCallback,  Class IMenu, Class ActionTable, Class ActionItem, Class ActionCallback, Class ActionContext, Class IActionManager, Class Interface.\n\n
\par Description:
This class is available in release 4.0 and later only.\n\n
This class provides a simple way for plug-ins to produce the menu needed in the
<b>ActionItem::GetDynamicMenu()</b> method. The constructor of this class is
used to create the menu and the GetMenu() method returns the appropriate IMenu
pointer.  */
class DynamicMenu: public MaxHeapOperators {
public:

    /*! \remarks Constructor.
    \par Parameters:
    <b>DynamicMenuCallback* pCallback</b>\n\n
    Points to the instance of the DynamicMenuCallback class that handles the
    menu selection. */
    CoreExport DynamicMenu(DynamicMenuCallback* pCallback);

    // Called after menu creation to get the IMenu created.
    // This is the value returned from  ActionItem::GetDynamicMenu()
    /*! \remarks Returns a pointer to the IMenu. This method may be called
    after menu creation to get a pointer to the IMenu created. This is the
    required value to return from <b>ActionItem::GetDynamicMenu()</b>. */
    CoreExport IMenu* GetMenu();

    enum DynamicMenuFlags {
        kDisabled   = 1 << 0,
        kChecked    = 1 << 1,
        kSeparator  = 1 << 2,
    };

    // Add an item to the dynamic menu.
    /*! \remarks This method adds an item to the dynamic menu.
    \par Parameters:
    <b>DWORD flags</b>\n\n
    One or more of the following values:\n\n
    <b>kDisabled</b>\n\n
    Item is disabled (can't be selected)\n\n
    <b>kChecked</b>\n\n
    Item has a check mark beside it.\n\n
    <b>kSeparator</b>\n\n
    Item is a seperator between the previous menu item and the next one.\n\n
    <b>UINT itemId</b>\n\n
    The ID for the menu item.\n\n
    <b>MCHAR* pItemTitle</b>\n\n
    The name to appear for the menu item. */
    CoreExport void AddItem(DWORD flags, UINT itemId, MCHAR* pItemTitle);
    /*! \remarks This begins a new sub menu. Items added after this call will
    appear as sub choices of this one until <b>EndSubMenu()</b> is called.
    \par Parameters:
    <b>MCHAR* pTitle</b>\n\n
    The name to appear for the sub menu item. */
    CoreExport void BeginSubMenu(MCHAR* pTitle);
    /*! \remarks This ends a sub menu. Items added after this call will appear
    as they did prior to calling <b>BeginSubMenu()</b>. */
    CoreExport void EndSubMenu();

private:

    Stack<IMenu*> mMenuStack;
    DynamicMenuCallback *mpCallback;
};

#endif
