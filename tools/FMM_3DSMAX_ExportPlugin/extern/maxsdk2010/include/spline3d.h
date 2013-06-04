/**********************************************************************
 *<
	FILE: spline3d.cpp

	DESCRIPTION: General-purpose 3D spline class

	CREATED BY: Tom Hudson & Dan Silva

	HISTORY: created 2/23/95
		4/16/97 TH: Converted to use relative values for bezier vectors

 *>	Copyright (c) 1995, All Rights Reserved.
 **********************************************************************/

#ifndef __SPLINE3D_H__

#define __SPLINE3D_H__

#include "maxheap.h"
#include "polyshp.h"	// Need this for PolyLine class

// Parameter types for shape interpolation (Must match types in object.h)
#define PARAM_SIMPLE 0		// Parameter space based on segments
#define PARAM_NORMALIZED 1	// Parameter space normalized to curve length

// Point flags for PolyShape representation
#define BEZ_SHAPE_KNOT			(1<<0)	// It's a knot point
#define BEZ_SHAPE_INTERPOLATED	(1<<1)	// It's an interpolated point between two knots

// Line types:
#define LTYPE_CURVE 0
#define LTYPE_LINE 1

// Compound line types
#define CURVE_CURVE (LTYPE_CURVE | (LTYPE_CURVE<<2))
#define LINE_CURVE (LTYPE_LINE | (LTYPE_CURVE<<2))
#define CURVE_LINE (LTYPE_CURVE | (LTYPE_LINE<<2))
#define LINE_LINE (LTYPE_LINE | (LTYPE_LINE<<2))

// Knot types
#define KTYPE_AUTO 0
#define KTYPE_CORNER 1
#define KTYPE_BEZIER 2
#define KTYPE_BEZIER_CORNER (KTYPE_BEZIER | KTYPE_CORNER)
#define KTYPE_RESET	4

// Parameter types
#define PARM_UNIFORM		0
#define PARM_ARCLENGTH		1
#define PARM_CENTRIPETAL	2
#define PARM_CUSTOM			3

// The drawPhase values
#define DRAW_IDLE 0
#define DRAW_INITIAL_MOUSE_DOWN 1
#define DRAW_FREEMOVE_POINT 2
#define DRAW_FREEMOVE_POINT_MOUSE_DOWN 3	// Inserting's initial click inside spline
#define DRAW_INITIAL_BEZ_ADJ 11
#define DRAW_DRAGGING_VECTOR 22

// Parameter types for shape interpolation (Must match interpolation types in object.h)
#define SPLINE_INTERP_SIMPLE 0		// Parameter space based on segments
#define SPLINE_INTERP_NORMALIZED 1	// Parameter space normalized to curve length

class Spline3D;
class SplineKnotAssy;
class SplineKnot;			  

//
// 'aux' fields in spline are available in 3 channels:
// 0: Used in capping process
// 1: Used to track topology changes in spline editing
// 2: Available to user
//


// This class has the vertex-level point information for the Spline3D class
class SplinePoint: public MaxHeapOperators {
	friend class Spline3D;
	friend class SplineKnotAssy;
	friend class SplineKnot;

	private:
		Point3 point;
		int aux[3];
	public:
		CoreExport SplinePoint();
		CoreExport SplinePoint(Point3 &p, int a1 = -1, int a2 = -1, int a3 = -1);
		CoreExport SplinePoint& operator=(SplinePoint &fromSP);
		inline Point3& GetPoint() { return point; }
		inline int GetAux(int index) { return aux[index]; }
		inline void SetPoint(const Point3 &p) { point = p; }
		inline void SetAux(int index, int a) { aux[index] = a; }
	};

// The mat ID is stored in the HIWORD of the knot flags
#define SPLINE_MATID_SHIFT	16
#define SPLINE_MATID_MASK	0xFFFF
//watje
#define SEGMENT_VISIBLE		(1<<0)
#define SPLINEKNOT_NO_SNAP	(1<<1)	// Suppresses snapping to knot if set

/*! /remarks This is primarily for internal use, and is designed for the 
	new "connect copy" feature of spline segments. The method XFormVerts() 
	normally operates on the selected knots (or the knots of selected spline
	segments), but if the knots have the SPLINEKNOT_ADD_SEL flag set, and the 
	SplineShape also has the ES_ADDED_SELECT flag set, then those additional 
	knots will be affected as well. */
#define SPLINEKNOT_ADD_SEL	(1<<2)	// CAL-05/23/03: additional selection for transformation

// This class is used for the internal storage of spline knot assemblies
// in the Spline3D class
class SplineKnotAssy: public MaxHeapOperators {
	friend class Spline3D;
	friend class SplineKnot;

	private:
		int ktype;			// Knot type
		int ltype;			// Line type
		float du;			// Parameter value
		SplinePoint inVec;	// The in vector
		SplinePoint knot;	// The knot
		SplinePoint outVec;	// The out vector
		DWORD flags;
	public:
		CoreExport SplineKnotAssy();
		CoreExport SplineKnotAssy(int k, int l, Point3 p, Point3 in, Point3 out, int a1= -1, int a2= -1, int a3= -1, int Ia1= -1, int Ia2= -1, int Ia3= -1, int Oa1= -1, int Oa2= -1, int Oa3= -1, DWORD f=0);
		CoreExport SplineKnotAssy(int k, int l, SplinePoint p, SplinePoint in, SplinePoint out, DWORD f=0);
		CoreExport SplineKnotAssy(SplineKnot &k);
		inline	int		Ktype() { return ktype; }
		inline	void	SetKtype(int t) { ktype=t; }
		inline	int		Ltype() { return ltype; }
		inline	void	SetLtype(int t) { ltype=t; }
		inline	Point3	Knot() { return knot.point; }
		inline	void	SetKnot(const Point3 &p) { knot.point=p; }
		inline	Point3	InVec() { return inVec.point; }
		inline	void	SetInVec(const Point3 &p) { inVec.point=p; }
		inline	Point3	OutVec() { return outVec.point; }
		inline	void	SetOutVec(const Point3 &p) { outVec.point=p; }
		inline	float	GetParm() { return du; }
		inline	void	SetParm(float p) { du = p; }
		inline	MtlID	GetMatID() {return (int)((flags>>SPLINE_MATID_SHIFT)&SPLINE_MATID_MASK);}
		inline	void    SetMatID(MtlID id) {flags &= 0xFFFF; flags |= (DWORD)(id<<SPLINE_MATID_SHIFT);}
		
		// The following methods allow access as if the in/knot/out components
		// are contained vertices.  index:0=inVec 1=knot 2=outVec  which: 0=aux1 1=aux2 2=aux3
		CoreExport int	GetAux(int index, int which);
		CoreExport void	SetAux(int index, int which, int value);
		CoreExport Point3 GetVert(int index);
		CoreExport void SetVert(int index, const Point3 &p);

		inline	SplinePoint GetKnot() { return knot; }
		inline	SplinePoint GetInVec() { return inVec; }
		inline	SplinePoint GetOutVec() { return outVec; }
		inline	void SetKnot(SplinePoint &sp) { knot = sp; }
		inline	void SetInVec(SplinePoint &sp) { inVec = sp; }
		inline	void SetOutVec(SplinePoint &sp) { outVec = sp; }
		inline  DWORD GetFlags() { return flags; }

//watje		
		inline	BOOL	IsHidden() {return (flags&SEGMENT_VISIBLE);}
		inline	void    Hide() { flags |= (DWORD)(SEGMENT_VISIBLE);}
		inline	void    Unhide() { flags &= (DWORD)(~SEGMENT_VISIBLE);}
//TH 8/22/00
		inline	BOOL	IsNoSnap() {return (flags&SPLINEKNOT_NO_SNAP);}
		inline	void    SetNoSnap() { flags |= (DWORD)(SPLINEKNOT_NO_SNAP);}
		inline	void    ClearNoSnap() { flags &= (DWORD)(~SPLINEKNOT_NO_SNAP);}
// CAL-05/23/03
		inline	BOOL	GetFlag (DWORD fl) { return (flags & fl) ? TRUE : FALSE; }
		inline	void	SetFlag (DWORD fl, BOOL val=TRUE) { if (val) flags |= fl; else flags &= ~fl; }
		inline	void	ClearFlag (DWORD fl) { flags &= ~fl; }
	};

// This class is used by plugins to get and set knot information in the Spline3D class.
// This is primarily here for backward-compatibility with versions prior to MAXr3

/*! \sa  Class Spline3D.\n\n
\par Description:
This class describes a single knot in a spline.\n\n
In 3ds Max 2.0 and later there are methods which provide full access to the
private data members of the class.\n\n
All methods of this class are implemented by the system.
\par Description:
private:\n\n
<b>int ktype;</b>\n\n
The knot type.\n\n
<b>int ltype;</b>\n\n
The line type.\n\n
<b>Point3 point;</b>\n\n
The point location.\n\n
<b>Point3 inVec;</b>\n\n
The in vector.\n\n
<b>Point3 outVec;</b>\n\n
The out vector.\n\n
<b>int aux;</b>\n\n
This is simply an integer value which may be used for temporary storage of data
associated with the knot. This data will be overwritten by the internal
EditSpline code.\n\n
<b>int aux2;</b>\n\n
Used to track topo changes in spline editing.\n\n
<b>Spline Knot Summary:</b>\n\n
<b>Picture a bezier spline with three knots, going from left to right:</b>\n\n
<b>A---\>AB----BA\<---B---\>BC----CB\<---C</b>\n\n
The knot points are A, B and C. The vectors are labeled the same as patch
vectors (AB is the vector from A going toward B, the vector from B to A is
labeled BA, and so on).\n\n
In this diagram, AB is the OUT vector for knot A. BA is the IN vector for knot
B. BC is the OUT vector for knot B, and CB is the IN vector for knot C. Because
this is an open spline, knot A doesn't use its IN vector, and knot C doesn't
use its OUT vector.\n\n
The IN and OUT terminology is based on the way a spline flows from the first
knot to the last. If the spline is reversed, the IN and OUT sense is reversed,
as well.\n\n
Regarding the vectors, the only difference between a circle and a square is
that the square has vectors that are at the same location as the knot point (in
other words, zero length vectors) causing sharp corners. The circle uses
vectors which cause each segment to bulge to form a quarter-circle.\n\n
Take a look at the <b>/MAXSDK/SAMPLES/OBJECTS/NGON.CPP</b> source file for an
example of how the vectors are generated to form a linear NGON versus a
circular one. */
class SplineKnot: public MaxHeapOperators {
	friend class Spline3D;
	friend class SplineKnotAssy;

	int ktype;
	int ltype;
	Point3 point;
	Point3 inVec;
	Point3 outVec;
	int aux;		// Used for capping
	int aux2;		// Used to track topo changes in spline editing
	int aux3;		// User aux field
	int inAux;
	int inAux2;
	int inAux3;
	int outAux;
	int outAux2;
	int outAux3;
	DWORD flags;
public:
	CoreExport SplineKnot();
	/*! \remarks Constructor. The data members are initialized to the values passed. */
	CoreExport SplineKnot(int k, int l, Point3 p, Point3 in, Point3 out, int a1= -1, int a2= -1, int a3= -1, int Ia1= -1, int Ia2= -1, int Ia3= -1, int Oa1= -1, int Oa2= -1, int Oa3= -1, DWORD f=0);
	CoreExport SplineKnot(SplineKnotAssy &k);
	/*! \remarks Returns the knot type.
	\return  See <a href="ms-its:listsandfunctions.chm::/idx_R_list_of_spline_knot_types.html">List of Spline
	Knot Types</a>. */
	inline	int		Ktype() { return ktype; }
	/*! \remarks	Sets the knot type.
	\par Parameters:
	<b>int t</b>\n\n
	The type to set. See <a href="ms-its:listsandfunctions.chm::/idx_R_list_of_spline_knot_types.html">List of
	Spline Knot Types</a>. */
	inline	void	SetKtype(int t) { ktype=t; }
	/*! \remarks Returns the line type.
	\return  See <a href="ms-its:listsandfunctions.chm::/idx_R_list_of_spline_line_types.html">List of Spline
	Line Types</a>. */
	inline	int		Ltype() { return ltype; }
	/*! \remarks	Sets the line type.
	\par Parameters:
	<b>int t</b>\n\n
	The type to set. See <a href="ms-its:listsandfunctions.chm::/idx_R_list_of_spline_line_types.html">List of
	Spline Line Types</a>. */
	inline	void	SetLtype(int t) { ltype=t; }
	/*! \remarks	Provides access to the first integer chunk of auxiliary data for the knot.
	*/
	inline	int		Aux() { return aux; }
	/*! \remarks	Sets the first integer of auxilliary data for the knot.
	\par Parameters:
	<b>int a</b>\n\n
	The value to set. */
	inline	void	SetAux(int a) { aux=a; }
	/*! \remarks	Provides access to the second integer of auxiliary data for the knot. */
	inline	int		Aux2() { return aux2; }
	/*! \remarks	Sets the second integer of auxilliary data for the knot.
	\par Parameters:
	<b>int a</b>\n\n
	The value to set. */
	inline	void	SetAux2(int a) { aux2=a; }
	inline	int		Aux3() { return aux3; }
	inline	void	SetAux3(int a) { aux3=a; }
	inline	int		InAux() { return inAux; }
	inline	void	SetInAux(int a) { inAux=a; }
	inline	int		InAux2() { return inAux2; }
	inline	void	SetInAux2(int a) { inAux2=a; }
	inline	int		InAux3() { return inAux3; }
	inline	void	SetInAux3(int a) { inAux3=a; }
	inline	int		OutAux() { return outAux; }
	inline	void	SetOutAux(int a) { outAux=a; }
	inline	int		OutAux2() { return outAux2; }
	inline	void	SetOutAux2(int a) { outAux2=a; }
	inline	int		OutAux3() { return outAux3; }
	inline	void	SetOutAux3(int a) { outAux3=a; }
	/*! \remarks	Returns the point location for the knot. */
	inline	Point3	Knot() { return point; }
	/*! \remarks	Sets the point location for the knot.
	\par Parameters:
	<b>Point3 p</b>\n\n
	The point to set. */
	inline	void	SetKnot(Point3 p) { point=p; }
	/*! \remarks	Returns the in vector for the knot. */
	inline	Point3	InVec() { return inVec; }
	/*! \remarks	Sets the in vector for the knot.
	\par Parameters:
	<b>Point3 p</b>\n\n
	The in vector to set. */
	inline	void	SetInVec(Point3 p) { inVec=p; }
	/*! \remarks	Returns the out vector for the knot. */
	inline	Point3	OutVec() { return outVec; }
	/*! \remarks	Sets the out vector for the knot.
	\par Parameters:
	<b>Point3 p</b>\n\n
	The out vector to set. */
	inline	void	SetOutVec(Point3 p) { outVec=p; }
	inline	MtlID	GetMatID() {return (int)((flags>>SPLINE_MATID_SHIFT)&SPLINE_MATID_MASK);}
	inline	void    SetMatID(MtlID id) {flags &= 0xFFFF; flags |= (DWORD)(id<<SPLINE_MATID_SHIFT);}
	inline  DWORD	GetFlags() { return flags; }

//watje
	inline	BOOL	IsHidden() {return (flags&SEGMENT_VISIBLE);}
	inline	void    Hide() { flags |= (DWORD)(SEGMENT_VISIBLE);}
	inline	void    Unhide() { flags &= (DWORD)(~SEGMENT_VISIBLE);}
//TH 8/22/00
	inline	BOOL	IsNoSnap() {return (flags&SPLINEKNOT_NO_SNAP);}
	inline	void    SetNoSnap() { flags |= (DWORD)(SPLINEKNOT_NO_SNAP);}
	inline	void    ClearNoSnap() { flags &= (DWORD)(~SPLINEKNOT_NO_SNAP);}
// CAL-05/23/03
	inline	BOOL	GetFlag (DWORD fl) { return (flags & fl) ? TRUE : FALSE; }
	inline	void	SetFlag (DWORD fl, BOOL val=TRUE) { if (val) flags |= fl; else flags &= ~fl; }
	inline	void	ClearFlag (DWORD fl) { flags &= ~fl; }
	};

// Private spline flags
#define SPLINE_CLOSED	(1<<0)
#define SPLINE_ORTHOG   (1<<1)

/*! \sa  Class SplineKnot, Class PolyLine, 
Class Structure Knot.\n\n
\par Description:
General-purpose 3D spline class. The <b>BezierShape</b> class has a list of
these splines that make up the bezier shape. Methods of this class are used to
access the properties of the spline. All methods of this class are implemented
by the system.
\par Method Groups:
See <a href="class_spline3_d_groups.html">Method Groups for Class Spline3D</a>. */
class Spline3D: public MaxHeapOperators {
    friend class BezierShape;
    friend class SplineShape;
    friend class Railing; // this is a VIZ-only class; requires access to percents
private:
	static	int			splineCount;	// Number of splines in the system
			int			parmType;		// Interpolation parameter type	(needed?)
			int			knotCount;		// Number of points in spline

			int			flags;			// Private flags
			int			iCur;			// Current editing point
			int			Interact(ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3* mat, IObjParam *ip=NULL );	// Handle mouse interaction
			float		cachedLength;
			BOOL		cacheValid;
			SplineKnotAssy*	knots;		// Knot assembly array
			float *		lengths;		// Cached lengths
			float *		percents;		// Cached length percentages
			int			drawPhase;		// Drawing phase
			int			editMode;		// 1 if editing, 0 otherwise

			// Creation settings
			int			initialType;	// Knot type at initial click
			int			dragType;		// Knot type at drag
			
			BOOL		selfInt;		// Cached self-intersection flag
			BOOL		siCacheValid;	// Self-intersection cache valid?
			Box3		bbox;			// Cached bounding box
			BOOL		bboxValid;		// Bounding box valid?
			BOOL		paramsValid;	// Knot parameter values valid?
			BOOL		bezPointsValid;	// Bezier points valid?
			BOOL		clockwise;		// Clockwise cache
			BOOL		cwCacheValid;	// Clockwise cache valid?
			PolyLine	polyLine;		// Polyline cache
			BOOL		plineOpt;
			int			plineSteps;
			BOOL		plineCacheValid;	// Polyline cache valid?
protected:
	CoreExport		void		Allocate(int count);
	CoreExport		void		ChordParams();							// Compute chord length params
	CoreExport		void		UniformParams();						// Compute uniform params
	CoreExport		void		CentripetalParams();					// Compute centripetal params
	CoreExport		void		LinearFwd(int i);
	CoreExport		void		LinearBack(int i);
	CoreExport		void		ContinFwd(int i);
	CoreExport		void		ContinBack(int i);
	CoreExport		void		HybridPoint(int i);
	CoreExport		void		CompCornerBezPoints(int n);
	CoreExport		void		CompAdjBesselBezPoints(int i);
	CoreExport		void		BesselStart(int i);
	CoreExport		void		BesselEnd(int i);
	CoreExport		void		NaturalFwd(int i);
	CoreExport		void		NaturalBack(int i);
public:

	/*! \remarks Constructor. The data members are initialized to the values
	passed. */
	CoreExport		Spline3D(int itype = KTYPE_CORNER,int dtype = KTYPE_BEZIER,int ptype = PARM_UNIFORM);		// Constructor	
	CoreExport		Spline3D(Spline3D& fromSpline);
	/*! \remarks Destructor. */
	CoreExport		virtual ~Spline3D();	// Destructor
	/*! \remarks Assignment operator. */
	CoreExport		Spline3D& 	operator=(Spline3D& fromSpline);
	/*! \remarks Assignment operator. This generates a PolyLine from the
	spline, where points are added in between the knots on the spline. For
	example if the steps value was 5, it will interpolate 5 points in between
	each knot on the spline. */
	CoreExport		Spline3D& 	operator=(PolyLine& fromLine);
	/*! \remarks This method clears out the spline. It frees the knots
	attributes array and the bezier points array. */
	CoreExport		void		NewSpline();
	/*! \remarks This method is used internally. */
	inline			int			ParmType() { return parmType; };
	/*! \remarks Returns the Knot (point) count. */
	inline			int			KnotCount() { return knotCount; }						// Point (knot) count
	/*! \remarks Returns the private spline flags.\n\n
	<b>SPLINE_CLOSED</b>\n\n
	This indicates if the spline is closed or not. */
	inline			int			Flags() { return flags; }
	/*! \remarks Returns the number of line segments in the spline. For
	example if you have a 4 knot spline that is open you'll get 3 segments. */
	CoreExport		int			Segments();												// Segment count
	/*! \remarks Returns the closed status. Nonzero if closed; otherwise zero.
	*/
	inline			int			Closed() { return (flags & SPLINE_CLOSED) ? 1:0; }		// Returns closed status
	/*! \remarks This method is used internally. */
	CoreExport		int			ShiftKnot(int where,int direction);						// Shove array left or right 1,
	/*! \remarks Add a knot to the spline at the specified location.
	\par Parameters:
	<b>SplineKnot \&k</b>\n\n
	The knot to add.\n\n
	<b>int where = -1</b>\n\n
	The location to add the knot. <b>where</b>\< 0 indicates the end of the spline.
	\return  Nonzero on success; otherwise zero. */
	CoreExport		int			AddKnot(SplineKnot &k,int where = -1);					// Add a knot to the spline
	/*! \remarks	This method sets the 'i-th' knot object which contain the knot point, in
	and out\n\n
	vectors, knot and line types and auxiliary values.
	\par Parameters:
	<b>int i</b>\n\n
	Specifies the knot to set.\n\n
	<b>SplineKnot \&k</b>\n\n
	The knot to set. */
	CoreExport		void		SetKnot(int i, SplineKnot &k);
	/*! \remarks	This method gets the 'i-th' knot object which contain the knot point, in
	and out\n\n
	vectors, knot and line types and auxiliary values.
	\par Parameters:
	<b>int i</b>\n\n
	Specifies the knot to retrieve. */
	CoreExport		SplineKnot	GetKnot(int i);

	/*! \remarks Delete the specified knot.
	\par Parameters:
	<b>int where</b>\n\n
	The location of the knot to delete.
	\return  Nonzero if the knot was deleted; otherwise zero. */
	CoreExport		int			DeleteKnot(int where);									// Delete the specified knot
	/*! \remarks This method is used internally by the free form line object
	<b>SPLINE.CPP</b>. This method allows the user to use the mouse to create a
	line. See the sample code in <b>/MAXSDK/SAMPLES/OBJECTS/SPLINE.CPP</b> for
	an example of this method in use.\n\n
	New for 3ds Max 2.0 is an additional parameter is a pointer to an
	<b>IObjParam</b> object, which is used to access the
	<b>DisplayTempPrompt()</b> mechanism. The parameter is optional; omitting
	it allows the spline operation to work as in 3ds Max 1.x. Adding the
	parameter causes the spline to display the delta, distance and angle of the
	current segment being edited. */
	CoreExport		int			Create(ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3* mat, IObjParam *ip=NULL);	// Create the spline
	/*! \remarks This method is used internally by the free form line object
	<b>SPLINE.CPP</b>. See the sample code in
	<b>/MAXSDK/SAMPLES/OBJECTS/SPLINE.CPP</b> for an example of this method in
	use. */
	CoreExport		int			StartInsert(ViewExp *vpt,int msg, int point, int flags, IPoint2 theP, Matrix3* mat, int where );	// Start an insertion operation on the spline
	/*! \remarks This method is used internally. */
	CoreExport		int			SetParam(int index,float param);		// Set custom param value
	/*! \remarks This method is used internally. */
	CoreExport		float		GetParam(int index);					// Get param value
	/*! \remarks Returns the knot type from the specified knot. See
	<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_spline_knot_types.html">List of Spline Knot
	Types</a>.
	\par Parameters:
	<b>int index</b>\n\n
	The index of the knot type to return. */
	inline			int			GetKnotType(int index) { return knots[index].ktype; }
	/*! \remarks Sets the knot type of the specified knot.
	\par Parameters:
	<b>int index</b>\n\n
	The knot to set.\n\n
	<b>int type</b>\n\n
	See <a href="ms-its:listsandfunctions.chm::/idx_R_list_of_spline_knot_types.html">List of Spline Knot
	Types</a>.
	\return  Nonzero if set; otherwise zero. */
	CoreExport		int			SetKnotType(int index,int type);		// Set the knot type
	/*! \remarks Returns the type of line segment between knots for the
	specified segment.
	\par Parameters:
	<b>int index</b>\n\n
	The index of the segment whose line type to return.
	\return  See <a href="ms-its:listsandfunctions.chm::/idx_R_list_of_spline_line_types.html">List of Spline
	Line Types</a>. */
	inline			int			GetLineType(int index) { return knots[index].ltype; }
	/*! \remarks Sets the line type of the specified segment.
	\par Parameters:
	<b>int index</b>\n\n
	The index of the segment.\n\n
	<b>int type</b>\n\n
	See <a href="ms-its:listsandfunctions.chm::/idx_R_list_of_spline_line_types.html">List of Spline Line
	Types</a>.
	\return  Nonzero if set; otherwise zero. */
	CoreExport		int			SetLineType(int index,int type);		// Set the line type
	/*! \remarks This method is used internally. */
	virtual			void		CustomParams() { UniformParams(); }		// Replace this as needed
	/*! \remarks This method is used internally. */
	CoreExport		void		CompParams();							// Compute param values
	/*! \remarks This method should be called whenever you finish changing
	points on the spline. This updates all the information internal to the
	spline needed to calculate all the bezier handles. */
	CoreExport		void		ComputeBezPoints();
	CoreExport		void		FindSegAndParam(float u, int ptype, int &seg, float &param);	// Find segment and parameter for whole-curve parameter
	CoreExport		void		RefineCurve(float u, int ptype=SPLINE_INTERP_SIMPLE);
	CoreExport		void		RefineSegment(int segment, float t, int ptype=SPLINE_INTERP_SIMPLE);
	/*! \remarks Developers should use <b>InterpBezier3D()</b> below instead
	of this method and just use the x and y values returned. */
	CoreExport		Point2		InterpBezier(IPoint2 *bez, float t);
	/*! \remarks This method returns a point interpolated on a segment between
	two knots.
	\par Parameters:
	<b>int segment</b>\n\n
	The index of the segment to interpolate.\n\n
	<b>float t</b>\n\n
	A value in the range of 0.0 to 1.0. 0 is the first knot and 1 is the second
	knot.\n\n
	<b>int ptype=SPLINE_INTERP_SIMPLE</b>\n\n
	The spline type to use.
	\return  The interpolated point. */
	CoreExport		Point3		InterpBezier3D(int segment, float t, int ptype=SPLINE_INTERP_SIMPLE);
	/*! \remarks This method returns a point interpolated on the entire curve.
	This method returns a point but you don't know which segment the point
	falls on. Typically the method <b>InterpBezier3D()</b> will give better
	control of the curve as it interpolates a bezier segment.
	\par Parameters:
	<b>float u</b>\n\n
	A value in the range of 0.0 to 1.0 for the entire curve.\n\n
	<b>int ptype=SPLINE_INTERP_SIMPLE</b>\n\n
	The spline type to use.
	\return  The interpolated point. */
	CoreExport		Point3		InterpCurve3D(float u, int ptype=SPLINE_INTERP_SIMPLE);
	/*! \remarks This method returns a tangent vector interpolated on a
	segment between two knots.
	\par Parameters:
	<b>int segment</b>\n\n
	The index of the segment.\n\n
	<b>float t</b>\n\n
	A value in the range of 0.0 to 1.0. 0 is the first knot and 1 is the second
	knot.\n\n
	<b>int ptype=SPLINE_INTERP_SIMPLE</b>\n\n
	The spline type to use.
	\return  The tangent vector. */
	CoreExport		Point3		TangentBezier3D(int segment, float t, int ptype=SPLINE_INTERP_SIMPLE);
	/*! \remarks This method returns a tangent vector interpolated on the
	entire curve.
	\par Parameters:
	<b>float u</b>\n\n
	A value in the range of 0.0 to 1.0 for the entire curve.\n\n
	<b>int ptype=SPLINE_INTERP_SIMPLE</b>\n\n
	The spline type to use.
	\return  The tangent vector. */
	CoreExport		Point3		TangentCurve3D(float u, int ptype=SPLINE_INTERP_SIMPLE);
	/*! \remarks This method should not be used. */
	CoreExport		Point3		AverageTangent(int i);
	/*! \remarks This method should not be used. */
	CoreExport		void		MakeBezCont(int i);
	/*! \remarks This method should not be used. */
	CoreExport		void		RedistTangents(int i, Point3 d);
	/*! \remarks This method should not be used. */
	CoreExport		void		FixAdjBezTangents(int i);
	/*! \remarks This method should not be used. */
	CoreExport		void		DrawCurve(GraphicsWindow *gw, Material *mtl);
	/*! \remarks This method should not be used. */
	inline			void		SetEditMode(int mode) { editMode = mode ? 1:0; }
	/*! \remarks Returns nonzero if the knot type is <b>KTYPE_AUTO</b>
	otherwise zero.
	\par Parameters:
	<b>int i</b>\n\n
	The index of the knot. */
	CoreExport		int			IsAuto(int i);
	/*! \remarks Returns nonzero if the knot type is <b>KTYPE_BEZIER</b>
	otherwise zero.
	\par Parameters:
	<b>int i</b>\n\n
	The index of the knot. */
	CoreExport		int			IsBezierPt(int i);
	/*! \remarks Returns nonzero if the knot type is <b>KTYPE_CORNER</b>
	otherwise zero.
	\par Parameters:
	<b>int i</b>\n\n
	The index of the knot. */
	CoreExport		int			IsCorner(int i);
	/*! \remarks This method is used internally. */
	CoreExport		Point3		GetDragVector(ViewExp *vpt,IPoint2 p,int i,Matrix3* mat);
	/*! \remarks This method is used internally. */
	CoreExport		int			AppendPoint(ViewExp *vpt,const Point3& p, int where = -1);
	/*! \remarks This method is used internally. */
	CoreExport		int			DrawPhase() { return drawPhase; }
	/*! \remarks This method is used internally. */
	CoreExport		int			GetiCur() { return iCur; }
	/*! \remarks Returns the bounding box of the curve in the space specified
	by the <b>tm</b>.
	\par Parameters:
	<b>TimeValue t</b>\n\n
	This parameter is not used.\n\n
	<b>Matrix3\& tm</b>\n\n
	The <b>tm</b> to transform the points by prior to computing the bounding
	box.\n\n
	<b>Box3\& box</b>\n\n
	The bounding box */
	CoreExport		void		GetBBox(TimeValue t,  Matrix3& tm, Box3& box);
	/*! \remarks This method is used internally. */
	CoreExport		IPoint2		ProjectPoint(ViewExp *vpt, Point3 fp, Matrix3 *mat);
	/*! \remarks This method is used internally. */
	CoreExport		Point3		UnProjectPoint(ViewExp *vpt, IPoint2 p, Matrix3 *mat);
	/*! \remarks This method is used internally. */
	CoreExport		void		Snap(GraphicsWindow *gw, SnapInfo *snap, IPoint2 *p, Matrix3 &tm);
	/*! \remarks This method is used internally to save the class data from
	disk. */
	CoreExport		IOResult 	Save(ISave *isave);
	/*! \remarks This method is used internally to load the class data from
	disk. */
	CoreExport		IOResult	Load(ILoad *iload);
	/*! \remarks This method may be used to close or open the spline. If the
	optional parameter is not specified it is closed.
	\par Parameters:
	<b>int flag = 1</b>\n\n
	Nonzero to close; zero to open.
	\return  Nonzero if changed, zero if not. */
	CoreExport		int			SetClosed(int flag = 1);
	/*! \remarks Sets the spline to open.
	\return  Nonzero if changed, zero if not. */
	CoreExport		int			SetOpen();
	/*! \remarks This displays data about the specified knot using
	<b>DebugPrints()</b>. See
	<a href="ms-its:3dsmaxsdk.chm::/debug_debugging.html">Debugging</a>.
	\par Parameters:
	<b>int where</b>\n\n
	The index of the knot. */
	CoreExport		void		Dump(int where);
	/*! \remarks	This method is used to retrieve the 'i-th' in vector position in absolute
	coordinates.
	\par Parameters:
	<b>int i</b>\n\n
	The vector position to retrieve. */
	CoreExport 		Point3		GetInVec(int i);
	/*! \remarks	This method is used to set the 'i-th' in vector position in absolute
	coordinates.
	\par Parameters:
	<b>int i</b>\n\n
	The position to alter.\n\n
	<b>const Point3 \&p</b>\n\n
	The value to set in absolute coordinates. */
	CoreExport		void		SetInVec(int i, const Point3 &p);
	/*! \remarks	This method is used to retrieve the 'i-th' in vector position relative to
	the knot point.
	\par Parameters:
	<b>int i</b>\n\n
	The postion to retrieve. */
	CoreExport		Point3		GetRelInVec(int i);
	/*! \remarks	This method sets the 'i-th' bezier in vector position relative to the knot
	point.
	\par Parameters:
	<b>int i</b>\n\n
	The vector to set.\n\n
	<b>const Point3 \&p</b>\n\n
	The vector data to set, relative to the knot point. */
	CoreExport		void		SetRelInVec(int i, const Point3 &p);
	/*! \remarks	This methods returns the 'i-th' knot point.
	\par Parameters:
	<b>int i</b>\n\n
	Specifies which knot point to get. */
	CoreExport		Point3		GetKnotPoint(int i);
	/*! \remarks	This methods sets the 'i-th' knot point to the specified value.
	\par Parameters:
	<b>int i</b>\n\n
	Specifies which knot point to set.\n\n
	<b>const Point3 \&p</b>\n\n
	The value to set. */
	CoreExport		void		SetKnotPoint(int i, const Point3 &p);
	/*! \remarks	This method is used to retrieve the 'i-th' out vector position in absolute
	coordinates.
	\par Parameters:
	<b>int i</b>\n\n
	Specifies which out vector point to get. */
	CoreExport		Point3		GetOutVec(int i);
	/*! \remarks	This method is used to set the 'i-th' out vector position in absolute
	coordinates.
	\par Parameters:
	<b>int i</b>\n\n
	Specifies which out vector point to get.\n\n
	<b>const Point3 \&p</b>\n\n
	The out vector to set in absolute coordinates. */
	CoreExport		void		SetOutVec(int i, const Point3 &p);
	/*! \remarks	This method is used to retrieve the 'i-th' out vector position relative to
	the knot point.
	\par Parameters:
	<b>int i</b>\n\n
	Specifies the point to get. */
	CoreExport		Point3		GetRelOutVec(int i);
	/*! \remarks	This method is used to set the 'i-th' out vector position relative to the
	knot point.
	\par Parameters:
	<b>int i</b>\n\n
	Specifies the point to set.\n\n
	<b>const Point3 \&p</b>\n\n
	The out vector position to set relative to the knot point. */
	CoreExport		void		SetRelOutVec(int i, const Point3 &p);
	// The following methods return absolute coords for the bezier vertices
	/*! \remarks Each control point is made up of three points. The in vector
	coming off the bezier control point, the knot point itself, and the out vector.
	There are these three points for every control point. This method will return
	any item in this list.
	\par Parameters:
	<b>int i</b>\n\n
	The index into the vertex list. */
	CoreExport		Point3		GetVert(int i);
	/*! \remarks This method should not be used. */
	CoreExport		void		SetVert(int i, const Point3& p);
	/*! \remarks Returns the number of vertices. This is always the number of
	knots times 3. */
	inline			int			Verts() { return knotCount*3; }
	// The following methods get/set the knot aux fields based on knot index
	// These are here for backward compatibility with MAXr2
	/*! \remarks	Returns the auxilliary data associated with the specified knot. This is
	used internally for tracking topological changes to the spline during
	editing. Developers can use it for temporary purposes but it will be
	altered by the EditableSpline (SplineShape) code.
	\par Parameters:
	<b>int knot</b>\n\n
	The knot to retrieve the data from. */
	CoreExport		int			GetAux(int knot);
	/*! \remarks	Sets the first integer auxilliary data associated with the specified knot.
	\par Parameters:
	<b>int knot</b>\n\n
	Specifies the knot to whose auxilliary data is set.\n\n
	<b>int value</b>\n\n
	The value to set. */
	CoreExport		void		SetAux(int knot, int value);
	/*! \remarks	Returns the second integer auxilliary data associated with the specified
	knot
	\par Parameters:
	<b>int knot</b>\n\n
	The knot to retrieve the data from. */
	CoreExport		int			GetAux2(int knot);
	/*! \remarks	Sets the second integer auxilliary data associated with the specified knot.
	\par Parameters:
	<b>int knot</b>\n\n
	Specifies the knot to whose auxilliary data is set.\n\n
	<b>int value</b>\n\n
	The value to set. */
	CoreExport		void		SetAux2(int knot, int value);
	/*! \remarks	Returns the third integer auxilliary data associated with the specified
	knot. This field is available for any use.
	\par Parameters:
	<b>int knot</b>\n\n
	Specifies the knot to whose auxilliary data is returned. */
	CoreExport		int			GetAux3(int knot);
	/*! \remarks	Sets the third integer auxilliary data associated with the specified knot.
	This field is available for any use.
	\par Parameters:
	<b>int knot</b>\n\n
	Specifies the knot whose auxilliary data is set.\n\n
	<b>int value</b>\n\n
	The value to set. */
	CoreExport		void		SetAux3(int knot, int value);
	// The following methods are new to MAXr3 and get/set aux fields for in/knot/out
	// knot: knot index
	// which: 0=aux1 1=aux2 2=aux3
	/*! \remarks	Returns the specified integer auxilliary data associated with the specified
	knot.
	\par Parameters:
	<b>int knot</b>\n\n
	Specifies the knot whose auxilliary data is returned.\n\n
	<b>int which</b>\n\n
	Specifies which auxiliary field. One of the following values:\n\n
	<b>0</b>=aux1\n\n
	<b>1</b>=aux2\n\n
	<b>2</b>=aux3 */
	CoreExport		int			GetKnotAux(int knot, int which);
	/*! \remarks	Sets the specified integer auxilliary data associated with the specified
	knot.
	\par Parameters:
	<b>int knot</b>\n\n
	Specifies the knot whose auxilliary data is set.\n\n
	<b>int which</b>\n\n
	Specifies which auxiliary field. One of the following values:\n\n
	<b>0</b>=aux1\n\n
	<b>1</b>=aux2\n\n
	<b>2</b>=aux3\n\n
	<b>int value</b>\n\n
	The value to set. */
	CoreExport		void		SetKnotAux(int knot, int which, int value);
	/*! \remarks	Returns the specified integer auxilliary data associated with the specified
	in vector.
	\par Parameters:
	<b>int knot</b>\n\n
	Specifies the knot whose auxilliary data is returned.\n\n
	<b>int which</b>\n\n
	Specifies which auxiliary field. One of the following values:\n\n
	<b>0</b>=aux1\n\n
	<b>1</b>=aux2\n\n
	<b>2</b>=aux3 */
	CoreExport		int			GetInAux(int knot, int which);
	/*! \remarks	Sets the specified integer auxilliary data associated with the specified in
	vector.
	\par Parameters:
	<b>int knot</b>\n\n
	Specifies the knot whose auxilliary data is set.\n\n
	<b>int which</b>\n\n
	Specifies which auxiliary field. One of the following values:\n\n
	<b>0</b>=aux1\n\n
	<b>1</b>=aux2\n\n
	<b>2</b>=aux3\n\n
	<b>int value</b>\n\n
	The value to set. */
	CoreExport		void		SetInAux(int knot, int which, int value);
	/*! \remarks	Returns the specified integer auxilliary data associated with the specified
	out vector.
	\par Parameters:
	<b>int knot</b>\n\n
	Specifies the knot whose auxilliary data is returned.\n\n
	<b>int which</b>\n\n
	Specifies which auxiliary field. One of the following values:\n\n
	<b>0</b>=aux1\n\n
	<b>1</b>=aux2\n\n
	<b>2</b>=aux3 */
	CoreExport		int			GetOutAux(int knot, int which);
	/*! \remarks	Sets the specified integer auxilliary data associated with the specified
	out vector.
	\par Parameters:
	<b>int knot</b>\n\n
	Specifies the knot whose auxilliary data is set.\n\n
	<b>int which</b>\n\n
	Specifies which auxiliary field. One of the following values:\n\n
	<b>0</b>=aux1\n\n
	<b>1</b>=aux2\n\n
	<b>2</b>=aux3\n\n
	<b>int value</b>\n\n
	The value to set. */
	CoreExport		void		SetOutAux(int knot, int which, int value);
	// The following methods get/set the aux fields	based on bezier vertex index
	// i: bezier vertex index
	// which: 0=aux1 1=aux2 2=aux3
	/*! \remarks	Returns the specified integer auxilliary data associated with the specified
	bezier vertex.
	\par Parameters:
	<b>int i</b>\n\n
	The zero based bezier vertex index.\n\n
	<b>int which</b>\n\n
	Specifies which auxiliary field. One of the following values:\n\n
	<b>0</b>=aux1\n\n
	<b>1</b>=aux2\n\n
	<b>2</b>=aux3 */
	CoreExport		int			GetVertAux(int i, int which);
	/*! \remarks	Sets the specified integer auxilliary data associated with the specified
	bezier vertex.
	\par Parameters:
	<b>int i</b>\n\n
	The zero based bezier vertex index.\n\n
	<b>int which</b>\n\n
	Specifies which auxiliary field. One of the following values:\n\n
	<b>0</b>=aux1\n\n
	<b>1</b>=aux2\n\n
	<b>2</b>=aux3\n\n
	<b>int value</b>\n\n
	The value to set. */
	CoreExport		void		SetVertAux(int i, int which, int value);
	// The following methods get/set the material ID for a spline segment
	/*! \remarks	Returns the material ID for the specified spline segment.
	\par Parameters:
	<b>int seg</b>\n\n
	The zero based index of the segment. */
	CoreExport		MtlID		GetMatID(int seg);
	/*! \remarks	Sets the material ID for the specified spline segment.
	\par Parameters:
	<b>int seg</b>\n\n
	The zero based index of the segment.\n\n
	<b>MtlID id</b>\n\n
	The material ID to set. */
	CoreExport		void		SetMatID(int seg, MtlID id);

	/*! \remarks Returns the length of the spline. */
	CoreExport		float		SplineLength();
	/*! \remarks	Returns the length of the specified segment of this spline.
	\par Parameters:
	<b>int seg</b>\n\n
	The zero based index of the segment to check. */
	CoreExport		float		SegmentLength(int seg);
	/*! \remarks This method may be used to transform the points of the spline
	into another space defined by the specified transformation matrix.
	\par Parameters:
	<b>Matrix3 *tm</b>\n\n
	The transformation matrix. */
	CoreExport		void		Transform(Matrix3 *tm);
	/*! \remarks This method reverses all the points of the spline.
	\par Parameters:
	<b>BOOL keepZero = FALSE</b>\n\n
	This optional parameter is available in release 2.0 and later only.\n\n
	This parameter defaults to FALSE in order to retain backwards compatibility.
	Setting it to TRUE insures that a closed spline will have the same vertex as
	its first point when it is reversed. The parameter is ignored on open
	splines.*/
	CoreExport		void		Reverse(BOOL keepZero = FALSE);
	/*! \remarks This method appends the specified spline onto the end of this
	one. The splines should both be opened.
	\par Parameters:
	<b>Spline3D *spline</b>\n\n
	The spline to append.\n\n
	<b>BOOL weldCoincidentFirstVertex=TRUE</b>\n\n
	Set this to TRUE to weld coincident first vertices. FALSE to disable welding.
	Returns TRUE if first point auto-welded	*/
	CoreExport		BOOL		Append(Spline3D *spline, BOOL weldCoincidentFirstVertex=TRUE);	
	/*! \remarks This method takes the specified spline and puts it on the front
	of this spline.
	\par Parameters:
	<b>Spline3D *spline</b>\n\n
	The spline to prepend. 
	Returns TRUE if last point auto-welded */
	CoreExport		BOOL		Prepend(Spline3D *spline, BOOL weldCoincidentLastVertex=TRUE);
	/*! \remarks Returns TRUE if the spline is clockwise in the XY plane (it
	ignores Z); otherwise FALSE. This call is meaningless if the shape self intersects. */
	CoreExport		BOOL		IsClockWise();			// 2D!
	/*! \remarks Returns TRUE if the spline intersects itself in the XY plane
	(it ignores Z); otherwise FALSE. */
	CoreExport		BOOL		SelfIntersects();		// 2D!
	/*! \remarks Returns TRUE if this spline intersects the specified spline
	in the XY plane (it ignores Z); otherwise FALSE.
	\par Parameters:
	<b>Spline3D *spline</b>\n\n
	The spline to check. */
	CoreExport		BOOL		IntersectsSpline(Spline3D *spline);		// 2D!
	/*! \remarks Returns TRUE if the specified point is surrounded (contained
	within) this spline.
	\par Parameters:
	<b>Point2 p</b>\n\n
	The point to check. */
	CoreExport		BOOL		SurroundsPoint(Point2 p);	// 2D!
	/*! \remarks This creates a PolyLine from this spline given a steps setting
	and an optimize parameter.\n\n
	Note the following constraints on this method. When a ShapeObject is asked to
	output a PolyShape with a given number of steps and FALSE is specified for
	optimization, it must output a PolyLine with [steps * pieces + pieces + 1]
	vertices if it's an open shape and [steps * pieces + pieces] vertices if it's closed.
	\par Parameters:
	<b>PolyLine \&line</b>\n\n
	The result is stored here.\n\n
	<b>int steps = -1</b>\n\n
	The number of steps between knots in the spline.\n\n
	<b>BOOL optimize = FALSE</b>\n\n
	If TRUE, linear segments between control points in the spline will not generate
	steps in between. It will just be one line segment. */
	CoreExport		void		MakePolyLine(PolyLine &line, int steps = -1, BOOL optimize = FALSE);
	/*! \remarks This method makes sure the shape has flushed out any cached
	data it may have had. */
	CoreExport		void		InvalidateGeomCache();
	/*! \remarks	This method allows this Spline3D to create a map of smoothing groups that
	eases the creation of meshes.\n\n
	It fills in a developer supplied IntTab with smoothing groups for each
	segment of the spline. A spline with 4 segments will cause the IntTab to be
	set to 4 entries, for example.\n\n
	Five smoothing groups are used for this operation, 1\<\<0 through 1\<\<4.
	Once you have them, you can shift them as needed for your application. The
	smoothing groups are set up so that segments connected by knots with
	<b>KTYPE_SMOOTH</b> or <b>KTYPE_BEZIER</b> types are smoothed together.
	\par Parameters:
	<b>IntTab \&map</b>\n\n
	A table of integers. See Template Class Tab. */
	CoreExport		void		GetSmoothingMap(IntTab &map);
	};

#endif // __SPLINE3D_H__
