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
// FILE:        gfx.h
// DESCRIPTION: main graphics system include file.
// AUTHOR:      Don Brittain
// HISTORY:     
//**************************************************************************/

#if !defined(_GFX_H_)

#define _GFX_H_

#include "maxheap.h"
#include "geomlib.h"
#include "export.h"
#include "tab.h"
#include "mtl.h"
#include "BaseInterface.h"
#include "HWMesh.h"

typedef Tab<DWORD> DWTab;
typedef unsigned short MtlID;



class VertexBuffer: public MaxHeapOperators {
public:
	Point3	position;
	DWORD		color;
};

class LineBuffer: public MaxHeapOperators {
public:
	Point3	position;
	DWORD		color;
};

// strip-related stuff

class Strip: public MaxHeapOperators {
public:
	MtlID	mID;
	DWORD	smGrp;
	DWTab	v;
	DWTab	n;
	DWTab	tv;
	void AddVert(DWORD vtx)	{ v.Append(1, &vtx); }
	void AddVert(DWORD vtx, DWORD tvtx) { v.Append(1, &vtx); tv.Append(1, &tvtx); }
	void AddVertN(DWORD vtx, DWORD nor) { v.Append(1, &vtx); n.Append(1, &nor); }
	void AddVertN(DWORD vtx, DWORD tvtx, DWORD nor) { v.Append(1, &vtx); tv.Append(1, &tvtx); n.Append(1, &nor); }
};

typedef Strip *StripPtr;
typedef Tab<StripPtr> StripTab;


// Face Flags: (moved from mesh.h)
// 		3 LSBs hold the edge visibility flags
// 		Bit 3 indicates the presence of texture verticies

// if bit is 1, edge is visible
#define EDGE_VIS			1
#define EDGE_INVIS			0

// first edge-visibility bit field
#define VIS_BIT				0x0001
#define VIS_MASK			0x0007

#define EDGE_A		(1<<0)
#define EDGE_B		(1<<1)
#define EDGE_C		(1<<2)
#define EDGE_ALL	(EDGE_A|EDGE_B|EDGE_C)

#define FACE_HIDDEN	(1<<3)
#define HAS_TVERTS	(1<<4)	// DO NOT USE: This flag is obselete.
#define FACE_WORK	(1<<5) // used in various algorithms
#define FACE_STRIP	(1<<6)

// flags to indicate that face normal is used because no smoothing group
// normal is found
#define FACE_NORM_A	(1<<8)
#define FACE_NORM_B	(1<<9)
#define FACE_NORM_C	(1<<10)
#define FACE_NORM_MASK	0x0700

#define FACE_INFOREGROUND	(1<<11)  //watje this is used to track faces that are in the foreground we should not draw thes but they are hitested against
#define FACE_BACKFACING		(1<<12)  //watje this is used to track which faces are back faacing this is a temporary flag 


// The mat ID is stored in the HIWORD of the face flags
#define FACE_MATID_SHIFT	16
#define FACE_MATID_MASK		0xFFFF


class GWFace: public MaxHeapOperators {
public:
	DWORD	v[3];	// indexed references to the vertex array
	DWORD	flags;	// see face flags description above
};

/*! \defgroup patchDisplayFlags Patch Display Flags
For processWireFaces and general mesh class use */
//@{
#define DISP_VERTTICKS		(1<<0)	//!< Display vertices as tick marks
#define DISP_SELVERTS		(1<<10)	//!< Display selected vertices.
#define DISP_SELFACES		(1<<11)	//!< Display selected faces.
#define DISP_SELEDGES		(1<<12)	//!< Display selected edges.
#define DISP_SELPOLYS		(1<<13)	//!< Display selected polygons.
#define DISP_OBJSELECTED	(1<<8)	//!< Mimics COMP_OBJSELECTED in mesh.h
//@}


// General definitions

#define WM_SHUTDOWN			(WM_USER+2001)
#define WM_INIT_COMPLETE	(WM_USER+2002)

#define GW_MAX_FILE_LEN		128
#define GW_MAX_CAPTION_LEN	128

#define GW_MAX_VERTS		32
#define GFX_MAX_STRIP		100
#define GFX_MAX_TEXTURES	8

typedef BOOL	(*HitFunc)(int, int, void *);

// Rendering modes
#define GW_NO_ATTS				0x0000000
#define GW_WIREFRAME			0x0000001
#define GW_ILLUM				0x0000002
#define GW_FLAT					0x0000004
#define GW_SPECULAR				0x0000008
#define GW_TEXTURE				0x0000010
#define GW_Z_BUFFER				0x0000020
#define GW_PERSP_CORRECT		0x0000040
#define GW_POLY_EDGES			0x0000080
#define GW_BACKCULL				0x0000100
#define GW_TWO_SIDED			0x0000200
#define GW_COLOR_VERTS			0x0000400
#define GW_SHADE_CVERTS			0x0000800
#define GW_PICK					0x0001000
#define GW_BOX_MODE				0x0002000
#define GW_ALL_EDGES			0x0004000
#define GW_VERT_TICKS			0x0008000
#define GW_SHADE_SEL_FACES		0x0010000
#define GW_TRANSPARENCY			0x0020000
#define GW_TRANSPARENT_PASS		0x0040000
#define GW_EMISSIVE_VERTS		0x0080000
#define GW_ALL_OPAQUE			0x0100000
#define GW_EDGES_ONLY			0x0200000
#define GW_CONSTANT				0x0400000
#define GW_HIDDENLINE			0x0800000  //this is the same as constant but the shade color will the color of the background
#define GW_BLENDING				0x1000000
#define GW_DEPTHWRITE_DISABLE	0x2000000	//disable writing into the depth buffer


#define GW_LIGHTING			(GW_ILLUM | GW_SPECULAR)


// spotlight shapes
#define GW_SHAPE_RECT		0
#define GW_SHAPE_CIRCULAR	1

// texture tiling
#define GW_TEX_NO_TILING	0
#define GW_TEX_REPEAT		1
#define GW_TEX_MIRROR		2

// texture operations
#define GW_TEX_LEAVE				0	// Use the source pixel value
#define GW_TEX_REPLACE				1	// Use the texture pixel value
#define GW_TEX_MODULATE				2	// Multiply the source with the texture
#define GW_TEX_ADD					3	// Add the source and texture
#define GW_TEX_ADD_SIGNED			4	// Add the source and texture with an 0.5 subtraction
#define GW_TEX_SUBTRACT				5	// Subtract the source from the texture
#define GW_TEX_ADD_SMOOTH			6	// Add the source and the texture then subtract their product
#define GW_TEX_ALPHA_BLEND			7	// Alpha blend the texture with the source
#define GW_TEX_PREMULT_ALPHA_BLEND	8	// Alpha blend the the source with a premultiplied alpha texture
#define GW_TEX_ALPHA_BLEND2			9	// Alpha blend the the source with a premultiplied alpha texture with GL_MODULATE as the tex env instead of GL_DECAL.

// texture scale factors
#define GW_TEX_SCALE_1X		0	// Multiply the tex op result by 1
#define GW_TEX_SCALE_2X		1	// Multiply the tex op result by 2
#define GW_TEX_SCALE_4X		2	// Multiply the tex op result by 4

// texture alpha sources
#define GW_TEX_ZERO			0	// Use no alpha value
#define GW_TEX_SOURCE		1	// Use the source alpha
#define GW_TEX_TEXTURE		2	// Use the texture alpha
#define GW_TEX_CONSTANT		3	// Use a constant BGRA color as an alpha
#define GW_TEX_PREVIOUS		4	// Use the previous texture stage alpha

// View volume clip flags
#define GW_LEFT_PLANE		0x0100
#define GW_RIGHT_PLANE		0x0200
#define GW_BOTTOM_PLANE		0x0400
#define GW_TOP_PLANE		0x0800
#define GW_FRONT_PLANE		0x1000
#define GW_BACK_PLANE		0x2000
#define GW_PLANE_MASK		0x3f00

// edge styles
#define GW_EDGE_SKIP		0
#define GW_EDGE_VIS			1
#define GW_EDGE_INVIS		2

// buffer types (for dual-plane stuff)
#define BUF_F_BUFFER		0
#define BUF_Z_BUFFER		1

// support method return values
#define GW_DOES_SUPPORT			TRUE
#define GW_DOES_NOT_SUPPORT		FALSE

// support queries
#define GW_SPT_TXT_CORRECT		0	// allow persp correction to be toggled?
#define GW_SPT_GEOM_ACCEL		1	// do 3D xforms, clipping, lighting thru driver?
#define GW_SPT_TRI_STRIPS		2	// send down strips instead of individual triangles?
#define GW_SPT_DUAL_PLANES		3	// allow dual planes to be used?
#define GW_SPT_SWAP_MODEL		4	// update viewports with complete redraw on WM_PAINT?
#define GW_SPT_INCR_UPDATE		5	// redraw only damaged areas on object move?
#define GW_SPT_1_PASS_DECAL		6	// do decaling with only one pass?
#define GW_SPT_DRIVER_CONFIG	7	// allow driver config dialog box?
#define GW_SPT_TEXTURED_BKG		8	// is viewport background a texture?
#define GW_SPT_VIRTUAL_VPTS		9	// are viewports bigger than the window allowed?
#define GW_SPT_PAINT_DOES_BLIT	10	// does WM_PAINT cause a backbuffer blit?
#define GW_SPT_WIREFRAME_STRIPS	11	// if true, wireframe objects are sent as tristrips
#define GW_SPT_ORG_UPPER_LEFT	12	// true if device origin is at upper left, false o/w
#define GW_SPT_ARRAY_PROCESSING	13	// true if the driver can handle vertex array data
#define GW_SPT_NUM_LIGHTS		14	// number of lights supported
#define GW_SPT_NUM_TEXTURES		15	// number of multitexture stages supported
#define GW_SPT_WIRE_FACES		16	// support for wireframe faces with visibility flags
#define GW_SPT_TOTAL			17	// always the max number of spt queries

// display state of the graphics window
#define GW_DISPLAY_MAXIMIZED	1
#define GW_DISPLAY_WINDOWED		2
#define GW_DISPLAY_INVISIBLE	3

// multi-pass rendering
#define GW_PASS_ONE				0
#define GW_PASS_TWO				1

// light types
enum LightType { OMNI_LGT, SPOT_LGT, DIRECT_LGT, AMBIENT_LGT };

// Light attenuation types -- not fully implemented
#define GW_ATTEN_NONE		0x0000
#define GW_ATTEN_START		0x0001
#define GW_ATTEN_END		0x0002
#define GW_ATTEN_LINEAR		0x0010
#define GW_ATTEN_QUAD		0x0020

// General 3D light structure
/*! \sa  <a href="ms-its:3dsmaxsdk.chm::/start_data_types.html">Data
Types</a>.\n\n
\par Description:
This class describes the lights used in the interactive renderer. All methods
of this class are implemented by the system.  */
class Light : public BaseInterfaceServer {
public:
    /*! \remarks Class constructor. Sets the default values of the light.\n\n
    <b>type = OMNI_LGT;</b>\n\n
    <b>attenType = NO_ATTEN;</b>\n\n
    <b>atten = (float)0.0;</b>\n\n
    <b>intensity = (float)1.0;</b>\n\n
    <b>angle = (float)30.0;</b>\n\n
    <b>color[0] = (float)1.0;</b>\n\n
    <b>color[1] = (float)1.0;</b>\n\n
    <b>color[2] = (float)1.0;</b>
    \par Data Members:
    <b>LightType type;</b>\n\n
    One of the following values:\n\n
    <b>OMNI_LGT</b>\n\n
    Omni-directional.\n\n
    <b>SPOT_LGT</b>\n\n
    Spot light.\n\n
    <b>DIRECT_LGT</b>\n\n
    Directional light.\n\n
    <b>AMBIENT_LGT</b>\n\n
    Ambient light - global illumination.\n\n
    <b>Point3 color;</b>\n\n
    The color of the light. Individual values are from 0.0 to 1.0 with 1.0 as
    white.\n\n
    <b>Attenuation attenType;</b>\n\n
    Attenuation is not currently implemented. A developer should pass
    <b>NO_ATTEN</b>.\n\n
    <b>float atten;</b>\n\n
    Note: Attenuation is not currently implemented.\n\n
    Light attenuation factor.\n\n
    <b>float intensity;</b>\n\n
    Light multiplier factor.\n\n
    <b>float angle;</b>\n\n
    Angle of cone for spot and cone lights in degrees.\n\n
    <b>int shape;</b>\n\n
    One of the following values:\n\n
    <b>GW_SHAPE_RECT</b> - Rectangular spotlights.\n\n
    <b>GW_SHAPE_CIRCULAR</b> - Circular spotlights.\n\n
    <b>float aspect;</b>\n\n
    The aspect ratio of the light.\n\n
    <b>int overshoot;</b>\n\n
    Nonzero indicates the light supports overshoot; otherwise 0.\n\n
    <b>BOOL affectDiffuse;</b>\n\n
    This data member is available in release 2.0 and later only.\n\n
    This defaults to TRUE, but if the user set it to FALSE in the light
    modifier panel, then the\n\n
    light is not supposed to illuminate the diffuse component of an object's
    material.\n\n
    <b>BOOL affectSpecular;</b>\n\n
    This data member is available in release 2.0 and later only.\n\n
    This defaults to TRUE, but if the user set it to FALSE in the light
    modifier panel, then the\n\n
    light is not supposed to illuminate the specular component of an object's
    material. */
    DllExport Light();
    LightType		type;
    Point3			color;
    int				attenType;
    float			attenStart;
	float			attenEnd;
    float			intensity;
    float			hotSpotAngle;
	float			fallOffAngle;
	int				shape;
	float			aspect;
	int				overshoot;
	BOOL 			affectDiffuse;
	BOOL 			affectSpecular;
};

enum CameraType { PERSP_CAM, ORTHO_CAM };

// General camera structure
class Camera : public BaseInterfaceServer {
public:
	DllExport Camera();
	void			setPersp(float f, float asp)
						{ type = PERSP_CAM; persp.fov = f; 
						  persp.aspect = asp; makeMatrix(); }
	void			setOrtho(float l, float t, float r, float b)
						{ type = ORTHO_CAM; ortho.left = l; ortho.top = t; 
						  ortho.right = r; ortho.bottom = b; makeMatrix(); }
	void			setClip(float h, float y) 
						{ hither = h; yon = y; makeMatrix(); }
	CameraType		getType(void)	{ return type; }
	float			getHither(void) { return hither; }
	float			getYon(void)	{ return yon; }
	DllExport void	reset();
	DllExport void	getProj(float mat[4][4]);
private:
	DllExport void	makeMatrix();
	float			proj[4][4];
	CameraType		type;
	union {
	    struct : public MaxHeapOperators {
            float	fov;
            float	aspect;
		} persp;
		struct : public MaxHeapOperators {
		    float	left;
		    float	right;
		    float	bottom;
		    float	top;
		} ortho;
	};
	float			hither;
	float			yon;
};

const double pi        = 3.141592653589793;
const double piOver180 = 3.141592653589793 / 180.0;

// Color types (used by setColor)
enum ColorType { LINE_COLOR, FILL_COLOR, TEXT_COLOR, CLEAR_COLOR };

// Marker types
enum MarkerType  { POINT_MRKR, HOLLOW_BOX_MRKR, PLUS_SIGN_MRKR, 
						   ASTERISK_MRKR, X_MRKR, BIG_BOX_MRKR, 
						   CIRCLE_MRKR, TRIANGLE_MRKR, DIAMOND_MRKR,
						   SM_HOLLOW_BOX_MRKR, SM_CIRCLE_MRKR, 
						   SM_TRIANGLE_MRKR, SM_DIAMOND_MRKR,
						   DOT_MRKR, SM_DOT_MRKR,
						   BOX2_MRKR, BOX3_MRKR, BOX4_MRKR, BOX5_MRKR,
						   BOX6_MRKR, BOX7_MRKR,
						   DOT2_MRKR, DOT3_MRKR, DOT4_MRKR, DOT5_MRKR,
						   DOT6_MRKR, DOT7_MRKR
};


#define AC_DIR_RL_CROSS		0	// right->left => crossing (AutoCAD compatible)
#define AC_DIR_LR_CROSS		1	// left->right => crossing (ACAD incompatible)

DllExport void	setAutoCross(int onOff);
DllExport int	getAutoCross();
DllExport void	setAutoCrossDir(int dir);
DllExport int	getAutoCrossDir();

// Region types (for built-in hit-testing)
#define POINT_RGN	0x0001
#define	RECT_RGN	0x0002
#define CIRCLE_RGN	0x0004
#define FENCE_RGN	0x0008

// region directions (left or right)
#define RGN_DIR_UNDEF	-1
#define RGN_DIR_RIGHT	0	
#define RGN_DIR_LEFT	1

struct CIRCLE: public MaxHeapOperators
{
    LONG  x;
    LONG  y;
	LONG  r;
};

/*! \sa  <a href="ms-its:3dsmaxsdk.chm::/start_data_types.html">Data
Types</a>.\n\n
\par Description:
This class describes the properties of a region used for built-in hit testing
of items in the interactive renderer.
\par Data Members:
<b>int type;</b>\n\n
The region type. One of the following values:\n\n
<b>POINT_RGN</b>\n\n
A single point.\n\n
<b>RECT_RGN</b>\n\n
A rectangular region.\n\n
<b>CIRCLE_RGN</b>\n\n
A circular region.\n\n
<b>FENCE_RGN</b>\n\n
An arbitrary multi-point polygon region.\n\n
<b>int crossing;</b>\n\n
If nonzero, elements that are contained within <b>or cross</b> the region
boundary are hit. If zero, only those elements <b>entirely within</b> the
boundary are hit. This is not used for point hit testing.\n\n
<b>int epsilon;</b>\n\n
Specifies the distance in pixels outside the pick point within which elements
may be and still be hit. This is not used for rect or circle testing, is sometimes used with fence hit
testing, where it doubles the size of the region, especially when selecting subobject edges or vertices
and is always used for point hit testing.\n\n
<b>union {</b>\n\n
<b>POINT pt;</b>\n\n
<b>RECT rect;</b>\n\n
<b>CIRCLE circle;</b>\n\n
<b>POINT *pts;</b>\n\n
<b>};</b>\n\n
The storage for the region. */
class HitRegion: public MaxHeapOperators {
	DWORD size;
public:
	int				type;
	int				dir;		// region direction
	int				crossing;	// not used for point
	int				epsilon;	// not used for rect or circle
	union {
		POINT		pt;
		RECT		rect;
		CIRCLE		circle;
		POINT *		pts;
	};
	HitRegion()		{ dir = RGN_DIR_UNDEF; size = sizeof(HitRegion);}
};

inline int ABS(const int x) { return (x > 0) ? x : -x; }

typedef void (*GFX_ESCAPE_FN)(void *);


// driver types for getDriver() method
#define GW_DRV_RENDERER		0
#define GW_DRV_DEVICE		1

// for possible future implementation
#define GW_HEIDI			0
#define GW_OPENGL			1
#define GW_DIRECT3D			2
#define GW_HEIDI3D			3
#define GW_NULL				4
#define GW_CUSTOM			5

// graphics window setup structure
/*! \sa  Class GraphicsWindow.\n\n
\par Description:
This class is the graphics window setup structure. An instance of this class is
passed to the function <b>createGW()</b> to create a new graphics window.\n\n
Note: This is no longer available for use by plug-ins in 3ds Max 2.0 and later.
*/
class GWinSetup: public MaxHeapOperators {
public:
    DllExport GWinSetup();
    MCHAR		caption[GW_MAX_CAPTION_LEN];
	MCHAR		renderer[GW_MAX_FILE_LEN];
	MCHAR		device[GW_MAX_FILE_LEN];
	DWORD		winStyle;
	POINT		size;
	POINT		place;
	INT_PTR			id;
				// WIN64 Cleanup: Shuler
	int			type;
	bool		quietMode;
};

// abstract graphics window class
/*! \sa  Class InteractiveRenderer for Graphics Window, 
Class GWinSetup, Class HitRegion, <a href="ms-its:listsandfunctions.chm::/idx_R_list_of_marker_types.html">List of Marker Types</a>,
<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_rendering_limits.html">List of Rendering Limits</a>,
Class Point3, Class IPoint3, Class Matrix3, Class Interface, Class ViewExp.\n\n
\par Description:
The abstract graphics window class. The methods here provide low-level access
to 3ds Max's graphics system. These methods are available for plug-ins to do
any graphics work not possible using the standard high-level graphics methods
of 3ds Max.\n\n
These methods are for use in the existing 3ds Max viewports. Note that these
APIs are not for casual use, as they are not intended to be a high level
graphics library. For example, many steps are required to display a single lit
polygon. These APIs are optimized for speed, and not at all for plug-in
programmer ease of use.\n\n
These methods are provided, however, so that developers can do things that are
otherwise impossible using the high-level methods.\n\n
<b>Developers should use these methods with an understanding of exactly what
they are doing since it's quite easy to crash 3ds Max when inappropriate
arguments are supplied. The calls are specifically optimized for exactly the
way 3ds Max uses them. In no way should the calls in GraphicsWindow be
considered an "ordinary" 2D/3D API. (That's what OpenGL, D3D, and HEIDI are
for.)</b>\n\n
One final note of warning: most graphics windows methods execute in a separate
thread (or in multiple separate threads) that are owned by the graphics window.
Thus, due to thread scheduling, when a bad argument or incorrect sequencing of
graphics windows calls causes 3ds Max to crash, it is not at all easy to figure
out where the problem is. In particular, the location of the main 3ds Max
thread is not relevant.\n\n
All the methods of this class are implemented by the system.
\par Method Groups:
See <a href="class_graphics_window_groups.html">Method Groups for Class GraphicsWindow</a>.
*/
class GraphicsWindow : public InterfaceServer {
public:
	virtual	~GraphicsWindow() {}
	/*! \remarks	This is called after all four GraphicsWindows used by 3ds Max are created.
	SDK users shouldn't need this call */
	virtual void	postCreate(int ct, GraphicsWindow **gw) = 0;
	/*! \remarks	This is used to tell the driver that it is shutting down. */
	virtual void	shutdown() = 0;
	/*! \remarks	This returns "0x200" to indicate R2.0 */
	virtual int		getVersion() = 0;
	/*! \remarks	This identifies the driver (and includes manufacturer info if available) */
	virtual MCHAR * getDriverString(void) = 0;
	/*! \remarks	This is called to put up the config dialog if the driver supports
	GW_SPT_CAN_CONFIG
	\par Parameters:
	<b>HWND hWnd</b>\n\n
	The parent window handle for the dialog. */
	virtual void	config(HWND hWnd) = 0;
	/*! \remarks Determines if the driver supports the specified feature.
	\par Parameters:
	<b>int what</b>\n\n
	One of the following values:\n\n
	<b>GW_SPT_TXT_CORRECT</b>\n\n
	This is used to enable or gray-out the perspective correction right-click
	viewport menu option.\n\n
	<b>GW_SPT_GEOM_ACCEL</b>\n\n
	This is used to indicate to 3ds Max (and the mesh class in particular) that
	the driver wants to handle all of the 3D data natively. In this case,
	meshes are rendered by passing 3D world space data and letting the driver
	transform, clip, and light the vertices. If this returns FALSE, then the
	mesh class handles all xforms/clip/lighting calculations (using a lazy
	evaluation algorithm) and then calls the hPolygon or hPolyline 2 1/2D calls
	for the driver to rasterize. (Primitives that are actually clipped are
	still sent to the polygon/polyline methods.)\n\n
	Right now, only the OpenGL driver returns TRUE to this query, but other
	drivers have been developed that return TRUE, and the HEIDI and D3D drivers
	may change in the future.\n\n
	<b>GW_SPT_TRI_STRIPS</b>\n\n
	If this returns TRUE, then 3ds Max will try to stripify meshes before
	calling the rendering methods. Right now, the drivers just return the user
	preference that is set in the driver config dialog. It defaults to
	TRUE.\n\n
	<b>GW_SPT_DUAL_PLANES</b>\n\n
	If a driver has dual-planes support it returns TRUE. Our OpenGL display
	driver only returns TRUE for this if the underlying display driver has
	implemented a custom OpenGL extension that allows us to handle this
	efficiently.\n\n
	<b>GW_SPT_SWAP_MODEL</b>\n\n
	This returns TRUE if 3ds Max has to redraw the whole scene any time the
	viewports are exposed.\n\n
	<b>GW_SPT_INCR_UPDATE</b>\n\n
	This returns TRUE if the driver can update a rectangular subset of the
	viewport without trashing the image outside that rectangle. This is TRUE
	for most drivers that blit the viewport region and FALSE for those that do
	page-flipping in the hardware. For OpenGL, this is TRUE if the display
	driver implements the Microsoft glSwapRectHintWIN extension.\n\n
	<b>GW_SPT_1_PASS_DECAL</b>\n\n
	This is TRUE if the driver can handle decalling with only one pass. Right
	now, this is TRUE for OpenGL, but FALSE for HEIDI and D3D. (And as with all
	of these options, it may change in a given driver anytime in the
	future.)\n\n
	<b>GW_SPT_DRIVER_CONFIG</b>\n\n
	This is TRUE if the driver has a configuration dialog box. This is TRUE for
	all three of our standard drivers.\n\n
	<b>GW_SPT_TEXTURED_BKG</b>\n\n
	This is TRUE if the viewport background is implemented as a textured
	rectangle, and FALSE if it is a blitted bitmap.\n\n
	<b>GW_SPT_VIRTUAL_VPTS</b>\n\n
	This is TRUE if the driver allows viewports to be made larger than the
	physical window they are attached to. Right now this is ony TRUE for
	OGL.\n\n
	<b>GW_SPT_PAINT_DOES_BLIT</b>\n\n
	This is TRUE if WM_PAINT messages result in a blit of the backbuffer (as
	opposed to a page-flipping swap). This allows 3ds Max to do quick damage
	region repair, and works together with the GW_SPT_SWAP_MODEL flag.\n\n
	<b>GW_SPT_WIREFRAME_STRIPS</b>\n\n
	This is TRUE if the driver wants 3ds Max to send down wireframe models
	using triangle strips instead of a bundle of 2-pt segments. This is only
	used by the OGL driver, and it is there as a user-choosable
	performance-accuracy tradeoff (since the strips are faster and are
	back-culled, but they displayhidden edges as though they are visible). */
	virtual int		querySupport(int what) = 0;

	/*! \remarks	This returns the "output" window handle. (Input goes to an invisible window
	above the viewport. The invisible window is owned by 3ds Max.) */
	virtual HWND	getHWnd(void) = 0;
	/*! \remarks Sets the size and position of the GraphicsWindow. The
	coordinates are all Windows coordinates in the space of the
	GraphicsWindows' parent window. (The origin is the upper left.)
	\par Parameters:
	<b>int x</b>\n\n
	Window x origin.\n\n
	<b>int y</b>\n\n
	Window y origin.\n\n
	<b>int w</b>\n\n
	Window width.\n\n
	<b>int h</b>\n\n
	Window height. */
	virtual void	setPos(int x, int y, int w, int h) = 0;
	/*! \remarks	The specified value may be sent to the driver to indicate the display state
	of the viewport window controlled by the driver.
	\par Parameters:
	<b>int s</b>\n\n
	The display state to set. One of the following values:\n\n
	<b>GW_DISPLAY_MAXIMIZED</b>\n\n
	<b>GW_DISPLAY_WINDOWED</b>\n\n
	<b>GW_DISPLAY_INVISIBLE</b> */
	virtual void	setDisplayState(int s) = 0;
	/*! \remarks	This method returns the current state. One of the following values:\n\n
	<b>GW_DISPLAY_MAXIMIZED</b>\n\n
	<b>GW_DISPLAY_WINDOWED</b>\n\n
	<b>GW_DISPLAY_INVISIBLE</b> */
	virtual int		getDisplayState() = 0;
	/*! \remarks	This method gets the current window size in X. */
	virtual int		getWinSizeX() = 0;
	/*! \remarks	This method gets the current window size in Y. */
	virtual int		getWinSizeY() = 0;
	/*! \remarks	This method returns the z-buffer depth (in bits) */
	virtual DWORD	getWinDepth(void) = 0;
	/*! \remarks	This method returns the largest device Z value. */
	virtual DWORD	getHitherCoord(void) = 0;
	/*! \remarks	This method returns the smallest device Z value. */
	virtual DWORD	getYonCoord(void) = 0;
	/*! \remarks	This method returns the size (in pixels) that the specified text string
	will occupy
	\par Parameters:
	<b>MCHAR *text</b>\n\n
	The string to check.\n\n
	<b>SIZE *sp</b>\n\n
	The size is returned here. See
	<a href="ms-its:3dsmaxsdk.chm::/start_data_types.html">Data Types</a>. */
	virtual void	getTextExtents(MCHAR *text, SIZE *sp) = 0;
	/*! \remarks	This method returns the largest number of triangles that can be in a strip
	*/
	virtual int		getMaxStripLength() { return GFX_MAX_STRIP; }
	virtual void	setFlags(DWORD f) = 0;
	virtual DWORD	getFlags() = 0;

	/*! \remarks This method resets the update rectangle. The update rectangle
	is the region of the screen that needs to be updated to reflect items that
	have changed. When the system is done rendering items, the goal is to only
	update the region that has actually been altered. This method sets the
	update rectangle (the region that will be blitted to the display) to
	invalid. In this way when <b>enlargeUpdateRect()</b> is later called, the
	RECT passed will be used as the region. */
	virtual void	resetUpdateRect() = 0;
	/*! \remarks This method enlarges the update rectangle to include the RECT
	passed. If <b>rp</b> is NULL, then the whole window will later be updated.
	\par Parameters:
	<b>RECT *rp</b>\n\n
	Pointer to a rectangle (or NULL). */
	virtual void	enlargeUpdateRect(RECT *rp) = 0;
	/*! \remarks This method retrieves the current update rectangle.
	\par Parameters:
	<b>RECT *rp</b>\n\n
	The current update rectangle.
	\return  Zero if the update rectangle is invalid; otherwise nonzero. */
	virtual int		getUpdateRect(RECT *rp) = 0;
    /*! \remarks This method is used internally and should not be called by
    plug-in developers. */
    virtual void	updateScreen() = 0;

	/*! \remarks This method is used internally. Most drivers control two
	image buffers. One is displayed on the screen, and the other is used to
	rasterize geometric primitives. When rasterization of a complete frame is
	done, the off-screen buffer is blitted onto the display screen. This is
	referred to as dual-plane mode. This method will turn dual-plane mode on or
	off. This is used internally by the File/Preferences... Viewport page Use
	Dual Planes toggle.
	\par Parameters:
	<b>int which</b>\n\n
	Specifies which buffer should use dual-planes.\n\n
	<b>BUF_F_BUFFER</b>\n\n
	The image (Framebuffer) buffer.\n\n
	<b>BUF_Z_BUFFER</b>\n\n
	The Z buffer.\n\n
	<b>int b</b>\n\n
	Nonzero to enable access (toggle on); 0 to toggle off.
	\return  TRUE if the graphics window has access to the specified buffer;
	otherwise FALSE. */
	virtual BOOL	setBufAccess(int which, int b) = 0;
	/*! \remarks This method is used internally. It returns a boolean value
	indicating if dual plane mode is on or off for the specified buffer.
	\par Parameters:
	The buffer whose dual-planes setting will be returned.\n\n
	<b>int which</b>\n\n
	The buffer whose dual-planes setting will be returned. One of the following
	values:\n\n
	<b>BUF_F_BUFFER</b>\n\n
	The Framebuffer.\n\n
	<b>BUF_Z_BUFFER</b>\n\n
	The Z buffer.
	\return  TRUE if the dual-plane mode is on; otherwise FALSE. */
	virtual BOOL	getBufAccess(int which) = 0;
	/*! \remarks This method is used internally. It retrieves the size of the
	specified buffer in bytes.
	\par Parameters:
	<b>int which</b>\n\n
	One of the following values:\n\n
	<b>int *size</b>\n\n
	The size of the buffer in bytes.\n\n
	Note the following concerning the HEIDI driver. For HEIDI getBufSize()
	always returns 10 if dual-planes are on (and 0 otherwise). This is because
	HEIDI actually never returns the image - it keeps its own copy stored away.
	Thus the "logical" way to think is that we actually get a copy of the
	buffer by calling getBuf, and that we give it back by calling setBuf. But
	in reality (with the HEIDI driver) getBuf and setBuf only tell HEIDI to do
	some internal buffer manipulation.
	\return  TRUE if the size was returned; otherwise FALSE. */
	virtual BOOL	getBufSize(int which, int *size) = 0;
	/*! \remarks This method is used internally. It retrieves the specified
	buffer.
	\par Parameters:
	<b>int which</b>\n\n
	The buffer to retrieve. One of the following values:\n\n
	<b>BUF_F_BUFFER -</b> The image Framebuffer.\n\n
	<b>BUF_Z_BUFFER -</b> The Z buffer.\n\n
	<b>int size</b>\n\n
	The number of bytes to retrieve. This must be at least the size returned
	from <b>getBufSize()</b>.\n\n
	<b>void *buf</b>\n\n
	Storage for the buffer data.
	\return  TRUE if the buffer was retrieved; otherwise FALSE. */
	virtual BOOL	getBuf(int which, int size, void *buf) = 0;
	/*! \remarks Stores the specified buffer.
	\par Parameters:
	<b>int which</b>\n\n
	The buffer to store. One of the following values:\n\n
	<b>BUF_F_BUFFER -</b> The image Framebuffer.\n\n
	<b>BUF_Z_BUFFER -</b> The Z buffer.\n\n
	<b>int size</b>\n\n
	The number of bytes to store.\n\n
	<b>void *buf</b>\n\n
	The buffer data.\n\n
	<b>RECT *rp</b>\n\n
	This allows only a subset of the saved image rect to be blitted back to the
	screen.
	\return  TRUE if the buffer was stored; otherwise FALSE. */
	virtual BOOL	setBuf(int which, int size, void *buf, RECT *rp) = 0;
	/*! \remarks This method returns the viewport image of this graphics
	window in a packed DIB format. A packed DIB is the standard BMI header
	followed immediately by all the data bytes (pixels) that make up the image.
	This is the standard way in Windows to pass a DIB around. See the sample
	code below for an example of this call in use. Note how it is called twice:
	once to get the size, once to get the DIB.
	\par Parameters:
	<b>BITMAPINFO *bmi</b>\n\n
	The BITMAPINFO structure defines the dimensions and color information for a
	Windows device-independent bitmap (DIB). Note that if this parameter is
	NULL, then only the size value is returned.\n\n
	<b>int *size</b>\n\n
	The size of the image in bytes.
	\return  TRUE if the image was returned; otherwise FALSE.
	\par Sample Code:
	The following sample code saves the current 3ds Max viewport to a user
	specified file.\n\n
\code	
	void TestGetDIB(IObjParam *ip)
	{
	BITMAPINFO *bmi = NULL;
	BITMAPINFOHEADER *bmih;
	BitmapInfo biFile;
	Bitmap *map;
	int size;
	TheManager->SelectFileOutput(\&biFile,
	ip->GetMAXHWnd(), _M("Testing"));
	if(!biFile.Name()[0])
	return;
	ViewExp *vpt = ip->GetActiveViewport();
	vpt->getGW()->getDIB(NULL, \&size);
	bmi = (BITMAPINFO *)malloc(size);
	bmih = (BITMAPINFOHEADER *)bmi;
	vpt->getGW()->getDIB(bmi, \&size);
	biFile.SetWidth((WORD)bmih->biWidth);
	biFile.SetHeight((WORD)bmih->biHeight);
	biFile.SetType(BMM_TRUE_32);
	map = TheManager->Create(\&biFile);
	map->OpenOutput(\&biFile);
	map->FromDib(bmi);
	map->Write(\&biFile);
	map->Close(\&biFile);
	if(bmi)
	free(bmi);
	ip->ReleaseViewport(vpt);
	} 
\endcode */
	virtual BOOL	getDIB(BITMAPINFO *bmi, int *size) = 0;
	/*! \remarks	This method is used internally to zoom the viewport. */
	virtual BOOL	setBackgroundDIB(int width, int height, BITMAPINFO *bmi) = 0;
	/*! \remarks	This method is used internally to pan the viewport. */
	virtual void	setBackgroundOffset(int x, int y) = 0;
	virtual int		useClosestTextureSize(int bkg=FALSE) = 0;
	/*! \remarks	This method returns the size of the texture bitmap that the driver wants
	sent in to the <b>getTextureHandle()</b> call (if bkg is FALSE). If bkg is
	TRUE, this returns the size of the texture that 3ds Max shoud send to the
	<b>setBackgroundDIB()</b> call. In general, the return value needs to be a
	power of 2, though that could be driver-specific
	\par Parameters:
	<b>int bkg=FALSE</b>\n\n
	TRUE to get the size for <b>setBackgroundDIB()</b>; FALSE to get the size
	for <b>getTextureHandle()</b>. */
	virtual int		getTextureSize(int bkg=FALSE) = 0;
	/*! \remarks This method returns a handle for the specified texture bitmap.
	This handle is then used with the <b>setTextureByHandle()</b> method (there is
	only one current texture active at any time). The texture dimensions must be a
	power of 2.\n\n
	When a material is on an object, and the material has a texture map, and when
	the button used to display the texture map is pressed, 3ds Max calls this
	method to get the texture handle. This basically loads the texture into the
	hardware RAM (if available). When this mapped object gets displayed the method
	<b>setTextureHanel()</b> is called. When the material is deleted, or the
	dispaly in viewport button is turned off, <b>freeTextureHandle()</b> is called.
	\par Parameters:
	<b>BITMAPINFO *bmi</b>\n\n
	The DIB image to use as a texture.
	\return  The texture handle. */
	virtual DWORD_PTR	getTextureHandle(BITMAPINFO *bmi) = 0;
	/*! \remarks When you are finished with the texture handle, call this method
	to free it.
	\par Parameters:
	<b>DWORD_PTR handle</b>\n\n
	The texture handle to free.
	\return  TRUE if the texture was set; otherwise FALSE. */
	virtual void	freeTextureHandle(DWORD_PTR handle) = 0;
	/*! \remarks This sets the current texture to the image whose handle is passed
	(see <b>getTextureHandle()</b>). The texture dimensions must be a power of 2.
	\par Parameters:
	<b>DWORD_PTR handle</b>\n\n
	The handle of the texture to make current. */
	virtual BOOL	setTextureByHandle(DWORD_PTR handle, int texStage=0) = 0;
				// WIN64 Cleanup: Shuler
	virtual void	setTextureColorOp(int texStage=0, int texOp=GW_TEX_MODULATE, int texAlphaSource=GW_TEX_TEXTURE, int texScale=GW_TEX_SCALE_1X) = 0;
	virtual void	setTextureAlphaOp(int texStage=0, int texOp=GW_TEX_MODULATE, int texAlphaSource=GW_TEX_TEXTURE, int texScale=GW_TEX_SCALE_1X) = 0;
	/*! \remarks Sets the way in which textures are tiled across the surface of
	the object.
	\par Parameters:
	The following parameters may use one of these values:\n\n
	<b>GW_TEX_NO_TILING</b>\n\n
	The texture clamped - Any UVW that is bigger than 1 is interpreted as being
	1.\n\n
	<b>GW_TEX_REPEAT</b>\n\n
	As the UVW numbers keep getting larger than 1 the image is repeated.\n\n
	<b>GW_TEX_MIRROR</b>\n\n
	If UVW goes beyond 1, the numbers are interpreted as going backwards. So if you
	had 0 to 2 it would actually go 0 to 1 then 1 down to 0.\n\n
	<b>int u</b>\n\n
	The type of texturing in the U direction.\n\n
	<b>int v</b>\n\n
	The type of texturing in the V direction.\n\n
	<b>int w=GW_TEX_NO_TILING</b>\n\n
	The type of texturing in the W direction.
	\return  TRUE if the tiling mode was set; otherwise FALSE. */
	virtual BOOL	setTextureTiling(int u, int v, int w=GW_TEX_NO_TILING, int texStage=0) = 0;
	/*! \remarks Returns the type of texture tiling set for the particular
	direction.\n\n
	For example, if <b>setTextureTiling(GW_TEX_NO_TILING, GW_TEX_REPEAT,
	GW_TEX_MIRROR)</b> were called first, then\n\n
	<b>getTextureTiling(0)</b> would yield <b>GW_TEX_NO_TILING</b>, and\n\n
	<b>getTextureTiling(1)</b> would yield <b>GW_TEX_REPEAT</b>.
	\par Parameters:
	<b>int which</b>\n\n
	This value is 0 or 1 and it represents the U or V direction respectively. The
	value 2 is not yet implemented.
	\return  <b>GW_TEX_NO_TILING</b>\n\n
	The texture clamped - Any UVW that is bigger than 1 is interpreted as being
	1.\n\n
	<b>GW_TEX_REPEAT</b>\n\n
	As the UVW numbers keep getting larger than 1 the image is repeated.\n\n
	<b>GW_TEX_MIRROR</b>\n\n
	If UVW goes beyond 1, the numbers are interpreted as going backwards. So if you
	had 0 to 2 it would actually go 0 to 1 then 1 down to 0. */
	virtual int		getTextureTiling(int which, int texStage=0) = 0;

	/*! \remarks If a developer is working with an existing 3ds Max instance
	of GraphicsWindow (one of 3ds Max's viewports) this method should NOT be
	called. */
	virtual void	beginFrame() = 0;
	/*! \remarks As above, if a developer is working with an existing 3ds Max
	instance of GraphicsWindow (one of 3ds Max's viewports) this method should
	NOT be called. */
	virtual void	endFrame() = 0;
	/*! \remarks This method sets the clipping boundaries within a viewport
	within the graphics window. This allows more than one viewport to appear
	within a single graphics window. It has the side-effect of building a 4x4
	viewport matrix. This routine should be called anytime the graphics window
	is resized, or else rendering will still occur to the old window size. (And
	since most drivers do not do range-checking since it is too time-costly,
	this could cause a system crash.)
	\par Parameters:
	Note: all coordinates are in Windows format, with the origin in the upper
	left\n\n
	<b>int x</b>\n\n
	Specifies the left viewport origin.\n\n
	<b>int y</b>\n\n
	Specifies the top viewport origin.\n\n
	<b>int w</b>\n\n
	Specifies the viewport width.\n\n
	<b>int h</b>\n\n
	Specifies the viewport height. */
	virtual void	setViewport(int x, int y, int w, int h) = 0;
	/*! \remarks	This method is used to setup a virtual viewport. Note that this is a no-op
	unless <b>GW_SPT_VIRTUAL_VPTS</b> is TRUE. Plug-in developers should not
	call this method -- it's for internal use only. */
	virtual void	setVirtualViewportParams(float zoom, float xOffset, float yOffset) = 0;
	/*! \remarks	This method is used to set a virtual viewport as active. Note that this is
	a no-op unless <b>GW_SPT_VIRTUAL_VPTS</b> is TRUE. Plug-in developers
	should not call this method -- it's for internal use only.
	\par Parameters:
	<b>int onOff</b>\n\n
	TRUE to set the virtual viewport active; FALSE to make it inactive. */
	virtual void	setUseVirtualViewport(int onOff) = 0;
    /*! \remarks Clears the specified rectangular region of the screen.
    \par Parameters:
    <b>RECT *rp</b>\n\n
    Specifies the rectangular region to clear.\n\n
    <b>int useBkg = FALSE</b>\n\n
    Specifies if the background should be used to fill the cleared area.
    Nonzero indicate the background should be used; 0 indicates the 'clear'
    color should be used (see <b>setColor()</b> above). */
    virtual void	clearScreen(RECT *rp, int useBkg = FALSE) = 0;
    /*! \remarks Sets the current transformation matrix, and updates the
    modeling coordinates to normalized projection coordinates matrix. This
    routine also back-transforms each light and the eye point (so that lighting
    can be done in modeling coordinates).\n\n
    This method may be used to set a matrix that transforms the point passed to
    the drawing methods (like <b>text(), marker(), polyline()</b> or
    <b>polygon()</b>). Normally these methods expect world coordinates. However
    if this matrix is set to an objects transformation matrix you can pass
    objects space coordinates and they will be transformed into world space
    (and then put into screen space when they are drawn). If however this is
    set to the identity matrix you would pass world space coordinates. You can
    set this matrix to the objects matrix using the following code:\n\n
    <b>gw-\>setTransform(inode-\>GetObjectTM(t));</b>\n\n
    Note: For world-to-screen space conversions by the methods <b>text(),
    marker(), polyline()</b>, <b>polygon()</b>, etc, a developer must
    explicitly set this matrix to the identity. This is because the
    <b>GraphicsWindow</b>transform may have a non-identity matrix already in
    place from a previous operation.
    \par Parameters:
    <b>const Matrix3 \&m</b>\n\n
    The new current transformation matrix. */
    virtual void	setTransform(const Matrix3 &m) = 0;
	virtual Matrix3	getTransform(void) = 0;
	virtual void	multiplePass(int pass, BOOL onOff, float scaleFact = 1.005f) = 0;
    /*! \remarks Sets the current transparency flags for the current pass.\n\n

    \par Parameters:
    <b>DWORD settings</b>\n\n
    This can be a combination if GW_TRANSPARENCY and GW_TRANSPARENT_PASS See
    <a href="ms-its:listsandfunctions.chm::/idx_R_list_of_rendering_limits.html">Rendering Limits</a>. You
    also use these settings in the Render limits as well. */
    virtual void	setTransparency(DWORD settings) = 0;
	 /*! \remarks This method allows one to put an affine transformation on a
	 texture. This allows you to translate, rotate or scale a texture on an object.
	 \par Parameters:
	 <b>const Matrix3 \&m</b>\n\n
	 The texture transformation matrix. */
    virtual void	setTexTransform(const Matrix3 &m, int texStage=0) = 0;
	/*! \remarks This is used internally. It returns if the determinant of the
	current transform is positive or negative. If it's positive 0 is returned;
	if it's negative 1 is returned. */
	virtual BOOL	getFlipped(void)=0;
	/*! \remarks Sets the number of triangles skipped when the viewport is set
	as a 'Fast View Display' viewport. To disable fastview, specify 1. Note
	that the GraphicsWindow class doesn't actually do anything with this number
	other than store it. Since triangles are handed down to GFX one at a time,
	it is up to the code that feeds triangles to the GraphicsWindow to skip the
	specified number of triangles. The mesh rendering in 3ds Max uses the skip
	count in this way.
	\par Parameters:
	<b>int n</b>\n\n
	Specifies that every 'n-th' triangle should be drawn. If set to 2, every
	other triangle should be drawn. */
	virtual void	setSkipCount(int c) = 0;
	/*! \remarks Returns the current skip count setting. */
	virtual int		getSkipCount(void) = 0;
	virtual void	setViewportLimits(DWORD l) = 0;
	virtual DWORD	getViewportLimits(void) = 0;
    /*! \remarks Sets the rendering limits used by primitive calls.\n\n
    Note: Setting the rendering limits is used in communication between the
    various parts of 3ds Max that handle the display of objects. For example,
    setting this limit to <b>GW_POLY_EDGES</b> and then drawing a polygon won't
    result in a polygon drawn with edges. It only sets a flag that indicates
    the edge should be drawn.\n\n
    What happens is as follows. Inside the upper level 3ds Max, part of the
    code knows that polygon edges have been turned on. However this is not
    related through the object oriented architecture to the part of 3ds Max
    that does the actual drawing. When 3ds Max goes to draw objects it will see
    that the polygon edge flag is on. This tells it to do two drawing passed --
    one to do the polygon, then it calls <b>outlinePass()</b> call with TRUE,
    draws a bunch of edges, then calls <b>outline Pass()</b> with FALSE. Thus,
    the drawing routine is responsible for looking at the flags and drawing
    appropriately. This method is only responsible setting the limit which can
    later be checked.
    \par Parameters:
    <b>DWORD l</b>\n\n
    Specifies the rendering limit used by the viewport. See
    <a href="ms-its:listsandfunctions.chm::/idx_R_list_of_rendering_limits.html">Rendering Limits</a>. */
    virtual void	setRndLimits(DWORD l) = 0;
	/*! \remarks Retrieves the rendering limits used by primitive calls. See
	<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_rendering_limits.html">Rendering Limits</a>. */
	virtual DWORD 	getRndLimits(void) = 0;
	/*! \remarks Returns the current rendering mode used by the viewport. This
	is a subset of the rendering limit, in that any limits imposed by the
	rendering limit are forced onto the current mode. See
	<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_rendering_limits.html">Rendering Limits</a>. */
	virtual DWORD 	getRndMode(void) = 0;
	/*! \remarks Returns the maximum number of lights that may be used by the
	interactive renderer. */
	virtual int		getMaxLights(void) = 0;
    /*! \remarks Turns a light on or off, and sets the light parameters. The
    light's position is set through the current transformation matrix at the
    time of this call. A particular light is specified by its light number (-1
    through <b>getMaxLights()-1</b>). Light number -1 is reserved for ambient
    light. If a particular field in the Light class is not needed for a
    particular type of light, that field's value is ignored (for example, only
    the color field is used for ambient light.)
    \par Parameters:
    <b>int num</b>\n\n
    The light number of the light to set. This is a value in the range -1 to
    <b>getMaxLights()-1</b>.\n\n
    <b>const Light *l</b>\n\n
    The light class instance used to set the light parameters. If this is NULL,
    the light is turned off. */
    virtual void	setLight(int num, const Light *l) = 0;
	/*! \remarks This allows a developer to control if a light is used to
	render an object. There is one bit per light (bits 0 through
	<b>getMaxLights()</b>). If the bit is set the light is NOT used to render
	the object. If the bit is off, the light IS used. This method allows you to
	set the exclusion vector controlling the lights.
	\par Parameters:
	<b>DWORD exclVec</b>\n\n
	The exclusion vector controlling the lights. */
	virtual void	setLightExclusion(DWORD exclVec) = 0;
    /*! \remarks This method is no longer used. */
    virtual void	setCamera(const Camera &c) = 0;
	/*! \remarks Sets the properties of the current camera used by the
	GraphicsWindow.
	\par Parameters:
	<b>float mat[4][4]</b>\n\n
	The transformation matrix times the projection matrix.\n\n
	<b>Matrix3 *invTM</b>\n\n
	This is the inverse of the affine part of the camera transformation matrix
	(not the inverse of the projection part).\n\n
	<b>int persp</b>\n\n
	Nonzero indicates this is a perspective view; 0 is orthogonal.\n\n
	<b>float hither</b>\n\n
	Near clip value.\n\n
	<b>float yon</b>\n\n
	Far clip value. */
	virtual void	setCameraMatrix(float mat[4][4], Matrix3 *invTM, int persp, float hither, float yon) = 0;
	/*! \remarks Retrieves the properties of the current camera.
	\par Parameters:
	<b>float mat[4][4]</b>\n\n
	The transformation matrix times the projection matrix.\n\n
	<b>Matrix3 *invTM</b>\n\n
	This is the inverse of the affine part of the camera transformation matrix
	(not the inverse of the projection part).\n\n
	<b>int *persp</b>\n\n
	Nonzero indicates this is a perspective view; 0 is orthogonal.\n\n
	<b>float *hither</b>\n\n
	Near clip value.\n\n
	<b>float *yon</b>\n\n
	Far clip value. */
	virtual void	getCameraMatrix(float mat[4][4], Matrix3 *invTM, int *persp, float *hither, float *yon) = 0;
    /*! \remarks Sets the RGB color used for the specified drawing type (line,
    fill, text, clear).
    \par Parameters:
    <b>ColorType t</b>\n\n
    One of the following values:\n\n
    <b>LINE_COLOR</b>\n\n
    Line drawing color.\n\n
    <b>FILL_COLOR</b>\n\n
    Polygon fill color.\n\n
    <b>TEXT_COLOR</b>\n\n
    Text drawing color.\n\n
    <b>CLEAR_COLOR</b>\n\n
    The color that the viewport is cleared to when you call
    <b>clearScreen()</b>.\n\n
    <b>float r</b>\n\n
    Specifies the red amount 0.0 - 1.0.\n\n
    <b>float g</b>\n\n
    Specifies the green amount 0.0 - 1.0.\n\n
    <b>float b</b>\n\n
    Specifies the blue amount 0.0 - 1.0. */
    virtual void	setColor(ColorType t, float r, float g, float b) = 0;
			void	setColor(ColorType t, Point3 clr) { setColor(t,clr.x,clr.y,clr.z); }
    /*! \remarks Sets the current rendering material, and modifies the
    rendering mode parameter for controlling the rasterizer driver. Note: You
    must have your rendering limit set BEFORE you set the material because the
    material setting may lower the rendering mode based on the material limits.
    \par Parameters:
    <b>const Material \&m</b>\n\n
    The new material to instantiate\n\n
    <b>int index=0</b>\n\n
    Indicates which material index refers to the material which gets set. */
    virtual void	setMaterial(const Material &m, int index=0) = 0;
	/*! \remarks Returns the current rendering material.\n\n
	  */
	virtual Material *getMaterial(void) = 0;
	virtual int		getMaxTextures(void) = 0;

	/*! \remarks	This method converts coordinates to "<b>h</b>" format device coordinates.
	Note: This method maps points from the GraphicsWindow's current transform
	to device space. If the GraphicsWindow's transform is set to the identity
	matrix then the mapping is done from points specified in world space.
	Otherwise the points given are transformed by the GraphicsWindow transform,
	and are <b>then</b> considered to be in world space. Thus, to get a
	world-space to screen-space conversion, you need to set the transform to
	the identity with <b>gw-\>setTransform(Matrix3(1))</b>.
	\par Parameters:
	<b>const Point3 *in</b>\n\n
	The input point.\n\n
	<b>IPoint3 *out</b>\n\n
	The output point in integer format values in the native device coords for
	the device. For HEIDI and OpenGL the origin at the lower left. For Direct3D
	the origin is at the upper left.
	\return  DWORD containing the clipping flags for the point. If a flag is
	set it indicates the transformed point lies outside the view volume. See
	<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_clip_flags.html">List of Clip Flags</a>. */
	virtual DWORD	hTransPoint(const Point3 *in, IPoint3 *out) = 0;
	/*! \remarks	This method is used to convert coordinates to "<b>w</b>" format device
	coordinates. Note: This method maps points from the GraphicsWindow's
	current transform to device space. If the GraphicsWindow's transform is set
	to the identity matrix then the mapping is done from points specified in
	world space. Otherwise the points given are transformed by the
	GraphicsWindow transform, and are <b>then</b> considered to be in world
	space. Thus, to get a world-space to screen-space conversion, you need to
	set the transform to the identity with
	<b>gw-\>setTransform(Matrix3(1))</b>.\n\n

	\par Parameters:
	<b>const Point3 *in</b>\n\n
	The input point.\n\n
	<b>IPoint3 *out</b>\n\n
	The output point in integer format with the origin at the upper left.
	\return  DWORD containing the clipping flags for the point. If a flag is
	set it indicates the transformed point lies outside the view volume. See
	<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_clip_flags.html">List of Clip Flags</a>. */
	virtual DWORD	wTransPoint(const Point3 *in, IPoint3 *out) = 0;
	/*! \remarks	This method is used to convert coordinates to "<b>h</b>" <b>floating
	point</b> coordinates. This is just a helper routine to avoid building up
	round-off error. 3ds Max uses it just for IK. Note: This method maps points
	from the GraphicsWindow's current transform to device space. If the
	GraphicsWindow's transform is set to the identity matrix then the mapping
	is done from points specified in world space. Otherwise the points given
	are transformed by the GraphicsWindow transform, and are <b>then</b>
	considered to be in world space. Thus, to get a world-space to screen-space
	conversion, you need to set the transform to the identity with
	<b>gw-\>setTransform(Matrix3(1))</b>.
	\par Parameters:
	<b>const Point3 *in</b>\n\n
	The input point.\n\n
	<b>Point3 *out</b>\n\n
	The output point in floating point format with the origin at the lower
	left.
	\return  DWORD containing the clipping flags for the point. If a flag is
	set it indicates the transformed point lies outside the view volume. See
	<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_clip_flags.html">List of Clip Flags</a>. */
	virtual DWORD	transPoint(const Point3 *in, Point3 *out) = 0;
	/*! \remarks Lights a vertex, using all the current lights. The vertex
	appears to be transformed using the current transformation matrix, although
	actually the calculations are done using back-transformed light coordinates
	(for speed). The vertex position and normal are passed in, and a color is
	returned. The rendering uses the current material.
	\par Parameters:
	<b>const Point3 \&pos</b>\n\n
	Vertex position.\n\n
	<b>const Point3 \&nor</b>\n\n
	Vertex normal.\n\n
	<b>Point3 \&rgb</b>\n\n
	Returned color. */
	virtual void	lightVertex(const Point3 &pos, const Point3 &nor, Point3 &rgb) = 0;

	/*! \remarks	Draws 2D fixed font annotation text to the specified location. Note: This
	routine DOES perform clipping of the text if it is off the screen.
	\par Parameters:
	<b>IPoint3 *xyz</b>\n\n
	This is the device coordinate for the text. The origin of the text is at
	the lower left corner.\n\n
	<b>MCHAR *s</b>\n\n
	The text to display. */
	virtual void	hText(IPoint3 *xyz, MCHAR *s) = 0;
	/*! \remarks	Draws a marker at the specified location.
	\par Parameters:
	<b>IPoint3 *xyz</b>\n\n
	This is the device coordinate for the marker (with the origin at the lower
	left).\n\n
	<b>MarkerType type</b>\n\n
	See <a href="ms-its:listsandfunctions.chm::/idx_R_list_of_marker_types.html">List of Marker Types</a>. */
	virtual void	hMarker(IPoint3 *xyz, MarkerType type) = 0;
	/*! \remarks	This method draws a multi-segment polyline.
	\par Parameters:
	<b>int ct</b>\n\n
	The number of points in the polyline. The maximum number of points that may
	be used in drawing a polyline is 32.\n\n
	<b>IPoint3 *xyz</b>\n\n
	Array of points. These are device coordinates with the origin at the lower
	left.\n\n
	<b>Point3 *rgb</b>\n\n
	If the shade mode is set to smooth and these colors for the vertices are
	specified the polyline will be drawn Gourand shaded. This is how 3ds Max
	draws lit wireframes for instance. If you simply want ordinary lines (drawn
	using the line color) pass NULL.\n\n
	Note: The use of these colors is not supported under the OpenGL driver. The
	rgb values are ignored. Only the current material is taken into
	consideration. (This is how OpenGL works.)\n\n
	<b>int closed</b>\n\n
	If nonzero the first point is connected to the last point, i.e. the
	polyline is closed.\n\n
	<b>int *es</b>\n\n
	Edge state array. This is an array that Indicates if the 'i-th' edge is one
	of three state:\n\n
	<b>GW_EDGE_SKIP</b>\n\n
	Nonexistent - totally invisible.\n\n
	<b>GW_EDGE_VIS</b>\n\n
	Exists and is solid.\n\n
	<b>GW_EDGE_INVIS</b>\n\n
	Exists and is hidden - shown as a dotted line.\n\n
	You may pass NULL for this array and the method will assume that the edges
	are all solid. */
	virtual void	hPolyline(int ct, IPoint3 *xyz, Point3 *rgb, int closed, int *es) = 0;
			/*! \remarks This method is available in release 2.0 and later
			only.\n\n
			This method draws a multi-segment polyline.
			\par Parameters:
			<b>int ct</b>\n\n
			The number of points in the polyline. The maximum number of points
			that may be used in drawing a polyline is 32.\n\n
			<b>IPoint3 *xyz</b>\n\n
			Array of points. These are device coordinates with the origin at
			the lower left.\n\n
			<b>Point3 *rgb</b>\n\n
			If the shade mode is set to smooth and these colors for the
			vertices are specified the polyline will be drawn Gourand shaded.
			This is how 3ds Max draws lit wireframes for instance. If you
			simply want ordinary lines (drawn using the line color) pass
			NULL.\n\n
			Note: The use of these colors is not supported under the OpenGL
			driver. The rgb values are ignored. Only the current material is
			taken into consideration. (This is how OpenGL works.)\n\n
			<b>Point3 *uvw</b>\n\n
			This is not currently used. Pass NULL.\n\n
			<b>int closed</b>\n\n
			If nonzero the first point is connected to the last point, i.e. the
			polyline is closed.\n\n
			<b>int *es</b>\n\n
			Edge state array. This is an array that Indicates if the 'i-th'
			edge is one of three state:\n\n
			<b>GW_EDGE_SKIP</b>\n\n
			Nonexistent - totally invisible.\n\n
			<b>GW_EDGE_VIS</b>\n\n
			Exists and is solid.\n\n
			<b>GW_EDGE_INVIS</b>\n\n
			Exists and is hidden - shown as a dotted line.\n\n
			You may pass NULL for this array and the method will assume that
			the edges are all solid. */
			void	hPolyline(int ct, IPoint3 *xyz, Point3 *rgb, Point3 *uvw, int closed, int *es)
					{ hPolyline(ct, xyz, rgb, closed, es); }
	/*! \remarks	This method draws a multi-point polygon.
	\par Parameters:
	<b>int ct</b>\n\n
	The number of points in the polygon.\n\n
	<b>IPoint3 *xyz</b>\n\n
	Array of points. These are device coordinates with the origin at the lower
	left.\n\n
	<b>Point3 *rgb</b>\n\n
	The color values at the vertices. The rendering mode must include
	<b>GW_ILLUM</b> for these values to be used.\n\n
	Note: The use of these colors is not supported under the OpenGL driver. The rgb
	values are ignored. Only the current material is taken into consideration.
	(This is how OpenGL works.)\n\n
	<b>Point3 *uvw</b>\n\n
	The UVW coordinates. The rendering mode must include <b>GW_TEXTURE</b> for
	these values to be used. */
	virtual void	hPolygon(int ct, IPoint3 *xyz, Point3 *rgb, Point3 *uvw, int texNum=1) = 0;
	/*! \remarks	This method is used for drawing a series of triangles specified as 'strips'. It
	takes a count of 3 or more, and builds triangles in a strip. This sends a lot
	less data and the underlying graphics library has to set up a lot less data
	since it can use the previous information to start the rasterization. This
	results in a significant speed increase.\n\n
	Note that this routine does no clipping so all the points passed must be within
	view.
	\par Parameters:
	<b>int ct</b>\n\n
	The total number of points. After the first two points, each new point is used
	to create a new triangle.\n\n
	<b>IPoint3 *xyz</b>\n\n
	The point data with the origin at the lower left. For instance, to draw a quad,
	the first three points specify the first triangle and the next one is combined
	with the previous two to complete the square.\n\n
	The order for these points follows the 'standard' conventions for stripping
	used in most graphics libraries (for example Direct3D, OpenGL and Heidi).\n\n
	<b>Point3 *rgb</b>\n\n
	The colors for the vertices.\n\n
	Note: The use of these colors is not supported under the OpenGL driver. The rgb
	values are ignored. Only the current material is taken into consideration.
	(This is how OpenGL works.)\n\n
	<b>Point3 *uvw</b>\n\n
	The UVW texture coordinates for the vertices. */
	virtual void	hTriStrip(int ct, IPoint3 *xyz, Point3 *rgb, Point3 *uvw, int texNum=1) = 0;

	/*! \remarks	Draws 2D fixed font annotation text to the specified location. Note: This
	routine DOES perform clipping of the text if it is off the screen.
	\par Parameters:
	<b>IPoint3 *xyz</b>\n\n
	This is the device coordinate for the text. The origin of the text is at
	the upper left corner.\n\n
	<b>MCHAR *s</b>\n\n
	The text to display. */
	virtual void	wText(IPoint3 *xyz, MCHAR *s) = 0;
	/*! \remarks	Draws a marker at the specified location.
	\par Parameters:
	<b>IPoint3 *xyz</b>\n\n
	This is the device coordinate for the marker.\n\n
	<b>MarkerType type</b>\n\n
	See <a href="ms-its:listsandfunctions.chm::/idx_R_list_of_marker_types.html">List of Marker Types</a>. */
	virtual void	wMarker(IPoint3 *xyz, MarkerType type) = 0;
	/*! \remarks	This method draws a multi-segment polyline.
	\par Parameters:
	<b>int ct</b>\n\n
	The number of points in the polyline. The maximum number of points that may
	be used in drawing a polyline is 32.\n\n
	<b>IPoint3 *xyz</b>\n\n
	Array of points. These are device coordinates with the origin at the upper
	left.\n\n
	<b>Point3 *rgb</b>\n\n
	If the shade mode is set to smooth and these colors for the vertices are
	specified the polyline will be drawn Gourand shaded. This is how 3ds Max
	draws lit wireframes for instance. If you simply want ordinary lines (drawn
	using the line color) pass NULL.\n\n
	Note: The use of these colors is not supported under the OpenGL driver. The
	rgb values are ignored. Only the current material is taken into
	consideration. (This is how OpenGL works.)\n\n
	<b>int closed</b>\n\n
	If nonzero the first point is connected to the last point, i.e. the
	polyline is closed.\n\n
	<b>int *es</b>\n\n
	Edge state array. This is an array that Indicates if the 'i-th' edge is one
	of three state:\n\n
	<b>GW_EDGE_SKIP</b>\n\n
	Nonexistent - totally invisible.\n\n
	<b>GW_EDGE_VIS</b>\n\n
	Exists and is solid.\n\n
	<b>GW_EDGE_INVIS</b>\n\n
	Exists and is hidden - shown as a dotted line.\n\n
	You may pass NULL for this array and the method will assume that the edges
	are all solid. */
	virtual void	wPolyline(int ct, IPoint3 *xyz, Point3 *rgb, int closed, int *es) = 0;
			/*! \remarks This method is available in release 2.0 and later
			only.\n\n
			This method draws a multi-segment polyline.
			\par Parameters:
			<b>int ct</b>\n\n
			The number of points in the polyline. The maximum number of points
			that may be used in drawing a polyline is 32.\n\n
			<b>IPoint3 *xyz</b>\n\n
			Array of points. These are device coordinates with the origin at
			the upper left.\n\n
			<b>Point3 *rgb</b>\n\n
			If the shade mode is set to smooth and these colors for the
			vertices are specified the polyline will be drawn Gourand shaded.
			This is how 3ds Max draws lit wireframes for instance. If you
			simply want ordinary lines (drawn using the line color) pass
			NULL.\n\n
			Note: The use of these colors is not supported under the OpenGL
			driver. The rgb values are ignored. Only the current material is
			taken into consideration. (This is how OpenGL works.)\n\n
			<b>Point3 *uvw</b>\n\n
			This is not currently used. Pass NULL.\n\n
			<b>int closed</b>\n\n
			If nonzero the first point is connected to the last point, i.e. the
			polyline is closed.\n\n
			<b>int *es</b>\n\n
			Edge state array. This is an array that Indicates if the 'i-th'
			edge is one of three state:\n\n
			<b>GW_EDGE_SKIP</b>\n\n
			Nonexistent - totally invisible.\n\n
			<b>GW_EDGE_VIS</b>\n\n
			Exists and is solid.\n\n
			<b>GW_EDGE_INVIS</b>\n\n
			Exists and is hidden - shown as a dotted line.\n\n
			You may pass NULL for this array and the method will assume that
			the edges are all solid. */
			void	wPolyline(int ct, IPoint3 *xyz, Point3 *rgb, Point3 *uvw, int closed, int *es)
					{ wPolyline(ct, xyz, rgb, closed, es); }
	
	/*! \remarks	This method draws a multi-point polygon.
	\par Parameters:
	<b>int ct</b>\n\n
	The number of points in the polygon.\n\n
	<b>IPoint3 *xyz</b>\n\n
	Array of points. These are device coordinates with the origin at the upper
	left.\n\n
	<b>Point3 *rgb</b>\n\n
	The color values at the vertices. The rendering mode must include
	<b>GW_ILLUM</b> for these values to be used.\n\n
	Note: The use of these colors is not supported under the OpenGL driver. The rgb
	values are ignored. Only the current material is taken into consideration.
	(This is how OpenGL works.)\n\n
	<b>Point3 *uvw</b>\n\n
	The UVW coordinates. The rendering mode must include <b>GW_TEXTURE</b> for
	these values to be used. */
	virtual void	wPolygon(int ct, IPoint3 *xyz, Point3 *rgb, Point3 *uvw, int texNum=1) = 0;
	
	/*! \remarks	This method is used for drawing a series of triangles specified as 'strips'. It
	takes a count of 3 or more, and builds triangles in a strip. This sends a lot
	less data and the underlying graphics library has to set up a lot less data
	since it can use the previous information to start the rasterization. This
	results in a significant speed increase.\n\n
	Note that this routine does no clipping so all the points passed must be within
	view.
	\par Parameters:
	<b>int ct</b>\n\n
	The total number of points. After the first two points, each new point is used
	to create a new triangle.\n\n
	<b>IPoint3 *xyz</b>\n\n
	The point data with the origin at the upper left. For instance, to draw a quad,
	the first three points specify the first triangle and the next one is combined
	with the previous two to complete the square.\n\n
	The order for these points follows the 'standard' conventions for stripping
	used in most graphics libraries (for example Direct3D, OpenGL and Heidi).\n\n
	<b>Point3 *rgb</b>\n\n
	The colors for the vertices.\n\n
	Note: The use of these colors is not supported under the OpenGL driver. The rgb
	values are ignored. Only the current material is taken into consideration.
	(This is how OpenGL works.)\n\n
	<b>Point3 *uvw</b>\n\n
	The UVW texture coordinates for the vertices. */
	virtual void	wTriStrip(int ct, IPoint3 *xyz, Point3 *rgb, Point3 *uvw, int texNum=1) = 0;

    /*! \remarks Draws 2D fixed font annotation text to the specified
    location. This method does perform clipping.\n\n
    Note: This method maps points from the GraphicsWindow's current transform
    to screen space. If the GraphicsWindow's transform is set to the identity
    matrix then the mapping is done from points specified in world space.
    Otherwise the points given are transformed by the GraphicsWindow transform,
    and are <b>then</b> considered to be in world space. Thus, to get a
    world-space to screen-space conversion, you need to set the transform to
    the identity with <b>gw-\>setTransform(Matrix3(1))</b>.
    \par Parameters:
    <b>Point3 *xyz</b>\n\n
    This is the coordinate for the text.\n\n
    <b>MCHAR *s</b>\n\n
    The text to display. Note: This routine DOES perform clipping of the text
    if it is off the screen. */
    virtual void 	text(Point3 *xyz, MCHAR *s) = 0;
	virtual void	startMarkers() = 0;
    /*! \remarks Draws a marker at the specified location in world space. This
    method does perform clipping.\n\n
    Note: This method maps points from the GraphicsWindow's current transform
    to screen space. If the GraphicsWindow's transform is set to the identity
    matrix then the mapping is done from points specified in world space.
    Otherwise the points given are transformed by the GraphicsWindow transform,
    and are <b>then</b> considered to be in world space. Thus, to get a
    world-space to screen-space conversion, you need to set the transform to
    the identity with <b>gw-\>setTransform(Matrix3(1))</b>.
    \par Parameters:
    <b>Point3 *xyz</b>\n\n
    This is the coordinate for the marker.\n\n
    <b>MarkerType type</b>\n\n
    See <a href="ms-its:listsandfunctions.chm::/idx_R_list_of_marker_types.html">List of Marker Types</a>. */
    virtual void	marker(Point3 *xyz, MarkerType type) = 0;
	virtual void	endMarkers() = 0;
	virtual void	polyline(int ct, Point3 *xyz, Point3 *rgb, int closed, int *es) = 0;
			/*! \remarks Draws a multi-segment polyline with the coordinates
			specified in world space. This method does perform clipping.\n\n
			Note: The arrays of points and vertex related data all must be at
			least one element larger than the <b>ct</b> parameter that is
			passed in. The 3D clipper will use the "extra" space to clip as
			efficiently as possible. If room for the extra element is not
			provided, 3ds Max may crash.\n\n
			Note: This method maps points from the GraphicsWindow's current
			transform to screen space. If the GraphicsWindow's transform is set
			to the identity matrix then the mapping is done from points
			specified in world space. Otherwise the points given are
			transformed by the GraphicsWindow transform, and are <b>then</b>
			considered to be in world space. Thus, to get a world-space to
			screen-space conversion, you need to set the transform to the
			identity with <b>gw-\>setTransform(Matrix3(1))</b>.
			\par Parameters:
			<b>int ct</b>\n\n
			The number of points in the polyline. The maximum number of points
			that may be used in drawing a polyline is 32.\n\n
			<b>Point3 *xyz</b>\n\n
			Array of points. This array must be at least one element larger
			than the <b>ct</b> parameter that is passed in. The 3D clipper will
			use the "extra" space to clip as efficiently as possible. If room
			for the extra element is not provided, 3ds Max will crash.\n\n
			<b>Point3 *rgb</b>\n\n
			If the shade mode is set to smooth and these colors for the
			vertices are specified the polyline will be drawn Gourand shaded.
			This is how 3ds Max draws lit wireframes for instance. If you
			simply want ordinary lines (drawn using the line color) pass
			NULL.\n\n
			Note: The use of these colors is not supported under the OpenGL
			driver. The rgb values are ignored. Only the current material is
			taken into consideration. (This is how OpenGL works.)\n\n
			<b>Point3 *uvw</b>\n\n
			This is not currently used. Pass NULL.\n\n
			<b>int closed</b>\n\n
			If nonzero the first point is connected to the last point, i.e. the
			polyline is closed.\n\n
			<b>int *es</b>\n\n
			Edge state array. This is an array that Indicates if the 'i-th'
			edge is one of three state:\n\n
			<b>GW_EDGE_SKIP</b>\n\n
			Nonexistent - totally invisible.\n\n
			<b>GW_EDGE_VIS</b>\n\n
			Exists and is solid.\n\n
			<b>GW_EDGE_INVIS</b>\n\n
			Exists and is hidden - shown as a dotted line.\n\n
			You may pass NULL for this array and the method will assume that
			the edges are all solid. */
			void	polyline(int ct, Point3 *xyz, Point3 *rgb, Point3 *uvw, int closed, int *es)
					{ polyline(ct, xyz, rgb, closed, es); }
	/*! \remarks	Draws a multi-segment polyline with the coordinates specified in world
	space. This method takes a polyline with a normal for each vertex. This is
	used for hardware accelerated lit wireframes (when <b>GW_SPT_GEOM_ACCEL</b>
	is TRUE).\n\n
	Note: The arrays of points and vertex related data all must be at least one
	element larger than the <b>ct</b> parameter that is passed in. The 3D
	clipper will use the "extra" space to clip as efficiently as possible. If
	room for the extra element is not provided, 3ds Max may crash.\n\n
	This method does perform clipping.\n\n
	Note: This method maps points from the GraphicsWindow's current transform
	to screen space. If the GraphicsWindow's transform is set to the identity
	matrix then the mapping is done from points specified in world space.
	Otherwise the points given are transformed by the GraphicsWindow transform,
	and are <b>then</b> considered to be in world space. Thus, to get a
	world-space to screen-space conversion, you need to set the transform to
	the identity with <b>gw-\>setTransform(Matrix3(1))</b>.
	\par Parameters:
	<b>int ct</b>\n\n
	The number of points in the polyline. The maximum number of points that may
	be used in drawing a polyline is 32.\n\n
	<b>Point3 *xyz</b>\n\n
	Array of points. This array must be at least one element larger than the
	<b>ct</b> parameter that is passed in. The 3D clipper will use the "extra"
	space to clip as efficiently as possible. If room for the extra element is
	not provided, 3ds Max will crash.\n\n
	<b>Point3 *nor</b>\n\n
	The normal values at the vertices, one for each vertex.\n\n
	<b>int closed</b>\n\n
	If nonzero the first point is connected to the last point, i.e. the
	polyline is closed.\n\n
	<b>int *es</b>\n\n
	Edge state array. This is an array that Indicates if the 'i-th' edge is one
	of three state:\n\n
	<b>GW_EDGE_SKIP</b>\n\n
	Nonexistent - totally invisible.\n\n
	<b>GW_EDGE_VIS</b>\n\n
	Exists and is solid.\n\n
	<b>GW_EDGE_INVIS</b>\n\n
	Exists and is hidden - shown as a dotted line.\n\n
	You may pass NULL for this array and the method will assume that the edges
	are all solid. */
	virtual void	polylineN(int ct, Point3 *xyz, Point3 *nor, int closed, int *es) = 0;
	/*! \remarks	This method is used to begin efficiently sending a lot of 3D line segments.
	First call this method, then call <b>segment()</b> many times (with two
	points), then call <b>endSegments()</b>. */
	virtual void	startSegments() = 0;
	/*! \remarks	This method draws a single 3D line segment between the specified points.
	Call <b>startSegments()</b> once before calling this method.
	\par Parameters:
	<b>Point3 *xyz</b>\n\n
	Points to the two line endpoints in world space.\n\n
	<b>int vis</b>\n\n
	Nonzero for the segment to be visible; zero for invisible. */
	virtual void	segment(Point3 *xyz, int vis) = 0;
	/*! \remarks	Call this method after sending 3D line segments with <b>segment()</b>. */
	virtual void	endSegments() = 0;
	/*! \remarks Draws a multi-point polygon. Note: All arrays (<b>xyz, rgb,
	uvw</b>) must be at least one element larger than the <b>ct</b> parameter that
	is passed in. The 3D clipper will use the "extra" space to clip as efficiently
	as possible. If room for the extra element is not provided, 3ds Max may
	crash.\n\n
	Note: This method maps points from the GraphicsWindow's current transform to
	screen space. If the GraphicsWindow's transform is set to the identity matrix
	then the mapping is done from points specified in world space. Otherwise the
	points given are transformed by the GraphicsWindow transform, and are
	<b>then</b> considered to be in world space. Thus, to get a world-space to
	screen-space conversion, you need to set the transform to the identity with
	<b>gw-\>setTransform(Matrix3(1))</b>.
	\par Parameters:
	<b>int ct</b>\n\n
	The number of points in the polygon.\n\n
	<b>Point3 *xyz</b>\n\n
	Array of points.\n\n
	<b>Point3 *rgb</b>\n\n
	The color values at the vertices. The rendering mode must include
	<b>GW_ILLUM</b> for these values to be used.\n\n
	Note: The use of these colors is not supported under the OpenGL driver. The rgb
	values are ignored. Only the current material is taken into consideration.
	(This is how OpenGL works.)\n\n
	<b>Point3 *uvw</b>\n\n
	The UVW coordinates. The rendering mode must include <b>GW_TEXTURE</b> for
	these values to be used. */
	virtual void 	polygon(int ct, Point3 *xyz, Point3 *rgb, Point3 *uvw, int texNum=1) = 0;
	/*! \remarks	Draws a multi-point polygon. Note: All arrays (<b>xyz, nor, uvw</b>) must be at
	least one element larger than the <b>ct</b> parameter that is passed in. The 3D
	clipper will use the "extra" space to clip as efficiently as possible. If room
	for the extra element is not provided, 3ds Max will crash.\n\n
	This method sends in normal vectors instead of color for 3D accelerated
	rendering (when <b>GW_SPT_GEOM_ACCEL</b> is TRUE)\n\n
	Note: This method maps points from the GraphicsWindow's current transform to
	screen space. If the GraphicsWindow's transform is set to the identity matrix
	then the mapping is done from points specified in world space. Otherwise the
	points given are transformed by the GraphicsWindow transform, and are
	<b>then</b> considered to be in world space. Thus, to get a world-space to
	screen-space conversion, you need to set the transform to the identity with
	<b>gw-\>setTransform(Matrix3(1))</b>.
	\par Parameters:
	<b>int ct</b>\n\n
	The number of points in the polygon.\n\n
	<b>Point3 *xyz</b>\n\n
	Array of points.\n\n
	<b>Point3 *nor</b>\n\n
	The normal values at the vertices, one for each vertex.\n\n
	<b>Point3 *uvw</b>\n\n
	The UVW coordinates. The rendering mode must include <b>GW_TEXTURE</b> for
	these values to be used. */
	virtual void 	polygonN(int ct, Point3 *xyz, Point3 *nor, Point3 *uvw, int texNum=1) = 0;
	/*! \remarks	This method is used for drawing a series of triangles specified as 'strips'. It
	takes a count of 3 or more, and builds triangles in a strip. This sends a lot
	less data and the underlying graphics library has to set up a lot less data
	since it can use the previous information to start the rasterization. This
	results in a significant speed increase.
	\par Parameters:
	<b>int ct</b>\n\n
	The total number of points. After the first two points, each new point is used
	to create a new triangle.\n\n
	<b>IPoint3 *xyz</b>\n\n
	The point data. For instance, to draw a quad, the first three points specify
	the first triangle and the next one is combined with the previous two to
	complete the square.\n\n
	The order for these points follows the 'standard' conventions for stripping
	used in most graphics libraries (for example Direct3D, OpenGL and Heidi).\n\n
	<b>Point3 *rgb</b>\n\n
	The colors for the vertices.\n\n
	Note: The use of these colors is not supported under the OpenGL driver. The rgb
	values are ignored. Only the current material is taken into consideration.
	(This is how OpenGL works.)\n\n
	<b>Point3 *uvw</b>\n\n
	The UVW texture coordinates for the vertices. */
	virtual void	triStrip(int ct, Point3 *xyz, Point3 *rgb, Point3 *uvw, int texNum=1) = 0;
	/*! \remarks	This method is used for drawing a series of triangles specified as 'strips'. It
	takes a count of 3 or more, and builds triangles in a strip. This sends a lot
	less data and the underlying graphics library has to set up a lot less data
	since it can use the previous information to start the rasterization. This
	results in a significant speed increase. This method sends in normal vectors
	instead of color for 3D accelerated rendering (when <b>GW_SPT_GEOM_ACCEL</b> is
	TRUE)
	\par Parameters:
	<b>int ct</b>\n\n
	The total number of points. After the first two points, each new point is used
	to create a new triangle.\n\n
	<b>Point3 *xyz</b>\n\n
	The point data. For instance, to draw a quad, the first three points specify
	the first triangle and the next one is combined with the previous two to
	complete the square.\n\n
	The order for these points follows the 'standard' conventions for stripping
	used in most graphics libraries (for example Direct3D, OpenGL and Heidi).\n\n
	<b>Point3 *nor</b>\n\n
	The normal for each vertex.\n\n
	<b>Point3 *uvw</b>\n\n
	The UVW texture coordinates for the vertices. */
	virtual void	triStripN(int ct, Point3 *xyz, Point3 *nor, Point3 *uvw, int texNum=1) = 0;
	/*! \remarks	This method is called to begin sending a series of non-stripped triangles
	to render. Call this method, then any of the <b>triangle*()</b> methods
	many times, then <b>endTriangles()</b> to finish. */
	virtual void	startTriangles() = 0;
	/*! \remarks	This method sends a single non-stripped triangle to render. Call
	<b>startTriangles()</b> first.\n\n
	Note: This method maps points from the GraphicsWindow's current transform
	to screen space. If the GraphicsWindow's transform is set to the identity
	matrix then the mapping is done from points specified in world space.
	Otherwise the points given are transformed by the GraphicsWindow transform,
	and are <b>then</b> considered to be in world space. Thus, to get a
	world-space to screen-space conversion, you need to set the transform to
	the identity with <b>gw-\>setTransform(Matrix3(1))</b>.
	\par Parameters:
	<b>Point3 *xyz</b>\n\n
	The three points for the triangle.\n\n
	<b>Point3 *rgb</b>\n\n
	The color for each vertex.\n\n
	Note: The use of these colors is not supported under the OpenGL driver. The
	rgb values are ignored. Only the current material is taken into
	consideration. (This is how OpenGL works.) */
	virtual void	triangle(Point3 *xyz, Point3 *rgb) = 0;
	/*! \remarks	This method draws a single triangle by specifying the vertex points in world
	space, a normal, and texture coordinates for each vertex.\n\n
	Note: This method maps points from the GraphicsWindow's current transform to
	screen space. If the GraphicsWindow's transform is set to the identity matrix
	then the mapping is done from points specified in world space. Otherwise the
	points given are transformed by the GraphicsWindow transform, and are
	<b>then</b> considered to be in world space. Thus, to get a world-space to
	screen-space conversion, you need to set the transform to the identity with
	<b>gw-\>setTransform(Matrix3(1))</b>.
	\par Parameters:
	<b>Point3 *xyz</b>\n\n
	The three points for the triangle.\n\n
	<b>Point3 *nor</b>\n\n
	The three normals for the triangle.\n\n
	<b>Point3 *uvw</b>\n\n
	The texture coordinate for each vertex. */
	virtual void	triangleN(Point3 *xyz, Point3 *nor, Point3 *uvw, int texNum=1) = 0;
	/*! \remarks	This method draws a single triangle by specifying the vertex points in
	world space, a normal, and a color for each vertex.\n\n
	Note: This method maps points from the GraphicsWindow's current transform
	to screen space. If the GraphicsWindow's transform is set to the identity
	matrix then the mapping is done from points specified in world space.
	Otherwise the points given are transformed by the GraphicsWindow transform,
	and are <b>then</b> considered to be in world space. Thus, to get a
	world-space to screen-space conversion, you need to set the transform to
	the identity with <b>gw-\>setTransform(Matrix3(1))</b>.
	\par Parameters:
	<b>Point3 *xyz</b>\n\n
	The three points for the triangle.\n\n
	<b>Point3 *nor</b>\n\n
	The normal for each vertex.\n\n
	<b>Point3 *rgb</b>\n\n
	The color for each vertex. */
	virtual void	triangleNC(Point3 *xyz, Point3 *nor, Point3 *rgb) = 0;
	/*! \remarks	This method draws a single triangle by specifying the vertex points in world
	space, a normal, a color, and a texture coordinate for each vertex.\n\n
	Note: This method maps points from the GraphicsWindow's current transform to
	screen space. If the GraphicsWindow's transform is set to the identity matrix
	then the mapping is done from points specified in world space. Otherwise the
	points given are transformed by the GraphicsWindow transform, and are
	<b>then</b> considered to be in world space. Thus, to get a world-space to
	screen-space conversion, you need to set the transform to the identity with
	<b>gw-\>setTransform(Matrix3(1))</b>.
	\par Parameters:
	<b>Point3 *xyz</b>\n\n
	The three points for the triangle.\n\n
	<b>Point3 *nor</b>\n\n
	The normal for each vertex.\n\n
	<b>Point3 *rgb</b>\n\n
	The color for each vertex.\n\n
	<b>Point3 *uvw</b>\n\n
	The texture coordinate for each vertex. */
	virtual void	triangleNCT(Point3 *xyz, Point3 *nor, Point3 *rgb, Point3 *uvw, int texNum=1) = 0;
	virtual void	triangleW(Point3 *xyz, int *es) = 0;
	virtual void	triangleNW(Point3 *xyz, Point3 *nor, int *es) = 0;
	/*! \remarks	Call this method to finish rendering triangles. See <b>startTriangles()</b>
	above. */
	virtual void	endTriangles() = 0;
	virtual void	loadMeshData(DWORD_PTR id, int xyzCt, Point3 *xyz, int norCt, Point3 *nor, int texNum, int uvwCt, Point3 *uvw, int mtlCt, Material *mtl) = 0;
	virtual void	processStrips(DWORD_PTR id, int stripCt, StripTab *s, GFX_ESCAPE_FN fn) = 0;
	virtual void	processWireFaces(int xyzCt, Point3 *xyz, int faceCt, GWFace *face, int dispFlags, BitArray *faceSel, BitArray *edgeSel, int mtlCt, Material *mtl, GFX_ESCAPE_FN fn) = 0;

	/*! \remarks Sets the hit region used for hit testing. See
	Class HitRegion.
	\par Parameters:
	<b>HitRegion *rgn</b>\n\n
	The hit region to use. */
	virtual void	setHitRegion(HitRegion *rgn) = 0;
	/*! \remarks This methods clears the hit code. Call this method before
	performing a hit test. */
	virtual void	clearHitCode(void) = 0;
	/*! \remarks Returns TRUE if the hit code is set indicating a hit was
	made; otherwise FALSE. */
	virtual BOOL	checkHitCode(void) = 0;
	/*! \remarks	This method allows drawing code to manually set the state of the hit code,
	which is returned by the <b>checkHitCode()</b> method. For more information
	see the topic on
	<a href="ms-its:3dsmaxsdk.chm::/vports_hit_testing.html">Hit
	Testing</a>.\n\n
	The new methods <b>setHitDistance()</b> and <b>setHitCode()</b> make it
	possible to work with GraphicsWindow hit-testing in otherwise impossible
	situations. Why are they necessary? An example from is shown below. The
	patch object contains bezier spline-based edges which can consist of up to
	102 vertices. Since the GraphicsWindow::polyline function can only plot
	lines with up to 32 vertices, it is impossible to plot these in a single
	call to the polyline function. Multiple calls to the polyline call do not
	return a proper hitcode when using a "window"-type hit region. By using the
	new <b>setHitCode()</b> method, code can properly handle this situation.
	The code below shows the function in use from the PatchMesh::renderEdge method:
\code	
	int steps = GetMeshSteps();
	int segNum = steps+2;
	float fsegNum = (float) (segNum-1);
	// If steps are too high for GraphicsWindow's buffer, we must draw it
	manually
	if((steps + 2) > GW_MAX_VERTS) {
	 Point3 line[2];
	 Point3 prev,current(.0f,.0f,.0f);
	 BOOL hitAll = TRUE;
	 BOOL hitAny = FALSE;
	 DWORD hitDist = 0xffffffff;
	 for(int terp = 0; terp \
	 {
	  prev = current;
	  current = work.InterpCurve3D((float)terp / fsegNum);
	  if (terp != 0)
	  {
	   line[0] = prev;
	   line[1] = current;
	   gw->clearHitCode();
	   gw->polyline(2, line, NULL, NULL, 0, NULL);
	   if(gw->checkHitCode()) {
	    hitAny = TRUE;
	    if(gw->getHitDistance() \
	     hitDist = gw->getHitDistance();
	   }
	   else hitAll = FALSE;
	  }
	 }
	 if(hr && !hr->crossing && hr->type != POINT_RGN)
	  gw->setHitCode(hitAll);
	 else
	  gw->setHitCode(hitAny);
	 gw->setHitDistance(hitDist);
	} else {
	 for(int terp = 0; terp \
	  fixedBuf[terp] = work.InterpCurve3D((float)terp /
	fsegNum);
	 gw->polyline(steps+2, fixedBuf, NULL, NULL, 0, NULL);
	}
\endcode 
	Note that the <b>gw-\>polyline</b> call is preceded by a call to
	<b>clearHitCode()</b>, and followed by code which checks the hit code,
	maintaining "hitAny" and "hitAll" flags. When all the segments are drawn,
	the <b>gw-\>setHitCode()</b> call is made, setting the hit code depending
	on the hit region type. When the code which called this function checks the
	GraphicsWindow's hit code, it will contain the proper value. This code also
	keeps track of the closest hit distance and places that into the
	GraphicsWindow when all line segments are drawn.
	\par Parameters:
	<b>BOOL h</b>\n\n
	Set to TRUE if the hit code is set, otherwise FALSE. */
	virtual void	setHitCode(BOOL h) = 0;
	/*! \remarks If <b>checkHitCode()</b> returns TRUE you may call this
	method to return the hit distance. In wireframe mode this is the distance
	to the line. In shaded mode, this is the z distance. This allows you to
	perform 'smart' hit testing by choosing the item with the smallest hit
	distance. This method only returns meaningful values when the hit region is
	a point. */
	virtual DWORD	getHitDistance(void) = 0;
	/*! \remarks	This method allows drawing code to manually set the hit distance, which is
	returned by the <b>getHitDistance()</b> method. For more information see
	the topic on <a href="ms-its:3dsmaxsdk.chm::/vports_hit_testing.html">Hit
	Testing</a>.
	\par Parameters:
	<b>DWORD d</b>\n\n
	In wireframe mode this is the distance to the line. In shaded mode, this is
	the z distance. */
	virtual void	setHitDistance(DWORD d)	= 0;

	/*! \remarks Returns TRUE if the view is in perspective projection;
	otherwise FALSE (orthographic projection). */
	virtual int		isPerspectiveView(void) = 0;
	/*! \remarks This method is used internally. */
	virtual float	interpWorld(Point3 *world1, Point3 *world2, float sParam, Point3 *interpPt) = 0;

	//watje
	virtual void			MarkerBufferSetMarkerType( MarkerType type) {};
	virtual DWORD			MarkerGetDXColor(Point3 p) {return 0;};

	virtual VertexBuffer*	MarkerBufferLock() { return NULL;};
	virtual void			MarkerBufferUnLock() {};
	virtual int				MarkerBufferSize( ) {return 0;};
	virtual int				MarkerBufferStride( ){ return 0;};
	virtual void			MarkerBufferDraw(int numberOfMarkers) {};

	//watje
	virtual DWORD			LineGetDXColor(Point3 p) {return 0;};

	virtual LineBuffer*	LineBufferLock() { return NULL;};
	virtual void			LineBufferUnLock() {};
	virtual int				LineBufferSize( ) {return 0;};
	virtual int				LineBufferStride( ){ return 0;};
	virtual void			LineBufferDraw(int numberOfSegments) {};

	virtual void	escape(GFX_ESCAPE_FN fn, void *data) = 0;
/*! \remarks	This method calculates the depth value of the screen pixel located at positon x,y.
	This method returns TRUE if the depth value is calculated, FALSE otherwise.
	It is currently supported in Direct3D and OpenGL driver. Thus returning FALSE for unsupported
	driver. This method is used to locate center pivot in SteeringWheel.
	\param[in] x The x coordinate in screen space. \n
	\param[in] y The y coordinate in screen space. \n
	\param[out] z A pointer to the buffer that receives the calculated depth value.
	\return TRUE if calculated correctly, FALSE if the method failed or not supported.
	*/
	virtual BOOL	getDepthValue(float x, float y, float* z) { return FALSE; }
	/*! \remarks This method will clear the depth buffer bits of this GraphicsWindow.
		It is currently supported in Direct3D and OpenGL driver. This method is used
		clear the depth before actually drawing the ViewCube, which cannot be messed up 
		with the scene geometry.
	*/
	virtual void	clearViewportDepth() {};

	/*! \remarks	This take a GFX_MESH::HWTupleMesh and creates a hardware specific mesh buffers
	\param[in]GFX_MESH::HWTupleMesh *hwMesh the mesh used to create the Hardware specific buffers. \n
	\return the hardware specific IHWDrawMesh.	*/
	virtual  GFX_MESH::IHWSubMesh  *CreateHWDrawMesh(GFX_MESH::HWTupleMesh *hwMesh) {return NULL;};

	/*! \remarks	This draws the hwMesh to the display.  If the hardware mesh holds a valid IHWDrawMesh
	it will use that to draw the mesh in retained mode.  Otherwise the buffers in the hwmesh will be used
	to draw the mesh in immediate mode which is much slower.
	\param[in]GFX_MESH::HWTupleMesh *hwMesh the mesh to be drawn. \n
		*/
	virtual  void DrawHWDrawMesh(GFX_MESH::HWTupleMesh *hwMesh) {};

};

// for Windows int coords with origin at upper-left
inline int wIsFacingBack(const IPoint3 &v0, const IPoint3 &v1, const IPoint3 &v2, int flip=0 )
{
	int s = ( (v0[0]-v1[0])*(v2[1]-v1[1]) - (v2[0]-v1[0])*(v0[1]-v1[1]) ) < 0;
	return flip ? !s : s;
}

// for HEIDI int coords with origin at lower-left
inline int hIsFacingBack(const IPoint3 &v0, const IPoint3 &v1, const IPoint3 &v2, int flip=0 )
{
	int s = ( (v0[0]-v1[0])*(v2[1]-v1[1]) - (v2[0]-v1[0])*(v0[1]-v1[1]) );
	return flip ? s < 0 : s > 0;
}

// CAL-03/05/03: include side facing in the facing type
enum FacingType {kFrontFacing, kSideFacing, kBackFacing};

//! \brief Returns the facing of a given triangle relative to the screen.
/*! \remarks Returns whether a given triangle is front-facing, 
 side-facing, or back-facing relative to the screen. The triangle is passed as three points in screen space. 
 This function is used for "w" format device coordinates.
\param v0 The 1st triangle vertex
\param v1 The 2nd triangle vertex
\param v2 The 3rd triangle vertex 
\param flip If true, flip the triangle (so backfacing would return frontfacing) */
inline FacingType wFacingType(const IPoint3 &v0, const IPoint3 &v1, const IPoint3 &v2, int flip=0 )
{
	int s = ( (v0[0]-v1[0])*(v2[1]-v1[1]) - (v2[0]-v1[0])*(v0[1]-v1[1]) );
	return (s == 0) ? kSideFacing : ((flip ? s > 0 : s < 0) ? kBackFacing : kFrontFacing);
}

//! \brief Returns the facing of a given triangle relative to the screen.
/*! \remarks The methods wFacingType() and hFacingType() will return whether a given triangle is front-facing, 
side-facing, or back-facing relative to the screen. The triangle is passed as three points in screen space. 
This function is used for "h" format device coordinates. 
\param v0 The 1st triangle vertex
\param v1 The 2nd triangle vertex
\param v2 The 3rd triangle vertex
\param flip If true, flip the triangle (so backfacing would return frontfacing) */
inline FacingType hFacingType(const IPoint3 &v0, const IPoint3 &v1, const IPoint3 &v2, int flip=0 )
{
	int s = ( (v0[0]-v1[0])*(v2[1]-v1[1]) - (v2[0]-v1[0])*(v0[1]-v1[1]) );
	return (s == 0) ? kSideFacing : ((flip ? s < 0 : s > 0) ? kBackFacing : kFrontFacing);
}

/*! \remarks This function is available in release 2.0 and later only.\n\n
This function is not supported for use in the SDK. */
DllExport HINSTANCE GetGraphicsLibHandle(MCHAR *driverLibName);
/*! \remarks This function is available in release 2.0 and later only.\n\n
This function is not supported for use in the SDK. */
DllExport BOOL GraphicsSystemIsAvailable(HINSTANCE drv);
/*! \remarks This function is available in release 2.0 and later only.\n\n
This function is not supported for use in the SDK. */
DllExport BOOL GraphicsSystemCanConfigure(HINSTANCE drv);
/*! \remarks This function is available in release 2.0 and later only.\n\n
This function is not supported for use in the SDK. */
DllExport BOOL GraphicsSystemConfigure(HWND hWnd, HINSTANCE drv);
/*! \remarks This function is available in release 2.0 and later only.\n\n
This function is not supported for use in the SDK. */
DllExport void FreeGraphicsLibHandle(HINSTANCE drv);

/*! \remarks This function is used internally to create a new graphics window.
Use of this method is not supported for plug-ins. */
DllExport GraphicsWindow *createGW(HWND hWnd, GWinSetup &gws);

/*! \remarks Returns a bounding rectangle that encloses the entire hit region.
For example if the hit regions was a fence region, this method would return the
smallest rectangle that included the entire set of fence region points.
\par Parameters:
<b>HitRegion *hr</b>\n\n
The hit region to check.\n\n
<b>RECT *rect</b>\n\n
The returned bounding rectangle. */
DllExport void getRegionRect(HitRegion *hr, RECT *rect);
/*! \remarks Returns TRUE if the specified point is inside the region
<b>hr</b>; otherwise FALSE. */
DllExport BOOL pointInRegion(int x, int y, HitRegion *hr);

/*! \remarks Returns the signed distance from <b>x,y</b> to the line defined
by <b>p1-\>p2</b>. */
DllExport int distToLine(int x, int y, int *p1, int *p2);
DllExport int zDepthToLine(int x, int y, int *p1, int *p2);

/*! \remarks Returns nonzero if the line defined by <b>p1-\>p2</b> crosses
into the RECT and 0 otherwise. */
DllExport int lineCrossesRect(RECT *rc, int *p1, int *p2);
DllExport int segCrossesRect(RECT *rc, int *p1, int *p2);
/*! \remarks Returns nonzero if the line defined by <b>p1-\>p2</b> crosses the
circle center at (<b>cx, cy</b>) with a radius of <b>r</b> 0 otherwise. */
DllExport int segCrossesCircle(int cx, int cy, int r, int *p1, int *p2);
/*! \remarks Returns TRUE if the point passed is inside the specified
triangle.
\par Parameters:
<b>IPoint3 \&p0</b>\n\n
The first point of the triangle.\n\n
<b>IPoint3 \&p1</b>\n\n
The second point of the triangle.\n\n
<b>IPoint3 \&p2</b>\n\n
The third point of the triangle.\n\n
<b>IPoint3 \&q</b>\n\n
The point to check.
\return  Returns TRUE if the point passed is inside the specified triangle;
otherwise FALSE. */
DllExport BOOL insideTriangle(IPoint3 &p0, IPoint3 &p1, IPoint3 &p2, IPoint3 &q);
/*! \remarks Returns the z value of where the projected screen point <b>q</b>
would intersect the triangle defined by (<b>p0, p1, p2</b>).
\par Parameters:
<b>IPoint3 \&p0</b>\n\n
The first point of the triangle.\n\n
<b>IPoint3 \&p1</b>\n\n
The second point of the triangle.\n\n
<b>IPoint3 \&p2</b>\n\n
The third point of the triangle.\n\n
<b>IPoint3 \&q</b>\n\n
The screen point to check. */
DllExport int getZfromTriangle(IPoint3 &p0, IPoint3 &p1, IPoint3 &p2, IPoint3 &q);

DllExport int getClosestPowerOf2(int num);

/*! \defgroup viewportDrawingColors Viewport Drawing Color Indices
\todo Document Viewport Drawing Colors defines
*/
//@{
#define COLOR_SELECTION				0 //!< 
#define COLOR_SUBSELECTION			1 //!< 
#define COLOR_FREEZE				2 //!< 
#define COLOR_GRID					3 //!< 
#define COLOR_GRID_INTENS			4 //!< 
#define COLOR_SF_LIVE				5 //!< 
#define COLOR_SF_ACTION				6 //!< 
#define COLOR_SF_TITLE				7 //!< 
#define COLOR_VP_LABELS				8 //!< 
#define COLOR_VP_INACTIVE			9 //!< 
#define COLOR_ARCBALL				10 //!< 
#define COLOR_ARCBALL_HILITE		11 //!< 
#define COLOR_ANIM_BUTTON			12 //!< 
#define COLOR_SEL_BOXES				13 //!< 
#define COLOR_LINK_LINES			14 //!< 
#define COLOR_TRAJECTORY			15 //!< 
#define COLOR_ACTIVE_AXIS			16 //!< 
#define COLOR_INACTIVE_AXIS			17 //!< 
#define COLOR_SPACE_WARPS			18 //!< 
#define COLOR_DUMMY_OBJ				19 //!< 
#define COLOR_POINT_OBJ				20 //!< 
#define COLOR_POINT_AXES			21 //!< 
#define COLOR_TAPE_OBJ				22 //!< 
#define COLOR_BONES					23 //!< 
#define COLOR_GIZMOS				24 //!< 
#define COLOR_SEL_GIZMOS			25 //!< 
#define COLOR_SPLINE_VECS			26 //!< 
#define COLOR_SPLINE_HANDLES		27 //!< 
#define COLOR_PATCH_LATTICE			28 //!< No longer used
#define COLOR_PARTICLE_EM			29 //!< 
#define COLOR_CAMERA_OBJ			30 //!< 
#define COLOR_CAMERA_CONE			31 //!< 
#define COLOR_CAMERA_HORIZ			32 //!< 
#define COLOR_NEAR_RANGE			33 //!< 
#define COLOR_FAR_RANGE				34 //!< 
#define COLOR_LIGHT_OBJ				35 //!< 
#define COLOR_TARGET_LINE			36 //!< 
#define COLOR_HOTSPOT				37 //!< 
#define COLOR_FALLOFF				38 //!< 
#define COLOR_START_RANGE			39 //!< 
#define COLOR_END_RANGE				40 //!< 
#define COLOR_VIEWPORT_BKG			41 //!< 
#define COLOR_TRAJ_TICS_1			42 //!< 
#define COLOR_TRAJ_TICS_2			43 //!< 
#define COLOR_TRAJ_TICS_3			44 //!< 
#define COLOR_GHOST_BEFORE			45 //!< 
#define COLOR_GHOST_AFTER			46 //!< 
#define COLOR_12FIELD_GRID			47 //!< 
#define COLOR_START_RANGE1			48 //!< 
#define COLOR_END_RANGE1			49 //!< 
#define COLOR_CAMERA_CLIP  			50 //!< 
#define COLOR_NURBS_CV				51 //!< 
#define COLOR_NURBS_LATTICE			52 //!< 
#define COLOR_NURBS_CP				53 //!< 
#define COLOR_NURBS_FP				54 //!< 
#define COLOR_NURBS_DEP				55 //!< 
#define COLOR_NURBS_ERROR			56 //!< 
#define COLOR_NURBS_HILITE			57 //!< 
#define COLOR_NURBS_FUSE			58 //!< 
#define COLOR_END_EFFECTOR			59 //!< 
#define COLOR_END_EFFECTOR_STRING	60 //!< 
#define COLOR_JOINT_LIMIT_SEL		61 //!< 
#define COLOR_JOINT_LIMIT_UNSEL		62 //!< 
#define COLOR_IK_TERMINATOR			63 //!< 
#define COLOR_SF_USER				64 //!< 
#define COLOR_VERT_TICKS			65 //!< 
#define COLOR_XRAY					66 //!< 
#define COLOR_GROUP_OBJ				67 //!< 
#define COLOR_MANIPULATOR_X			68 //!< 
#define COLOR_MANIPULATOR_Y			69 //!< 
#define COLOR_MANIPULATOR_Z			70 //!< 
#define COLOR_MANIPULATOR_ACTIVE	71 //!< 
#define COLOR_VPT_CLIPPING			72 //!< 
#define COLOR_DECAY_RADIUS			73 //!< 
#define COLOR_VERT_NUMBERS			74 //!< 
#define COLOR_CROSSHAIR_CURSOR		75 //!< 

#define COLOR_SV_WINBK              76 //!< SV Window Background
#define COLOR_SV_NODEBK             77 //!< SV Default Node Background
#define COLOR_SV_SELNODEBK          78 //!< SV Selected Node Background
#define COLOR_SV_NODE_HIGHLIGHT     79 //!< SV Viewport Selected Node Highlight
#define COLOR_SV_MATERIAL_HIGHLIGHT 80 //!< SV MEDIT Selected Node Highlight
#define COLOR_SV_MODIFIER_HIGHLIGHT 81 //!< SV Selected Modifier Highlight
#define COLOR_SV_PLUGIN_HIGHLIGHT   82 //!< SV Plug-in Highlight
#define COLOR_SV_SUBANIM_LINE       83 //!< SV Subanim line color
#define COLOR_SV_CHILD_LINE         84 //!< SV Child node line color
#define COLOR_SV_FRAME              85 //!< SV Frame color
#define COLOR_SV_SELTEXT            86 //!< SV Selected Label Color
#define COLOR_SV_TEXT               87 //!< SV Label Color

#define COLOR_UNSEL_TAB				88
#define COLOR_ATMOS_APPARATUS		89	//!< 
#define COLOR_SUBSELECTION_HARD		90  //!< 
#define COLOR_SUBSELECTION_MEDIUM	91  //!< 
#define COLOR_SUBSELECTION_SOFT		92  //!< 

#define COLOR_SV_UNFOLD_BUTTON      93 //!< SV Unfold Button
#define COLOR_SV_GEOMOBJECT_BK      94 //!< Geometry Object Node Background
#define COLOR_SV_LIGHT_BK           95 //!< Light Node Background
#define COLOR_SV_CAMERA_BK          96 //!< Camera Node Background
#define COLOR_SV_SHAPE_BK           97 //!< Shape Node Background
#define COLOR_SV_HELPER_BK          98 //!< Helper Node Background
#define COLOR_SV_SYSTEM_BK          99 //!< System Node Background
#define COLOR_SV_CONTROLLER_BK     100 //!< Controller Node Background
#define COLOR_SV_MODIFIER_BK       101 //!< Modifier Node Background
#define COLOR_SV_MATERIAL_BK       102 //!< Material Node Background
#define COLOR_SV_MAP_BK            103 //!< Map Node Background
#define COLOR_SETKEY_BUTTON        104

#define COLOR_BACK_LINES           105 //!< Backface lines on selected objects
#define COLOR_BACK_VERTS           106 //!< Backface vertices on selected objects

#define COLOR_MANIPULATOR_CONTOUR  107 //!< Background color for the rotation gizmo
#define COLOR_MANIPULATOR_SCREEN   108 //!< screen space manipulator handle color for the rotation gizmo
#define COLOR_MANIPULATOR_TRAIL    109 //!< move gizmo displacement trail color
//@}

const int kColorNormalsUnspecified = 110;
const int kColorNormalsSpecified = 111;
const int kColorNormalsExplicit = 112;

/*! \addtogroup viewportDrawingColors */
//@{
#define COLOR_SV_GRID              113 //!< SV Grid
#define COLOR_SV_REL_INSTANCE      114 //!< SV Relationship Instances
#define COLOR_SV_REL_CONSTRAINT    115 //!< SV Relationship Contraints
#define COLOR_SV_REL_PARAMWIRE     116 //!< SV Relationship Param Wires
#define COLOR_SV_REL_LIGHT         117 //!< SV Relationship Lights
#define COLOR_SV_REL_MODIFIER      118 //!< SV Relationship Modifiers
#define COLOR_SV_REL_CONTROLLER    119 //!< SV Relationship Controllers
#define COLOR_SV_REL_OTHER         120 //!< SV Relationship Others
#define COLOR_SV_SPACEWARP_BK      121 //!< SV SpaceWarp
#define COLOR_SV_BASEOBJECT_BK     122 //!< SV BaseObject

#define COLOR_VP_STATISTICS			123	//!< Colour for viewport statistics display

#define COLOR_SPLINE_KNOT_UNSELECTED		124  //!< color of bezier knots unselected
#define COLOR_SPLINE_KNOT_FIRST				125  //!< color of first knot of the bezier spline
#define COLOR_HIDDENLINE_UNSELECTED			126  //!< color of first knot of the bezier spline
#define COLOR_VP_LABEL_HIGHLIGHT			127  //!< Color of selected/mouse over viewport labels

#define COLOR_TOTAL                128	//!< always the max number of colors
//@}

// Returns/sets color values for drawing in the viewport (selection, subsel, etc)
DllExport Point3 GetUIColor(int which);
DllExport void SetUIColor(int which, Point3 *clr);
DllExport Point3 GetDefaultUIColor(int which);

#define GetSelColor()		GetUIColor(COLOR_SELECTION)
/*! \remarks Returns the color used for sub-object selection. */
#define GetSubSelColor()	GetUIColor(COLOR_SUBSELECTION)
#define GetFreezeColor()	GetUIColor(COLOR_FREEZE)

//! Internal Use only
//! NH Flags used to tell Max graphics drivers that the user has returned from various system events
//! Internal Use only - resolves issues with DirectX - the flags are used with GraphicsWindow::setFlags
#define SYSTEM_LOCK_RETURN 0x001
#define SYSTEM_REMOTE_RETURN 0x002




#endif // _GFX_H_

