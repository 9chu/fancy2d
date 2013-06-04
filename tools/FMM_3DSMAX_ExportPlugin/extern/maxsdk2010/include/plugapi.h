//**************************************************************************/
// Copyright (c) 1998-2007 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
// DESCRIPTION: Plugin DLL API
// AUTHOR: Dan Silva - Nov.30.1994
//***************************************************************************/

#ifndef PLUGAPI_H_DEFINED
#define PLUGAPI_H_DEFINED

#ifndef	_CPPRTTI
#error 3ds Max plugins and dlls must compile with run-time type information (RTTI) On.\
RTTI can be turned on via the Language tab in the C++ folder of the \
Visual Studio project settings of your project, or the /GR compiler flag.
#endif

#include "maxheap.h"
#include "maxtypes.h"
#include "buildver.h"	// russom - 03/08/01
#include "maxversion.h"

// 3DSMAX Version number to be compared against that returned by DLL's
// LibVersion function to detect obsolete DLLs.
//#define VERSION_3DSMAX 100      // changed  4/ 9/96 -- DB
//#define VERSION_3DSMAX 110      // changed  7/17/96 -- DS
//#define VERSION_3DSMAX 111      // changed  9/24/96 -- DS
//#define VERSION_3DSMAX 120      // changed  9/30/96 -- DB
//#define VERSION_3DSMAX 200      // changed  10/30/96 -- DS


// MAX release number X 1000
//#define MAX_RELEASE				2000      // DDS 9/30/97  
//#define MAX_RELEASE				2500      // JMK 2/25/98  
//#define MAX_RELEASE				3000      // DDS 9/3/98  
//#define MAX_RELEASE				3100      // CCJ 10/21/99  
//#define MAX_RELEASE				3900      // CCJ 6/26/00
//#define MAX_RELEASE				4000      // CCJ 11/29/00
//#define MAX_RELEASE				4200      // 010514  --prs.
//#define MAX_RELEASE				4900      // for R5 alpha, 011115  --prs.
//#define MAX_RELEASE				5000      // for R5, 020604  --prs.
//#define MAX_RELEASE				5100      // for R5.1, 020801  --prs.
//#define MAX_RELEASE				5900      // for R6 alpha, 030403  --prs.
//#define MAX_RELEASE				6000      // for R6, 030806  --prs.
//#define MAX_RELEASE_R7_ALPHA		6900 // for R7 alpha
#define MAX_RELEASE_R7					7000	// for R7 
#define MAX_RELEASE_R8_ALPHA		7800	// for R8 alpha
#define MAX_RELEASE_R8					8000	// for R8 
#define MAX_RELEASE_R9_ALPHA		8900	// for R9 alpha 
#define MAX_RELEASE_R9_ALPHA2		8901	// for R9 alpha - file format change
#define MAX_RELEASE_R9					9000	// for R9 release 
#define MAX_RELEASE_R10_ALPHA		9900	// for R10 alpha   
#define MAX_RELEASE_R10					10000	// for R10 release
#define MAX_RELEASE_R11_ALPHA		10900	// for R11 alpha
#define MAX_RELEASE_R11					11000	// for R11 release
#define MAX_RELEASE_R12_ALPHA		11900	// for R12 alpha
#define MAX_RELEASE_R12					12000	// for R12 release
#define MAX_RELEASE							MAX_RELEASE_R12

// MAX_RELEASE_EXTERNAL is an alternative for MAX_RELEASE
// that can be used for UI when the internal version of the product
// is different from the branded version number.
// MAX_RELEASE_EXTERNAL is defined in maxversion.h
#ifndef MAX_RELEASE_EXTERNAL
 #define MAX_RELEASE_EXTERNAL MAX_RELEASE
#endif


//! \brief Max SDK version numbers
/*! The SDK version number is updated every time a build of Max containing SDK 
changes is distributed to 3rd party plugin developers. This can be an alpha, beta 
or release candidate build.
Although we make every effort to keep Max backward compatible with plugins built 
against previous versions of the Max SDK, occasionally plugins are required to be 
re-built against the latest version of the Max SDK in order for Max to load them. 
Plugins built against a certain version of the Max SDK will not load in previous 
versions of Max. 
Note that the Max SDK version number is encoded in VERSION_3DSMAX. 

SDK backward compatibility history:
- No SDK break between Max R4.x and R5.0, thus plugins built against all R4.x 
versions will load into Max R5.0
- No SDK break between Max R6.x and R8.0, thus plugins built against all R6.x 
versions will load into Max R8.0
- Max R9.0 SDK breaks compatibility with previous versions of Max, thus plugins 
must be re-built in order to load in Max R9.0
- No SDK break between Max R9.x and R10.0 (3ds Max 2008), thus plugins built 
against all R9.x versions will load into Max R10.0

\see VERSION_3DSMAX
\see LibVersion
*/
// Blocks of values are reserved for specific products that are meant
// to have non-compatible APIs. Currently assigned:
//    000-???
//    255:     temporary use to prevent NOVA plugins from loading in MAX
#if defined(GAME_VER)			// russom 03/08/01

	// API numbers used by plugins to GMax. 
	#define GMAX_API_NUM_MAX_R50			128	// russom 01/17/01
	// Plugins to a version of GMax that is based on Max 9 would need to recompile 
	#define GMAX_API_NUM_MAX_R90			129	
	#define MAX_API_NUM						GMAX_API_NUM_MAX_R9	

#elif defined(WEBVERSION)

	// API numbers used by plugins to Plasma
	#define PLASMA_API_NUM_MAX_R50			144	// russom 02/07/01
	// Plugins to a version of Plasma that is based on Max 9 would need to recompile 
	#define PLASMA_API_NUM_MAX_R90			145
	#define MAX_API_NUM						PLASMA_API_NUM_MAX_R9

#elif defined(RENDER_VER)

	// API numbers used by plugins to Viz Render
	#define VIZRENDER_API_NUM_MAX_R50		160	// cleve ard 11/6/02
	#define VIZRENDER_API_NUM_MAX_R90		161	
	#define MAX_API_NUM						VIZRENDER_API_NUM_MAX_R9

#else

	// API numbers used by plugins to Max. The API number needs to be incremented for
	// every Max build (alpha, beta, final version) that is distributed to 3rd party 
	// developers and contains SDK changes.

	#define MAX_API_NUM_R20					4	   // DDS 9/30/97	
	#define MAX_API_NUM_R25					5	   // DDS 10/06/97
	#define MAX_API_NUM_R30					6	   // DDS 9/3/98
	#define MAX_API_NUM_R31					7	   // CCJ 5/14/99
	#define MAX_API_NUM_R40					8	   // CCJ 11/29/00 - R4

#ifndef MAXIMUS_BUILD

	// API numbers used by plugins to Max
	#define MAX_API_NUM_R42						09	 // R5 011031  --prs.
	#define MAX_API_NUM_R50						11	 // R5 FCS 020624  --prs.
	#define MAX_API_NUM_R60_PRE_REL		12	 // R6 prerelease 030409  --prs.
	#define MAX_API_NUM_R60						13	 // R6 FCS 030806  --prs.
	#define MAX_API_NUM_R70_PRE_REL		14	 // R7 prerelease 05/18/04 - russom
	#define MAX_API_NUM_R70						15	 // R7 FCS -- 2004-09-02 -- raffenn
	#define MAX_API_NUM_R80_PRE_REL		16	 // R8 pre-release Vesper -- 2005-01-18 -- raffenn
	#define MAX_API_NUM_R80						17	 // R8 FCS max 8 -- 2005-09-01 -- raffenn
	#define MAX_API_NUM_R90_ALPHA			18	 // R9 alpha -- 2005-09-21 -- aszabo
	#define MAX_API_NUM_R90_BETA1			19	 // R9 SDK Freeze for Beta 1 
	#define MAX_API_NUM_R90_BETA3			20	 // R9 SDK Freeze for Beta 3 (skipped beta 2)
	#define MAX_API_NUM_R90						21	 // R9 release - SDK incompatible with Max R8
	#define MAX_API_NUM_R100_BETA5		22	 // R10.0 Beta 5 - 2007-07-27 - aszabo
	#define MAX_API_NUM_R100					23	 // R10.0 release - SDK compatible with Max R9
	#define MAX_API_NUM_R110_ALPHA		24	 // R11.0 alpha - SDK is incompatible with Max R10
	#define MAX_API_NUM_R110					25	 // R11.0 release - SDK is compatible with Max R11 Alpha
	#define MAX_API_NUM_R120_ALPHA2		26	 // R12.0 Alpha 2 - SDK is incompatible with Max R11 (we skipped Alpha 1)
	#define MAX_API_NUM_R120_ALPHA3		27	 // R12.0 Alpha 3 - SDK is incompatible with Max R12 Alpha 2
	#define MAX_API_NUM_R120_ALPHA4		28	 // R12.0 Alpha 4 - SDK is incompatible with Max R12 Alpha 3
	#define MAX_API_NUM_R120_BETA1		29	 // R12.0 Beta 1 - SDK is incompatible with Max R12 Alpha 4
	#define MAX_API_NUM_R120_BETA3		30	 // R12.0 Beta 3 - SDK is compatible with Max R12 Beta 1 (Beta 2 SDK was identical to Beta 1 SDK)
	#define MAX_API_NUM_R120_BETA4		31	 // R12.0 Beta 4 - SDK is compatible with Max R12 Beta 3 
	#define MAX_API_NUM_R120_BETA5		32	 // R12.0 Beta 5 - SDK is compatible with Max R12 Beta 4 
	#define MAX_API_NUM_R120					33	 // R12.0 Release - SDK is compatible with Max R12 Beta 5 
	#define MAX_API_NUM								MAX_API_NUM_R120

#else	// if MAXIMUS_BUILD:
	
	#define MAXIMUS_API_NUM_MAX_R50			10	 // Maximus 020315  --prs.
	#define MAX_API_NUM						MAXIMUS_API_NUM_MAX_R50

#endif // Maximus

#endif // products

// This denotes the revision of the SDK for a given API. Increment this
// when the SDK functionality changes in some significant way (for instance
// a new GetProperty() query  response is added), but the headers have 
// not been changed.
#define MAX_SDK_REV     0	  // DDS 9/20/97

// This value should be returned by the LibVersion() method for DLL's:
#define VERSION_3DSMAX ((MAX_RELEASE<<16)+(MAX_API_NUM<<8)+MAX_SDK_REV)
#ifdef DESIGN_VER
#define VERSION_DMAX ((MAX_RELEASE<<16)+(MAX_API_NUM<<8)+MAX_SDK_REV)
#endif

// update jagapi.cpp product_app_id if new enums are added
enum APPLICATION_ID {kAPP_NONE, kAPP_MAX, kAPP_VIZ, kAPP_GMAX, kAPP_PLASMA, kAPP_VIZR};

//This method returns the ApplicationID, either VIZ or MAX. If a plugin
//is designed to work only in one product, then you could use this method
//in your IsPublic() call to switch between exposing the plug-in or not.
CoreExport APPLICATION_ID GetAppID();

// Macros for extracting parts of VERSION_3DSMAX
#define GET_MAX_RELEASE(x)  (((x)>>16)&0xffff)
#define GET_MAX_API_NUM(x)  (((x)>>8)&0xff)
#define GET_MAX_SDK_REV(x)  ((x)&0xff)
#define GET_MAX_SDK_NUMREV(x)  ((x)&0xffff) 

// Internal super classes, that plug-in developers need not know about
#define GEN_MODAPP_CLASS_ID 		0x00000b
#define MODAPP_CLASS_ID 			0x00000c
#define OBREF_MODAPP_CLASS_ID 		0x00000d
#define BASENODE_CLASS_ID			0x000001
#define GEN_DERIVOB_CLASS_ID 		0x000002
#define DERIVOB_CLASS_ID 			0x000003
#define WSM_DERIVOB_CLASS_ID 		0x000004
#define PARAMETER_BLOCK_CLASS_ID 	0x000008	
#define PARAMETER_BLOCK2_CLASS_ID 	0x000082	// <JBW> ParamBlocks Ed. 2
#define EASE_LIST_CLASS_ID			0x000009
#define AXIS_DISPLAY_CLASS_ID		0x00000e
#define MULT_LIST_CLASS_ID			0x00000f
#define NOTETRACK_CLASS_ID			0x0000ff
#define TREE_VIEW_CLASS_ID			0xffffff00
#define SCENE_CLASS_ID				0xfffffd00
#define THE_GRIDREF_CLASS_ID		0xfffffe00
#define VIEWREF_CLASS_ID			0xffffff01
#define BITMAPDAD_CLASS_ID			0xffffff02 // for drag/drop of bitmaps
#define PARTICLE_SYS_CLASS_ID		0xffffff03 // NOTE: this is for internal use only. Particle systems return GEOMOBJECT_CLASS_ID -- use IsParticleObject() to determine if an object is a particle system.
#define AGGMAN_CLASS_ID				0xffffff05 // Object aggregation, VIZ
#define MAXSCRIPT_WRAPPER_CLASS_ID	0xffffff06 // JBW, 5/14/99, MAX object wrappers within MAXScript
#define TRACKBAR_CLASS_ID			0xffffff07	// nleonard 22-DEC-05 the trackbar
#define MAKEREF_REST_CLASS_ID		0x9876544	// the superclassid for MakeRefRestore
#define DELREF_REST_CLASS_ID		0x9876543	// the superclassid for DeleteRefRestore
#define ASSIGNREF_REST_CLASS_ID		0x121242	// the superclassid for AssignMtlRestore

//-------------------------------------------
// Super-classes that are plugable.
#define GEOMOBJECT_CLASS_ID			0x000010
#define CAMERA_CLASS_ID				0x000020
#define LIGHT_CLASS_ID				0x000030
#define SHAPE_CLASS_ID				0x000040
#define HELPER_CLASS_ID				0x000050
#define SYSTEM_CLASS_ID	 			0x000060 
#define REF_MAKER_CLASS_ID			0x000100 	
#define REF_TARGET_CLASS_ID	 		0x000200
#define OSM_CLASS_ID				0x000810
#define WSM_CLASS_ID				0x000820
#define WSM_OBJECT_CLASS_ID			0x000830
#define SCENE_IMPORT_CLASS_ID		0x000A10
#define SCENE_EXPORT_CLASS_ID		0x000A20
#define BMM_STORAGE_CLASS_ID		0x000B10
#define BMM_FILTER_CLASS_ID			0x000B20
#define BMM_IO_CLASS_ID				0x000B30
#define BMM_DITHER_CLASS_ID			0x000B40
#define BMM_COLORCUT_CLASS_ID		0x000B50
#define USERDATATYPE_CLASS_ID		0x000B60
#define MATERIAL_CLASS_ID			0x000C00    // Materials
#define TEXMAP_CLASS_ID				0x000C10    // Texture maps
#define UVGEN_CLASS_ID				0x0000C20   // UV Generator
#define XYZGEN_CLASS_ID				0x0000C30   // XYZ Generator
#define TEXOUTPUT_CLASS_ID			0x0000C40   // Texture output filter 
#define SOUNDOBJ_CLASS_ID			0x000D00
#define FLT_CLASS_ID				0x000E00
#define RENDERER_CLASS_ID			0x000F00
#define BEZFONT_LOADER_CLASS_ID		0x001000
#define ATMOSPHERIC_CLASS_ID		0x001010
#define UTILITY_CLASS_ID			0x001020	// Utility plug-ins
#define TRACKVIEW_UTILITY_CLASS_ID	0x001030
#define FRONTEND_CONTROL_CLASS_ID	0x001040
#define MOT_CAP_DEV_CLASS_ID		0x001060
#define MOT_CAP_DEVBINDING_CLASS_ID	0x001050
#define OSNAP_CLASS_ID				0x001070
#define TEXMAP_CONTAINER_CLASS_ID	0x001080    // Texture map container
#define RENDER_EFFECT_CLASS_ID      0x001090    // Render post-effects
#define FILTER_KERNEL_CLASS_ID      0x0010a0    // AA Filter kernel
#define SHADER_CLASS_ID				0x0010b0    // plugable shading into stdmtl
#define COLPICK_CLASS_ID		  	0x0010c0    // color picker
#define SHADOW_TYPE_CLASS_ID		0x0010d0    // shadow generators
#define GUP_CLASS_ID		  		0x0010e0    // Global Utility Plug-In
#define LAYER_CLASS_ID				0x0010f0
#define SCHEMATICVIEW_UTILITY_CLASS_ID	0x001100
#define SAMPLER_CLASS_ID			0x001110
#define ASSOC_CLASS_ID				0x001120
#define GLOBAL_ASSOC_CLASS_ID		0x001130
#define IK_SOLVER_CLASS_ID			0x001140
#define RENDER_ELEMENT_CLASS_ID		0x001150	// render output elements
#define BAKE_ELEMENT_CLASS_ID		0x001151	// texture bake output elements
#define CUST_ATTRIB_CLASS_ID		0x001160    // Custom attributes
#define RADIOSITY_CLASS_ID			0x001170	// global illumination plugins
#define TONE_OPERATOR_CLASS_ID		0x001180	// tone operator plugins
#define MPASS_CAM_EFFECT_CLASS_ID	0x001190	// multi-pass camera effect plugins
#define MR_SHADER_CLASS_ID_DEFUNCT	0x0011a0	// mental ray shaders (defunct, kept for file compatibility)

// Super-class ID's of various controls
#define	CTRL_SHORT_CLASS_ID 	   	0x9001
#define	CTRL_INTEGER_CLASS_ID		0x9002
#define	CTRL_FLOAT_CLASS_ID			0x9003
#define	CTRL_POINT2_CLASS_ID	   	0x9004
#define	CTRL_POINT3_CLASS_ID	   	0x9005
#define	CTRL_POS_CLASS_ID		   	0x9006
#define	CTRL_QUAT_CLASS_ID			0x9007
#define	CTRL_MATRIX3_CLASS_ID		0x9008
#define	CTRL_COLOR_CLASS_ID     	0x9009	// float color (RGB)
#define	CTRL_COLOR24_CLASS_ID   	0x900A   // 24 bit color
#define	CTRL_POSITION_CLASS_ID		0x900B
#define	CTRL_ROTATION_CLASS_ID		0x900C
#define	CTRL_SCALE_CLASS_ID			0x900D
#define CTRL_MORPH_CLASS_ID			0x900E
#define CTRL_USERTYPE_CLASS_ID		0x900F  // User defined type
#define CTRL_MASTERPOINT_CLASS_ID	0x9010	
#define MASTERBLOCK_SUPER_CLASS_ID	0x9011
#define	CTRL_POINT4_CLASS_ID	   	0x9012
#define	CTRL_FRGBA_CLASS_ID     	0x9013	// float color (RGBA)

//------------------------------------------------------------------------------
// Pseudo-super-class ids
#define DEFORM_OBJ_CLASS_ID 		0x000005 // Deformable object psuedo-super-class
#define MAPPABLE_OBJ_CLASS_ID 		0x000006 // Mappable object psuedo-super-class
#define GENERIC_SHAPE_CLASS_ID		0x0000ab // Shape psuedo-super-class

//------------------------------------------------------------------------------
// Class ID's of built-in classes. The value is the first ULONG of the 
// 8 byte Class ID: the second ULONG is 0 for all built-in classes.
// NOTE: Only built-in classes should have the second ULONG == 0.
#define PATH_CONTROL_CLASS_ID				0x2011
#define EULER_CONTROL_CLASS_ID				0x2012
#define EXPR_POS_CONTROL_CLASS_ID			0x2013
#define EXPR_P3_CONTROL_CLASS_ID			0x2014
#define EXPR_FLOAT_CONTROL_CLASS_ID			0x2015
#define EXPR_SCALE_CONTROL_CLASS_ID			0x2016
#define EXPR_ROT_CONTROL_CLASS_ID			0x2017
#define LOCAL_EULER_CONTROL_CLASS_ID		0x2018
#define POSITION_CONSTRAINT_CLASS_ID		0x2019  // AG added for Position Constraint 4/21/2000
#define ORIENTATION_CONSTRAINT_CLASS_ID		0x2020  // AG added for Position Constraint 5/04/2000
#define LOOKAT_CONSTRAINT_CLASS_ID			0x2021  // AG added for new LookAt Constraint 5/24/2000
#define ADDITIVE_EULER_CONTROL_CLASS_ID		0x2022	// 000830  --prs.
#define BOOLCNTRL_CLASS_ID					Class_ID(0x5ed56fc5, 0x6af0e521) // AG added for the new Boolean Controller 11/08/2001


#define FLOATNOISE_CONTROL_CLASS_ID		0x87a6df24
#define POSITIONNOISE_CONTROL_CLASS_ID	0x87a6df25
#define POINT3NOISE_CONTROL_CLASS_ID	0x87a6df26
#define ROTATIONNOISE_CONTROL_CLASS_ID	0x87a6df27
#define SCALENOISE_CONTROL_CLASS_ID		0x87a6df28
#define SURF_CONTROL_CLASSID			Class_ID(0xe8334011,0xaeb330c8)
#define LINKCTRL_CLASSID				Class_ID(0x873fe764,0xaabe8601)
#define ATTACH_CONTROL_CLASS_ID			Class_ID(0xbb27e611,0xa72f43e7)

#define FLOATLIMITCTRL_CLASS_ID			0x17127e3e

#define DUMMYCHANNEL_CLASS_ID				0xefffffff // used by PB and PB2 - superclass is CTRL_USERTYPE_CLASS_ID

#define RINGARRAY_CLASS_ID			0x9120 // use it as Class_ID(RINGARRAY_CLASS_ID,0);
#define SUNLIGHT_CLASS_ID			Class_ID(0x5897670e, 0x61b5008d)
#define DAYLIGHT_CLASS_ID			Class_ID(0x18147db5, 0x20f17194)

#define CUSTATTRIB_CONTAINER_CLASS_ID Class_ID(0x5ddb3626, 0x23b708db) // CustAttribContainer class id


// subclasses of GEOMOBJECT_CLASS_ID:
// Built into CORE
#define TRIOBJ_CLASS_ID 	 	0x0009	  
#define EDITTRIOBJ_CLASS_ID	0xe44f10b3	// base triangle mesh
#define POLYOBJ_CLASS_ID		0x5d21369a	// polygon mesh
#define PATCHOBJ_CLASS_ID  		0x1030
#define NURBSOBJ_CLASS_ID		0x4135

// editable poly
#define EPOLYOBJ_CLASS_ID Class_ID(0x1bf8338d,0x192f6098)

// Primitives
// xavier robitaille | 03.02.15 
// Prevents the user from creating box/cylinder/sphere objects without
// removing them completely (we need them for the material editor).
#ifndef NO_OBJECT_STANDARD_PRIMITIVES
#define BOXOBJ_CLASS_ID 		0x0010
#define SPHERE_CLASS_ID 		0x0011 
#define CYLINDER_CLASS_ID 		0x0012
#define PLANE_CLASS_ID          Class_ID(0x81f1dfc, 0x77566f65)
#define PYRAMID_CLASS_ID        Class_ID(0x76bf318a, 0x4bf37b10)
#define GSPHERE_CLASS_ID        Class_ID(0, 32670)

#else
#define BOXOBJ_CLASS_ID 		0x108f1589
#define SPHERE_CLASS_ID 		0x72ce5ad6 
#define CYLINDER_CLASS_ID 		0x759e40e1
#endif
#define CONE_CLASS_ID			0xa86c23dd
#define TORUS_CLASS_ID			0x0020
#define TUBE_CLASS_ID			0x7B21
#define HEDRA_CLASS_ID			0xf21c5e23
#define BOOLOBJ_CLASS_ID		0x387BB2BB
// Above used in obselete Boolean -- henceforth use following:
#define NEWBOOL_CLASS_ID Class_ID(0x51db4f2f,0x1c596b1a)

// Class ID for XRef object
#define XREFOBJ_CLASS_ID		Class_ID(0x92aab38c, 0)
// Class ID for Xref atmospherics
#define XREFATMOS_CLASS_ID	Class_ID(0x4869d60f, 0x21af2ae4)
// Class ID for Xref Materials
#define XREFMATERIAL_CLASS_ID Class_ID(0x272c0d4b, 0x432a414b)
// Class ID for Xref Controllers
#define XREFCTRL_CLASS_ID Class_ID(0x32fb4637, 0x65fd482b)

//Subclasses of object snaps.
#define GRID_OSNAP_CLASS_ID Class_ID(0x62f565d6, 0x110a1f97)


// The teapot is unique in that it uses both DWORDs in its class IDs
// Note that this is what 3rd party plug-ins SHOULD do.
#define TEAPOT_CLASS_ID1		0xACAD13D3
#define TEAPOT_CLASS_ID2		0xACAD26D9

#define PATCHGRID_CLASS_ID  	0x1070

// New procedural bone object for Magma (subclass of GeomObject)
#define BONE_OBJ_CLASSID		Class_ID(0x28bf6e8d, 0x2ecca840)


// Particles
#define RAIN_CLASS_ID			0x9bd61aa0
#define SNOW_CLASS_ID			0x9bd61aa1

// Space Warp Objects
#define WAVEOBJ_CLASS_ID 		0x0013

// The basic lofter class
#define LOFTOBJ_CLASS_ID		0x1035
#define LOFT_DEFCURVE_CLASS_ID	0x1036

// Our implementation of the lofter
#define LOFT_GENERIC_CLASS_ID	0x10B0

#define TARGET_CLASS_ID  		0x1020  // should this be a helper?
#define MORPHOBJ_CLASS_ID		0x1021

// Subclasses of SHAPE_CLASS_ID
#define SPLINESHAPE_CLASS_ID 	0x00000a
#define LINEARSHAPE_CLASS_ID 	0x0000aa
#define SPLINE3D_CLASS_ID  		0x1040
#define NGON_CLASS_ID  			0x1050
#define DONUT_CLASS_ID  		0x1060
#define STAR_CLASS_ID			0x1995
#define RECTANGLE_CLASS_ID		0x1065
#define HELIX_CLASS_ID			0x1994
#define ELLIPSE_CLASS_ID		0x1097
#define CIRCLE_CLASS_ID			0x1999
#define TEXT_CLASS_ID			0x1993
#define ARC_CLASS_ID			0x1996
#define HALF_ROUND_CLASS_ID		0x1997
#define QUARTER_ROUND_CLASS_ID	0x1998
#define PIPE_CLASS_ID			0x199A
#define WALLED_RECTANGLE_CLASS_ID 0xd8bff66c
#define CHANNEL_CLASS_ID		0x7dbc0e96
#define ANGLE_CLASS_ID			0x93257030
#define TEE_CLASS_ID			0xfaead70c
#define WIDE_FLANGE_CLASS_ID	0xa2b72ef6


// subclasses of CAMERA_CLASS_ID:
#define SIMPLE_CAM_CLASS_ID  	0x1001
#define LOOKAT_CAM_CLASS_ID  	0x1002

// subclasses of LIGHT_CLASS_ID:
#define OMNI_LIGHT_CLASS_ID  		0x1011
#define SPOT_LIGHT_CLASS_ID  		0x1012
#define DIR_LIGHT_CLASS_ID  		0x1013
#define FSPOT_LIGHT_CLASS_ID  		0x1014
#define TDIR_LIGHT_CLASS_ID  		0x1015

// subclasses of HELPER_CLASS_ID
#define DUMMY_CLASS_ID 			0x876234
#define BONE_CLASS_ID 			0x8a63c0
#define TAPEHELP_CLASS_ID 		0x02011
#define GRIDHELP_CLASS_ID		0x02010
#define POINTHELP_CLASS_ID		0x02013
#define PROTHELP_CLASS_ID		0x02014

//subclasses of UVGEN_CLASS_ID
#define STDUV_CLASS_ID 			0x0000100

//subclasses of XYZGEN_CLASS_ID
#define STDXYZ_CLASS_ID 		0x0000100

//subclasses of TEXOUT_CLASS_ID
#define STDTEXOUT_CLASS_ID 		0x0000100

// subclasses of MATERIAL_CLASS_ID	
#define DMTL_CLASS_ID  			0x00000002	// Origninal Stdmtl
#define DMTL2_CLASS_ID  		0x00000003	// R2.5 stdmtl

#ifndef NO_MTL_TOPBOTTOM // orb 01-07-2001
#define CMTL_CLASS_ID 			0x0000100  // top-bottom material 
#endif // NO_MTL_TOPBOTTOM

#define MULTI_CLASS_ID 			0x0000200  // super class ID multi material

// the actual Class_ID used by the multi-sub-object material
#define MULTI_MATERIAL_CLASS_ID		Class_ID(MULTI_CLASS_ID, 0) 

#define DOUBLESIDED_CLASS_ID 	0x0000210  // double-sided mtl
#define MIXMAT_CLASS_ID 		0x0000250  // blend mtl
#define BAKE_SHELL_CLASS_ID 	0x0000255  // 2 material shell for baking

#ifndef NO_MTL_MATTESHADOW // orb 01-07-2001
#define MATTE_CLASS_ID 			0x0000260  // Matte mtl
#endif // NO_MTL_MATTESHADOW

// subclasses of TEXMAP_CLASS_ID	
#define CHECKER_CLASS_ID 		0x0000200
#define MARBLE_CLASS_ID 		0x0000210
#define MASK_CLASS_ID 			0x0000220  // mask texture
#define MIX_CLASS_ID 			0x0000230
#define NOISE_CLASS_ID 			0x0000234

#ifndef NO_MAPTYPE_GRADIENT // orb 01-07-2001
#define GRADIENT_CLASS_ID 		0x0000270
#endif // NO_MAPTYPE_GRADIENT

#ifndef NO_MAPTYPE_RGBTINT // orb 01-07-2001
#define TINT_CLASS_ID 			0x0000224  // Tint texture
#endif // NO_MAPTYPE_RGBTINT

#define BMTEX_CLASS_ID 			0x0000240  // Bitmap texture

#ifndef NO_MAPTYPE_REFLECTREFRACT // orb 01-07-2001
#define ACUBIC_CLASS_ID 		0x0000250  // Reflect/refract
#endif // NO_MAPTYPE_REFLECTREFRACT

#ifndef NO_MAPTYPE_FLATMIRROR // orb 01-07-2001
#define MIRROR_CLASS_ID 		0x0000260  // Flat mirror
#endif // NO_MAPTYPE_FLATMIRROR

#define COMPOSITE_CLASS_ID 		0x0000280   // Composite texture
#define COMPOSITE_MATERIAL_CLASS_ID Class_ID(0x61dc0cd7, 0x13640af6)  // Composite Material

#ifndef NO_MAPTYPE_RGBMULT // orb 01-07-2001
#define RGBMULT_CLASS_ID 		0x0000290   // RGB Multiply texture
#endif //NO_MAPTYPE_RGBMULT

#define FALLOFF_CLASS_ID 		0x00002A0   // Falloff texture

#ifndef NO_MAPTYPE_OUTPUT // orb 01-07-2001
#define OUTPUT_CLASS_ID 		0x00002B0   // Output texture
#endif // NO_MAPTYPE_OUTPUT

#define PLATET_CLASS_ID 		0x00002C0   // Plate glass texture

#define COLORCORRECTION_CLASS_ID    0x00002D0   // Color Correction texture

#ifndef NO_MAPTYPE_VERTCOLOR // orb 01-07-2001
#define VCOL_CLASS_ID 			0x0934851	// Vertex color map
#endif // NO_MAPTYPE_VERTCOLOR

// Subclasses of SHADER_CLASS_ID
#define STDSHADERS_CLASS_ID		0x00000035	// to 39 

// Subclasses of SHADOW_TYPE_CLASS_ID	
#define STD_SHADOW_MAP_CLASS_ID       0x0000100
#define STD_RAYTRACE_SHADOW_CLASS_ID  0x0000200
		
// subclasses of RENDERER_CLASS_ID		  
#define SREND_CLASS_ID 			0x000001 // default scan-line renderer

// subclasses of BAKE_ELEMENT_CLASS_ID
#define COMPLETE_BAKE_ELEMENT_CLASS_ID			0x00010001
#define SPECULAR_BAKE_ELEMENT_CLASS_ID			0x00010002
#define DIFFUSE_BAKE_ELEMENT_CLASS_ID			0x00010003
#define REFLECT_REFRACT_BAKE_ELEMENT_CLASS_ID	0x00010004
#define SHADOW_BAKE_ELEMENT_CLASS_ID			0x00010005
#define LIGHT_BAKE_ELEMENT_CLASS_ID				0x00010006
#define NORMALS_ELEMENT_CLASS_ID				0x00010007
#define BLEND_BAKE_ELEMENT_CLASS_ID				0x00010008
#define ALPHA_BAKE_ELEMENT_CLASS_ID				0x00010009
#define HEIGHT_BAKE_ELEMENT_CLASS_ID			0x0001000A
#define AMBIENTOCCLUSION_BAKE_ELEMENT_CLASS_ID	0x0001000B

// subclasses of REF_MAKER_CLASS_ID			
#define MTL_LIB_CLASS_ID 		0x001111
#define MTLBASE_LIB_CLASS_ID 	0x003333
#define THE_SCENE_CLASS_ID   	0x002222
#define MEDIT_CLASS_ID 	 		0x000C80
#define MTL_CATEGORY_CLASS_ID   Class_ID(0xb8e3b2d, 0x19854ad2)

// subclass of all classes
#define STANDIN_CLASS_ID   		0xffffffff  // subclass of all super classes



// Default sound object
#define DEF_SOUNDOBJ_CLASS_ID	0x0000001

// Default atmosphere
#define FOG_CLASS_ID 0x10000001

//------------------ Modifier sub classes --------
#define SKEWOSM_CLASS_ID			0x6f3cc2aa
#define BENDOSM_CLASS_ID 			0x00010
#define TAPEROSM_CLASS_ID 			0x00020
#define TWISTOSM_CLASS_ID 			0x00090
#define SPLINEIKCONTROL_CLASS_ID	Class_ID(0x5f43ba4, 0x55fe9305) //AG added : 01/08/02

#define UVWMAPOSM_CLASS_ID			0xf72b1
#define SELECTOSM_CLASS_ID			0xf8611
#define MATERIALOSM_CLASS_ID		0xf8612
#define SMOOTHOSM_CLASS_ID			0xf8613
#define NORMALOSM_CLASS_ID			0xf8614
#define OPTIMIZEOSM_CLASS_ID		0xc4d31
#define AFFECTREGION_CLASS_ID		0xc4e32
#define SUB_EXTRUDE_CLASS_ID		0xc3a32
#define TESSELLATE_CLASS_ID			0xa3b26ff2
#define DELETE_CLASS_ID				0xf826ee01
#define MESHSELECT_CLASS_ID			0x73d8ff93
#define UVW_XFORM_CLASS_ID			0x5f32de12
#define UVW_XFORM2_CLASS_ID			0x5f32de13 //SS 11/20/2002: added

#define EXTRUDEOSM_CLASS_ID 		0x000A0
#define SURFREVOSM_CLASS_ID 		0x000B0

#define DISPLACEOSM_CLASS_ID		0xc4d32
#define DISPLACE_OBJECT_CLASS_ID	0xe5240
#define DISPLACE_WSM_CLASS_ID		0xe5241

#define SINEWAVE_OBJECT_CLASS_ID 	0x00030
#define SINEWAVE_CLASS_ID 			0x00040
#define SINEWAVE_OMOD_CLASS_ID 		0x00045
#define LINWAVE_OBJECT_CLASS_ID 	0x00035
#define LINWAVE_CLASS_ID 			0x00042
#define LINWAVE_OMOD_CLASS_ID 		0x00047

#define GRAVITYOBJECT_CLASS_ID		0xe523c
#define GRAVITYMOD_CLASS_ID			0xe523d
#define WINDOBJECT_CLASS_ID			0xe523e
#define WINDMOD_CLASS_ID			0xe523f

#define DEFLECTOBJECT_CLASS_ID		0xe5242
#define DEFLECTMOD_CLASS_ID			0xe5243

#define BOMB_OBJECT_CLASS_ID 		0xf2e32
#define BOMB_CLASS_ID 				0xf2e42

// These are the FFD Modifier's class ID's
#define FFDNMOSSQUARE_CLASS_ID		Class_ID(0x8ab36cc5,0x82d7fe74)
#define FFDNMWSSQUARE_CLASS_ID		Class_ID(0x67ea40b3,0xfe7a30c4)
#define FFDNMWSSQUARE_MOD_CLASS_ID	Class_ID(0xd6636ea2,0x9aa42bf3)

#define FFDNMOSCYL_CLASS_ID			Class_ID(0x98f37a63,0x3ffe9bca)
#define FFDNMWSCYL_CLASS_ID			Class_ID(0xfa4700be,0xbbe85051)
#define FFDNMWSCYL_MOD_CLASS_ID		Class_ID(0xf1c630a3,0xaa8ff601)

#define FFD44_CLASS_ID				Class_ID(0x21325596, 0x2cd10bd8)
#define FFD33_CLASS_ID				Class_ID(0x21325596, 0x2cd10bd9)
#define FFD22_CLASS_ID				Class_ID(0x21325596, 0x2cd10bd0)

//JH Association context modifiers
//GEOM TO GEOM
#define ACMOD_GEOM_GEOM_BOOLADD_CID	0x4e0f483a
#define ACMOD_GEOM_GEOM_BOOLSUB_CID	0x61661a5c
#define ACMOD_GEOM_GEOM_BOOLINT_CID	0x2a4f3945
#define ACMOD_GEOM_GEOM_SIMPAGG_CID	0x40cb05ab

//SHAPE To GEOM
#define ACMOD_SHAPE_GEOM_HOLE_CID	0x366307b0
#define ACMOD_SHAPE_GEOM_INT_CID	0x782d8d50
#define ACMOD_SHAPE_GEOM_EMBOSS_CID	0x7a13397c
#define ACMOD_SHAPE_GEOM_REVEAL_CID	0x55ed658c

//JH Solids
#define GENERIC_AMSOLID_CLASS_ID	Class_ID(0x5bb661e8, 0xa2c27f02)


//------------------ Controller sub classes --------
#define LININTERP_FLOAT_CLASS_ID 			0x2001
#define LININTERP_POSITION_CLASS_ID 		0x2002
#define LININTERP_ROTATION_CLASS_ID 		0x2003
#define LININTERP_SCALE_CLASS_ID			0x2004
#define PRS_CONTROL_CLASS_ID				0x2005
#define LOOKAT_CONTROL_CLASS_ID				0x2006				

#define HYBRIDINTERP_FLOAT_CLASS_ID 		0x2007
#define HYBRIDINTERP_POSITION_CLASS_ID 		0x2008
#define HYBRIDINTERP_ROTATION_CLASS_ID 		0x2009
#define HYBRIDINTERP_POINT3_CLASS_ID		0x200A
#define HYBRIDINTERP_SCALE_CLASS_ID			0x2010
#define HYBRIDINTERP_COLOR_CLASS_ID			0x2011
#define HYBRIDINTERP_POINT4_CLASS_ID		0x2012
#define HYBRIDINTERP_FRGBA_CLASS_ID			0x2013

#define TCBINTERP_FLOAT_CLASS_ID 			0x442311
#define TCBINTERP_POSITION_CLASS_ID 		0x442312
#define TCBINTERP_ROTATION_CLASS_ID 		0x442313
#define TCBINTERP_POINT3_CLASS_ID			0x442314
#define TCBINTERP_SCALE_CLASS_ID			0x442315
#define TCBINTERP_POINT4_CLASS_ID			0x442316

#define MASTERPOINTCONT_CLASS_ID			0xd9c20ff


//------------------ PF sub classes ---------------
#define PF_SUBCLASS_ID_PARTB		0x1eb34000
//-------------------sub class part A
#define PF_OPERATOR_SUBCLASS_ID 	0x74f93a02
#define PF_OPERATOR3D_SUBCLASS_ID 	0x74f93a03
#define PF_TEST_SUBCLASS_ID 		0x74f93a04
#define PF_TEST3D_SUBCLASS_ID 		0x74f93a05
#define PF_ITEM_SUBCLASS_ID 		0x74f93a06
#define PF_OBJECT_SUBCLASS_ID 		0x74f93a07

//--------------------------------------------------

class ISave;
class ILoad;
class Interface;
class ActionTable;
typedef short BlockID;
class ParamBlockDesc2;
class IParamBlock2;
class IObjParam;
class Animatable;
class ParamMap2UserDlgProc;
class IParamMap2;
class FPInterface;
class Manipulator;
class ReferenceTarget;


// System keeps a list of the DLL's found on startup.
// This is the interface to a single class
/*! \sa  Class ClassDesc2, Class IParamBlock2, Class ParamBlockDesc2, Class ILoad,  Class ISave,  Class Interface, Class Class_ID.\n\n
\par Description:
Class descriptors provide the system with information about the plug-in classes
in the DLL. The developer creates a class descriptor by deriving a class from
<b>ClassDesc</b> and implementing several of its methods.\n\n
In release 3.0 and later there are new methods which are supplied and
implemented by ClassDesc2. These methods relate to the parameter block2
system.\n\n
In release 4.0 plug-ins wishing to use the new Function Publishing system must
use <b>ClassDesc2</b> rather than this class for their class descriptors. See
<a href="ms-its:3dsmaxsdk.chm::/function_root.html">Function Publishing
System</a>.
\par Method Groups:
See <a href="class_class_desc_groups.html">Method Groups for Class ClassDesc</a>.
*/
class ClassDesc: public MaxHeapOperators {
	private:
		Tab<FPInterface*>		interfaces;		// the FnPub interfaces published by this plugin
	public:
		virtual					~ClassDesc() {}
		/*! \remarks Controls if the plug-in shows up in lists from the user to choose from.
		\return  If the plug-in can be picked and assigned by the user, as is
		usually the case, return TRUE. Certain plug-ins may be used privately
		by other plug-ins implemented in the same DLL and should not appear in
		lists for user to choose from. These plug-ins would return FALSE. */
		virtual int				IsPublic()=0;  // Show this in create branch?
		/*! \remarks 3ds Max calls this method when it needs a pointer to a new instance of
		the plug-in class. For example, if 3ds Max is loading a file from disk
		containing a previously used plug-in (procedural object, modifier,
		controller, etc...), it will call the plug-in's <b>Create()</b> method.
		The plug-in responds by allocating a new instance of its plug-in class.
		See the Advanced Topic section on
		<a href="ms-its:3dsmaxsdk.chm::/alloc_memory_allocation.html">Memory
		Allocation</a> for more details.
		\param loading This parameter is a flag indicating if the class being created is going
		to be loaded from a disk file. If the flag is TRUE, the plug-in may not
		have to perform any initialization of the object because the loading
		process will take care of it. See the Advanced Topics section on
		<a href="ms-its:3dsmaxsdk.chm::/loading_and_saving.html">Loading
		and Saving</a> for more information.\n\n
		Note: If this parameter is TRUE, developers must initialize their
		references to NULL. Otherwise 3ds Max may crash.\n\n
		3ds Max provides a default plug-in object creation process. Many
		plug-ins fit this form. When the system is about to create an instance
		of the plug-in object it calls a method
		<b>BaseObject::GetCreateMouseCallBack()</b>.This method returns a
		callback object whose <b>proc()</b> method handles the mouse input
		during its creation phase. Most of the work is then handled by the
		system. The procedural sphere is an example of this type of plug-in.
		Certain plug-ins may have special creation needs however. The target
		camera is an example of such a plug-in. Because it needs to create two
		nodes in the scene (the camera and the target) it requires a custom
		creation process. To support these plug-ins the following two methods
		are provided. They allow the plug-in to manage the creation process
		themselves. See
		<a href="ms-its:3dsmaxsdk.chm::/nodes_object_creation_methods.html">Object
		Creation Methods</a> for more details. */
		virtual void *			Create(BOOL loading=FALSE)=0;   // return a pointer to an instance of the class.
		
		/*! \remarks The custom creation process of the plug-in object is handled by this
		method. For example, a plug-in can create a custom command mode and
		push it on the command stack to handle the creation process.\n\n
		Important Note: A plug-in that doesn't want to participate in the
		standard object creation mechanism using CreateMouseCallBack
		<b>must</b> push a CommandMode on the stack in this method and remove
		it in <b>EndCreate()</b>. This is true even if the plug-in doesn't do
		anything inside the mode. A mode has to be pushed on the stack and then
		later popped off otherwise a crash will occur (if the default
		implementation of this method is not used). For more details on object
		creation see the Advanced Topics section
		<a href="ms-its:3dsmaxsdk.chm::/nodes_object_creation_methods.html">Object
		Creation Methods</a>.
		\param i An interface pointer the plug-in may use to call functions in 3ds Max.
		\return  To use the default creation process (the system implementation
		for this method) return 0; Return nonzero if the plug-in implements
		this method.
		\par Default Implementation:
		<b>{ return 0; }</b> */
		virtual	int 			BeginCreate(Interface *i) {return 0;}
		
		/*! \remarks The termination of the custom creation process is managed by the
		implementation of this method. For example, the plug-in could remove a
		custom command mode from the command stack. See the Advanced Topics
		section on
		<a href="ms-its:3dsmaxsdk.chm::/nodes_object_creation_methods.html">Object
		Creation Methods</a> for more details.
		\param i An interface pointer the plug-in may use to call functions in 3ds Max.
		\return  To use the system implementation for this method return 0;
		Return nonzero if the plug-in implements this method.
		\par Default Implementation:
		<b>{ return 0; }</b> */
		virtual int 			EndCreate(Interface *i) {return 0;};
		
		/*! \remarks This method returns the name of the class. This name appears in the
		button for the plug-in in the 3ds Max user interface.
		\return  The name of the class. */
		virtual const MCHAR* 	ClassName()=0;
		
		/*! \remarks This method returns a system defined constant describing the class this
		plug-in class was derived from. For example, the Bend modifier returns
		<b>OSM_CLASS_ID</b>. This super class ID is used by all object space
		modifiers. See <a href="ms-its:listsandfunctions.chm::/idx_R_list_of_super_class_ids.html">List of
		SuperClassIDs</a>.
		\return  The SuperClassID of the plug-in. */
		virtual SClass_ID		SuperClassID()=0;
		
		/*! \remarks This method must return the <b>unique</b> ID for the object. If two
		ClassIDs conflict, the system will only load the first one it finds.
		The ClassID consists of two unsigned 32-bit quantities. The constructor
		assigns a value to each of these, for example <b>Class_ID(0xA1C8E1D1,
		0xE7AA2BE5)</b>. A developer should use the random Class_ID generator
		to avoid conflicts (Generate a random Class_ID). See
		Class Class_ID for more information.
		\return  The unique ClassID of the plug-in. */
		virtual Class_ID		ClassID()=0;
		/*! \remarks This methods returns a string describing the category a plug-in fits
		into. The category is usually selected in the drop down list in the
		create, or utility branch of the command panel. In the create branch,
		if this is set to be an existing category (i.e. "Standard Primitives",
		"Splines", ...) then the plug-in will appear in that category. If the
		category doesn't yet exists then it is created. If the plug-in does not
		need to appear in the list, it may simply return a null string as in
		<b>_M("")</b>.\n\n
		In the modify branch, the category determines which group it appears in
		the Configure Button Sets / Modifiers list. These are the categories
		such as "MAX STANDARD", "MAX EDIT", and "MAX SURFACE".\n\n
		This method is also used to distinguish between the various types of
		texture maps so they can be separated in the Material/Map Browser. The
		appropriate string should be returned by this method of the Texmap. For
		example:\n\n
		<b>const MCHAR* Category() { return TEXMAP_CAT_3D; }</b>\n\n
		The options for texture maps are:\n\n
		<b>MCHAR TEXMAP_CAT_2D[]; -</b> 2D maps.\n\n
		<b>MCHAR TEXMAP_CAT_3D[]; -</b> 3D maps.\n\n
		<b>MCHAR TEXMAP_CAT_COMP[]; -</b> Composite.\n\n
		<b>MCHAR TEXMAP_CAT_COLMOD[]; -</b> Color modifier.\n\n
		<b>MCHAR TEXMAP_CAT_ENV[]; -</b> Environment. */
		virtual const MCHAR* 	Category()=0;   // primitive/spline/loft/ etc
		/*! \remarks This method is used to enable or disable the button that allows the
		plug-ins class to be created. For example, at certain times it is not
		appropriate to for the Boolean object to be created. When there is not
		an object of the appropriate type selected the Boolean object cannot be
		created. At these times the button should be disabled (the button will
		appear as grayed out in the Create branch of the command panel). The
		button should be enabled if there is an object of the appropriate type
		selected. This method allows a plug-in to control the state of the
		button.
		\param i An interface pointer the plug-in may use to call functions in 3ds Max.
		\return  TRUE to enable the class creation button; FALSE to disable it.
		\par Default Implementation:
		<b>{ return TRUE; }</b>
		\par Sample Code:
		The following code from <b>/MAXSDK/SAMPLES/OBJECTS/BOOLOBJ.CPP</b>
		demonstrates an implementation of this method. If there is not a node
		selected, it is not OK to use the command so the button should appear
		disabled. To disable the button <b>OkToCreate()</b> returns FALSE. If
		the object that is selected is not of the appropriate type it the
		button is disabled as well.\n\n
		\code
		BOOL BoolObjClassDesc::OkToCreate(Interface *i)
		{
			if (i->GetSelNodeCount()!=1) return FALSE;
			ObjectState os = i->GetSelNode(0)->GetObjectRef()->Eval(i->GetTime());
			if (os.obj->SuperClassID()!=GEOMOBJECT_CLASS_ID) {
				return FALSE;
			}
			return TRUE;
		}
		\endcode */
		virtual BOOL			OkToCreate(Interface *i) { return TRUE; }	// return FALSE to disable create button		
		/*! \remarks If a plug-in class has default parameters that it needs to allow the
		user to edit, TRUE should be returned and <b>EditClassParams()</b> and
		<b>ResetClassParams()</b> should be implemented. Otherwise return FALSE
		(the default). See the description above.
		\par Default Implementation:
		<b>{return FALSE;}</b> */
		virtual BOOL			HasClassParams() {return FALSE;}
		
		/*! \remarks If the user picks the class from the list this method is called. The
		plug-in should put up a modal dialog that allows the user to edit the
		plug-ins default parameters. The plug-in should not return until the
		user has finished editing the parameters. See the description above.
		\param hParent The parent window handle. */
		virtual void			EditClassParams(HWND hParent) {}
		
		/*! \remarks When the user executes File / Reset or presses the 'Reset to Factory
		Settings...' button in the File / Preferences... / Animation tab /
		Controller Defaults section this method is called. The plug-in can
		respond by resetting itself to use its default values. See the
		description above.
		\param fileReset When TRUE, the user has performed a File / Reset operation. When FALSE,
		the user is in the Preferences... dialog doing a reset controller
		defaults operation. */
		virtual void			ResetClassParams(BOOL fileReset=FALSE) {}

				// These functions return keyboard action tables that plug-ins can use
				/*! \remarks This method is called at Dll-load time to get the number of action
				tables from a plug-in. Note: If more than one class uses the table only
				one of the classes should export the table, but they can all use them.
				See Class ActionTable.
				\par Default Implementation:
				<b>{ return 0; }</b> */
				virtual int             NumActionTables() { return 0; }
				/*! \remarks Returns a pointer to the 'i-th' action table. See Class ActionTable.
				\param i The zero based index of the table to return.
				\par Default Implementation:
				<b>{ return NULL; }</b> */
				virtual ActionTable*  GetActionTable(int i) { return NULL; }

				// Manipulator Interface

				// Returns true of the class implements a manipulator object.
				/*! \remarks This methods is what is used by the system to determine whether the
				ClassDesc describes a manipulator. Returns TRUE if the class implements
				a manipulator object; otherwise FALSE.
				\par Default Implementation:
				<b>{ return FALSE; }</b> */
				virtual BOOL IsManipulator() { return FALSE; }

				/*! \remarks				The method returns true if the class is a manipulator and it
				manipulates the given base object, modifier or controller. When
				starting "Manipulate" mode, this is called on selected nodes for the
				base object, all modifiers, the TM controller and the position,
				rotation and scale controllers, if the TM controller is a
				PRSController.
				\param hTarget A pointer to a reference target.
				\par Default Implementation:
				<b>{ return FALSE; }</b> */
				virtual BOOL CanManipulate(ReferenceTarget* hTarget) { return FALSE; }

				/*! \remarks Returns TRUE if the manipulator applies to the given node; otherwise FALSE.
				This method can be used to indicate that the manipulator works on a part
				of an object that is not covered by BOOL CanManipulate(ReferenceTarget* hTarget)
				such as the visibility controller of a node.
				\param pNode The node to check.
				\par Default Implementation:
				<b>{ return FALSE; }</b> */
				virtual BOOL CanManipulateNode(INode* pNode) { return FALSE; }

				/*! \brief Creates a manipulator object
				When a manipulator returns TRUE to CanManipulate(ReferenceTarget* hTarget), the system 
				calls this version of CreateManipulator() to create an instance of the manipulator.
				\param hTarget - The ReferenceTarget for which a manipulator is requested
				\param pNode - The node that the manipulator needs to manipulate (know about)
				\return - Pointer to the newly created manipulator, or NULL if the creation failed.
				\code
				Manipulator* BendManipClassDesc::CreateManipulator(ReferenceTarget* hTarget, INode* node) {
					if (hTarget->ClassID() != Class_ID(BENDOSM_CLASS_ID, 0))
						return NULL;
					return (new BendManip((SimpleMod2*)hTarget, node));
				}
				\endcode
				*/
				virtual Manipulator* CreateManipulator(
					ReferenceTarget* hTarget,
					INode* pNode) { return NULL; }

				/*! \brief Creates a manipulator object
				When a manipulator returns TRUE to CanManipulateNode(INode* pNode), the system 
				calls this version of CreateManipulator() to create an instance of the manipulator.
				\param pNode - The node that the manipulator needs to manipulate (know about)
				\return - Pointer to the newly created manipulator, or NULL if the creation failed.
				*/
				virtual Manipulator* CreateManipulator(INode* pNode) {return NULL;}

		// Class IO
		/*! \remarks Returns TRUE if there is data associated with the class that needs to
		be saved in the 3ds Max file. If this is so, implement the
		<b>Save()</b> and <b>Load()</b> methods below. If there is no class
		data to save return FALSE.
		\par Default Implementation:
		<b>{ return FALSE; }</b> */
		virtual BOOL			NeedsToSave() { return FALSE; }
		
		/*! \remarks If <b>NeedsToSave()</b> returns TRUE then this method should be
		implemented to save the data associated with the class.
		\param isave A pointer that may be used to call methods to save data to disk.
		\return  <b>IO_OK</b> if the save was successful; otherwise <b>IO_ERROR</b>. */
		virtual IOResult 		Save(ISave *isave) { return IO_OK; }
		
		/*! \remarks If <b>NeedsToSave()</b> returns TRUE then this method should be
		implemented to load the data associated with the class.
		\param iload A pointer that may be used to load data from a file.
		\return  <b>IO_OK</b> if the load was successful; otherwise
		<b>IO_ERROR</b>. */
		virtual IOResult 		Load(ILoad *iload) { return IO_OK; }

		// bits of dword set indicate corrresponding rollup page is closed.
		// the value 0x7fffffff is returned by the default implementation so the
		// command panel can detect this method is not being overridden, and just leave 
		// the rollups as is.
		/*! \remarks This method returns a DWORD which is used to initialize the rollup
		state in both the create branch and the modify branch. The semantics
		are different, however for these two cases. Whenever the rollups are
		created in the create branch, their state will be that specified by
		this method. In the modify branch, the first time an object of this
		type is modified the state will be that of this method, but after that
		it will remain what it was last set to.
		\return  The bits of this DWORD set indicate the corrresponding rollup
		page is closed. The zero bit corresponds to the plug-ins first rollup,
		the first bit is the second rollup, etc. The value <b>0x7fffffff</b> is
		returned by the default implementation so the command panel can detect
		this method is not being overridden, and just leave the rollups as is.
		\par Default Implementation:
		<b>{ return 0x7fffffff; }</b> */
		virtual DWORD			InitialRollupPageState() { return 0x7fffffff; }

		// ParamBlock2-related metadata interface, supplied & implemented in ClassDesc2 (see maxsdk\include\iparamb2.h)
		/*! \remarks Returns a string which provides a fixed, machine parsable internal name
		for the plug-in. This name is used by MAXScript.
		\par Default Implementation:
		<b>{ return NULL; }</b> */
		virtual const MCHAR*	InternalName() { return NULL; }
		
		/*! \remarks Returns the DLL instance handle of the plug-in. This is used so that
		string resources can be loaded by the ParamBlock2 system.
		\par Default Implementation:
		<b>{ return NULL; }</b> */
		virtual HINSTANCE		HInstance() { return NULL; }
		
		// access parameter block descriptors for this class
		/*! \remarks Implemented by the System.\n\n
		Returns the number or ParamBlockDesc2s used by the plug-in.
		\par Default Implementation:
		<b>{ return 0; }</b> */
		virtual int				NumParamBlockDescs() { return 0; }
		
		/*! \remarks Implemented by the System.\n\n
		Returns a pointer to the 'i-th' parameter block 2 descriptor.
		\param i The zero based index of the descriptor to return.
		\par Default Implementation:
		<b>{ return NULL; }</b> */
		virtual ParamBlockDesc2* GetParamBlockDesc(int i) { return NULL; }
		
		/*! \remarks Implemented by the System.\n\n
		Returns a pointer to the specified parameter block 2 descriptor.
		\param id The ID of the parameter block.
		\par Default Implementation:
		<b>{ return NULL; }</b> */
		virtual ParamBlockDesc2* GetParamBlockDescByID(BlockID id) { return NULL; }
		
		/*! \remarks Implemented by the System.\n\n
		Adds the specified parameter block 2 descriptor to the list of those
		maintained by the class.
		\param pbd Points to the parameter block 2 descriptor to add.
		\par Default Implementation:
		<b>{ }</b> */
		virtual void			AddParamBlockDesc(ParamBlockDesc2* pbd) { }
		
		// automatic UI management
		/*! \remarks  Implemented by the System.\n\n
		This method is called to handle the beginning of the automatic command
		panel user interface management provided by the param map 2 system.
		This method is called by the plug-in from its
		<b>Animatable::BeginEditParams()</b> method. The parameters passed to
		that method are simply passed along to this method.
		\param ip The interface pointer passed to the plug-in.
		\param obj Points to the plug-in class calling this method.
		\param flags The flags passed along to the plug-in in <b>Animatable::BeginEditParams()</b>.
		\param prev The pointer passed to the plug-in in <b>Animatable::BeginEditParams()</b>.
		\par Default Implementation:
		<b>{ }</b> */
		virtual void			BeginEditParams(IObjParam *ip, ReferenceMaker* obj, ULONG flags, Animatable *prev) { }
		/*! \remarks Implemented by the System.\n\n
		This method is called to handle the ending of the automatic command
		panel user interface management provided by the param map 2 system.
		This method is called by the plug-in from its
		<b>Animatable::EndEditParams()</b> method. The parameters passed to
		that method are simply passed along to this method.
		\param ip The interface pointer passed to the plug-in.
		\param obj Points to the plug-in class calling this method.
		\param flags The flags passed along to the plug-in in <b>Animatable::EndEditParams()</b>.
		\param prev The pointer passed to the plug-in in <b>Animatable::EndEditParams()</b>.
		\par Default Implementation:
		<b>{ }</b> */
		virtual void			EndEditParams(IObjParam *ip, ReferenceMaker* obj, ULONG flags, Animatable *prev) { }
		/*! \remarks Implemented by the System.\n\n
		Invalidates the user interface for the rollup or dialog managed by the
		specified descriptor. This will cause the controls in that rollup to be
		redrawn.
		\param pbd Points to the parameter block 2 descriptor whose corresponding UI is invalidated.
		\par Default Implementation:
		<b>{ }</b> */
		virtual void			InvalidateUI(ParamBlockDesc2* pbd) { }
		CoreExport virtual MCHAR* GetRsrcString(INT_PTR id);
		// automatic ParamBlock construction
		/*! \remarks Implemented by the System.\n\n
		This method creates the automatic parameter blocks for the specified
		plug-in. These are the ones with the <b>ParamBlockDesc2.flags
		P_AUTO_CONSTRUCT</b> bit set.
		\param owner Points to the owner of the parameter block.
		\par Default Implementation:
		<b>{ }</b> */
		virtual void			MakeAutoParamBlocks(ReferenceMaker* owner) { }
		// access automatically-maintained ParamMaps, by simple index or by associated ParamBlockDesc
		/*! \remarks Implemented by the System.\n\n
		Returns the number of parameter map2s used by the plug-in.
		\par Default Implementation:
		<b>{ return 0; }</b> */
		virtual int				NumParamMaps() { return 0; }
		/*! \remarks Implemented by the System.\n\n
		Returns a pointer to the 'i-th' parameter map2.
		\param i The zero based index of the parameter map2 to return.
		\par Default Implementation:
		<b>{ return NULL; }</b> */
		virtual IParamMap2*		GetParamMap(int i) { return NULL; }
		/*! \remarks Implemented by the System.\n\n
		Returns a pointer to the parameter map2 whose descriptor is passed.
		\param pbd Points to the parameter block2 descriptor.
		\par Default Implementation:
		<b>{ return NULL; }</b> */
		virtual IParamMap2*		GetParamMap(ParamBlockDesc2* pbd) { return NULL; }
		// maintain user dialog procs on automatically-maintained ParamMaps
		/*! \remarks Implemented by the System.\n\n
		Sets the parameter map 2 user dialog proc for the specified descriptor.
		\param pbd Points to the parameter block 2 descriptor.
		\param proc This object manages user interface control that require special
		processing. See Class ParamMap2UserDlgProc.
		\par Default Implementation:
		<b>{ }</b> */
		virtual void			SetUserDlgProc(ParamBlockDesc2* pbd, ParamMap2UserDlgProc* proc=NULL) { }
		/*! \remarks Implemented by the System.\n\n
		Returns a pointer to the parameter map 2 user dialog proc (if any) for
		the specified descriptor.
		\param pbd Points to the parameter block 2 descriptor.
		\return  See Class ParamMap2UserDlgProc.
		\par Default Implementation:
		<b>{ return NULL; }</b> */
		virtual ParamMap2UserDlgProc* GetUserDlgProc(ParamBlockDesc2* pbd) { return NULL; }

		// Class can draw an image to represent itself graphically...
		/*! \remarks This method allows this plug-in class to provide a custom image for
		display in Schematic %View.
		\param bkColor The background color. See <a href="ms-its:listsandfunctions.chm::/idx_R_colorref.html">COLORREF-DWORD format</a>.
		\param hDC The handle for the device context.
		\param rect The rectangle to draw in.
		\return  TRUE if this class can draw an image to represent itself
		graphically; otherwise FALSE.
		\par Default Implementation:
		<b>{ return FALSE; }</b> */
		virtual bool DrawRepresentation(COLORREF bkColor, HDC hDC, Rect &rect) { return FALSE; }

		// Function Publishing interfaces - jbw 3.13.00
		/*! \remarks Implemented by the System.\n\n
		Returns the number of function publishing interfaces maintained by the
		class descriptor. */
		virtual int				NumInterfaces() { return interfaces.Count(); }
		virtual FPInterface*	GetInterfaceAt(int i) { return interfaces[i]; }
		/*! \remarks Returns a pointer to the function publishing interface whose ID is specified.
		\param id The inteface ID. */
		CoreExport virtual FPInterface*	GetInterface(Interface_ID id);
		/*! \remarks Returns a pointer to the function publishing interface whose name is
		specified.
		\param name The name of the interface. */
		CoreExport virtual FPInterface*	GetInterface(MCHAR* name);
		/*! \remarks Adds the specified interface to the list maintained by this class
		descriptor.
		\param fpi Points to the interface to add. */
		CoreExport virtual void	AddInterface(FPInterface* fpi);
		/*! \remarks Deletes all the interfaces maintained by the class descriptor. */
		virtual void			ClearInterfaces() { interfaces.ZeroCount(); }
		
		// This method can be used for further categorizing plugin's. If a plugin has 
		// sub-plugins(like light > shadows, particles > operators), this method can be 
		// used to differentiate them. sub-plugins can be derived off reference target
		// but return a particular class ID published by the parent plugins SDK headers. 
		// Then parent plugin can get a list of all reference targets whose SubClassID 
		// matches the published SubClassID
		/*! \remarks This method can be used for further categorizing plugin's. If a plugin
		has sub-plugins(like light \> shadows, particles \> operators), this
		method can be used to differentiate them. sub-plugins can be derived
		off reference target but return a particular class ID published by the
		parent plugins SDK headers. Then parent plugin can get a list of all
		reference targets whose SubClassID matches the published SubClassID
		\par Default Implementation:
		<b>{ return Class_ID(); }</b> */
		virtual Class_ID		SubClassID() { return Class_ID(); }

		// Generic expansion function
		virtual INT_PTR Execute(int cmd, ULONG_PTR arg1=0, ULONG_PTR arg2=0, ULONG_PTR arg3=0) { return 0; } 
	};


// ClassDesc::Execute cmd values
#define I_EXEC_CTRL_BYPASS_TREE_VIEW 1001 // return 1 to hide your controller in track view

// arg1 is expected to be bool* The bool gets set to true or false 
// depending whether sub-obj anim is or not enabled for that object type.
// The Execute method returns TRUE if the arg1 is set successfully, otherwise FALSE
#define I_EXEC_GET_SOA_STATE 1002 

// arg1 is expected to be a TimeValue* The TimeValue gets set appropriately,
// depending on whether sub-obj animation is allowed of not for that object type.
// The Execute method returns TRUE if the arg1 is set successfully, otherwise FALSE
#define I_EXEC_EVAL_SOA_TIME 1003


// Create instance of the specified class
CoreExport void* CreateInstance(SClass_ID superID, Class_ID classID);

//! \brief CallParam data for NOTIFY_CLASSDESC_REPLACED broadcasts.
/*! A pointer to an instance of this structure is passed to BroadcastNotification after a DataClassDesc is 
replaced by a ClassDesc in a ClassDirectory. This occurs when the dll containing a deferred loaded plugin 
is loaded.
\see NOTIFY_CLASSDESC_REPLACED */
struct ClassDescReplaced: public MaxHeapOperators {  
	//! the old class descriptor 
	const ClassDesc* oldClassDesc;
	//! the new class descriptor
	const ClassDesc* newClassDesc;
}; 


#endif
