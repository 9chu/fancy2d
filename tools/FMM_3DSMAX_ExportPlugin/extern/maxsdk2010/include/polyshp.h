/**********************************************************************
 *<
	FILE: polyshp.h

	DESCRIPTION: Polyline shape methods

	CREATED BY: Tom Hudson

	HISTORY: Created 3 October 1995

 *> Copyright (c) 1995, All Rights Reserved.
 **********************************************************************/

#ifndef __POLYSHP_H_

#define __POLYSHP_H_

#include "maxheap.h"
#include "shphier.h"
#include "captypes.h"
#include "shpsels.h"	// Shape selection classes
#include "templt.h"

// Display flags
#define DISP_VERTTICKS		(1<<0)
//#define DISP_BEZHANDLES		(1<<1)
#define DISP_SELVERTS		(1<<10)
#define DISP_SELSEGMENTS	(1<<11)
#define DISP_SELPOLYS		(1<<13)

// Snap flags
#define PSHAPE_SNAP_IGNORELAST	(1<<0)
#define PSHAPE_SNAP_NOEDGES		(1<<1)

// Selection level bits.
#define SHAPE_OBJECT		(1<<0)
#define SHAPE_SPLINE		(1<<1)
#define SHAPE_SEGMENT		(1<<2)
#define SHAPE_VERTEX		(1<<3)

// Parameter types for shape interpolation (Must match interpolation types in object.h)
#define POLYSHP_INTERP_SIMPLE 0		// Parameter space based on segments
#define POLYSHP_INTERP_NORMALIZED 1	// Parameter space normalized to curve length

// Flags for sub object hit test

// NOTE: these are the same bits used for object level.
#define SUBHIT_SHAPE_SELONLY	(1<<0)
#define SUBHIT_SHAPE_UNSELONLY	(1<<2)
#define SUBHIT_SHAPE_ABORTONHIT	(1<<3)
#define SUBHIT_SHAPE_SELSOLID	(1<<4)

#define SUBHIT_SHAPE_VERTS		(1<<24)
#define SUBHIT_SHAPE_SEGMENTS	(1<<25)
#define SUBHIT_SHAPE_POLYS		(1<<26)
#define SUBHIT_SHAPE_TYPEMASK	(SUBHIT_SHAPE_VERTS|SUBHIT_SHAPE_SEGMENTS|SUBHIT_SHAPE_POLYS)

class Spline3D;

//--------------------------------------------------------------
// Capping classes, etc.

// CapVert flags
#define CAPVERT_VISEDGE (1<<0)

class CapVert: public MaxHeapOperators {
	public:
		int vert;
		DWORD flags;
		float ang;
		CapVert *prev,*next;
		CapVert *smaller,*bigger;
		CapVert() { vert=0; flags = 0; ang = 0.0f; prev=next=smaller=bigger = NULL; }
	};

//--------------------------------------------------------------

// Predefined PolyPt flags
// Bits 0-7 are available to the user.  Bits 8-31 are reserved for internal use

// Use these to make capping more efficient
// If your class converts to a PolyLine, use them!
#define POLYPT_KNOT 		(1<<8)	// A control point
#define POLYPT_INTERPOLATED (1<<9)	// An interpolated point

// If you convert to a PolyLine, use this bit to control smoothing of the resulting shape
// If this bit is set, it means that any mesh generated will share smoothing across the edge
#define POLYPT_SMOOTH		(1<<10) // Point is part of a smooth transition

#define POLYPT_SEG_SELECTED	(1<<11)	// The segment that starts with this point is selected

// Used internally by capping code
#define POLYPT_BRIDGE		(1<<16)	// Span between two polygons
#define POLYPT_SPLICE		(1<<17)	// Point is endpoint of a bridge
#define POLYPT_VISEDGE		(1<<18)	// Segment should be visible on mesh
#define POLYPT_NO_SPLICE	(1<<19)	// Don't allow a bridge at this point
#define POLYPT_INVIS_EDGE	(1<<20)	// Force segment to be invisible on capping
#define POLYPT_NO_SNAP		(1<<21) // Suppress snapping when set

// Flags2 field contains material IDs:
// The mat ID is stored in the HIWORD of the PolyPt flags2 field
#define POLYPT_MATID_SHIFT	16
#define POLYPT_MATID_MASK	0xFFFF

/*! \sa  Class PolyLine.\n\n
\par Description:
This class represents a single point of a <b>PolyLine</b>. All methods of this
class are implemented by the system.
\par Data Members:
<b>Point3 p;</b>\n\n
The location of the point.\n\n
<b>DWORD flags;</b>\n\n
Predefined <b>PolyPt</b> flags. Note that developers can use bits 0-7 of the
flags for their own use. Bits 8 and above are used by the system.\n\n
If you are converting some other type of shape or spline to a <b>PolyShape</b>
(and thus <b>PolyLines</b>) you can set certain flags to make things operate
smoother. These flags are described below:\n\n
<b>POLYPT_KNOT</b>\n\n
This indicates if this point in the <b>PolyLine</b> corresponds to a knot in
the original spline. For example if you had a circle that was a bezier spline
it should have four of the points in the <b>PolyLine</b> designated as
<b>POLYPT_KNOT</b>. These are the points at the 12 o'clock, 3 o'clock, 6
o'clock and 9 o'clock positions. Then all the other points would be
<b>POLYPT_INTERPOLATED</b>. This is used to make capping more efficient, for
example, the system generally tries to attach to a knot when making connections
between polygons.\n\n
<b>POLYPT_INTERPOLATED</b>\n\n
This indicates the point is an interpolated point (not a knot).\n\n
<b>POLYPT_SMOOTH</b>\n\n
If you convert to a <b>PolyLine</b>, use this bit to control smoothing of the
resulting shape. If this bit is set, it means that any mesh generated will
share smoothing across the edge. For example, all the points on a curved
section of a spline between knots will have this flag set. Then depending on
the knot type (if it's a corner or bezier corner) then this smooth flag will
not be set. If this shape is then extruded or lofted this information is used
to determine smoothing groups. If this flag is set you'll get a smooth
transition however if this bit is not set you'll get a nice sharp corner.\n\n
<b>POLYPT_SEG_SELECTED</b>\n\n
The segment that starts with this point is selected. This is used in the
drawing routines so that PolyShapes generated from BezierShapes will still have
the selected segments drawn in the selected color.\n\n
<b>POLYPT_BRIDGE</b>\n\n
This flag is used internally by the capping code.\n\n
<b>POLYPT_SPLICE</b>\n\n
This flag is used internally by the capping code.\n\n
<b>POLYPT_VISEDGE</b>\n\n
This flag is used internally by the capping code.\n\n
<b>POLYPT_NO_SPLICE</b>\n\n
This flag is used internally by the capping code.\n\n
<b>int aux;</b>\n\n
Auxiliary data attached to this point (usually a mesh vertex number for
capping).\n\n
<b>DWORD flags2;</b>\n\n
This flag contains the material information for the segments. The mat ID is
stored in the HIWORD.\n\n
<b>#define POLYPT_MATID_SHIFT 16</b>\n\n
<b>#define POLYPT_MATID_MASK  0xFFFF</b>  */
class PolyPt: public MaxHeapOperators {
	public:
		Point3 p;
		DWORD flags;	// See above
		DWORD flags2;	// See above
		int aux;		// Auxiliary data attached to this point (usually mesh vertex number for capping)
		/*! \remarks Constructor. The point <b>p</b> is set to 0,0,0,
		<b>flags</b> and <b>flags2</b> are set to 0, <b>aux</b> is set to 0. */
		PolyPt() { p = Point3(0,0,0); flags = 0; flags2 = 0; aux = 0; }
		/*! \remarks Constructor. The point, flags, aux and flags2 data
		members are initialized to the data passed. */
		PolyPt(Point3 ip, DWORD f=0, int a=0, DWORD f2=0) { p = ip; flags = f; aux = a; flags2 = f2;}
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		This method gets the material ID on a per-segment basis within the
		spline or polyline. */
		inline	MtlID	GetMatID() {return (int)((flags2>>POLYPT_MATID_SHIFT)&POLYPT_MATID_MASK);}
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		This method sets the material ID on a per-segment basis within the
		spline or polyline.
		\par Parameters:
		<b>MtlID id</b>\n\n
		The ID to set. */
		inline	void    SetMatID(MtlID id) {flags2 &= 0xFFFF; flags2 |= (DWORD)(id<<POLYPT_MATID_SHIFT);}
	};

// PolyLine::Cap3DS / PolyShape::Make3DSCap options
#define CAP3DS_OPT_CLOSEST_BRIDGE (1<<0)	// Bridge polys at closest point

// PolyLine flags
#define POLYLINE_CLOSED			(1<<0)
#define POLYLINE_NO_SELF_INT	(1<<1)		// Ignore self-intersections (special!)

/*! \sa  Class PolyPt, Class PolyShape.\n\n
\par Description:
This class describes a single polygon in a <b>PolyShape</b> using linear
segments. All methods of this class are implemented by the system.
\par Method Groups:
See <a href="class_poly_line_groups.html">Method Groups for Class PolyLine</a>.
\par Data Members:
<b>int numPts;</b>\n\n
The number of points in the polyline.\n\n
<b>PolyPt *pts;</b>\n\n
The points themselves.\n\n
<b>DWORD flags;</b>\n\n
PolyLine flags. One or more of the following values:\n\n
<b>POLYLINE_CLOSED</b>\n\n
Indicates the polyline is closed.\n\n
<b>POLYLINE_NO_SELF_INT</b>\n\n
Ignore self-intersections. This is used internally in the patch capping to
indicate that this line always returns FALSE from the self intersects test.
Normally this should not be set.\n\n
<b>Box3 bdgBox;</b>\n\n
The bounding box of the polyline.\n\n
<b>float cachedLength;</b>\n\n
The length of the polyline.\n\n
<b>float *lengths;</b>\n\n
Cached lengths for each point\n\n
<b>float *percents;</b>\n\n
Cached percentages for each point\n\n
<b>BOOL cacheValid;</b>\n\n
Indicates if the cache is valid.  */
class PolyLine: public MaxHeapOperators {
	public:
		int numPts;
		PolyPt *pts;
		DWORD flags;
		Box3 bdgBox;
		float cachedLength;
		float *lengths;		// Cached lengths for each point
		float *percents;	// Cached percentages for each point
		BOOL cacheValid;
		/*! \remarks Constructor. The number of points is set to zero, the
		points array is set to NULL, the flags are set to 0, the cache validity
		is set to FALSE and the bounding box is set to empty. */
		CoreExport PolyLine();
		/*! \remarks Constructor. The <b>PolyLine</b> is initialized using
		<b>from</b>. */
		CoreExport PolyLine(PolyLine& from);
		/*! \remarks Destructor. The array of points is freed. */
		CoreExport ~PolyLine();
		/*! \remarks Initializes the PolyLine. The <b>pts</b> array is freed,
		the number of points is set to 0, and the cache validity is set to
		FALSE. */
		CoreExport void Init();
		/*! \remarks Call this method to indicate the polyline is closed. */
		void Close() { flags |= POLYLINE_CLOSED; }
		/*! \remarks Returns TRUE if the polyline is closed; otherwise FALSE.
		*/
		CoreExport BOOL IsClosed();
		/*! \remarks Call this method to indicate the polyline is open. */
		void Open() { flags &= ~POLYLINE_CLOSED; }
		/*! \remarks Returns TRUE if the polyline is open; otherwise FALSE. */
		CoreExport BOOL IsOpen();
		/*! \remarks Call this method to set the self intersects flag. This is
		used in the patch capping to indicate that this line always returns
		FALSE from the self intersects test. Normally this should not be set.
		*/
		void SetNoSelfInt() { flags |= POLYLINE_NO_SELF_INT; }
		/*! \remarks Returns TRUE if the no self intersect flag is set;
		otherwise FALSE. */
		BOOL IsNoSelfInt() { return (flags & POLYLINE_NO_SELF_INT) ? TRUE : FALSE; }
		/*! \remarks Returns the number of vertices (points) in the polyline.
		*/
		int Verts() { return numPts; }
		/*! \remarks Returns the number of segments (edges between vertices)
		of the polyline. */
		CoreExport int Segments();
		/*! \remarks Sets the number of points in the polyline.
		\par Parameters:
		<b>int count</b>\n\n
		The number to set.\n\n
		<b>BOOL keep = TRUE</b>\n\n
		If TRUE any existing points are copied to the new array; otherwise they
		are freed.
		\return  TRUE if the number of points was set; otherwise FALSE. */
		CoreExport BOOL SetNumPts(int count, BOOL keep = TRUE);
		/*! \remarks Appends the specified point to the polyline. This adds
		the point to the end of the points list.
		\par Parameters:
		<b>PolyPt\& p</b>\n\n
		The point to append. */
		CoreExport void Append(PolyPt& p);
		/*! \remarks Inserts the specified point at the location passed.
		\par Parameters:
		<b>int where</b>\n\n
		The <b>pts</b> array index indicating where to insert the point.\n\n
		<b>PolyPt\& p</b>\n\n
		The point to insert. */
		CoreExport void Insert(int where, PolyPt& p);
		/*! \remarks Deletes the specified point.
		\par Parameters:
		<b>int where</b>\n\n
		The <b>pts</b> array index indicating which point to delete. */
		CoreExport void Delete(int where);
		/*! \remarks Reverses the order of the points in the polyline.
		\par Parameters:
		<b>BOOL keepZero=FALSE</b>\n\n
		This should normally be passed as TRUE. If TRUE, and the polyline is
		closed, this method will make sure that vertex zero is the same on the
		reversed version as on the non-reversed version. Otherwise if passed as
		FALSE the last vertex becomes the first vertex, and the first vertex
		becomes the last. This is an important distinction for the lofter
		because it always wants vertex zero to be in the same place. */
		CoreExport void Reverse(BOOL keepZero=FALSE);
		/*! \remarks Assignment operator. */
		CoreExport PolyLine& operator=(PolyLine& from);
		/*! \remarks Assignment operator. This generates a polyline from the
		spline. */
		CoreExport PolyLine& operator=(Spline3D& from);
		/*! \remarks Array access operator. Returns the specified point in the
		<b>pts</b> array. */
		CoreExport PolyPt& operator[](int index) { return pts[index]; }
		/*! \remarks Computes the bounding box of the polyline. The result is
		stored in the <b>bdgBox</b> data member. */
		CoreExport void BuildBoundingBox(void);
		/*! \remarks This method makes sure the <b>PolyLine</b> has flushed
		out any cached data it may have had. This resets the bounding box size
		and sets the cache validity to FALSE. This should be called when any
		points have been changed in the polyline. */
		CoreExport void InvalidateGeomCache();
		/*! \remarks Returns the bound box of the polyline. The optional TM
		allows the box to be calculated in any space.
		\par Parameters:
		<b>Matrix3 *tm=NULL</b>\n\n
		The points of the polyline are multiplied by this matrix before the box
		computation. */
		CoreExport Box3 GetBoundingBox(Matrix3 *tm=NULL); // RB: optional TM allows the box to be calculated in any space.
		/*! \remarks This method is used internally. */
		CoreExport void Render(GraphicsWindow *gw, Material *ma, RECT *rp, int compFlags, int numMat);

		// New for R4: To render a proxy, colorSegs=TRUE and numMat=-1
		//! \brief This renders the polyline to the display
		/*!	This renders the polyline to the display.
		\param[in] gw Points to the graphics window to render to.
		\param[in] ma The list of materials to use to render the polyline. 
		See Class Material, Class INode - Material methods.
		\param[in] numMat The number of materials for the polyline.
		\param[in] colorSegs if this is set material is used to determine what colors 
		to render the polyline. Otherwise the standard sel/subsel colors are used
		\param[in] segsel this is the segment sel bitarray used to draw the selected segments
		\param[in] useStartSegments = TRUE if this is set the start/endsegmens will be used 
		around the polyline draw.  If you are drawing several polylines in a row it is 
		faster to set this to false and use start/endsegments around the entire draw 
		instead of each individual polyline
		*/
		CoreExport void Render(GraphicsWindow *gw, Material *ma, int numMat, BOOL colorSegs, BitArray &segsel, BOOL useStartSegments = TRUE);
		/*! \remarks This method is used internally. */
		CoreExport BOOL Select(GraphicsWindow *gw, Material *ma, HitRegion *hr, int abortOnHit = FALSE);
		/*! \remarks This method is used internally. */
		CoreExport void Snap(GraphicsWindow *gw, SnapInfo *snap, IPoint2 *p, Matrix3 &tm, DWORD flags);
		/*! \remarks Transforms the points of the polyline by the specified matrix.
		\par Parameters:
		<b>Matrix3 \&tm</b>\n\n
		The matrix to transform the points. */
		CoreExport void Transform(Matrix3 &tm);
		/*! \remarks You may call this method to dump the polyline structure
		via <b>DebugPrint()</b>. See
		<a href="ms-its:3dsmaxsdk.chm::/debug_debugging.html">Debugging</a>.
		\par Parameters:
		<b>MCHAR *title = NULL</b>\n\n
		This title string is displayed using a <b>DebugPrint()</b> before the
		rest of the data. */
		CoreExport void Dump(MCHAR *title = NULL);
		/*! \remarks This method is used internally as part of the capping
		mechanism and should not be used. */
		CoreExport void SpliceLine(int where, PolyLine &source, int splicePoint);
		/*! \remarks Returns TRUE if the line segment defined between points
		<b>p1</b> and <b>p2</b> intersects this <b>PolyLine</b>; otherwise
		FALSE.
		\par Parameters:
		<b>Point2 p1, Point2 p2</b>\n\n
		The endpoints of the line to check.\n\n
		<b>BOOL findAll=FALSE</b>\n\n
		TRUE to find all intersections. FALSE to find only the first
		intersection.\n\n
		<b>IntersectionCallBack3D *cb = NULL</b>\n\n
		A pointer to an IntersectionCallback3D class. */
		CoreExport BOOL HitsSegment(Point2 p1, Point2 p2, BOOL findAll = FALSE, IntersectionCallback3D *cb = NULL);
		/*! \remarks This method is used internally. */
		CoreExport int Cap3DS(CapVert *capverts, MeshCapInfo &capInfo, DWORD options = 0);
		/*! \remarks Returns TRUE if the specified <b>PolyLine</b> intersects
		this PolyLine; otherwise FALSE.
		\par Parameters:
		<b>PolyLine \&line</b>\n\n
		The PolyLine to check\n\n
		<b>BOOL findAll=FALSE</b>\n\n
		TRUE to find all intersections. FALSE to find only the first
		intersection.\n\n
		<b>IntersectionCallBack3D *cb = NULL</b>\n\n
		A pointer to an IntersectionCallback3D class. */
		CoreExport BOOL HitsPolyLine(PolyLine &line, BOOL findAll = FALSE, IntersectionCallback3D *cb = NULL);
		/*! \remarks Returns TRUE if the specified point is surrounded
		(contained within) this spline. This method should only be called on
		closed <b>PolyLines</b>.
		\par Parameters:
		<b>Point2 \&point</b>\n\n
		The point to check. */
		CoreExport BOOL SurroundsPoint(Point2 &point);
		/*! \remarks This method returns a point interpolated on a segment
		between two points.
		\par Parameters:
		<b>int segment</b>\n\n
		The index of the segment to interpolate.\n\n
		<b>float t</b>\n\n
		A value in the range of 0.0 to 1.0. 0 is the first point and 1 is the
		second point.
		\return  The interpolated point. */
		CoreExport Point3 InterpPiece3D(int segment, float t);
		/*! \remarks This method returns a point interpolated on the entire
		curve. This method returns a point but you don't know which segment the
		point falls on.
		\par Parameters:
		<b>float u</b>\n\n
		A value in the range of 0.0 to 1.0 for the entire curve.\n\n
		<b>int ptype=POLYSHP_INTERP_SIMPLE</b>\n\n
		This parameter is available in release 2.0 and later only.\n\n
		The parameter type for interpolation. The default value is based on
		segments (rather than the entire curve). See
		<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_ptypes_for_polyline.html">List of Parameter
		Types for PolyLine Interpolation</a>.
		\return  The interpolated point. */
		CoreExport Point3 InterpCurve3D(float u, int ptype=POLYSHP_INTERP_SIMPLE);
		/*! \remarks This method returns a tangent vector interpolated on a
		segment between two points.
		\par Parameters:
		<b>int segment</b>\n\n
		The index of the segment.\n\n
		<b>float t</b>\n\n
		A value in the range of 0.0 to 1.0. 0 is the first point and 1 is the
		second point.
		\return  The tangent vector. */
		CoreExport Point3 TangentPiece3D(int segment, float t);
		/*! \remarks This method returns a tangent vector interpolated on the
		entire curve.
		\par Parameters:
		<b>float u</b>\n\n
		A value in the range of 0.0 to 1.0 for the entire curve.\n\n
		<b>int ptype=POLYSHP_INTERP_SIMPLE</b>\n\n
		This parameter is available in release 2.0 and later only.\n\n
		The parameter type for interpolation. The default value is based on
		segments (rather than the entire curve). See
		<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_ptypes_for_polyline.html">List of Parameter
		Types for PolyLine Interpolation</a>.
		\return  The tangent vector. */
		CoreExport Point3 TangentCurve3D(float u, int ptype=POLYSHP_INTERP_SIMPLE);
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Returns the material ID for the specified segment.
		\par Parameters:
		<b>int segment</b>\n\n
		The zero based index of the segment. */
		CoreExport MtlID GetMatID(int segment);
		/*! \remarks Returns the length of the <b>PolyLine</b>. */
		CoreExport float CurveLength();
		/*! \remarks Returns TRUE if the polyline is clockwise in the XY plane
		(it ignores Z); otherwise FALSE. If the <b>PolyLine</b> self
		intersects, the results from this method are meaningless. */
		CoreExport BOOL IsClockWise();			// 2D!
		/*! \remarks Returns TRUE if the polyline intersects itself in the XY
		plane (it ignores Z); otherwise FALSE.
		\par Parameters:
		<b>BOOL findAll = FALSE</b>\n\n
		TRUE to find all self intersections. FALSE to find only the first self
		intersection.\n\n
		<b>IntersectionCallback3D *cb = NULL</b>\n\n
		A pointer to an IntersectionCallback3D class. */
		CoreExport BOOL	SelfIntersects(BOOL findAll = FALSE, IntersectionCallback3D *cb = NULL);		// 2D!
		CoreExport void GetSmoothingMap(IntTab &map);
		// IO
		CoreExport IOResult Save(ISave *isave);
		CoreExport IOResult Load(ILoad *iload);

		//! \brief Swaps the contents of this polyline with another polyline object.
		/*!	This method lets you transfer the content of a line object to another object
		without suffering from any copy penalties.  This is useful to transfer polylines
		to a new array, for instance.

				\param[in, out] PolyLine& line PolLine object to swap with.
		*/
		CoreExport void Swap(PolyLine& line);
	};

#define CAPFACE_AB	(1<<0)
#define CAPFACE_BC	(1<<1)
#define CAPFACE_CA	(1<<2)

class ShapeObject;

// Options for steps in MakePolyShape (>=0: Use fixed steps)
// NOTE: DO NOT change these defines -- They're also used by ShapeObject (object.h)
#define PSHAPE_BUILTIN_STEPS -2		// Use the shape's built-in steps/adaptive settings (default)
#define PSHAPE_ADAPTIVE_STEPS -1	// Force adaptive steps

/*! \sa  Class ShapeObject, Class PolyLine, Class ShapeVSel, Class ShapeSSel, Class ShapePSel, Class ShapeHierarchy.\n\n
\par Description:
A multi-polygon shape class. This class is used in the caching of bezier
shapes. This is used for doing a one time interpolation of a bezier shape into
a form that is the same shape but doesn't require any further interpolation. In
this way the system can do the complex calculations once, store the shape into
this <b>PolyShape</b> representation, and not have to go through the cubic
spline calculations to figure out where the points are in the future. This
class maintains an array of <b>PolyLines</b>.\n\n
This is used for example in the Extrude modifier. The first thing it does is
generate a <b>PolyShape</b> from the bezier shape it is extruding. This
<b>PolyShape</b> is then used to generate the mesh.
\par Method Groups:
See <a href="class_poly_shape_groups.html">Method Groups for Class PolyShape</a>.
\par Data Members:
<b>int numLines;</b>\n\n
The number of lines in the polyshape.\n\n
<b>PolyLine *lines;</b>\n\n
The array of lines.\n\n
<b>DWORD flags;</b>\n\n
These are not currently used.\n\n
<b>Box3 bdgBox;</b>\n\n
Stores the bounding box surrounding each <b>PolyLine</b> in the shape.\n\n
<b>ShapeVSel vertSel;</b>\n\n
The selected vertices.\n\n
<b>ShapeSSel segSel;</b>\n\n
The selected segments.\n\n
<b>ShapePSel polySel;</b>\n\n
The selected polygons.\n\n
<b>DWORD selLevel;</b>\n\n
The selection level. One of the following values:\n\n
<b>SHAPE_OBJECT</b> - Object level selection.\n\n
<b>SHAPE_SPLINE</b> - Spline level selection.\n\n
<b>SHAPE_SEGMENT</b> - Segment level selection.\n\n
<b>SHAPE_VERTEX</b> - Vertex level selection.\n\n
<b>DWORD dispFlags;</b>\n\n
Display attribute flags.\n\n
<b>DISP_VERTTICKS -</b> Display vertices as ticks.\n\n
<b>DISP_SELVERTS -</b> Display selected vertices.\n\n
<b>DISP_SELSEGMENTS -</b> Display selected segments.\n\n
<b>DISP_SELPOLYS -</b> Display selected polygons.\n\n
The following capping and cache information is used internally:\n\n
<b>MeshCapInfo morphCap;</b>\n\n
The morph cap information.\n\n
<b>BOOL morphCapCacheValid;</b>\n\n
Determines if the morph cap is valid.\n\n
<b>MeshCapInfo gridCap;</b>\n\n
The grid cap information.\n\n
<b>BOOL gridCapCacheValid;</b>\n\n
Determines if the grid cap is valid.\n\n
<b>PatchCapInfo patchCap;</b>\n\n
The patch cap information.\n\n
<b>BOOL patchCapCacheValid;</b>\n\n
Determines if the patch cap is valid.\n\n
<b>ShapeHierarchy cachedHier;</b>\n\n
Hierarchy cache.\n\n
<b>BOOL hierCacheValid;</b>\n\n
Determines if the hierarchy cache is valid.  */
class PolyShape: public MaxHeapOperators {
	public:
		int numLines;
		PolyLine *lines;
		DWORD flags;
		Box3 bdgBox;

 		// Selection
		ShapeVSel	vertSel;  		// selected vertices
		ShapeSSel	segSel;  		// selected segments
		ShapePSel	polySel;  		// selected polygons

		// Selection level
		DWORD		selLevel;

		// Display attribute flags
		DWORD		dispFlags;

		// Capping caches
		MeshCapInfo morphCap;
		BOOL morphCapCacheValid;
		MeshCapInfo gridCap;
		BOOL gridCapCacheValid;
		PatchCapInfo patchCap;
		BOOL patchCapCacheValid;

		// Hierarchy cache
		ShapeHierarchy cachedHier;
		BOOL hierCacheValid;

		/*! \remarks Constructor. The number of lines is set to 0, the lines
		array is set to NULL, the flags and display flags are set to 0, the
		selection level is set to SHAPE_OBJECT, the geometry cache is
		invalidated, and the masterObject is set to NULL. */
		CoreExport PolyShape();
		/*! \remarks Constructor. The <b>PolyShape</b> is initialized from the
		specified <b>PolyShape</b>. */
		CoreExport PolyShape(PolyShape& from);
		/*! \remarks Destructor. */
		CoreExport ~PolyShape();
		/*! \remarks <b>PolyShape::Init()</b> is a special version used by the
		constructors and should not be called by plug-in developers. If you
		need to clear out a <b>PolyShape</b> use <b>NewShape()</b> described
		below. */
		CoreExport void Init();			// Used by constructors
		/*! \remarks This deletes all the lines from the <b>PolyShape</b> and
		clears the shape out. Use this method instead of <b>Init()</b> above.
		*/
		CoreExport void NewShape();		// Deletes all lines
		/*! \remarks Sets the number of polygons used by the poly shape.
		\par Parameters:
		<b>int count</b>\n\n
		The number of lines.\n\n
		<b>BOOL keep = TRUE</b>\n\n
		If TRUE any old lines are copied to the new storage; otherwise they are
		freed.
		\return  TRUE if the number of lines were set; otherwise FALSE. */
		CoreExport BOOL SetNumLines(int count, BOOL keep = TRUE);
		/*! \remarks Creates a new <b>PolyLine</b> and appends it to the end
		of the list of lines maintained by this <b>PolyShape</b>.
		\return  The address of the newly added line. */
		CoreExport PolyLine* NewLine();
		/*! \remarks Appends the specified <b>PolyLine</b> to the end of the
		lines list.
		\par Parameters:
		<b>PolyLine \&l</b>\n\n
		The <b>PolyLine</b> to append. */
		CoreExport void Append(PolyLine &l);
		/*! \remarks Inserts the specified <b>PolyLine</b> at the location
		passed.
		\par Parameters:
		<b>int where</b>\n\n
		The index into the lines list specifying where to insert the
		<b>PolyLine</b>.\n\n
		<b>PolyLine\& l</b>\n\n
		The <b>PolyLine</b> to insert. */
		CoreExport void Insert(int where, PolyLine& l);
		/*! \remarks Deletes the specified <b>PolyLine</b> from the lines
		list.
		\par Parameters:
		<b>int where</b>\n\n
		The index into the lines list specifying which line to delete. */
		CoreExport void Delete(int where);
		/*! \remarks Assignment operator. */
		CoreExport PolyShape& operator=(PolyShape& from);
		/*! \remarks Assignment operator. Note that this operator does not
		offer as much control as calling the method on the <b>BezierShape</b>
		itself named <b>MakePolyShape()</b>. That version allows you to specify
		the number of steps and an optimize parameter. These options are not
		available on this simple assignment operator. */
		CoreExport PolyShape& operator=(BezierShape& from);
		/*! \remarks Builds a bounding box surrounding every line in the lines
		list. The bounding box is returned through the <b>bdgBox</b> data
		member. */
		CoreExport void BuildBoundingBox(void);
		/*! \remarks Invalidates the cache of each line in the
		<b>PolyShape</b>. The bounding box is set to empty. This method also
		invalidates the capping caches.
		\par Parameters:
		<b>BOOL unused</b>\n\n
		This parameter is not used. */
		CoreExport void InvalidateGeomCache(BOOL unused);		// Also invalidates capping caches
		/*! \remarks Invalidates the morph, and grid cap caches. */
		CoreExport void InvalidateCapCache();
		/*! \remarks Returns the bounding box of the <b>PolyLines</b> in this
		<b>PolyShape</b>. The optional TM allows the box to be calculated in
		any space.
		\par Parameters:
		<b>Matrix3 *tm=NULL</b>\n\n
		The points of the <b>PolyLines</b> in this <b>PolyShape</b> are
		transformed by this matrix prior to the bounding box computations. */
		CoreExport Box3 GetBoundingBox(Matrix3 *tm=NULL); // RB: optional TM allows the box to be calculated in any space.
		/*! \remarks Computes the bounding box of this <b>PolyShape</b>.
		\par Parameters:
		<b>TimeValue t</b>\n\n
		The time at which to evaluate the bounding box.\n\n
		<b>Box3\& box</b>\n\n
		The result is stored here.\n\n
		<b>Matrix3 *tm</b>\n\n
		The points of each <b>PolyLine</b> in this <b>PolyShape</b> are
		deformed using this matrix.\n\n
		<b>BOOL useSel</b>\n\n
		If TRUE the box is computed about the selected vertices only; otherwise	all points. */
		CoreExport void GetDeformBBox(TimeValue t, Box3& box, Matrix3 *tm, BOOL useSel );
		/*! \remarks This method is used internally. */
		CoreExport void Render(GraphicsWindow *gw, Material *ma, RECT *rp, int compFlags, int numMat);
		/*! \remarks This method is used internally. */
		CoreExport BOOL Select(GraphicsWindow *gw, Material *ma, HitRegion *hr, int abortOnHit = FALSE);
		/*! \remarks This method is used internally. */
		CoreExport void Snap(GraphicsWindow *gw, SnapInfo *snap, IPoint2 *p, Matrix3 &tm);
		/*! \remarks This method is used internally. */
		CoreExport void Snap(GraphicsWindow *gw, SnapInfo *snap, IPoint2 *p, Matrix3 &tm, DWORD flags);
		/*! \remarks Transforms the vertices of each PolyLine in this
		PolyShape by the specified matrix.
		\par Parameters:
		<b>Matrix3 \&tm</b>\n\n
		The transformation matrix. */
		CoreExport void Transform(Matrix3 &tm);
		/*! \remarks This method may be called to fill in the MeshCapInfo
		passed with the appropriate capping information. See
		<a href="ms-its:3dsmaxsdk.chm::/patches_root.html">Working with
		Patches</a> for more details on capping. This method is used for
		meshes. The method below is used for patches.
		\par Parameters:
		<b>TimeValue t</b>\n\n
		This should be passed as the current time. You may retrieve this using
		<b>Interface::GetTime()</b>. 
		See Class Interface.\n\n
		<b>MeshCapInfo \&capInfo</b>\n\n
		This information is filled in by this method. See
		Class MeshCapInfo.\n\n
		<b>int capType</b>\n\n
		The cap type. See \ref capTypes</a>.
		\return  Nonzero if the cap info was set up successfully; otherwise
		zero. */
		CoreExport int MakeCap(TimeValue t, MeshCapInfo &capInfo, int capType);
		/*! \remarks This method may be called to fill in the
		<b>PatchCapInfo</b> passed with the appropriate capping information.
		This method is used for patches. Note that it is generally not
		recommended to try to create patch caps from <b>PolyShapes</b>. The
		patch cap uses bezier information, so it is much better to use a
		<b>BezierShape</b> to make a patch cap. It is very inefficient to do
		this with a <b>PolyShape</b>.
		\par Parameters:
		<b>TimeValue t</b>\n\n
		This should be passed as the current time. You may retrieve this using
		<b>Interface::GetTime()</b>. 
		See Class Interface.\n\n
		<b>PatchCapInfo \&capInfo</b>\n\n
		This information is filled in by this method. See
		Class PatchCapInfo.
		\return  Nonzero if the cap info was set up successfully; otherwise
		zero. */
		CoreExport int MakeCap(TimeValue t, PatchCapInfo &capInfo);
		/*! \remarks This method is used internally. */
		CoreExport int Make3DSCap(MeshCapInfo &capInfo, DWORD options = 0);
		/*! \remarks This method is used internally. */
		CoreExport int MakeGridCap(MeshCapInfo &capInfo);
		/*! \remarks This may be called to display information about the
		PolyShape to the debug window via <b>DebugPrint()</b>. See
		<a href="ms-its:3dsmaxsdk.chm::/debug_debugging.html">Debugging</a>.
		\par Parameters:
		<b>MCHAR *title = NULL</b>\n\n
		A string to be display before the <b>PolyShape</b> data is displayed.
		*/
		CoreExport void Dump(MCHAR *title = NULL);
		/*! \remarks This method is used internally. */
		CoreExport void UpdateCachedHierarchy();
		/*! \remarks This methods looks at the shape organization, and puts
		together a shape hierarchy. This provides information on how the shapes
		are nested. For example on a donut object with two circles, this method
		determines which circle is inside the other one.
		\par Parameters:
		<b>TimeValue t</b>\n\n
		This parameter is not used.\n\n
		<b>ShapeHierarchy *hier = NULL</b>\n\n
		If non-NULL the result is store here (in addition to being returned).
		See Class ShapeHierarchy.
		\return  The result is store here. */
		CoreExport ShapeHierarchy &OrganizeCurves(TimeValue t, ShapeHierarchy *hier = NULL);
		/*! \remarks This is a very important call to make. Whenever you have
		changed the <b>PolyShape</b>, for example after you are done adding
		polygons to the shape or have changed the number of points in a shape,
		you should call this method. This method updates a set of embedded
		selection set data within the shape. This selection set data tells what
		polygons are selected, what segments are selected, and what control
		points are selected. This resets the sizes of the selection bit arrays
		for this shape. */
		CoreExport void UpdateSels();
		/*! \remarks Reverses the <b>PolyLine</b> whose index is passed.
		\par Parameters:
		<b>int poly</b>\n\n
		The spline to reverse.\n\n
		<b>BOOL keepZero=FALSE</b>\n\n
		This should normally be passed as TRUE. If TRUE, and the polyline is
		closed, this method will make sure that vertex zero is the same on the
		reversed version as on the non-reversed version. Otherwise if passed as
		FALSE the last vertex becomes the first vertex, and the first vertex
		becomes the last. This is an important distinction for the lofter
		because it always wants vertex zero to be in the same place. */
		CoreExport void Reverse(int poly, BOOL keepZero=FALSE);
		/*! \remarks Reverses the splines of this shape if the corresponding
		bit in <b>reverse</b> is set. The BitArray has one bit for each
		polyline.
		\par Parameters:
		<b>BitArray \&reverse</b>\n\n
		If the bit is set the spline is reversed; otherwise it is left
		alone.\n\n
		<b>BOOL keepZero=FALSE</b>\n\n
		This should normally be passed as TRUE. If TRUE, and the polyline is
		closed, this method will make sure that vertex zero is the same on the
		reversed version as on the non-reversed version. Otherwise if passed as
		FALSE the last vertex becomes the first vertex, and the first vertex
		becomes the last. This is an important distinction for the lofter
		because it always wants vertex zero to be in the same place. */
		CoreExport void Reverse(BitArray &reverse, BOOL keepZero=FALSE);
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Returns the material ID for the specified segment of the specified
		poly.
		\par Parameters:
		<b>int poly</b>\n\n
		The zero based index of the poly.\n\n
		<b>int piece</b>\n\n
		The zero based index of the segment. */
		CoreExport MtlID GetMatID(int poly, int piece);
		// Constructs a vertex selection list based on the current selection level.
		/*! \remarks Constructs a vertex selection list based on the current
		selection level of the specified poly. For example if the selection
		level is at object level all the bits are set. If the selection level
		is at vertex level only the selected vertex bits are set. See
		Class BitArray.
		\par Parameters:
		<b>int poly</b>\n\n
		The poly whose selection level is constructed. */
		CoreExport BitArray	VertexTempSel(int poly);
		// functions for use in data flow evaluation
		/*! \remarks This method is used internally in data flow evaluation in the	pipeline. */
		CoreExport void ShallowCopy(PolyShape *ashape, ChannelMask channels);
		/*! \remarks This method is used internally in data flow evaluation in the	pipeline. */
		CoreExport void DeepCopy(PolyShape *ashape, ChannelMask channels);
		/*! \remarks This method is used internally in data flow evaluation in the	pipeline. */
		CoreExport void NewAndCopyChannels(ChannelMask channels);
		/*! \remarks This method is used internally in data flow evaluation in the	pipeline. */
		CoreExport void FreeChannels(ChannelMask channels, int zeroOthers=1);
		// IO
		/*! \remarks This method is used internally in saving to the MAX file.
		*/
		CoreExport IOResult Save(ISave *isave);
		/*! \remarks This method is used internally in loading from the MAX
		file. */
		CoreExport IOResult Load(ILoad *iload);

		//! \brief Delete multiple lines at once.
		/*!	Deleting multiple lines at once is faster than deleting one by one by calling Delete(), because the internal bookkeeping only needs to be done once.

		Lines are deleted in reverse order, from highest index to lowest index.  If the array of indices is not properly sorted (in ascending order), or if the
		indices are not unique, the results will be undefined: the wrong lines may be deleted, or more likely 'false' will be returned, leaving you
		with a ShapeObject in an undefined state.

				\param[in] indices Point to an array of indices to delete.  Indices should be unique, and sorted in ascending order.
				\param[in] Number of entries in the indices array.
				\return true if the delete operations succeeded, false if any of the indices are invalid.
		*/
		CoreExport bool Delete(const unsigned int* indices, unsigned int n);

		//! \brief Append multiple lines at once.
		/*!	Appending multiple lines at once is faster than appending one by one by calling Append(), because the internal bookkeeping only needs to be done once.

				\param[in] lines Array of lines to append. 
				\param[in] n Number of entries in the lines array.
				\param[in] useSwap Should PolyLine.Swap() be used instead of operator=?  Using Swap() is faster, and should be used if you are just going to discard the contents of the lines array.
		*/
		CoreExport void Append(PolyLine* lines, unsigned int n, bool useSwap = false);
	};

#endif // __POLYSHP_H_
