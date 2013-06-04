
#ifndef PLUGIN_H_DEFINED
#define PLUGIN_H_DEFINED

/*******************************************************************
 *
 *    DESCRIPTION: DLL Plugin classes
 *
 *    AUTHOR: Dan Silva
 *
 *    HISTORY: 11/30/94 Started coding
 *
 *******************************************************************/

typedef unsigned long ulong;

#pragma warning(disable:4786)
#include "maxheap.h"
#include <plugapi.h>

/*! \brief This class models a plugin DLL. 
	3ds Max creates an instance of this class for each plugin dlls that it loads.
	This class allows for querying:
	\li The number of plugins exposed by the plugin dll. Each plugin DLL may expose several plugin classes.
	\li Each plugin exposed by the plugin dll 
	\li Whether the plugin dll supports run-time type information (RTTI) or not
	\li The description of the plugin dll
	
	Plugins must be built with support for RTTI, and must export all of the following 
	functions in order for them to be loaded into 3ds Max.
	\li ULONG LibVersion(void)
	\li const MCHAR* LibDescription(void)
	\li int LibNumberClasses(void)
	\li ClassDesc* LibClassDesc(int i)

	Plugins may implement the following functions for added flexibility:
	\li int LibInitialize(void)
	\li int LibShutdown(void);
	\li ULONG CanAutoDefer(void) 

	\sa Class DllDir, Class ClassDesc, Class Interface, 
	\sa <a href="ms-its:3dsmaxsdk.chm::/start_dll_functions.html">Required DLL Functions and Class Descriptors</a>
	\sa <a href="ms-its:3dsMaxSDK.chm::/deferred_loading.html">Deferred Loading of Plugins</a>
*/
class DllDesc: public MaxHeapOperators {
public:
	//! The DLL instance handle.
	HINSTANCE handle;
	//! The file name the DLL was loaded from.
	MSTR fname;
	bool loaded;	// if false, just basic info gleaned from Registry
	const MCHAR*	(*vDescription)();	// copy of this string can be stored in the DLL dir 
	int (*vNumClasses)();	// How many classes does this DLL implement?
	ClassDesc* (*vClassDesc)(int i);	// Hands back a ClassDesc for the ith class

	/*! \brief Default constructor */
	CoreExport DllDesc(); 

	/*! \brief This method decrements the reference count of the DLL. 
	When the reference count reaches zero, the module is unmapped from the address
	space of the calling process. */
	CoreExport void Free();

	/*! \remarks Returns the Description defined in the LibDescription function. */
	CoreExport const MCHAR* Description();

	/*! \remarks Returns the number of classes implemented in the DLL. This is
	the value returned by the library function LibNumberClasses. */
	CoreExport int NumberOfClasses();

	/*! \remarks Returns a pointer to the 'i-th' class descriptor.
	\param	int i - The zero based index of the ClassDesc to return. */
	CoreExport ClassDesc* operator[](int i);

	/*! \remarks Equality operator. Returns nonzero if the <b>DllDesc</b>
	passed matches this one; otherwise zero. */
	CoreExport int operator==( const DllDesc& dd ) const;

	MSTR tDescription;
	int tNumClasses;
	MSTR directory;

	//! \brief Called when the DLL is initialized. 
	/*! This calls the function that is exported from the DLL module. This method is NOT implemented
		by the developer. For this method to work correctly, the developer should export a function 
		from the DLL with the following prototype:
		\code
		__declspec( dllexport ) BOOL LibInitialize()
		\endcode
		The developer should also be sure to note they must list this function in their module definition
		file. 
		Initializations should not be done from within DllMain, but from within this function. 
		See Microsoft's documentation on DllMain(). If present this is the first function 
		called in a DLL. It may be called more than once, so the function should be prepared
		to handle it.  We try to call it just once though. 	*/
	BOOL (*vLibInitialize)();

	//! \brief Function that gets called when the DLL is unloaded.
	//! Similar to LibInitialize above except called when the DLL is unloaded.
	BOOL (*vLibShutdown)();

	//! \brief Function that gets called when the DLL is loaded.
	/*! 	This calls the function that is exported from the DLL module. This method is NOT implemented by the developer.
		\return Return TRUE if you deem your plugin successfully loaded, or FALSE otherwise. If 
		the function returns FALSE, the system will NOT load the plugin, it will then call FreeLibrary
		on your DLL, and send you a message. */
	CoreExport BOOL Initialize();

	//! \brief Function that gets called when the DLL is unloaded.
	/*!	\return The system doesn't pay attention to a return value.*/
	CoreExport BOOL Shutdown();

	/*! \brief Checks if this plugins dll was built with run-time type information (RTTI)
	\pre NumberOfClasses must return a positive non-zero value for the check to be performed
	\return Returns true if the dll was built with RTTI, otherwise false. 
	In the later case, the plugin dll will not be loaded into Max.
	*/
	CoreExport bool HasRttiSupport() const;
};


/*-----------------------------------------------------------------
 We will build one big DllDir on startup.
 As we do this, we will build a set of lists, one for each SuperClass.
 For a given super class, we want to 
	(a) Enumerate all public subclasses 
	(b) Enumerate all subclasses
	(c) Find the subClass for a given subClass ID.

--------------------------------------------------------------*/

/*! \brief This class provides information about a class in the SubClassList table
	of classes. 
	This is information such as the class ID, category, public status, usage count, etc.
	All methods of this class are implemented by the system.  
	\sa  Class DllDir, Class SubClassList, Class ClassDesc, Class DataClassDesc, Class Class_ID.
*/
class ClassEntry: public MaxHeapOperators {
	//! Index into the master Dll list
	int dllNumber;  
	//! Index of the class within the DLL
	int classNum;  	
	int isPublic;
	//! For enumerating classes in summary info
	int useCount;  
	Class_ID classID;
	MSTR name;
	MSTR category;
	ClassDesc *cd;
	bool loaded;
	
	//! The scroll amount of the command panel
	int scroll;
	//! The open close state for up to 32 rollup pages. (when the bit is set that means the rollup page is closed)
	DWORD pageState;
	
	public:
		CoreExport ClassEntry();
		CoreExport ClassEntry(const ClassEntry &ce);
		CoreExport ClassEntry(ClassDesc *cld, int dllN, int index, bool load);
		CoreExport ~ClassEntry();
		/*! \remarks This method is used internally. */
		CoreExport void Set(ClassDesc *cld, int dllN, int index, bool load);
		/*! \remarks Returns the index into the master DLL list of this entry. */
		int DllNumber() { return dllNumber; }
		/*! \remarks Returns nonzero if the class is public; otherwise zero.
		Non-public classes are those that are meant for private use by other
		plug-ins. */
		int IsPublic() { return isPublic; }
		/*! \remarks Returns the Class_ID of the entry. */
		Class_ID ClassID() { return classID; }
		/*! \remarks Returns the class name of the entry. */
		MSTR &ClassName() { return name; }
		/*! \remarks Returns a reference to the category string for this
		entry. */
		MSTR &Category() { return category; }
		/*! \remarks This method is used internally. */
		int GetScroll() {return scroll;}
		/*! \remarks This method is used internally. */
		void SetScroll(int s) {scroll = s;}
		BOOL PageStateSet() {return pageState != 0x7fffffff;}  // test if pageState has been actually set, default initializes to 0x7fffffff
		/*! \remarks This method is used internally. */
		BOOL GetPageState(int i) {return (pageState&(1<<i))?TRUE:FALSE;}
		/*! \remarks This method is used internally.
		\par Operators:
		*/
		void SetPageState(int i,BOOL state) {if (state) pageState |= (1<<i); else pageState &= ~(1<<i);}
		/*! \remarks Returns the number of instance of this class used in 3ds
		Max. */
		int UseCount() { return useCount; }
		/*! \remarks This is used internally to increment the usage count for
		this entry. */
		void IncUseCount () { useCount++; }
			/*! \remarks This is used internally to set the usage count for this
			entry. */
			void SetUseCount(int i) { useCount = i; }

		/*! \brief Returns a pointer to the class descriptror for this entry. 
		With the addition of deferred-loaded plug-ins introduced
		in 3ds Max 3.0, this method may return a pointer to an instance of
		class DataClassDesc (a sub-class of ClassDesc). A developer can tell 
		if the returned class is a DataClassDesc object if ClassEntry::IsLoaded()
		returns FALSE. This derived	class implements ClassDesc::Create() by 
		returning NULL. Developers need to call ClassEntry::FullCD() in order to 
		ensure that the class is actually present. Developers can call this method to
		retrieve data about the plug-in without forcing a load. It may also be
		called to get a 'full' ClassDesc if it is known that the plug-in
		is loaded (IsLoaded() returns TRUE). */
		ClassDesc* CD() { return cd; }

		/*! \remarks Returns nonzero if this entry matches the specifed access
		type; otherwise zero.
		\par Parameters:
		<b>int accType</b>
		One of the following values:
		<b>ACC_PUBLIC</b> - public classes
		<b>ACC_PRIVATE</b> - non-public classes */
		CoreExport int IsAccType(int accType);
		/*! \remarks Assignment operator. */
		CoreExport ClassEntry& operator=( const ClassEntry &ce ); 
		/*! \remarks Equality operator. */
		int operator==( const ClassEntry &ce ) const { return 0; }
		/*! \remarks This method is available in release 3.0 and later
		only.
		Returns TRUE if loaded; otherwise FALSE. */
		bool IsLoaded() { return loaded; }
		/*! \remarks This method is available in release 3.0 and later
		only.
		Returns the index of the class within the DLL */
		int ClassNumber() { return classNum; }
		/*! \remarks This method is available in release 3.0 and later
		only.
		This method fetches the class descriptor from a class entry. Unlike
		<b>CD()</b> above, this method will load the relevant DLL if its
		loading was deferred, and will return a pointer to the loaded class
		descriptor. See
		<a href="ms-its:3dsmaxsdk.chm::/deferred_loading.html">Deferred
		Loading of Plug-Ins</a> for more details. */
		CoreExport ClassDesc *FullCD();
		DWORD GetPageState() {return pageState;} // new to r7
	};

// access type.
#define ACC_PUBLIC 1
#define ACC_PRIVATE 2
#define ACC_ALL (ACC_PUBLIC|ACC_PRIVATE)

typedef ClassEntry* PClassEntry;
typedef Tab<PClassEntry> SubClassTab;

/*! \brief This class allows developers to provide some additional information on a superclass.
	This class is available in release 3.0 and later only. Currently this includes a color, and a method which draws a
	representative image in a Windows Device Context. DrawRepresentation(...) can return false to 
	indicate that no image was drawn. DrawRepresentation(...) should cache its image (if applicable) as the method is
	called repeatedly while drawing certain UI components (like the schematic view).  
	\sa  Class SubClassList, <a href="ms-its:listsandfunctions.chm::/idx_R_colorref.html">COLORREF</a>.
*/
class SClassUIInfo: public MaxHeapOperators
	{
	public:
		virtual ~SClassUIInfo() {}
	// A color associated with the superclass.  This is currently used to draw nodes
	// in the schematic view at extreme zoom-outs where it is impossible to draw
	// legible node names.
	/*! \remarks Returns a color associated with the specified super class.
	This is currently used to draw nodes in the schematic view at extreme
	zoom-outs where it is impossible to draw legible node names.
	\par Parameters:
	<b>SClass_ID superClassID</b>
	The Super Class whose associated color to return.
	\par Default Implementation:
	<b>{ return RGB(128, 128, 128); }</b> */
	CoreExport virtual COLORREF Color(SClass_ID superClassID) { return RGB(128, 128, 128); };

	// Draws an image which represents the superclass (usually an icon) in a rectangle
	// in a given Windows DC.  The implementation should attempt to draw the image as fast
	// as possible as this method is called repeatedly while drawing certain UI
	// components.  Return false if no image was drawn and a generic stand-in image will
	// be used.  The provided "bkColor" is the average background color of the surface on
	// which the image is being drawn.  It can be used, if desired, to antialias the image.
	/*! \remarks Draws an image which represents the superclass (usually an
	icon) in a rectangle in a given Windows DC. The implementation should
	attempt to draw the image as fast as possible as this method is called
	repeatedly while drawing certain UI components.
	\par Parameters:
	<b>SClass_ID superClassID</b>
	The super class to draw.
	<b>COLORREF bkColor</b>
	This is the average background color of the surface on which the image is
	being drawn. It can be used, if desired, to antialias the image.
	<b>HDC hDC</b>
	The handle to the device context.
	<b>Rect \&rect</b>
	The rectangle to draw in.
	\return  Return <b>false</b> if no image was drawn and a generic stand-in
	image will be used.
	\par Default Implementation:
	<b>{ return false; }</b> */
	CoreExport virtual bool DrawRepresentation(SClass_ID superClassID, COLORREF bkColor, HDC hDC, Rect &rect) { return false; }
	};


/*! \brief A Sub Class list is a table of ClassEntry objects.
	This also provides information on plug-in classes as well as usage counts for these classes within 3ds Max. 
	These sub class lists are organized by super class ID by the ClassDirectory class.
	All methods of this class are implemented by the system.  
	\sa  Class DllDir, Class ClassDirectory, Class ClassEntry, Class ClassDesc, Class SClassUIInfo.
*/
class SubClassList: public MaxHeapOperators {
		int iEnum;
		ulong superID;
		SubClassTab subClList;
		SClassUIInfo *uiInfo;
		public:
		CoreExport ~SubClassList();
		SubClassList(ulong sid=0) { superID = sid; uiInfo = NULL; }
		/*! \remarks Returns a reference to the 'i-th' <b>ClassEntry</b> for
		this super class.
		\par Parameters:
		<b>int i</b>
		The index of the entry to return. Valid values begin at an index of 1.
		*/
		ClassEntry& operator[](int i){ return(*subClList[i]);}
		/*! \remarks Returns the index in the list of sub-classes of the class
		whose Class_ID is passed.
		\par Parameters:
		<b>Class_ID subClassID</b>
		Specifies which class to return the index of. */
		CoreExport int FindClass(Class_ID subClassID);	  // Get a class by its class ID
		/*! \remarks Returns the index in the list of sub-classes of the class
		whose <b>ClassName()</b> is passed.
		\par Parameters:
		<b>const MCHAR *name</b>
		Specifies which class to return the index of. */
		CoreExport int FindClass(const MCHAR *name);    // Get a class by its class name
		/*! \remarks This method is used internally. */
		CoreExport void AddClass(ClassDesc *cld, int dllNum, int index, bool load); 
		/*! \remarks Returns the number of sub-classes that match the
		specified access type.
		\par Parameters:
		<b>int accType</b>
		One of the following values:
		<b>ACC_PUBLIC</b> - public classes
		<b>ACC_PRIVATE</b> - non-public classes
		<b>ACC_ALL</b> - both of the above (<b>ACC_PUBLIC|ACC_PRIVATE</b>). */
		CoreExport int  Count(int accType);
		/*! \remarks Returns the Super class ID corresponding to this
		sub-class list. */
		ulong SuperID() { return superID; }
		/*! \remarks Equality operator. */
		int operator==(const SubClassList &sl) {return(0);}

		// Enumerate.
		/*! \remarks Returns the index of the first <b>ClassDesc</b> of the
		specified type in the list of sub-classes.
		\par Parameters:
		<b>int accType</b>
		One of the following values:
		<b>ACC_PUBLIC</b> - public classes
		<b>ACC_PRIVATE</b> - non-public classes */
		CoreExport int GetFirst(int accType);	 // get first ClassDesc of accType
		/*! \remarks Returns the index of the next <b>ClassDesc</b> of the
		specified type (or -1 at the end).
		\par Parameters:
		<b>int accType</b>
		One of the following values:
		<b>ACC_PUBLIC</b> - public classes
		<b>ACC_PRIVATE</b> - non-public classes */
		CoreExport int GetNext(int accType);	 // get next ClassDesc of accType (NULL for End)
		/*! \remarks Equality operator. */
		int operator==( const SubClassList& lst ) const { return 0; }
		CoreExport int DeleteClass(ClassDesc *cld); 

		// Allows developer to provide some additional information on a superclass.
		// Currently this includes a descriptive string, a color, and a method which
		// draws a representative image in a Windows DC.
		/*! \remarks This method is available in release 3.0 and later
		only.
		Allows developer to provide some additional information on a
		superclass. Currently this includes a color, and a method which draws a
		representative image in a Windows DC.
		\par Parameters:
		<b>SClassUIInfo *uiInfo</b>
		Points to the information to set. */
		CoreExport void SetUIInfo(SClassUIInfo *uiInfo);

		// Retrieves additional UI related information on a given superclass.  Returns
		// NULL if no superclass information was assigned.
		/*! \remarks This method is available in release 3.0 and later
		only.
		This method returns additional user interface related information on a
		given superclass. Returns NULL if no superclass information was
		assigned. */
		CoreExport SClassUIInfo *GetUIInfo();
		/*! \remarks This method is available in release 3.0 and later
		only.
		This method is for internal use only.
		\par Operators:
		*/
		CoreExport void ReplaceClass(int idx, ClassDesc *cld, int dllNum, int index, bool load); 
	};


typedef SubClassList* PSubClassList;
typedef Tab<PSubClassList> ListOfClassLists;

// forward declaration
class DataClassDesc;

/*! \brief A manager class for maintaining SuperClass's and Class's.
	This class provides a table of SubClassList objects, one for each pluggable super class.
	Methods are provided for accessing specific sub class lists by specifying a super class ID, and 
	retrieving the class descriptors or class entries of the classes in the lists.
	All methods of this class are implemented by the system.  
	\sa  Class DllDir, Class SubClassList, Class Class_ID, Class ClassDesc. */
class ClassDirectory : public MaxHeapOperators 
{
private:
	//! \brief Holds pointers to DataClassDesc pointers that were replaced by the system. 
	/*! Future versions of 3dsmax will simply delete DataClassDesc pointers */ 
	Tab<DataClassDesc*> mExpiredDataClassDescs;
	ListOfClassLists cl;
public:
	//! Destructor
		CoreExport ~ClassDirectory();
	//! Reserved Method
	SDK_RESERVED_METHOD(1);

#if SDK_ORDER_VER == 0
	/*! \brief Returns a pointer to the list of sub-classes derived from the specified super class ID.
		\param SClass_ID superClassID - The super class ID whose list of sub-classes are to be returned. */
		CoreExport SubClassList* GetClassList(SClass_ID superClassID);	
#endif

	/*! \brief Returns a pointer to the class descriptor for the class
		whose super class ID and class ID are passed.
		\param SClass_ID superClassID - The super class ID which specifies which sub class list to search.
		\param Class_ID subClassID - The class ID of the class to find. */
		CoreExport ClassDesc* FindClass(SClass_ID superClassID, Class_ID subClassID);

#if SDK_ORDER_VER > 2
	/*! \brief Returns a pointer to the list of sub-classes derived from the specified super class ID.
		\param SClass_ID superClassID - The super class ID whose list of sub-classes are to be returned. */
		CoreExport SubClassList* GetClassList(SClass_ID superClassID);	
#endif

	/*! \brief Returns a pointer to the class entry for the class whose
		super class ID and class ID are passed.
		\param SClass_ID superClassID - The super class ID which specifies which sub class list to search.
		\param Class_ID subClassID - The class ID of the class to find. */
	CoreExport ClassEntry* FindClassEntry(SClass_ID superClassID, Class_ID subClassID);

	/*! \brief Returns a reference to the 'i-th' sub class list.
		\param int i - Specifies which sub class list to return a reference to. This value is between
		0 and Count()-1. */
		SubClassList& operator[](int i){ 
			return(*cl[i]);
		}

	/*! \brief Adds a new Super Class ID to the list maintained by this class.
		It is illegal to create your own Super Class of persistent objects 
		in 3ds Max. 3ds Max will crash if a scene is saved and later loaded into a system that doesn't
		have a plug-in added Super Class. The problem is that 3ds Max needs a stand-in class for each
		'plug-able' Super Class and if a plug-in is loaded that requires (but
		doesn't have) a stand-in, an assertion is fired.
		The only legal Super Classes that you can derive your plug-in from are
		listed <a href="ms-its:listsandfunctions.chm::/idx_R_list_of_super_class_ids.html">List of Super Class IDs</a>. 
		This method is used internally only. */
		CoreExport void AddSuperClass(SClass_ID superClassID);

	//! \brief Reserved Method
		SDK_RESERVED_METHOD(2);	 // russom 02/26/01

	/*! \brief Add a class descriptor to the list maintained by this class.
		\param ClassDesc* cdesc - Pointer to the class descriptor. If cdesc is a DataClassDesc 
		pointer (which means the ClassEntry::IsLoaded() returns false), then this class 
		descriptor represents a plugin that was deferred loaded. This means a DataClassDesc gets
		its information from the registry instead of the DLL itself. If this pointer is a DataClassDesc, then the
		system will load the full Class Descriptor and not use the temporary DataClassDesc pointer. When this
		switch happens, the system will broadcast a notification:
		\code
		BroadcastNotification(NOTIFY_CLASSDESC_REPLACED, &cdr);
		\endcode
		That way if something holds a pointer to a DataClassDesc, it can use this broadcast to switch to the
		new pointer.
		DataClassDesc will be stored for later deletion when 3dsmax shuts down.
		In future versions of 3dsmax, this DataClassDesc pointer will simply be deleted when it gets replaced. 
		Used Internaly.
		\param int dllNum - The Dll number
		\param int index - The index 
		\return Returns 0 if class already exists, -1 if superclass was unknown and 1 if the class added successfully */
	CoreExport int  AddClass(ClassDesc* cdesc, int dllNum, int index, bool load);

	/*! \brief Returns the number of class lists in the class directory. */
	int Count() {
		return cl.Count();
	}

#if SDK_ORDER_VER == 2	//  // russom 02/26/01
		/*! \remarks Returns a pointer to the list of sub-classes derived from
		the specified super class ID.
		\param SClass_ID superClassID - The super class ID whose list of sub-classes are to be returned. */
		CoreExport SubClassList* GetClassList(SClass_ID superClassID);	
#endif
	/*! \brief Deletes a Class Descriptor
		\return Returns 1 if successful, or -1 if not. */
	CoreExport int DeleteClass(ClassDesc* cdesc);

	//! \brief Allows developer to provide some additional information on a superclass.
	/*! Currently this includes a descriptive string, a color, and a method which
		draws a representative image in a Windows DC.
		\param SClass_ID superClassID - The Super class ID to set
		\param SClassUIInfo* uiInfo - A pointer to a SClassUIInfo object
		\return - Returns true if successful or false if the superclass was not found. */
	CoreExport bool SetUIInfoForSClass(SClass_ID superClassID, SClassUIInfo* uiInfo);

#if SDK_ORDER_VER == 1	 // russom 02/26/01
	/*! \brief Returns a pointer to the list of sub-classes derived from
		the specified super class ID.
		\param SClass_ID superClassID - The super class ID whose list of sub-classes are to be returned. */
		CoreExport SubClassList* GetClassList(SClass_ID superClassID);	
#endif

	/*! \brief Retrieves additional UI related information on a given superclass.  
		\param SClass_ID superClassID - The Super class ID
		\return - Returns NULL if the superclass was not found or if no superclass information was
		assigned. */
	CoreExport SClassUIInfo* GetUIInfoForSClass(SClass_ID superClassID);
	};

/* DllDirectory: A list DllDescriptors, one for every DLL loaded 
	 Also contains the ClassDirectory of all classes implemented in these
	 DLL's */
typedef DllDesc* PDllDesc;

/*! \brief It provides access to the DLL Directory which is a list of every DLL loaded in 3DSMax.
	It also contains the Class Directory which maintains lists of all classes implemented in these DLLs.
	The following diagram shows the relationship between the classes that make up
	the Dll Directory structure. The lines in the diagram indicate how the objects
	are accessed. Class DllDir provides access to a list of DllDesc objects. Each of
	these has a list of ClassDesc objects. The ClassDesc is the object implemented by the
	plug-in to let 3ds Max know about it's classification and capabilities. Class DllDir
	also provides access to the ClassDirectory which provides access to the a table of
	SubClassList objects which are grouped by super class ID. Each SubClassList has a
	series of ClassEntry objects. The ClassEntry object provides information about the 
	plug-in classes (some of the same information as the class descriptor, usage counts, etc.). \n  
	\n \image html "dlldir_wmf.gif"
	Note: To get a reference to the central DLL directory see the method Interface::GetDllDir().
	All methods of this class are implemented by the system.  
	\sa  Class DllDesc, Class ClassDirectory, Class ClassDesc, Class ClassEntry, Class Interface.
*/
class DllDir: public MaxHeapOperators {
	Tab<PDllDesc> dll;    // list of Descriptors for all the loaded DLL's
	ClassDirectory classDir; // All the classes implemented in these DLL's 
	int  AddDll(const DllDesc *dllDesc, bool late);
	public:
		CoreExport ~DllDir();
		/*! \remarks This method unloads every DLL 3ds Max has loaded. This
		calls the Win32 function <b>FreeLibrary()</b> on every DLL handle in
		the DLL directory. */
		CoreExport void UnloadAllDlls();
		/*! \remarks Returns the number of DLLs currently loaded. */
		int Count() { return dll.Count(); }
		SDK_RESERVED_METHOD(1);	 // russom 02/26/01
		/*! \remarks Access operator. This returns a reference to the 'i-th'
		<b>DllDesc</b>.
		\par Parameters:
		<b>int i</b>
		Zero based index of specifying which <b>DllDesc</b> to return. */
		DllDesc& operator[](int i) { return(*dll[i]); }
		SDK_RESERVED_METHOD(2);	 // russom 02/26/01
		/*! \remarks This method may be called to load DLLs from the specified	directory.
		\par Parameters:
		<b>MCHAR *directory</b>
		Point to a null-terminated string that specifies a valid directory or path to
		load the DLLs from.
		<b>MCHAR *wildcard</b>
		Points to a null-terminated string that contains wildcard characters (* and	?).
		<b>HWND hwnd=NULL</b>
		This parameter is not currently used.
		\return  Nonzero if successful; zero on error. */
		CoreExport int LoadDllsFromDir(const MCHAR *directory, const MCHAR *  wildcard, HWND hwnd=NULL);
		/*! \remarks Returns a reference to the <b>ClassDirectory</b> for this	DLL directory. */
		ClassDirectory& ClassDir() { return classDir; }
		SDK_RESERVED_METHOD(3);	 // russom 02/26/01
		/*! \remarks This method is for internal use only. */
		CoreExport bool LoadADll(MCHAR *, bool late);
	};



/*! \brief A partial class desciptor for classes in plug-ins but described by Registry entries
	The registry is a system-defined database that applications and system components use to store
	and retrieve configuration data. This class provides a "stub" or partial class description, 
	as read from the Registry, which provides basic descriptive information about the class (which proves
	adequate for certain purposes). Proper use of the method ClassEntry::FullCD() ensures that the partial
	class description will be replaced by the full one, which includes the ClassDesc::Create() method and
	other functionality. 
	When a DataClassDesc is replaced by a full class descriptor, Inside of ClassDirectory::AddClass then a
	notification is sent:
	\code
	BroadcastNotification(NOTIFY_CLASSDESC_REPLACED, &cdr);
	\endcode
	That way if something holds a pointer to a DataClassDesc, it can use this broadcast to switch to the
	new pointer. Currently (3dsmax 11) the DataClassDesc will be stored for later deletion when 3dsmax shuts down.
	In future versions of 3dsmax, this DataClassDesc pointer will simply be deleted when it gets replaced. 
	Note: With the addition of delay-loaded plugins, the method ClassEntry::CD() may
	return a pointer to an instance of this class. This derived class implements Create() by returning NULL.
	Developers need to call FullCD() in order to ensure that the class is actually there.
	For additional details see Class ClassEntry and the Advanced Topics section <a href="ms-its:3dsmaxsdk.chm::/deferred_loading.html">Deferred Loading of Plug-Ins</a>.
	<br>
	Note: Data members are initialized from the Registry.
	\sa  Class ClassDesc, Class ClassEntry, Class BitmapIO, Class SceneImport, Class SceneExport.
*/
class DataClassDesc : public ClassDesc {
public:
	MSTR category;		//! The category string
	DWORD classIDA;		//! The first ulong of the Class_ID
	DWORD classIDB;		//! The second ulong of the Class_ID 
	DWORD superClassID; //! The SuperClassID of the class
	MSTR className;		//! The class name
	DWORD isPublic;		//! If this plugin is public or not.
	DWORD okToCreate;	//! If it is OK to create
	DWORD extCount;		//! Returns the number of file name extensions supported
	MSTR ext;			//! The file name extension.
	MSTR shortDesc;		//! The short ASCII description.
	MSTR longDesc;		//! The long ASCII description
	DWORD supportsOptions;	//! The export options
	DWORD capability;	//! The BitmapIO module capability flags
	DWORD inputTypeA;	// For Modifiers. The first ulong of theModifier::InputType() Class_ID
	DWORD inputTypeB;	// For Modifiers. The second ulong of the Modifier::InputType() Class_ID
	MSTR internalName;
	MCHAR* internalNamePtr;

	DataClassDesc() : internalNamePtr(NULL) {}
	~DataClassDesc();
	/*! \remarks Returns the <b>isPublic</b> setting. */
	int IsPublic() { return isPublic; }
	void *Create(BOOL loading=FALSE) { return NULL; }
	int BeginCreate(Interface *i) { return 0; }
	int EndCreate(Interface *i) { return 0; }
	/*! \remarks Returns the class name for the plug-in (<b>className</b>). */
	const MCHAR *ClassName() { return className; }
	/*! \remarks Returns the SuperClassID (<b>superClassID</b>). Note:
	<b>typedef ulong SClass_ID;</b> */
	SClass_ID SuperClassID() { return superClassID; }
	/*! \remarks Returns the Class_ID. This is <b>Class_ID(classIDA,
	classIDB)</b>. */
	Class_ID ClassID() { return Class_ID(classIDA, classIDB); }
	/*! \remarks Returns the <b>category</b> string. */
	const MCHAR *Category() { return category; }
	/*! \remarks Returns the <b>okToCreate</b> state. */
	BOOL OkToCreate(Interface *i) { return okToCreate; }	
	BOOL HasClassParams() { return FALSE; }
	void EditClassParams(HWND hParent) {}
	void ResetClassParams(BOOL fileReset=FALSE) {}

		//! These functions return keyboard action tables that plug-ins can use
		int NumActionTables() { return 0; }
		ActionTable *GetActionTable(int i) { return NULL; }

	// Class IO
	BOOL NeedsToSave() { return FALSE; }
	IOResult Save(ISave *isave) { return IO_OK; }
	IOResult Load(ILoad *iload) { return IO_OK; }

	// bits of dword set indicate corrresponding rollup page is closed.
	// the value 0x7fffffff is returned by the default implementation so the
	// command panel can detect this method is not being overridden, and just leave 
	// the rollups as is.
	DWORD InitialRollupPageState() { return 0x7fffffff; }

	// ParamBlock2-related metadata interface, supplied & implemented in ClassDesc2 (see maxsdk\include\iparamb2.h)
	
	const MCHAR *InternalName() { return internalNamePtr; }
	HINSTANCE HInstance() { return NULL; }
	//! access parameter block descriptors for this class
	int NumParamBlockDescs() { return 0; }
	ParamBlockDesc2 *GetParamBlockDesc(int i) { return NULL; }
	ParamBlockDesc2 *GetParamBlockDescByID(BlockID id) { return NULL; }
	void AddParamBlockDesc(ParamBlockDesc2* pbd) {}
	//! automatic UI management
	void BeginEditParams(IObjParam *ip, ReferenceMaker* obj, ULONG flags, Animatable *prev) {}
	void EndEditParams(IObjParam *ip, ReferenceMaker* obj, ULONG flags, Animatable *prev) {}
	void InvalidateUI(ParamBlockDesc2* pbd) {}
	//! automatic ParamBlock construction
	void MakeAutoParamBlocks(ReferenceMaker* owner) {}
	//! access automatically-maintained ParamMaps, by simple index or by associated ParamBlockDesc
	int NumParamMaps() { return 0; }
	IParamMap2 *GetParamMap(int i) { return NULL; }
	IParamMap2 *GetParamMap(ParamBlockDesc2* pbd) { return NULL; }
	//! maintain user dialog procs on automatically-maintained ParamMaps
	void SetUserDlgProc(ParamBlockDesc2* pbd, ParamMap2UserDlgProc* proc=NULL) {}
	ParamMap2UserDlgProc *GetUserDlgProc(ParamBlockDesc2* pbd) { return NULL; }

	//! Class can draw an image to represent itself graphically...
	bool DrawRepresentation(COLORREF bkColor, HDC hDC, Rect &rect) { return FALSE; }
};


#endif // PLUGIN_H_DEFINED
