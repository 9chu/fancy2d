//**************************************************************************/
// Copyright (c) 1998-2008 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/

#pragma once

#include "buildver.h"	// Pulls in ALPHABETA
#include "utilexp.h"
#include "maxheap.h"

/*! \brief Method to be called to signal a violation of some assumption in the code
*
* This method should not be called directly, but rather via the various assertion 
* macros such as DbgAssert, MaxAssert, DbgVerify, etc.
* \param line - the line number where the assertion occurred
* \param file - the file path and name where the assertion occurred
* \param function - the non-decorated name of the function in which the assertion occurred
* \param expr - the expression that was asserted 
* \return Currently, this method either returns TRUE or it triggers the shutdown of 3ds Max
*/
UtilExport int assert1(int line, char* file, char* function = NULL, char* expr = NULL );

#undef assert
#undef MaxAssert
#undef DbgVerify
#undef MaxAssert64
#undef DbgAssert64
#undef assert64

// Enable this symbol in order to turn On evaluation of DbgAsserts in Release builds
// We'll turn it on automatically for pre-release versions.

#undef DBG_ASSERT_ACTIVE_IN_RELEASE_BUILD
#undef REL_ASSERT_ACTIVE_IN_RELEASE_BUILD

// You can do a global override to always, always, always prevent 'release asserts'
// using /DMAX_DISABLE_RELEASE_ASSERT.
// 
// is: set CL=/DMAX_DISABLE_RELEASE_ASSERT
#ifndef MAX_DISABLE_RELEASE_ASSERT
#   ifdef ALPHABETA
#       define DBG_ASSERT_ACTIVE_IN_RELEASE_BUILD
#       define REL_ASSERT_ACTIVE_IN_RELEASE_BUILD
#   endif
#endif

// SR NOTE: Finally disabling asserts in release builds, unless you're asking for them
// by setting REL_ASSERT_ACTIVE_IN_RELEASE_BUILD.
// 
#if !defined( NDEBUG ) || defined( REL_ASSERT_ACTIVE_IN_RELEASE_BUILD )
#  define assert( expr ) ( expr || assert1( __LINE__, __FILE__, __FUNCTION__, (#expr) ) )
#  define MaxAssert( expr ) ( (expr) || assert1( __LINE__, __FILE__, __FUNCTION__, (#expr) ) )
#else    
#  define assert( expr )      ((void *)0)
#  define MaxAssert( expr )   ((void *)0)
#endif   

#if !defined( NDEBUG ) || defined( DBG_ASSERT_ACTIVE_IN_RELEASE_BUILD )
#  define DbgAssert( expr ) ( (expr) || assert1( __LINE__, __FILE__, __FUNCTION__, (#expr) ) )
#  define DbgVerify( expr ) ( (expr) || assert1( __LINE__, __FILE__, __FUNCTION__, (#expr) ) )
#else	
#  define DbgAssert( expr )
#  define DbgVerify( expr ) ( expr )
#endif	

#if defined( _WIN64 )
// Verify that a condition holds only in x64 builds.
// typical usage would be DbgAssert64( size_t_value == static_cast<DWORD>(size_t_value) );
#  define DbgAssert64   DbgAssert
#  define MaxAssert64   MaxAssert
#  define assert64      assert
#else
#  define DbgAssert64( expr )   ((void*)0)
#  define MaxAssert64( expr )   ((void*)0)
#  define assert64( expr )      ((void*)0)
#endif

//-----------------------------------------------------------------------------
//-- Assert callback mechanism. (GG:06/27/02)

/*! \brief Callback class that allows for hooking into 3ds Max's assertion mechanims
 *
 * Plugins can derive from this class and overwrite the CAssertCB::AssertEx method
 * if they wish to execute a custom action, such as logging, when an assertion fails 
 * during execution of 3ds Max. See CAssertCB::AssertResult for more information on
 * the customization options this callback mechanism offers.
*/
class CAssertCB: public MaxHeapOperators 
{
	public:
	/*! \brief Possible return values for callback method (CAssertCB::AssertEx) invoked 
	 * when an assertion fails.
	 */
	enum AssertResult 
	{
		ASSERTCB_IGNORE,	//!< Ignore the assertion and continue. No assertion dialog is displayed.
		ASSERTCB_CANCEL,	//!< Cancel the execution. 3ds Max will be exit.
		ASSERTCB_CONTINUE	//!< Proceed with the built in handling of the assertion (display assert dialog)
	};

	/*! \deprecated This method has been deprecated. The assert callback mechanism calls CAssertCB::AssertEx instead. */
	MAX_DEPRECATED virtual int Assert(int line, char* file) { return ASSERTCB_CONTINUE; };

	/*! \brief Callback method invoked when an assertion fails
	 *
	 * \param line - the line number where the assertion occurred
	 * \param file - the file path and name where the assertion occurred
	 * \param function - the non-decorated name of the function in which the assertion occurred
	 * \param expression - the expression that was asserted 
	 * \return One of the possible AssertResult values.
	 */
	virtual AssertResult AssertEx(int line, char* file, char* function = NULL, char* expression = NULL) =0;
};

/*! \brief Sets a callback object that allows for performing a custom action when 
 * an assertion fails (is triggered).
 *
 * A plugin can supply an object that allows for executing a custom action when 
 * an assertion fails in 3ds Max. For details on the extent of possible customization 
 * see class CAssertCB.
 * Currently, only one callback object can be active at any given time. 
 * This means that the callback installed by a plugin can be de-installed by another plugin.
 * Thus, plugins should keep their callbacks active for as short of a time period 
 * as possible, and restore the previous callback object when they de-activate their own.
 * \param cb - The new callback object to use 
 * \return The old callback object or NULL if there's none set
*/
UtilExport CAssertCB* SetAssertCB(CAssertCB* cb);


