/**********************************************************************
 *<
	FILE: MNMesh.h

	DESCRIPTION:  Special mesh structures useful for face and edge based mesh operations.

	CREATED BY: Steve Anderson, working for Kinetix!

	HISTORY: created March 1997 from old SDMesh and WMesh.

 *>	Copyright (c) 1996 Autodesk, Inc., All Rights Reserved.
 **********************************************************************/

// Necessary prior inclusions: max.h

// Classes:
// MNMesh
// MNVert
// MNEdge
// MNFace
// MNMeshBorder

#ifndef __MN_MESH_H_
#define __MN_MESH_H_

// Turn on the following to enable Face Data in PolyObjects:
#define MNMESH_FACEDATA_IMPL

#include "maxheap.h"
#include "export.h"
#include "baseinterface.h"
#include <hitdata.h>

#define REALLOC_SIZE 10

//#define SEPARATE_GEOM_TOPO_INVALIDATION

// Boolean types: we use the same ones defined in mesh.h
//#define MESHBOOL_UNION 1
//#define MESHBOOL_INTERSECTION 2
//#define MESHBOOL_DIFFERENCE 3

// General flags for all components
// For MNVerts, MNEdges, and MNFaces, bits 0-7 are used for common characteristics
// of all components.  Bits 8-15 are used for component-specific flags.  Bits 16-23 are reserved
// for temporary use in MNMesh algorithms.  Bits 24-31 are reserved for MNMath.lib users.
#define MN_SEL (1<<0)
#define MN_DEAD (1<<1)
#define MN_TARG (1<<2)
#define MN_BACKFACING (1<<3)
#define MN_HIDDEN (1<<4)
#define MN_CACHEINVALID (1<<5)
#define MN_INFOREGROUND (1<<7) //this tags a face as not renderable but hittestable BTW 6 is skipped because the tri strip uses that as face flag signifying a face to be stripped

#define MN_WHATEVER (1<<16) // Temporary flag used internally for whatever.
#define MN_LOCAL_SEL (1<<17) // Alternate selections (not passed up the pipe).
#define MN_HITTEST_CULLED (1<<18)	// used to indicate "culled" components (neither selected nor not selected) in window-region hit testing
#define MN_USER (1<<24)	// Anything above this can be used by applications.

// Vertex flags
#define MN_VERT_DONE (1<<8)
#define MN_VERT_WELDED (1<<9)
// CAL-04/07/03: ANDY: New flag here, to track which vertices are direct "descendants" of
// the original cage vertices in subdivision.
#define MN_VERT_SUBDIVISION_CORNER (1<<10)

class IHardwareShader;
class TriStrip;
class MNMesh;

/*! \sa  Class FlagUser, Class MNMesh, Template Class Tab.\n\n
\par Description:
This class is available in release 2.0 and later only.\n\n
MNVert is the vertex class used with the MNMesh mesh. MNVerts have not only a
Point3 location, but also some flags.\n\n
All methods of this class are implemented by the system.
\par Data Members:
<b>Point3 p</b>\n\n
The location of this MNVert.\n\n
<b>int orig</b>\n\n
This data member is obsolete and should not be used.
\par Flags:
For more information on flags, see Class FlagUser.\n\n
<b>MN_SEL</b>\n\n
Indicates that the vertex is selected.\n\n
<b>MN_TARG</b>\n\n
Indicates that the vertex is targeted. (See the MNMesh methods starting with
the words TargetBy.)\n\n
<b>MN_DEAD</b>\n\n
Indicates that the vertex is not used and should be ignored. Vertices with the
MN_DEAD flag are deleted in the next MNMesh call to CollapseDeadVerts ().\n\n
<b>MN_BACKFACING</b>\n\n
Indicates that the vertex faces "backwards" in the current viewport. (Changes
often.)\n\n
<b>MN_WHATEVER</b>\n\n
Developers should not use this flag and should restrict themselves to
<b>MN_USER</b> and higher bits.\n\n
<b>MN_VERT_DONE</b>\n\n
Set in algorithms that may accidentally revisit the same vertex twice, to keep
them from processing it the second time.\n\n
<b>MN_VERT_HIDDEN</b>\n\n
Different from MN_HIDDEN - means a vert is on a face's hvtx list, rather than
existing independently of faces.\n\n
<b>MN_USER(1\<\<16)</b>\n\n
Flag bits at or above MN_USER are reserved in all MNMesh components for the
plug-in developer, if needed. Since FlagUser-derived classes have 32 flag bits,
this allows for up to 16 user-defined flags.  */
class MNVert : public FlagUser {
public:
	Point3 p;
	int orig;	// Original point this vert comes from

	/*! \remarks Constructor.\n\n
	This method initializes the MNVert. */
	MNVert () { orig = -1; }
	/*! \remarks Assignment operator. Copies over all data from "from". */
	DllExport MNVert & operator= (const MNVert & from);
	bool operator==(const MNVert & from) { return (from.p==p)&&(from.ExportFlags()==ExportFlags()); }
};

// Edge flags
#define MN_EDGE_INVIS (1<<8)
#define MN_EDGE_NOCROSS (1<<9)
#define MN_EDGE_MAP_SEAM (1<<10)
// CAL-03/14/03: ANDY: New flag here, to track which edges are direct "descendants" of
// the original cage edges in subdivision.
#define MN_EDGE_SUBDIVISION_BOUNDARY (1<<11)
// SCA - 6/20/04 - new flag to track unwanted edges created by Cut, so they might
// be removed later.
#define MN_EDGE_CUT_EXTRA (1<<12)

// Edge goes from v1 to v2
// f1 is forward-indexing face (face on "left" if surface normal above, v2 in front)
// f2 is backward-indexing face, or -1 if no such face exists.  (Face on "right")
/*! \sa  Class FlagUser, Class MNMesh.\n\n
\par Description:
This class is available in release 2.0 and later only.\n\n
MNEdge is the edge class used with the MNMesh mesh. MNEdges are "winged-edge"
structures, which means they keep track of a start vertex, an end vertex, and
the (unique) face that uses the start and end vertices in that order. If there
is a face that uses the end and start vertices in that order, i.e. that travels
this edge in the other direction, it is also recorded.\n\n
All methods of this class are implemented by the system.
\par Data Members:
<b>int v1, v2</b>\n\n
The start and end vertices. These values are indices into the parent MNMesh's
list of MNVerts.\n\n
<b>int f1</b>\n\n
The (unique) face that references this edge in the forward direction. This
value is an index into the parent MNMesh's list of MNFaces.\n\n
<b>int f2</b>\n\n
The face (if any) that references this edge in the backward direction. Faces
with f2=-1 are considered "one-sided", and lie on the boundary of a hole in the
mesh. This value is an index into the parent MNMesh's list of MNFaces.\n\n
<b>int track</b>\n\n
This data member is obsolete and should not be used.
\par Flags:
For more information on flags, see Class FlagUser.\n\n
<b>MN_SEL</b>\n\n
Indicates that the edge is selected.\n\n
<b>MN_TARG</b>\n\n
Indicates that the edge is targeted. (See the MNMesh methods starting with the
words TargetBy.)\n\n
<b>MN_DEAD</b>\n\n
Indicates that the edge is not used and should be ignored. Edges with the
MN_DEAD flag are deleted in the next MNMesh call to CollapseDeadEdges ().\n\n
<b>MN_EDGE_INVIS</b>\n\n
Both faces using this edge consider it invisible.\n\n
<b>MN_EDGE_HALFINVIS</b>\n\n
One face using this edge considers it invisible.\n\n
<b>MN_EDGE_NOCROSS</b>\n\n
This edge should not be crossed in algorithms like MNMesh::SabinDoo that can
"mix" faces across edges.\n\n
<b>MN_EDGE_TV_SEAM</b>\n\n
This edge lies on a mapping coordinate "seam". You must use
MNMesh::SetTVSeamFlags on a mesh with mapping coordinates in order to set these
flags correctly.\n\n
<b>MN_EDGE_WHATEVER</b>\n\n
Developers should not use this flag and should restrict themselves to
<b>MN_USER</b> and higher bits.\n\n
<b>MN_USER(1\<\<16)</b>\n\n
Flag bits at or above MN_USER are reserved in all MNMesh components for the
plug-in developer, if needed. Since FlagUser-derived classes have 32 flag bits,
this allows for up to 16 user-defined flags.  */
class MNEdge : public FlagUser {
public:
	int v1, v2;
	int f1, f2;
	int track;	// Keep track of whatever.

	/*! \remarks Constructor. Initializes both faces to -1 and both vertices
	to 0. */
	MNEdge() { Init(); }
	/*! \remarks Constructor. Initializes edge to run from vv1 to vv2 with f1
	set to fc. */
	MNEdge (int vv1, int vv2, int fc) { f1=fc; f2=-1; v1=vv1; v2=vv2; track=-1; }
	/*! \remarks	Initializes v1, v2 and f1 to 0, f2 to -1 and track to -1. */
	void Init() { v1=v2=f1=0; f2=-1; track=-1; }
	/*! \remarks Assuming that ff is one of the faces using this edge,
	OtherFace will return the other. If the edge is one-sided, -1 will be
	returned. If ff is not one of the faces, f2 (which may be -1) will be
	returned. */
	int OtherFace (int ff) { return (ff==f1) ? f2 : f1; }
	/*! \remarks Assuming that vv is one of the vertices on this edge,
	OtherVert will return the other. If vv is not one of the faces, v2 will be
	returned. */
	int OtherVert (int vv) { return (vv==v1) ? v2 : v1; }
	/*! \remarks Flips the edge around, so that it now goes from what was v2
	to what was v1. f1 and f2 are also switched. This should not be called on
	one-sided edges. */
	void Invert () { int hold=v1; v1=v2; v2=hold; hold=f1; f1=f2; f2=hold; }
	/*! \remarks	Replaces face <b>of</b> with face <b>nf</b> in the edge records. NOTE that
	this method causes an assertion failure if face of is not currently used by
	the edge. If <b>of</b> is on both sides of the edge, which is possible on
	some valid NONTRI meshes, a nonnegative <b>vv1</b> is used to specify which
	side is replaced. <b>Vv1</b> should be the "starting vertex" for the edge
	on face of. Assertion failures will also result if <b>vv1</b> is
	nonnegative and is not either of the edge's verts, or if <b>vv1</b>
	indicates that of should be the edge's f1, but it is not, etc. */
	DllExport void ReplaceFace (int of, int nf, int vv1=-1);
	/*! \remarks	Replaces vertex ov in the edge records with vertex nv. NOTE that this
	method causes an assertion failure if vertex ov is not used by this edge */
	void ReplaceVert (int ov, int nv) { if (v1 == ov) v1 = nv; else { MaxAssert (v2==ov); v2 = nv; } }
	/*! \remarks If this edge has the <b>MN_EDGE_NOCROSS</b> flag set, or if
	it has no second face, this method returns TRUE. Otherwise, it returns
	FALSE. It's a shorthand equivalent for <b>(GetFlag(MN_EDGE_NOCROSS)
	||(f2\<0))</b>. */
	DllExport bool Uncrossable ();
	/*! \remarks Assignment operator. Copies over all data. */
	DllExport MNEdge & operator= (const MNEdge & from);
	bool operator==(MNEdge & f) { return (f.v1==v1)&&(f.v2==v2)&&(f.f1==f1)&&(f.f2==f2)&&(f.ExportFlags()==ExportFlags()); }
	/*! \remarks	Vertex access operator.
	\par Default Implementation:
	<b>{ return i ? v2 : v1; }</b> */
	int& operator[](int i) { return i ? v2 : v1; }  
	/*! \remarks	Vertex access operator.
	\par Default Implementation:
	<b>{ return i ? v2 : v1; }</b> */
	const int& operator[](int i) const { return i ? v2 : v1; }  
	/*! \remarks	Uses DebugPrint to print out edge information to the Debug Results window
	in DevStudio. The information consists of the vertices and faces using this
	edge. It is generally a good idea to put in a DebugPrint immediately before
	this with the index of the edge, so you know which one is being printed
	out:\n\n
	<b>DebugPrint(_M("Edge %d: "), eid);</b>\n\n
	<b>E(eid)-\>MNDebugPrint();</b> */
	DllExport void MNDebugPrint ();
};

class MNFace;

/*! \sa  Class MNMesh, Class MNMap.\n\n
\par Description:
This class is available in release 3.0 and later only.\n\n
This class is a face used to store map vertex information for a given face and
map channel. It replaces the information previously contained in the
MNFace::tvrt and MNFace::cvrt data members (prior to release 3).\n\n
By way of analogy: MNMapFace is to MNFace as TVFace is to Face. MNMapFace is to
MNMap as MNFace is to MNMesh as TVFace is to MeshMap as Face is to Mesh.
\par Data Members:
<b>int deg</b>\n\n
Degree of this face. Must match degree of related MNFace in MNMesh.\n\n
<b>int *tv</b>\n\n
Mapping vertices used by this mapping face. (This array is of size <b>deg</b>).
 */
class MNMapFace: public MaxHeapOperators {
	friend class MNMesh;
	int dalloc;
public:
	int deg;
	int *tv;

	/*! \remarks Constructor. Calls <b>Init()</b>. */
	MNMapFace() { Init(); }
	/*! \remarks Constructor. Sets the degree and hidden vertex count.
	\par Parameters:
	<b>int d</b>\n\n
	Desired degree. */
	DllExport MNMapFace (int d);
	/*! \remarks Destructor. Calls <b>Clear()</b>. */
	~MNMapFace () { Clear(); }
	/*! \remarks Initializes MNMapFace. <b>deg</b> is set to 0, pointers set
	to NULL. */
	DllExport void Init();
	/*! \remarks Clears and frees MNMapFace. */
	DllExport void Clear();
	/*! \remarks Allocates enough memory in the arrays for the face to have
	degree <b>d</b>, but does not actually set the degree. If the arrays are
	already large enough (or larger), it does not reallocate them.
	\par Parameters:
	<b>int d</b>\n\n
	The degree for this map face. */
	DllExport void SetAlloc (int d);
	/*! \remarks Allocates enough memory in the arrays for the face to have
	degree <b>d</b>, but does not actually set the degree. If the arrays are
	already large enough (or larger), it does not reallocate them. You
	generally don't need to use this method separately; MakePoly, Insert, and
	other methods which may require additional memory will call this if needed.
	*/
	void SetSize (int d) { SetAlloc(d); deg=d; }
	/*! \remarks Makes this face into a polygon with the specified vertices
	and other information.
	\par Parameters:
	<b>int fdeg</b>\n\n
	The degree to set this face to.\n\n
	<b>int *tt</b>\n\n
	The list of vertices for this face. */
	DllExport void MakePoly (int fdeg, int *tt);
	/*! \remarks Inserts space for more vertices at the specified position.
	\par Parameters:
	<b>int pos</b>\n\n
	The location within the map face where the new vertices should be
	added.\n\n
	<b>int num=1</b>\n\n
	The number of new vertices to add. */
	DllExport void Insert (int pos, int num=1);
	/*! \remarks Deletes vertices from this map face.
	\par Parameters:
	The location within the map face where the vertices should be deleted.\n\n
	<b>int num=1</b>\n\n
	The number of vertices to delete. */
	DllExport void Delete (int pos, int num=1);
	/*! \remarks Re-indexes the vertices and edges so that the vertex in
	position <b>newstart</b> becomes the new first vertex. Triangulation is
	also corrected.
	\par Parameters:
	<b>int newstart</b>\n\n
	The new first vertex. */
	DllExport void RotateStart (int newstart);
	/*! \remarks Reverses order of verts, effectively inverting the face. */
	DllExport void Flip ();	// Reverses order of verts.  0 remains start.

	/*! \remarks Returns the position of vertex <b>vv</b> in this face's list
	of vertices.
	\par Parameters:
	<b>int vv</b>\n\n
	The vertex whose index is returned. */
	DllExport int VertIndex (int vv);
	/*! \remarks Replaces vertex <b>ov</b> with vertex <b>nv</b> in the list
	of vertices.
	\par Parameters:
	<b>int ov</b>\n\n
	The vertex to replace.\n\n
	<b>int nv</b>\n\n
	The vertex to replace it with. */
	DllExport void ReplaceVert (int ov, int nv);

	/*! \remarks Assignment operator. */
	DllExport MNMapFace & operator= (const MNMapFace & from);
	DllExport MNMapFace & operator= (const MNFace & from);
	DllExport bool operator== (const MNMapFace & from);
	DllExport void MNDebugPrint ();
};

// MNFace flags:
#define MN_FACE_OPEN_REGION (1<<8)	// Face is not part of closed submesh.
#define MN_FACE_CHECKED (1<<9)	// for recursive face-and-neighbor-checking
#define MN_FACE_CHANGED (1<<10)
// The following macro has been added
// in 3ds max 4.2.  If your plugin utilizes this new
// mechanism, be sure that your clients are aware that they
// must run your plugin with 3ds max version 4.2 or higher.
#define MN_FACE_CULLED (1<<11)	// Used during hit-testing.
// End of 3ds max 4.2 Extension

// Diagonal sorting algorithm:
// Puts diagonals in increase-by-last-index, decrease-by-first order:
/*! \remarks<b>Note that this function is not part of the class but is available for
us.</b>\n\n
This sorts the diagonals in the following fashion: each diagonal is reordered
so that its smaller index comes first, then its larger. Then the list of
diagonals is sorted so that it increases by second index, then decreases by
first index. Such an ordered list for a 9-gon might be (1,3),(0,3),
(0,4),(5,7),(4,7),(4,8). (This order is especially convenient for converting
into triangles - it makes for a linear-time conversion.) <b>DiagSort()</b> uses
qsort for speed.
\par Parameters:
<b>int dnum</b>\n\n
The size of the diag array - essentially double the number of diagonals.\n\n
<b>int *diag</b>\n\n
The diagonals. */
DllExport void DiagSort (int dnum, int *diag);

/*! \sa  Class FlagUser, Class MNFace, Class BitArray.\n\n
\par Description:
This class is available in release 2.0 and later only.\n\n
<b>MNFace</b> is the face structure used with the <b>MNMesh</b> mesh.
<b>MNFaces</b> need not be triangles. They also may contain "hidden" vertices
which are used in converting the face back to triangles. This "triangulation"
is always maintained in the face.\n\n
All methods of this class are implemented by the system.
\par Data Members:
private:\n\n
<b>int dalloc</b>\n\n
This is the amount of space allocated for verts, edges, mapping coords, and
other information that's based on degree (deg).\n\n
<b>int talloc</b>\n\n
This is the amount of space allocated for the triangulation.\n\n
<b>int deg</b>\n\n
Degree: this is the number of vertices and edges that this face has.\n\n
<b>int *vtx</b>\n\n
This is the list of vertices that make up the corners of this face. Each value
is an index into the parent MNMesh's list of MNVerts.\n\n
<b>int *edg</b>\n\n
This is the list of edges that border this face, in order. Each edg[i] goes
between vtx[i] and vtx[(i+1)%deg]. Each value is an index into the parent
MNMesh's list of MNEdges. These values may not be valid if the MNMesh's
MN_MESH_FILLED_IN flag is not set.\n\n
<b>int *diag</b>\n\n
This is where the triangulation is stored. The number of triangles in a face is
given by deg - 2 + hdeg*2. This array contains three times this number, for all
the corners of all the sub-triangles. The triangle values are indices into the
vtx and hvtx arrays of this face. Hidden vertices are indicated by values less
than zero: hvtx[i] is represented by -1-i. Thus a triangle (1, 2, -2) would
represent a triangle using vtx[1], vtx[2], and hvtx[-1]. The diag array's
allocated size is always (dalloc-3)*2. If dalloc==3 (triangle), this pointer is
NULL.\n\n
<b>DWORD smGroup</b>\n\n
This contains the smoothing groups assigned to this face.\n\n
<b>MtlID material</b>\n\n
This is the material ID assigned to this face.\n\n
<b>int track</b>\n\n
This member is obsolete and should not be used.\n\n
<b>BitArray visedg</b>\n\n
Contains a visibility bit for each edge on this face. See the MNMesh note on
edge selection \& visibility for more information.\n\n
<b>BitArray edgsel</b>\n\n
Contains a selection bit for each edge on this face. See the MNMesh note on
edge selection \& visibility for more information.
\par Flags:
<b>MN_SEL</b>\n\n
Indicates that the face is selected.\n\n
<b>MN_TARG</b>\n\n
Indicates that the face is targeted. (See the MNMesh methods starting with the
words TargetBy.)\n\n
<b>MN_DEAD</b>\n\n
Indicates that the face is not used and should be ignored. Faces with the
MN_DEAD flag are deleted in the next MNMesh call to CollapseDeadFaces ().\n\n
<b>MN_FACE_OPEN_REGION</b>\n\n
This flag is available in release 2.5 and later only.\n\n
This face is part of a region of the mesh that is not closed, i.e. there are
1-sided edges. This means that the mesh is not a solid object, it has gaps or
holes. The flag is set by the\n\n
<b>MN_FACE_CHECKED</b>\n\n
This flag is available in release 2.5 and later only.\n\n
Reserved for internal use (in recursive face-traversing algorithms).\n\n
<b>MN_FACE_CHANGED</b>\n\n
This flag can be cleared on all faces before an operation that moves some of
the vertices of an MNMesh, then set for each face touching one of the moved
vertices. This tells the parent MNMesh that these faces may need to have
information such as triangulation recomputed. This flag is set by the MNMesh
method SabinDoo in particular.\n\n
<b>MN_FACE_CULLED</b>\n\n
Indicates that the face is culled (used during hit-testing). <b>Release 4.2 and
above</b>\n\n
<b>MN_FACE_WHATEVER</b>\n\n
Developers should not use this flag and should restrict themselves to
<b>MN_USER</b> and higher bits.\n\n
<b>MN_USER(1\<\<16)</b>\n\n
Flag bits at or above MN_USER are reserved in all MNMesh components for the
plug-in developer, if needed. Since FlagUser-derived classes have 32 flag bits,
this allows for up to 16 user-defined flags.  */
class MNFace : public FlagUser {
	friend class MNMesh;
	int dalloc;
	void SwapContents (MNFace & from);
public:
	int deg;	// Degree: number of vtx's and edg's that are relevant.
	int *vtx;	// Defining verts of this face.
	int *edg;	// edges on this face.
	int *diag;	// Diagonals
	DWORD smGroup;
	MtlID material;
	int track;	// Keep track of whatever -- MNMesh internal use only.
	BitArray visedg, edgsel;
	BitArray bndedg;	// CAL-03/18/03: boundary edges

	/*! \remarks Constructor. Initializes the face's arrays to NULL. */
	MNFace() { Init(); }
	/*! \remarks Constructor. Initializes the face's degree to d and allocates
	space for all the arrays. */
	MNFace (int d) { Init(); SetDeg (d); }
	/*! \remarks Constructor. Copies flags, smoothing groups, and material
	from "from", but initializes the face's arrays to NULL and degree to 0. */
	DllExport MNFace (const MNFace *from);
	/*! \remarks Frees all arrays. */
	~MNFace () { Clear(); }

	/*! \remarks	Initialize the face. */
	DllExport void Init();
	/*! \remarks	Set the number of edges and vertices this face has.
	\par Parameters:
	<b>int d</b>\n\n
	Number of vertices and edges. */
	DllExport void SetDeg (int d);
	/*! \remarks Frees all arrays, setting them to NULL, and resets degree. */
	DllExport void Clear();
	/*! \remarks	Returns the number of triangles in this face. */
	int TriNum() { return deg-2; }
	/*! \remarks	Given the index of a particular edge, this routine returns the point
	(distinct from edge and (edge+1)%deg) that forms a triangle with the edge,
	given the current scheme of diagonals.
	\par Parameters:
	<b>int edge</b>\n\n
	An index into the vertex array (in the range 0 to deg-1) that indicates the
	starting vertex of the edge. (In other words, the edge falls between vertex
	vtx[edge] and vtx[(edge+1)%deg].)
	\return  The index of the desired vertex, again in the (0,deg-1) range, or
	edge if there's an error. */
	DllExport int FindTriPoint (int edge);	// finds point that makes tri with this edge.
	/*! \remarks	Given two verts that form a diagonal in the polygon, this method finds the
	vertex between them that connects by a diagonal or an outer edge to both of
	them. (Here, "between them" means after a and before b in sequence around
	the outside of the polygon. If we have an octagon where a=6 and b=2, the
	result would be 7, 0, or 1. To get the other result, in the 3,4,5 range,
	call the method with a=2 and b=6.)
	\par Parameters:
	<b>int a, b</b>\n\n
	Two vertices, "internally indexed" in the 0 to deg-1 range. This method is
	only guaranteed to work if the vertices share a diagonal. (Otherwise, there
	may be no solution.)
	\return  The index of the desired vertex, again in the (0,deg-1) range, or
	a if there's an error. */
	DllExport int FindTriPoint (int a, int b);	// does the same for nonsequential verts - result is between a,b.
	/*! \remarks	This method fills in the table with the full triangulation for the face,
	based on the internal diagonal list. The table is set to size (deg-2)*3.
	\par Parameters:
	<b>Tab\<int\> \&tri</b>\n\n
	The table of triangles. */
	DllExport void GetTriangles (Tab<int> &tri);
	/*! \remarks Allocates enough memory in the arrays for the face to have degree
	d, but does not actually set the degree. If the arrays are already large enough
	(or larger), it does not reallocate them. You generally don't need to use this
	method separately; MakePoly, Insert, and other methods which may require
	additional memory will call this if needed. */
	DllExport void SetAlloc (int d);
	/*! \remarks	Makes this face into a polygon with the specified vertices and other
	information. This routine also supplies a default triangulation for the
	face; however, since this MNFace-level routine cannot access the vertex
	positions contained in the parent MNMesh, this triangulation may not work
	for non-convex faces. If the face may not be convex, a call to
	MNMesh::RetriangulateFace for this face will correct the triangulation.
	\par Parameters:
	<b>int fdeg</b>\n\n
	The degree to set this face to.\n\n
	<b>int *vv</b>\n\n
	The list of vertices for this face. There must be at least fdeg of these.
	These values should be indices into the parent MNMesh's array of
	MNVerts.\n\n
	<b>bool *vis=NULL</b>\n\n
	The edge visibility flags for the edges of this face. If this is NULL, it
	is ignored; otherwise, there must be at least fdeg of these. vis[i]
	represents the visibility of the edge going from vv[i] to vv[(i+1)%fdeg].
	See the MNMesh note on edge selection \& visibility for more
	information.\n\n
	<b>bool *sel=NULL</b>\n\n
	The edge selection flags for the edges of this face. If this is NULL, it is
	ignored; otherwise, there must be at least fdeg of these. sel[i] represents
	the selection bit of the edge going from vv[i] to vv[(i+1)%fdeg]. See the
	MNMesh note on edge selection \& visibility for more information. */
	DllExport void MakePoly (int fdeg, int *vv, bool *vis=NULL, bool *sel=NULL);
	/*! \remarks Inserts space for more vertices and edges on this face. This
	is used, for example, when two faces are joined, to add room for the
	vertices \& edges of one face to the other. This routine also renumbers the
	existing vertices and corrects the existing face triangulation, although it
	cannot provide the triangulation for the new vertices. It reserves space
	for the new triangles at the end of the triangle array. If you do not want
	to compute the triangulation for the new vertices yourself, you may use the
	MNMesh RetriangulateFace method after filling in the new vertices.
	\par Parameters:
	<b>int pos</b>\n\n
	The location within the face where the new vertices and edges should be
	added.\n\n
	<b>int num</b>\n\n
	The number of new vertices and edges. */
	DllExport void Insert (int pos, int num=1);
	/*! \remarks Deletes vertices \& edges from this face. This routine also
	corrects the face triangulation, removing those triangles that include the
	deleted edges and re-indexing the rest. However, delete may cause the
	triangulation to become invalid, by causing one or more of the corrected
	triangles to have a flipped normal.
	\par Parameters:
	<b>int pos</b>\n\n
	The position of the first vertex to be deleted.\n\n
	<b>int num=1</b>\n\n
	The number of vertices \& edges to delete.\n\n
	<b>int edir=1</b>\n\n
	There are two choices for the edges to be deleted: we can delete the edges
	going from pos to pos+1, pos+1 to pos+2, ... pos+num-1 to pos+num, or we can
	delete pos-1 to pos, pos to pos+1, ... pos+num-2 to pos+num-1. (pos+num-1 is
	the last vertex deleted.) That is to say, we can delete the edges "before" the
	vertices we're deleting, or we can delete the edges "after" them. If edir is
	positive, we delete the edges after the vertices. If it's negative, we delete
	the edges before. Keep in mind that this also affects edge visibility and
	selection information on this face.\n\n
	<b>bool fixtri=TRUE</b>\n\n
	This parameter is available in release 3.0 and later only.\n\n
	This argument indicates how far Delete should go in fixing the triangulation.
	Delete will always correct the values of the tri array to correspond to the
	reduced-degree face. If fixtri is true, it will also delete those triangles
	that have collapsed because they had two vertices in the deleted region. If
	not, it will leave these triangles with overlapping vertices, as in (0,0,2).
	\return  Delete returns TRUE if fixtri is FALSE. If fixtri is TRUE, Delete will
	return TRUE if it successfully corrected the triangulation, or FALSE if there
	was a problem. If FALSE is returned, the triangulation will need to be revised
	with a call to RetriangulateFace. */
	DllExport bool Delete (int pos, int num=1, int edir=1, bool fixtri=TRUE);
	/*! \remarks	Re-indexes the vertices and edges so that the vertex in position
	<b>newstart</b> becomes the new first vertex. Triangulation is also
	corrected. Mapping coordinates and vertex colors are corrected
	automatically. */
	DllExport void RotateStart (int newstart);
	/*! \remarks	Reverses order of verts, effectively inverting the face. vtx[0] remains
	unchanged, but vertex deg-1 becomes vertex 1, etc. Note that this operation
	wreaks havoc on nearby edges and should be used with caution. */
	DllExport void Flip ();	// Reverses order of verts.  0 remains start.

	/*! \remarks Returns the position of vertex <b>vv</b> in this face's list
	of vertices. For a given face fc, if fc.vtx[i] = vv, fc.VertIndex (vv) =
	i.\n\n
	Sometimes a single vertex from the MNMesh's MNVert list can be referenced
	more than once by a single face. The picture below illustrates this
	problem. The small triangle is actually outside of the face, and the vertex
	at the top of it is referenced twice by the face. Thus an additional edge
	parameter can be accepted. If ee is -1, it is ignored, and the first
	instance of vv is used. If ee\>-1, this method looks for the instance of vv
	that starts out edge ee. Thus if fc.vtx[i] = vv and fc.vtx[j] = vv, but
	fc.edg[i] != ee and fc.edg[j] = ee, j is returned.\n\n
	IMPORTANT: If no vertex is found matching the given parameters, this method
	generates an assertion failure. Please be sure that vertex vv is actually
	on the face (and that edge ee follows it if ee is not -1) before using this
	method. */
	DllExport int VertIndex (int vv, int ee=-1);
	/*! \remarks Returns the position of edge ee in this face's list of edges.
	For a given face fc, if fc.edg[i] = ee, fc.EdgeIndex (ee) = i.\n\n
	Sometimes a single edge from the MNMesh's MNEdge list can be referenced
	more than once by a single face. The small rectangle is actually outside of
	the face, and the edge above it is referenced twice by the face, once in
	each direction. Thus an additional vertex parameter can be accepted. If vv
	is -1, it is ignored, and the first instance of ee is used. If vv\>-1, this
	method looks for the instance of ee that starts out with vertex vv. Thus if
	fc.edg[i] = ee and fc.edg[j] = ee, but fc.vtx[i] != vv and fc.vtx[j] = vv,
	j is returned.\n\n
	IMPORTANT: If no edge is found matching the given parameters, this method
	generates an assertion failure. Please be sure that edge ee is actually on
	the face (and that vertex vv follows it if vv is not -1) before using this
	method. */
	DllExport int EdgeIndex (int ee, int vv=-1);
	/*! \remarks	Replaces vertex <b>ov</b> with vertex <b>nv</b> in the list of vertices. It
	is possible for a face to reference the same vertex more than once, however
	the combination of a vertex followed by a specified edge is unique.
	Therefore if <b>ee\<0</b>, all instances of <b>ov</b> are replaced by
	<b>nv</b>, but if not, only the instance of <b>ov</b> followed by <b>ee</b>
	is replaced. */
	DllExport void ReplaceVert (int ov, int nv, int ee=-1);
	/*! \remarks Replaces edge <b>oe</b> with edge <b>ne</b> in the list of
	edges. It is possible for a face to reference the same edge twice, however
	the combination of an edge preceded by a specified vertex is unique.
	Therefore if <b>vv\<0</b>, all instances of <b>oe</b> are replaced by
	<b>ne</b>, but if not, only the instance of oe preceded by <b>vv</b> is
	replaced. */
	DllExport void ReplaceEdge (int oe, int ne, int vv=-1);

	/*! \remarks Assignment operator. Copies all information from "from",
	including triangulation, hidden vertices, flags, smoothing \& material
	info, and "track". */
	DllExport MNFace & operator= (const MNFace & from);
	DllExport bool operator== (const MNFace &from);
	/*! \remarks	Access operator.
	\par Default Implementation:
	<b>{ return vtx[i]; }</b> */
	int& operator[](int i) { return vtx[i]; }
	/*! \remarks	Access operator.
	\par Default Implementation:
	<b>{ return vtx[i]; }</b> */
	const int& operator[](int i) const { return vtx[i]; }  
	/*! \remarks	Uses DebugPrint to print out face information to the Debug Results window in
	DevStudio. The information consists of the vertices and edges used by this
	face. It is generally a good idea to put in a DebugPrint immediately before
	this with the index of the edge, so you know which one is being printed
	out:\n\n
	<b>DebugPrint(_M("Face %d: "), fid);</b>\n\n
	<b>F(fid)-\>MNDebugPrint();</b>
	\par Parameters:
	<b>bool triprint=FALSE</b>\n\n
	Print out triangulation information.\n\n
	<b>bool hinfo=TRUE</b>\n\n
	Print out hidden vertex information. */
	DllExport void MNDebugPrint (bool triprint=FALSE);

	DllExport IOResult Save (ISave *isave);
	DllExport IOResult Load (ILoad *iload);
};

/*! \sa  Class FlagUser, Class MNMapFace, Class Point3, Class Matrix3.\n\n
\par Description:
This class is available in release 3.0 and later only. It replaces the old tv
and cv arrays in MNMesh, and the old tvrt and cvrt arrays in MNFace.\n\n
Each MNMap holds the mapping information for a particular map channel of the
MNMesh. As with regular Mesh maps, the MNMap holds an array of map vertices and
an array of map faces that define how those mapping vertices are applied to the
MNMesh. The number (numf) of mapping faces should always match the number of
faces in the parent MNMesh, and each MNMapFace gives the map vertices for the
related MNFace. The number of map vertices may be different from the number of
MNVerts in the parent MNMesh.
\par Data Members:
<b>MNMapFace *f;</b>\n\n
The array of map faces.\n\n
<b>UVVert *v;</b>\n\n
The array of UVVerts. Note: <b>typedef Point3 UVVert;</b>\n\n
<b>int numv</b>\n\n
The number of vertices in the <b>v</b> array.\n\n
<b>int numf;</b>\n\n
The number of map faces in the <b>f</b> array.  */
class MNMap : public FlagUser {
	friend class MNMesh;
	int nv_alloc, nf_alloc;
public:
	MNMapFace *f;
	UVVert *v;

	int numv, numf;

	/*! \remarks Constructor. This calls <b>Init()</b>.\n\n
	  */
	MNMap () { Init(); }
	/*! \remarks Destructor. This calls <b>ClearAndFree()</b>. */
	~MNMap () { ClearAndFree (); }

	// Initialization, allocation:
	/*! \remarks Initializes the map - sets numv and numf to 0, and sets the
	pointers to NULL. Also initializes private allocation related data members.
	*/
	DllExport void Init ();
	/*! \remarks Allocates the specified number of UVVerts in the <b>v</b>
	array. (If you're creating a number of map verts, but you're not sure
	exactly how many, it's good to pre-allocate a large number using this
	method.) This method doesn't affect MNMap::numv, only the allocation
	amount.
	\par Parameters:
	<b>int num</b>\n\n
	The number of UVVerts to allocate.\n\n
	<b>bool keep=TRUE</b>\n\n
	If TRUE any previous verts are kept; otherwise they are discarded. */
	DllExport void VAlloc (int num, bool keep=TRUE);
	/*! \remarks Allocates the specified number of map faces in the <b>f</b>
	array. This method doesn't affect MNMap::numf, just the number allocated.
	\par Parameters:
	<b>int num</b>\n\n
	The number of map verts to allocate.\n\n
	<b>bool keep=TRUE</b>\n\n
	If TRUE any previous map faces are kept; otherwise they are discarded. */
	DllExport void FAlloc (int num, bool keep=TRUE);

	// Data access:
	/*! \remarks Returns the current number of UVVerts. */
	int VNum () const { return numv; }
	/*! \remarks Returns the 'i-th' UVVert.
	\par Parameters:
	<b>int i</b>\n\n
	The zero based index into the <b>v</b> array of the UVVert to return. */
	UVVert V(int i) const { return v[i]; }
	/*! \remarks Returns the number of map faces.\n\n
	  */
	int FNum () const { return numf; }
	/*! \remarks Returns a pointer to the 'i-th' map face.
	\par Parameters:
	<b>int i</b>\n\n
	The zero based index of the map face to return. */
	MNMapFace *F(int i) const { return &(f[i]); }

	// Adding new components -- all allocation should go through here!
	/*! \remarks Like MNMesh::NewTri, this creates a new mapping face, of
	degree 3, with the specified map vertices.
	\par Parameters:
	<b>int a</b>\n\n
	<b>int b</b>\n\n
	<b>int c</b>\n\n
	The mapping vertices for the new triangle. */
	DllExport int NewTri (int a, int b, int c);
	/*! \remarks Like MNMesh::NewTri, this creates a new mapping face, of
	degree 3, with the specified map vertices.
	\par Parameters:
	<b>int *vv</b>\n\n
	The mapping vertices for the new triangle. */
	DllExport int NewTri (int *vv);
	/*! \remarks Like MNMesh::NewQuad, this creates a quad, with the specified
	map vertices.
	\par Parameters:
	<b>int a</b>\n\n
	<b>int b</b>\n\n
	<b>int c</b>\n\n
	<b>int d</b>\n\n
	The mapping vertices for this quad. (The two faces are formed with mapping
	verts (a,b,c) and (c,d,a). */
	DllExport int NewQuad (int a, int b, int c, int d);
	/*! \remarks Like MNMesh::NewQuad, this creates a quad, with the specified
	map vertices.
	\par Parameters:
	<b>int *vv</b>\n\n
	The mapping vertices for this quad. (The two faces are formed with mapping
	verts (vv[0],vv[1],vv[2]) and (vv[2],vv[3],vv[0]). */
	DllExport int NewQuad (int *vv);
	/*! \remarks Like MNMesh::NewFace, this creates a new mapping face with
	the specified degree and map vertices.
	\par Parameters:
	<b>int degg=0</b>\n\n
	The degree of the new face. (Note that this should match the degree of the
	associated MNFace.)\n\n
	<b>int *vv=NULL</b>\n\n
	The new mapping vertices, in order around the perimeter. */
	DllExport int NewFace (int degg=0, int *vv=NULL);
	/*! \remarks Sets the number of map faces keeping any previously allocated
	faces.
	\par Parameters:
	<b>int nfnum</b>\n\n
	The number of faces to set. */
	DllExport void setNumFaces (int nfnum);
	/*! \remarks Allocates storage for a new specified UVVert. An optional
	offset to the UVVert may be supplied. (The offset is useful when you want
	to create a new UVVert which is on the other side of a seam in U or V from
	some existing UVVert p.)
	\par Parameters:
	<b>UVVert p</b>\n\n
	The UVVert to store.\n\n
	<b>int uoff=0</b>\n\n
	If non-zero this is an offset applied in U when p is stored.\n\n
	<b>int voff=0</b>\n\n
	If non-zero this is an offset applied in V when p is stored. */
	DllExport int NewVert (UVVert p, int uoff=0, int voff=0);
	/*! \remarks Sets the number of UVVerts keeping any previously allocated
	UVVerts.
	\par Parameters:
	<b>int nvnum</b>\n\n
	The number of UVVerts to set. */
	DllExport void setNumVerts (int nvnum);

	/*! \remarks Eliminates the mapping vertices not used by any active
	mapping faces in this map. (Analogous to the Mesh::DeleteIsoMapVerts
	method.)
	\par Parameters:
	<b>MNFace *faces</b>\n\n
	This should point to the MNFace array of the parent MNMesh. This allows the
	method to find out if any faces should be considered MN_DEAD, so that it
	won't mark such faces' map verts as used. (The MN_DEAD flag is stored in
	the MNFace, but not in the related MNMapFaces.) */
	DllExport void CollapseDeadVerts (MNFace *faces);	// Figures out which are dead.
	/*! \remarks Eliminates the MN_DEAD map faces from the array. This should
	be called immediately before calling CollapseDeadFaces on the parent
	MNMesh, to keep the face arrays in sync.
	\par Parameters:
	<b>MNFace *faces</b>\n\n
	The parent MNMesh's array of MNFaces. This array is used to find out which
	faces are MN_DEAD. */
	DllExport void CollapseDeadFaces (MNFace *faces);
	/*! \remarks Sets the number of UVVerts and map faces to 0. */
	DllExport void Clear ();	// Deletes everything.
	/*! \remarks Sets the number of UVVerts and map faces to 0 and deallocates
	the memory. */
	DllExport void ClearAndFree ();	// Deletes everything, frees all memory

	/*! \remarks Transforms each UVVert with the specified matrix.
	\par Parameters:
	<b>Matrix3 \&xfm</b>\n\n
	The matrix to transform the UVVerts by. */
	DllExport void Transform (Matrix3 & xfm);	// o(n) -- transforms verts

	// operators and debug printing
	/*! \remarks Assignment operator. */
	DllExport MNMap & operator= (const MNMap & from);
	/*! \remarks Appends the specified MNMap object onto this MNMap.
	\par Parameters:
	<b>const MNMap \&from</b>\n\n
	The source MNMap. */
	DllExport MNMap & operator+= (const MNMap & from);
	/*! \remarks Appends a default planar map corresponding to the mesh passed
	onto this map. This is typically used when joining two MNMeshes together
	when one MNMesh has a map channel active but the other doesn't. Rather than
	eliminate the map channel, we just use the object coordinates of the other
	mesh to generate a planar map for its faces.
	\par Parameters:
	<b>const MNMesh \&from</b>\n\n
	The source MNMesh. */
	DllExport MNMap & operator+= (const MNMesh & from);
	/*! \remarks Copies pointers. To avoid memory errors, this method should
	only be used by the pipeline.
	\par Parameters:
	<b>const MNMap \& from</b>\n\n
	The map to copy pointers from. */
	DllExport void ShallowCopy (const MNMap & from);
	/*! \remarks Creates new pointers and copies over all the data. To avoid
	memory errors, this method should only be used by the pipeline. */
	DllExport void NewAndCopy ();
	/*! \remarks Uses DebugPrint to print out information about this MNMap to
	the Debug Results window in DevStudio. This includes all map verts and
	faces, one per line.
	\par Parameters:
	<b>MNFace *faces</b>\n\n
	This should be the MNFace array of the parent MNMesh. It's used to prevent
	the DebugPrinting of MN_DEAD faces. */
	DllExport void MNDebugPrint (MNFace *faces);
	/*! \remarks Checks the MNMap for internal errors, such as a MNMapFace
	referring to an out of range map vertex. If an error is found, an error
	message is DebugPrinted, and a the method returns FALSE. This is strictly a
	debugging tool of no use in releases - a good way to use it is to say
	DbgAssert (CheckAllData()), so it won't be called when not in a Debug
	build, and so it'll throw up an assertion failure if something's wrong.\n\n
	Error messages generated:\n\n
	Map %d: Wrong number of faces. Should be %d, is %d. (Sent if nf !=
	MNMap::numf)\n\n
	Map %d, Face %d has wrong degree. Should be %d, is %d. (Sent if f[i].deg !=
	faces[i].deg)\n\n
	Map %d, Face %d has wrong hdegree. Should be %d, is %d. (Sent if f[i].hdeg
	!= faces[i].hdeg)\n\n
	Map %d, Face %d has an out-of-range map vertex: %d.\n\n
	Map %d, Face %d has an out-of-range hidden map vertex: %d
	\par Parameters:
	<b>int mp</b>\n\n
	The map channel this MNMap represents - used to give more helpful debug
	information.\n\n
	<b>int nf</b>\n\n
	The number of faces of the parent MNMesh.\n\n
	<b>MNFace *faces</b>\n\n
	The parent MNMesh's face array. This is used to prevent checking of MN_DEAD
	faces, which may safely be invalid. */
	DllExport bool CheckAllData (int mapChannel, int nf, MNFace *faces);

	/*! \remarks Used internally to save this MNMap to the 3ds Max file. */
	DllExport IOResult Save (ISave *isave, MNFace *faces=NULL);
	/*! \remarks Used internally to load this MNMap from the 3ds Max file. */
	DllExport IOResult Load (ILoad *iload, MNFace *faces=NULL);
};

// Per-edge data
#define MAX_EDGEDATA 10
#define EDATA_KNOT 0
#define EDATA_CREASE 1

DllExport int EdgeDataType (int edID);
DllExport void *EdgeDataDefault (int edID);

// MNMesh::flags definitions
/*! \defgroup meshFlags Mesh Flags
The following represent flags describing various flags that can be set on the mnmesh.
*/
//@{
#define MN_MESH_NONTRI (1<<0)				//!< At least 2 triangles have been joined
#define MN_MESH_FILLED_IN (1<<1)			//!< All topological links complete
#define MN_MESH_RATSNEST (1<<2)				//!< Set if we've replicated points to avoid rats' nest meshes.
#define MN_MESH_NO_BAD_VERTS (1<<3)			//!< Set if we've established that each vert has exactly one connected component of faces & edges.
#define MN_MESH_VERTS_ORDERED (1<<4)		//!< Set if we've ordered the fac, edg tables in each vert.
#define MN_MESH_FACE_NORMALS_INVALID (1<<6)	//!< Set if any of the face normals (fnorm) have become invalid - making this flag instead of deleting fnorm all the time
#define MN_MESH_HAS_VOLUME (1<<7)			//!< Some subset of mesh describes closed surface of solid
#define MN_MESH_HITTEST_REQUIRE_ALL (1<<8)	//!< Force faces to be hit only if all triangles are hit.  (Internal use.)
#define MN_MESH_PARTIALCACHEINVALID (1<<16) //!< MNMesh flag that means that the mnmesh has only set some of the vertices as invalid and not to reprocess the entire mesh just the vertices that changed
//@}

#define MN_MESH_CACHE_FLAGS (MN_MESH_FILLED_IN|MN_MESH_NO_BAD_VERTS|MN_MESH_VERTS_ORDERED)

// These flags are for internal use only.
#define MN_MESH_TEMP_1 (1<<13)
#define MN_MESH_TEMP_2 (1<<14)
//watje internal use flag
#define MN_MESH_DONTTRISTRIP (1<<15)


class MNMeshBorder;
class MNChamferData;
class MNChamferData10;
class MNFaceClusters;

// MNMesh selection levels:
// MNM_SL_CURRENT is an acceptable argument to methods that take a selection level,
// indicating "use the current mesh selection level".
enum PMeshSelLevel { MNM_SL_OBJECT, MNM_SL_VERTEX, MNM_SL_EDGE, MNM_SL_FACE, MNM_SL_CURRENT };

// MNMesh display flags
#define MNDISP_VERTTICKS 0x01
#define MNDISP_SELVERTS	0x02
#define MNDISP_SELFACES	0x04
#define MNDISP_SELEDGES	0x08
#define MNDISP_NORMALS 0x10
#define MNDISP_SMOOTH_SUBSEL 0x20
#define MNDISP_BEEN_DISP 0x40
#define MNDISP_DIAGONALS 0x80
// CAL-03/14/03: ANDY: New display flag here for the behavior we want.
#define MNDISP_HIDE_SUBDIVISION_INTERIORS 0x100

// Flags for sub object hit test
// NOTE: these are the same bits used for object level.
//#define SUBHIT_SELONLY		(1<<0)
//#define SUBHIT_UNSELONLY	(1<<2)
//#define SUBHIT_ABORTONHIT	(1<<3)
//#define SUBHIT_SELSOLID		(1<<4)

#define SUBHIT_MNUSECURRENTSEL (1<<22)		// When this bit is set, the sel only and unsel only tests will use the current level (edge or face) selection when doing a vertex level hit test
#define SUBHIT_OPENONLY		(1<<23)
#define SUBHIT_MNVERTS		(1<<24)
#define SUBHIT_MNFACES		(1<<25)
#define SUBHIT_MNEDGES		(1<<26)
#define SUBHIT_MNDIAGONALS  (1<<27)		// Hit-test on "diagonals" of polygons - MNDiagonalHitData is the type of HitData returned.
#define SUBHIT_MNTYPEMASK	(SUBHIT_MNVERTS|SUBHIT_MNFACES|SUBHIT_MNEDGES|SUBHIT_MNDIAGONALS)

///  \brief 
/// This class is a new form of HitData used for hit-testing diagonals of polygons in Editable Poly and Edit Poly.
///
///  <br><b>Example: </b>  See example usage in the Editable Poly object, in maxsdk/samples/mesh/EditablePoly,
/// as well as in the Edit Poly modifier, in maxsdk\\samples\\mesh\\EditPoly 
class MNDiagonalHitData : public HitData {
public:
	///  \brief 
	/// mFace and mDiag are the data members describing the face and diagonal hit.
	/// (For each face with degree "deg", there are deg-3 diagonals.)
	/// 
	int mFace, mDiag;

	///  \brief 
	/// Constructor.  Sets mFace and mDiagonal to the face and diagonal given.
	/// 
	MNDiagonalHitData (int face, int diagonal) : mFace(face), mDiag(diagonal) { }

	///  \brief 
	/// Destructor.  Doesn't do anything.
	/// 
	~MNDiagonalHitData () { }
};

// Subdivision flags:
#define MN_SUBDIV_NEWMAP 0x01
#define MN_SUBDIV_HIDE_INTERNAL_EDGES 0x10

/*! \sa  Class FlagUser, Class MNFace, Class MNVert, Class MNEdge, Class MNMap, Class PerData, Class MNMeshBorder, Class Mesh,  Class BitArray,  Template Class Tab, Class MNMeshBorder, <a href="ms-its:listsandfunctions.chm::/idx_R_mnmesh_notes_on_debugging.html">MNMesh Notes on Debugging</a>.\n\n
\par Description:
This class is available in release 2.0 and later only.\n\n
The MNMesh class is provided for temporary use by plug-ins, to help with
complex topology-based modifications to Meshes. It has certain capabilities,
such as the ability to recognize faces with more than 3 sides, that are useful
in some applications. It is not suitable for use as a pipeline object. All
methods of this class are implemented by the system.\n\n
MNMesh has a winged-edge structure and a highly interlinked topology. This is
very useful for some modifiers, but requires far more memory \& processing time
than an equivalent normal mesh. It's convenient for the programmer, but may be
sluggish for the user.\n\n
Although some methods can be a bit slow, none of them are non-linear in the
amount of time they take. Even the tessellation algorithms, which can as much
as quadruple the size of the meshes, operate locally, vertex by vertex, so they
are order-of-n algorithms.\n\n
Note: You must <b>#include "MNMATH.H"</b> to use this class as it's not
included by default by <b>MAX.H</b>.
\par Method Groups:
See <a href="class_m_n_mesh_groups.html">Method Groups for Class MNMesh</a>.
\par Data Members:
<b>MNVert *v;</b>\n\n
This data member is available in release 3.0 and later only.\n\n
The array of MNMesh vertices.\n\n
<b>MNEdge *e;</b>\n\n
This data member is available in release 3.0 and later only.\n\n
The array of MNMesh edges.\n\n
<b>MNFace *f;</b>\n\n
This data member is available in release 3.0 and later only.\n\n
The array of MNMesh faces.\n\n
<b>int numv</b>\n\n
This data member is available in release 3.0 and later only.\n\n
The number of MNVerts in <b>v</b>.\n\n
<b>int nume</b>\n\n
This data member is available in release 3.0 and later only.\n\n
The number of MNEdges in <b>e</b>.\n\n
<b>int numf</b>\n\n
This data member is available in release 3.0 and later only.\n\n
The number of MNFaces in <b>f</b>.\n\n
<b>int numm;</b>\n\n
This data member is available in release 3.0 and later only.\n\n
The number of MNMaps in <b>m</b>.\n\n
<b>PerData *vd;</b>\n\n
This data member is available in release 3.0 and later only.\n\n
The array of PerData objects which maintain and provide access to the floating
point vertex data. There is one of these for each supported channel. PerData
objects are used to store such information as vertex weighting and weighted
selections.\n\n
<b>BitArray vdSupport;</b>\n\n
This data member is available in release 3.0 and later only.\n\n
This bit array indicates if a particular vertex data channel is supported in
this MNMesh. If the bit is set the channel is supported.\n\n
<b>PerData *ed;</b>\n\n
This data member is available in release 3.0 and later only.\n\n
The array of PerData objects which maintain and provide access to the floating
point edge data. There is one of these for each supported channel. One PerData
object store edge weighting for NURMS-type MeshSmooths.\n\n
<b>BitArray edSupport;</b>\n\n
This data member is available in release 3.0 and later only.\n\n
This bit array indicates if a particular edge data channel is supported in this
MNMesh. If the bit is set the channel is supported.\n\n
<b>DWORD selLevel;</b>\n\n
This data member is available in release 3.0 and later only. Note however that,
since MNMeshes are not yet used as pipeline objects, the subobject selection
methods are not yet used in 3ds Max. Third party developers are welcome to use
these methods themselves, but they should be aware that this is a
work-in-progress.\n\n
The current MNMesh selection level. One of the following values:\n\n
<b>MNM_SL_OBJECT</b> - object level selection.\n\n
<b>MNM_SL_VERTEX</b> - select vertices\n\n
<b>MNM_SL_EDGE</b> - select edges\n\n
<b>MNM_SL_FACE</b> - select polygon faces\n\n
<b>DWORD dispFlags;</b>\n\n
This data member is available in release 3.0 and later only. Note however that,
since MNMeshes are not yet used as pipeline objects, the display methods and
flags are not yet used in 3ds Max. Third party developers are welcome to use
the display methods themselves, but they should be aware that this is a
work-in-progress.\n\n
The display flags. One or more of the following values:\n\n
<b>MNDISP_VERTTICKS</b> - Display vertex tick marks.\n\n
<b>MNDISP_SELVERTS</b> - Display selected vertices.\n\n
<b>MNDISP_SELFACES</b> - Display selected faces.\n\n
<b>MNDISP_SELEDGES</b> - Display selected edges.\n\n
<b>MNDISP_NORMALS</b> - Display face normals.\n\n
<b>MNDISP_SMOOTH_SUBSEL</b> - Analogous to the Mesh display flag
MESH_SMOOTH_SUBSEL, this indicates whether we should display smooth faces with
selection-color outlines (TRUE) or transparent shaded faces (FALSE).\n\n
<b>MNDISP_BEEN_DISP</b> - Set when the MNMesh has been displayed (at the end of
the render method.)\n\n
<b>MNDISP_DIAGONALS</b> - Set when diagonals should be displayed.\n\n
<b>Tab\<int\> *vedg;</b>\n\n
This data member is available in release 3.0 and later only.\n\n
This is an array of int Tabs that records which edges use each vertex. For
instance, if edges 3, 5, and 6 have an endpoint at vertex 0, vedg[0] would
include 3, 5, and 6, though not in any particular order. This replaces the 2.5
MNVert::edg data member. Note that this information is only valid if the
MN_MESH_FILLED_IN flag is set.\n\n
<b>Tab\<int\> *vfac;</b>\n\n
This data member is available in release 3.0 and later only.\n\n
This is an array of int Tabs that records which faces touch each vertex. For
instance, if faces 0, 1, 3, 8, and 9 use vertex 0 as one of their corners,
vfac[0] would include 0, 1, 3, 8, and 9, though not in any particular order.
This replaces the 2.5 MNVert::fac data member. Note that this information is
only valid if the MN_MESH_FILLED_IN flag is set.
\par Flags:
For more information on flags, see Class FlagUser.\n\n
<b>MN_MESH_NONTRI</b>\n\n
If set, this mesh's faces are not all triangles -- at least one face has more
than 3 sides.\n\n
<b>MN_MESH_FILLED_IN</b>\n\n
If set, all topological links, such as the list of edges, are complete.\n\n
<b>MN_MESH_RATSNEST</b>\n\n
Some regular Meshes have more than two faces referencing the same edge, or more
than one referencing it in the same direction. These are termed "rats' nest
meshes". Since our edge structure only permits 1 or 2 faces (one in each
direction), these meshes are unacceptable. Upon conversion, certain vertices \&
edges are replicated (increasing the vertex count) to separate these into
regular, non-rats'-nest parts. If this happens, this flag is set to let you
know this change has occurred. In particular, converting this MNMesh back into
a regular Mesh will produce a Mesh with more vertices than you started
with.\n\n
<b>MN_MESH_NO_BAD_VERTS</b>\n\n
This indicates that the mesh has had its vertices checked and "bad" ones
eliminated by EliminateBadVerts .\n\n
<b>MN_MESH_VERTS_ORDERED</b>\n\n
This indicates that the mesh has had its vertices ordered by the method
OrderVerts.\n\n
<b>MN_MESH_HAS_VOLUME</b>\n\n
This flag is available in release 2.5 or later only.\n\n
This mesh has at least one connected component which contains volume, i.e.
represents a solid object with no gaps or holes. The flag is set by the
<b>MNMesh::FindOpenRegions()</b> method.\n\n
<b>MN_USER(1\<\<16)</b>\n\n
Flag bits at or above MN_USER are reserved in all MNMesh components for the
plug-in developer, if needed. Since FlagUser-derived classes have 32 flag bits,
this allows for up to 16 user-defined flags.  */
class MNMesh : public FlagUser, public BaseInterfaceServer {
	friend class HardwareMNMesh;
	friend void gfxCleanup(void *data);
	friend class MNNormalSpec;
	friend class MNMeshUtilites8Impl;
	friend class MNMeshUtilities10Impl;
	friend class MNMeshFGBGObject;

private:
	static int		refCount;
	static HANDLE	workMutex;
	static HANDLE	workEndEvent;

	int nv_alloc, ne_alloc, nf_alloc, nm_alloc;

	// Cache geometric data for quick rendering
	Box3 bdgBox;
	Point3 *fnorm;
	RVertex *rVerts;		// <<< instance specific.
	GraphicsWindow *cacheGW;  		// identifies rVerts cache

	//these are counter of various derived data so we can track when to delete them when they become unused
	UINT		mDXMeshCacheDrawCount;  //this the number of times that the direct mesh cache has been used to draw the mesh exclusively
	UINT		mRedrawCountFromLoad;  //this is the number of times the mesh has been redrawn from load.  This is used to prevent an initial memory spike on load
										//this will be set to -1 if the mesh was created and not loaded
	UINT		mBoxDisplayCount;  //this the number of times that the direct mesh cache has been drawn as a box exclusively


	// SCA revision for Max 5.0 - value of 0 for unbuilt, 1 for built according to legacy 4.0 scheme,
	// and 2 for built according to new scheme - this is so that we refresh normals if the user changes
	// the "Use Legacy 4.0 Vertex Normals" checkbox.
	int normalsBuilt;
	float norScale;

	// Hidden map channels
	// There are always NUM_HIDDENMAPS of these, but we dynamically allocate them
	// so that M(mapChannel) can be a const method.  (This wouldn't work if we had hmap[NUM_HIDDENMAPS].)
	MNMap *hmap;
	// Formerly public normal map channels:
	MNMap *m;

	// Vertex color arrays - for display use.
	int curVCChan;	// current map channel to use for colors (default = 0)
	UVVert *curVCVerts;	// possible external color array (default = NULL)
	MNMapFace *curVCFaces;	// possible external face array (default = NULL)

	DWTab		norInd;			// indirection array for fast normal lookup
	int			normalCount;	// total number of normals
	Point3 *	gfxNormals;		// flattened list of normals

	// Derived arrays to contain generated texture coordinates
	int			numTexCoords[GFX_MAX_TEXTURES];
	Point3 *	texCoords[GFX_MAX_TEXTURES];

	// Derived table of TriStrips, depends on topology
//	Tab<TriStrip *>	*tstab; // OLP Doesn't seem to be used anymore (HardwareMNMesh redeclares it). Should remove at next SDK breakage

	// Internal part of SabinDoo method:
	void SDVEdgeFace (int id, int vid, int *fv, Tab<Tab<int> *> & fmv, int selLevel);

	// Internal part of recursive smoothing-group search.
	DWORD FindReplacementSmGroup (int ff, DWORD os, int call);

	// Internal parts of Boolean. (MNBool.cpp)
	int BooleanZip (DWORD sortFlag, float weldThresh);
	bool BoolZipSeam (Tab<int> *lpi, Tab<int> *lpj, int & starth, int & startk, float weldThresh);
	void BoolPaintClassification (int ff, DWORD classification);

	// Internal data cache stuff (MNPipe.cpp)
	void buildBoundingBox ();
	void buildFaceNormals ();

	GraphicsWindow *getCacheGW() { return cacheGW; }
	void setCacheGW (GraphicsWindow *gw) { cacheGW = gw; }

	//Just some temporary structs to hold a list of only changed faces and verts.  They are used when building normals.
	BitArray        mInvalidVerts;
	BitArray        mInvalidFaces;
	Tab<int>	mInvalidVertList;
	Tab<int>	mInvalidFaceList;

	//! \brief This only draws the selected edges used to draw them over the directX cache 
	/*!	This draws the selected edges of the mnmesh.  Backface culling is not supported
	\param[in] gw The graphics window associated with the draw.
	*/
	void	DrawOnlySelectedEdges(GraphicsWindow *gw);

	//! \brief This only draws the selected faces used to draw them over the directX cache 
	/*!	This draws the selected faces of the mnmesh.  Backface culling is not supported
	\param[in] gw The graphics window associated with the draw.
	*/
	void	DrawOnlySelectedFaces(GraphicsWindow *gw);


	//! \brief This draws the normals vectors of the mnmesh 
	/*!	This draws the normals vectors of the mnmesh.  	
	\param[in] gw The graphics window associated with the draw.
	*/
	void	DrawNormals(GraphicsWindow *gw);
	//! \brief This draws the subobject vertex ticks of the mnmesh 
	/*!	This draws the subobject vertex ticks of the mnmesh.  This includes selected vertex and soft selection of the vertices
		In the end this just ends up calling DrawVertexticksOGL_SZB or DrawVertexTicksDX
	\param[in] gw The graphics window associated with the draw.
	*/
	void	DrawVertexTicks(GraphicsWindow *gw);

	// These next 2 functions need to be kept in sync if you make a change to the vertex display
	//! \brief This draws the subobject vertex ticks of the mnmesh for all GFX drivers except Dx9 or using non 3d markers such as the +,0 etc markers
	/*!	This draws the subobject vertex ticks of the mnmesh for all GFX drivers except Dx9 or using non 3d markers such as the +,0 etc markers
	\param[in] gw The graphics window associated with the draw.
	*/
	void	DrawVertexTicksGen(GraphicsWindow *gw);
	//! \brief This draws the subobject vertex ticks of the mnmesh when in DirectX and only using the regular point marker
	/*!	This draws the subobject vertex ticks of the mnmesh when in DirectX and only using the regular point marker
		this is because it uses the DirectX vertex buffer list directly and ONLY SUPPORTS THE DOT MARKER TYPE
	\param[in] gw The graphics window associated with the draw.
	*/
	void	DrawVertexTicksDX(GraphicsWindow *gw);

	/// We have 3 draw mesh functions the drawpoly in the hardware mesh when the DX mesh cache is used
	/// and a regular non cached dx draw, ogl and szb draw
	/// These all need to be kept in sync 
	/// NOTE: This is ONLY FOR DX9
	//! \brief This draws the mesh edges and or shaded triangles in DX when not using the cache
	/*!	This draws the mesh edges and or shaded triangles in DX when not using the cache.
		This typically happens when the mesh has an fx shader on it or is being edited
		PLEASE NOTE: This only applies to DirectX 9.
	\param[in] gw The graphics window associated with the draw.
	\param[in] compFlags One or more of the following flags:
		COMP_TRANSFORM
			Forces recalculation of the model to screen transformation; otherwise attempt to use the cache.
		COMP_IGN_RECT
			Forces all polygons to be rendered; otherwise only those intersecting the box will be rendered.
		COMP_LIGHTING
			Forces re-lighting of all vertices (as when a light moves); otherwise only re-light moved vertices
		COMP_ALL
			All of the above flags.
		COMP_OBJSELECTED
			If this bit is set then the node being displayed by this mesh is selected. Certain display flags only activate when this bit is set.
	\param[in] ma The The list of materials to use to render the mesh. See Class Material.
	\param[in] numMat The number of materials for the MNMesh.
	*/
	void	DrawMeshDX_NoCache(GraphicsWindow *gw, int compFlags, Material *ma, int numMat=-1);

	//! \brief This draws the mesh edges and or shaded triangles in OGL and Dx10
	/*!	This draws the mesh edges and or shaded triangles in OGL and Dx10.
	\param[in] gw The graphics window associated with the draw.
	\param[in] compFlags One or more of the following flags:
		COMP_TRANSFORM
			Forces recalculation of the model to screen transformation; otherwise attempt to use the cache.
		COMP_IGN_RECT
			Forces all polygons to be rendered; otherwise only those intersecting the box will be rendered.
		COMP_LIGHTING
			Forces re-lighting of all vertices (as when a light moves); otherwise only re-light moved vertices
		COMP_ALL
			All of the above flags.
		COMP_OBJSELECTED
			If this bit is set then the node being displayed by this mesh is selected. Certain display flags only activate when this bit is set.
	\param[in] ma The The list of materials to use to render the mesh. See Class Material.
	\param[in] numMat The number of materials for the MNMesh.
	*/
	void	DrawMeshGenHW(GraphicsWindow *gw, int compFlags, Material *ma, int numMat=-1);
	
	//! \brief This draws the mesh edges and or shaded triangles in SZB
	/*!	This draws the mesh edges and or shaded triangles in SZB.
	\param[in] gw The graphics window associated with the draw.
	\param[in] compFlags One or more of the following flags:
		COMP_TRANSFORM
			Forces recalculation of the model to screen transformation; otherwise attempt to use the cache.
		COMP_IGN_RECT
			Forces all polygons to be rendered; otherwise only those intersecting the box will be rendered.
		COMP_LIGHTING
			Forces re-lighting of all vertices (as when a light moves); otherwise only re-light moved vertices
		COMP_ALL
			All of the above flags.
		COMP_OBJSELECTED
			If this bit is set then the node being displayed by this mesh is selected. Certain display flags only activate when this bit is set.
	\param[in] ma The The list of materials to use to render the mesh. See Class Material.
	\param[in] numMat The number of materials for the MNMesh.
	*/
	void	DrawMeshSZB(GraphicsWindow *gw, int compFlags, Material *ma, int numMat=-1);


	// New MNMesh routines to drive HardwareShaders
	// NOTE alot of these were never implemented
	bool	CanDrawTriStrips(DWORD rndMode, int numMtls, Material *mtl);
	bool	BuildTriStrips(DWORD rndMode, int numMtls, Material *mtl);
	void	TriStripify(DWORD rndMode, int numTex, TVFace *tvf[], TriStrip *s, StripData *sd, int vtx);
	void	Draw3DTriStrips(IHardwareShader *phs, int numMat, Material *ma);
	void	Draw3DWireTriStrips(IHardwareShader *phs, int numMat, Material *ma);
	void	Draw3DVisEdgeList(IHardwareShader *phs, DWORD flags);
	int		render3DTriStrips(IHardwareShader *phs, int kmat, int kstrips);
	int		render3DWireTriStrips(IHardwareShader *phs, int kmat, int kstrips);
	int		render3DFaces(IHardwareShader *phs, DWORD index, int *custVis=NULL);

	// Vertex Removal approaches - Luna task 748O
	bool RemoveInternalVertex (int vertex);
	bool RemoveBorderVertex (int vertex);

	// Renders without any color handling - used only in hit-testing at this point.
	void renderSingleDiagonal (GraphicsWindow *gw, int ff, int d);

	// new methods for edge connection with slide and pinch values max8 
	bool ConnectEdges (DWORD edgeFlag, int segments=2, int pinch = 0, int slide = 0 );
	void MultiDivideEdge (int edge, int segments, int pinch, int slide, bool in_flipEdge );

	// new split method with extra distance parameters 
	bool SplitFlaggedVertices (DWORD in_vertexFlag,	float in_distance );
	bool SplitFlaggedEdges ( DWORD in_edgeFlag , float in_distance );

	// new chamfer method with extra open flag 
	bool ChamferVertices (DWORD in_flag, MNChamferData *in_mcd, bool in_open);
	bool ChamferEdges (DWORD in_flag, MNChamferData *in_mcd, bool in_open );

	// new chamfer method with segments parameter
	bool ChamferEdges (DWORD in_flag, MNChamferData10& in_mcd, bool in_open, int in_segments);

public:
	MNVert *v;
	MNEdge *e;
	MNFace *f;
	int numv, nume, numf, numm;

	// Vertex Data -- handled as in Meshes:
	PerData *vd;
	BitArray vdSupport;

	// Edge Data
	PerData *ed;
	BitArray edSupport;

	int selLevel;
	DWORD dispFlags;

	// Derived data:
	Tab<int> *vedg;
	Tab<int> *vfac;

	// Basic class ops
	/*! \remarks Constructor. Initializes the MNMesh with no components and
	the default flags. */
	MNMesh () { Init(); DefaultFlags (); }
	/*! \remarks Constructor. Initializes the MNMesh with the mesh "from", and
	fills in the topology completely (using FillInMesh). */
	MNMesh (const Mesh & from) { Init(); SetFromTri (from); FillInMesh (); }
	/*! \remarks	Copy constructor. */
	MNMesh (const MNMesh & from) { Init(); DefaultFlags(); *this = from; }
	/*! \remarks Destructor. Frees all allocated memory (using Clear). */
	DllExport ~MNMesh();

	// Initialization:
	/*! \remarks Clears all flags. */
	void DefaultFlags () { ClearAllFlags (); }
	/*! \remarks	The data members are initialized as follows:\n\n
	<b>nv_alloc = ne_alloc = nf_alloc = nm_alloc = 0;</b>\n\n
	<b>numv = nume = numf = numm = 0;</b>\n\n
	<b>v = NULL;</b>\n\n
	<b>e = NULL;</b>\n\n
	<b>f = NULL;</b>\n\n
	<b>m = NULL;</b>\n\n
	<b>vd = NULL;</b>\n\n
	<b>ed = NULL;</b>\n\n
	<b>vedg = NULL;</b>\n\n
	<b>vfac = NULL;</b>\n\n
	<b>bdgBox.Init();</b>\n\n
	<b>fnorm = NULL;</b>\n\n
	<b>dispFlags = 0;</b>\n\n
	<b>rVerts = NULL;</b>\n\n
	<b>cacheGW = NULL;</b>\n\n
	<b>normalsBuilt = 0;</b>\n\n
	<b>norScale = 0.0f;</b>\n\n
	<b>selLevel = MNM_SL_OBJECT;</b> */
	DllExport void Init ();

	// Array allocation: these functions (& Init) have sole control over nvalloc, etc.
	/*! \remarks	Allocates and inits the specified number of MNVerts.
	\par Parameters:
	<b>int num</b>\n\n
	The number of verts to allocate.\n\n
	<b>bool keep=TRUE</b>\n\n
	If TRUE any previous verts are kept; otherwise they are discarded. */
	DllExport void VAlloc (int num, bool keep=TRUE);
	/*! \remarks	Shrinks the nv_alloc size of the MNVert array to the specified size.
	\par Parameters:
	<b>int num=-1</b>\n\n
	The new size of the array. The default -1 means to shrink array allocation
	to <b>numv</b>. */
	DllExport void VShrink (int num=-1);	// default means "Shrink array allocation to numv"
	/*! \remarks	Deallocates any MNEdges in the <b>vedg</b> table and sets the <b>vedg</b>
	pointer to NULL. */
	DllExport void freeVEdge();
	/*! \remarks	Allocates the <b>vedg</b> array. */
	DllExport void VEdgeAlloc();
	/*! \remarks	Deallocates the <b>vfac</b> array. */
	DllExport void freeVFace();
	/*! \remarks	Allocates the <b>vfac</b> array. */
	DllExport void VFaceAlloc();
	/*! \remarks	Allocates the MNEdge data array <b>e</b> with the specified size.
	\par Parameters:
	<b>int num</b>\n\n
	The number of MNEdges to allocate.\n\n
	<b>bool keep=TRUE</b>\n\n
	If TRUE any previously allocated edges are kept; otherwise they are
	discarded. */
	DllExport void EAlloc (int num, bool keep=TRUE);
	/*! \remarks	Reduces the ne_alloc size of the MNEdge data array <b>e</b> to the
	specified number of elements.
	\par Parameters:
	<b>int num=-1</b>\n\n
	The new size for the array. The value -1 means to use the current number of
	edges, nume. */
	DllExport void EShrink (int num=-1);
	/*! \remarks	Allocates the MNFace array <b>f</b> with the specified size.
	\par Parameters:
	<b>int num</b>\n\n
	The number of MNFaces to allocate.\n\n
	<b>bool keep=TRUE</b>\n\n
	If TRUE any previously allocated faces are kept; otherwise they are
	discarded. */
	DllExport void FAlloc (int num, bool keep=TRUE);
	/*! \remarks	Reduces the nf_alloc size of the MNFace data array <b>f</b> to the
	specified number of elements.
	\par Parameters:
	<b>int num=-1</b>\n\n
	The new size for the array. The value -1 means to use the current number of
	faces, numf. */
	DllExport void FShrink (int num=-1);
	/*! \remarks	Allocates the MNMap array <b>m</b> with the specified size.
	\par Parameters:
	<b>int num</b>\n\n
	The number of MNMap elements to allocate.\n\n
	<b>bool keep=TRUE</b>\n\n
	If TRUE any previously allocated MNMaps are kept; otherwise they are
	discarded. */
	DllExport void MAlloc (int num, bool keep=TRUE);
	/*! \remarks	Reduces the nm_alloc size of the MNMap data array <b>m</b> to the specified
	number of elements.
	\par Parameters:
	<b>int num=-1</b>\n\n
	The new size for the array. The value -1 means to use the current number of
	maps, numm. */
	DllExport void MShrink (int num=-1);

	// Access to components
	/*! \remarks Returns the number of vertices. */
	int VNum () const { return numv; }
	/*! \remarks Returns a pointer to the i'th MNVert. */
	MNVert *V(int i) const { return &(v[i]); }
	/*! \remarks Returns the point in the i'th MNVert. P(i) is the same as
	V(i)-\>p. */
	Point3 & P(int i) const { return v[i].p; }
	/*! \remarks Returns the number of edges. */
	int ENum () const { return nume; }
	/*! \remarks Returns a pointer to the i'th MNEdge. */
	MNEdge *E(int i) const { return &(e[i]); }
	/*! \remarks Returns the number of faces. */
	int FNum () const { return numf; }
	/*! \remarks Returns a pointer to the i'th MNFace. */
	MNFace *F(int i) const { return &(f[i]); }
	int MNum () const { return numm; }
	/*! \remarks	Access to the MNMaps in each MNMesh is made private. Instead of using the
	MNMap *m data member, you must now use this accessor method. This accessor
	now accepts a value in the range -<b>NUM_HIDDENMAPS</b> to <b>MNum()</b>.
	\par Parameters:
	<b>int mp</b>\n\n
	The map channel */
	DllExport MNMap *M(int mapChannel) const;
	/*! \remarks	Allocates and initializes the specified number of MNMap elements in the
	<b>m</b> array. Initializing sets the number of verts and faces in the map
	to zero and sets the <b>MN_DEAD</b> flag.
	\par Parameters:
	<b>int mpnum</b>\n\n
	The number of MNMaps to allocate and initialize. */
	DllExport void SetMapNum (int mpnum);
	/*! \remarks	Allocates and initializes basic planar map, or a white map for the vertex
	color channel.
	\par Parameters:
	<b>int mp</b>\n\n
	The map channel to initialize:\n\n
	0: Vertex Color channel.\n\n
	1: Default mapping channel.\n\n
	2 through MAX_MESHMAPS-1: The new mapping channels available in release
	3.0. */
	DllExport void InitMap (int mapChannel);	// Inits to current MNMesh topology.
	/*! \remarks	Clears and frees the specified map channel, setting the MN_DEAD flag.
	\par Parameters:
	<b>int mp</b>\n\n
	The map channel to clear.\n\n
	0: Vertex Color channel.\n\n
	1: Default mapping channel.\n\n
	2 through MAX_MESHMAPS-1: The new mapping channels available in release
	3.0. */
	DllExport void ClearMap (int mapChannel);
	/*! \remarks	Returns the specified UVVert from the specified mapping channel.
	\par Parameters:
	<b>int mp</b>\n\n
	The map channel.\n\n
	0: Vertex Color channel.\n\n
	1: Default mapping channel.\n\n
	2 through MAX_MESHMAPS-1: The new mapping channels available in release
	3.0.\n\n
	<b>int i</b>\n\n
	The zero based index of the UVVert to return. */
	DllExport UVVert MV (int mapChannel, int i) const;
	/*! \remarks	Returns a pointer to the specified MNMapFace from the specified mapping
	channel.
	\par Parameters:
	<b>int mp</b>\n\n
	The map channel:\n\n
	0: Vertex Color channel.\n\n
	1: Default mapping channel.\n\n
	2 through MAX_MESHMAPS-1: The new mapping channels available in release
	3.0.\n\n
	<b>int i</b>\n\n
	The zero based index of the MNMapFace to return. */
	DllExport MNMapFace *MF (int mapChannel, int i) const;
	/*! \remarks Returns the total number of triangles; this is a sum of the
	number of triangles in each face that does not have the MN_DEAD flag set.
	*/
	DllExport int TriNum () const;

	// Per Vertex Data:
	/*! \remarks	Sets the specified number of vertex data elements.
	\par Parameters:
	<b>int ct</b>\n\n
	The number of vertex data elements to set.\n\n
	<b>BOOL keep=FALSE</b>\n\n
	If TRUE any previously allocated elements are kept; otherwise they are
	discarded. */
	DllExport void setNumVData (int ct, BOOL keep=FALSE);
	/*! \remarks	Returns the number of vertex data channels maintained by this MNMesh. */
	int VDNum () const { return vdSupport.GetSize(); }

	/*! \remarks	Returns TRUE if the specified channel of vertex data is available for this
	MNMesh; otherwise FALSE.
	\par Parameters:
	<b>int vdChan</b>\n\n
	The vertex data channel. See
	<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_vertex_data_index_options.html">List of Vertex Data
	Index Options</a>. */
	DllExport BOOL vDataSupport (int vdChannel) const;
	/*! \remarks	Sets if the specified channel of vertex data is supported by this MNMesh.
	\par Parameters:
	<b>int vdChan</b>\n\n
	The vertex data channel. See
	<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_vertex_data_index_options.html">List of Vertex Data
	Index Options</a>.\n\n
	<b>BOOL support=TRUE</b>\n\n
	TRUE to indicate the channel is supported; FALSE to indicate it's not. If
	TRUE is specified then elements are allocated (if needed). If FALSE is
	specified the data for the channel is freed. */
	DllExport void setVDataSupport (int vdChannel, BOOL support=TRUE);
	/*! \remarks	Returns a pointer to the vertex data for the specified channel or NULL if
	the channel is not supported.
	\par Parameters:
	<b>int vdChan</b>\n\n
	The vertex data channel. See
	<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_vertex_data_index_options.html">List of Vertex Data
	Index Options</a>. */
	void *vertexData (int vdChannel) const { return vDataSupport(vdChannel) ? vd[vdChannel].data : NULL; }
	/*! \remarks	Returns a pointer to the floating point vertex data for the specified
	channel of this mesh or NULL if the channel is not supported.
	\par Parameters:
	<b>int vdChan</b>\n\n
	The vertex data channel. See
	<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_vertex_data_index_options.html">List of Vertex Data
	Index Options</a>. */
	float *vertexFloat (int vdChannel) const { return (float *) vertexData (vdChannel); }
	/*! \remarks	Deletes (deallocates) the vertex data for the specified channel.
	\par Parameters:
	<b>int vdChan</b>\n\n
	The vertex data channel. See
	<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_vertex_data_index_options.html">List of Vertex Data
	Index Options</a>. */
	DllExport void freeVData (int vdChannel);
	/*! \remarks	Deallocates the vertex data from all channels and sets the number of
	supported channels to 0. */
	DllExport void freeAllVData ();

	// Two specific vertex data: these VDATA constants are defined in mesh.h
	/*! \remarks	Returns a pointer to the floating point vertex weight data. */
	float *getVertexWeights () { return vertexFloat(VDATA_WEIGHT); }
	/*! \remarks	Sets the channel support for the vertex weights channel
	(<b>VDATA_WEIGHT</b>). */
	void SupportVertexWeights () { setVDataSupport (VDATA_WEIGHT); }
	/*! \remarks	Frees (deallocates) the vertex weight channel data. */
	void freeVertexWeights () { freeVData (VDATA_WEIGHT); }
	/*! \remarks	Returns a pointer to the floating point vertex selection weight data. */
	float *getVSelectionWeights () { return vertexFloat(VDATA_SELECT); }
	/*! \remarks	Sets the channel support for the vertex weights channel
	(<b>VDATA_SELECT</b>). */
	void SupportVSelectionWeights () { setVDataSupport (VDATA_SELECT); }
	/*! \remarks	Frees (deallocates) the vertex selection weight channel data. */
	void freeVSelectionWeights () { freeVData (VDATA_SELECT); }

	// Per Edge Data:
	/*! \remarks	Sets the specified number of edge data elements.
	\par Parameters:
	<b>int ct</b>\n\n
	The number of edge data elements to set.\n\n
	<b>BOOL keep=FALSE</b>\n\n
	If TRUE any previously allocated elements are kept; otherwise they are
	discarded. */
	DllExport void setNumEData (int ct, BOOL keep=FALSE);
	/*! \remarks	Returns the number of edge data channels maintained by this MNMesh. */
	int EDNum () const { return edSupport.GetSize(); }

	/*! \remarks	Returns TRUE if the specified channel of edge data is available for this
	MNMesh; otherwise FALSE.
	\par Parameters:
	<b>int edChan</b>\n\n
	The edge data channel. See
	<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_edge_data_index_options.html">List of Edge Data
	Index Options</a>. */
	DllExport BOOL eDataSupport (int edChannel) const;
	/*! \remarks	Sets if the specified channel of edge data is supported by this MNMesh.
	\par Parameters:
	<b>int edChan</b>\n\n
	The edge data channel. See
	<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_edge_data_index_options.html">List of Edge Data
	Index Options</a>.\n\n
	<b>BOOL support=TRUE</b>\n\n
	TRUE to indicate the channel is supported; FALSE to indicate it's not. */
	DllExport void setEDataSupport (int edChannel, BOOL support=TRUE);
	/*! \remarks	Returns a pointer to the edge data for the specified channel or NULL if the
	channel is not supported.
	\par Parameters:
	<b>int edChan</b>\n\n
	The edge data channel. See
	<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_edge_data_index_options.html">List of Edge Data
	Index Options</a>. */
	void *edgeData (int edChannel) const { return eDataSupport(edChannel) ? ed[edChannel].data : NULL; }
	/*! \remarks	Returns a pointer to the floating point edge data for the specified channel
	of this MNMesh or NULL if the channel is not supported.
	\par Parameters:
	<b>int edChan</b>\n\n
	The edge data channel. See
	<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_edge_data_index_options.html">List of Edge Data
	Index Options</a>. */
	float *edgeFloat (int edChannel) const { return (float *) edgeData (edChannel); }
	/*! \remarks	Deletes (deallocates) the edge data for the specified channel.
	\par Parameters:
	<b>int edChan</b>\n\n
	The edge data channel. See
	<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_edge_data_index_options.html">List of Edge Data
	Index Options</a>. */
	DllExport void freeEData (int edChannel);
	/*! \remarks	Deallocates the edge data from all channels and sets the number of
	supported channels to 0. */
	DllExport void freeAllEData ();

	// One specific edge data: this EDATA constant is defined above
	/*! \remarks	Returns a pointer to the floating point edge knot data. */
	float *getEdgeKnots () { return edgeFloat(EDATA_KNOT); }
	/*! \remarks	Sets the channel support for the edge knot channel (<b>EDATA_KNOT</b>). */
	void SupportEdgeKnots () { setEDataSupport (EDATA_KNOT); }
	/*! \remarks	Frees (deallocates) the edge knot channel data. */
	void freeEdgeKnots () { freeEData (EDATA_KNOT); }

	// Vertex face/edge list methods:
	/*! \remarks This method is available in release 3.0 and later only. It
	replaces the old MNVert::Clear method.\n\n
	Clears and frees the flags and face/edge lists for the specified vertex.
	\par Parameters:
	<b>int vv</b>\n\n
	The vertex to clear. */
	DllExport void VClear (int vv);
	/*! \remarks This method is available in release 3.0 and later only. It
	replaces the old MNVert::Init method.\n\n
	Initializes the specified MNVert, clearing its flags and emptying its face
	\& edge lists (if vfac \& edg are allocated).
	\par Parameters:
	<b>int vv</b>\n\n
	The vertex to initialize. */
	DllExport void VInit (int vv);
	/*! \remarks This method is available in release 3.0 and later only. It
	replaces the old MNVert::FaceIndex method.\n\n
	Returns the index of face ff in the vfac[vv] table.\n\n
	NOTE that if this face cannot be found, or if it cannot be found
	accompanied by edge ee\>-1, it will cause an assertion failure.
	\par Parameters:
	<b>int vv</b>\n\n
	The vertex to check the face list of.\n\n
	<b>int ff</b>\n\n
	The face to look for.\n\n
	<b>int ee=-1</b>\n\n
	In cases where the same face touches this vertex more than once, and is
	therefore represented twice in the vfac table, an optional edge parameter
	is used to specify which instance of the face you want the index of.
	However, if the parent MNMesh doesn't have its vertices ordered (as
	indicated by the MN_MESH_VERTS_ORDERED flag), the extra edge parameter is
	meaningless and should not be used. */
	DllExport int VFaceIndex (int vv, int ff, int ee=-1);
	/*! \remarks This method is available in release 3.0 and later only. It
	replaces the old MNVert::EdgeIndex method.\n\n
	Finds the position of edge ee in this MNVert's vedg table. Unlike
	VFaceIndex (and MNFace's VertIndex and EdgeIndex), each vertex can only
	touch an edge once, so there's no need for an additional parameter.
	(There's no such thing as an edge with both ends at the same vertex.)
	\par Parameters:
	<b>int vv</b>\n\n
	The vertex whose edge list should be searched.\n\n
	<b>int ee</b>\n\n
	The edge to find.
	\return  Returns -1 if edge ee is not in the edge table. */
	DllExport int VEdgeIndex (int vv, int ee);
	/*! \remarks This method is available in release 3.0 and later only. It
	replaces the old MNVert::DeleteEdge method.\n\n
	Finds edge ee in the vedg[vv] table and removes it.
	\par Parameters:
	<b>int vv</b>\n\n
	The vertex from whose edge list the edge should be deleted.\n\n
	<b>int ee</b>\n\n
	The edge to delete from the vertex's edge list.
	\return  Returns -1 if edge ee is not in the edge table.\n\n
	  */
	void VDeleteEdge (int vv, int ee) { if (vedg) vedg[vv].Delete (VEdgeIndex(vv, ee), 1); }
	/*! \remarks This method is available in release 3.0 and later only. It
	replaces the old MNVert::DeleteFace method.\n\n
	Finds face <b>ff</b> in the <b>vfac[vv]</b> table and removes it. NOTE that
	this method causes an assertion failure if face <b>ff</b> is not in the
	<b>vfac</b> table. If ff occurs more than once, which is possible on some
	valid NONTRI meshes, only the first ff is removed.
	\par Parameters:
	<b>int vv</b>\n\n
	The vertex from whose face list the face should be deleted.\n\n
	<b>int ff</b>\n\n
	The face to delete from the vertex's face list. */
	DllExport void VDeleteFace (int vv, int ff);
	/*! \remarks This method is available in release 3.0 and later only. It
	replaces the old MNVert::ReplaceEdge method.\n\n
	Finds edge <b>oe</b> in the <b>vedg[vv]</b> table and replaces it with
	<b>ne</b>. NOTE that this method causes an assertion failure if edge
	<b>oe</b> is not in the <b>vedg</b> table.
	\par Parameters:
	<b>int vv</b>\n\n
	The vertex in whose edge list the edge should be replaced.\n\n
	<b>int oe</b>\n\n
	The edge to replace.\n\n
	<b>int ne</b>\n\n
	The replacement edge. */
	DllExport void VReplaceEdge (int vv, int oe, int ne);
	/*! \remarks This method is available in release 3.0 and later only. It
	replaces the old MNVert::ReplaceFace method.\n\n
	Finds face <b>of</b> in the <b>vfac[vv]</b> table and replaces it with
	<b>nf</b>. NOTE that this method causes an assertion failure if face
	<b>of</b> is not in the <b>vfac</b> table. If <b>of</b> occurs more than
	once, which is possible on some valid NONTRI meshes, only the first of is
	replaced.
	\par Parameters:
	<b>int vv</b>\n\n
	The vertex in whose face list the face should be replaced.\n\n
	<b>int of</b>\n\n
	The face to replace.\n\n
	<b>int nf</b>\n\n
	The replacement. */
	DllExport void VReplaceFace (int vv, int of, int nf);
	/*! \remarks This method is available in release 3.0 and later only. It
	replaces the old MNVert::operator= by allowing the developer to copy face
	and edge adjacency information as well as vertex location and flags.\n\n
	Copies the MNVert data from ov to nv. The face and edge data is copied too
	if appropriate (ie if MN_MESH_FILLED_IN is set and vfac and vedg are
	allocated).
	\par Parameters:
	<b>int nv</b>\n\n
	The destination index.\n\n
	<b>int ov</b>\n\n
	The source index. */
	DllExport void CopyVert (int nv, int ov);	// copies face & edge too if appropriate
	/*! \remarks This method is available in release 3.0 and later only. It
	replaces the old MNVert::MNDebugPrint method.\n\n
	Uses DebugPrint to print out vertex information to the Debug Results window
	in DevStudio. The information consists of the position, edge list, and face
	list. It is generally a good idea to put in a DebugPrint immediately before
	this with the index of the vertex, so you know which vertex is being
	printed out:\n\n
	<b>DebugPrint(_T("Vertex %d: "), vid);</b>\n\n
	<b>MNVDebugPrint(vid);</b>
	\par Parameters:
	<b>int vv</b>\n\n
	The zero based index of the MNVert to debug print. */
	DllExport void MNVDebugPrint (int vv);

	// Adding new components -- all allocation should go through here!
	/*! \remarks Creates a new tri-face. Note that no mapping coords or vertex
	colors can be specified.
	\par Parameters:
	<b>int a,b,c</b>\n\n
	The indices of the vertices that form this triangle.\n\n
	<b>DWORD smG=0</b>\n\n
	The smoothing group(s) assigned to the new face.\n\n
	<b>MtlID mt=0</b>\n\n
	The material ID assigned to the new face.
	\return  Returns the index of the new face created. */
	DllExport int NewTri (int a, int b, int c, DWORD smG=0, MtlID mt=0);
	/*! \remarks Creates a new face of degree 3. Edge selection and visibility
	flags are set to the default: all visible and not selected.
	\par Parameters:
	<b>int *vv</b>\n\n
	The indices of the vertices that form this face. (There must be 3 of
	these.)\n\n
	<b>DWORD smG</b>\n\n
	The smoothing group(s) assigned to this face.\n\n
	<b>MtlID mt</b>\n\n
	The material ID assigned to this face
	\return  The index of the face created. */
	DllExport int NewTri (int *vv, DWORD smG=0, MtlID mt=0);
	/*! \remarks This method will create a new quad.\n\n
	Previous to 4.0 this method used two tri-faces that shared and invisible
	edge.
	\par Parameters:
	<b>int a, b, c, d</b>\n\n
	The indices of the vertices that form this quad.\n\n
	<b>DWORD smG=0</b>\n\n
	The smoothing group(s) assigned to the new faces.\n\n
	<b>MtlID mt=0</b>\n\n
	The material ID assigned to the new faces.
	\return  Returns the index of the quad face created. */
	DllExport int NewQuad (int a, int b, int c, int d, DWORD smG=0, MtlID mt=0);
	/*! \remarks This method will create a new quad.\n\n
	Previous to 4.0 this method used two tri-faces that shared and invisible edge.
	\par Parameters:
	<b>int *vv</b>\n\n
	The indices of the vertices that form this face. (There must be 4 of
	these.)\n\n
	<b>DWORD smG</b>\n\n
	The smoothing group(s) assigned to this face.\n\n
	<b>MtlID mt</b>\n\n
	The material ID assigned to this face
	\return  Returns the index of the quad face created. */
	DllExport int NewQuad (int *vv, DWORD smG=0, MtlID mt=0);
	/*! \remarks This method creates a (single) new face with the characteristics
	given. The default triangulation for a face of this degree is used; if the face
	is not convex, this triangulation may be inappropriate. If this is the case,
	call <b>RetriangulateFace()</b> on this face after it's created.
	\par Parameters:
	<b>int initFace</b>\n\n
	A current face from which smoothing groups, material ID, and flags should be
	copied. If this is NULL, these values are left at their default values.\n\n
	<b>int degg</b>\n\n
	The degree of the face to be created.\n\n
	<b>int *vv</b>\n\n
	The indices of the vertices that form the new face. (There must be degg of
	these.)\n\n
	<b>bool *vis</b>\n\n
	This is an array of visibility bits for the edges of the new face. If this is
	NULL, the default of all edges being visible is used. If this is not NULL,
	there must be degg values.\n\n
	<b>bool *sel</b>\n\n
	This is an array of selection bits for the edges of the new face. If this is
	NULL, the default of all edges not being selected is used. If this is not NULL,
	there must be degg values. */
	DllExport int NewFace (int initFace, int degg=0, int *vv=NULL, bool *vis=NULL, bool *sel=NULL);
	/*! \remarks	Appends the specified number of MNFaces to <b>f</b>.
	\par Parameters:
	<b>int nfnum</b>\n\n
	The number of MNFaces to append.
	\return  The index of the first appended face (ie the old numf). */
	DllExport int AppendNewFaces (int nfnum);
	/*! \remarks	Sets the specified number of MNFaces allocated in <b>f</b>.
	\par Parameters:
	<b>int nfnum</b>\n\n
	The number of MNFaces to set. */
	DllExport void setNumFaces (int nfnum);
	/*! \remarks Edge creation tool. If there is no edge currently joining
	vertices v1 and v2, it creates such an edge. If there is an edge starting
	at v2 and ending on v1, it registers face f as being on the "other side" of
	the edge.
	\par Parameters:
	<b>int v1, v2</b>\n\n
	The start \& end vertices of the edge you wish to register.\n\n
	<b>int f</b>\n\n
	The face on the "left" side of this edge, if you're looking from v1 towards
	v2 with the surface normal above.\n\n
	<b>int fpos</b>\n\n
	The index of this edge in face f. This is used to extract visibility and
	selection information from the face.
	\return  The index of the new edge, or -1 if the edge already exists in the
	specified direction. */
	DllExport int RegisterEdge (int v1, int v2, int f, int fpos);
	/*! \remarks Edge creation tool. Simply makes a new edge from v1 to v2,
	without worrying about whether such an edge may already exist, or what
	faces may be on either side. Since edges are required to have at least one
	valid face on them, using this method obligates the developer to assign f1
	on the new edge themselves.
	\return  The index of the new edge. */
	DllExport int SimpleNewEdge (int v1, int v2);
	/*! \remarks Edge creation tool. Requires the developer to previously
	ascertain that there is no edge from v1 to v2 or from v2 to v1.
	\par Parameters:
	<b>int v1, v2</b>\n\n
	The start \& end vertices of the edge you wish to create.\n\n
	<b>int f</b>\n\n
	The face on the "left" side of this edge, if you're looking from v1 towards
	v2 with the surface normal above.\n\n
	<b>int fpos</b>\n\n
	The index of this edge in face f. This is used to extract visibility and
	selection information from the face.
	\return  The index of the new edge. */
	DllExport int NewEdge (int v1, int v2, int f, int fpos);
	/*! \remarks	Appends the specified number of edges.
	\par Parameters:
	<b>int nenum</b>\n\n
	The number of edges to append. */
	DllExport int AppendNewEdges (int nenum);
	/*! \remarks	Sets the number of MNEdges allocated in <b>e</b>.
	\par Parameters:
	<b>int nenum</b>\n\n
	The number of MNEdges to set. */
	DllExport void setNumEdges (int nenum);
	/*! \remarks	Creates a new vertex (increasing numv) and sets it to the specified point.
	\par Parameters:
	<b>Point3 \&p</b>\n\n
	The point to which the new vert should be initialized. */
	DllExport int NewVert (Point3 & p);
	/*! \remarks This method is available in release 3.0 and later only. It
	replaces the old MNMesh::NewVert (Point3 \&p, MNVert *mv=NULL) method.\n\n
	Creates a new vertex (increasing numv) and initializes it to the point p
	and the flags and other characteristics of vertex vid.
	\par Parameters:
	<b>Point3 \&p</b>\n\n
	The point to which the new vert should be initialized.\n\n
	<b>int vid</b>\n\n
	The index of the existing MNVert from which flags and PerData info should
	be copied. (Only the MN_SEL and MN_TARG flags are copied.) */
	DllExport int NewVert (Point3 & p, int vid);
	/*! \remarks	Creates a new vertex and initializes it to location, flags and other
	characteristics of vertex vid.
	\par Parameters:
	<b>int vid</b>\n\n
	The index of the existing MNVert from which location, flags and PerData
	info should be copied. (Only the MN_SEL and MN_TARG flags are copied.) */
	DllExport int NewVert (int vid);
	/*! \remarks	Creates a new vertex which is a linear combination of two existing
	vertices. The new vertex has the MN_SEL and MN_TARG flags of whichever
	vertex it's closest to. The location and PerData info is interpolated.
	\par Parameters:
	<b>int v1</b>\n\n
	The first vertex to combine.\n\n
	<b>int v2</b>\n\n
	The second vertex to combine.\n\n
	<b>float prop</b>\n\n
	The proportion along the segment from v1 to v2 where the new vertex should
	be located. */
	DllExport int NewVert (int v1, int v2, float prop);
	/*! \remarks	Appends the specified number of MNVerts.
	\par Parameters:
	<b>int nvnum</b>\n\n
	The number of MNVerts to append.
	\return  The index of the first appended vertex (ie the old numv). */
	DllExport int AppendNewVerts (int nvnum);
	/*! \remarks	Sets the number of verts, allocating if needed.
	\par Parameters:
	<b>int nvnum</b>\n\n
	The desired number of vertices in the mesh. */
	DllExport void setNumVerts (int nvnum);

	// To delete, set MN_*_DEAD flag and use following routines, which are all o(n).
	/*! \remarks Removes all MNVerts with the MN_DEAD flag from the list of
	vertices. Also, it re-indexes all the faces' and edges' vertex references
	to maintain mesh integrity. */
	DllExport void CollapseDeadVerts ();
	/*! \remarks Removes all MNEdges with the MN_DEAD flag from the list of
	edges. Also, re-indexes all the faces' and vertices' edge references to
	maintain mesh integrity. */
	DllExport void CollapseDeadEdges ();
	/*! \remarks Removes all MNFaces with the MN_DEAD flag from the list of
	faces. Also, re-indexes all the edges' and vertices' face references to
	maintain mesh integrity. */
	DllExport void CollapseDeadFaces ();
	/*! \remarks Performs all 5 of the above collapse functions, safely
	removing all unused components from this mesh. */
	DllExport void CollapseDeadStructs();
	/*! \remarks Reinitializes all verts, faces, and edges, freeing the data
	members of these components, but not freeing the vertex, edge, and face
	arrays themselves. This option is suitable if you need to clear a MNMesh
	you will be reusing. numv, etc, are set to 0. */
	DllExport void Clear ();	// Deletes everything.
	/*! \remarks	Deletes everything and frees all relevant memory. Leaves you with an empty
	MNMesh with the default flags. */
	DllExport void ClearAndFree ();	// Deletes everything, frees all memory
	/*! \remarks	Deletes the MNVert array and frees any corresponding vertex data. */
	DllExport void freeVerts();
	/*! \remarks	Deletes the MNEdge array and frees and corresponding edge data. */
	DllExport void freeEdges();
	/*! \remarks	Deletes the MNFace array. */
	DllExport void freeFaces();
	/*! \remarks	Deletes the MNMap on the specified map channel.
	\par Parameters:
	<b>int mp</b>\n\n
	The map channel.\n\n
	0: Vertex Color channel.\n\n
	1: Default mapping channel.\n\n
	2 through MAX_MESHMAPS-1: The new mapping channels available in release
	3.0. */
	DllExport void freeMap(int mapChannel);
	/*! \remarks	Deletes and frees all the MNMaps. */
	DllExport void freeMaps();

	// En Masse flag-clearing and setting:
	/*! \remarks Clears all specified flag bits in all MNVerts. */
	void ClearVFlags (DWORD fl) { for (int i=0; i<numv; i++) v[i].ClearFlag (fl); }
	/*! \remarks Clears all specified flag bits in all MNEdges. */
	void ClearEFlags (DWORD fl) { for (int i=0; i<nume; i++) e[i].ClearFlag (fl); }
	/*! \remarks Clears all specified flag bits in all MNFaces. */
	void ClearFFlags (DWORD fl) { for (int i=0; i<numf; i++) f[i].ClearFlag (fl); }
	/*! \remarks	Recursively sets flag on given face and all faces connected to it.
	\par Parameters:
	<b>int ff</b>\n\n
	The face to begin painting the face flag on.\n\n
	<b>DWORD fl</b>\n\n
	The flag to set on these faces.\n\n
	<b>DWORD fenceflags=0x0</b>\n\n
	If nonzero, this represents flags of edges that should not be crossed. In
	this way you can set up a "fence" of edges and set a particular face flag
	on all the faces within that fence. */
	DllExport void PaintFaceFlag (int ff, DWORD fl, DWORD fenceflags=0x0);
	/*! \remarks	Selects or deselects the vertices as specified by on bits in the given bit
	array. If the bit array size is smaller than the number of vertices then
	only those vertices in the bit array are modified.
	\par Parameters:
	<b>const BitArray \& vsel</b>\n\n
	Specifies which vertices to select or deselect. */
	DllExport void VertexSelect (const BitArray & vsel);
	/*! \remarks	Selects or deselects the edges as specified by on bits in the given bit
	array. If the bit array size is smaller than the number of edges then only
	those edges in the bit array are modified.
	\par Parameters:
	<b>const BitArray \& esel</b>\n\n
	Specifies which edges to select or deselect. */
	DllExport void EdgeSelect (const BitArray & esel);
	/*! \remarks	Selects or deselects the faces as specified by on bits in the given bit
	array. If the bit array size is smaller than the number of faces then only
	those faces in the bit array are modified.
	\par Parameters:
	<b>const BitArray \& fsel</b>\n\n
	Specifies which faces to select or deselect. */
	DllExport void FaceSelect (const BitArray & fsel);
	/*! \remarks	Fills the given bit array with the current vertex selection state.
	\par Parameters:
	<b>BitArray \& vsel</b>\n\n
	The results are stored here. Bits which are on indicate selected vertices.
	*/
	bool getVertexSel (BitArray & vsel) { return getVerticesByFlag (vsel, MN_SEL); }
	/*! \remarks	Fills the given bit array with the current edge selection state.
	\par Parameters:
	<b>BitArray \& esel</b>\n\n
	The results are stored here. Bit which are on indicate selected edges. */
	bool getEdgeSel (BitArray & esel) { return getEdgesByFlag (esel, MN_SEL); }
	/*! \remarks	Fills the given bit array with the current face selection state.
	\par Parameters:
	<b>BitArray \& fsel</b>\n\n
	The results are stored here. Bit which are on indicate selected faces. */
	bool getFaceSel (BitArray & fsel) { return getFacesByFlag (fsel, MN_SEL); }
	// In following 3, if fmask is 0 it's set to flags, so only those flags are compared.
	/*! \remarks	Creates a BitArray of all vertices using the specified flags.
	\par Parameters:
	<b>BitArray \& vset</b>\n\n
	The BitArray which is filled in. vset is set to size numv.\n\n
	<b>DWORD flags</b>\n\n
	The flags to search for.\n\n
	<b>DWORD fmask=0x0</b>\n\n
	This optional parameter allows the user to look for particular combinations
	of on and off flags. For instance, if flags is MN_DEAD and fmask is 0, the
	method finds vertices with the MN_DEAD flag set. But if flags is MN_SEL and
	fmask is MN_SEL|MN_DEAD, it would find vertices that have MN_SEL set, but
	don't have MN_DEAD set. */
	DllExport bool getVerticesByFlag (BitArray & vset, DWORD flags, DWORD fmask=0x0);
	/*! \remarks	Creates a BitArray of all edges using the specified flags.
	\par Parameters:
	<b>BitArray \& eset</b>\n\n
	The BitArray which is filled in. eset is set to size nume.\n\n
	<b>DWORD flags</b>\n\n
	The flags to search for.\n\n
	<b>DWORD fmask=0x0</b>\n\n
	This optional parameter allows the user to look for particular combinations
	of on and off flags. For instance, if flags is MN_DEAD and fmask is 0, the
	method finds edges with the MN_DEAD flag set. But if flags is MN_SEL and
	fmask is MN_SEL|MN_DEAD, it would find edges that have MN_SEL set, but
	don't have MN_DEAD set. */
	DllExport bool getEdgesByFlag (BitArray & eset, DWORD flags, DWORD fmask=0x0);
	/*! \remarks	Creates a BitArray of all faces using the specified flags.
	\par Parameters:
	<b>BitArray \& fset</b>\n\n
	The BitArray which is filled in. fset is set to size numf.\n\n
	<b>DWORD flags</b>\n\n
	The flags to search for.\n\n
	<b>DWORD fmask=0x0</b>\n\n
	This optional parameter allows the user to look for particular combinations
	of on and off flags. For instance, if flags is MN_DEAD and fmask is 0, the
	method finds faces with the MN_DEAD flag set. But if flags is MN_SEL and
	fmask is MN_SEL|MN_DEAD, it would find faces that have MN_SEL set, but
	don't have MN_DEAD set. */
	DllExport bool getFacesByFlag (BitArray & fset, DWORD flags, DWORD fmask=0x0);
	/*! \remarks	Sets bits for all faces in the same "element", or connected component, with
	face ff. Faces already selected in fset will be considered "walls" for this
	processing and will not be evaluated. That is, if ff is not selected, but
	there's a ring of faces around it that is, the algorithm will stop at that
	ring.
	\par Parameters:
	<b>int ff</b>\n\n
	The zero based index of the face.\n\n
	<b>BitArray \& fset</b>\n\n
	The bits of the faces in the element are selected in this array. */
	DllExport void ElementFromFace (int ff, BitArray & fset);
	/*! \remarks	Takes a one-sided edge and sets the bits representing this edge's "border".
	(All one-sided edges in MNMeshes can be grouped into chains, end to end,
	that represent boundaries of the mesh. For instance, in a box with one side
	deleted, all the one-sided edges are part of the chain that goes around the
	hole.
	\par Parameters:
	<b>int ee</b>\n\n
	The edge to start looking from. (Note that edge ee should be one-sided -
	e[ee].f2 should be -1.\n\n
	<b>BitArray \& eset</b>\n\n
	The bitarray to return the border's edge set in. */
	DllExport void BorderFromEdge (int ee, BitArray & eset);

	// Following also set visedg, edgsel bits on faces:
	/*! \remarks	This not only sets the edge's MN_EDGE_INVIS flag, it also sets the visedg
	state of the faces on either side. This is the proper way to set edge
	visibility permanently, as the MNEdge flag will be lost if the edge list
	needs to be reconstructed.
	\par Parameters:
	<b>int ee</b>\n\n
	The edge whose visibility should be set.\n\n
	<b>BOOL vis=TRUE</b>\n\n
	The desired visibility - TRUE for visible, FALSE for invisible. */
	DllExport void SetEdgeVis (int ee, BOOL vis=TRUE);
	/*! \remarks	This not only sets the edge's MN_SEL flag, it also sets the edgsel state of
	the faces on either side. This is the proper way to set edge selection
	permanently, as the MNEdge flag will be lost if the edge list needs to be
	reconstructed.
	\par Parameters:
	<b>int ee</b>\n\n
	The edge whose selection state should be set.\n\n
	<b>BOOL sel=TRUE</b>\n\n
	The desired selection state. */
	DllExport void SetEdgeSel (int ee, BOOL sel=TRUE);

	// I/O with regular Meshes.
	/*! \remarks Clears out all current information, and copies in new faces
	\& vertices from "from". */
	void SetFromTri (const Mesh & from) { Clear (); AddTri (from); }
	/*! \remarks Adds vertices and faces in "from" to current MNMesh. */
	DllExport void AddTri (const Mesh & from);	// o(n) -- Add another mesh -- simple union
	/*! \remarks Outputs current MNMesh into the mesh given. Note that even if
	the MNMesh was originally taken from this Mesh, the internal processing may
	have changed PART_TOPO, PART_GEOM, PART_SELECT, PART_MAPPING, or
	PART_VERTCOLOR. */
	DllExport void OutToTri (Mesh & tmesh);	// o(n)

	// Internal computation routines
	// These three build on each other and should go in order:
	// FillInMesh, EliminateBadVerts, OrderVerts.
	/*! \remarks If this mesh does not have the MN_MESH_FILLED_IN flag, this
	method completely recomputes all combinatorial information. It re-creates
	all MNEdges and MNFace::edg, MNVert::edg, and MNVert::fac lists based on
	the information in the MNFace::vtx lists.\n\n
	Since this routine completely reconstructs the combinatorics, it clears the
	MN_MESH_VERTS_ORDERED flag. */
	DllExport void FillInMesh ();	// o(n*5) or so
	/*! \remarks	Fills in the faces' edge arrays based on the edge list. */
	DllExport void FillInFaceEdges (); // o(n).
	/*! \remarks	Fills in the vertex edge and face lists based on the edge list. */
	DllExport void FillInVertEdgesFaces ();	// o(n)
	/*! \remarks A "bad" vertex in this context is one which is shared between
	two distinct boundaries for this mesh. As an example, imagine a union
	between two circles, converted to meshes, that touch at a single vertex.
	This causes a vertex to exist which is on two faces, but four edges. Since
	most vertices are on equal numbers of faces and edges (if they're not on a
	boundary) or on one more edge than face (if they are on a boundary), these
	types of vertices can mess up some forms of processing. This method
	eliminates such vertices by duplicating them, assigning one vertex to each
	boundary. A MNMesh that has gone through this method will have the
	MN_MESH_NO_BAD_VERTS flag set until a method (such as NewFace) is called
	that could conceivably create bad vertices.\n\n
	EliminateBadVerts requires a filled in mesh, and will call FillInMesh if
	the MN_MESH_FILLED_IN flag is not set.
	\par Parameters:
	<b>DWORD flag:</b>\n\n
	This parameter is available in release 4.0 and later only.\n\n
	If nonzero, it indicates that only flagged vertices should be split up if
	"bad".
	\return  False if nothing changed, true if at least one bad vertex was
	found and split.\n\n
	  */
	DllExport bool EliminateBadVerts (DWORD flag=0);	// o(n*8) or so
	/*! \remarks This routine organizes the face and edge lists of each vertex
	such that going counterclockwise around the vertex (with the surface normal
	pointing up), you'll encounter edg[0], fac[0], edg[1], fac[1], etc, ending
	in either the last face if the vertex is not on a boundary or the last edge
	if the vertex is on a boundary. (If the vertex is on a boundary, the first
	\& last edges are part of that boundary.)\n\n
	OrderVerts requires a filled-in mesh with no bad vertices, so it will call
	FillInMesh and/or EliminateBadVerts as needed. */
	DllExport void OrderVerts ();	// o(n*3) or so
	/*! \remarks	This routine organizes the face and edge lists of the specified vertex such
	that going counterclockwise around the vertex (with the surface normal
	pointing towards you), you'll encounter edg[0], fac[0], edg[1], fac[1],
	etc, ending in either the last face if the vertex is not on a boundary or
	the last edge if the vertex is on a boundary. (If the vertex is on a
	boundary, the first \& last edges are part of that boundary.)\n\n
	NOTE: OrderVert requires a filled-in mesh with no "bad" vertices. Failing
	to adequately prepare your mesh may result in a crash. (See the methods
	FillInMesh and EliminateBadVerts for details.) */
	DllExport void OrderVert (int vid);
	/*! \remarks Converts a <b>MN_MESH_NONTRI</b> mesh, with polygon faces and
	possibly hidden vertices, into a completely triangulated mesh, wherein all
	faces have degree 3. This routine is called from OutToTri if the
	<b>MN_MESH_NONTRI</b> flag is set. */
	DllExport void Triangulate ();	// o(n)
	/*! \remarks Triangulates the specified face, splitting it into as many
	faces as are needed (deg-2+hdeg*2) to represent all the triangles.
	\par Parameters:
	<b>int ff</b>\n\n
	Specifies the face to triangulate. */
	DllExport void TriangulateFace (int ff);	// o(triangles)

	// Random useful stuff.
	/*! \remarks Transforms all vertices \& hidden vertices by xfm. */
	DllExport void Transform (Matrix3 & xfm);	// o(n) -- transforms verts
	/*! \remarks Figures out if this mesh is completely closed. Meshes with
	the MN_MESH_RATSNEST flags are automatically considered open. Otherwise,
	each edge is checked to see if it has a face on both sides. If so, the mesh
	is closed. Otherwise, it's open. */
	bool IsClosed() { for (int i=0; i<nume; i++) if (e[i].f2<0) return FALSE; return nume?TRUE:FALSE; } // o(n)
	/*! \remarks Finds the bounding box of all vertices and hidden vertices
	used by face ff. */
	DllExport void FaceBBox (int ff, Box3 & bbox);
	/*! \remarks Calculates the bounding box of the vertices \& hidden
	vertices of this mesh.
	\par Parameters:
	<b>Box3 \& bbox</b>\n\n
	The computed bounding box is placed here.\n\n
	<b>bool targonly</b>\n\n
	If this is TRUE, only targeted vertices are used to compute the bounding
	box. Hidden vertices, which are also normally used, will be ignored in this
	case, since they cannot be targeted. */
	DllExport void BBox (Box3 & bbox, bool targonly=FALSE);
	/*! \remarks	Retrieves a bounding box for the MNMesh.
	\par Parameters:
	<b>Matrix3 *tm</b>\n\n
	Like the corresponding method in class Mesh, this method takes an optional
	transform, so the user can get a bounding box in any desired space (with a
	slower calculation, as all the points must be transformed).\n\n
	<b>bool targonly</b>\n\n
	If set, only vertices with the <b>MN_TARG</b> flag set are used to compute
	the bounding box. */
	DllExport Box3 getBoundingBox (Matrix3 *tm=NULL, bool targonly=FALSE);

	// Methods for handling MN_TARG flags.
	/*! \remarks Sets vertex MN_TARG flags based on the existing MN_SEL flags.
	\par Parameters:
	<b>int selLevel</b>\n\n
	Specifies which components to check for MN_SEL flags. If selLevel is
	MNM_SL_OBJECT, all vertices are targeted. If selLevel is MNM_SL_VERTEX, the
	vertices with MN_SEL flags set also have their MN_TARG flags set. If selLevel
	is MNM_SL_FACE or MNM_SL_EDGE, vertices that touch selected faces or edges,
	respectively, have their MN_TARG flags set.
	\return  The (highly) approximate number of targeted vertices. */
	DllExport int TargetVertsBySelection (int selLevel);	// o(n)
	/*! \remarks Sets edge MN_TARG flags based on the existing MN_SEL flags.
	\par Parameters:
	<b>int selLevel</b>\n\n
	Specifies which components to check for MN_SEL flags. If selLevel is
	MNM_SL_OBJECT, all edges are targeted. If selLevel is MNM_SL_EDGE, the edges
	with MN_SEL flags set also have their MN_TARG flags set. If selLevel is
	MNM_SL_FACE or MNM_SL_VERTEX, edges that touch selected faces or vertices,
	respectively, have their MN_TARG flags set.
	\return  The (highly) approximate number of targeted edges. */
	DllExport int TargetEdgesBySelection (int selLevel);	// o(n)
	/*! \remarks Sets face MN_TARG flags based on the existing MN_SEL flags.
	\par Parameters:
	<b>int selLevel</b>\n\n
	Specifies which components to check for MN_SEL flags. If selLevel is
	MNM_SL_OBJECT, all faces are targeted. If selLevel is MNM_SL_FACE, the faces
	with MN_SEL flags set also have their MN_TARG flags set. If selLevel is
	MNM_SL_EDGE or MNM_SL_VERTEX, faces that touch selected edges or vertices,
	respectively, have their MN_TARG flags set.
	\return  The (highly) approximate number of targeted faces. */
	DllExport int TargetFacesBySelection (int selLevel);	// o(n)
	/*! \remarks	This allows the developer to set flags in one type of component based on what
	the nearby flags of another type of component are. For instance, you might want
	to set MN_TARG on all vertices that are used by faces with the MN_SEL flag set;
	that would be PropegateComponentFlags (MESH_VERTEX, MN_TARG, MESH_FACE,
	MN_SEL);\n\n
	Another example:\n\n
	PropegateComponentFlags (MNM_SL_OBJECT, MN_MESH_FILLED_IN, MNM_SL_EDGE,
	MN_DEAD, FALSE, FALSE);\n\n
	This would clear the MN_MESH_FILLED_IN flag from the MNMesh if any of its
	MNEdges were dead.
	\par Parameters:
	<b>DWORD slTo</b>\n\n
	The selection level of the components you wish to set. This would be one of
	MNM_SL_OBJECT, MNM_SL_VERTEX, MNM_SL_EDGE, or MNM_SL_FACE.\n\n
	<b>DWORD flTo</b>\n\n
	The flag to set.\n\n
	<b>DWORD slFrom</b>\n\n
	The selection level of the components you wish to base the selection upon. This
	would be one of MNM_SL_OBJECT, MNM_SL_VERTEX, MNM_SL_EDGE, or MNM_SL_FACE.\n\n
	<b>DWORD flFrom</b>\n\n
	The flag to test.\n\n
	<b>bool ampersand=FALSE</b>\n\n
	When <b>slFrom</b> and <b>slTo</b> are different, this indicates whether the
	flags of the nearby components should be "or'd" or "and'd". If it's false, then
	any flagged components in the "from" level will cause the component in the "to"
	level to be affected. If true, then all the components in the "from" level that
	touch a component in the "to" level must be flagged in order for the "to" level
	component to be affected. (i.e., if from is faces and to is vertices, a vertex
	would only be modified if all faces that use it have the <b>flFrom</b> flag
	set.)\n\n
	<b>bool set=TRUE</b>\n\n
	If TRUE (as would be usual) the slTo components have flags flTo set. If FALSE,
	these flags would be cleared instead.
	\return  Returns the number of slFrom components that tested positive for the
	flFrom flags. (If 0, nothing happened.) */
	DllExport int PropegateComponentFlags (int slTo, DWORD flTo,
		int slFrom, DWORD flFrom, bool ampersand=FALSE, bool set=TRUE);
	/*! \remarks Clears the MN_TARG flag on vertices representing relatively
	flat areas, keeping the flag on vertices at sharper corners. The purpose of
	this method is to avoid working on smooth areas of the mesh in, for
	instance, tessellation algorithms which are designed to smooth an area. An
	example of this is the sharpness option in 3ds Max's MeshSmooth modifier,
	although the parameter there is 1.0 - this value.\n\n
	Note that this method requires filled in geometry and ordered vertices, and
	will call FillInMesh and OrderVerts as needed.
	\par Parameters:
	<b>float sharpval</b>\n\n
	The threshold for determining whether a vertex is sharp enough to continue
	being targeted. A value of 0 indicates that all vertices are sharp enough;
	a value of 1 would de-target all vertices. The actual determination is made
	by comparing the dot product of any two consecutive face normals is less
	that 1 - 2*sharpval.\n\n
	The following "fence" methods are useful for algorithms such as SabinDoo
	wherein you don't want to mix faces with different characteristics. */
	DllExport void DetargetVertsBySharpness (float sharpval);	// o(n*deg)

	// Face-center methods
	/*! \remarks Finds the centers of all the faces, using repeated calls to
	ComputeCenter.
	\par Parameters:
	<b>Point3 *ctr</b>\n\n
	An array of at least FNum() points for storing the centers.\n\n
	<b>bool targonly</b>\n\n
	If this is TRUE, centers will only be computed for targeted faces. (The
	rest of the ctr array will remain unmodified.) */
	DllExport void ComputeCenters (Point3 *ctr, bool targonly=FALSE);	// o(n)
	/*! \remarks Finds the center of face ff by finding the average of all its
	vertices. */
	DllExport void ComputeCenter (int ff, Point3 & ctr);
	/*! \remarks	Computes a "balanced" normal, in that the normal takes the contribution of
	all vertices equally. (This is significant in the case of nonplanar
	polygons.)
	\par Parameters:
	<b>int ff</b>\n\n
	The face you want the normal of.\n\n
	<b>Point3 \& N</b>\n\n
	The place to store the computed normal.\n\n
	<b>Point3 *ctr</b>\n\n
	If not NULL, it points to a place where ComputeNormal should put the face
	center. */
	DllExport void ComputeNormal (int ff, Point3 & normal, Point3 *ctr=NULL);
	/*! \remarks Finds the "safe" centers of all the faces, using repeated
	calls to ComputeSafeCenter.
	\par Parameters:
	<b>Point3 *ctr</b>\n\n
	An array of at least FNum() points for storing the centers.\n\n
	<b>bool targonly</b>\n\n
	If this is TRUE, centers will only be computed for targeted faces. (The
	rest of the ctr array will remain unmodified.)\n\n
	<b>bool detarg</b>\n\n
	If TRUE, this will remove the MN_TARG flag from faces where safe centers
	could not be found. */
	DllExport void ComputeSafeCenters (Point3 *ctr, bool targonly=FALSE, bool detarg=FALSE);	// o(n)
	/*! \remarks Finds the "safe" center of face ff, if possible. For
	non-convex faces, the average point found in ComputeCenter is unsuitable
	for some applications because it can lie outside the face completely, or in
	a region where it cannot "see" all the faces' vertices. (I.e., line
	segments from the center to the corner pass outside of the face.)\n\n
	This routine provides a better solution in some cases by finding the center
	of the convex hull of the face. The convex hull is defined as the region in
	a face with a clear line-of-sight to all the corners. Some faces, such as
	the top face in an extruded letter M, have an empty convex hull, in which
	case this routine fails and merely provides the regular center given by
	ComputeCenter.
	\return  Returns TRUE if a safe center was found, FALSE if no such center
	could be found. */
	DllExport bool ComputeSafeCenter (int ff, Point3 & ctr);	// o(deg^2)

	// Triangulation-of-polygon methods:
	/*! \remarks	Throws out the current triangulation for face ff and computes a new one.
	Note that hidden vertices that actually fall outside of the region of the
	face will be thrown out during this routine, since they cannot be
	incorporated into any triangulation and don't make sense anyway. */
	DllExport void RetriangulateFace (int ff);	// o(deg^2)
	/*! \remarks Finds a diagonal of face ff that does not include any hidden
	vertices. This can be used with the method FindFacePointTri to get a
	sub-triangle and barycentric coordinates for a hidden vertex, so that
	vertex can be kept in the face when the non-hidden vertices are moved in
	some way.
	\par Parameters:
	<b>int ff</b>\n\n
	The face to get an external diagonal of.\n\n
	<b>int *diag</b>\n\n
	An array of at least (F(ff)-\>deg-2)*3 elements to store the diagonals in.
	*/
	DllExport void FindDiagonals (int ff, int *diag);
	/*! \remarks	This method finds diagonals for this sequence of vertices, creating a
	triangulation for the polygon they form.
	\par Parameters:
	<b>int deg</b>\n\n
	The number of vertices in the sequence.\n\n
	<b>int *vv</b>\n\n
	The array of vertices.\n\n
	<b>int *diag</b>\n\n
	A pointer to an array of size (deg-3)*2 where the diagonals can be put. */
	DllExport void FindDiagonals (int deg, int *vv, int *diag);
	/*! \remarks	If the given face is convex, this will often produce a better diagonals
	than RetriangulateFace would. The diagonals are less likely to overuse any
	single vertex. The face is not checked for convexity, but if it is not
	convex the diagonals produced will probably be self-overlapping.
	\par Parameters:
	<b>int ff</b>\n\n
	The face to find a diagonal for.\n\n
	<b>int *diag=NULL</b>\n\n
	If NULL, the new diagonals are put into the face's tri array. If this tri
	is non-NULL, the diagonals are put here instead. Be sure that tri is
	allocated with space for at least (deg-2+hdeg)*3 elements. */
	DllExport void BestConvexDiagonals (int ff, int *diag=NULL);
	/*! \remarks	Uses a triangulation scheme optimized for convex polygons to find a set of
	diagonals for this sequence of vertices, creating a triangulation for the
	polygon they form.
	\par Parameters:
	<b>int deg</b>\n\n
	The number of vertices in the sequence.\n\n
	<b>int *vv</b>\n\n
	The array of vertices.\n\n
	<b>int *diag</b>\n\n
	A pointer to an array of size (deg-3)*2 where the diagonals can be put. */
	DllExport void BestConvexDiagonals (int deg, int *vv, int *diag);
	DllExport bool SetDiagonal (int ff, int d1, int d2);

	// Normal methods
	// NOTE that these do not make use of user-specified normals or smoothing groups at all.
	/*! \remarks Finds an edge connecting these two vertices, if one exists.
	This algorithm is relatively fast, since it just checks the edges in
	V(vrt1)'s edge list. However, it requires that the MNMesh be filled in, and
	it WILL call the much slower FillInMesh routine if the MN_MESH_FILLED_IN
	flag is not set.
	\return  The index of the desired edge, or -1 if no such edge could be
	found. */
	DllExport int FindEdgeFromVertToVert (int vrt1, int vrt2);	// o(deg)
	/*! \remarks	Chooses a suitable "local space", and sets tm's rotation to match that
	space. Does not set tm's translation component. (This is easily done by
	setting tm.SetRow (3, v[vrt].p).)\n\n
	The purpose of this method is to support a consistent definition of "local
	space" around a given vertex. As usual, Z comes from the local normal; the
	X direction is chosen to be a particular edge, but then this direction is
	modified to balance the contribution of the other edges. (Thus it points
	roughly in the direction of one edge, but moving other edges' far endpoints
	will rotate which way is considered "X" just as much as moving that edge
	will.)
	\par Parameters:
	<b>int vrt</b>\n\n
	The vertex index.\n\n
	<b>Matrix3 \& tm</b>\n\n
	The transformation matrix. */
	/*! \remarks	Chooses a suitable "local space", and sets tm's rotation to match that
	space. Does not set tm's translation component. This is easily done by
	setting tm.SetRow (3, v[vrt].p).
	\par Parameters:
	<b>int vrt</b>\n\n
	The vertex index.\n\n
	<b>Matrix3 \& tm</b>\n\n
	The transformation matrix. */
	DllExport void GetVertexSpace (int vrt, Matrix3 & tm);	// o(deg)
	/*! \remarks Returns the surface normal at the vertex vrt. It computes the
	normal by taking the average of the face normals from faces that V(vrt) is
	on, weighted by the angles of the vrt corners of each of those faces. That
	is, if vrt is at a very acute angle on one face, but a very obtuse angle on
	the next, the face with the obtuse vrt corner will count much more heavily.
	The return value has a length of 1.\n\n
	This normal is not related to the normals used in rendering. */
	DllExport Point3 GetVertexNormal (int vrt);	// o(deg)
	/*! \remarks Returns the surface normal at the edge <b>ed</b>. This is
	just the average of the face normals on either side, or, if this is an edge
	with only one face, it's just that face's normal. The return value has a
	length of 1.\n\n
	This normal is not related to the normals used in rendering. */
	DllExport Point3 GetEdgeNormal (int ed);	// o(deg)
	/*! \remarks Returns the surface normal of face fc. If this face has
	degree 3, this is the same as the regular 3ds Max normal for this face.
	However, if the face is more complex, the normal may not be the same as the
	ones for the component triangles.
	\par Parameters:
	<b>int fc</b>\n\n
	The index of the face to find the normal for.\n\n
	<b>bool nrmlz</b>\n\n
	Whether or not to scale the result to length 1. If this is FALSE, the
	length of the return value corresponds (in planar faces) to the area of the
	face, times 2. */
	DllExport Point3 GetFaceNormal (int fc, bool nrmlz=FALSE);	//o(deg)
	/*! \remarks This merely combines GetEdgeNormal (int ed) with FindEdge
	(int vrt1, int vrt2). It returns the normal of the edge connecting vrt1 to
	vrt2, if such an edge can be found. (If there is no such edge, it returns
	Point3(0.0f, 0.0f, 0.0f).) */
	Point3 GetEdgeNormal (int vrt1, int vrt2) { return GetEdgeNormal (FindEdgeFromVertToVert(vrt1, vrt2)); }
	/*! \remarks	Computes the angle at the given edge. (In other words, the angle between
	the planes of the faces on either side.) Note that in the case of nonplanar
	polygons, these planes are the "average" planes for the polygon, not the
	plane of the triangle nearest the edge.
	\par Parameters:
	<b>int ed</b>\n\n
	The edge index.
	\return  The angle in radians. */
	DllExport float EdgeAngle (int ed);
	/*! \remarks	Flips the normal of the specified face, as well as the corresponding map
	faces in active maps. Note that doing this on an isolated face with
	neighbors will cause an illegal condition. Use the FlipElementNormals
	method to safely and completely flip the normals of entire elements to
	avoid this problem.\n\n
	This method uses the MNFace and MNMapFace methods "Flip", which changes the
	order of the vertices from (0,1,2...,deg-1) to (0, deg-1, deg-2,...1) and
	rearrange the edge and diagonal information accordingly.
	\par Parameters:
	<b>int faceIndex</b>\n\n
	The face index. */
	/*! \remarks	This method flips the normal of the indicated face. This is done by
	reordering the vertices. The faces for any assigned texture map is handled
	as well.
	\par Parameters:
	<b>int faceIndex</b>\n\n
	The index of the face for which to flip the normal. */
	DllExport void FlipNormal(int faceIndex);

	// Smoothing-group handling
	/*! \remarks Applies new smoothing (or removes smoothing groups) from
	selected areas of the MNMesh. With the default parameters, it smooths all
	faces with the same group.
	\par Parameters:
	<b>bool smooth</b>\n\n
	If TRUE, Resmooth will generate a new smoothing group (using GetNewSmGroup)
	to apply to the relevant faces. All smoothing groups previously assigned to
	the faces will be removed. If FALSE, Resmooth will strip all smoothing
	groups, leaving the faces faceted.\n\n
	<b>bool targonly</b>\n\n
	If TRUE, Resmooth will not affect faces that are do not have the MN_TARG
	flag set.\n\n
	<b>DWORD targmask</b>\n\n
	Resmooth will only affect faces whose smoothing groups are included in
	targmask. Some examples: with the default value of ~0x0, this is no
	restriction at all. With a value of 0, Resmooth will only affect faces that
	previously had no smoothing group assigned. With a value of 0x02, Resmooth
	will only affect faces that either had smoothing group 2 (and nothing else)
	or no smoothing group.\n\n
	Note: if <b>targonly</b> is TRUE and targmask is not at the default, a face
	must both be targeted and have its smoothing groups fall into targmask in
	order to be affected. */
	DllExport void Resmooth (bool smooth=TRUE, bool targonly=FALSE, DWORD targmask=~0x0);	// o(n)
	/*! \remarks Finds what smoothing groups, if any, are common to all faces
	in this MNMesh.
	\par Parameters:
	<b>bool targonly</b>\n\n
	If this is TRUE, this routine will find smoothing groups that are common to
	all faces with MN_TARG set, ignoring the rest. */
	DllExport DWORD CommonSmoothing (bool targonly=FALSE);	// o(n)
	/*! \remarks Produces a single smoothing group that is not currently used
	in the MNMesh. If this is impossible because all smoothing groups are used
	(a rare condition), it produces the least commonly used group.
	\par Parameters:
	<b>bool targonly</b>\n\n
	If TRUE, this routine will find a smoothing group not used by any of the
	faces with MN_TARG set. If this is impossible because all smoothing groups
	are used in targeted faces, it produces the least commonly used group. */
	DllExport DWORD GetNewSmGroup (bool targonly=FALSE);	// o(n)
	/*! \remarks Produces the lowest material ID not used in any faces in the
	MNMesh.
	\par Parameters:
	<b>bool targonly</b>\n\n
	If TRUE, this routine will instead find the lowest material ID not used in
	the targeted faces of this MNMesh. */
	DllExport MtlID GetNewMtlID (bool targonly = FALSE); // o(n)
	/*! \remarks Returns a smoothing group that is currently used somewhere in
	the mesh, or returns zero if all faces have no smoothing.
	\par Parameters:
	<b>bool targonly</b>\n\n
	If TRUE, this routine will find a smoothing group used in one of the
	targeted faces, or return zero if all targeted faces have no smoothing. */
	DllExport DWORD GetOldSmGroup (bool targonly=FALSE);	// up to o(n).
	/*! \remarks	Find all smoothing groups used in this mesh.
	\par Parameters:
	<b>bool targonly=FALSE</b>\n\n
	If TRUE, this method will return only smoothing groups set in targeted
	faces, i.e. those with the MN_TARG flag set.
	\return  The return value is a DWORD with every used smoothing bit set. */
	DllExport DWORD GetAllSmGroups (bool targonly=FALSE);	// up to o(n)
	/*! \remarks	Finds available smoothing groups you can use to replace the given smoothing
	group without messing up the mesh's shading. This method recursively looks
	at the entire region of faces sharing this smoother with this face, and it
	finds all smoothing groups used by neighboring faces. Then it returns the
	bits which are not used by any of them.\n\n
	This is useful if, for instance, you want to attach two separate mesh
	components, but you don't want smooth shading across the join. It's used
	internally by SeparateSmGroups.
	\par Parameters:
	<b>int ff</b>\n\n
	The face to start the examination on.\n\n
	<b>DWORD os</b>\n\n
	The old smoothing group you wish to replace.
	\return  All bits that are available to replace the old smoothing group
	with. */
	DllExport DWORD FindReplacementSmGroup (int ff, DWORD os);
	/*! \remarks	Recursively replaces the old smoothing group with the new smoothing group.
	The recursion traverses all faces with the old smoother that share an edge
	or a vertex.
	\par Parameters:
	<b>int ff</b>\n\n
	The face to begin the replacement on\n\n
	<b>DWORD os</b>\n\n
	The old smoothing group\n\n
	<b>DWORD ns</b>\n\n
	The new smoothing group */
	DllExport void PaintNewSmGroup (int ff, DWORD os, DWORD ns);
	/*! \remarks	Changes the smoothing groups on faces using v2 so that they are distinct
	from any smoothing groups on faces using v1. This is used, for instance, in
	joining the seam between the operands MakeBoolean. Before welding each pair
	of vertices, this method is called to prevent smoothing across the boolean
	seam.
	\return  If, due to overuse of the 32 smoothing groups, the algorithm can't
	find enough new ones to replace the overlapping smoothing groups around v2,
	it will do the best it can and return FALSE. If it succeeds, it returns
	TRUE. */
	DllExport bool SeparateSmGroups (int v1, int v2);
	/*! \remarks	Applies new smoothing groups to the whole mesh or to selected faces based
	on angles between faces.
	\par Parameters:
	<b>float angle</b>\n\n
	The threshold angle in radians. Edges with angles above this amount will
	not be smoothed across.\n\n
	<b>BOOL useSel</b>\n\n
	Indicates if the auto-smoothing should be done only on selected faces.\n\n
	<b>BOOL preventIndirectSmoothing</b>\n\n
	Sometimes even though two neighboring faces are more than "angle" apart,
	there may be a path from one to the other via other faces, crossing only
	edges that are less than "angle" apart, so they'll wind up sharing the same
	smoothing group anyway. To prevent this sort of "indirect" smoothing, set
	this value to TRUE. */
	/*! \remarks	This method performs an auto smooth on the mesh, setting the smoothing
	groups based on the surface normals.
	\par Parameters:
	<b>float angle</b>\n\n
	The minimum angle between surface normals for smoothing to be applied, in
	radians.\n\n
	<b>BOOL useSel</b>\n\n
	If TRUE only the selected faces are smoothed.\n\n
	<b>BOOL preventIndirectSmoothing=FALSE</b>\n\n
	TRUE to turn on; FALSE to leave off. This matches the option in the Smooth
	Modifier UI -- use this to prevent smoothing 'leaks" when using this
	method. If you use this method, and portions of the mesh that should not be
	smoothed become smoothed, then try this option to see if it will correct
	the problem. Note that the problem it corrects is rare, and that checking
	this slows the Auto Smooth process. */
	DllExport void AutoSmooth(float angle,BOOL useSel,BOOL preventIndirectSmoothing);

	// Use following to unify triangles into polygons across invisible edges.
	/*! \remarks Turns a mesh with triangle faces into a mesh with (fewer) polygon
	faces by removing all hidden edges. This method can take unusually long if
	there are faces of ridiculously high degree, such as the top of a cylinder with
	200 sides but only 1 cap segment. (It can be up to an n-squared routine where n
	is the number of invisible edges removed to make a given face.)\n\n
	This routine is essentially a bunch of calls to <b>RemoveEdge()</b>, followed
	by a call to <b>EliminateCollinearVerts()</b>. */
	DllExport void MakePolyMesh (int maxdeg=0, BOOL elimCollin=TRUE);
	// NOTE: MakeConvexPolyMesh result not guaranteed for now.  Still requires MakeConvex() afterwards to be sure.
	/*! \remarks	Turns a mesh with triangle faces into a mesh with (fewer) convex polygon faces
	by removing all hidden edges that aren't necessary to maintain convexity. As of
	the 2.5 release this method has not been thoroughly tested to ensure that the
	result is indeed always convex and that it doesn't leave edges that should
	ideally be removed. Therefore a call to MakeConvex is recommended after use.
	What we can say about it is that it will not produce any hidden vertices, as
	MakePolyMesh does, and that if you're going to use MakeConvex anyway,
	MakeConvexPolyMesh and MakeConvex are more efficient than MakePolyMesh and
	MakeConvex are.\n\n
	MakeConvex essentially undoes a lot of the work done by MakePolyMesh.
	MakeConvexPolyMesh gives it much less (if anything) to undo. */
	DllExport void MakeConvexPolyMesh (int maxdeg=0);
	/*! \remarks Kills the edge and joins the faces on either side. Does not
	work with one-sided edges. Re-indexes triangulation in the resulting face
	to maintain MN_MESH_FILLED_IN integrity. This routine also checks the
	resulting face for "dangling" edges: if two faces share a common boundary
	of 3 edges, and the middle edge is removed, the other two edges will
	actually have the same face on both sides. This is silly, so such edges are
	also removed \& killed, reducing the degree of the face and adding a hidden
	vertex. */
	DllExport void RemoveEdge (int edge);
	/*! \remarks Calls <b>MakeFaceConvex()</b> on all (non-dead) faces,
	producing a mesh with 100% convex faces. */
	DllExport void MakeConvex ();
	/*! \remarks Makes face ff convex, essentially by chopping off corners.
	The corners themselves become new faces. This routine is actually
	recursive: if this face is already convex, it returns without affecting it.
	If not, it finds a suitable line (between outside vertices) to divide the
	face along, and then calls <b>MakeFaceConvex()</b> again on both of the
	smaller faces. It is crucial that the face given to MakeFaceConvex has a
	valid triangulation. */
	DllExport void MakeFaceConvex (int ff);
	/*! \remarks	Subdivides polygons as needed so that no polygon has degree larger than
	maxdeg. (For instance, if maxdeg was 4, an octagon would have edges added
	until it was composed of 3 quads, or some combination of quads and tris.)
	\par Parameters:
	<b>int maxdeg</b>\n\n
	Maximum degrees. */
	DllExport void RestrictPolySize (int maxdeg);
	/*! \remarks	Makes all faces planar, within the angle threshold given, by subdividing
	them as necessary. (See MakeFacePlanar.)
	\par Parameters:
	<b>float planarThresh</b>\n\n
	The planar angle threshold. */
	DllExport void MakePlanar (float planarThresh);
	/*! \remarks	Makes the specified face planar by subdividing if necessary. planarThresh
	represents an angle in radians. If the angle across any of the face's
	diagonals is larger than this amount, the face is divided on that diagonal.
	\par Parameters:
	<b>int ff</b>\n\n
	The face of the index to make planar\n\n
	<b>float planarThresh</b>\n\n
	The planar angle threshold. */
	DllExport void MakeFacePlanar (int ff, float planarThresh);
	/*! \remarks This routine scans through a mesh from which invisible edges
	have been removed, looking for vertices that essentially seem to lie in the
	middle of edges. Such "collinear vertices" actually lie between two edges
	that are parallel, with the same faces on either side, and with no other
	edges incident on them. These are easily removed, and if not removed, they
	can cause unpleasant artifacts in some tessellation algorithms.\n\n
	The removal of these vertices consists of deleting them, merging the
	parallel edges into one (longer) edge, and correcting the faces on either
	side to have one less vertex and edge.\n\n
	Note that this is essentially the opposite of the method SplitEdge. */
	DllExport void EliminateCollinearVerts ();
	/*! \remarks This merges vertices that lie extremely close together,
	similar to what's done in Edit Mesh's Weld function, but it only affects
	vertices that are joined by an edge. Another way of looking at it is that
	it uses weld to remove extremely small edges.
	\par Parameters:
	<b>float thresh=MNEPS</b>\n\n
	This is the maximum length of an edge that will be welded away. The default
	value of MNEPS is defined in MNCommon.h to be .0001 -- generally we use
	this to delete edges of more or less zero length.
	*/
	DllExport void EliminateCoincidentVerts (float thresh=MNEPS);

	// Following set NOCROSS flags, delete INVIS flags to make "fences" for Sabin-Doo
	/*! \remarks Sets the MN_EDGE_NOCROSS flag on all edges that lie between
	faces with different material IDs. */
	DllExport void FenceMaterials ();
	/*! \remarks Sets the MN_EDGE_NOCROSS flag on all edges that lie between
	faces with exclusive smoothing groups. */
	DllExport void FenceSmGroups ();
	/*! \remarks Sets the MN_EDGE_NOCROSS flag on all edges that lie between
	selected \& non-selected faces. (This checks the MN_SEL, not the MN_TARG,
	flags on the faces.) */
	DllExport void FenceFaceSel ();
	/*! \remarks Sets the MN_EDGE_NOCROSS flag on all edges that are on the
	boundary. */
	DllExport void FenceOneSidedEdges();
	/*! \remarks	Sets <b>MN_EDGE_NOCROSS</b> flags on edges between faces that aren't in the
	same plane.
	\par Parameters:
	<b>float thresh=.9999f</b>\n\n
	This is the threshold used to determine if two adjacent faces have the same
	normals, i.e. lie in the same plane. If the dot product between the normals
	is less than thresh, they are considered different, otherwise they're
	considered the same. The threshold angle between faces is the arc cosine of
	this amount, so for instance to set a threshold angle of .5 degrees, you
	would call FenceNonPlanarEdges with a thresh of cos(.5*PI/180.). The
	default value is equivalent to about .81 degrees.\n\n
	<b>bool makevis=FALSE</b>\n\n
	Indicates whether nonplanar edges should be made visible, i.e. have their
	MN_EDGE_INVIS flag cleared. (This is sometimes done before MakePolyMesh, so
	it can be used to influence whether nonplanar faces are joined together.)
	*/
	DllExport void FenceNonPlanarEdges (float thresh=.9999f, bool makevis=FALSE);
	/*! \remarks	Sets the MN_EDGE_MAP_SEAM on all edges that are "seams" for any active
	map(s). A map seam is an edge where the faces on either side use different
	mapping vertices for at least one end. */
	DllExport void SetMapSeamFlags ();
	/*! \remarks	Sets the MN_EDGE_MAP_SEAM on all edges that are "seams" for the specified
	map(s). A map seam is an edge where the faces on either side use different
	mapping vertices for at least one end.
	\par Parameters:
	<b>int mp</b>\n\n
	The map to use to set the seams. If left at the default -1, it'll check all
	active maps. */
	DllExport void SetMapSeamFlags (int mapChannel);

	// Find get detail about a point on a face.
	/*! \remarks Finds the sub-triangle and (optionally) barycentric
	coordinates within that triangle of a point that lies on this face. For
	faces that are not themselves triangles, this helps in particular to create
	mapping coordinates or vertex colors for new points on this face. To use
	this method, you must now call <b>MNFace::GetTriangles</b> to get a
	triangle table, then pass the contents of that table to this method. Note
	that the optional size of the triangulation array was removed; this is
	assumed to be (f[ff].deg-2)*3.
	\par Parameters:
	<b>int ff</b>\n\n
	The face.\n\n
	<b>Point3 \& pt</b>\n\n
	The point.\n\n
	<b>double *bary</b>\n\n
	An array of 3 double-precision values to store the barycentric coordinates
	in. If this is NULL, barycentric coordinates are not computed.\n\n
	<b>int *tri</b>\n\n
	An optional alternative triangulation, such as that produced by
	FindExternalTriangulation. (If NULL, the face's regular triangulation is
	used.)
	\return  3 times the index of the triangle the point is found in, or -1 if
	this point doesn't seem to lie on this face. That is, if this point is
	found in the triangle represented by tri[6], tri[7], and tri[8], this
	routine will return 6. */
	DllExport int FindFacePointTri (int ff, Point3 & pt, double *bary, int *tri);
	/*! \remarks This method is available in release 3.0 and later only. It's
	the generalized version of the old FindFacePointCV and FindFacePointTV.\n\n
	Uses FindFacePointTri and the mapping coordinates of the endpoints of the
	relevant triangle to compute a map vertex corresponding to the point pt. If
	the point is not on face ff, UVVert (0,0,0) is returned.\n\n
	This method is useful e.g. for getting map coordinates to match a new
	vertex when dividing up a face.
	\par Parameters:
	<b>int ff</b>\n\n
	The face to find map coordinates on.\n\n
	<b>Point3 \& pt</b>\n\n
	The point (lying on the face) to find map coordinates for.\n\n
	<b>int mp</b>\n\n
	The map channel to get map coordinates in. */
	DllExport UVVert FindFacePointMapValue (int ff, Point3 & pt, int mapChannel);

	// Extrapolate map information about a point near a face.
	/*! \remarks	Given a point near a given edge of a given face, but near to it, this
	method will extrapolate what that point's mapping coordinates would be in
	the mapping scheme used by the face.
	\par Parameters:
	<b>int face</b>\n\n
	The face we want to base mapping information on.\n\n
	<b>int edge</b>\n\n
	The index of the edge we're closest to on the face. (This should be the
	face-based index, in the range (0, deg-1), not the index of the edge in the
	MNMesh.)\n\n
	<b>Point3 \& pt</b>\n\n
	The object-space coordinates of the point.\n\n
	<b>int mp</b>\n\n
	The map channel we're analyzing. */
	DllExport UVVert ExtrapolateMapValue (int face, int edge, Point3 & pt, int mapChannel);

	// Useful for tessellation algorithms
	/*! \remarks Similar to the 3ds Max "Relax" modifier: this modifier moves
	each MNVert towards the average of all vertices to which it is connected
	(by MNEdges).
	\par Parameters:
	<b>float relaxval</b>\n\n
	The proportion to move it. If p is the original location of this vertex and
	q is the average point of all vertices it's connected to, this vertex is
	moved to p*(1-relaxval) + q*relaxval. A value of 0 generates no
	"relaxation", 1 is maximum relaxation, and values below 0 or above 1
	generate non-relaxing results.\n\n
	<b>bool targonly</b>\n\n
	If this is set, only those MNVerts with the MN_TARG flag set will be moved.
	*/
	DllExport void Relax (float relaxval, bool targonly=TRUE);

	// Returns map verts for both ends of each edge (from f1's perspective)
	// (Very useful for creating new faces at borders.) mv[j*2] corresponds to edge j's v1.
	/*! \remarks	Returns map verts for both ends of each edge (from f1's perspective) (Very
	useful for creating new faces at borders.) mv[j*2] is the map vertex
	corresponding to edge j's v1.
	\par Parameters:
	<b>const Tab\<int\> \& lp</b>\n\n
	A loop of border edges, such as is generated by MNMesh::FindBorders.\n\n
	<b>Tab\<int\> \& mv</b>\n\n
	The table into which the map vertices should be put. The size is set to
	2*lp.Count().\n\n
	<b>int mp</b>\n\n
	The map channel we're analyzing. */
	DllExport void FindEdgeListMapVerts (const Tab<int> & lp, Tab<int> & mv, int mapChannel);

	// Following functions can be used to find & fix holes in a mesh, if any.
	/*! \remarks Finds border loops composed of all one-sided edges. (One-sided
	edges on a mesh must necessarily be organizable into closed loops along the
	borders of the mesh.)
	\par Parameters:
	<b>MNMeshBorder \& brd</b>\n\n
	The class in which to put the border loops. See
	Class MNMeshBorder for details.\n\n
	<b>DWORD targetFlag</b>\n\n
	The selection level in the mesh to use to decide whether the border loops are
	targeted or not. For instance, with the default MNM_SL_OBJECT, all border loops
	are targeted, but with a value of MNM_SL_VERTEX, only those border loops
	containing at least one selected vertex will be targeted. Same for MNM_SL_EDGE.
	For MNM_SL_FACE, those loops touching at least one selected face will be
	targeted. */
	DllExport void GetBorder (MNMeshBorder & brd, int selLevel=MNM_SL_OBJECT, DWORD targetFlag=MN_SEL);
	/*! \remarks Fills in all the borders of a mesh. This is what the 3ds Max
	CapHoles modifier does. It creates new faces on the other side of each of
	the loops.
	\par Parameters:
	<b>MNMeshBorder *b</b>\n\n
	If this is NULL, FillInBorders finds all the borders with a call to
	GetBorders. If you have already found the borders, however, you can save
	time by passing a pointer to the MNMeshBorder class here. (This is
	especially convenient if you wish to modify the border targeting; only
	targeted borders are filled in.) */
	DllExport void FillInBorders (MNMeshBorder *b=NULL);
	/*! \remarks	Scans mesh to determine what parts of it are open or closed. Consider a
	"submesh" to be one connected components of faces and edges. Each submesh
	is open if it has any holes, i.e. if there's at least one edge which has
	only one face. (The hole is on the other side.) A submesh is closed if it
	doesn't have any. This method sets the MN_FACE_OPEN_REGION flag on all
	faces in open submeshes, and sets the MN_MESH_HAS_VOLUME flag if at least
	one submesh is closed.
	\par Examples:
	A 3ds Max box has one submesh, itself, and it is closed.\n\n
	A 3ds Max teapot has four distinct submeshes (handle, spout, lid, and
	body), and they are all open. */
	DllExport void FindOpenRegions ();

	// Doubled mapping verts are individual map vertices that are used to correspond
	// to different regular vertices.  For instance, a box could have a single (1,1,0)
	// vertex that it uses in the upper-right corner of all quads.  This design is
	// harmful to some of our algorithms, such as the various Tessellators.  So the
	// following two methods detect and fix such vertices.

	// This method is not really appropriate for release, it's more of a debugging
	// tool.  All doubled mapping verts will be DebugPrinted.
	/*! \remarks	This method is a debugging tool. All doubled mapping verts are
	DebugPrinted. Return value is TRUE if there is at least one doubled mapping
	vertex (on at least one map channel). FALSE if the mesh is clean. Note that
	this method is not significantly faster than EliminateDoubledMappingVerts,
	so you should not use it to determine if EliminateDoubledMappingVerts
	should be called. */
	/*! \remarks	This method is used as a debugging tool. All double mapping vertices will
	be debugprinted. "Doubled" mapping verts are individual map vertices that
	are used to correspond to different regular vertices. For example, the
	standard map on the box object uses the same mapping vertices on the top as
	on the bottom. This can be a problem if the user wants to change the map in
	one region, but not the other. It also causes problems in NURMS MeshSmooth,
	where the user can change weights in one region but not in the other. */
	DllExport BOOL CheckForDoubledMappingVerts();

	// This one's ok and encouraged for release.  (Linear-time algorithm.)
	/*! \remarks	"Doubled" mapping vertices are individual map vertices that are used to
	correspond to different regular vertices. For instance, a box could have a
	single (1,1,0) map vertex that it uses in the upper-right corner of all
	quads. This design is harmful to some of our algorithms, such as the
	various Tessellators. So this method is available to fix such vertices. It
	clones map vertices as needed to ensure that a given map vertex is only
	used by one regular vertex. Linear-time algorithm. */
	/*! \remarks	This method will allow you to eliminate double mapping vertices. "Doubled"
	mapping verts are individual map vertices that are used to correspond to
	different regular vertices. For example, the standard map on the box object
	uses the same mapping vertices on the top as on the bottom. This can be a
	problem if the user wants to change the map in one region, but not the
	other. It also causes problems in NURMS MeshSmooth, where the user can
	change weights in one region but not in the other. */
	DllExport void EliminateDoubledMappingVerts();
	/*! \remarks	Deletes isolated mapping vertices - i.e., mapping vertices that aren't used
	by any mapping faces. */
	/*! \remarks	Eliminates mapping vertices that aren't used by any mapping faces. */
	DllExport void EliminateIsoMapVerts();
	/*! \remarks	Deletes isolated mapping vertices - i.e., mapping vertices that aren't used
	by any mapping faces. If the "int mp" parameter is given, the algorithm
	operates only on that map channel. If not, the algorithm operates on all
	map channels.
	\par Parameters:
	<b>int mp</b>\n\n
	The map channel to operate on. */
	/*! \remarks	Eliminates mapping vertices that aren't used by any mapping faces.
	\par Parameters:
	<b>int mp</b>\n\n
	If mp\>=0, it indicates the mapping channel that should have its isolated
	verts removed. If left at the default, all mapping channels are cleared of
	iso verts. */
	DllExport void EliminateIsoMapVerts(int mapChannel);

	// operators and debug printing (MNFace.cpp)
	/*! \remarks Assignment operator. Allocates space \& copies over all data
	from "from". */
	DllExport MNMesh & operator= (const MNMesh & from);
	/*! \remarks Union operator. Adds all data from "from" to this MNMesh.
	Flags MN_MESH_NONTRI and MN_MESH_RATSNEST are or'd together, while flags
	MN_MESH_FILLED_IN, MN_MESH_NO_BAD_VERTS, and MN_MESH_VERTS_ORDERED are
	and'd together. (Uniting a rat's nest with a non-rat's nest makes a rat's
	nest, but uniting a mesh with mapping coordinates and one without makes one
	without.)\n\n
	  */
	DllExport MNMesh & operator+= (const MNMesh & from);
	/*! \remarks	Uses DebugPrint to print out the entire MNMesh to the Debug Results window
	in DevStudio. This can be useful for tracking down bugs. Be careful not to
	leave MNDebugPrint calls in your final build; they will slow down your
	effect to no purpose.
	\par Parameters:
	<b>bool triprint=FALSE</b>\n\n
	Controls whether or not triangulation information is printed out for each
	face. Usually this information isn't desired. */
	DllExport void MNDebugPrint (bool triprint=FALSE);
	/*! \remarks	Uses DebugPrint to print out information about all the edges, faces, and
	vertices in the immediate vicinity of vertex vv.
	\par Parameters:
	<b>int vv</b>\n\n
	The vertex whose information is output.\n\n
	<b>bool triprint=FALSE</b>\n\n
	Controls whether or not triangulation information is printed out for each
	face. Usually this information isn't desired. */
	DllExport void MNDebugPrintVertexNeighborhood (int vv, bool triprint=FALSE);
	/*! \remarks	Thoroughly checks a MNMesh to make sure that no components refer to dead
	components; that when one component refers to another, the other refers
	back to the first; and that orientation is correctly matched between faces
	and edges. If everything checks out, TRUE is returned. If any errors are
	detected, DebugPrint is used to send a message to the DevStudio Debug
	Results window and FALSE is returned. Be careful not to leave CheckAllData
	calls in your final build; they will slow down your effect to no
	purpose.\n\n
	Here is the sort of debug output you can expect. CheckAllData always
	outputs one of the following messages:\n\n
	Checking all data in filled-in MNMesh: (if the mesh has the
	MN_MESH_FILLED_IN flag set)\n\n
	Checking all data in MNMesh: (if the mesh does not)\n\n
	These errors will be detected for any mesh:\n\n
	Face %d has an out-of-range vertex: %d\n\n
	Face %d has an out-of-range tvert: %d\n\n
	Face %d has an out-of-range cvert: %d\n\n
	Face %d refs dead edge %d.\n\n
	Face %d has bad triangulation index: %d\n\n
	These errors will be detected only for filled-in meshes:\n\n
	Face %d has an out-of-range edge: %d\n\n
	Face %d uses dead edge %d.\n\n
	Face %d refs edge %d, but edge doesn't ref face.\n\n
	Face %d refs edge %d from vert %d to vert %d, but edge doesn't go
	there.\n\n
	Orientation mismatch between face %d and edge %d.\n\n
	Face %d refs vertex %d, but vertex doesn't ref face.\n\n
	Edge %d refs dead vertex %d.\n\n
	Edge %d refs vertex %d, but vertex doesn't ref edge.\n\n
	Edge %d has no face-1\n\n
	Edge %d refs dead face %d.\n\n
	Edge %d refs face %d, but face doesn't ref edge.\n\n
	Vertex %d refs dead edge %d.\n\n
	Vertex %d refs edge %d, but edge doesn't ref vertex.\n\n
	Vertex %d references more faces than edges.\n\n
	Vertex %d refs dead face %d.\n\n
	Vertex %d refs face %d, but face doesn't ref vertex. */
	DllExport bool CheckAllData ();

	// Split functions maintain topological info.  (MNSplit.cpp)
	// Luna task 747: Someday these should all support specified normals - but not today.
	/*! \remarks Creates a new vertex in the middle of an edge, splitting the
	edge into two edges. Unlike SplitEdge, this routine requires a triangle
	face on each side. Since it has this, it also creates new edges connecting
	the new vertex with the point opposite the split edge on each face. This in
	turn splits the faces. So two triangles sharing this single edge become
	four triangles with four shared edges meeting at a single new vertex.\n\n
	This method requires that MN_MESH_FILLED_IN be set (otherwise it will cause
	an assertion failure), and maintains the combinatorics completely.
	\par Parameters:
	<b>int ee</b>\n\n
	The edge to split. Note that if the faces on either side do not have deg=3
	and hdeg=0, an assertion failure will result.\n\n
	<b>float prop</b>\n\n
	The proportion along the edge for the new vertex to be located. This ranges
	from 0, at v1, to 1, at v2. A value of .3, for instance, would create a new
	vertex at .7*P(v1) + .3*P(v2).\n\n
	<b>float thresh</b>\n\n
	If prop is less than thresh or more than 1-thresh, the method will not
	split the edge, and will return v1 or v2 respectively. This is so that
	calling routines can safely pass values anywhere from 0 to 1, without
	worrying about creating tiny "shard" faces.\n\n
	<b>bool neVis</b>\n\n
	This tells whether the new edges that split the faces should be visible.
	(The new edge formed from part of the old edge takes such characteristics
	from the old edge.)\n\n
	<b>bool neSel</b>\n\n
	This tells whether the new edges that split the faces should be selected.
	(The new edge formed from part of the old edge takes such characteristics
	from the old edge.)
	\return  The index of the new vertex created, or the index of the old
	vertex you can use instead if you're within thresh of the endpoints. */
	DllExport int SplitTriEdge (int ee, float prop=.5f, float thresh=MNEPS,
		bool neVis=TRUE, bool neSel=FALSE);
	/*! \remarks Adds a vertex somewhere in a triangle face, and also adds
	edges from that vertex to each of the corners, splitting the face into 3
	smaller triangle faces. The triangle face used here can have hidden
	vertices, as long as it is of degree 3. However, if the threshold parameter
	comes into play and this routine calls SplitTriEdge, both this face and the
	one across the relevant edge will need to have both degree 3. (Otherwise
	there's an assertion failure.)\n\n
	This method requires that MN_MESH_FILLED_IN be set (otherwise it will cause
	an assertion failure), and maintains the combinatorics completely.
	\par Parameters:
	<b>int ff</b>\n\n
	The face to split.\n\n
	<b>double *bary</b>\n\n
	The barycentric coordinates of the new point you wish to add. If this is
	NULL, the default values of (1/3, 1/3, 1/3) (the middle of the triangle)
	are used. These values MUST all fall between 0 and 1, and they MUST add up
	to 1 (give or take a floating point error) to get a sensible result.\n\n
	<b>float thresh</b>\n\n
	If one of the barycentric coordinates is greater than 1-thresh, that vertex
	dominates completely. No splitting is done, and that vertex is returned. If
	not, but if one of the barycentric coordinates is less than thresh, the new
	point must fall on the opposite edge. SplitTriEdge is therefore called on
	that edge, and passes along the value this returns.\n\n
	<b>bool neVis</b>\n\n
	Whether or not the new edges connecting the new vertex to the corners
	should be visible.\n\n
	<b>bool neSel</b>\n\n
	Whether or not the new edges connecting the new vertex to the corners
	should be selected.
	\return  The index of the new vertex created, or the index of the old
	vertex you can use instead if you're within thresh of one of the corners.
	*/
	DllExport int SplitTriFace (int ff, double *bary=NULL, float thresh=MNEPS,
		bool neVis=TRUE, bool neSel=FALSE);
	/*! \remarks This is another way to subdivide a face on a triangular mesh
	into sub-triangles. In this case, 4 new vertices are produced, and this
	face becomes 6 new faces. <b>MN_MESH_FILLED_IN</b> is required and
	preserved, and <b>MN_MESH_NONTRI</b> cannot be true when this method is
	called.\n\n
	The subdivision technique is as follows: a new point is added at the
	barycentric coordinates given on the face given. New points are also added
	in each edge of the original face, such that a line from each of these
	three new edge points to the opposite original vertex passes through the
	new center point. These three edge points have edges drawn between them,
	and have edges to the new center point, dividing the face into 3 large
	outer triangles and 3 smaller inner triangles. Neighboring faces are split
	into 2 triangles, since the common edge is divided. This is a useful
	subdivision technique when you know you're going to want to add a lot of
	detail to a specific region of a face.
	\par Parameters:
	<b>int ff</b>\n\n
	The face to split.\n\n
	<b>double *bary=NULL</b>\n\n
	The barycentric coordinates for the center point. If bary is NULL, the
	default barycentric coordinates of (1/3, 1/3, 1/3) are used.\n\n
	<b>int *nv=NULL</b>\n\n
	This is a pointer to an array of at least 4 int's in which the 4 new vertex
	indices should be stored. (This is mainly if the calling routine needs to
	know what these new vertices are.) If this is NULL, it is ignored. */
	DllExport void SplitTri6 (int ff, double *bary=NULL, int *nv=NULL);
	/*! \remarks	Creates a new vertex somewhere in the middle of an edge, splitting the edge
	into two edges. Incident faces are updated in their vertex, edge, mapping
	coordinate, and vertex color lists, as well as in their triangulation. This
	method requires that MN_MESH_FILLED_IN be set (otherwise it will cause an
	assertion failure), and maintains the combinatorics completely.
	\par Parameters:
	<b>int ee</b>\n\n
	The edge to split.\n\n
	<b>float prop=.5f</b>\n\n
	The proportion along the edge where the new vertex should go. This
	proportion should be between 0 and 1. The new vertex location is
	P(E(ee)-\>v1)*(1.0f-prop) + P(E(ee)-\>v2)*prop.
	\return  The index of the new vertex. */
	DllExport int SplitEdge (int ee, float prop=.5f);
	/*! \remarks	This new SplitEdge variant allows you to recover information about the new
	map vertices created at the point where the edge is split. Everything else
	is the same as the existing SplitEdge (int ee, float prop=.5f) method.
	\par Parameters:
	<b>Tab\<int\> *newTVerts</b>\n\n
	Pointer to a table in which the new map vertices can be stored. This table
	is set to size (NUM_HIDDENMAPS + numm)*2. The two entries for each map
	channel are used to store the two new map vertices at this edge split:
	newTVerts[(NUM_HIDDENMAPS+mp)*2+0] is the map vertex for map channel mp on
	the "f1" side of the edge, and newTVerts[(NUM_HIDDENMAPS+mp)*2+1] is the map vertex for the
	"f2" side if f2\>=0 (or otherwise left uninitialized). (These values are
	often, but not always, the same. They are different if the map has a seam
	along this edge.) */
	DllExport int SplitEdge (int ee, float prop, Tab<int> *newTVerts);
	/*! \remarks	Splits an edge that is specified from the face level, and splits off
	triangles from the adjacent faces. This method is often preferable in
	convex meshes, when it's important that no nonconvex faces are introduced.
	The other SplitEdge methods leave faces with three vertices in a line,
	which is not strongly convex. This version makes a triangle out of half of
	the split edge, the next face edge, and a diagonal.\n\n
	Note: This method absolutely requires that the faces on either side of the
	edge being split are Convex. Use MakeFaceConvex if needed.
	\par Parameters:
	<b>int ff</b>\n\n
	One of the faces which uses the edge you wish to split.\n\n
	<b>int ed</b>\n\n
	The index of the edge in face ff.\n\n
	<b>float prop</b>\n\n
	The proportion along the edge where the new vertex should go. This
	proportion should be between 0 and 1. The new vertex location is
	P(E(ee)-\>v1)*(1.0f-prop) + P(E(ee)-\>v2)*prop.\n\n
	<b>bool right</b>\n\n
	If FALSE, the new triangle is formed from the lower or "left" half of the
	split edge. If TRUE, it's formed from the higher or "right" half. (These
	orientations make sense if you imagine viewing the face from above with the
	split edge on the bottom, as pictured.) If you want the triangle to be
	formed from the smaller half, use (prop\<=.5f) for this argument.\n\n
	<b>int *nf=NULL</b>\n\n
	If non-NULL, this should point to an array of at least 2 elements where the
	new face indices should be put. Nf[0] is set to the new face created from
	face ff, while nf[1] is set to the new face created from the face on the
	other side of the edge, if any.\n\n
	<b>int *ne=NULL</b>\n\n
	If non-NULL, this should point to an array of at least 3 elements where the
	new edge indices should be put. Ne[0] is set to the new edge created from
	the second half of the edge we're splitting. Ne[1] represents the diagonal
	edge on face ff, while ne[2] represents the diagonal edge on the face on
	the other side of the split edge (if any).\n\n
	<b>bool neVis=FALSE</b>\n\n
	Indicates whether the new edges ne[1] and ne[2] should be visible.\n\n
	<b>bool neSel=FALSE</b>\n\n
	Indicates whether the new edges ne[1] and ne[2] should be selected.\n\n
	<b>bool allconvex=FALSE</b>\n\n
	Indicates whether the original faces on both sides of the edge are
	guaranteed to be convex. If so, these faces are retriangulated with
	BestConvexTriangulation; otherwise, RetriangulateFace is used.
	\return  The index of the new vertex. */
	DllExport int SplitEdge (int ff, int ed, float prop, bool right, int *nf=NULL,
		int *ne=NULL, bool neVis=FALSE, bool neSel=FALSE, bool allconvex=FALSE);
	/*! \remarks	Uses a new vertex to "indent" one of the sides of the face. The indentation
	triangle is split off as a new face, which is returned.
	\par Parameters:
	<b>int ff</b>\n\n
	The face to be "indented".\n\n
	<b>int ei</b>\n\n
	The index of the edge which will be replaced by the indentation.\n\n
	<b>int nv</b>\n\n
	The index of the new vertex. This vertex should lie within the face, in the
	face's plane, and should not be connected to any faces or edges.\n\n
	<b>int *ne=NULL</b>\n\n
	If non-NULL, this should point to an array of at least 2 elements where the
	new edges should be stored.\n\n
	<b>bool nevis=TRUE</b>\n\n
	Indicates whether the new edges should be visible.\n\n
	<b>bool nesel=FALSE</b>\n\n
	Indicates whether the new edges should be selected.
	\return  The index of the new face representing the indentation triangle.
	*/
	DllExport int IndentFace (int ff, int ei, int nv, int *ne=NULL, bool nevis=TRUE, bool nesel=FALSE);
	/*! \remarks	This routine takes a larger face and divides it into two smaller faces,
	creating a new edge in the process. It is crucial that the face given to
	SeparateFace has a valid triangulation.
	\par Parameters:
	<b>int ff</b>\n\n
	The face to divide.\n\n
	<b>int a, b</b>\n\n
	The indices (in the face's vtx list) of the vertices used to divide the
	face. Note that a and b should be at least two units apart in each
	direction: a can't be (b+1) mod deg, and b can't be (a+1) mod deg. Also,
	the straight line connecting MNVerts V(vtx[a]) and V(vtx[b]) should not
	cross any of the edges of the face, otherwise the results will not be
	valid.\n\n
	<b>int \& nf</b>\n\n
	A variable to hold the new face created by this division. (Half the face
	remains as face ff, the other half is the new face nf.)\n\n
	<b>int \& ne</b>\n\n
	A variable to hold the new edge created by this division.\n\n
	<b>bool neVis</b>\n\n
	Indicates whether the new edge should be visible. (The other version of
	SeparateFace always leaves the edge invisible.)\n\n
	<b>bool neSel</b>\n\n
	Indicates whether the new edge should be selected. (The other version of
	SeparateFace always leaves the edge unselected.) */
	DllExport void SeparateFace (int ff, int a, int b, int & nf, int & ne, bool neVis=FALSE, bool neSel=FALSE);
	/*! \remarks	Slices the MNMesh along a specified plane, splitting edges and faces as needed
	to divide faces into those above and those below the plane. Equivalent to the
	Slice modifier.
	\par Parameters:
	<b>Point3 \& N</b>\n\n
	The normal to the slice plane. This should be a unit vector.\n\n
	<b>float off</b>\n\n
	The offset of the slice plane. For any point X in the plane, DotProd(N,X) =
	off.\n\n
	<b>float thresh</b>\n\n
	If a vertex lies within thresh of the splitting plane, ie if absf(DotProd (N,X)
	- offset) \< thresh for a point X, it's considered to be on the plane. This
	prevents some points being created extremely close to each other. Zero is an
	acceptable value; the constant MNEPS is what's used in the Slice Modifier.\n\n
	<b>bool split</b>\n\n
	Indicates whether the points and edges along the slice should be replicated,
	dividing the mesh into separate "above" and "below" connected components.
	Equivalent to the Slice modifier's "Split Mesh".\n\n
	<b>bool remove</b>\n\n
	Indicates whether the portion of the mesh above the split plane should be
	removed. Equivalent to the Slice modifier's "Remove Top". (To "Remove Bottom",
	just multiply N and off by -1.)*/
	DllExport bool Slice (Point3 & N, float off, float thresh, bool split, bool remove, bool flaggedFacesOnly=false, DWORD faceFlags=MN_SEL);
	/*! \remarks	Deletes faces with any of the death flags set, as well as delete vertices
	and edges surrounded by faces with death flags, and correct the mesh
	components remaining.
	\par Parameters:
	<b>DWORD deathflags</b>\n\n
	The collection of flags marking the faces you wish to kill.\n\n
	<b>DWORD nvCopyFlags=0x0</b>\n\n
	If the NO_BAD_VERTS flag is set on this mesh, DeleteFlaggedFaces will
	preserve this property. This may involve duplicating some vertices. (See
	EliminateBadVerts for more information.) If you have vertex flags that you
	want preserved in this duplication, indicate them in nvCopyFlags. MN_SEL
	and MN_TARG are always copied, but all other flags are cleared on the new
	vertex. */
	DllExport void DeleteFlaggedFaces (DWORD deathflags, DWORD nvCopyFlags=0x0);
	/*! \remarks	Welds vertices a and b, correcting the mesh topology to match. All
	references to b are moved to a, and b is killed. If there is a topological
	problem preventing the weld from occurring, the method does nothing and
	returns FALSE. If there is no such problem, the weld goes ahead and returns
	TRUE. Note that if a and b are joined by an edge, this method just calls
	WeldEdge on that edge. */
	DllExport bool WeldVerts (int a, int b);
	/*! \remarks	Welds the endpoints of edge ee, correcting the mesh topology to match. All
	references to E(ee)-\>v2 are moved to E(ee)-\>v1, and both ee and
	E(ee)-\>v2 are killed. If there is a topological problem preventing the
	weld from occurring, the method does nothing and returns FALSE. If there is
	no such problem, the weld goes ahead and returns TRUE. */
	DllExport bool WeldEdge (int ee);

	// Tessellation methods: (MNTess.cpp)
	// Luna task 747: Someday these should all support specified normals - but not today.
	/*! \remarks This is a tessellation routine similar to AndersonDo. It
	produces the same topology, but bulges new vertices outward to maintain
	even curvature instead of bringing the original vertices inward. For a
	demonstration of the effect this algorithm has on meshes, apply 3ds Max's
	Tessellate modifier with the "Edge" and "Operate On: Polygons" options
	selected. Note that this algorithm roughly quadruples the size of the
	MNMesh.
	\par Parameters:
	<b>float bulge</b>\n\n
	The factor to "push out" new vertices by in the direction of the surface
	curvature. Values of about .25 are reasonable. This value is equivalent to
	one hundredth of the value specified as "Tension" in the Tessellate
	modifier.\n\n
	<b>MeshOpProgress *mop=NULL</b>\n\n
	A pointer to a MeshOpProgress. See Class MeshOpProgress for details. */
	DllExport void TessellateByEdges (float bulge, MeshOpProgress *mop=NULL);
	/*! \remarks This is a tessellation routine. It breaks every MNFace into deg
	new faces with four sides. The four vertices of each of these new quads come
	from the center point of the original face, one corner of the original face,
	and the middles of the two edges on either side of that corner. For a
	demonstration of the effect this algorithm has on meshes, apply 3ds Max's
	MeshSmooth modifier with the "Quad Output" box checked. Note that this
	algorithm roughly quadruples the size of the MNMesh.
	\par Parameters:
	<b>float interp</b>\n\n
	This is the proportion of relaxation applied to the original vertices to
	produce a smoothed result. Vertices in the result correspond to three sources:
	all the original vertices are still included, new vertices are produced at the
	center of each face, and new vertices are produced in the middle of each edge.
	Face-vertices are always at the exact center of the original faces, but
	edge-vertices and vertex-vertices are relaxed into the mesh to improve
	smoothness.\n\n
	<b>int selLevel</b>\n\n
	The selection level that the Mesh we're modifying was set to. If this selection
	level is MNM_SL_FACE, we'd want to keep non-selected faces as unmodified as
	possible, whereas if the selection level is MNM_SL_OBJECT or MNM_SL_VERTEX, we
	could break off chunks of some unselected faces that touch modified vertices.
	Again, observe the result of MeshSmooth, with Quad Output, on various selection
	sets of a mesh for examples of this behavior.\n\n
	<b>MeshOpProgress *mop=NULL</b>\n\n
	This optional parameter points to an instance of the virtual class
	MeshOpProgress, which is used to allow the user to abort out of lengthy
	calculations. (SabinDoo is a lengthy calculation.) See
	class MeshOpProgress.\n\n
	<b>DWORD subdivFlags=0</b>\n\n
	This parameter is available in release 4.0 and later only.\n\n
	There is currently only one flag for the extra argument,
	<b>MNM_SUBDIV_NEWMAP</b>. If this flag is set, the new mapping scheme will be
	used. */
	DllExport bool AndersonDo (float interp, int selLevel, MeshOpProgress *mop=NULL, DWORD subdivFlags=0);
	/*! \remarks This is a tessellation routine. Note that this algorithm roughly
	triples the size of the MNMesh. For an example of how it works, apply 3ds Max's
	Tessellate modifier with the "Face" and "Operate On: Polygons" options
	selected. This algorithm splits each face into deg triangles, where each
	triangle contains the center of the face and one edge from the perimeter. */
	DllExport void TessellateByCenters (MeshOpProgress *mop=NULL);

	// Sabin-Doo tessellation: (MNSabDoo.cpp)
	// Luna task 747: Someday these should both support specified normals - but not today.
	/*! \remarks This is a tessellation routine. Note that this algorithm roughly
	triples the size of the MNMesh. For an example of how it works, apply 3ds Max's
	MeshSmooth modifier with "classic" MeshSmooth Type.\n\n
	This technique for tessellation is based on a paper, "A Subdivision Algorithm
	for Smoothing Down Irregularly Shaped Polygons", published by D. W. H. Doo of
	Brunel University, Middlesex, England, in IEEE in 1978. It essentially creates
	a new face for every vertex, face, and edge of the original mesh. The new
	vertices this technique requires are made by creating one vertex for each
	corner of each original face. These vertices are located on a line from the
	original face corner to its center. All the faces around a given (targeted)
	vertex will create such a point; these points form the corners of the face
	created from this vertex. The original faces become smaller, as they use the
	new points instead of their old corners. And the four new points created on the
	faces on either side of an edge, for the endpoints of that edge, become the
	four corners of the new face representing the edge. Apply a MeshSmooth with
	default values to a 3ds Max box to see how this plays out.
	\par Parameters:
	<b>float interp</b>\n\n
	The position along the line from a vertex to the center of each face to create
	the new vertex at. If this value is near 0, the original faces will shrink very
	little, and the new faces at the vertices and edges will be very small. If this
	value is near 1, the original faces will shrink to almost nothing, and the new
	vertex faces will be dominant. Again, adjust the Strength parameter in
	MeshSmooth to gain an understanding of this parameter.\n\n
	<b>int selLevel</b>\n\n
	This parameter is included so that special handling can be applied to selected
	faces, if we're at the MNM_SL_FACE selection level. Generally, this algorithm
	works on all targeted vertices, affecting all faces that contain at least one
	targeted vertex. However, if the vertices are targeted by whether or not
	they're on selected faces, we'll wind up "spilling" the algorithm over into the
	non-selected faces. Generally in such routines as MeshSmooth, the user would
	want non-selected faces not to be affected at all. If this is the case, setting
	this value to MNM_SL_FACE will prevent faces without the MN_SEL flag from being
	affected.\n\n
	<b>MeshOpProgress *mop=NULL</b>\n\n
	This optional parameter points to an instance of the virtual class
	MeshOpProgress, which is used to allow the user to abort out of lengthy
	calculations. (SabinDoo is a lengthy calculation.) See
	Class MeshOpProgress.\n\n
	<b>Tab\<Point3\> *offsets=NULL</b>\n\n
	The SabinDoo algorithm is typically used to go from one polygonal approximation
	of a smooth surface to another polygonal approximation of 4 times the size, but
	it never actually returns the vertices as they would appear on the "limit
	surface". If this parameter is non-NULL, it's filled in with offsets that will,
	when added to the vertices in the SabinDoo result, take those vertices onto the
	limit surface. This makes a smoother result that matches better between
	different iterations. */
	DllExport void SabinDoo (float interp, int selLevel, MeshOpProgress *mop=NULL, Tab<Point3> *offsets=NULL);
	/*! \remarks This applies the Sabin-Doo tessellation technique described above
	to a single vertex. The vertex is split into as many new vertices as there are
	faces using this vertex. The incident faces are shrunk back to make use of
	these new vertices, and a new face is created from them representing the
	vertex. Incident edges are split into triangles using their other end and the
	appropriate edge from the new vertex-face. To see the result of this algorithm,
	apply MeshSmooth (with default parameters) to an EditableMesh with a single
	vertex selected.
	\par Parameters:
	<b>int vid</b>\n\n
	The vertex to SabinDoo.\n\n
	<b>float interp</b>\n\n
	The proportion from this vertex to the center of each of the incident faces to
	put the new vertex for that face.\n\n
	<b>int selLevel</b>\n\n
	If this is equal to MNM_SL_FACE, faces without the MN_SEL flag will not be
	affected.\n\n
	<b>Point3 *ctr</b>\n\n
	This is a list of centers of all the faces, which cannot be NULL. Since this
	algorithm changes the original faces, merely computing face centers on the fly
	won't work. (When SabinDooing two vertices on a given face, the second vertex
	will get bad center information.) Thus the user should compute all the face
	centers in advance using ComputeCenters or (preferably) ComputeSafeCenters.\n\n
	<b>MeshOpProgress *mop=NULL</b>\n\n
	This optional parameter points to an instance of the virtual class
	MeshOpProgress, which is used to allow the user to abort out of lengthy
	calculations. (SabinDoo is a lengthy calculation.) See
	Class MeshOpProgress.\n\n
	<b>Tab\<Point3\> *offsets=NULL</b>\n\n
	The SabinDoo algorithm is typically used to go from one polygonal approximation
	of a smooth surface to another polygonal approximation of 4 times the size, but
	it never actually returns the vertices as they would appear on the "limit
	surface". If this parameter is non-NULL, it's filled in with offsets that will,
	when added to the vertices in the SabinDoo result, take those vertices onto the
	limit surface. This makes a smoother result that matches better between
	different iterations.\n\n
	(In SabinDooVert, the offsets for the vertices created by this method are
	filled in. The offsets table is resized if necessary to accommodate this.) */
	DllExport void SabinDooVert (int vid, float interp, int selLevel, Point3 *ctr, Tab<Point3> *offsets=NULL);

	// Non-uniform Rational Mesh Smooth (NURMS.cpp)
	// Luna task 747: Someday this should support specified normals - but not today.
	/*! \remarks	This is a tessellation routine. Note that this algorithm roughly quadruples
	the size of the MNMesh. For an example of how it works, apply 3ds Max's
	MeshSmooth modifier with "NURMS" MeshSmooth Type.\n\n
	This technique for tessellation is based loosely on a 1998 SIGGraph paper,
	"Non-Uniform Recursive Subdivision Surfaces", by Sederberg, Zheng, Sewell,
	and Sabin, with additional work by Autodesk staff. Topologically, it's
	quite simple, as it creates a vertex in the center of every edge and face
	and connects these vertices with quads. However, the geometry is a
	non-uniform rational extension of the old Catmull-Clark standard cubic
	subdivision.\n\n
	To set the vertex and edge weights used by the algorithm, use the EdgeKnots
	and VertexWeights methods of MNMesh. Like other subdivision routines, this
	routine only subdivides areas of the MNMesh indicated by the MN_TARG flag
	on vertices, and uses the MNEdge::Uncrossable method to determine where the
	regional boundaries should be.
	\par Parameters:
	<b>MeshOpProgress *mop=NULL</b>\n\n
	See Class MeshOpProgress -
	provides a way to abort the calculation.\n\n
	<b>Tab\<Point3\> *offsets=NULL</b>\n\n
	Unused for now.\n\n
	<b>DWORD subdivFlags=0</b>\n\n
	This parameter is available in release 4.0 and later only.\n\n
	There is currently only one flag for the extra argument,
	<b>MNM_SUBDIV_NEWMAP</b>. If this flag is set, the new mapping scheme will
	be used. */
	DllExport void CubicNURMS (MeshOpProgress *mop=NULL,
		Tab<Point3> *offsets=NULL, DWORD subdivFlags=0);

	//DllExport void CubicNURMSMesh (Mesh & out, MeshOpProgress *mop=NULL, DWORD subdivFlags=0);

	// Boolean functions: (MNBool.cpp)
	// Luna task 747: Someday these should all support specified normals - but not today.
	/*! \remarks	Prepares a MNMesh for a Boolean operation. This is required for successful
	Booleans. Generally, it makes the mesh into a convex poly mesh, removing
	any hidden vertices, finds open regions, and collapses dead structures. */
	DllExport void PrepForBoolean ();
	/*! \remarks	BooleanCut uses the same techniques as MakeBoolean to cut the faces of this
	with the faces of m2. As with Boolean, both this and m2 should be prepared
	with the PrepForBoolean method.
	\par Parameters:
	<b>MNMesh \& m2</b>\n\n
	The MNMesh to use to cut this one.\n\n
	<b>int cutType</b>\n\n
	One of the following:\n\n
	<b>BOOLOP_CUT_REFINE</b>\n\n
	Slice the faces of this with the surface of m2, but do not separate the
	mesh along the seam.\n\n
	<b>BOOLOP_CUT_SEPARATE</b>\n\n
	Slice and separate the faces of this along the surface of m2.\n\n
	<b>BOOLOP_CUT_REMOVE_IN</b>\n\n
	Slice the faces of this with the surface of m2, then remove all the faces
	of this that are inside m2.\n\n
	<b>BOOLOP_CUT_REMOVE_OUT</b>\n\n
	Slice the faces of this with the surface of m2, then remove all the faces
	of this that are inside m2.\n\n
	<b>int fstart=0</b>\n\n
	Indicates the face (of this) we should start checking for cuts by m2. This
	can be useful if, for example, you know that the first 100 faces of this
	are nowhere near m2.\n\n
	<b>MeshOpProgress *mop=NULL</b>\n\n
	See Class MeshOpProgress.
	Provides a way to abort the calculation.
	\return  TRUE if finished successfully; FALSE if aborted by the
	MeshOpProgress. */
	DllExport bool BooleanCut (MNMesh & m2, int cutType, int fstart=0, MeshOpProgress *mop=NULL);
	/*! \remarks	Makes this MNMesh into a Boolean result of the given MNMeshes. The operands
	are not modified during this process. The Boolean algorithm is identical to
	the one used in the Boolean 2 compound object. Notice that there are no
	transform arguments, as there are in the previously used CalcBoolOp -
	transforms should be applied to the operands beforehand using the
	MNMesh::Transform method.
	\par Parameters:
	<b>MNMesh \& m1</b>\n\n
	The first operand mesh, which should have had PrepForBoolean called.\n\n
	<b>MNMesh \& m2</b>\n\n
	The second operand mesh, which should have had PrepForBoolean called.\n\n
	<b>int type</b>\n\n
	The type of Boolean. The Boolean types are defined in mesh.h, and are one
	of MESHBOOL_UNION, MESHBOOL_INTERSECTION, or MESHBOOL_DIFFERENCE. If
	MESHBOOL_DIFFERENCE is selected, operand m2 is subtracted from operand m1.
	(To get the opposite result, just switch the order of m1 and m2.)\n\n
	<b>MeshOpProgress *mop=NULL</b>\n\n
	A pointer to a MeshOpProgress. See
	Class MeshOpProgress for details.
	The mop is initialized with the number of faces in mesh 1 plus the number
	of faces in mesh 2 plus 10. MakeBoolean aborts acceptably if mop-\>Progress
	returns FALSE, in which case the partially Booleaned MNMesh is returned.
	(Usually this consists of mesh 1 partially cut by mesh 2, or mesh 1 fully
	cut and mesh 2 partially cut, with no faces removed.) This argument may
	safely be left at NULL if you do not wish to be updated on the Boolean
	progress or have the capability to abort.
	\return  Returns FALSE if the operation was cancelled by the MeshOpProgress
	or if it was unable to match the seams of the two operands at the end,
	resulting in a mesh with holes. It returns TRUE if everything went well,
	producing a solid, hole-free mesh.\n\n
	Note that it will always return FALSE if one of the operands has holes.
	\par Sample Code:
	Sample code: the following code can be used to replace the old CalcBoolOp
	method.\n\n
\code
BOOL CalcNewBooleanOp(Mesh & mesh, Mesh & mesh1, Mesh & mesh2, int op, MeshOpProgress *prog=NULL, Matrix3 *tm1=NULL, Matrix3 *tm2=NULL, int whichinv=0)
{
	MNMesh m1(mesh1);
	MNMesh m2(mesh2);
	if(tm1) m1.Transform(*tm1);
	if(tm2) m2.Transform(*tm2);
	m1.PrepForBoolean();
	m2.PrepForBoolean();
	MNMesh mOut;
	mOut.MakeBoolean(m1, m2, op, prog);
	if(whichinv==0) mOut.Transform(Inverse(*tm1));
	if(whichinv==1) mOut.Transform(Inverse(*tm2));
	mOut.OutToTri(mesh);
	return TRUE;
}
\endcode  */
	DllExport bool MakeBoolean (MNMesh & m1, MNMesh & m2, int type, MeshOpProgress *mop=NULL);
	/*! \remarks	Connect automatically figures out which loops in the given MNMeshBorder
	would make good candidates for connecting, and calls ConnectLoop on them
	all to connect them. Good candidates are pairs of loops that face each
	other. Centers and normals of each of the "holes" are compared to find the
	best matches.
	\par Parameters:
	<b>MNMeshBorder \& borderList</b>\n\n
	The boundary information for the MNMesh mesh.\n\n
	<b>int segs</b>\n\n
	The number of segments in each bridge.\n\n
	<b>float tension</b>\n\n
	The tension of each bridge - this controls how much the surface tangents at
	each end of the bridge affect the bridge's shape.\n\n
	<b>bool sm_bridge</b>\n\n
	If TRUE, each bridge should be smoothed with some unused smoothing
	group.\n\n
	<b>bool sm_ends</b>\n\n
	If TRUE, additional smoothing groups are applied to the end faces of each
	bridge to ensure that the bridge smooths with the existing faces around
	each border loop.\n\n
	<b>Tab\<int\> *vsep=NULL</b>\n\n
	If not NULL, this points to a "separation list" of vertices. Frequently the
	user wishes to connect two distinct object (as with the Connect compound
	object). In these cases, the vertices of the different connected components
	are often grouped in distinct sets: vertices 0 through 26 come from the
	first component, 27 through 118 from the second, etc. Generally the user
	would not want a component to connect to itself, but rather only to other
	components. So if non-NULL, this is assumed to point to a list describing
	the vertex ranges for each component - (0, 27, 119, etc). The list should
	have size (number of components+1), and the last element should be
	MNMesh::numv. Given such an input, the Connect algorithm will only connect
	loops to loops in other components. */
	DllExport void Connect (MNMeshBorder & borderList, int segs, float tension,
		bool sm_bridge, bool sm_ends, Tab<int> *vsep=NULL);
	/*! \remarks	Connects two border loops by filling in new geometry between them, as is
	done in the Connect compound object.
	\par Parameters:
	<b>Tab\<int\> \& loop1</b>\n\n
	<b>Tab\<int\> \& loop2</b>\n\n
	These parameters, which are interchangeable, are lists of edges that
	represent border loops in the MNMesh. It is assumed that the MNMesh is
	filled in, that each of the edges in the loops is in fact one-sided
	(e[loop1[i]].f2 == -1), and that the edges go in counterclockwise order
	when looking from outside the mesh (so e[loop1[i]].v1 == e[loop1[i+1]].v2).
	(These are the sort of border loops you find in an MNMeshBorder class.)\n\n
	<b>int segs</b>\n\n
	The number of segments in the bridge.\n\n
	<b>float tension</b>\n\n
	The tension of the bridge - this controls how much the surface tangents at
	each end of the bridge affect the bridge's shape.\n\n
	<b>DWORD smGroup</b>\n\n
	These smoothing group(s) should be applied to the entire bridge. (Use 0 for
	a faceted bridge.)\n\n
	<b>MtlID mat</b>\n\n
	This is the material used for the bridge faces.\n\n
	<b>bool sm_ends</b>\n\n
	If TRUE, additional smoothing groups are applied to the end faces of the
	bridge to ensure that the bridge smooths with the existing faces around
	each border loop. */
	DllExport void ConnectLoops (Tab<int> & loop1, Tab<int> & loop2,
		int segs, float tension, DWORD smGroup, MtlID mat, bool sm_ends);

	// Small-scale Operations - in MNOps.cpp
	// Luna task 747: Someday these should all support specified normals - but not today.
	/*! \remarks	Finds "Generalized Barycentric Coordinates" for the point given.
	Generalized barycentric coordinates are not uniquely determined for
	polygons of degree greater than 3, but this algorithm should find a
	reasonable balance, where for instance a point in the center of a polygon
	would have a significant contribution from all vertices.\n\n
	Generalized barycentric coordinates are a set of floats, one per vertex in
	the polygon, such that the sum of all the floats is 1, and the sum of all
	the floats times the corresponding vertices comes out to the point given.
	\par Parameters:
	<b>int ff</b>\n\n
	The face we're finding barycentric coordinates on.\n\n
	<b>Point3 \& p</b>\n\n
	The point we're trying to find barycentric coordinates for. If this point
	is not in the plane of the polygon, the coordinates produced should
	represent its projection into the polygon's plane. Points outside the
	boundary of the polygon should be acceptable; some of the barycentric
	coordinates will be negative in this case.\n\n
	<b>Tab\<float\> \& bary</b>\n\n
	The table to put the results in. This table is set to size f[ff].deg. */
	DllExport void FacePointBary (int ff, Point3 & p, Tab<float> & bary);
	/*! \remarks	Clones flagged vertices, creating new vertices that aren't used by any
	faces.
	\par Parameters:
	<b>DWORD cloneFlag = MN_SEL</b>\n\n
	Indicates which vertices should be cloned.\n\n
	<b>bool clear_orig = TRUE</b>\n\n
	If true, the original vertices should have the cloneFlag cleared. (The
	clones will always have this flag set.) */
	DllExport void CloneVerts (DWORD cloneFlag = MN_SEL, bool clear_orig=TRUE);
	/*! \remarks	Clones the flagged faces, as well as all the vertices and edges that are
	used by the faces.
	\par Parameters:
	<b>DWORD cloneFlag = MN_SEL</b>\n\n
	Indicates which faces should be cloned.\n\n
	<b>bool clear_orig = TRUE</b>\n\n
	If true, the original faces should have the cloneFlag cleared. (The clones
	will always have this flag set.) */
	DllExport void CloneFaces (DWORD cloneFlag = MN_SEL, bool clear_orig=TRUE);
	/*! \remarks	Divides a face by creating a point in the face's interior and creating an
	edge from that point to each of the face's vertices. An n-gon becomes n
	triangles by this method.
	\par Parameters:
	<b>int ff</b>\n\n
	The face to divide.\n\n
	<b>Tab\<float\> \& bary</b>\n\n
	The generalized barycentric coordinates of the point that should be
	created. (See FacePointBary for details on barycentric coordinates.)
	\return  The index of the newly created vertex, or -1 if there's an error.
	*/
	DllExport int DivideFace (int ff, Tab<float> & bary);
	/*! \remarks	Creates a new face, using the vertices given.\n\n
	Note that this method, unlike the similar NewFace method, maintains all
	topological links. If there's an edge between vv[0] and vv[1] which is
	already used by some other face (on the other side), that edge is modified
	to use this face on its "f2" side. If there's an edge between two
	sequential vertices with faces on both sides, the creation fails, because
	it would create an illegal condition.\n\n
	This method also creates map faces in any active map channels to maintain
	the validity of the mesh. These map faces use the map vertices that other
	faces use for the vertices passed in, or if there are no corresponding map
	vertices yet, use newly created map vertices with the value (1,1,1).
	\par Parameters:
	<b>int degg</b>\n\n
	The degree of the new face.\n\n
	<b>int *vv</b>\n\n
	A pointer to an array of degg vertices for the new face.
	\return  The index of the new face, or -1 if the method was unable to
	create that face. */
	DllExport int CreateFace (int degg, int *vv);

	// Moves the flagged subobjects to their average plane.
	/*! \remarks	Moves the flagged components into their "average plane". (Plane computed
	using average component positions and normals.)
	\par Parameters:
	<b>int selLev</b>\n\n
	Selection level, one of MNM_SL_VERTEX, MNM_SL_EDGE, MNM_SL_FACE, or
	MNM_SL_OBJECT.\n\n
	<b>DWORD flag=MN_SEL</b>\n\n
	The flag that indicates which components to align. Ignored if msl is set to
	MNM_SL_OBJECT.\n\n
	<b>Point3 *delta=NULL</b>\n\n
	If non-NULL, this is presumed to point to an array of size equal to
	MNMesh::numv, and instead of actually moving the vertices, the algorithm
	stores offsets in this array such that v[i].p + delta[i] is in the plane.
	\return  Indicates whether anything was moved. (Or if delta is non-NULL, if
	any deltas are nonzero.) */
	DllExport bool MakeFlaggedPlanar (int selLev, DWORD flag=MN_SEL, Point3 *delta=NULL);
	// Moves flagged vertices to the plane given, or produces an array of offsets which would do so.
	/*! \remarks	Projects the flagged vertices into the specified plane.
	\par Parameters:
	<b>Point3 \& norm, float offset</b>\n\n
	The definition of the plane: DotProd (norm, x) - offset = 0.\n\n
	<b>DWORD flag=MN_SEL</b>\n\n
	The flag that indicates which vertices to move.\n\n
	<b>Point3 *delta=NULL</b>\n\n
	If non-NULL, this is presumed to point to an array of size equal to
	MNMesh::numv, and instead of actually moving the vertices, the algorithm
	stores offsets in this array such that v[i].p + delta[i] is in the plane.
	\return  Indicates whether anything was moved. (Or if delta is non-NULL, if
	any deltas are nonzero.) */
	DllExport bool MoveVertsToPlane (Point3 & norm, float offset, DWORD flag=MN_SEL, Point3 *delta=NULL);
	/*! \remarks	Splits the flagged vertices into a clone for each face using the vertex.
	For example, if used on the front top left corner of a standard 3ds Max
	box, it splits the vertex into 3, one for the front face, one for the top
	face, and one for the left face.
	\par Parameters:
	<b>DWORD flag=MN_SEL</b>\n\n
	The flag that indicates which vertices to split.
	\return  TRUE if anything happened, FALSE if none of the vertices were
	split. (Note that this method will return FALSE if no vertices are flagged,
	but also if there are flagged vertices but they're all on 1 or 0 faces
	already.) */
	DllExport bool SplitFlaggedVertices (DWORD flag=MN_SEL);
	/*! \remarks	"Splits" edges by breaking vertices on two or more flagged edges into as
	many copies as needed. In this way, any path of flagged edges becomes an
	two open seams.
	\par Parameters:
	<b>DWORD flag=MN_SEL</b>\n\n
	Indicates which edges should be split. (Left at the default, selected edges
	are split.)
	\return  True if any topological changes happened, false if nothing
	happened. */
	DllExport bool SplitFlaggedEdges (DWORD flag=MN_SEL);
	/*! \remarks	Detaches specified faces to a separate element, cloning vertices and edges
	as necessary on the boundary between flagged and unflagged faces.
	\par Parameters:
	<b>DWORD flag=MN_SEL</b>\n\n
	Indicates which edges should be split. (Left at the default, selected edges
	are split.)
	\return  True if any faces were detached, false if nothing happened. (Note
	that the algorithm will return false if, for instance, all flagged faces
	form elements which are already distinct from those formed from non-flagged
	faces.) */
	DllExport bool DetachFaces (DWORD flag=MN_SEL);
	/*! \remarks	Detaches specified faces to a new MNMesh.
	\par Parameters:
	<b>MNMesh \& mesh</b>\n\n
	An empty new mesh into which the detached faces (and accompanying vertices
	and edges) should be put.\n\n
	<b>DWORD fflags=MN_SEL</b>\n\n
	Indicates which faces should be detached.
	\return  True if any faces were detached, false if nothing happened. */
	/*! \remarks	Detaches specified faces (and related vertices and edges) to a new MNMesh.
	As indicated by the name, the specified region should be a single element;
	crashes will occur if faces that are flagged share vertices or edges with
	nonflagged faces.
	\par Parameters:
	<b>MNMesh \& nmesh</b>\n\n
	The new mesh. This is assumed to be empty. The flagged elements are put
	here, and reindexed so there are no unused components. (This occurs for all
	map channels as well - only the necessary map vertices are copied
	over.)\n\n
	<b>DWORD fflags</b>\n\n
	The flags that indicate the faces which compose the element we want to
	detach.\n\n
	<b>bool delDetached=true</b>\n\n
	Indicates whether the specified element should be deleted from this MNMesh.
	\return  true if anything was detached, false otherwise. */
	DllExport bool DetachElementToObject (MNMesh & nmesh, DWORD fflags=MN_SEL, bool delDetached=true);
	/*! \remarks	Performs the topological component of an extrusion on all face clusters.
	Each cluster is "extruded", which means that vertices and edges on the
	boundary of the cluster are cloned and new faces and edges are created to
	connect the clones to the originals. (Note that nothing is moved in this
	process - movement is handled separately. See GetExtrudeDirection for
	details.)
	\par Parameters:
	<b>MNFaceClusters \& fclust</b>\n\n
	A list of face clusters. See the constructors in class MNFaceClusters and
	the face cluster related methods of class MNTempData for information on how
	to set up these clusters based on edge angles and face flags.
	\return  True if any faces were extruded, false if nothing happened. */
	DllExport bool ExtrudeFaceClusters (MNFaceClusters & fclust);
	/*! \remarks	Performs the topological component of an extrusion on the specified face
	cluster. This means that vertices and edges on the boundary of the cluster
	are cloned and new faces and edges are created to connect the clones to the
	originals. (Note that nothing is moved in this process - movement is
	handled separately. See GetExtrudeDirection for details.)
	\par Parameters:
	<b>MNFaceClusters \& fclust</b>\n\n
	A list of face clusters. See the constructors in class MNFaceClusters and
	the face cluster related methods of class MNTempData for information on how
	to set up these clusters based on edge angles and face flags.\n\n
	<b>int cl</b>\n\n
	The cluster we wish to extrude.
	\return  True if any faces were extruded, false if nothing happened. */
	DllExport bool ExtrudeFaceCluster (MNFaceClusters & fclust, int cl);
	/*! \remarks	Performs the topological component of an extrusion on each flagged face
	individually. (This differs from ExtrudeFaceClusters in that each face is
	treated like its own cluster.) Extrusion means that the vertices and edges
	used by each flagged face are cloned, and new faces and edges are created
	on the "sides" to connect the clones to their originals. (Note that nothing
	is moved in this process - movement is handled separately. See
	GetExtrudeDirection for details.)
	\par Parameters:
	<b>DWORD flag=MN_SEL</b>\n\n
	The flag that identifies the faces we wish to extrude.
	\return  True if any faces were extruded, false if nothing happened. */
	DllExport bool ExtrudeFaces (DWORD flag=MN_SEL);	// Does each face separately
	// Bug fix: second version of GetExtrudeDirection so that a faceFlag can be
	// passed in when there are no relevant face clusters.  -sca 2001.12.12
	// Version which accepts faceFlag is NEW FOR 5.0!
	// Do not use if you wish to maintain backward compatibility with 4.0!
	DllExport void GetExtrudeDirection (MNChamferData *mcd, DWORD faceFlag);
	/*! \remarks	Finds the direction vectors for the geometric component of an extrusion
	after the topological component has been completed. (See methods
	ExtrudeFaceClusters, ExtrudeFaceCluster, and ExtrudeFaces for details on
	the topological component.)
	\par Parameters:
	<b>MNChamferData *mcd</b>\n\n
	The data structure in which the extrusion directions are stored. (Note that
	there is no map support for this operation, as there is no well-defined way
	to modify mapping values during an extrusion drag.)\n\n
	<b>MNFaceClusters *fclust = NULL</b>\n\n
	The face clusters.\n\n
	<b>Point3 *clustNormals = NULL</b>\n\n
	The cluster normals.\n\n
	This information is only needed if we're extruding by cluster normals. If
	we're extruding clusters by local normals or just extruding faces
	separately, these parameters can be left at NULL. See
	class MNTempData for handy methods
	to obtain cluster normals. */
	DllExport void GetExtrudeDirection (MNChamferData *mcd,
		MNFaceClusters *fclust=NULL, Point3 *clustNormals=NULL);
	/*! \remarks	Sets vertex colors for the specified vertices. This is done by finding all
	map vertices in the specified vertex color channel that correspond to each
	flagged vertex and setting them to the color given.
	\par Parameters:
	<b>UVVert clr</b>\n\n
	The color to set the vertices to.\n\n
	<b>int mp</b>\n\n
	The map channel - use 0 for the standard vertex color channel, MAP_SHADING
	for the vertex illumination channel, or MAP_ALPHA for the alpha channel.
	(Note that alpha color values should always be shades of grey - clr.r
	should equal clr.g and clr.b.)\n\n
	<b>DWORD flag=MN_SEL</b>\n\n
	Indicates which vertices to modify the colors of.
	\return  Returns true if any vertex colors were modified. */
	DllExport bool SetVertColor (UVVert clr, int mapChannel, DWORD flag=MN_SEL);
	/*! \remarks	Sets vertex colors for the specified faces. This is done by finding all map
	vertices used by flagged faces and setting them to the color given. In
	cases where a map vertex is used by both a flagged and an unflagged face,
	the map vertex is split so that the unflagged faces' colors are unaffected
	by this change.
	\par Parameters:
	<b>UVVert clr</b>\n\n
	The color to set the faces to.\n\n
	<b>int mp</b>\n\n
	The map channel - use 0 for the standard vertex color channel, MAP_SHADING
	for the vertex illumination channel, or MAP_ALPHA for the alpha channel.
	(Note that alpha color values should always be shades of grey - clr.r
	should equal clr.g and clr.b.)\n\n
	<b>DWORD flag=MN_SEL</b>\n\n
	Indicates which faces to modify the colors of.
	\return  Returns true if any vertex colors were modified. */
	DllExport bool SetFaceColor (UVVert clr, int mapChannel, DWORD flag=MN_SEL);
	/*! \remarks	Performs the topological component of a vertex chamfer on the flagged
	vertices, and provides the data needed to do the geometric component. That
	is to say, this method clones the flagged vertices and creates the new
	edges and faces needed in a chamfer operation. It also determines the
	direction each vertex and mapping vertex will go as the user drags out the
	chamfer - but it doesn't actually move any vertices.
	\par Parameters:
	<b>DWORD flag=MN_SEL</b>\n\n
	Indicates which vertices to chamfer.\n\n
	<b>MNChamferData *mcd=NULL</b>\n\n
	If non-NULL, this points to a data structure which should be filled with
	information needed to perform the geometric component of the chamfer, such
	as vertex directions and limits. See
	class MNChamferData for
	additional information.
	\return  True if any vertices were chamfered, false otherwise. */
	DllExport bool ChamferVertices (DWORD flag=MN_SEL, MNChamferData *mcd=NULL);
	/*! \remarks	Performs the topological component of an edge chamfer on the flagged edges,
	and provides the data needed to do the geometric component. That is to say,
	this method clones the flagged edges and creates the new vertices and faces
	needed in the edge chamfer operation. It also determines the direction each
	vertex and mapping vertex will go as the user drags out the chamfer - but
	it doesn't actually move anything.
	\par Parameters:
	<b>DWORD flag=MN_SEL</b>\n\n
	Indicates which edges to chamfer.\n\n
	<b>MNChamferData *mcd=NULL</b>\n\n
	If non-NULL, this points to a data structure which should be filled with
	information needed to perform the geometric component of the chamfer, such
	as vertex directions and limits. If NULL, then this method returns false. See
	Class MNChamferData for
	additional information.
	\return  True if any edges were chamfered, false otherwise. Returns false if mcd == NULL. */
	DllExport bool ChamferEdges (DWORD flag=MN_SEL, MNChamferData *mcd=NULL);
	/*! \remarks	Flips the normals of the specified elements in the mesh.
	\par Parameters:
	<b>DWORD flag=MN_SEL</b>\n\n
	Indicates which elements should be flipped, in the following way: any
	element that has at least one flagged face is completely flipped. Elements
	without any flagged faces are not.
	\return  Returns true if anything was flipped, false otherwise. */
	DllExport bool FlipElementNormals (DWORD flag=MN_SEL);
	/*! \remarks	This is an auto-smooth algorithm that allows the developer to specify
	exactly which edges should be creases and which should be smoothed across.
	All face smoothing groups are rewritten by this algorithm. This algorithm
	is used, for example, in MeshSmooth, NURMS style, when the user turns on
	"smooth result" and applies crease values to some edges.
	\par Parameters:
	<b>DWORD creaseFlag</b>\n\n
	Indicates which edges should be treated as creases. Edges that have the
	flag (or flags) set should be creases. Those that don't should not be
	creases. */
	DllExport void SmoothByCreases (DWORD creaseFlag);
	/*! \remarks	Implements the Editable Poly Cut algorithm from the face level - cuts from
	a point on one face to a point on another face.
	\par Parameters:
	<b>int f1</b>\n\n
	The starting face of the Cut.\n\n
	<b>Point3 \& p1</b>\n\n
	The starting point of the Cut, which should lie on face f1.\n\n
	<b>Point3 \& p2</b>\n\n
	The end point of the Cut.\n\n
	<b>Point3 \& Z</b>\n\n
	The view direction. All Cut algorithms require a view direction to
	establish the plane that the cut occurs in. This plane is defined by this Z
	vector and by the vector p2-p1. All new vertices created by the cut are in
	this plane.\n\n
	<b>bool split</b>\n\n
	If true, the faces on the top and bottom of the cut should have an open
	seam between them. All the edges and vertices along the cut (except for the
	first and last vertex) are split into two parts, one copy for the top and
	one for the bottom.
	\return  The last vertex created by the cut, or -1 if the cut was unable to
	finish. */
	DllExport int CutFace (int f1, Point3 & p1, Point3 & p2, Point3 & Z, bool split);
	/*! \remarks	Implements the Editable Poly Cut algorithm from the edge level - cuts from
	a point on one edge to a point on another edge.
	\par Parameters:
	<b>int e1</b>\n\n
	The starting edge of the Cut.\n\n
	<b>float prop1</b>\n\n
	The proportion along edge e1 where the cut should begin. That is, the first
	point should be located at (1-prop1)*v[e[e1].v1].p +
	prop1*v[e[e1].v2].p.\n\n
	<b>int e2</b>\n\n
	The ending edge of the Cut.\n\n
	<b>float prop2</b>\n\n
	The proportion along edge e2 where the cut should end. That is, the last
	point should be located at (1-prop2)*v[e[e2].v1].p +
	prop2*v[e[e2].v2].p.\n\n
	<b>Point3 \& Z</b>\n\n
	The view direction. All Cut algorithms require a view direction to
	establish the plane that the cut occurs in. This plane is defined by this Z
	vector and by the vector between the start and end points. All new vertices
	created by the cut are in this plane.\n\n
	<b>bool split</b>\n\n
	If true, the faces on the top and bottom of the cut should have an open
	seam between them. All the edges and vertices along the cut (except for the
	first and last vertex) are split into two parts, one copy for the top and
	one for the bottom.
	\return  The last vertex created by the cut, or -1 if the cut was unable to
	finish. */
	DllExport int CutEdge (int e1, float prop1, int e2, float prop2, Point3 & Z, bool split);
	/*! \remarks	Implements the Editable Poly Cut algorithm from the vertex level - cuts
	from one vertex to another.
	\par Parameters:
	<b>int startv</b>\n\n
	The starting vertex of the Cut.\n\n
	<b>Point3 \& end</b>\n\n
	The location of the end vertex of the Cut.\n\n
	<b>Point3 \& Z</b>\n\n
	The view direction. All Cut algorithms require a view direction to
	establish the plane that the cut occurs in. This plane is defined by this Z
	vector and by the vector between the start and end points. All new vertices
	created by the cut are in this plane.\n\n
	<b>bool split</b>\n\n
	If true, the faces on the top and bottom of the cut should have an open
	seam between them. All the edges and vertices along the cut (except for the
	first and last vertex) are split into two parts, one copy for the top and
	one for the bottom.
	\return  The last vertex created by the cut, or -1 if the cut was unable to
	finish. */
	DllExport int Cut (int startv, Point3 & end, Point3 & Z, bool split);
	/*! \remarks	Welds the specified border vertices together.
	\par Parameters:
	<b>int v1, int v2</b>\n\n
	The vertices to be welded. They must be border vertices, in the sense that
	each of them must be used by open edges (those on only one face).\n\n
	<b>Point3 *destination</b>\n\n
	If non-NULL, this indicates where the joined vertex should be located. (If
	NULL, it's put at the average location, (v[v1].p + v[v2].p)/2.)
	\return  True if anything was welded, false if the operation could not
	proceed. */
	/*! \remarks	This method welds vertices v1 and v2 together. Both vertices must be on
	borders, and they cannot share an edge. If destination isn't NULL, it
	indicates where the welded vertex should be located. (If it is NULL, the
	welded vertex is placed at the midpoint of the inputs.)
	\par Parameters:
	<b>int v1, v2</b>\n\n
	The vertex indices to wel.\n\n
	<b>Point3 *destination</b>\n\n
	The destination of the welded vertex.
	\return  TRUE if something was welded, FALSE otherwise. */
	DllExport bool WeldBorderVerts (int v1, int v2, Point3 *destination);
	/*! \remarks	Welds the specified border edges together.
	\par Parameters:
	<b>int e1, int e2</b>\n\n
	The edges to be welded. They must be border edges, in the sense that each
	of them must be open (used by only one face). The result is located where
	edge e2 was. e[e1].v1 is joined to e[e2].v2, and e[e1].v2 is joined to
	e[e2].v1.
	\return  True if anything was welded, false if the operation could not
	proceed. */
	/*! \remarks	This method welds edges e1 and e2 together. Both edges must be on borders.
	\par Parameters:
	<b>int e1, e2</b>\n\n
	The two edges you want to weld.
	\return  TRUE if something was welded, FALSE otherwise. */
	DllExport bool WeldBorderEdges (int e1, int e2);
	/*! \remarks	Welds the specified border vertices together.
	\par Parameters:
	<b>float thresh</b>\n\n
	The welding threshold. Vertices further apart than this distance (in object
	space) will not be welded.\n\n
	<b>DWORD flag=MN_SEL</b>\n\n
	Indicates which vertices should be welded. (Non-border vertices, those in
	the "interior" of the surface, are ignored even if flagged.)
	\return  True if anything was welded, false otherwise. */
	/*! \remarks	This method welds all flagged border vertices within "thresh" of each
	other. Vertices are only welded pairwise. If vert A's closest target is
	vert B, but vert B is closer to vert C, verts B and C are welded and A is
	left out in the cold.
	\par Parameters:
	<b>float thresh</b>\n\n
	The threshold.\n\n
	<b>DWORD flag=MN_SEL</b>\n\n
	This indicates the vertices we look at. If left at the default, selected
	vertices' colors are analyzed. If flag were to equal <b>MN_WHATEVER</b>,
	then vertices with the <b>MN_WHATEVER</b> flag would have their colors
	analyzed.
	\return  TRUE if something was welded, FALSE otherwise. */
	DllExport bool WeldBorderVerts (float thresh, DWORD flag=MN_SEL);

	// Pipeline object requirements.  (MNPipe.cpp)
	/*! \remarks	Applies UVW Mapper to create the desired mapping in the specified channel.
	\par Parameters:
	<b>UVWMapper \& mp</b>\n\n
	The mapping scheme. See class UVWMapper
	for details.\n\n
	<b>BOOL channel=0</b>\n\n
	The channel to apply the map to. Channel 0 is normally the vertex color
	channel. Channels 1-99 are the normal user-accessible mapping channels.
	Channels <b>MAP_SHADING</b> and <b>MAP_ALPHA</b> are used for vertex
	illumination and alpha.\n\n
	<b>BOOL useSel=FALSE</b>\n\n
	Indicates whether the specified mapping should be applied only to selected
	faces (instead of the whole object). In cases where the map channel was
	previously unused, a default map will be applied to nonselected faces, and the
	specified map will be applied to the selected faces. */
	DllExport void ApplyMapper (UVWMapper & mapChannel, int channel=0, BOOL useSel=FALSE);
	//Invalidates the geometry cache of the MNMesh - Forces any geometry related data to be rebuilt
	/*! \remarks	Invalidates information, like bounding box and face normals, that's
	dependent on Geometry. Call after changing the geometry. */
	DllExport void InvalidateGeomCache ();
	//Invalidates the topology cache of the MNMesh - Forces any topology related data to be rebuilt
	//in_cacheClearFlags specifies whether the "MN_MESH_CACHE_FLAGS" flags will be cleared
	//true means the flags will be cleared, false means the flags will not be touched
	//These flags dictate whether the mesh has valid data in the edge and vertex caches, which is
	//typically false after invalidating the topology cache.  (i.e. these flags should be cleared)
	//If unsure about what to pass to this method, use the default behaviour
	/*! \remarks Clears out topology-dependent cache information. Note that this
	method clears topology-dependent flags such as <b>MN_MESH_FILLED_IN</b>, and
	thus invalidates the edge list. If you have taken pains to preserve the
	integrity of your edge list, you should set the <b>MN_MESH_FILLED_IN</b> flag
	immediately after calling <b>InvalidateTopoCache()</b>. */
	DllExport void InvalidateTopoCache ( bool in_clearCacheFlags = true );
	/*! \remarks	This method is used to manage the display of vertex colors from any channel
	(or those that are passed in by some calling routine). For instance, to set
	the mesh to display the Illumination channel as the current vertex colors,
	you would call <b>SetDisplayVertexColors (MAP_SHADING)</b>. (Normally, it
	shows the standard vertex color channel, channel 0.)
	<b>UpdateDisplayVertexColors()</b> is used to refresh the vertex color
	pointers just before the MNMesh displays itself. */
	DllExport void UpdateDisplayVertexColors ();
	/*! \remarks	This method is used to manage the display of vertex colors from any channel
	(or those that are passed in by some calling routine). For instance, to set
	the mesh to display the Illumination channel as the current vertex colors,
	you would call <b>SetDisplayVertexColors (MAP_SHADING)</b>. (Normally, it
	shows the standard vertex color channel, channel 0.)
	<b>UpdateDisplayVertexColors()</b> is used to refresh the vertex color
	pointers just before the MNMesh displays itself.
	\par Parameters:
	<b>int chan</b>\n\n
	The channel you wish to use. */
	DllExport void SetDisplayVertexColors (int chan);
	/*! \remarks	If you have cached your own vertex color information that isn't in any of
	the map channels provided, you can use this method to set the internal
	pointers to use your data.
	\par Parameters:
	<b>UVVert *mv</b>\n\n
	The array of UV vertices.\n\n
	<b>MNMapFace *mf</b>\n\n
	The map face data. */
	DllExport void SetDisplayVertexColors (UVVert *mv, MNMapFace *mf);
	/*! \remarks	This method reconciles flags with arrays, checks and modifies data, ensures
	that the caches are consistent, and prepares the MNMesh pipeline. For
	instance, if the <b>MN_MESH_FILLED_IN</b> flag is absent but there is still
	an edge array, this will free the edge array. This method is important to
	call if a <b>MNMesh</b> has been subjected to topology changing operations
	and should be called at the end of any operation on an MNMesh. */
	/*! \remarks	Prepares MNMesh for pipeline. This just does a few basic checks and
	modifies data caches to be consistent. In particular it throws away the
	edge list if the <b>MN_MESH_FILLED_IN</b> flag is not set, and it frees any
	data in <b>MN_DEAD</b> map channels. This is a good method to call at the
	end of any operation on an MNMesh. */
	DllExport void PrepForPipeline ();
	/*! \remarks	Allocates the "render vertices" used to optimize display routines. Called
	by the system as needed. */
	DllExport void allocRVerts ();
	/*! \remarks	Fills in the render vertices with display coordinates based on the
	specified view. */
	DllExport void updateRVerts (GraphicsWindow *gw);
	/*! \remarks	Frees the "render vertices" used to optimize display routines. */
	DllExport void freeRVerts ();

	/*! \remarks	This method can be used to build the normals and allocate RVert space only
	if necessary. This is a very inexpensive call if the normals are already
	calculated. When illum is FALSE, only the RVerts allocation is checked
	(since normals aren't needed for non-illum rendering). When illum is TRUE,
	normals will also be built, if they aren't already. So, to make sure
	normals are built, call this with illum=TRUE.
	\par Parameters:
	<b>BOOL illum</b>\n\n
	If TRUE normals are built. If FALSE only the RVert array is allocated. */
	DllExport void checkNormals (BOOL illum);
	/*! \remarks	This method resolves the normals on the RVertex array. If the MNMesh
	already has normals prescribed on each vertex, the normal is just moved to
	the RVertex array. See Class RVertex and
	Class RNormal.\n\n
	If you are creating or modifying a MNMesh, after you are done specifying
	all the vertices and faces, this method should be called. This allocates
	the RVertex and RNormal database for the MNMesh. This will allow you to
	query the MNMesh and ask about normals on the vertices.\n\n
	This method also builds the face normals for the mesh if needed. */
	DllExport void buildNormals ();
	/*! \remarks	This method is similar to <b>buildNormals()</b> above, but ignores the
	material index (mtlIndex). In other words, the difference between this and
	<b>buildNormals()</b> is that it doesn't look at the mtlIndex of the faces:
	normals of faces with the same smoothing group are averaged regardless. */
	DllExport void buildRenderNormals ();
	/*! \remarks	Updates the <b>MN_BACKFACING</b> flag in all components based on the
	specified view.
	\par Parameters:
	<b>GraphicsWindow *gw</b>\n\n
	A pointer to the current graphics window.\n\n
	<b>bool force</b>\n\n
	If the gw points to the same GraphicsWindow that it did last time this
	method was called, it doesn't necessarily need to update the flags. If
	force is true, it will update them anyway. (Useful if you think the
	GraphicsWindow perspective may have changed.) */
	DllExport void UpdateBackfacing (GraphicsWindow *gw, bool force);

	// This method can be used to invalidate any display-technology-specific caches of the MNMesh.
	// In particular, it's currently used to invalidate the DirectX cache of the mesh.
	// This is important to call, for example, after changing Vertex Colors or Texture coordinates, if
	// you don't already call InvalidateGeomCache ().
	// "DWORD keepFlags" should be zero.  We may someday add flags indicating that some parts of
	// the hardware mesh cache should be kept, but for now, the whole cache is invalidated.
	// (An example usage is in maxsdk/samples/mesh/EditablePoly\PolyEdOps.cpp.)
	DllExport void InvalidateHardwareMesh (DWORD keepFlags=0);

	// Display flags
	/*! \remarks	This method allows you to set the display flags.
	\par Parameters:
	<b>DWORD f</b>\n\n
	The following flags are supported;\n\n
	<b>MNDISP_VERTTICKS</b>\n\n
	Displays vertices with tick-marks (plus signs).\n\n
	<b>MNDISP_SELVERTS</b>\n\n
	Displays selected vertices in red (and soft-selected vertices in soft
	selection colors).\n\n
	<b>MNDISP_SELFACES</b>\n\n
	Displays selected faces.\n\n
	<b>MNDISP_SELEDGES</b>\n\n
	Displays selected edges.\n\n
	<b>MNDISP_NORMALS</b>\n\n
	Displays face normals on selected faces.\n\n
	<b>MNDISP_DIAGONALS</b>\n\n
	Displays diagonals (using the same drawing style as regular Meshes use for
	hidden edges). */
	void		SetDispFlag(DWORD f) { dispFlags |= f; }
	/*! \remarks	This method returns the <b>displFlags \& f</b>.
	\par Parameters:
	<b>DWORD f</b>\n\n
	The following flags are supported;\n\n
	<b>MNDISP_VERTTICKS</b>\n\n
	Displays vertices with tick-marks (plus signs).\n\n
	<b>MNDISP_SELVERTS</b>\n\n
	Displays selected vertices in red (and soft-selected vertices in soft
	selection colors).\n\n
	<b>MNDISP_SELFACES</b>\n\n
	Displays selected faces.\n\n
	<b>MNDISP_SELEDGES</b>\n\n
	Displays selected edges.\n\n
	<b>MNDISP_NORMALS</b>\n\n
	Displays face normals on selected faces.\n\n
	<b>MNDISP_DIAGONALS</b>\n\n
	Displays diagonals (using the same drawing style as regular Meshes use for
	hidden edges). */
	DWORD		GetDispFlag(DWORD f) { return dispFlags & f; }
	/*! \remarks	This method allows you to clear the specified display flags.
	\par Parameters:
	<b>DWORD f</b>\n\n
	The following flags are supported;\n\n
	<b>MNDISP_VERTTICKS</b>\n\n
	Displays vertices with tick-marks (plus signs).\n\n
	<b>MNDISP_SELVERTS</b>\n\n
	Displays selected vertices in red (and soft-selected vertices in soft
	selection colors).\n\n
	<b>MNDISP_SELFACES</b>\n\n
	Displays selected faces.\n\n
	<b>MNDISP_SELEDGES</b>\n\n
	Displays selected edges.\n\n
	<b>MNDISP_NORMALS</b>\n\n
	Displays face normals on selected faces.\n\n
	<b>MNDISP_DIAGONALS</b>\n\n
	Displays diagonals (using the same drawing style as regular Meshes use for
	hidden edges). */
	void		ClearDispFlag(DWORD f) { dispFlags &= ~f; }

	//ack what do all these do?
	/*! \remarks	Causes the MNMesh to display itself in the indicated GraphicsWindow.
	\par Parameters:
	<b>GraphicsWindow *gw</b>\n\n
	Points to the graphics window to render to.\n\n
	<b>Material *ma</b>\n\n
	The list of materials to use to render the mesh. See
	Class Material.\n\n
	<b>RECT *rp</b>\n\n
	Specifies the rectangular region to render. If the mesh should be rendered
	to the entire viewport pass NULL.\n\n
	<b>int compFlags</b>\n\n
	One or more of the following flags:\n\n
	<b>COMP_TRANSFORM</b>\n\n
	Forces recalculation of the model to screen transformation; otherwise
	attempt to use the cache.\n\n
	<b>COMP_IGN_RECT</b>\n\n
	Forces all polygons to be rendered; otherwise only those intersecting the
	box will be rendered.\n\n
	<b>COMP_LIGHTING</b>\n\n
	Forces re-lighting of all vertices (as when a light moves); otherwise only
	re-light moved vertices\n\n
	<b>COMP_ALL</b>\n\n
	All of the above flags.\n\n
	<b>COMP_OBJSELECTED</b>\n\n
	If this bit is set then the node being displayed by this mesh is selected.
	Certain display flags only activate when this bit is set.\n\n
	<b>int numMat=1</b>\n\n
	The number of materials for the MNMesh. */
	DllExport void render(GraphicsWindow *gw, Material *ma, RECT *rp, int compFlags, int numMat=1, InterfaceServer *pi=NULL);
	/*! \remarks	Displays the indicated face in the GraphicsWindow. This method is usually
	called only by <b>MNMesh::render()</b>.
	\par Parameters:
	<b>GraphicsWindow *gw</b>\n\n
	The GraphicsWindow in which to display this face.\n\n
	<b>int ff</b>\n\n
	The face to display. */
	DllExport void renderFace (GraphicsWindow *gw, int ff);
	/*! \remarks	Displays the indicated face in the GraphicsWindow using hardware
	acceleration for texture and lighting if available. This method is usually
	called only by <b>MNMesh::render()</b>.
	\par Parameters:
	<b>GraphicsWindow *gw</b>\n\n
	The GraphicsWindow in which to display this face.\n\n
	<b>int ff</b>\n\n
	The face to display. */
	DllExport void render3DFace (GraphicsWindow *gw, int ff);
	/*! \remarks	Displays all face diagonals in the GraphicsWindow using hardware
	acceleration for texture and lighting if available. See the render method
	for a description of the parameters. This method is usually called only by
	<b>MNMesh::render()</b>. */
	DllExport void render3DDiagonals (GraphicsWindow *gw, DWORD compFlags);
	/*! \remarks	Displays all face diagonals in the GraphicsWindow. See the render method
	for a description of the parameters. This method is usually called only by
	<b>MNMesh::render()</b>. */
	DllExport void renderDiagonals (GraphicsWindow *gw, DWORD compFlags);
	/*! \remarks	Displays diagonals for the specified face in the GraphicsWindow. This
	method is usually called only by <b>MNMesh::render()</b>.
	\par Parameters:
	<b>GraphicsWindow *gw</b>\n\n
	The GraphicsWindow in which to display these diagonals.\n\n
	<b>int ff</b>\n\n
	The face to display the diagonals of.\n\n
	<b>bool useSegments = false</b>\n\n
	Indicates if we are in segment-drawing mode. See
	class GraphicsWindow, method
	<b>segment()</b> for details.\n\n
	<b>bool *lastColorSubSel=NULL</b>\n\n
	If non-NULL, it points to a bool variable which should be true if the last
	color set was the subobject selection color (<b>GetSubSelColor()</b>), and
	false if the color is set to the selected object color
	(<b>GetSelColor()</b>). This saves processing time that would be needed to
	switch between the two colors. */
	DllExport void renderDiagonal (GraphicsWindow *gw, int ff, bool useSegments=false, bool *lastColorSubSel=NULL);
	/*! \remarks	Displays all edges in the GraphicsWindow using hardware acceleration for
	texture and lighting if available. See the render method for a description
	of the parameters. This method is usually called only by
	<b>MNMesh::render()</b>. */
	DllExport void render3DEdges (GraphicsWindow *gw, DWORD compFlags);
	/*! \remarks	Displays all edges in the GraphicsWindow. See the render method for a
	description of the parameters. This method is usually called only by
	<b>MNMesh::render()</b>. */
	DllExport void renderEdges (GraphicsWindow *gw, DWORD compFlags);
	/*! \remarks	Displays an edge in a GraphicsWindow. This method is usually called only by
	<b>MNMesh::render()</b>.
	\par Parameters:
	<b>GraphicsWindow *gw</b>\n\n
	The GraphicsWindow in which to display this edge.\n\n
	<b>int ee</b>\n\n
	The edge to display.\n\n
	<b>bool useSegments = false</b>\n\n
	Indicates if we are in segment-drawing mode. See
	class GraphicsWindow, method
	<b>segment()</b> for details.\n\n
	<b>bool *lastColorSubSel=NULL</b>\n\n
	If non-NULL, it points to a bool variable which should be true if the last
	color set was the subobject selection color (<b>GetSubSelColor()</b>), and
	false if the color is set to the selected object color
	(<b>GetSelColor()</b>). This saves processing time that would be needed to
	switch between the two colors. */
	DllExport void renderEdge (GraphicsWindow *gw, int ee, bool useSegments=false, bool *lastColorSubSel=NULL);


	/*! \remarks	Checks the given HitRegion to see if it intersects this Mesh object.
	\par Parameters:
	<b>GraphicsWindow *gw</b>\n\n
	Points to the graphics window to check.\n\n
	<b>Material *ma</b>\n\n
	The list of materials for the mesh.\n\n
	<b>HitRegion *hr</b>\n\n
	This describes the properties of a region used for the hit testing. See
	Class HitRegion.\n\n
	<b>int abortOnHit = FALSE</b>\n\n
	If nonzero, the hit testing is complete after any hit. Note that although
	there is only one object to hit, setting this to TRUE prevents the
	algorithm from finding the closest hit on the MNMesh.\n\n
	<b>int numMat=1</b>\n\n
	The number of materials for the mesh.
	\return  TRUE if the item was hit; otherwise FALSE. */
	DllExport BOOL select (GraphicsWindow *gw, Material *ma, HitRegion *hr, int abortOnHit=FALSE, int numMat=1);
	/*! \remarks	This method may be called to perform sub-object hit testing on this mesh.
	\par Parameters:
	<b>GraphicsWindow *gw</b>\n\n
	The graphics window associated with the viewport the mesh is being hit
	tested in.\n\n
	<b>Material *ma</b>\n\n
	The list of materials for the mesh. See Class Material\n\n
	<b>HitRegion *hr</b>\n\n
	This describes the properties of a region used for the hit testing. See
	Class HitRegion.\n\n
	<b>DWORD flags</b>\n\n
	Flags for sub object hit testing. One or more of the following values:\n\n
	<b>SUBHIT_MNUSECURRENTSEL</b>\n\n
	When this bit is set, the sel only and unsel only tests will use the
	current level (edge or face) selection when doing a vertex level hit test.)
	This is like the Mesh hit-testing flag SUBHIT_USEFACESEL.\n\n
	<b>SUBHIT_MNVERTS</b>\n\n
	<b>SUBHIT_MNFACES</b>\n\n
	<b>SUBHIT_MNEDGES</b>\n\n
	<b>SUBHIT_MNTYPEMASK (SUBHIT_MNVERTS|SUBHIT_MNFACES|SUBHIT_MNEDGES)</b>\n\n
	<b>SubObjHitList\& hitList</b>\n\n
	The results are stored here. See
	Class SubObjHitList.\n\n
	<b>int numMat=1</b>\n\n
	The number of materials for the mesh.
	\return  TRUE if the item was hit; otherwise FALSE. */
	DllExport BOOL SubObjectHitTest(GraphicsWindow *gw, Material *ma, HitRegion *hr,
							DWORD flags, SubObjHitList& hitList, int numMat=1 );
	/*! \remarks	Provides the intersection point and normal for the ray with this mesh.
	\par Parameters:
	<b>Ray \& ray</b>\n\n
	The ray we want an intersection point for.\n\n
	<b>float \& at</b>\n\n
	This is filled in with a value giving the intersection point along the ray.
	(The actual point is computed by ray.p + ray.dir*at.)\n\n
	<b>Point3 \& norm</b>\n\n
	Filled in with the surface normal at the intersection point.
	\return  Returns TRUE if an intersection point was found, or FALSE if the
	ray doesn't intersect this MNMesh. */
	DllExport int IntersectRay (Ray& ray, float& at, Point3& norm);
	/*! \remarks	Provides the intersection point and normal for the ray with this mesh.
	\par Parameters:
	<b>Ray \& ray</b>\n\n
	The ray we want an intersection point for.\n\n
	<b>float \& at</b>\n\n
	This is filled in with a value giving the intersection point along the ray.
	(The actual point is computed by ray.p + ray.dir*at.)\n\n
	<b>Point3 \& norm</b>\n\n
	Filled in with the surface normal at the intersection point.\n\n
	<b>int \& fi</b>\n\n
	Filled in with the face index for the face that was hit by the ray.\n\n
	<b>Tab\<float\> \& bary</b>\n\n
	Filled in with the "generalized barycentric coordinates" of the
	intersection point on the face. This is a table of floats of size
	f[fi].deg, where each float represents the contribution of the
	corresponding face vertex, and where the floats all sum to 1.
	\return  Returns TRUE if an intersection point was found, or FALSE if the
	ray doesn't intersect this MNMesh. */
	DllExport int IntersectRay (Ray& ray, float& at, Point3& norm, int &fi, Tab<float> & bary);
	/*! \remarks	Gets the current vertex selection, based on the current selection level.
	That is, if the current selection level is <b>MNM_SL_VERTEX</b> and the
	parameters are at their defaults, it'll return the current vertex
	selection, but if the selection level is <b>MNM_SL_FACE</b>, for instance,
	it'll return the vertices used by currently selected faces.\n\n
	This method is used, e.g. in PolyObject::Deform to determine which vertices
	to affect by pipeline modifiers.
	\par Parameters:
	<b>DWORD fmask=MN_DEAD|MN_SEL</b>\n\n
	Indicates the flags we're trying to match in the components at the current
	selection level.\n\n
	<b>DWORD fset=MN_SEL</b>\n\n
	Indicates which flags (from fmask) we want to see set. The default values
	mean "find components with <b>MN_SEL</b> set and <b>MN_DEAD</b> cleared".
	*/
	DllExport BitArray VertexTempSel (DWORD fmask=MN_DEAD|MN_SEL, DWORD fset=MN_SEL);
	/*! \remarks	Provides the guts of pipeline shallow copying. Used by
	<b>PolyObject::ShallowCopy()</b>. */
	DllExport void 		ShallowCopy(MNMesh *amesh, ChannelMask channels);
	/*! \remarks	Provides the guts of pipeline new \& copying. Used by
	<b>PolyObject::NewAndCopyChannels()</b>. */
	DllExport void		NewAndCopyChannels(ChannelMask channels);
	DllExport void 		FreeChannels (ChannelMask channels, BOOL zeroOthers=1);

	DllExport IOResult Save (ISave *isave);
	DllExport IOResult Load (ILoad *iload);
	DllExport void ClearFlag (DWORD fl);

	// --- from InterfaceServer
	/*! \remarks	This method returns a pointer to the base interface of the associated and
	provided Interface ID.
	\par Parameters:
	<b>Interface_ID id</b>\n\n
	The interface ID. */
	DllExport BaseInterface* GetInterface(Interface_ID id);

	// Luna task 748 - New operations for 5.0
	// Source in MNOps2.cpp
	// NOT AVAILABLE IN PREVIOUS VERSIONS - AVOID FOR BACKWARD COMPATIBILITY
	DllExport bool SeparateMapVerticesInFaceCluster (MNFaceClusters & fclust, int clusterID);
	DllExport bool ExtrudeFaceClusterAlongPath (Tab<Matrix3> tFrenets,
		MNFaceClusters & fclust, int clusterID, bool align);
	DllExport bool LiftFaceClusterFromEdge (int liftEdge, float liftAngle, int segments,
		MNFaceClusters & fclust, int clusterID);
	DllExport bool IsEdgeMapSeam (int mapChannel, int edge);
	DllExport void MultiDivideEdge (int edge, int segments);
	DllExport bool ConnectEdges (DWORD edgeFlag, int segments=2);
	DllExport bool ConnectVertices (DWORD vertexFlag);
	DllExport void SelectEdgeLoop (BitArray & edgeSel);
	DllExport void SelectEdgeRing (BitArray & edgeSel);
	DllExport bool ExtrudeVertices (DWORD vertexFlag, MNChamferData *pMCD, Tab<Point3> & tUpDir);
	DllExport bool ExtrudeEdges (DWORD edgeFlag, MNChamferData *pMCD, Tab<Point3> & tUpDir);
	DllExport bool WeldOpposingEdges (DWORD edgeFlag);
	DllExport bool FlipFaceNormals (DWORD faceFlag);
	DllExport bool RemoveVertex (int vertex);	// Luna task 748O
	DllExport bool RemoveSpur (int spur);	// Used both by WeldEdge and in Luna task 748O
	DllExport int InsertSpur (int face, int vertIndex, Tab<int> *ptMapVertArray=NULL);	// New for task 748F
	DllExport void FindFaceBaryVector (int face, int vertIndex, Point3 & vector, Tab<float> & tBary);
	DllExport bool SplitFacesUsingBothSidesOfEdge (DWORD edgeFlag=0x0);

	// Luna task 747 - New operations for 5.0
	// Source in MNNormals.cpp
	// NOT AVAILABLE IN PREVIOUS VERSIONS - AVOID FOR BACKWARD COMPATIBILITY
	/*! \remarks	Returns a pointer to the user-specified normal interface, if present.\n\n

	\return  A pointer to this mesh's MNNormalSpec interface, or NULL if the
	interface\n\n
	has not been created in this mesh.\n\n
	  */
	DllExport MNNormalSpec *GetSpecifiedNormals();	// Returns NULL if SpecifyNormals() has not been called.
	/*! \remarks	Returns a non-NULL MNNormalSpec interface pointer only if the interface\n\n
	is present, and is prepared for use in display - otherwise, it returns\n\n
	NULL, and we fall back on the smoothing groups.\n\n
	A MNNormalSpec is considered "prepared for display" only if the\n\n
	MNNORMAL_NORMALS_BUILT and MNNORMAL_NORMALS_COMPUTED flags are set.\n\n
	(See the MNNormalSpec methods BuildNormals and ComputeNormals.)\n\n
	  */
	DllExport MNNormalSpec *GetSpecifiedNormalsForDisplay();	// Returns NULL if no specified normals, or if specified normals not ready to use for display.
	/*! \remarks	Clears out the specified normal interface, if present. Removes it\n\n
	completely, so it won't flow up the stack, etc.\n\n
	  */
	DllExport void ClearSpecifiedNormals ();
	/*! \remarks	Creates the user-specified normal interface in this mesh. Initializes\n\n
	the MNNormalSpec's "Parent" to this mesh, but does not allocate normal
	faces\n\n
	or otherwise prepare the normals. Note that this interface will flow\n\n
	up the pipeline, in the PART_GEOM and PART_TOPO channels.\n\n
	  */
	DllExport void SpecifyNormals ();
	// Copies bare geometry only - no per-vertex data, maps, normals, etc.
	/*! \remarks	Copies bare geometry only - no per-vertex data, maps, or normals.\n\n
	Useful for keeping low-memory local caches of a mesh where map,\n\n
	vertex data, edge data, normals, and other interfaces are not required.
	\par Parameters:
	<b>const MNMesh \& from</b>\n\n
	The mesh to copy into this mesh.\n\n
	<b>bool copyEdges = false</b>\n\n
	Indicates if the winged-edge array, as well as vertex and face\n\n
	references to edges, should be copied. If false, the\n\n
	MN_MESH_FILLED_IN flag is cleared in this mesh. */
	DllExport void CopyBasics (const MNMesh & from, bool copyEdges = false);

	//! \brief This will set the mesh or as static or not.  As static mesh has no animated channels
	/*!	This will set the mesh or as static or not.  As static mesh has no animated channels.  This allows 
	use to certain display optimization since we know the mesh is not changing.
	\param[in] staticMesh Whether the mesh is to tagged static or not.
	*/
	DllExport void SetStaticMesh(bool staticMesh);

	//! \brief This will force all the derived data used to display the mesh to be deleted
	/*!	This will force all the derived data used to display the mesh to be deleted.  This data can be things 
		like the gfx normals, the direct x mesh cache, tri stripping data etc.
	*/
	DllExport void ReduceDisplayCaches();

	/*! \brief This returns whether the Graphics Cache for this object needs to be rebuilt
	\par Parameters:
	GraphicsWindow *gw the active graphics window \n
	Material *ma the material aray assigned to the mesh \n
	int numMat the number of materials in the material array \n
	*/
	DllExport bool NeedGWCacheRebuilt(GraphicsWindow *gw, Material *ma, int numMat);
	/*! \brief This builds the graphics window cached mesh 
	\par Parameters:
	GraphicsWindow *gw the active graphics window \n
	Material *ma the material aray assigned to the mesh \n
	int numMat the number of materials in the material array \n
	BOOL threaded whether when building the cache it can use additional threads.  This is needed since the system may be creating many meshes at the same time\n
	*/
	DllExport void BuildGWCache(GraphicsWindow *gw, Material *ma, int numMat, BOOL threaded);


#ifdef SEPARATE_GEOM_TOPO_INVALIDATION
	DllExport void InvalidateHardwareMeshTopo (DWORD keepFlags=0);
#endif
};

/*! \sa  Class MNMesh, Template Class Tab, Class BitArray.\n\n
\par Description:
This class is available in release 2.0 and later only.\n\n
MNMeshBorder is a small class used to hold boundary information for an MNMesh
mesh. The principal data contained is a table of tables of int's, which
represent edge lists in the MNMesh. These edge lists form closed loops of
one-sided edges: boundaries, or borders, of the mesh. These edges are stored in
order such that E(loop(i)[j])-\>v1 is the same as E(loop(i)[j+1])-\>v2,
E(loop(i)[j+1])-\>v1 == E(loop(i)[j+2])-\>v2, and so on. (This is the
right-hand, counterclockwise order when looking down on the hole from outside
the mesh.)\n\n
Most 3ds Max primitives have no borders, but the Patch Grid, when converted to
a mesh, is an example of one with a single border.\n\n
All methods of this class are implemented by the system.
\par Data Members:
private:\n\n
<b>Tab\<IntTab *\> bdr</b>\n\n
IntTab is itself a Tab\<int\>. We use a table of Tab\<int\> pointers (rather
than a table of Tab\<int\>'s) for clean memory allocation \& freeing. However,
this is an unwieldy structure, so it's kept private.\n\n
<b>BitArray btarg</b>\n\n
This is simply an array of targeting bits for each of the boundary loops in
bdr.  */
class MNMeshBorder: public MaxHeapOperators {
	friend class MNMesh;
	Tab<Tab<int> *> bdr;
	BitArray btarg;
public:
	/*! \remarks Destructor; frees all reserved memory. There is no
	Constructor for this class, since both data members have their own,
	adequate constructors. */
	~MNMeshBorder () { Clear(); }
	/*! \remarks Frees all reserved memory and reinitializes the data,
	producing an empty border. */
	DllExport void Clear ();
	/*! \remarks Returns the number of border loops for the MNMesh analyzed.
	*/
	int Num () { return bdr.Count(); }
	/*! \remarks Returns a pointer to the i'th border loop. */
	Tab<int> *Loop (int i) { return bdr[i]; }
	/*! \remarks Indicates whether border loop <b>i</b> is targeted or not. */
	bool LoopTarg (int i) { return ((i>=0) && (i<bdr.Count()) && (btarg[i])); }
	/*! \remarks	Uses DebugPrint to print out the MNMesh borders to the Debug Results window
	in DevStudio. This can be useful for tracking down bugs. Be careful not to
	leave MNDebugPrint calls in your final build; they will slow down your
	effect to no purpose.
	\par Parameters:
	<b>MNMesh *m</b>\n\n
	The MNMesh to which this MNMeshBorder refers is required to give more
	details about the border. */
	DllExport void MNDebugPrint (MNMesh *m);
};

/*! \sa  Class MNTempData, Class MNMesh\n\n
\par Description:
This class is available in release 4.0 and later only.\n\n
This class is used to assist in the process of sorting <b>MNMesh</b> faces into
separate elements.\n\n
For convenient caching, it is recommended that you use this class through the
<b>MNTempData</b> class.
\par Data Members:
<b>Tab\<int\> elem;</b>\n\n
The list indicating which element each face is in. The size is the number of
faces in the associated MNMesh.\n\n
<b>int count;</b>\n\n
The total number of elements in the associated MNMesh.  */
class MNFaceElement: public MaxHeapOperators {
public:
	// For each face, which element is it in
	Tab<int> elem;
	int count;

	/*! \remarks Constructor.\n\n
	This method will create an element list based on the specified MNMesh.
	\par Parameters:
	<b>MNMesh \&mesh</b>\n\n
	A reference to the MNMesh for which to create the element list. */
	DllExport MNFaceElement (MNMesh &mesh);
	/*! \remarks Index operator for accessing elements.
	\par Default Implementation:
	<b>{ return elem[i]; }</b> */
	int operator[](int i) {return elem[i];}
};

/*! \sa  Class MNTempData, Class MNMesh\n\n
\par Description:
This class is available in release 4.0 and later only.\n\n
This class may be used for grouping faces in an <b>MNMesh</b> into clusters for
applying transformations. Depending on the constructor used, it may group faces
into clusters based on minimal angles between faces, on face selections, or on
both. The class contains a list of face "clusters" for a given mesh. A typical
application would be in Editable Poly, where the user has selected two separate
groups of faces on different parts of the mesh and wants to extrude them both,
or rotate both around their local centers. Each "cluster" is a contiguous group
of selected faces. This class is only defined in relation to some
<b>MNMesh</b>.\n\n
For convenient caching, it is recommended that you use this class through the
<b>MNTempData</b> class.\n\n
All methods of this class are implemented by the system.
\par Data Members:
<b>Tab\<int\> clust;</b>\n\n
The cluster number, one for each face. Note that non-selected faces have
UNDEFINED for their id.\n\n
<b>int count;</b>\n\n
The total number of clusters in the MNMesh.  */
class MNFaceClusters: public MaxHeapOperators {
public:
	// Cluster #, one for each face - non-selected faces have -1 for their id.
	Tab<int> clust;
	int count;

	// Makes clusters from distinct selected components.
	/*! \remarks Constructor.\n\n
	This method will create face cluster lists based on the specified MNMesh.
	Each contiguous group of selected faces is grouped into a cluster.
	\par Parameters:
	<b>MNMesh \&mesh</b>\n\n
	The mesh these clusters are based on.\n\n
	<b>DWORD clusterFlags</b>\n\n
	The face flags to cluster the faces by.\n\n
	For instance, if this value was set to <b>MN_SEL</b>, then faces would be
	clustered by their selection. */
	DllExport MNFaceClusters (MNMesh & mesh, DWORD clusterFlags);
	// This version separates cluster also using a minimum angle and optionally by flags.
	/*! \remarks	This method will create face cluster lists based on the specified MNMesh.
	Cluster boundaries will be determined by the angle between faces and
	optionally by the face flags.
	\par Parameters:
	<b>MNMesh \&mesh</b>\n\n
	The mesh these clusters are based on.\n\n
	<b>float angle</b>\n\n
	The minimum edge angle (in radians) used to define a separation between
	clusters.\n\n
	<b>DWORD clusterFlags</b>\n\n
	The face flags to cluster the faces by.\n\n
	For instance, if this value was set to <b>MN_SEL</b>, then faces would be
	clustered by their selection. If this value is set to 0, then the clusters
	are based only on edge angles. */
	DllExport MNFaceClusters (MNMesh & mesh, float angle, DWORD clusterFlags);
	/*! \remarks Index operator for accessing cluster data.
	\par Default Implementation:
	<b>{ return clust[i]; }</b> */
	int operator[](int i) { return (i<clust.Count()) ? clust[i] : -1; }
	/*! \remarks This method will create a table indicating which face cluster
	each vertex in the mesh is in.
	\par Parameters:
	<b>MNMesh \&mesh</b>\n\n
	The mesh this face cluster is based on.\n\n
	<b>Tab\<int\> \& vclust</b>\n\n
	The table of vertex clusters. This is set to size <b>mesh.VNum()</b>.
	Values of UNDEFINED (0xffffffff) in the table indicate that a vertex is not
	in any cluster. If a vertex is in two clusters (because it's a point where
	corners of two clusters touch), the higher-indexed face's cluster is
	dominant. */
	DllExport void MakeVertCluster(MNMesh &mesh, Tab<int> & vclust);
	/*! \remarks Computes average normals and centers for each of the face
	clusters.
	\par Parameters:
	<b>MNMesh \&mesh</b>\n\n
	The mesh this face cluster is based on.\n\n
	<b>Tab\<Point3\> \& norm</b>\n\n
	The tables where the normals should be put. Each of these tables has its
	size set to the number of clusters, and is indexed by cluster.\n\n
	<b>Tab\<Point3\> \& ctr</b>\n\n
	The tables where the centers should be put. Each of these tables has its
	size set to the number of clusters, and is indexed by cluster. */
	DllExport void GetNormalsCenters (MNMesh &mesh, Tab<Point3> & norm, Tab<Point3> & ctr);
	/*! \remarks This method will finds the edge list that borders this
	cluster. This edge list is a set of closed loops of edges, which may be
	empty. For instance, if the mesh is a sphere, and all the faces are in the
	cluster, there are no border edges for the cluster. But if one horizontal
	row of faces, such as the faces just above the equator, are in the cluster,
	then the edges above those faces form one loop, while the edges below form
	another.
	\par Parameters:
	<b>MNMesh \&mesh</b>\n\n
	The mesh this face cluster is based on.\n\n
	<b>int clustID</b>\n\n
	The ID of the cluster we want to get the border of.\n\n
	<b>Tab\<int\> \& cbord</b>\n\n
	The table for putting the border output. This table is set up as follows:
	each border loop is represented by a series of edge indices, followed by a
	-1 to indicate a separation between loops. So a result of size 10 with data
	1, 4, 6, 9, -1, 2, 10, 15, 14, -1, would indicate two border loops
	consisting of four edges each. The order of the edges in the loops is
	chosen so that as you look from outside the mesh and follow the path of the
	edges, the face cluster will always be on the left. */
	DllExport void GetBorder (MNMesh &mesh, int clustID, Tab<int> & cbord);
	/*! \remarks This method will retrieve the "outline" direction for the
	border of the cluster. This is the direction used in the "Outline" feature
	in Editable Poly face level, as well as in the Bevel command mode.
	\par Parameters:
	<b>MNMesh \&m</b>\n\n
	The mesh this face cluster is based on.\n\n
	<b>Tab\<Point3\> \& cnorms</b>\n\n
	The cluster normals, as computed in the GetNormalsCenters method. (This
	data is input, not output.)\n\n
	<b>Tab\<Point3\> \& odir</b>\n\n
	This is where the outline vectors are stored. This table is set to size
	<b>mesh.VNum()</b>, and stores one direction vector for each vertex. Most
	direction vectors are usually zero, since most vertices are not on the
	cluster's border. All vectors are scaled so that moving along them moves
	the cluster's border edges by one unit. (For instance, the length of a
	vector at a right angle between two border edges would be <b>sqrt(2)</b>,
	so that each edge can move by 1 unit "out" from the cluster.) */
	DllExport void GetOutlineVectors (MNMesh & m, Tab<Point3> & cnorms, Tab<Point3> & odir);
};

/*! \sa  Class MNTempData, Class MNMesh\n\n
\par Description:
This class is available in release 4.0 and later only.\n\n
This class represents a list of edge "clusters" for a given <b>MNMesh</b>. A
typical application would be in Editable Poly, where the user has selected a
two separate groups of edges on different parts of the mesh and wants to rotate
both around their local centers. Each "cluster" is a contiguous group of
selected edges (ie they all touch each other). This class is only defined in
relation to some <b>MNMesh</b>.\n\n
For convenient caching, it is recommended that you use this class through the
<b>MNTempData</b> class.
\par Data Members:
<b>Tab\<int\> clust;</b>\n\n
The cluster IDs of all the edges - this table has size <b>MNMesh::nume</b>.
<b>clust[i]</b> is UNDEFINED if edge i is not in any cluster.\n\n
<b>int count;</b>\n\n
The total number of clusters.  */
class MNEdgeClusters: public MaxHeapOperators {
public:
	Tab<int> clust;
	int count;

	/*! \remarks Constructor.
	\par Parameters:
	<b>MNMesh \&m</b>\n\n
	The mesh these clusters are based on.\n\n
	<b>DWORD clusterFlags</b>\n\n
	The edge flags to cluster the edges by.\n\n
	For instance, if this value was set to <b>MN_SEL</b>, then edges would be
	clustered by their selection. */
	DllExport MNEdgeClusters (MNMesh &mesh, DWORD clusterFlags);
	/*! \remarks Index operator to access cluster data.
	\par Default Implementation:
	<b>{ return clust[i]; }</b> */
	int operator[](int i) {return clust[i];}
	/*! \remarks This method will create a list of cluster IDs for vertices.
	\par Parameters:
	<b>MNMesh \&m</b>\n\n
	The mesh associated with these <b>MNEdgeClusters.</b>\n\n
	<b>Tab\<int\> \& vclust</b>\n\n
	This is where the output goes: <b>vclust</b> is set to size
	<b>MNMesh::numv</b>, and the value of each entry in this table tells which
	cluster the vertex has been assigned to, based on the edges using it. If
	vertex "v" is not in any clusters (i.e. none of the edges that use it are
	in any clusters), <b>vclust[v]</b> is UNDEFINED. */
	DllExport void MakeVertCluster (MNMesh &mesh, Tab<int> & vclust);
	/*! \remarks This method extracts normal and center information for each
	of the edge clusters.
	\par Parameters:
	<b>MNMesh \&m</b>\n\n
	The mesh associated with these <b>MNEdgeClusters.</b>\n\n
	<b>Tab\<Point3\> \& norm</b>\n\n
	This table has its size set to the number of clusters in the cluster list.
	Normals are computed as the normalized average of the normal vectors of all
	edges in the cluster.\n\n
	<b>Tab\<Point3\> \& ctr</b>\n\n
	This table has its size set to the number of clusters in the cluster list.
	Centers are the average location of the edge centers -- thus a point on
	three edges in the same cluster has more weight than a point on only one
	edge in the cluster. */
	DllExport void GetNormalsCenters (MNMesh &mesh, Tab<Point3> & norm, Tab<Point3> & ctr);
};

/*! \sa  Class MNTempData, Class MNMesh\n\n
\par Description:
This class is available in release 4.0 and later only.\n\n
This class contains all the data needed to move points (and map vertices) as
the user drags a chamfer or extrude. It's created by the topological change
that happens at the start of the chamfer or extrude. The strategy is this: The
chamfer/extrude operation is divided into two parts, the topological change and
a later geometric change. (This works well for Editable Poly, where the
topology change is completed first, then apply a series of geometry changes as
the user spins a spinner or drags a mouse. Each geometry change is undone
before the next is applied, but the topology change only happens once.)\n\n
This class is first initialized to a mesh. Then its data is filled in by the
topological change. This data is used to find "directions" for all the
geometric and mapping vert changes over the course of the geometric
modification.\n\n
For convenient caching, it is recommended that you use this class through the
<b>MNTempData</b> class.
\par Data Members:
private:\n\n
<b>Tab\<UVVert\> hmdir[NUM_HIDDENMAPS];</b>\n\n
The direction vectors for mapping vertices in any active "hidden" mapping
channels.\n\n
<b>Tab\<Point3\> vdir;</b>\n\n
The related direction vectors for mapping vertices in all active mapping
channels.\n\n
<b>Tab\<Point3\> vmax;</b>\n\n
The maximum amount each vector may be applied (before vertices start crossing
over each other).\n\n
<b>Tab\<UVVert\> *mdir;</b>\n\n
The related direction vectors for mapping vertices in all active mapping
channels.  */
class MNChamferData: public MaxHeapOperators {
	Tab<UVVert> hmdir[NUM_HIDDENMAPS];
public:
	Tab<Point3> vdir;
	Tab<float> vmax;
	Tab<UVVert> *mdir;

	/*! \remarks Constructor.
	\par Default Implementation:
	<b>{ mdir=NULL; }</b> */
	MNChamferData () { mdir=NULL;}
	/*! \remarks Constructor.\n\n
	This constructor Initializes to the mesh passed (allocates mapping
	channels, etc.)
	\par Parameters:
	<b>const MNMesh \& m</b>\n\n
	The specified mesh.
	\par Default Implementation:
	<b>{ mdir=NULL; InitToMesh(m); }</b> */
	MNChamferData (const MNMesh & m) { mdir=NULL; InitToMesh(m);}
	/*! \remarks Destructor.
	\par Default Implementation:
	<b>{ if (mdir) delete [] mdir; }</b> */
	DllExport ~MNChamferData ();

	/*! \remarks This method sets up the <b>MNChamferData</b> based on a given
	mesh, allocating the vertex and mapping vertex tables as appropriate.
	\par Parameters:
	<b>const MNMesh \& m</b>\n\n
	The Mesh to initialize from. */
	DllExport void InitToMesh (const MNMesh & m);
	/*! \remarks This method simply allocates the <b>vdir</b> and <b>vmax</b>
	tables, and initializes the new members of <b>vmax</b> to 0. (Note: this
	method can be applied to an existing <b>MNChamferData</b> to reflect an
	increase in vertices in the <b>MNMesh</b> as topological changes occur.)
	\par Parameters:
	<b>int nv</b>\n\n
	The number of vertices.\n\n
	<b>bool keep=TRUE</b>\n\n
	TRUE to keep old data if resized; FALSE to discard old data.\n\n
	<b>int resizer=0</b>\n\n
	The number of extra elements the <b>vdir</b> and <b>vmax</b> tables are
	resized beyond their current size. (Extra allocation space to prevent
	excessive reallocation.) */
	DllExport void setNumVerts (int nv, bool keep=TRUE, int resizer=0);
	/*! \remarks Clears all the <b>vmax</b> limits to -1 (no limit). */
	DllExport void ClearLimits ();
	/*! \remarks Uses vectors and limits to obtain the offsets corresponding
	to a certain extrude or chamfer amount.
	\par Parameters:
	<b>float amount</b>\n\n
	The amount of the extrude or chamfer.\n\n
	<b>Tab\<Point3\> \& delta</b>\n\n
	A table (with size set equal to the number of vertices, aka
	<b>vdir.Count()</b>) containing the geometric offset for each vertex in the
	mesh. */
	DllExport void GetDelta (float amount, Tab<Point3> & delta);
	/*! \remarks Uses map vectors and limits to obtain the mapping offsets
	corresponding to a certain extrude or chamfer amount.
	\par Parameters:
	<b>MNMesh \& mm</b>\n\n
	The mesh this <b>MNChamferData</b> is based on.\n\n
	<b>int mapChannel</b>\n\n
	The index of the map channel (from <b>-NUM_HIDDENMAPS</b> to
	<b>mm.MNum()</b>).\n\n
	<b>float amount</b>\n\n
	The amount of the extrude or chamfer.\n\n
	<b>Tab\<UVVert\> \& delta</b>\n\n
	The offsets for each mapping vertex in this map in the mesh. */
	DllExport bool GetMapDelta (MNMesh & mm, int mapChannel, float amount, Tab<UVVert> & delta);
	/*! \remarks Data accessor. This method returns the appropriate map info.
	If <b>mp</b> \>= 0, it returns the member of the <b>mdir</b> array. If
	<b>mp</b> \< 0, it returns the member of the <b>hmdir</b> array (in keeping
	with "hidden map channel" indexing conventions).
	\par Default Implementation:
	<b>{ return (mp\<0) ? hmdir[-1-mp] : mdir[mp]; }</b>\n\n
	  */
	Tab<UVVert> & MDir (int mapChannel) { return (mapChannel<0) ? hmdir[-1-mapChannel] : mdir[mapChannel]; }
};



// The following function has been added
// in 3ds max 4.2.  If your plugin utilizes this new
// mechanism, be sure that your clients are aware that they
// must run your plugin with 3ds max version 4.2 or higher.
/*! \remarks This function uses calls to DebugPrint() to output all the data
in the specified MNChamferData to the DebugPrint buffer during debug runs. It
is available for programmers' use, providing easy access to MNChamferData
during development. It ought to be removed for release builds.
\par Parameters:
<b>MNChamferData \& mcd</b>\n\n
The MNChamferData we want to investigate.\n\n
<b>int mapNum</b>\n\n
The number of map channels in the MNMesh associated with this MNChamferData .
(For historical reasons, this information is not kept in the MNChamferData
class.) Generally this is retrieved with a call to MNMesh::MNum(). */
DllExport void MNChamferDataDebugPrint (MNChamferData & mcd, int mapNum);
// End of 3ds max 4.2 Extension

/*! \sa  Class BaseInterfaceServer,  Class MNMesh, Class MNFaceClusters, Class MNEdgeClusters , Class MNFaceElement, Class MNChamferData\n\n
\par Description:
This class is available in release 4.0 and later only.\n\n
This is a class for caching face and edge clusters, vertex normals, and other
derived data about an <b>MNMesh</b>. There is a <b>SetMesh()</b> method to set
the current mesh that the TempData is based on, then there's a series of
methods to update the cache and return some sort of derived data. All of these
methods follow the form:\n\n
<b>DerivedData *MNTempData::DData(parameters);</b>\n\n
DerivedData is the container for the derived data requested (often a simple
table, though there are some specialized classes returned from some methods).
If the data has already been computed, the parameters are ignored and the
cached data is returned. Otherwise, the data is computed from the parameters
and the\n\n
current mesh.\n\n
There are no procedures in place to detect changes in parameters or the mesh
since the last time a method was called, so it's the calling routine's
responsibility to free invalid structures. If you know that only certain
pipeline channels, such as <b>GEOM_CHANNEL</b>, have changed, you can use the
<b>Invalidate(DWORD partsChanged)</b> method. (<b>GEOM_CHANNEL</b> would free
the distances-to-selected-vertices, for example, but not the Edge
Clusters.)\n\n
In particular, there is no way for the <b>MNTempData</b> to know when its mesh
pointer is no longer valid, so it's vital that the calling routine clear the
mesh (with <b>SetMesh(NULL)</b>) or stop using the MNTempData when this
happens.\n\n
All data members are private. They basically consist of a series of pointers
which are initialized to NULL and then filled with allocated derived data as
requested. There is also a NULL-initialized, private mesh pointer which is set
with <b>SetMesh()</b>. Editable Poly uses this class to hold all the varieties
of temporary, cached data it creates -- examples are vertex normals and face
clusters.\n\n
To use MNTempData, just set it to your mesh and start asking for stuff:\n\n
<b>MyAlgorithm (MNMesh *m) {</b>\n\n
<b>MNTempData mtd(m);</b>\n\n
<b>// Get Edge Clusters.</b>\n\n
<b>MNEdgeClusters *eclust = mtd.EdgeClusters ();</b>\n\n
<b>}</b>  */
class MNTempData : public BaseInterfaceServer {
private:
	MNEdgeClusters *edgeCluster;
	MNFaceClusters *faceCluster;
	Tab<int> *vertCluster;
	Tab<Point3> *normals;
	Tab<Point3> *centers;
	Tab<Point3> *vnormals;
	Tab<Tab<float> *> *clustDist;
	Tab<float> *selDist;
	Tab<float> *vsWeight;
	MNChamferData *chamData;

	Tab<Point3> *outlineDir;

	MNMesh *mesh;

	//this is used to keep track of the selDist data.  We used to compute all the seldist data which
	//was really slow.  We now only track the selDist that fall within  this limit which can make things
	//much faster.
	float mFalloffLimit;

public:
	/*! \remarks Constructor. Sets all data members to NULL. */
	DllExport MNTempData ();
	/*! \remarks Constructor. Sets the internal mesh pointer to the mesh
	passed. (Sets all other data members to NULL.)
	\par Parameters:
	<b>MNMesh *m</b>\n\n
	The internal mesh pointer to set. */
	DllExport MNTempData (MNMesh *m);
	/*! \remarks Destructor. */
	DllExport ~MNTempData ();

	/*! \remarks Sets the internal mesh pointer to <b>m</b>.
	\par Parameters:
	<b>MNMesh *m</b>\n\n
	The internal mesh pointer to set.
	\par Default Implementation:
	<b>{ mesh = m; }</b> */
	void SetMesh (MNMesh *m) { mesh = m; }

	/*! \remarks This method returns a face cluster list, which groups
	selected faces into "clusters" for transformation. See
	Class MNFaceClusters for more
	information. If cached, the cache is returned. Otherwise a cache is
	allocated and computed from the current mesh.
	\par Parameters:
	<b>DWORD clusterFlags=MN_SEL</b>\n\n
	The face flags to cluster the faces by. For instance, with the default
	value, faces are clustered by their selection. */
	DllExport MNFaceClusters *FaceClusters (DWORD clusterFlags=MN_SEL);
	/*! \remarks Returns an edge cluster list, which groups selected edges
	into "clusters" for applying transforms. See
	Class MNEdgeClusters for more
	information. If cached, the cache is returned. Otherwise a cache is
	allocated and computed from the current mesh.
	\par Parameters:
	<b>DWORD clusterFlags=MN_SEL</b>\n\n
	The edge flags to cluster the edges by. For instance, with the default
	value, edges are clustered by their selection. */
	DllExport MNEdgeClusters *EdgeClusters (DWORD clusterFlags=MN_SEL);
	/*! \remarks This method returns an index of which cluster, if any, each
	vertex is in. If cached, the cache is returned. Otherwise a cache is
	allocated and computed from the current mesh and the parameter.
	\par Parameters:
	<b>int sl</b>\n\n
	The selection level. This should be either <b>MNM_SL_EDGE</b> or
	<b>MNM_SL_FACE</b>, to indicate whether the vertex cluster information
	should be based on edge or face clusters. Note that this parameter is
	ignored if there's already a vertex cluster cache.\n\n
	<b>DWORD clusterFlags=MN_SEL</b>\n\n
	The edge or face flags to cluster the edges or faces by. For instance, with
	the default value, edges or faces are clustered by their selection. Note
	that this parameter is ignored if there's already a vertex cluster cache.
	\return  A table of DWORD's is returned, one for each vertex. If
	(<b>VertexClusters(sl))[i]</b> is <b>UNDEFINED</b>, vertex i is not in any
	cluster. Otherwise, the value for vertex i is the cluster index. */
	DllExport Tab<int> *VertexClusters (int sl, DWORD clusterFlags=MN_SEL);
	/*! \remarks This method returns average normals for each cluster. If
	cached, the cache is returned. Otherwise a cache is allocated and computed
	from the current mesh and the parameter. Note that cluster centers and
	normals are computed and cached at the same time, when you call either
	method.
	\par Parameters:
	<b>int sl</b>\n\n
	The selection level. This should be either <b>MNM_SL_EDGE</b> or
	<b>MNM_SL_FACE</b>, to indicate whether the vertex cluster information
	should be based on edge or face clusters. Note that this parameter is
	ignored if there's already a cluster normal cache.\n\n
	<b>DWORD clusterFlags=MN_SEL</b>\n\n
	The edge or face flags to cluster the edges or faces by. For instance, with
	the default value, edges or faces are clustered by their selection. Note
	that this parameter is ignored if there's already a cluster normal cache.
	\return  A table of Point3's is returned, one for each cluster. The values
	are already normalized to length 1. */
	DllExport Tab<Point3> *ClusterNormals (int sl, DWORD clusterFlags=MN_SEL);
	/*! \remarks This method returns mean centers for each cluster. If cached,
	the cache is returned. Otherwise a cache is allocated and computed from the
	current mesh and the parameter. Note that cluster centers and normals are
	computed and cached at the same time, when you call either method.
	\par Parameters:
	<b>int sl</b>\n\n
	Selection level. This should be either <b>MNM_SL_EDGE</b> or
	<b>MNM_SL_FACE</b>, to indicate whether the clusters we're talking about
	are the edge or face clusters. Note that this parameter is ignored if
	there's already a cluster center cache.\n\n
	<b>DWORD clusterFlags=MN_SEL</b>\n\n
	The edge or face flags to cluster the edges or faces by. For instance, with
	the default value, edges or faces are clustered by their selection. Note
	that this parameter is ignored if there's already a cluster center cache.
	\return  A table of Point3's is returned, one for each cluster. */
	DllExport Tab<Point3> *ClusterCenters (int sl, DWORD clusterFlags=MN_SEL);
	/*! \remarks This method uses the current cluster center and normal caches
	to return the "objectspace to clusterspace" transform. This is the transform
	of the "local" axis in moving edge or face clusters in Editable Poly. If
	the cluster centers and normals have not been cached, the identity matrix
	is returned; thus the control over whether this is an edge or face cluster
	is handled by the last call to ClusterCenters or ClusterNormals.
	\par Parameters:
	<b>int clust</b>\n\n
	The cluster you want the transform for. */
	DllExport Matrix3 ClusterTM (int clust);
	/*! \remarks This method returns a table of local average normals for
	vertices. These normals are computed using the
	<b>MNMesh::GetVertexNormal()</b> method, wherein each face's contribution
	to the vertex normal is weighted by the face angle at the vertex. */
	DllExport Tab<Point3> *VertexNormals ();
	/*! \remarks This method returns Vertex Selection weights (for soft
	selection). If cached, the cache is returned. Otherwise a cache is
	allocated and computed from the current mesh and the parameters. Weights
	are based on the standard soft selection falloff from the currently
	selected vertices.\n\n
	Note: If <b>useEdgeDist</b> is FALSE, this is an n-log-n algorithm: it
	compares every vertex not in the cluster with every vertex in it. If
	<b>useEdgeDist</b> is TRUE, the time it takes is proportional to the number
	of <b>verts</b> in the cluster times <b>edgeIts</b>.
	\par Parameters:
	<b>BOOL useEdgeDist</b>\n\n
	If set to TRUE, the distance between vertices is computed along edges. If
	set to FALSE, it's computed directly through space.\n\n
	<b>int edgeIts</b>\n\n
	This indicates the maximum number of edges the algorithm may travel along
	in finding the distance between vertices. (Maximum path length.)\n\n
	<b>BOOL ignoreBack</b>\n\n
	If set to TRUE, vertices with a normal (as computed in VertexNormals) that
	points more than 90 degrees away from the average normal of the selection
	are not given any partial selections. They're either 1 if selected or 0
	otherwise.\n\n
	<b>float falloff</b>\n\n
	The limit distance of the effect. If distance \> falloff, the function will
	always return 0.\n\n
	<b>float pinch</b>\n\n
	Use this to affect the tangency of the curve near distance=0. Positive
	values produce a pointed tip, with a negative slope at 0, while negative
	values produce a dimple, with positive slope.\n\n
	<b>float bubble</b>\n\n
	Use this to change the curvature of the function. A value of 1.0 produces a
	half-dome. As you reduce this value, the sides of the dome slope more
	steeply. Negative values lower the base of the curve below 0.\n\n
	<b>DWORD selFlags=MN_SEL</b>\n\n
	Indicates what flag defines the hard selection we're basing this soft
	selection on.
	\return  A table of float values, one per vertex, that are 1.0 if the
	vertex is in the current selection, 0.0 if it's more than falloff distance
	(or more than edgeIts edges, if (useEdgeDist)), and
	AffectRegionFunction((*SelectionDist(useEdgeDist, edgeIts)), falloff,
	pinch, bubble) otherwise. */
	DllExport Tab<float> *VSWeight (BOOL useEdgeDist, int edgeIts,
									BOOL ignoreBack, float falloff, float pinch, float bubble,
									DWORD selFlags=MN_SEL);

	//! \brief This computes the distances of the unselected vertices from the selected vertices
	/*!	Computes the current distance of each vertex from the current selection within the falloff. If cached, the 
	    cache is returned. Otherwise a cache is allocated and computed from the current mnmesh and 
		the parameters. The term "Selected verts" below refers to the vertices that are selected 
		in the mesh's current selection level. (See the MNMesh method GetTempSel for details.)
		\param[in] useEdgeDist If TRUE, the distance between vertices is computed along edges. If FALSE, it's computed directly through space. 
		\param[in] edgeIts This indicates the maximum number of edges the algorithm may travel 
					along in finding the distance between vertices. (Maximum path length.).
					WARNING: If useEdgeDist is FALSE, this is an n-squared algorithm: it compares 
					every nonselected vertex with every selected one within the falloffLimit. If useEdgeDist is TRUE, 
					the time it takes is proportional to the number of selected vertices times 
					edgeIts. 
		\param[in] falloffLimit = -1.0f this limits the number of unselected vertices to check against
		           Only vertices that fall within the selected vertices bounding box + this limit will be
				   computed.  If this value is -1.0 all vertices will be computed.  Any vertex outside this limit
				   will be set to a distance of the limit.
		\return A table consisting of one float value per vertex. If this value is 0, the vertex is either 
					selected or on top of a selected vertex. Otherwise it represents the distance to the closest 
					selected vertex. If useEdgeDist is TRUE, values of -1.0 are returned for vertices with no edgeIts-length 
					path to a selected vertex.
	*/
	DllExport Tab<float> *SelectionDist (BOOL useEdgeDist, int edgeIts, DWORD selFlags=MN_SEL, float falloffLimit = -1.0f);
	/*! \remarks This method computes the current distance of each vertex from
	the specified cluster. If cached, the cache is returned. Otherwise a cache
	is allocated and computed from the current mesh and the parameters.\n\n
	NOTE: If <b>useEdgeDist</b> is FALSE, this is an n-log-n algorithm: it
	compares every vertex not in the cluster with every vertex in it. If
	<b>useEdgeDist</b> is TRUE, the time it takes is proportional to the number
	of verts in the cluster times <b>edgeIts</b>.
	\par Parameters:
	<b>int sl</b>\n\n
	Indicates whether we should use edges (<b>MNM_SL_EDGE</b>) or faces
	(<b>MNM_SL_FACE</b>) to construct the clusters, if needed.\n\n
	<b>DWORD clusterFlags</b>\n\n
	The edge or face flags to cluster the edges or faces by. For instance, if
	<b>clusterFlags==MN_SEL</b>, edges or faces are clustered by selection.\n\n
	<b>int clustId</b>\n\n
	The ID of the cluster we're measuring distance from.\n\n
	<b>BOOL useEdgeDist</b>\n\n
	If set to TRUE, the distance between vertices is computed along edges. If
	set to FALSE, it's computed directly through space.\n\n
	<b>int edgeIts</b>\n\n
	This indicates the maximum number of edges the algorithm may travel along
	in finding the distance between vertices. (Maximum path length.)
	\return  A table consisting of one float value per vertex. If this value is
	0, the vertex is either selected or on top of a vertex in the cluster.
	Otherwise it represents the distance to the closest selected vertex. If
	<b>useEdgeDist</b> is TRUE, values of -1.0 are returned for vertices with
	no <b>edgeIts</b>-length path to a vertex in the cluster. */
	DllExport Tab<float> *ClusterDist (int sl, DWORD clusterFlags, int clustId, BOOL useEdgeDist, int edgeIts);
	//DllExport Tab<Point3> *FaceExtDir (int extrusionType);
	/*! \remarks This method produces the "Outline" direction of all vertices,
	based on the current face selection. "Outlining" is the direction vertices
	move to move edges of the current face selection outward at a constant
	rate. They are not set to length 1, but rather to whatever "rate" best
	makes the outline edges move most consistently, without changing their
	angles.
	\par Parameters:
	<b>int extrusionType</b>\n\n
	This is one of <b>MESH_EXTRUDE_CLUSTER</b> or <b>MESH_EXTRUDE_LOCAL</b>, to
	indicate whether vertices should move according to cluster or local face
	normals.\n\n
	<b>DWORD clusterFlags=MN_SEL</b>\n\n
	The face flags to cluster the faces by. For instance, if left at the
	default value, faces are clustered by selection. Note that this parameter
	is ignored if the <b>extrusionType</b> is LOCAL. */
	DllExport Tab<Point3> *OutlineDir (int extrusionType, DWORD clusterFlags=MN_SEL);

	/*! \remarks This method returns the cache of a <b>ChamferData</b> for use
	in the <b>MNMesh</b> Chamfer methods. Unlike other <b>MeshTempData</b>
	methods, this method makes no calculations based on the current mesh, but
	merely supplies a memory cache. (Computing this information is left to the
	<b>MNMesh</b> methods <b>GetExtrudeDirection</b>, <b>ChamferVertices</b>,
	and <b>ChamferEdges</b>.) */
	DllExport MNChamferData *ChamferData();

	/*! \remarks This method invalidates all data based on the specified part
	of the mesh. In the following chart, the columns represent the channels GEOM_CHANNEL (G),
        TOPO_CHANNEL (T), SELECT_CHANNEL (S), and SUBSEL_TYPE_CHANNEL (U). X's indicate
        dependency of the specified data cache on the given channel.\n
	<table><tr>
	<td><b>Method to get cache</b></td>
	<td><b>G</b></td>
	<td><b>T</b></td>
	<td><b>S</b></td>
	<td><b>U</b></td>
	</tr>
	<tr>
	<td>FaceClusters</td>
	<td> </td>
	<td>X</td>
	<td>X</td>
	<td> </td>
	</tr>
	<tr>
	<td>EdgeClusters</td>
	<td> </td>
	<td>X</td>
	<td>X</td>
	<td> </td>
	</tr>
	<tr>
	<td>VertexClusters</td>
	<td> </td>
	<td>X</td>
	<td>X</td>
	<td>X</td>
	</tr>
	<tr>
	<td>ClusterCenters</td>
	<td>X</td>
	<td>X</td>
	<td>X</td>
	<td>X</td>
	</tr>
	<tr>
	<td>ClusterNormals</td>
	<td>X</td>
	<td>X</td>
	<td>X</td>
	<td>X</td>
	</tr>
	<tr>
	<td>VertexNormals</td>
	<td>X</td>
	<td>X</td>
	<td> </td>
	<td> </td>
	</tr>
	<tr>
	<td>SelectionDist</td>
	<td>X</td>
	<td>X</td>
	<td>X</td>
	<td>X</td>
	</tr>
	<tr>
	<td>ClusterDist</td>
	<td>X</td>
	<td>X</td>
	<td>X</td>
	<td>X</td>
	</tr>
	<tr>
	<td>VSWeight</td>
	<td>X</td>
	<td>X</td>
	<td>X</td>
	<td>X</td>
	</tr></table>
	NOTE: <b>ChamferData</b> and the Outline direction info are handled
	separately in <b>freeChamferData</b> and <b>freeBevelInfo</b>.
	\par Parameters:
	<b>DWORD part</b>\n\n
	One or more of the following channels: <b>GEOM_CHANNEL, TOPO_CHANNEL,
	SELECT_CHANNEL, SUBSEL_TYPE_CHANNEL</b>. */
	DllExport void Invalidate (DWORD part);
	/*! \remarks This method Uncaches (frees) the distance dependent data
	returned by <b>VSWeight</b>, <b>SelectionDist</b>, and <b>ClusterDist</b>.
	*/
	DllExport void InvalidateDistances ();
	/*! \remarks This method frees the <b>VSWeight</b> data (but not the
	underlying distance-from-selection info). This is useful, e.g., if the mesh
	has not changed, but you wish to change the falloff, pinch, or bubble
	parameters to get new vertex selection weights. */
	DllExport void InvalidateSoftSelection ();
	/*! \remarks This method is mainly for internal use, this frees just the
	cluster distance data. */
	DllExport void freeClusterDist ();
	/*! \remarks This method frees only the outlining data. */
	DllExport void freeBevelInfo ();
	/*! \remarks This method frees only the chamfer data structure. */
	DllExport void freeChamferData();
	/*! \remarks This method frees all cached data of any kind. */
	DllExport void freeAll ();

	// Added by PSF 
	DllExport BaseInterface* GetInterface(Interface_ID id);
};


#define IMNTEMPDATA10_INTERFACE_ID		Interface_ID(0x56d8fa2, 0x3459c774)
//! \sa IMNMeshUtilities10
//! \brief Interface for changes made to MNTempData in version 9.5. For use with IMNMeshUtilities10.
//! \n
//! Usage: 
//! \code
//! IMNTempData10* tempData10 = static_cast<IMNTempData10*>(tempData.GetInterface( IMNTEMPDATA10_INTERFACE_ID ));
//! \endcode

class IMNTempData10 : public BaseInterface {
public:
	Interface_ID	GetID() { return IMNTEMPDATA10_INTERFACE_ID; }
	/*! \brief Returns pointer to MNChamferData10 object. IMNTempData10 owns the memory.
	\return pointer to allocated MNChamferData10 object
	*/
	virtual MNChamferData10& ChamferData() = 0;
	//! \brief Deallocates data allocated by ChamferData(). MNTempData::freeChamferData calls this function.
	virtual void freeChamferData() = 0;
};

/*! \remarks This function computes the current distance of each vertex from
the current selection. NOTE: This is an n-log-n algorithm: it compares every
non-selected vertex with every selected one.
\par Parameters:
<b>MNMesh \& mesh</b>\n\n
The mesh we're computing distances in.\n\n
<b>float *selDist</b>\n\n
A pointer to an array of floats of size <b>mesh.VNum()</b>. This array is
filled in with one float value per vertex. If this value is 0, the vertex is
either selected or "on top of" a selected vertex. Otherwise it represents the
distance to the closest selected vertex.\n\n
<b>DWORD selFlags</b>\n\n
Indicates what flag defines the hard selection we're basing this soft selection
on. (<b>MN_SEL</b> is generally best.) */
DllExport void SelectionDistance (MNMesh & mesh, float *selDist, DWORD selFlags);
/*! \remarks This function computes the current distance of each vertex from
the current selection, along paths of edges. NOTE: This is an n-log-n
algorithm: it compares every non-selected vertex with every selected one.
\par Parameters:
<b>MNMesh \& mesh</b>\n\n
The mesh we're computing distances in.\n\n
<b>float *selDist</b>\n\n
A pointer to an array of floats of size <b>mesh.VNum()</b>. This array is
filled in with one float value per vertex. If this value is 0, the vertex is
either selected or "on top of" a selected vertex. Otherwise it represents the
distance to the closest selected vertex. Values of -1 are used to indicate
vertices that are more that <b>iters</b> edges away from any selected
vertex.\n\n
<b>int iters</b>\n\n
This indicates the maximum number of edges the algorithm may travel along in
finding the distance between vertices. (Maximum path length.).\n\n
<b>DWORD selFlags</b>\n\n
Indicates what flag defines the hard selection we're basing this soft selection
on. (<b>MN_SEL</b> is generally best.) */
DllExport void SelectionDistance (MNMesh & mesh, float *selDist, int iters, DWORD selFlags);
/*! \remarks Computes the current distance of each vertex from the specified
cluster. If cached, the cache is returned. Otherwise a cache is allocated and
computed from the current mesh and the parameters.\n\n
NOTE: This is an n-log-n algorithm for each cluster: it compares every vertex
not in the cluster with every vertex in it.
\par Parameters:
<b>MNMesh \& mesh</b>\n\n
The MNMesh these cluster distances are based on.\n\n
<b>int numClusts</b>\n\n
The number of clusters available.\n\n
<b>int *vclust</b>\n\n
A pointer into the vertex cluster table.\n\n
<b>Tab\<float\> **clustDist</b>\n\n
An array of pointers to tables which will be filled with one float value per
vertex. The table pointed to by <b>clustDist[i]</b> contains the cluster
distances for cluster i. Note that <b>clustDist</b> must be allocated, for
instance by <b>clustDist = new (Tab\<float\>*)[numClusts]</b>, and must have
its members allocated, for instance by <b>clustDist[i] = new Tab\<float\></b>,
by the calling routine. If a value in a table is 0, the vertex is either
selected or on top of a vertex in the cluster. Otherwise it represents the
distance to the closest selected vertex. If <b>useEdgeDist</b> is TRUE, values
of -1.0 are returned for vertices with no <b>edgeIts</b>-length path to a
vertex in the cluster. */
DllExport void ClustDistances (MNMesh & mesh, int numClusts, int *vclust,
							   Tab<float> **clustDist);
/*! \remarks Computes the current distance of each vertex from the specified
cluster. If cached, the cache is returned. Otherwise a cache is allocated and
computed from the current mesh and the parameters.\n\n
NOTE: This algorithm takes time proportional to the number of <b>verts</b> in
each cluster times <b>iters</b> times the number of clusters.
\par Parameters:
<b>MNMesh \& mesh</b>\n\n
The MNMesh these cluster distances are based on.\n\n
<b>int numClusts</b>\n\n
The number of clusters available.\n\n
<b>int *vclust</b>\n\n
A pointer into the vertex cluster table.\n\n
<b>Tab\<float\> **clustDist</b>\n\n
An array of pointers to tables which will be filled with one float value per
vertex. The table pointed to by <b>clustDist[i]</b> contains the cluster
distances for cluster i. Note that <b>clustDist</b> must be allocated, for
instance by <b>clustDist = new (Tab\<float\> *)[numClusts]</b>, and must have
its members allocated, for instance by <b>clustDist[i] = new Tab\<float\></b>,
by the calling routine. If a value in a table is 0, the vertex is either
selected or on top of a vertex in the cluster. Otherwise it represents the
distance to the closest selected vertex. Values of -1.0 are returned for
vertices with no <b>iters</b>-length path to a vertex in the cluster.\n\n
<b>int iters</b>\n\n
The maximum edge path length to compute distance along (in number of edges). */
DllExport void ClustDistances (MNMesh & mesh, int numClusts, int *vclust,
							   Tab<float> **clustDist, int iters);

// Made into a class to match design rules.
/*! This class computes two components of a basis vector given a single input
component.\n\n
   */
class BasisFinder: public MaxHeapOperators {
public:
	/*! \remarks Computes the x and y components of a basis vector given a z
	component. The orientation of x and y are arbitrary and may be oriented at
	any angle about z.\n\n

	\par Parameters:
	<b>Point3 zDir</b>\n\n
	The input z orientation.\n\n
	<b>Point3 xDir</b>\n\n
	The resultant x orientation.\n\n
	<b>Point3 yDir</b>\n\n
	The resultant y orientation. */
	DllExport static void BasisFromZDir (Point3 & zDir, Point3 & xDir, Point3 & yDir);
};

const int kMNNurmsHourglassLimit = 2500;	// between 4 and 5 iterations on a box.

//#define MNM_SELCONV_IGNORE_BACK 0x01	// Doesn't work - can't get good BACKFACING info.
#define MNM_SELCONV_REQUIRE_ALL 0x02

/*!  \n\n
class MeshSelectionConverter\n\n

\par Description:
This class is available in release 5.1 and later only.\n\n
When a Mesh object or modifier (such as Editable Mesh, Edit Mesh, or Mesh
Select) handles viewport subobject selection, the flow is basically as follows
from Editable Mesh:\n\n
- EditTriObject::HitTest is called with window information.\n\n
- It passes the hit region description to Mesh::SubObjectHitTest\n\n
- Mesh::SubObjectHitTest culls some subobjects (say, because they're
backfacing), and creates hits for others that are within the hit region.\n\n
- EditTriObject::SelectSubComponent is called with the list of hits, and\n\n
constructs the appropriate BitArray selection.\n\n
The problem was that in cases of Window selection, you want to only select an
element or polygon if all the faces of that element or polygon were in the
selection region. But since some of these faces could have been culled, they
might not have hit records even if they're within the correct region. This
would mean, for instance, that a window selection of an entire Sphere would
fail to get select its single element, because all the backfaces would not\n\n
have been selected.\n\n
So in order to do this correctly, we need the face culling information. This
information is stored and applied in the new MeshSelectionConverter interface,
which is part of class Mesh.\n\n
This sort of culling and subobject level translation is not only relevant for
Polygon and Element selections, but also for "By Vertex" selections of edges,
faces, polygons, or elements.\n\n
Examples of the use of this interface are available in
maxsdk/SAMPLES/mesh/editablemesh/triobjed.cpp, and in meshsel.cpp and
editmesh.cpp in maxsdk/SAMPLES/modifiers..\n\n
Owner: class Mesh\n\n
InterfaceID: MESHSELECTCONVERT_INTERFACE\n\n

\par Flags:
We have the usual three methods for setting, clearing, and getting flags: void
SetFlag (DWORD f, bool value=true) void ClearFlag(DWORD f) bool GetFlag (DWORD f)\n
We currently only have one flag influencing subobject selection conversion:
MESH_SELCONV_REQUIRE_ALL\n
If set, it implies that, for instance, an element is only selected if all of
its (non-culled) faces are selected. If cleared, an element is selected if any
of its faces are selected.
*/
class MNMeshSelectionConverter : public FlagUser {
public:
	/*! \remarks Converts a face selection to an element selection.\n\n
	NOTE: This method can only produce accurate results with culling if we are\n\n
	certain that the face selection corresponds to the results of the last call
	to\n\n
	the owner Mesh's SubObjectHitTest method. (To turn off culling, use GetCull
	().ClearAll(). Culling is not relevant if the MESH_SELCONV_REQUIRE_ALL flag\n\n
	is turned off.)
	\par Parameters:
	<b>Mesh \& mesh</b>\n\n
	The mesh that owns this interface\n\n
	<b>BitArray \& faceSel</b>\n\n
	The face selection representing actual hits in the viewport.\n\n
	<b>BitArray \& elementSel</b>\n\n
	This is where the output Element selection is stored.\n\n
	  */
	DllExport void FaceToElement (MNMesh & mesh, BitArray & faceSel, BitArray & elementSel);
	DllExport void EdgeToBorder (MNMesh & mesh, BitArray & edgeSel, BitArray & borderSel);
	/*! \remarks Converts a vertex selection to a "By Vertex" edge selection.\n\n
	NOTE: This method can only produce accurate results with culling if we are\n\n
	certain that the vertex selection corresponds to the results of the last
	call\n\n
	to the owner Mesh's SubObjectHitTest method. (To turn off culling, use GetCull
	().ClearAll(). Culling is not relevant if the MESH_SELCONV_REQUIRE_ALL flag\n\n
	is turned off.)\n\n

	\par Parameters:
	<b>Mesh \& mesh</b>\n\n
	The mesh that owns this interface\n\n
	<b>BitArray \& vertexSel</b>\n\n
	The face selection representing actual hits in the viewport\n\n
	<b>BitArray \& edgeSel</b>\n\n
	The face selection representing actual hits in the viewport\n\n
	  */
	DllExport void VertexToEdge (MNMesh & mesh, BitArray & vertexSel, BitArray & edgeSel);
	DllExport void VertexToFace (MNMesh & mesh, BitArray & vertexSel, BitArray & faceSel);
};

///  \brief 
/// This class is new with Max 7.0.
/// It contains new algorithms we would have put in MNMesh, but can't,
/// because we don't want to break the 6.0 SDK.
/// 
///  <br><b>Example: </b>  See example usage in the Editable Poly object, in maxsdk/samples/mesh/EditablePoly,
/// as well as in the Edit Poly modifier, in maxsdk\\samples\\mesh\\EditPoly 
class MNMeshUtilities: public MaxHeapOperators
{
private:
	MNMesh *mpMesh;

	// Private methods used by public ones:
	// (All Bridge stuff.)
	void GetBorderVertices (Tab<int> & border, Tab<int> & verts, bool reversedLoop=false);
	void GetBorderMapVertices (Tab<int> & border, Tab<int> & mapVerts, int mapChannel, bool reversedLoop=false);
	void InterpolateVertices (int v1, int v2, float f, int outputVert);
	void GetBridgeSmoothingGroups (Tab<int> & smVerts, Tab<int> & lgVerts, Tab<int> & match,
		float smoothThresh, Tab<DWORD> & smoothers);
	MtlID GetBridgeMaterial (Tab<int> & smaller, Tab<int> & larger);
	// Now PolygonToBorder() won't do any face deletion work, just to create borders.
	// Deletion will be handled in FinalizePolygonToBorder(), which is used internally
	bool PolygonToBorder (int face, int start, Tab<int> & border);
	bool OrientBorders (Tab<int> & border1, Tab<int> & border2);
	bool SynchBorders (Tab<int> & border1, Tab<int> & border2);
	bool BridgeLoops (Tab<int> & smaller, Tab<int> & larger, float smoothThresh);
	bool BridgeLoops (Tab<int> & smaller, Tab<int> & larger, float smoothThresh,
		int segments, float taper, float bias);
	void MatchLoops (Tab<Tab<int> *> & loopList, Tab<int> & loopMatch,
		Tab<int> *loopClusterID=NULL, Tab<int> *clusterMatch=NULL);

public:
	/// \brief 
	/// Constructor.  Takes MNMesh pointer, which is the mesh that is operated upon
	/// in all MNMeshUtilities methods.
	///
	///  <br><b>Example: </b>  See example usage in the Editable Poly object, in maxsdk/samples/mesh/EditablePoly 
	MNMeshUtilities (MNMesh *mesh) { mpMesh = mesh; }

	///  \brief 
	/// Given a one-sided edge in the MNMesh, retrieves the border loop which that edge forms with other
	/// one-sided edges.  The border loop produced will be in sequential order going CCW around the hole,
	/// starting with the edge given.
	/// 
	///  \param edge The edge to start from.  (If it is not a border edge, the "border" table will be
	/// left empty.
	///  \param border A reference to a table into which the border loop edge indices can be written.
	///
	///  <br><b>Example: </b>  See example usage in the Editable Poly object, in maxsdk/samples/mesh/EditablePoly
	DllExport bool GetBorderFromEdge (int edge, Tab<int> & border);

	///  \brief 
	/// Applies new smoothing groups to the MNMesh, smoothing between any faces that share an edge
	/// with an edge angle less than "threshold".
	/// 
	///  \param threshold The threshold angle for smoothing across edges, in radians.
	///  \param faceFlag If nonzero, this indicates that only faces with this flag set
	/// should have their smoothing groups affected.  (If faceFlag has multiple bits set, any flag
	/// matches are considered a match.)
	///  \remark  Similar to MNMesh::AutoSmooth, but allows the use of any flag or flags, not just MN_SEL 
	///
	///  <br><b>Example: </b>  See example usage in the Editable Poly object, in maxsdk/samples/mesh/EditablePoly 
	DllExport bool AutoSmooth (float threshold, DWORD faceFlag);

	///  \brief 
	/// Indicates whether the hinge angle should be multiplied by -1, based on the local topology and
	/// geometry of the mesh.
	/// When users pick hinge edges, they may pick edges oriented in either direction, with the result
	/// that rotating around the edge by the hinge angle can produce inconsistent results.  This method
	/// analyses the hinge orientation and indicates whether a correction should occur.
	/// 
	///  <br><b>Example: </b>  See example usage in the Editable Poly object, in maxsdk/samples/mesh/EditablePoly 
	DllExport bool FlipHingeAngle (Point3 & origin, Point3 & axis, int hingeEdge, DWORD faceFlag);

	///  \brief 
	/// New version of old "MNMesh::LiftFaceClusterFromEdge" method which doesn't require a hinge edge.
	/// Does a "hinged extrusion", where selected face clusters are rotated around a hinge axis, and new
	/// faces are created connecting the selected faces to the unselected ones.
	/// 
	///  \param origin  The origin point for the hinge 
	///  \param axis  The axis direction for the hinge 
	///  \param angle  The amount to rotate, in radians 
	///  \param segments  The number of segments for the sides of the extrusion 
	///  \param fclust  The current set of Face Clusters 
	///  \param clusterID  The ID of the cluster to rotate 
	///  \remark This extrusion has a slightly different topology from a regular extrusion, in that
	/// no new faces are created for edges between selected and unselected faces that lie exactly along
	/// the hinge.
	///
	///  <br><b>Example: </b>  See example usage in the Editable Poly object, in maxsdk/samples/mesh/EditablePoly 
	DllExport bool HingeFromEdge (Point3 & origin, Point3 & axis, float angle, int segments,
									  MNFaceClusters & fclust, int clusterID);
	/// \brief 
	/// Determines an average plane of the flagged subobjects, and moves vertices toward it
	/// according to their soft selection value.
	///
	///  <br><b>Example: </b>  See example usage in the Editable Poly object, in maxsdk/samples/mesh/EditablePoly 
	DllExport bool MakeFlaggedPlanar (int selLev, DWORD flag, float *softSel, Point3 *delta=NULL);

	/// \brief 
	/// Moves soft-selected vertices toward the plane given according to their soft selection value,
	/// or produces an array of offsets which would do so.  (Similar to MNMesh::MoveVertsToPlane,
	/// but with soft selection support.)
	///
	///  <br><b>Example: </b>  See example usage in the Editable Poly object, in maxsdk/samples/mesh/EditablePoly 
	DllExport bool MoveVertsToPlane (Point3 & norm, float offset, float *softSel, Point3 *delta=NULL);

	///  \brief 
	/// Edge-based collapse function - all edges with the given flag will be collapsed,
	/// which means their vertex endpoints will be joined into a single vertex.
	/// 
	///  <br><b>Example: </b>  See example usage in the Editable Poly object, in maxsdk/samples/mesh/EditablePoly 
	DllExport bool CollapseEdges (DWORD edgeFlag);

	///  \brief 
	/// Constrains the vectors given for each vertex so that they are limited to travel
	/// along a neighboring edge.  It's acceptable for deltaIn to be equal to deltaOut.
	/// 
	///  <br><b>Example: </b>  See example usage in the Editable Poly object, in maxsdk/samples/mesh/EditablePoly 
	DllExport void ConstrainDeltaToEdges (Tab<Point3> *deltaIn, Tab<Point3> *deltaOut);

	///  \brief 
	/// Constrains the vectors given for each vertex so that they are limited to travel
	/// along a neighboring face.  It's acceptable for deltaIn to be equal to deltaOut.
	/// 
	///  <br><b>Example: </b>  See example usage in the Editable Poly object, in maxsdk/samples/mesh/EditablePoly 
	DllExport void ConstrainDeltaToFaces (Tab<Point3> *deltaIn, Tab<Point3> *deltaOut);

	///  \brief 
	/// Extrude method which creates new faces along open edges.
	/// 
	///  \param edgeFlag  The edges which should be extruded.
	/// (Non-open edges that have this flag set always have it cleared.)
	///  \param clearFlagOnOldEdges If true, the "edgeFlag" is cleared on the edges
	/// used as the base of the extrusion.
	///  \param useTracking If true, the new edges at the "top" of the extrusion have
	/// their "track" data member set to the edge at the base of their extrusion.
	///
	///  <br><b>Example: </b>  See example usage in the Editable Poly object, in maxsdk/samples/mesh/EditablePoly 
	DllExport bool ExtrudeOpenEdges (DWORD edgeFlag, bool clearFlagOnOldEdges, bool useTracking);

	///  \brief 
	/// Creates a bridge between two border loops
	/// 
	///  \param edge1  An edge on the first border loop 
	///  \param twist1  The twist for the first border loop.  (The edge which is "twist1"
	/// edges around the loop from "edge1" will be bridged to the edge which is "twist2" edges
	/// around the loop from "edge2". 
	///  \param edge2  An edge on the second border loop 
	///  \param twist2  The twist for the second border loop.  (The edge which is "twist1"
	/// edges around the loop from "edge1" will be bridged to the edge which is "twist2" edges
	/// around the loop from "edge2". 
	///  \param smoothThresh  The threshold angle, in radians, for smoothing between successive
	/// columns of the bridge. 
	///  \param segments  The number of segments for the bridge 
	///  \param taper  The amount that the bridge should taper in
	///  \param bias  The bias for the location of the most tapered-in spot on the bridge.
	/// Range: -100 to 100, with 0 indicating that the taper should fall naturally in the middle of the 
	/// bridge.
	///
	///  <br><b>Example: </b>  See example usage in the Editable Poly object, in maxsdk/samples/mesh/EditablePoly 
	DllExport bool BridgeBorders (int edge1, int twist1, int edge2, int twist2,
		float smoothThresh, int segments, float taper, float bias);

	///  \brief 
	/// Creates a bridge between two polygons.
	/// 
	///  \param face1  The first polygon 
	///  \param twist1  The twist for the first polygon.  (The twist1'th edge of polygon face1
	/// will be bridged to the twist2'th edge on polygon face2.) 
	///  \param face2  The second polygon 
	///  \param twist2  The twist for the second polygon.  (The twist1'th edge of polygon face1
	/// will be bridged to the twist2'th edge on polygon face2.) 
	///  \param smoothThresh  The threshold angle, in radians, for smoothing between successive
	/// columns of the bridge. 
	///  \param segments  The number of segments for the bridge 
	///  \param taper  The amount that the bridge should taper in 
	///  \param bias  The bias for the location of the most tapered-in spot on the bridge.
	/// Range: -100 to 100, with 0 indicating that the taper should fall naturally in the middle of the 
	/// bridge.
	///
	///  <br><b>Example: </b>  See example usage in the Editable Poly object, in maxsdk/samples/mesh/EditablePoly 
	DllExport bool BridgePolygons (int face1, int twist1, int face2, int twist2,
		float smoothThresh, int segments, float taper, float bias);

	///  \brief 
	/// Creates bridges between polygon clusters.
	/// 
	///  \param polyFlag The flag indicating faces in the clusters.
	///  \param smoothThresh  The threshold angle, in radians, for smoothing between successive
	/// columns of the bridge. 
	///  \param segments  The number of segments for the bridge 
	///  \param taper  The amount that the bridge should taper in 
	///  \param bias  The bias for the location of the most tapered-in spot on the bridge.
	/// Range: -100 to 100, with 0 indicating that the taper should fall naturally in the middle of the 
	/// bridge.
	///  \param twist1  The twist for each "start" cluster. 
	///  \param twist2  The twist for each "end" cluster.
	///
	///  <br><b>Example: </b>  See example usage in the Editable Poly object, in maxsdk/samples/mesh/EditablePoly 
	DllExport bool BridgePolygonClusters (DWORD polyFlag, float smoothThresh, int segments, float taper, float bias,
		int twist1, int twist2);

	///  \brief 
	/// Creates bridges between flagged borders.
	/// 
	///  \param edgeFlag Any borders with at least one edge that has this flag set will be considered
	/// for bridging. (Only borders that match up in pairs will be bridged, of course.)  
	///  \param smoothThresh  The threshold angle, in radians, for smoothing between successive
	/// columns of the bridge. 
	///  \param segments  The number of segments for the bridge 
	///  \param taper  The amount that the bridge should taper in 
	///  \param bias  The bias for the location of the most tapered-in spot on the bridge.
	/// Range: -100 to 100, with 0 indicating that the taper should fall naturally in the middle of the 
	/// bridge.
	///  \param twist1  The twist for each "start" cluster. 
	///  \param twist2  The twist for each "end" cluster. 
	///
	///  <br><b>Example: </b>  See example usage in the Editable Poly object, in maxsdk/samples/mesh/EditablePoly 
	DllExport bool BridgeSelectedBorders (DWORD edgeFlag, float smoothThresh, int segments, float taper, float bias,
		int twist1, int twist2);

	///  \brief 
	/// Returns a good default twist for face2, when bridging between face1 and face2 with
	/// no twist for face1.  Default twist is based on an analysis of local geometry.
	/// 
	///  \param face1  The first polygon 
	///  \param face2  The second polygon
	///
	///  <br><b>Example: </b>  See example usage in the Editable Poly object, in maxsdk/samples/mesh/EditablePoly 
	DllExport int FindDefaultBridgeTwist (int face1, int face2);

	///  \brief 
	/// "Turns" the diagonal by removing it and creating a new one going across it.
	/// This is like the traditional "Turn Edge" in Editable Mesh.
	/// (Any "diagonal" in a polygon separates two triangles.  If you remove that diagonal,
	/// you're left with a quad; this algorithm replaces the original diagonal with the
	/// diagonal you'd get by connecting the other two vertices of that quad.)
	/// 
	///  \param face The polygon face in which to turn the diagonal
	///  \param diagonal The index of the diagonal to turn.  A polygon of degree "deg"
	/// has (deg-3) diagonals. 
	///
	///  <br><b>Example: </b>  See example usage in the Editable Poly object, in maxsdk/samples/mesh/EditablePoly 
	DllExport bool TurnDiagonal (int face, int diagonal);

	///  \brief 
	/// This is a new version of the MNMesh::Relax method, which is designed to move all vertices a little
	/// closer to their neighbors.  Neighbors are defined as vertices that share an edge.  This variation on
	/// the Relax method also accepts a soft selection.
	/// 
	///  \param vertexFlag  If "softSel" is NULL, only vertices with this flag set are relaxed. 
	///  \param softSel  This is an optional soft selection.  It may be NULL.  If not, it
	/// should point to an array of MNMesh::numv floats representing the selection value for each vertex.
	///  \param relaxAmount  This indicates the proportion of relaxing.  If it's zero, nothing happens;
	/// if it's one, each vertex is moved to the average of its neighbors.  Values of less than zero or more than one
	/// are accepted, but lead to unstable results.  The default in Editable Poly is .5f.
	///  \param relaxIters  This is the number of iterations of relax.  This is the number of times the
	/// relaxation is applied.  The more iterations, the more relaxed the result. 
	///  \param holdBoundaryPts  If true, "boundary" points should be held in place, unaffected by the Relax.
	/// "boundary" points are defined as those that are used by more edges than faces, which basically means
	/// they're on some sort of border of the mesh. This is turned on by default in Editable Poly, because
	/// if it's off, boundary points recede into the mesh very quickly.
	///  \param holdOuterPts  If true, "outer" points should be held in place.  "outer" points are defined
	/// as those where the sum of all the face angles is less than 2Pi (360 degrees).  This basically means the
	/// point is some sort of outer corner.  All the points on a 1-segment Box or on a Geosphere are "outer" points.
	/// On a multiple-segment plane, none of the interior points are "outer" points.  This parameter is most
	/// important in models like the Geosphere, which could relax down into nothing if outer points are not
	/// fixed in place.
	///  \param delta  If non-NULL, this should point to an array into which the effect of this relax
	/// should be placed, in the form of a Point3 offset per vertex. 
	///  \return  True if any relaxing occurred, false in cases where nothing happened, for instance because
	/// no vertices were selected.
	///
	///  <br><b>Example: </b>  See example usage in the Editable Poly object, in maxsdk/samples/mesh/EditablePoly 
	DllExport bool Relax (DWORD vertexFlag, float *softSel, float relaxAmount,
		int relaxIters, bool holdBoundaryPts, bool holdOuterPts, Point3 *delta=NULL);

	///  \brief 
	/// Starting with the polygon given, this algorithm creates a BitArray selection of all polygons it can get to
	/// without crossing any edges whose angle is greater than the angle given.
	/// 
	///  \param startPolygon The polygon to start on.  If this polygon is already set in
	/// "polySel", nothing will happen.
	///  \param angle The maximum crossable edge angle, in radians
	///  \param polySel The BitArray where polygons should be selected 
	///  \return The number of newly-selected polygons
	///  \remark This method can be called multiple times with the same polySel parameter, for instance
	/// to turn a set of face hits from hit-testing into a select-by-angle region, and is
	/// designed to be used in Editable Poly and Edit Poly's "Select by angle" feature
	///
	///  <br><b>Example: </b>  See example usage in the Editable Poly object, in maxsdk/samples/mesh/EditablePoly 
	DllExport int SelectPolygonsByAngle (int startPolygon, float angle, BitArray & polySel);

	///  \brief 
	/// This method is used at the beginning of a series of cuts.  It sets things up so that "CutCleanup"
	/// will work.
	/// 
	///  <br><b>Example: </b> See example usage in the Editable Poly object, in maxsdk/samples/mesh/EditablePoly
	DllExport void CutPrepare ();

	///  \brief 
	/// This method is used after a series of cuts, to "clean up" any extra edges which are necessary to have
	/// while cutting, but which aren't what the user actually intended to create.  "CutPrepare" should be called
	/// at the beginning of the series of cuts.
	/// 
	///  <br><b>Example: </b>  See example usage in the Editable Poly object, in maxsdk/samples/mesh/EditablePoly 
	DllExport void CutCleanup ();
};

///  \brief 
/// This class is used to "preserve" mapping coordinates while moving vertices.
/// The "Preserve" name is actually a little confusing; what this class helps us do is modify
/// map vertices such that the overall map on a surface looks about the same, even though the
/// regular vertices have moved.  For instance, if you start with a Plane object, and move the
/// middle vertex to the right, the map is typically distorted even though all the original
/// map vertices are unaffected.  This class will help by adjusting the middle map vertex so
/// that the overall regular texture pattern is preserved.
/// 
///  <br><b>Example: </b>  See example usage in the Editable Poly object, in maxsdk/samples/mesh/EditablePoly
class MNMapPreserveData: public MaxHeapOperators
{
private:
	Tab<Point3> mFcx, mFcy;	// face corner x and y directions.
	Tab<Point3 *> mMapXDir, mMapYDir;
	int mDegreeSum;
	bool mProtectVertexColors;

public:
	///  \brief 
	/// Constructor.  Initializes to empty data.
	/// 
	MNMapPreserveData () : mDegreeSum(0), mProtectVertexColors(true) { }

	///  \brief 
	/// Destructor.  Frees allocated data.
	/// 
	~MNMapPreserveData () { Clear(); }

	///  \brief 
	/// Clears out all allocated data, and resets to empty.
	/// 
	DllExport void Clear ();

	///  \brief 
	/// Creates caches of data based on the mesh given and the set of maps to be preserved.  Sets the
	/// "initial state" which the maps should try to adhere to.
	/// 
	///  \param mm  The mesh whose maps should be preserved 
	///  \param mapsToDo  A MapBitArray indicating which map channels should be preserved.
	/// See class MapBitArray for more details. 
	DllExport void Initialize (MNMesh & mm, const MapBitArray & mapsToDo);

	///  \brief 
	/// Given a mesh and a set of offsets for vertices, manipulate map channels to preserve the initial UV mapping.
	/// 
	///  \param mm  The mesh being affected.  Note that this algorithm uses the face information in the mesh,
	/// and affects the maps in the mesh, but it doesn't use the vertex information at all.  (All relevant vertex
	/// information was digested in Initialize.)  So this method may be called before or after applying the offsets
	/// given in pDelta. 
	///  \param pDelta  The set of offsets for the vertices. 
	///  \param numDeltas  The length of the pDelta array 
	DllExport void Apply (MNMesh & mm, Point3 *pDelta, int numDeltas);
};


// New interface id for mnmeshutilities8 (max 8 bridge edge methods , lr oct 2004 ) 
#define IMNMESHUTILITIES8_INTERFACE_ID Interface_ID(0xf5235e73, 0x3b304334)



//! \brief IMNMeshUtilities8 extends the MNMeshUtilities class, adding new functionality  applied on a MNMesh.
/*! \n Such an interface is obtained through the call to GetInterface, using the class id: IMNMESHUTILITIES8_INTERFACE_ID 
    \n on an instance of an MNMesh. 
    \n usage is : IMNMeshUtilities8* l_meshToBridge = static_cast<IMNMeshUtilities8*>(mesh.GetInterface( IMNMESHUTILITIES8_INTERFACE_ID ));
    \n 
	\b Edge \b bridging: This is an extension of the existing bridge border method. It bridges 2 sets od edges of different size.
    you can bridge 4 edges with 5 edges. 
	\n Limitations. the 2 edge lists must be disjoint, or at least separated by a sharp angle.
	\n this angular threshold can be adjusted by using the in_adjacentAngle parameter,
	\n \b Connect \b edges: 2 new parameters are added for better precision: in_pinch and in_slide.
	\n \b SelectEdgeRingShift: selects edges in the ring direction 
*/

class IMNMeshUtilities8 : public BaseInterface
{
	public:
		virtual Interface_ID	GetID() { return IMNMESHUTILITIES8_INTERFACE_ID; }

		/// 
		/// bridges 2 edges 
		/// \return true if the bridging operation was successful 
		/// \return false if the bridging operation failed
		/// \pre in_sourceEdge and in_targetEdge must be different 
		/// \pre in_sourceEdge and in_targetEdge must on border
		/// \post the mesh geometry is valid, even if the operation failed
		/// \param[in] in_Edge1 1st edge index to bridge
		/// \param[in] in_Edge2 2nd edge index to bridge
		/// \param[in] in_segmentsNumber number of polys between the edges
		/// \param[in] in_prevColumnIndex internal value, do not use 
		/// \param[in] in_smoothingGroup new face smoothing Group index 
		/// \param[in] in_materialId new face material id 
		/// \param[in] in_triangleAdded internal value, do not use
		/// 

		virtual bool BridgeTwoEdges(	
			const int 	in_Edge1,						
			const int 	in_Edge2,						
			const int	in_segmentsNumber,				
			const int 	in_prevColumnIndex = 0,			
			const DWORD in_smoothingGroup = 0,			
			const MtlID in_materialId = 0,				
			const bool  in_triangleAdded = false) = 0 ;	

		//
		/// Bridges edge selection
		/// \return true if the bridging operation was successful 
		/// \return false if the bridging operation failed
		/// \pre in_sourceEdge and in_targetEdge must be different 
		/// \pre in_sourceEdge and in_targetEdge must on border
		/// \post the mesh geometry is valid, even if the operation failed
		/// \param[in] in_edgeFlag edge cluster flag. 
		/// \param[in] in_smoothThresh threshold angle, in radians, for smoothing successive columns of the bridge. 
		/// \param[in] in_segmentsNumber number of polys between the edges
		/// \param[in] in_adjacentAngle angular threshold in radians for splitting one contiguous edge list in 2
		///	in order to perform the bridging on 2 separate edge lists
		/// \param[in] in_reverseTriangle reverse triangle's position: if true, triangles will be added at the 
		/// end of the bridge, if they are needed ( when bridging 4 edges with 5 edges. 



		virtual bool BridgeSelectedEdges (	
			const DWORD in_edgeFlag,				
			const float	in_smoothThresh,			
			const int	in_segmentsNumber,						
			const float	in_adjacentAngle,			
			const bool	in_reverseTriangle ) = 0;	

		//
		/// New Edge Connection: 2 new parameters are added, pinch and slide, for better precision
		/// \return true if the connection operation was successful 
		/// \return false if the connection operation failed
		/// \pre in_segments must be greater than 1 
		/// \pre in_pinch and in_slide must be between -100 and 100 
		/// \post the mesh geometry is valid, even if the operation failed
		/// \param[in] in_edgeFlag edge cluster flag
		/// \param[in] in_segments number of connection edges between successive selected edges
		/// \param[in] in_pinch distance between successive new connection edges. Can be < 0. 
		/// \param[in] in_slide position of the new connection edges, relative to the existing edges. Can be < 0. 

		virtual bool ConnectEdges (
			const DWORD in_edgeFlag,				
			const int	in_segments = 2,			
			const int	in_pinch = 0,				
			const int	in_slide = 0 ) = 0 ;		

		/// edge selection shifting routine: shifts the current edge selection in the ring direction
		/// \pre the io_edgeSel BitArray, must exist with the proper size 
		/// \post the io_edgeSel BitArray contains the list of the shifted edges 
		/// \param[in] in_ring_shift shifting value, can be positive or negative  
		/// \param[in,out] io_edgeSel the new edge selection list

		virtual void SelectEdgeRingShift( 
			const int	in_ring_shift,				
			BitArray&	io_edgeSel) = 0;			

		/// edge selection shifting routine: shifts the current edge selection in the loop direction
		/// \pre the io_edgeSel BitArray must exist with the proper size 
		/// \post the io_edgeSel BitArray contains the list of the shifted edges 
		/// \param[in] in_loop_shift shifting value, can be positive or negative  
		/// \param[in,out] io_edgeSel the new edge selection list

		virtual void SelectEdgeLoopShift( 
			const int	in_loop_shift,				
			BitArray&	io_edgeSel)=0;				
	
		/// vertex splitting method, using a distance value for specifying the distance 
		/// between the split vertices. 
		/// \post the  MNmesh is valid, with the selected vertices ,split apart, using the in_distance. 
		/// \param[in] in_vertexFlag the vertex flag used
		/// \param[in] in_distance to split the vertices apart

		virtual bool SplitFlaggedVertices (
			const DWORD in_vertexFlag	= MN_SEL,		
			const float in_distance		= 0.0 ) =0;		

		/// edge splitting method, using a distance value for specifying the distance 
		/// between the split edges. 
		/// \post the  MNmesh is valid with the selected vertices ,split apart, using the in_distance. 
		/// \param[in] in_edgeFlag the vertex flag used
		/// \param[in] in_distance to split the selected edges apart

		virtual bool SplitFlaggedEdges (
			const DWORD in_edgeFlag = MN_SEL,		
			const float in_distance = 0.0 )= 0;		

		/// Creates a bitArray indicating the directions of the MNMesh's edges relative to 2 arbitrary edges: 
		/// the 2 first edges of the first face of the MNMesh. For a MNesh made of quads ( like a converted plane),
		/// they are the 2 first edges of the lower left face. 
		/// \pre the array io_edgeFlip does not need to exist
		/// \post the array exists and contains number of edges entries.  
		/// \param[in,out] io_edgeFlip array of flipped edges
		/// \param[in] in_useOnlySelectedEdges if this is true, only selected edges will be tested against the 2 original edges.  

		virtual void GetFlippedEdges(  
			BitArray& io_edgeFlip,	
			const bool in_useOnlySelectedEdges = false) = 0;
		//
		/// New Edge chamfer method: A new parameter is added, allowing the creation of holes when chamfering.
		/// \return true if the chamfer operation was successful 
		/// \return false if the chamfer operation failed
		/// \post the mesh geometry is valid, even if the operation failed
		/// \param[in] in_flag the edge flag used 
		/// \param[in] in_mcd the chamfer data
		/// \param[in] in_open If true, the newly created faces are removed, creating a hole between chamfered edges. 

		virtual bool ChamferEdges ( 
			const DWORD			in_flag = MN_SEL,		
			      MNChamferData *in_mcd	= NULL,			
			const bool			in_open = false) = 0;	
		
		//
		/// New Vertex chamfer method  : A new parameter is added, allowing the creation of holes when chamfering.
		/// \return true if the chamfer operation was successful 
		/// \return false if the chamfer operation failed
		/// \post the mesh geometry is valid, even if the operation failed
		/// \param[in] flag the vertices flag used 
		/// \param[in] mcd the chamfer data
		/// \param[in] in_open If true, the newly created faces are removed, creating a hole between chamfered vertices. 

		virtual bool ChamferVertices (
			const DWORD			flag	= MN_SEL,		
			      MNChamferData *mcd	= NULL,			
			const bool			in_open = false) = 0 ;	

		// MNMesh optimization
		//! \brief Check if a face's cache is invalid
		/*!	The function first checks if the whole mesh or the face have been flagged as
			cache invalid (MN_CACHEINVALID). If none of these is true, it then cycles through the face's
			vertices and returns true as soon as it finds a vertex flagged with MN_CACHEINVALID.
			\pre faceIndex must be a valid index in the faces (f) array.
			\param[in] faceIndex The index of the face for which you want to determine cache validity
			\return true if the face is invalid, else false.
		*/
		virtual bool isFaceCacheInvalid(int faceIndex) = 0;

		//! \brief Invalidates a single vertex's cache indicating it needs to be rebuilt.
		/*!	The function flags the specified vertex with the MN_CACHEINVALID flag, telling the rebuild
			functions that this vertex, the faces surrounding it and the other vertices of those faces
			need to have their normals recomputed. It also sets normalsBuilt to 0 to trigger normals' rebuild.
			This function should be called when one to several vertices have been modified. If all or almost
			all vertices of the mesh have been invalidated, it's more efficient to call InvalidateGeomCache() instead.
			\pre vertex must be a valid index in the vertex (v) array.
			\param[in] vertex The index of the vertex that needs to be invalidated.
		*/
		virtual void InvalidateVertexCache (int vertex) = 0;

};

#define IMNMESHUTILITIES10_INTERFACE_ID Interface_ID(0x7b1acb31, 0x763982d5)

//! \brief IMNMeshUtilities10 extends the MNMeshUtilities class, adding new functionality applied on a MNMesh.
/*! 
Usage: 

\code
IMNMeshUtilities10* l_mesh10 = static_cast<IMNMeshUtilities10*>(mesh.GetInterface( IMNMESHUTILITIES10_INTERFACE_ID ));
\endcode

See IMNMeshUtilities8 for details on using this interface.
*/
class IMNMeshUtilities10 : public BaseInterface {
public:
		Interface_ID	GetID() { return IMNMESHUTILITIES10_INTERFACE_ID; }

		//! Chamfers mesh edges like MNMesh::ChamferEdges. 
		//! \return true if the chamfer operation was successful 
		//! \return false if the chamfer operation failed
		//! \post the mesh geometry is valid, even if the operation failed
		//! \param[in] in_flag the edge flag used. See MNEdge for a description of flag values.
		//! \param[in] in_mcd the chamfer data. 
		//! \param[in] in_open If true, the newly created faces are removed, creating a hole between chamfered edges. 
		//! \param[in] in_segments The number of new edges to create per selected edge. If in_segments < 1, then behave as though in_segments = 1.
		virtual bool ChamferEdges ( 
			const DWORD			  in_flag,		
			      MNChamferData10 &in_mcd,			
			const bool			  in_open,
			const int			  in_segments) = 0;	
	
		//! Constrains the positional offsets given for each vertex so that they are limited to travel along each corresponding vertex normal. 
		//! \param[in] deltaIn  A tab of positional changes for each vertex in the mesh. If the count of deltaIn is different than the number
		//! of vertices in the mesh, the function will return false. 
		//! \param[out] deltaOut A tab of the resulting positional changes for each vertex in the mesh after the incoming changes in deltaIn
		//! are constrained by the vertex normal.  The deltaOut parameter maybe equal to deltaIn parameter.  The count of the deltaOut parameter
		//! will get modified so that it's the same as number of vertices in the mesh.
		//! \return Returns true if the function succeeds false if it doesn't for any reason.
		virtual bool ConstrainDeltaToNormals (Tab<Point3> *deltaIn, Tab<Point3> *deltaOut) = 0;

};

#endif
