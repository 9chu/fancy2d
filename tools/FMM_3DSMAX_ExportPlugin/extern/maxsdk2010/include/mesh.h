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
// FILE:        mesh.h
// DESCRIPTION: Main include file for triangle meshes.
// AUTHOR:      Don Brittain
// HISTORY:     
//**************************************************************************/

#ifndef _MESH_H_

#define _MESH_H_

#include "maxheap.h"
#include "channels.h"
#include "snap.h"
#include <ioapi.h>
#include "export.h"
#include "vedge.h"  //DS
#include "utillib.h"
#include "tab.h"
#include "baseinterface.h"
#include "channels.h"

#include "assert.h"

// These typedefs must be the same as each other, since
// vertex colors are contained in a MeshMap.
typedef Point3 UVVert;
typedef Point3 VertColor;

#define MESH_USE_EXT_CVARRAY	(-32767)

#define MESH_MULTI_PROCESSING	TRUE		// TRUE turns on mp vertex transformation

#define MESH_CAGE_BACKFACE_CULLING	// for "cage" orange gizmo meshes in EMesh, EPoly, Mesh Select, etc.

class ISave;
class ILoad;
class IHardwareShader;
class TriStrip;
class MeshNormalSpec;

#define NEWMESH

/*! \sa  Class Mesh, Class RVertex, Class GraphicsWindow.\n\n
\par Description:
The 'rendered' normal class. <b>RNormals</b> are stored as unit vectors in
object space. An instance of this class is maintained by the <b>RVertex</b>
class since the <b>RVertex</b> is what gets rendered (lit), and the lighting
methods need to know about the normals. All methods of this class are
implemented by the system.\n\n
<b>Note:</b>This class is used internally by 3ds Max. Developers who need to
compute face and vertex normals for a mesh should instead refer to the Advanced
Topics section
<a href="ms-its:3dsmaxsdk.chm::/meshes_face_and_vertex_normals.html">Computing
Face and Vertex Normals</a>.
\par Data Members:
private:\n\n
<b>Point3 normal;</b>\n\n
The normal as a unit vector. Note that if you set this normal, you should call
the method <b>normalize()</b> if you are not sure the normal is already unit
length.\n\n
<b>DWORD smGroup;</b>\n\n
The smoothing group. Normals are attached to smoothing groups in the sense that
one vertex, if it is shared by two triangles with different smoothing groups,
will have two normals associated with it -- one for each smoothing group.\n\n
<b>MtlID mtlIndex;</b>\n\n
The material index. If one vertex is shared between two materials, there will
be two normals. Therefore the material index is stored with the normal as
well.\n\n
Note: <b>typedef unsigned short MtlID;</b>\n\n
<b>Point3 rgb;</b>\n\n
The RGB value.  */
class RNormal: public MaxHeapOperators {
	public:
		/*! \remarks Constructor. The smoothing group and material index are
		set to zero. */
		RNormal()	{ smGroup = mtlIndex = 0; }
		/*! \remarks Sets the normal to the specified value.
		\par Parameters:
		<b>const Point3 \&nor</b>\n\n
		The normal value to set. */
		void		setNormal(const Point3 &nor) { normal = nor; }
		/*! \remarks Adds the specified value to the value of the normal.
		\par Parameters:
		<b>const Point3 \&nor</b>\n\n
		The normal value to add to the existing value. */
		void		addNormal(const Point3 &nor) { normal += nor; }	
		/*! \remarks Converts the normal to a unit vector. */
		void		normalize(void) 	{ normal = Normalize(normal); }
		/*! \remarks Returns the normal. */
		Point3 &	getNormal(void) 	{ return normal; }
      inline const Point3& getNormal() const { return normal; }
		/*! \remarks Sets the smoothing group to the specified value.
		\par Parameters:
		<b>DWORD g</b>\n\n
		The smoothing group value to set. */
		void		setSmGroup(DWORD g)	{ smGroup = g; }
		/*! \remarks ORs the specified smoothing group value to the existing
		value.
		\par Parameters:
		<b>DWORD g</b>\n\n
		The smoothing group bits to set. */
		void		addSmGroup(DWORD g) { smGroup |= g; }
		/*! \remarks Returns the smoothing group value. */
		DWORD		getSmGroup(void)	{ return smGroup; }
		/*! \remarks Sets the material index to the specified value.
		\par Parameters:
		<b>MtlID i</b>\n\n
		The material index value to set. */
		void		setMtlIndex(MtlID i){ mtlIndex = i; }
		/*! \remarks Returns the material index. */
		MtlID		getMtlIndex(void)	{ return mtlIndex; }
		/*! \remarks Sets the RGB value.
		\par Parameters:
		<b>Point3 \&clr</b>\n\n
		The RGB color to set. */
		void		setRGB(Point3 &clr)	{ rgb = clr; };
		/*! \remarks Returns the RGB value. */
		Point3 &	getRGB(void)		{ return rgb; }
      inline const Point3& getRGB() const { return rgb; }
		
	private:	
		Point3		normal;	   
		DWORD		smGroup;    
		MtlID		mtlIndex;   
		Point3		rgb;	   
	};					   

// RVertex flags: contain clip flags, number of normals at the vertex
// and the number of normals that have already been rendered.
// fine PLANE_MASK	0x00003f00UL -- now in gfx.h
#define NORCT_MASK			0x000000ffUL
#define SPECIFIED_NORMAL	0x00004000UL
#define OUT_LEFT			0x00010000UL
#define OUT_RIGHT			0x00020000UL
#define OUT_TOP				0x00040000UL
#define OUT_BOTTOM			0x00080000UL
#define RECT_MASK			0x000f0000UL
#define RND_MASK			0xfff00000UL
#define RND_NOR0			0x00100000UL
#define RND_NOR(n)  		(RND_NOR0 << (n))

/*! \sa  Class RNormal, Class Mesh, Class GraphicsWindow.\n\n
\par Description:
A <b>RVertex</b> is a rendered vertex. A vertex becomes a <b>RVertex</b> after
it has been transformed. A <b>RVertex</b> has a position (x, y, z coordinate)
that is stored in device coordinates. These are stored in the data member
<b>fPos[3]</b>.\n\n
One vertex in a <b>Mesh</b> can be shared between many different smoothing
groups. In the 3ds Max <b>Mesh</b> database, the vertices are shared, however
the normals are not. This is why an <b>RVertex</b> has a <b>RNormal</b> data
member. For example, if you had a sphere that had the top and bottom
hemi-spheres smoothed separately (i.e. not smoothed across the equator), then
the vertices across the equator would have two <b>RNormals</b> for each
<b>RVertex</b> while the other vertices would have one. There may be as many
<b>RNormals</b> as there are smoothing groups colliding at a vertex. However,
it is by far the most common case to have one, and anything other than one or
two is very rare.\n\n
For purposes of smoothing, as many <b>RNormals</b> are allocated as required
and are stored in this class. <b>RNormals</b> are kept in this <b>RVertex</b>
class since the <b>RVertex</b> is what gets rendered (lit). When you light a
vertex you need to know its normal direction. Thus the <b>RNormal</b>(s) are
stored in this class (using data member <b>rn</b> or <b>*ern</b>).\n\n
All methods of this class are implemented by the system.\n\n
<b>Note:</b>This class is used internally by 3ds Max. Developers who need to
compute face and vertex normals for a mesh should instead refer to the Advanced
Topics section
<a href="ms-its:3dsmaxsdk.chm::/meshes_face_and_vertex_normals.html">Computing
Face and Vertex Normals</a>.
\par Data Members:
<b>DWORD rFlags;</b>\n\n
The flags contain the clip flags, the number of normals at the vertex, and the
number of normals that have already been rendered. These are used internally.
For example, the clipping flags are used to see if the RVertex can be either
trivially accepted or rejected when rendering.\n\n
<b>#define NORCT_MASK   0x000000ffUL</b>\n\n
<b>#define SPECIFIED_NORMAL 0x00004000UL</b>\n\n
<b>#define OUT_LEFT    0x00010000UL</b>\n\n
<b>#define OUT_RIGHT    0x00020000UL</b>\n\n
<b>#define OUT_TOP    0x00040000UL</b>\n\n
<b>#define OUT_BOTTOM   0x00080000UL</b>\n\n
<b>#define RECT_MASK    0x000f0000UL</b>\n\n
<b>#define RND_MASK    0xfff00000UL</b>\n\n
<b>#define RND_NOR0    0x00100000UL</b>\n\n
<b>#define RND_NOR(n)   (RND_NOR0 \<\< (n))</b>\n\n
<b>union {</b>\n\n
<b> int iPos[3];</b>\n\n
// This position is no longer used.\n\n
<b> float fPos[3];</b>\n\n
// This is used to store the position in device coordinates, [0]=x, [1]=y,
[2]=z.\n\n
<b> };</b>\n\n
<b>RNormal rn;</b>\n\n
If a single RNormal is used, it is stored here.\n\n
<b>RNormal *ern;</b>\n\n
In some cases, there may be two or more RNormals per vertex. If this is the
case, these 'extra' RNormals are allocated and the pointer to the memory is
stored here. If these are used, then data member <b>rn</b> is not used
(<b>rn</b> is copied into <b>ern[0]</b>).  */
class RVertex: public MaxHeapOperators {
	public:
      enum { 
         MAX_NUM_NORMALS  = 0xff
      };

   public:
		/*! \remarks Constructor. The flags are set to zero and the <b>ern</b>
		pointer is set to NULL. */
		RVertex()	{ rFlags = 0; ern = NULL; }
		/*! \remarks Destructor. */
		DllExport ~RVertex();	

		DWORD		rFlags;     
		int			pos[3];	
		RNormal		rn;		   
		RNormal 	*ern;		 
	};					  

/*! \sa  Class Mesh.\n\n
\par Description:
This class represents a single triangular face. The class maintains three
indices into the vertex list for the face, a 32-bit smoothing group for the
face, and 32-bits of face flags. The flags also store information about the
visibility of the face, the visibility of the three edges, and whether or not
the face has texture vertices present. The most significant 16-bits of the face
flags store the material index. All methods of this class are implemented by
the system.
\par Data Members:
<b>DWORD v[3];</b>\n\n
These are 0 based indices into a mesh object's array of vertices.\n\n
<b>DWORD smGroup;</b>\n\n
Smoothing group bits for the face.\n\n
Each bit of this 32 bit value represents membership in a smoothing group. The
least significant bit represents smoothing group #1 while the most significant
bit represents group #32. If two adjacent faces are assigned the same smoothing
group bit, the edge between them is rendered smoothly.\n\n
<b>DWORD flags;</b>\n\n
The Face Flags:\n\n
Edge visibility bits: If the bit is 1, the edge is visible.\n\n
<b>EDGE_A</b>\n\n
<b>EDGE_B</b>\n\n
<b>EDGE_C</b>\n\n
<b>EDGE_ALL (EDGE_A|EDGE_B|EDGE_C)</b>\n\n
Face visibility bit: If the bit is 1, the face is hidden.\n\n
<b>FACE_HIDDEN</b>\n\n
Texture vertices bit: If the bit is 1, texture vertices are present. <b>FROM R5
THIS IS NOW OBSOLETE. YOU CAN USE MAPSUPPORT METHODS INSTEAD</b>\n\n
<b>HAS_TVERTS</b>\n\n
The material ID is stored in the HIWORD of the face flags.\n\n
<b>FACE_MATID_SHIFT</b>\n\n
This is the number of bits to shift the flags to access the material.\n\n
<b>FACE_MATID_MASK</b>\n\n
This is a mask used to access the material ID.  */
class Face: public MaxHeapOperators {	
	public:
		DWORD	v[3];
		DWORD	smGroup;
		DWORD	flags;

		/*! \remarks Constructor. The smoothing groups and face flags are
		initialized to zero. */
		Face()	{ smGroup = flags = 0; }
		/*! \remarks Retrieves the zero based material ID for this face. Note:
		<b>typedef unsigned short MtlID;</b> */
		MtlID	getMatID() {return (int)((flags>>FACE_MATID_SHIFT)&FACE_MATID_MASK);}
		/*! \remarks Sets the material ID for this face.
		\par Parameters:
		<b>MtlID id</b>\n\n
		Specifies the zero based material index. */
		void    setMatID(MtlID id) {flags &= 0xFFFF; flags |= (DWORD)(id<<FACE_MATID_SHIFT);}
		/*! \remarks Sets the smoothing group bits for this face.
		\par Parameters:
		<b>DWORD i</b>\n\n
		Specifies the smoothing group bits for this face. */
		void	setSmGroup(DWORD i) { smGroup = i; }
		/*! \remarks Returns the smoothing group bits for this face. */
		DWORD	getSmGroup(void)	{ return smGroup; }
		/*! \remarks Sets the vertices of this face.
		\par Parameters:
		<b>DWORD *vrt</b>\n\n
		An array of the 3 vertices to store. These are zero based indices into
		the mesh object's array of vertices. */
		DllExport void	setVerts(DWORD *vrt);
		/*! \remarks Sets the vertices of this face. The specified indexes are
		zero based indices into the mesh object's array of vertices.
		\par Parameters:
		<b>int a</b>\n\n
		Specifies the first vertex.\n\n
		<b>int b</b>\n\n
		Specifies the second vertex.\n\n
		<b>int c</b>\n\n
		Specifies the third vertex. */
		void	setVerts(int a, int b, int c)  { v[0]=a; v[1]=b; v[2]=c; }
		/*! \remarks Sets the visibility of the specified edge.
		\par Parameters:
		<b>int edge</b>\n\n
		Specifies the edge to set the visibility of. You may use 0, 1, or
		2.\n\n
		<b>int visFlag</b>\n\n
		One of the following values:\n\n
		<b>EDGE_VIS</b>\n\n
		Sets the edge as visible.\n\n
		<b>EDGE_INVIS</b>\n\n
		Sets the edge as invisible. */
		DllExport void	setEdgeVis(int edge, int visFlag);
		/*! \remarks Sets the visibility of the all the edges.
		\par Parameters:
		<b>int va</b>\n\n
		Specifies the visibility for edge 0. Use either <b>EDGE_VIS</b> or
		<b>EDGE_INVIS</b>.\n\n
		<b>int vb</b>\n\n
		Specifies the visibility for edge 1. Use either <b>EDGE_VIS</b> or
		<b>EDGE_INVIS</b>.\n\n
		<b>int vc</b>\n\n
		Specifies the visibility for edge 2. Use either <b>EDGE_VIS</b> or
		<b>EDGE_INVIS</b>. */
		DllExport void    setEdgeVisFlags(int va, int vb, int vc); 
		/*! \remarks Retrieves the edge visibility for the specified edge.
		\par Parameters:
		<b>int edge</b>\n\n
		Specifies the edge.
		\return  Nonzero if the edge is visible, zero if the edge is invisible.
		*/
		int		getEdgeVis(int edge){ return flags & (VIS_BIT << edge); }
		/*! \remarks Returns the index into the mesh vertex array of the
		specified vertex.
		\par Parameters:
		<b>int index</b>\n\n
		Specifies the vertex to retrieve. You may use 0, 1 or 2. */
		DWORD	getVert(int index)	{ return v[index]; }
		/*! \remarks Retrieves a pointer to the vertex array.
		\return  A pointer to the vertex array. */
		DWORD *	getAllVerts(void)	{ return v; }
		/*! \remarks Determines if the face is hidden or visible.
		\return  TRUE if the face is hidden; otherwise FALSE. */
		BOOL	Hidden() {return flags&FACE_HIDDEN?TRUE:FALSE;}
		/*! \remarks Hides this face (makes it invisible in the viewports). */
		void	Hide() {flags|=FACE_HIDDEN;}
		/*! \remarks Shows this face (makes it visible in the viewports). */
		void	Show() {flags&=~FACE_HIDDEN;}
		/*! \remarks Sets the hidden state of this face.
		\par Parameters:
		<b>BOOL hide</b>\n\n
		Specifies the hidden state for the face. Pass TRUE to hide the face;
		FALSE to show it. */
		void	SetHide(BOOL hide) {if (hide) Hide(); else Show();}

		BOOL	InForeground() {return flags&FACE_INFOREGROUND?TRUE:FALSE;}
		void	PushToForeground() {flags|=FACE_INFOREGROUND;}
		void	PushToBackground() {flags&=~FACE_INFOREGROUND;}
		void	SetInForegound(BOOL foreground) {if (foreground) PushToForeground(); else PushToBackground();}

		BOOL	IsBackFacing() {return flags&FACE_BACKFACING?TRUE:FALSE;}
		void	SetBackFacing() {flags|=FACE_BACKFACING;}
		void	SetFrontFacing() {flags&=~FACE_BACKFACING;}
		void	SetBackFacing(BOOL backFacing) {if (backFacing) SetBackFacing(); else SetFrontFacing();}



		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Returns the first vertex in the face that isn't v0 or v1.
		\par Parameters:
		<b>DWORD v0</b>\n\n
		The zero based index of one of the vertices to check.\n\n
		<b>DWORD v1</b>\n\n
		The zero based index of the other vertex to check.
		\return  The zero based index of the vertex found in the Mesh's vertex
		list. */
		DllExport DWORD GetOtherIndex (DWORD v0, DWORD v1);
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Returns the index of the edge in the face that goes from v0 to v1, or
		v1 to v0.
		\par Parameters:
		<b>DWORD v0</b>\n\n
		The zero based index of the vertex at one end of the edge.\n\n
		<b>DWORD v1</b>\n\n
		The zero based index of the vertex at the other end of the edge.
		\return  The zero based index of the edge found in the Fesh's edge
		list.\n\n
		  */
		DllExport DWORD GetEdgeIndex (DWORD v0, DWORD v1);
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Indicates order in which vertices v0 and v1 appear in the face.
		\par Parameters:
		<b>DWORD v0</b>\n\n
		One vertex on this face.\n\n
		<b>DWORD v1</b>\n\n
		Another vertex on this face.
		\return  <b>1</b> if v1 follows v0 in sequence (This includes e.g. when
		Face::v[2] == v0 and Face::v[0] == v1.)\n\n
		<b>-1</b> if v0 follows v1 in sequence\n\n
		<b>0</b> if v0 or v1 are not on the face. */
		DllExport int Direction (DWORD v0, DWORD v1);
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Returns the index of the specified vertex in this face's vertex list
		(0, 1 or 2). If not found 3 is returned.
		\par Parameters:
		<b>DWORD v0</b>\n\n
		The zero based index of the vertex to check. */
		DllExport DWORD GetVertIndex (DWORD v0);
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		This method switches v0,v1 if needed to put them in face-order. If v0
		and v1 are in the order in which they appear in the face, or if one or
		both of them are not actually on the face, nothing happens. If however
		v0 follows v1, the values of the parameters are switched, so that they
		are then in the correct order for this face.
		\par Parameters:
		<b>DWORD \&v0</b>\n\n
		One vertex on this face.\n\n
		<b>DWORD \&v1</b>\n\n
		Another vertex on this face.
		\par Sample Code:
		\code
		Face & f = mesh.faces[edge.f[0]];
		DWORD v0 = edge.v[0];
		DWORD v1 = edge.v[1];
		// Switch v0, v1 if needed to match orientation in selected face.
		f.OrderVerts(v0,v1);
		\endcode */
		DllExport void OrderVerts (DWORD & v0, DWORD & v1);	// switches v0,v1 if needed to put them in face-order.
	};


// This is used both for UVWs and color verts
/*! \sa  Class Mesh.\n\n
\par Description:
This class is used for texture faces as well as vertex colors. The class
maintains an array of three indices into the object's <b>tVerts</b> array. See
the Mesh class for details on how its array of TVFaces and tVerts relate. All
methods of this class are implemented by the system.
\par Data Members:
<b>DWORD t[3];</b>\n\n
These are indices into the mesh object's <b>tVerts</b> array.  */
class TVFace: public MaxHeapOperators {
public:
	DWORD	t[3];  // indices into tVerts

	/*! \remarks Constructor. No initialization is done. */
	TVFace() {}
	/*! \remarks Constructor.
	\par Parameters:
	<b>DWORD a</b>\n\n
	Specifies the index into the <b>tVerts</b> array for vertex 0.\n\n
	<b>DWORD b</b>\n\n
	Specifies the index into the <b>tVerts</b> array for vertex 1.\n\n
	<b>DWORD c</b>\n\n
	Specifies the index into the <b>tVerts</b> array for vertex 2. */
	TVFace(DWORD a, DWORD b, DWORD c) {t[0]=a; t[1]=b; t[2]=c;}
	/*! \remarks Sets the texture vertices.
	\par Parameters:
	<b>DWORD *vrt</b>\n\n
	An array of indices into the <b>tVerts</b> array for vertices 0, 1, and 2.
	*/
	DllExport void	setTVerts(DWORD *vrt);
	/*! \remarks Sets the textured vertices.
	\par Parameters:
	<b>int a</b>\n\n
	Specifies the index into the <b>tVerts</b> array for vertex 0.\n\n
	<b>int b</b>\n\n
	Specifies the index into the <b>tVerts</b> array for vertex 1.\n\n
	<b>int c</b>\n\n
	Specifies the index into the <b>tVerts</b> array for vertex 2. */
	void	setTVerts(int a, int b, int c)  { t[0]=a; t[1]=b; t[2]=c; }	
	/*! \remarks Retrieves one of the texture vertices.
	\par Parameters:
	<b>int index</b>\n\n
	Specifies the index of the texture vertex to retrieve. You may use 0, 1 or
	2.
	\return  The texture vertex. */
	DWORD	getTVert(int index)	{ return t[index]; }
	/*! \remarks Returns a pointer to the array of texture vertices.\n\n
	  */
	DWORD *	getAllTVerts(void)	{ return t; }

	/*! \remarks	Returns the index of the specified texture vertex in this texture face's
	vertex list (0, 1 or 2). If not found 3 is returned.
	\par Parameters:
	<b>DWORD v0</b>\n\n
	The zero based index of the texture vertex to check. */
	DllExport DWORD GetVertIndex (DWORD v0);
	/*! \remarks	Returns the first texture vertex in this texture face that isn't v0 or v1.
	\par Parameters:
	<b>DWORD v0</b>\n\n
	The zero based index of one of the vertices to check.\n\n
	<b>DWORD v1</b>\n\n
	The zero based index of the other vertex to check. */
	DllExport DWORD GetOtherIndex (DWORD v0, DWORD v1);
	/*! \remarks	Indicates the order in which vertices v0 and v1 appear in the texture face.
	\par Parameters:
	<b>DWORD v0</b>\n\n
	One vertex on this texture face.\n\n
	<b>DWORD v1</b>\n\n
	Another vertex on this texture face.
	\return  <b>1</b> if v1 follows v0 in sequence.\n\n
	<b>-1</b> if v0 follows v1 in sequence.\n\n
	<b>0</b> if v0 or v1 are not on the face. */
	DllExport int Direction (DWORD v0, DWORD v1);
	/*! \remarks	This method switches v0,v1 if needed to put them in face-order. If v0 and
	v1 are in the order in which they appear in the texture face, or if one or
	both of them are not actually on the texture face, nothing happens. If
	however v0 follows v1, the values of the parameters are switched, so that
	they are then in the correct order for this texture face.
	\par Parameters:
	<b>DWORD \&v0</b>\n\n
	One vertex on this texture face.\n\n
	<b>DWORD \&v1</b>\n\n
	Another vertex on this texture face. */
	DllExport void OrderVerts (DWORD & v0, DWORD & v1);	// switches v0,v1 if needed to put them in face-order.
};


// MeshMap stuff:

#define MAX_MESHMAPS 100

#define MESHMAP_USED 0x0001
#define MESHMAP_TEXTURE 0x0002
#define MESHMAP_VERTCOLOR 0x0004
#define MESHMAP_USER 0x0100

#define NUM_HIDDENMAPS 2
// indexes for hidden maps are -1-(their position in the hidden map array).
// (These negative indexes are valid for methods such as mapSupport, mapVerts, etc.)
#define MAP_SHADING -1
#define MAP_ALPHA -2
//#define MAP_NORMALS -3

/*! \sa  Class Mesh.\n\n
\par Description:
This class is available in release 3.0 and later only.\n\n
In 3ds Max 3.0 and later the user may work with more than 2 mapping channels.
When the mapping channel is set to a value greater than 1 (by using a UVWMap
Modifier for example) then an instance of this class is allocated for each
channel up to the value specified. It maintains the mapping information for a
single channel.\n\n
An array of instances of this class is carried by the Mesh class in the public
data member:\n\n
<b>MeshMap *maps;</b>\n\n
All methods of this class are implemented by the system.
\par Data Members:
<b>DWORD flags;</b>\n\n
The mapping flags. One or more of the following values:\n\n
<b>MESHMAP_USED</b>\n\n
Indicates this mapping channel is actually used (carries mapping
information).\n\n
<b>MESHMAP_TEXTURE</b>\n\n
Indicates this is a texture mapping channel.\n\n
<b>MESHMAP_VERTCOLOR</b>\n\n
Indicates this is a vertex color channel.\n\n
<b>MESHMAP_USER</b>\n\n
Indicates the channel is used for a developer purpose.\n\n
<b>UVVert *tv;</b>\n\n
Array of texture vertices. This stores the UVW coordinates for the mapping
channel. <b>Note: typedef Point3 UVVert;</b>\n\n
<b>TVFace *tf;</b>\n\n
The texture vertex faces. There needs to be one TVFace for every face in the
Mesh, but there can be three indices into the UVVert array that are any
UV's.\n\n
<b>int vnum;</b>\n\n
The number of elements in the <b>UVVert</b> array.\n\n
<b>int fnum;</b>\n\n
The number of elements in the <b>TVFace</b> array. */
class MeshMap: public MaxHeapOperators {
public:
	DWORD flags;
	UVVert *tv;
	TVFace *tf;
	int vnum, fnum;

	/*! \remarks Constructor. The flags are cleared, the vertex and face
	numbers are set to 0, and the tv and tf pointers are set to NULL. */
	MeshMap () { flags=0x0; tv=NULL; tf=NULL; vnum = fnum = 0; }
	/*! \remarks Destructor. If the tv and tf arrays are allocated they are
	deleted. */
	DllExport ~MeshMap ();

	/*! \remarks Returns the number of <b>UVVerts</b>. */
	int getNumVerts () { return vnum; }
	/*! \remarks Sets the number of UVVerts allocated to the specified value.
	\par Parameters:
	<b>int vn</b>\n\n
	The new number of UVVerts to allocate.\n\n
	<b>BOOL keep=FALSE</b>\n\n
	If TRUE any previously allocated UVVerts are maintained (up to the maximum
	set by <b>vn</b>). If FALSE they are discarded. */
	DllExport void setNumVerts (int vn, BOOL keep=FALSE);
	/*! \remarks Returns the number of TVFaces. */
	int getNumFaces () { return fnum; }
	/*! \remarks Set the number of TVFaces allocated to the specified value.
	\par Parameters:
	<b>int fn</b>\n\n
	The new number of TVFaces to allocate.\n\n
	<b>BOOL keep=FALSE</b>\n\n
	If TRUE any previously allocated TVFaces are maintained (up to the maximum set
	by <b>fn</b>). If FALSE they are discarded. */
	DllExport void setNumFaces (int fn, BOOL keep=FALSE, int oldCt=0);
	/*! \remarks Clears (deletes) the <b>tv</b> and <b>tf</b> arrays and sets
	the counts to zero. */
	DllExport void Clear ();
	/*! \remarks This method returns a BitArray with size vnum, where isolated
	(unused) vertices are selected. */
	DllExport BitArray GetIsoVerts ();
	/*! \remarks This method is used to delete vertices from a mesh map.
	\par Parameters:
	<b>BitArray set</b>\n\n
	The array of bits where mapping vertices you want to delete are set.\n\n
	<b>BitArray *delFace=NULL</b>\n\n
	This is an optional parameter. If non-NULL, it's filled with a record of
	which faces, if any, were using the specified map verts and should
	therefore be deleted or considered invalid. (Note: in normal usage, it's
	preferable to remove any compromised faces _before_ deleting vertices, so
	this parameter would rarely be used.) */
	DllExport void DeleteVertSet (BitArray set, BitArray *delFace=NULL);
	/*! \remarks This method is used to delete faces from a mesh map.
	\par Parameters:
	<b>BitArray set</b>\n\n
	This is a list of mapping faces to delete.\n\n
	<b>BitArray *isoVert=NULL</b>\n\n
	If non-NULL, this BitArray is filled with a list of map vertices that were
	used by the deleted faces but not by any remaining faces. (This is a list
	of "newly isolated" map vertices.)\n\n
	NOTE: The number and arrangement of faces in a MeshMap should always agree
	with the "parent" mesh. It's safest in most cases to just let this be
	handled by <b>Mesh::DeleteFaceSet()</b>. */
	DllExport void DeleteFaceSet (BitArray set, BitArray *isoVert=NULL);

	/*! \remarks Sets the specified flag(s).
	\par Parameters:
	<b>DWORD fl</b>\n\n
	The flags to set. See the public data member <b>flags</b> above. */
	void SetFlag (DWORD fl) { flags |= fl; }
	/*! \remarks Clears the specified flag(s).
	\par Parameters:
	<b>DWORD fl</b>\n\n
	The flags to clear. See the public data member <b>flags</b> above. */
	void ClearFlag (DWORD fl) { flags &= ~fl; }
	/*! \remarks Returns TRUE if the specified flag(s) are set; otherwise
	FALSE.
	\par Parameters:
	<b>DWORD fl</b>\n\n
	The flags to check. See the public data member <b>flags</b> above. */
	BOOL GetFlag (DWORD fl) { return (flags & fl) ? TRUE : FALSE; }
	/*! \remarks Returns TRUE if this mapping channel is being used; otherwise
	FALSE. */
	BOOL IsUsed () const { return (flags & MESHMAP_USED) ? TRUE : FALSE; }

	/*! \remarks Exchanges the data between this MeshMap object and the
	specified one. The <b>flags</b>, <b>vnum</b> and <b>fnum</b> values are
	exchanged. The <b>UVVert</b> and <b>TVFace</b> pointers are swapped.
	\par Parameters:
	<b>MeshMap \&from</b>\n\n
	The MeshMap instance to swap with. */
	DllExport void SwapContents (MeshMap & from);
	/*! \remarks Assignment operator.
	\par Parameters:
	<b>MeshMap \&from</b>\n\n
	The MeshMap to assign. */
	DllExport MeshMap & operator= (MeshMap & from);
};

// Usually returns TEXMAP_CHANNEL or VERTCOLOR_CHANNEL:
/*! \remarks Returns the Channel ID of the map channel. if mp\>=1, this always returns
<b>TEXMAP_CHANNEL</b>. For mp\<1, including the hidden map channels, this is
currently always <b>VERTCOLOR_CHANNEL</b>. In the future it may include map
channels that are actually part of <b>GEOM_CHANNEL</b> or something.
\par Parameters:
<b>int mp</b>\n\n
The map channel. */
DllExport DWORD MapChannelID (int mp);
// Usually returns TEXMAP_CHAN_NUM, etc:
/*! \remarks Similar to MapChannelID, but this returns the CHAN_NUM
version:<b>TEXMAP_CHAN_NUM</b>, <b>VERTCOLOR_CHAN_NUM</b>, etc.
\par Parameters:
<b>int mp</b>\n\n
The map channel.  */
DllExport int MapChannelNum (int mp);

///  \brief 
/// This class can be used to store a bit per map channel,
/// including both negative and positive map channels, and it works with any
/// size index.
/// 
class MapBitArray: public MaxHeapOperators {
private:
	bool mTexture, mColor;
	BitArray mTextureFlip, mColorFlip;

public:
	///  \brief 
	/// Constructor.  Sets all channels to false.
	/// 
	MapBitArray () : mTexture(false), mColor(false) { }

	///  \brief 
	/// Constructor.  Sets all channels to the given default value.
	/// 
	MapBitArray (bool defaultValue)
		: mTexture(defaultValue), mColor(defaultValue) { }

	///  \brief 
	/// Constructor.  Sets all texture channels (1 and above) to the texture default,
	/// and all color channels (0 and below) to the color default.
	/// 
	MapBitArray (bool textureDefault, bool colorDefault)
		: mTexture(textureDefault), mColor(colorDefault) { }

	///  \brief 
	/// Returns the highest index that this MapBitArray could have a non-default value for.
	/// All indices above this are guaranteed to have value "TextureDefault()".
	/// 
	int Largest() const { return (mTextureFlip.GetSize()>1) ? mTextureFlip.GetSize()-1 : 0; }

	///  \brief 
	/// Returns the lowest index that this MapBitArray could have a non-default value for.
	/// All indices below this are guaranteed to have value "ColorDefault()".
	/// 
	int Smallest() const { return (mColorFlip.GetSize()>1) ? 1-mColorFlip.GetSize() : 0; }

	///  \brief 
	/// Sets the value of the given channel.
	/// 
	DllExport void Set (int mapChannel, bool val=true);

	///  \brief 
	/// Clears the value of the given channel.
	/// 
	void Clear (int mapChannel) { Set (mapChannel, false); }

	///  \brief 
	/// Gets the value of the given channel.  (If this particular channel has
	/// not been set before, the default for the channel will be returned.)
	/// 
	DllExport bool Get (int mapChannel) const;

	///  \brief 
	/// Returns the default value for texture channels, channels 1 and above.
	/// 
	bool TextureDefault () const { return mTexture; }

	///  \brief 
	/// Returns the default value for color channels, channels 0 and below.
	/// 
	bool ColorDefault () const { return mColor; }

	///  \brief 
	/// Inverts the value of all texture channels (1 and above).
	/// 
	void InvertTextureChannels () { mTexture=!mTexture; }

	///  \brief 
	/// Inverts the value of all color channels (0 and below).
	/// 
	void InvertColorChannels () { mColor=!mColor; }

	///  \brief 
	/// Inverts all channel values.
	/// 
	void InvertAll () { mColor = !mColor; mTexture = !mTexture; }

	///  \brief 
	/// Saves data to stream.
	/// 
	DllExport IOResult Save(ISave* isave);

	///  \brief 
	/// Loads data from stream.
	/// 
	DllExport IOResult Load(ILoad* iload);

	///  \brief 
	/// Indexing operator
	/// 
	bool operator[](int i) const { return Get(i); }

	///  \brief 
	/// Comparison operator.
	/// 
	DllExport BOOL operator==(const MapBitArray& b) const;

	///  \brief 
	/// Inequality operator.
	/// 
	BOOL operator!=(const MapBitArray& b) const { return !(*this == b); }

	///  \brief 
	/// Assignment operator
	/// 
	DllExport MapBitArray& operator=(const MapBitArray& b);
};

// Following is used for arbitrary per-element info in meshes, such as weighted verts
// or weighted vert selections.  Methods are deliberately made to look like Tab<> methods.

// For per-vertex info: set a maximum, and reserve first ten channels
// for Discreet's use only.
#define MAX_VERTDATA 100
#define VDATA_USER 10	// Third parties should use this channel or higher.

// Indices of important per-vertex data
#define VDATA_SELECT  0   // Soft Selection
#define VDATA_WEIGHT  1   // Vertex weights (for NURMS MeshSmooth)
#define VDATA_ALPHA   2   // Vertex Alpha values
#define VDATA_CORNER  3	// NOT USED

// Related constants:
#define MAX_WEIGHT ((float)1e5)
#define MIN_WEIGHT ((float)1e-5)

// Types of data
#define PERDATA_TYPE_FLOAT 0

// Vertex-specific methods:
/*! \remarks Returns the type of data supported, i.e.
<b>PERDATA_TYPE_FLOAT</b>.
\par Parameters:
<b>int vdID</b>\n\n
This parameter is ignored. */
DllExport int VertexDataType (int vdID);
/*! \remarks Returns a pointer to a default floating point value for the
specified channel.
\par Parameters:
<b>int vdID</b>\n\n
One of the following values:\n\n
<b>VDATA_SELECT</b> - Soft Selection\n\n
<b>VDATA_WEIGHT</b> - Vertex weights (for NURMS MeshSmooth)\n\n
<b>VDATA_ALPHA</b>- Vertex Alpha values (R4 and later only)\n\n
<b>VDATA_CORNER</b>- Cornering values for subdivision use (R4 and later only)
 */
DllExport void *VertexDataDefault (int vdID);

/*! \sa  Class Mesh.\n\n
\par Description:
This class is available in release 3.0 and later only.\n\n
This class is used for per -'something' floating-point information. For
example, it is used with Meshes to keep track of such per-<b>vertex</b>
information as weighted (Affect Region or Soft) selections and vertex weights.
It is used in MNMesh to store per-<b>edge</b> data (edge weights).\n\n
Currently there's only one "type" of data supported, floating point values, but
this may be extended in the future. PerData arrays in Meshes and MNMeshes
cannot be reserved for plug-ins at this time; 3ds Max maintains the list in
MESH.H of the reserved vertex data channels, and in MNMESH.H for the MNEdge
data channels.\n\n
The methods of this class are deliberately made to look like Tab\<\> methods.
All methods of this class are implemented by the system.
\par Data Members:
All data members are public.\n\n
<b>int dnum;</b>\n\n
The number of elements of per-vertex data.\n\n
<b>int type;</b>\n\n
The type of data held by this class. At present the only 3ds Max defined option
is:\n\n
<b>PERDATA_TYPE_FLOAT</b>\n\n
<b>int alloc;</b>\n\n
The number of elements currently allocated in the <b>data</b> array.\n\n
<b>void *data;</b>\n\n
Points to the actual data. */
class PerData: public MaxHeapOperators {
public:
	int dnum, type, alloc;
	void *data;

	/*! \remarks Constructor. The number of elements is set to 0, the type is
	set to 0 and the data pointer is set to NULL. */
	PerData () { data=NULL; dnum=0; alloc=0; type=0; }
	/*! \remarks Constructor. The data members are initialized as follows:\n\n
	<b>data=NULL; dnum=0; alloc=0; type=tp;</b>
	\par Parameters:
	<b>int n</b>\n\n
	The number of elements to allocate.\n\n
	<b>int tp</b>\n\n
	The type to set. */
	PerData (int n, int tp) { data=NULL; dnum=0; alloc=0; type=tp; setAlloc (n, FALSE); }
	/*! \remarks Destructor. Any allocated data is freed and the count and
	type are set to 0. */
	~PerData () { Clear (); }

	// Following only depend on type:
	/*! \remarks Allocates and returns a pointer to an array of floats of the
	specified size.
	\par Parameters:
	<b>int num</b>\n\n
	The number of floats to allocate. */
	DllExport void *AllocData (int num);
	/*! \remarks Deletes the specified array of floats.
	\par Parameters:
	<b>void *addr</b>\n\n
	Points to the array of floats to free. */
	DllExport void FreeData (void *addr);
	/*! \remarks Returns the number of bytes used by the base data type for
	the vertex data. This is only implemented for a type of
	<b>VDATA_TYPE_FLOAT</b> in which case it returns <b>sizeof(float)</b>.
	Other cases simply return 0. */
	DllExport int DataSize ();
	/*! \remarks Returns the address of the specified element in the array
	passed.
	\par Parameters:
	<b>void *ptr</b>\n\n
	The array whose <b>at</b>-th element address is returned.\n\n
	<b>int at</b>\n\n
	The zero based index of the element. */
	void *Addr (void *ptr, int at) { BYTE *vd=(BYTE *)ptr; return (void *)(vd+at*DataSize()); }
	/*! \remarks Returns the address of the specified element in the
	<b>data</b> array.
	\par Parameters:
	<b>int at</b>\n\n
	The zero based index of the element. */
	void *Addr (int at) { return Addr(data,at); }
	/*! \remarks Copies the specified number of elements between the two data
	arrays.
	\par Parameters:
	<b>void *to</b>\n\n
	Points to the destination data array.\n\n
	<b>void *from</b>\n\n
	Points to the source data array.\n\n
	<b>int num=1</b>\n\n
	The number of elements to copy. */
	DllExport void CopyData (void *to, void *from, int num=1);
	/*! \remarks Copies the specified number of elements between the two
	specified locations in the <b>data</b> array.
	\par Parameters:
	<b>int to</b>\n\n
	The zero based index into the <b>data</b> array of the destination.\n\n
	<b>int from</b>\n\n
	The zero based index into the <b>data</b> array of the source.\n\n
	<b>int num=1</b>\n\n
	The number of elements to copy. */
	void CopyData ( int to,  int from, int num=1) { CopyData (Addr(to), Addr(from), num); }
	/*! \remarks Computes the weighted sum of the arguments passed. This is
	effectivly c = a*prop1 + b*prop2.\n\n
	This is used, for example, in splitting an edge, where we would want to
	interpolate the vertex weight values from the edge's endpoints to create
	the weight for the new vertex.
	\par Parameters:
	<b>void *to</b>\n\n
	A pointer to the location in which the result should be stored.\n\n
	<b>void *fr1</b>\n\n
	A pointer to the first value to be summed.\n\n
	<b>float prop1</b>\n\n
	The weight given to the first value.\n\n
	<b>void *fr2</b>\n\n
	A pointer to the second value.\n\n
	<b>float prop2</b>\n\n
	The weight given to the second value. */
	DllExport void WeightedSum (void *to, void *fr1, float prop1, void *fr2, float prop2);
	/*! \remarks Computes the weighted sum of the arguments passed. This is
	similar to the method above except to, fr1, and fr2 are indices of the
	values in the PerData array. That is, PerData::WeightedSum (c, a, prop1, b,
	prop2), where a, b, and c are ints between 0 and PerData::dnum-1, is
	equivalent to the call PerData::WeightedSum (PerData::Addr(c),
	PerData::Addr(a), prop1, PerData::Addr(b), prop2).
	\par Parameters:
	<b>int to</b>\n\n
	The index in the PerData array of the location in which the result should
	be stored.\n\n
	<b>int fr1</b>\n\n
	The index of the first value to be summed in the PerData array.\n\n
	<b>float prop1</b>\n\n
	The weight given to the first value.\n\n
	<b>int fr2</b>\n\n
	The index of the second value to be summed in the PerData array.\n\n
	<b>float prop2</b>\n\n
	The weight given to the second value. */
	void WeightedSum (int to, int fr1, float prop1, int fr2, float prop2) { WeightedSum (Addr(to), Addr(fr1), prop1, Addr(fr2), prop2); }

	/*! \remarks Sets the number of elements allocated in the <b>data</b>
	array.
	\par Parameters:
	<b>int num</b>\n\n
	The number of elements to allocate.\n\n
	<b>BOOL keep=TRUE</b>\n\n
	If TRUE previous values are kept (copied to the new storage); otherwise
	they are discarded. */
	DllExport void setAlloc (int num, BOOL keep=TRUE);
	/*! \remarks Sets the number of elements allocated in the <b>data</b>
	array and sets the <b>dnum</b> member to <b>num</b>.
	\par Parameters:
	<b>int num</b>\n\n
	The number of elements to allocate.\n\n
	<b>BOOL keep = FALSE</b>\n\n
	If TRUE previous values are kept (copied to the new storage); otherwise
	they are discarded. */
	void SetCount (int num, BOOL keep = FALSE) { setAlloc (num, keep); dnum=num; }
	/*! \remarks Reduces the size of the <b>data</b> array to contain
	<b>dnum</b> elelemts. */
	void Shrink () { if (alloc>dnum) setAlloc(dnum); }
	/*! \remarks Returns the number of elements used (<b>dnum</b>) */
	int Count () { return dnum; }
	/*! \remarks Clears (deletes) any allocated data and sets the count and
	type to 0. */
	DllExport void Clear ();
	/*! \remarks Removes any element whose corresponding element in the
	BitArray is not set.
	\par Parameters:
	<b>BitArray del</b>\n\n
	Specifies which elements to delete. Data elelemts corresponding to bits
	that are on remain; for bits that are off the elements are deleted. */
	DllExport void DeleteSet (BitArray del);
	/*! \remarks Deletes the specifiec number of elements from the specified
	location in the data array.
	\par Parameters:
	<b>int at</b>\n\n
	The location to delete elements.\n\n
	<b>int num</b>\n\n
	The number of elements to delete. */
	DllExport void Delete (int at, int num);
	/*! \remarks Inserts the specified number of data elements into the
	specified location in the <b>data</b> array.
	\par Parameters:
	<b>int at</b>\n\n
	The zero based index of the location for the insert.\n\n
	<b>int num</b>\n\n
	The number of elements to insert.\n\n
	<b>void *el</b>\n\n
	The data to insert. */
	DllExport void Insert (int at, int num, void *el);
	/*! \remarks Appends the specified elements to the <b>data</b> array.
	\par Parameters:
	<b>int num</b>\n\n
	The number of elements to append.\n\n
	<b>void *el</b>\n\n
	The data to append. */
	DllExport void Append (int num, void *el);
	/*! \remarks Inserts the specified number of elements into the <b>data</b>
	array at the given location.
	\par Parameters:
	<b>int at</b>\n\n
	The zero based index of the location to insert the data.\n\n
	<b>int num</b>\n\n
	The number of elements to insert.\n\n
	<b>void *el</b> */
	DllExport void InsertCopies (int at, int num, void *el);
	/*! \remarks Appends the specified number of elements to the <b>data</b>
	array.
	\par Parameters:
	<b>int num</b>\n\n
	The number of elements to append.\n\n
	<b>void *el</b>\n\n
	The data to append. */
	DllExport void AppendCopies (int num, void *el);

	/*! \remarks Swaps the contents of this PerData object and the specified
	one.
	\par Parameters:
	<b>PerData \&from</b>\n\n
	The object to swap with. */
	DllExport void SwapContents (PerData & from);
	/*! \remarks Assignment operator.
	\par Parameters:
	<b>PerData \&from</b>\n\n
	The VertexData source. */
	DllExport PerData & operator= (const PerData & from);
	DllExport void MyDebugPrint ();
};

// Mesh::flags definitions
/*! \defgroup MeshVertexFaceFlags Mesh, Vertex and Face Flags
The following represent flags describing various flags that can be set on the mesh, vertices and faces.  Not all
flags can be applied to each sub object type.
*/
//@{
#define MESH_EDGE_LIST     (1<<1)			//!< Set this to prevent renderData from being deleted (except when mesh is deleted)
#define MESH_LOCK_RENDDATA (1<<2)
#define MESH_SMOOTH_BIT1   (1<<3)
#define MESH_SMOOTH_BIT2   (1<<4)
#define MESH_SMOOTH_BIT3   (1<<5)
#define MESH_SMOOTH_BIT4   (1<<6)
#define MESH_SMOOTH_MASK   0x78				//!< Face? flag mask for SMOOTH_BIT's 1 thru 4
#define MESH_BEEN_DSP	   (1<<9)			//!< Mesh? flag that marks the mesh as has been drawn I think
#define MESH_SMOOTH_SUBSEL (1<<10)
#define MESH_FACENORMALSINVALID  (1<<11)	//!< Mesh flag that Signals that face normals should be rebuilt
#define MESH_CACHEINVALID  (1<<12)			//!< Mesh, vertices and Face flag Signals that the they have changed and RVertices normals should be rebuilt 
#define MESH_BACKFACESCOMPUTED (1<<16)		//!< Mesh flag to determine whether the back facing attribute on the face list has been computed or not
#define MESH_PARTIALCACHEINVALID (1<<17)	//!< Mesh flag that means that the mesh has only set some of the vertices as invalid and not to reprocess the entire mesh just the vertices that changed
//@}

// For internal use only!
#define MESH_TEMP_1 (1<<13)
#define MESH_TEMP_2 (1<<14)
#define MESH_DONTTRISTRIP (1<<15)	// Mesh flag to determine whether to use tristripping or not



#define COMP_TRANSFORM	0x0001	// forces recalc of model->screen transform; else will attempt to use cache
#define COMP_IGN_RECT	0x0002	// forces all polys to be rendered; else only those intersecting the box will be
#define COMP_LIGHTING	0x0004	// forces re-lighting of all verts (as when a light moves); else only relight moved verts

#define COMP_ALL		0x00ff

// If this bit is set then the node being displayed by this mesh is selected.
// Certain display flags only activate when this bit is set.
#define COMP_OBJSELECTED	(1<<8)


class StripData: public MaxHeapOperators {
public:
	int ct;
	DWORD f[6];
	void AddFace(DWORD face)
		{ if(ct < 6) f[ct++] = face; }
};

typedef int (*INTRFUNC)();

DllExport void setMeshIntrFunc(INTRFUNC fn);


/*! \sa  Class Mesh, Class BitArray.\n\n
\par Description:
This class allows access to the sub-object hit records used in Mesh hit
testing. All methods of this class are implemented by the system.
\par Data Members:
<b>DWORD dist;</b>\n\n
The distance of the hit. If the user is in wireframe mode, this is the distance
in pixels to the item that was hit. If the user is in shaded mode, this is the
Z depth distance. Smaller numbers indicate a closer hit.\n\n
<b>int index;</b>\n\n
The index of the sub-object component. For example, if faces were being hit
tested, this would be the index of the mesh's BitArray <b>faceSel</b>. For
edges, this is the index into the <b>edgeSel</b> BitArray, where the index is
<b>3*faceIndex+edgeIndex</b>.\n\n
<b>DWORD flags;</b>\n\n
These are not currently used.  */
class MeshSubHitRec: public MaxHeapOperators {
	private:		
		MeshSubHitRec *next;
	public:
		DWORD	dist;
		int		index;
		DWORD	flags;

		/*! \remarks Constructor. The data members are initialized to the data
		members passed. */
		MeshSubHitRec(DWORD dist, int index, MeshSubHitRec *next) 
			{this->dist = dist; this->index = index; this->next = next;}
		/*! \remarks Constructor. The data members are initialized to the data
		members passed. */
		MeshSubHitRec(DWORD dist, int index, DWORD flags, MeshSubHitRec *next) 
			{this->dist = dist; this->index = index; this->next = next;this->flags = flags;}

		/*! \remarks Returns the next mesh sub hit record. */
		MeshSubHitRec *Next() { return next; }		
	};

/*! \sa  Class MeshSubHitRec.\n\n
\par Description:
This class describes a list of sub-object hit records. All methods of this
class are implemented by the system.  */
class SubObjHitList: public MaxHeapOperators {
	private:
		MeshSubHitRec *first;
	public:
		/*! \remarks Constructor. The list is set to NULL. */
		SubObjHitList() { first = NULL; }
		/*! \remarks Destructor. All the hit records are deleted. */
		DllExport ~SubObjHitList();
		/*! \remarks Returns the first item in the hit list. */
		MeshSubHitRec *First() { return first; }
		/*! \remarks Allocates and adds a new hit record to the list.
		\par Parameters:
		<b>DWORD dist</b>\n\n
		The distance of the hit.\n\n
		<b>int index</b>\n\n
		The index of the hit. */
		DllExport void AddHit( DWORD dist, int index );
	};



// Flags for sub object hit test

// NOTE: these are the same bits used for object level.
#define SUBHIT_SELONLY		(1<<0)
#define SUBHIT_UNSELONLY	(1<<2)
#define SUBHIT_ABORTONHIT	(1<<3)
#define SUBHIT_SELSOLID		(1<<4)

#define SUBHIT_USEFACESEL	(1<<23)   // When this bit is set, the sel only and unsel only tests will use the faces selection when doing a vertex level hit test
#define SUBHIT_VERTS		(1<<24)
#define SUBHIT_FACES		(1<<25)
#define SUBHIT_EDGES		(1<<26)
#define SUBHIT_TYPEMASK		(SUBHIT_VERTS|SUBHIT_FACES|SUBHIT_EDGES)

// Selection level bits.
#define MESH_OBJECT		(1<<0)
#define MESH_VERTEX		(1<<1)
#define MESH_FACE		(1<<2)
#define MESH_EDGE		(1<<3)

// Normal Display flags
#define MESH_DISP_NO_NORMALS		0
#define MESH_DISP_FACE_NORMALS		(1<<0)
#define MESH_DISP_VERTEX_NORMALS	(1<<1)


class MeshOpProgress;
class UVWMapper;

/*! \sa  Class Mesh.\n\n
\par Description:
A developer may derive a class from this class, put any required data in it,
and then hang this data off a Mesh. This is done using the methods of Class
Mesh:\n\n
<b>void SetRenderData(MeshRenderData *p);</b>\n\n
Sets the mesh render data hung off this Mesh.\n\n
<b>MeshRenderData * GetRenderData();</b>\n\n
Gets a pointer to the MeshRenderData.  */
class MeshRenderData: public MaxHeapOperators {
	public:
	/*! \remarks Destructor. */
	virtual ~MeshRenderData() {;}
	/*! \remarks Deletes this instance of the class. */
	virtual void DeleteThis()=0;
	};

class AdjFaceList;

/*! \sa  Class BitArray, Class Face,  Class TVFace,  Class Point3, Class TriObject, Class RNormal, Class RVertex, Class TriObject, Class MeshMap, <a href="ms-its:3dsmaxsdk.chm::/meshes_root.html">Working with Meshes</a>,
\par Description:
The Mesh class is provided for use by plug-ins and is used by the system. It is
the data structure for meshes in the system and maintains pointers to the
vertices, faces, texture vertices, etc. It provides methods to handle most of
the functionality that procedural objects must implement. All methods of this
class are implemented by the system.\n\n
Note: There is a set of classes for working with parts of a mesh such as its
face structure, element structure, and cluster structure. For details see:
Class AdjEdgeList, Class AdjFaceList, Class FaceElementList, Class FaceClusterList.
\par Method Groups:
See <a href="class_mesh_groups.html">Method Groups for Class Mesh</a>.
\par Data Members:
private:\n\n
<b>MeshMap hmaps[NUM_HIDDENMAPS];</b>\n\n
The "Hidden Maps". Eventually these will be supported by Patches and
PolyMeshes, as well as in the general object interface. The purpose is to use a
separate, private array for a fixed number of these maps (unlike the allocable
regular map array). This allows us to reserve and support as many map channels
as we need for features such as vertex alpha, vertex illumination, possible
vertex normals, or other needs that may arise.\n\n
The interface for accessing these channels uses the usual map methods, with
negative indices. For instance, <b>hmaps[0]</b> is indexed as
<b>MAP_SHADING</b>, -1. <b>hmaps[1]</b> is indexed as <b>MAP_ALPHA=-2</b>. So
now methods like <b>Mesh::mapVerts (int mp)</b> have a range of
<b>-NUM_HIDDENMAPS</b> to <b>getNumMaps()</b>, instead of 0 to
<b>getNumMaps()</b> as before.\n\n
<b>NUM_HIDDENMAPS</b>: The number of "Hidden" or negative-indexed maps in
objects which support hidden maps. (Currently set to 2.)\n\n
<b>MAP_SHADING</b>: The shading (or illumination) map. Set to -1.\n\n
<b>MAP_ALPHA</b>: The Alpha channel map. Note that only the x (or u) coordinate
of the map vertices is currently used. Set to -2.\n\n
\par Topology Data:
<b>int numVerts;</b>\n
Number of vertices.\n\n
<b>int numFaces;</b>\n
Number of faces.\n\n
<b>Face *faces;</b>\n
Array of faces.
\par Geometry Data:
<b>Point3 *verts;</b>\n
Array of vertex coordinates.
\par Texture Coordinate Data:
<b>int numTVerts;</b>\n\n
Number of texture vertices.\n\n
<b>UVVert *tVerts;</b>\n\n
The array of texture vertices. This stores the UVW coordinates. For a 2D
mapping only two of them are used, i.e. UV, VW, or WU. This just provides
greater flexibility so the user can choose to use UV, VW, or WU.\n\n
Note: <b>typedef Point3 UVVert;</b>\n\n
<b>TVFace *tvFace;</b>\n\n
The array of texture faces. There needs to be one <b>TVFace</b> for every face,
but there can be three indices into the UVVert array that are any UVs. Each
face of the object can have its own mapping.\n\n
<b>int numMaps;</b>\n\n
This data member is available in release 3.0 and later only.\n\n
The number of maps supported by the mesh. By default this is 2 but may be
changed with the multiple map methods listed below.\n\n
<b>MeshMap *maps;</b>\n\n
This data member is available in release 3.0 and later only.\n\n
When the number of mapping channels is set to a value greater than 1 then an
instance of this class is allocated for each channel up to <b>numMaps</b>. An
instance maintains the mapping information for a single channel.\n\n
<b>BitArray vdSupport;</b>\n\n
This data member is available in release 3.0 and later only.\n\n
This bit array indicates if a particular vertex data channel is supported in
this mesh. If the bit is set the channel is suported.\n\n
<b>PerData *vData;</b>\n\n
This data member is available in release 3.0 and later only.\n\n
The array of PerData objects which maintain and provide access to the floating
point vertex data. There is one of these for each supported channel. The first
two PerData objects in this array are used internally by 3ds Max.
\par Color Per Vertex Data:
<b>int numCVerts;</b>\n\n
Number of color vertices.\n\n
<b>VertColor *vertCol;</b>\n\n
Array of color vertices.\n\n
<b>TVFace *vcFace;</b>\n\n
Array of color per vertex faces.\n\n
<b>VertColor *curVCArray;</b>\n\n
This data member is available in release 4.0 and later only.\n\n
Points to storage for a possible external color array (default = NULL). This
can be either an external array or one of the mapping channels. See the method
<b>Mesh::setVCDisplayData();</b>\n\n
<b>int curVCChan;</b>\n\n
This data member is available in release 4.0 and later only.\n\n
Storage for the current mapping channel to use for vertex colors (default =
0).\n\n
<b>VertColor *vertColArray;</b>\n\n
This data member is available in release 4.0 and later only.\n\n
When 3ds Max is rendering the color values come from this variable. This array
defaults to the internal <b>vertCol</b> but can be set to an external array, or
a map channel. See the method <b>Mesh::setVCDisplayData();</b>\n\n
<b>TVFace *vcFaceData;</b>\n\n
This data member is available in release 4.0 and later only.\n\n
When 3ds Max is rendering the vertex color lookup comes from this structure.
This defaults to the <b>vcFace</b> data but if a mapping channel is used for
color lookup, its <b>TVFace</b> structure is used.
\par Selection Data:
<b>BitArray vertSel;</b>\n\n
Indicates the selected vertices. There is one bit for each vertex. Bits that
are 1 indicate the vertex is selected.\n\n
<b>BitArray faceSel;</b>\n\n
Indicates the selected faces. There is one bit for each face. Bits that are 1
indicate the face is selected.\n\n
<b>BitArray edgeSel;</b>\n\n
Indicates the selected edges. There is one bit for each edge of each face. Bits
that are 1 indicate the edge is selected. The edge is identified by
<b>3*faceIndex + edgeIndex</b>.\n\n
<b>BitArray vertHide;</b>\n\n
Hidden flags for vertices.
\par Display Attribute Flags:
<b>DWORD dispFlags;</b>\n\n
These control various aspect of the Mesh objects display and may be one or more
of the following values:\n\n
<b>DISP_VERTTICKS</b> - Display vertices as small tick marks.\n\n
<b>DISP_SELVERTS</b> - Display selected vertices.\n\n
<b>DISP_SELFACES</b> - Display selected faces.\n\n
<b>DISP_SELEDGES</b> - Display selected edges.\n\n
<b>DISP_SELPOLYS</b> - Display selected polygons. Polygons are defined as
adjacent triangles with hidden edges. A selected face would show all edges
regardless of if they were hidden edges. A polygon would only show the edges of
the polygon that were not hidden.
\par Selection Level:
<b>DWORD selLevel;</b>\n\n
This is the current level of selection. When all the bits are 0, the object is
at object level selection. The selection level bits are:\n\n
<b>MESH_OBJECT</b>- Object level.\n\n
<b>MESH_VERTEX</b> - Vertex level.\n\n
<b>MESH_FACE</b> - Face level.\n\n
<b>MESH_EDGE</b> - Edge level.
\par Normals:
<b>int normalsBuilt;</b>\n
Nonzero if normals have been built for the current mesh; otherwise 0.
\par Render Data:
<b>MeshRenderData *renderData;</b>\n
Points to the render data used by the renderer. */
class Mesh : public BaseInterfaceServer {
	friend class Face;
	friend class MeshAccess;
	friend class HardwareMesh;
	friend void gfxCleanup(void *data);

	private:

#if MESH_MULTI_PROCESSING
		static int		refCount;
		static HANDLE	xfmThread;
		static HANDLE	xfmMutex;
		static HANDLE	xfmStartEvent;
		static HANDLE	xfmEndEvent;
		friend DWORD WINAPI xfmFunc(LPVOID ptr);
		static HANDLE	fNorThread;
		static HANDLE	fNorMutex;
		static HANDLE	fNorStartEvent;
		static HANDLE	fNorEndEvent;
		friend DWORD WINAPI fNorFunc(LPVOID ptr);

		static HANDLE	workThread;
		static HANDLE	workMutex;
		static HANDLE	workStartEvent;
		static HANDLE	workEndEvent;
		friend DWORD WINAPI workFunc(LPVOID ptr);
#endif
		// derived data-- can be regenerated
		RVertex 		*rVerts;		// <<< instance specific.
		GraphicsWindow 	*cacheGW;  		// identifies rVerts cache
		Point3	 		*faceNormal;	// object space--depends on geom+topo
		Box3			bdgBox;			// object space--depends on geom+topo
		int 			numVisEdges;	// depends on topo 
		int				edgeListHasAll;	// depends on topo
		VEdge 			*visEdge;		// depends on topo 	
 
 		// Vertex and face work arrays -- for snap code
		int			snapVCt;
		int			snapFCt;
		char		*snapV;
		char		*snapF;

		// A simple edge list used so we can draw edges fast in object or vertex sub object mode
		Tab<DWORD>	mDisplayEdges;
		//these are counter of various derived data so we can track when to delete them when they become unused
		UINT		mDXMeshCacheDrawCount;  //this the number of times that the direct mesh cache has been used to draw the mesh exclusively
		UINT		mRedrawCountFromLoad;  //this is the number of times the mesh has been redrawn from load.  This is used to prevent an initial memory spike on load
											//this will be set to -1 if the mesh was created and not loaded

		// Reserved maps for special purposes, such as vertex shading.
		MeshMap hmaps[NUM_HIDDENMAPS];

		// -------------------------------------
		//
		long   		flags;		  	// work flags- 

		float 		norScale;	    // scale of normals -- couldn't this be done
		 							// automatically relative to bdgBox?




		// Rolf: these are instance specific and should be pulled out of here,
		// and just passed in from the Node.
		BYTE		dspNormals;    // display surface normals--- put in flags?
		BYTE		dspAllEdges;   // shows hidden edges  ---- put in flags?
		BYTE		dspVertTicks;  // shows vertex ticks

		//! \brief This only draws the selected edges used to draw them over the directX cache 
		/*!	This draws the selected edges of the mesh.  Backface culling is not supported
		\param[in] gw The graphics window associated with the draw.
		*/
		void	DrawOnlySelectedEdges(GraphicsWindow *gw);

		//! \brief This only draws the selected faces used to draw them over the directX cache 
		/*!	This draws the selected faces of the mesh.  Backface culling is not supported
		\param[in] gw The graphics window associated with the draw.
		*/
		void	DrawOnlySelectedFaces(GraphicsWindow *gw);

		int 		renderFace(GraphicsWindow *gw, DWORD index, int *custVis=NULL);
		int			renderEdge(GraphicsWindow *gw, DWORD face, DWORD edge);
		int			renderFaceVerts(GraphicsWindow *gw, DWORD index);
		void		renderStrip(GraphicsWindow *gw, Strip *s);
		void 		render3DFace(GraphicsWindow *gw, DWORD index, int *custVis=NULL);
		void		render3DFaceVerts(GraphicsWindow *gw, DWORD index);
		void		render3DStrip(GraphicsWindow *gw, Strip *s);
		void		render3DWireStrip(GraphicsWindow *gw, Strip *s);
		BOOL		CanDrawStrips(DWORD rndMode, Material *mtl, int numMtls);
		BOOL		NormalsMatchVerts();
		void		checkRVertsAlloc(void);
		void  		calcNormal(int i);
		void		buildFaceNormals();		// calcs just the face normals
		void		setCacheGW(GraphicsWindow *gw)	{ cacheGW = gw; }
		GraphicsWindow *getCacheGW(void)			{ return cacheGW; }

		// New Mesh routines to drive HardwareShaders
		bool		CanDrawTriStrips(DWORD rndMode, int numMtls, Material *mtl);
		bool		BuildTriStrips(DWORD rndMode, int numMtls, Material *mtl);
		void		TriStripify(DWORD rndMode, int numTex, TVFace *tvf[], TriStrip *s, StripData *sd, int vtx);
		void 		Draw3DTriStrips(IHardwareShader *phs, int numMat, Material *ma);
		void 		Draw3DWireTriStrips(IHardwareShader *phs, int numMat, Material *ma);
		void 		Draw3DVisEdgeList(IHardwareShader *phs, DWORD flags);
		int			render3DTriStrips(IHardwareShader *phs, int kmat, int kstrips);
		int			render3DWireTriStrips(IHardwareShader *phs, int kmat, int kstrips);
		int 		render3DFaces(IHardwareShader *phs, DWORD index, int *custVis=NULL);

		void 		freeVerts();
		void        freeVertCol();
		void  		freeFaces();
		void		freeFaceNormals();  
		void  		freeRVerts(BOOL forceDelete=FALSE);
		void  		freeTVerts();
		void  		freeTVFaces();
		void  		freeVCFaces();
		void		freeSnapData();
		int			buildSnapData(GraphicsWindow *gw,int verts,int edges);
		void		ComputeBackFaceFlags(GraphicsWindow *gw);

		//Just some temporary structs to hold a list of only changed faces and verts.  They are used when building normals.
		BitArray	mInvalidVert;
		BitArray	mInvalidFace;
		Tab<int>	mInvalidVertList;
		Tab<int>	mInvalidFaceList;
		//Frees one rvert in the list
		void		FreeRVert(int index);
		void		ComputeFaceAngle(int index, float *angles);
		void		ComputeRVert(int index, float *faceAngles);

	public:

		// Topology
		int			numVerts;
		int	 		numFaces;
		Face *		faces;

		// Geometry
		Point3 *	verts;		

		// Texture Coord assignment 
		int			numTVerts;
		UVVert *	tVerts;
		TVFace *	tvFace;  	 

		// Color per vertex
		int numCVerts;
		VertColor * vertCol;
		TVFace *    vcFace;

		int			curVCChan;	// current map channel to use for colors (default = 0)
		VertColor *	curVCArray;	// possible external color array (default = NULL)
		TVFace	  * curVCFace;	// possible external face array (default = NULL)

		// for rendering, the color values come from the vertColArray variable.  This array defaults
		// to the internal vertCol, but can be set to an external array, or a map channel
		VertColor * vertColArray;
		// for rendering, the vertex lookup comes from the vcFaceData structure.  This defaults to
		// the vcFace data, but if a map channel is used for color lookup, we use its TVFace structure
		TVFace *	vcFaceData;

		// More maps:
		int numMaps;
		MeshMap *maps;

		// Per-vertex info (of any kind):
		BitArray vdSupport;
		PerData *vData;

		// Material assignment
		MtlID		mtlIndex;		// object material

		// Selection
		BitArray	vertSel;  		// selected vertices
		BitArray	faceSel;  		// selected faces
		BitArray	edgeSel;		// selected edges, identified as 3*faceIndex + edgeIndex
		BitArray	vertHide;		// Hide flags for vertices

		// Display attribute flags
		DWORD		dispFlags;

		// Selection level
		DWORD		selLevel;

		// true if normals have been built for the current mesh
		// SCA revision for Max 5.0 - value of 0 for unbuilt, 1 for built according to legacy 4.0 scheme,
		// and 2 for built according to new scheme - this is so that we refresh normals if the user changes
		// the "Use Legacy 4.0 Vertex Normals" checkbox.
		int			normalsBuilt;

		MeshRenderData*	 renderData;  // used by the renderer

		// derived data-- can be regenerated
		StripTab		*stab;		// depends on topo
		DWTab		norInd;			// indirection array for fast normal lookup
		int			normalCount;	// total number of normals
		Point3 *	gfxNormals;		// flattened list of normals pointers back in the rverts data.  We need to get rid of this or something this is big memory eater

		//Builds the GFX Normals
		void		BuildGFXNormals();

		// Derived arrays to contain generated texture coordinates
		int			numTexCoords[GFX_MAX_TEXTURES];
		Point3 *	texCoords[GFX_MAX_TEXTURES];

		// Derived table of TriStrips, depends on topology
		Tab<TriStrip *>	*tstab;

		/*! \remarks Constructor. Initializes the mesh object. The mesh counts
		are set to 0 and its pointers are set to NULL. */
		DllExport Mesh();
		/*! \remarks Constructor. The mesh is initialized equal to
		<b>fromMesh</b>. */
		DllExport Mesh(const Mesh& fromMesh);
		/*! \remarks Destructor. Frees any allocated arrays (faces, verts,
		tverts, tvfaces). */
		DllExport ~Mesh(); 
		/*! \remarks Initializes the mesh object. The mesh counts are set to 0
		and its pointers are set to NULL. Note: This method is not intended to
		be called by developers. It is used internally. */
		DllExport void Init();
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		This method deletes this mesh. */
		DllExport void DeleteThis();

		/*! \remarks Assignment operator. Note: This operator does not copy
		the <b>rVerts</b> array. This means that developers who have specified
		normals will have to explicitly copy them after a mesh assignment is
		done using this operator. This is because <b>rVerts</b> are
		instance-specific. In general, normals are computed from smoothing
		groups, and hence are "generated data". In the case where normals are
		specified, 3ds Max stores the data in the <b>rVerts</b> array so as to
		not waste space. The way 3ds Max uses the mesh = operator assumes that
		the <b>rVerts</b> are not copied, and thus developers must do the
		copying themselves.
		\par Parameters:
		<b>const Mesh\& fromMesh</b>\n\n
		Specifies the mesh to copy. */
		DllExport Mesh& operator=(const Mesh& fromMesh);
		
		/*! \remarks Sets the number of geometric vertices in the mesh.
		\par Parameters:
		<b>int ct</b>\n\n
		Specifies the number of vertices.\n\n
		<b>BOOL keep=FALSE</b>\n\n
		Specifies if the previous vertices should be kept. If TRUE the previous
		vertices are kept; otherwise they are discarded.\n\n
		<b>BOOL synchSel=TRUE</b>\n\n
		This parameter is available in release 2.0 and later only.\n\n
		If TRUE the selection set BitArrays are resized to fit the number of
		vertices; otherwise they are left unaltered.
		\return  TRUE if storage was allocated and the number of vertices was
		set; otherwise FALSE. */
		DllExport BOOL 	setNumVerts(int ct, BOOL keep=FALSE, BOOL synchSel=TRUE);
		/*! \remarks Returns the number of vertices. */
		int				getNumVerts(void) const	{ return numVerts; }
		
		/*! \remarks Sets the number of faces in the mesh.
		\par Parameters:
		<b>int ct</b>\n\n
		Specifies the number of faces.\n\n
		<b>BOOL keep=FALSE</b>\n\n
		Specifies if the previous faces should be kept. If TRUE the previous
		faces are kept; otherwise they are discarded.\n\n
		<b>BOOL synchSel=TRUE</b>\n\n
		This parameter is available in release 2.0 and later only.\n\n
		If TRUE the selection set BitArrays are resized to fit the number of
		faces; otherwise they are left unaltered.
		\return  TRUE if storage was allocated and the number of faces was set;
		otherwise FALSE. */
		DllExport BOOL	setNumFaces(int ct, BOOL keep=FALSE, BOOL synchSel=TRUE);
		/*! \remarks Returns the number of faces in the mesh. */
		int				getNumFaces(void) const{ return numFaces; }
		
		// Original mapping coordinates (map channel 1)
		/*! \remarks Sets the number of texture vertices (in mapping channel
		1).
		\par Parameters:
		<b>int ct</b>\n\n
		Specifies the number of texture vertices.\n\n
		<b>BOOL keep=FALSE</b>\n\n
		Specifies if the previous texture vertices should be kept. If TRUE the
		previous texture vertices are kept; otherwise they are discarded.
		\return  TRUE if storage was allocated and the number of texture
		vertices was set; otherwise FALSE. */
		DllExport BOOL	setNumTVerts(int ct, BOOL keep=FALSE);
		/*! \remarks Returns the number of texture vertices (in mapping
		channel 1). */
		int				getNumTVerts(void) const { return numTVerts; }
		/*! \remarks Sets the number of TVFaces. This method is automatically
		called if you set the number of faces to keep these two in sync
		(because the number of TVFaces should be the same as the number of
		faces). The following rules apply:\n\n
		If you have no TVFaces and <b>keep</b> is TRUE then the TVFaces array
		stays empty.\n\n
		If you have no TVFaces and <b>keep</b> is FALSE they are allocated.\n\n
		If you have TVFaces and ct = 0 then the TVFaces are freed.
		\par Parameters:
		<b>int ct</b>\n\n
		The number of TVFaces.\n\n
		<b>BOOL keep=FALSE</b>\n\n
		Specifies if the old faces should be kept.\n\n
		<b>int oldCt=0</b>\n\n
		The length of the existing TVFaces array.
		\return  TRUE if storage has been allocated and the number is set;
		otherwise FALSE. */
		DllExport BOOL 	setNumTVFaces(int ct, BOOL keep=FALSE, int oldCt=0);

		// Color per vertex array (map channel 0)
		// these methods only affect the vertColArray, even when the vertex colors
		// come from a different array (as set by the setCVertArray method below)
		/*! \remarks This method is available in release 2.0 and later
		only.\n\n
		Sets the number of color per vertex vertices.
		\par Parameters:
		<b>int ct</b>\n\n
		The number of color vertices to set.\n\n
		<b>BOOL keep=FALSE</b>\n\n
		If TRUE previous values are kept; otherwise they are discarded.
		\return  TRUE if the value was set; otherwise FALSE. */
		DllExport BOOL 	setNumVertCol(int ct,BOOL keep=FALSE);
		/*! \remarks This method is available in release 2.0 and later
		only.\n\n
		Returns the number of color per vertex vertices. */
		int             getNumVertCol() const {return numCVerts;}
		/*! \remarks This method is available in release 2.0 and later
		only.\n\n
		Sets the number of color per vertex faces.
		\par Parameters:
		<b>int ct;</b>\n\n
		The number of color per vertex faces to set.\n\n
		<b>BOOL keep=FALSE</b>\n\n
		Specifies if the old faces should be kept if the array is being
		resized. If FALSE they are freed.\n\n
		<b>int oldCt=0</b>\n\n
		The length of the existing VCFaces array.
		\return  TRUE if storage has been allocated and the number is set;
		otherwise FALSE. */
		DllExport BOOL 	setNumVCFaces(int ct, BOOL keep=FALSE, int oldCt=0);

		// To use a different source array for displaying vertex color data:
		//  -- to use a different map channel, call with args: mapChanNum, NULL, NULL
		//  -- to use an external array, call with: MESH_USE_EXT_CVARRAY, vcArray, face_data_if_available
		//     (if no face array is supplied, then we will use the internal vertex color face array)
		//  -- to revert to the internal color vert array, call with "0 , NULL, NULL" (or no args)

		// This method would typically be called right before display, as with a node display callback, 
		// or through an extension object.
		/*! \remarks This method is available in release 4.0 and later
		only.\n\n
		This method would typically be called right before display, as with a
		node display callback, or through an extension object. If mapChan
		parameter is set to MESH_USE_EXT_CVARRAY then the data in VCArray and
		TVFace is stored for internal use and consequent drawing. If the arrays
		are NULL then the internal source is used.\n\n

		\par Parameters:
		<b>int mapChan</b>\n\n
		the mapping channel to use.\n\n
		<b>VertColor * VCArray</b>\n\n
		An external array hosting the vertex colors\n\n
		<b>TVFace * vcf</b>\n\n
		An external array of TVFace indexing into the color array\n\n
		  */
		DllExport void	setVCDisplayData(int mapChan = 0, VertColor *VCArray=NULL, TVFace *VCf=NULL);

		// For mp in following: 0=vert colors, 1=original TVerts, 2&up = new map channels
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Set the number of texture maps used by this Mesh. Note that this call
		is made automatically if <b>Mesh::setMapSupport()</b> is called.
		\par Parameters:
		<b>int ct</b>\n\n
		The number of texture maps to use. This is a value between 2 and
		<b>MAX_MESHMAPS-1</b>.\n\n
		<b>BOOL keep=FALSE</b>\n\n
		TRUE to keep the old mapping information after the resize; FALSE to
		discard it. */
		DllExport void setNumMaps (int ct, BOOL keep=FALSE);
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Returns the number of mapping channels in use. */
		int getNumMaps () const { return numMaps; }

		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Returns TRUE if the specified mapping channel is supported; otherwise
		FALSE.
		\par Parameters:
		<b>int mp</b>\n\n
		Specifies the channel. See
		<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_mapping_channel_index_values.html">List of
		Mapping Channel Index Values</a>. */
		DllExport BOOL mapSupport (int mp) const;
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Sets whether the specified mapping channels is supported or not.
		\par Parameters:
		<b>int mp</b>\n\n
		Specifies the channel. See
		<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_mapping_channel_index_values.html">List of
		Mapping Channel Index Values</a>.\n\n
		<b>BOOL support=TRUE</b>\n\n
		TRUE to indicate the channel is supported; otherwise FALSE. */
		DllExport void setMapSupport (int mp, BOOL support=TRUE);
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Sets the number of texture or vertex color vertices for the specified
		mapping channel of this mesh.
		\par Parameters:
		<b>int mp</b>\n\n
		Specifies the channel. See
		<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_mapping_channel_index_values.html">List of
		Mapping Channel Index Values</a>.\n\n
		<b>int ct</b>\n\n
		The number of vertices to allocate.\n\n
		<b>BOOL keep=FALSE</b>\n\n
		If TRUE previous values are kept; otherwise they are discarded. */
		DllExport void setNumMapVerts (int mp, int ct, BOOL keep=FALSE);
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Returns the number of texture or vertex color verticies for the
		specified channel of this mesh.
		\par Parameters:
		<b>int mp</b>\n\n
		Specifies the channel. See
		<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_mapping_channel_index_values.html">List of
		Mapping Channel Index Values</a>. */
		DllExport int getNumMapVerts (int mp) const;
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Sets the number of texture or vertex color faces for the specified
		channel of this mesh.
		\par Parameters:
		<b>int mp</b>\n\n
		Specifies the channel. See
		<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_mapping_channel_index_values.html">List of
		Mapping Channel Index Values</a>.\n\n
		<b>int ct</b>\n\n
		The number of faces to allocate.\n\n
		<b>BOOL keep=FALSE</b>\n\n
		If TRUE previous values are kept; otherwise they are discarded. */
		DllExport void setNumMapFaces (int mp, int ct, BOOL keep=FALSE, int oldCt=0);

		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Returns a pointer to the list of <b>UVVert</b>s for the specified
		channel of this mesh.
		\par Parameters:
		<b>int mp</b>\n\n
		Specifies the channel. See
		<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_mapping_channel_index_values.html">List of
		Mapping Channel Index Values</a>. */
		DllExport UVVert *mapVerts (int mp) const;
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Returns a pointer to the list of <b>TVFace</b>s for the specified
		channel of this mesh.
		\par Parameters:
		<b>int mp</b>\n\n
		Specifies the channel. See
		<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_mapping_channel_index_values.html">List of
		Mapping Channel Index Values</a>. */
		DllExport TVFace *mapFaces (int mp) const;
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Sets a single texture or vertex color value for the specified channel
		of this mesh.
		\par Parameters:
		<b>int mp</b>\n\n
		Specifies the channel. See
		<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_mapping_channel_index_values.html">List of
		Mapping Channel Index Values</a>.\n\n
		<b>int i</b>\n\n
		The zero based index of the vertex to set.\n\n
		<b>const UVVert\&xyz</b>\n\n
		The value to set. */
		void		setMapVert (int mp, int i, const UVVert&xyz) { if (mapVerts(mp)) mapVerts(mp)[i] = xyz; }
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Applies a simple planar mapping to the specified channel. This is done
		by copying the mesh topology and vertex locations into the map.
		\par Parameters:
		<b>int mp</b>\n\n
		Specifies the channel. See
		<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_mapping_channel_index_values.html">List of
		Mapping Channel Index Values</a>. */
		DllExport void MakeMapPlanar (int mp);	// Copies mesh topology, vert locations into map.
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Returns a BitArray with a bit set for each isolated vertex
		(un-referenced by any face) for the specified channel.
		\par Parameters:
		<b>int mp</b>\n\n
		Specifies the channel. See
		<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_mapping_channel_index_values.html">List of
		Mapping Channel Index Values</a>. */
		DllExport BitArray GetIsoMapVerts (int mp);
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Deletes the map vertices indicated.
		\par Parameters:
		<b>int mp</b>\n\n
		Specifies the channel. See
		<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_mapping_channel_index_values.html">List of
		Mapping Channel Index Values</a>.\n\n
		<b>BitArray set</b>\n\n
		Indicates which map verts should be deleted. set.GetSize() should equal
		this mesh's getNumMapVerts(mp).\n\n
		<b>BitArray *fdel=NULL</b>\n\n
		If non-NULL, this points to a BitArray that will be filled in with the
		faces that will need to be deleted or have new map verts assigned
		because they used a map vert that was deleted. (The size will be set to
		this mesh's <b>numFaces</b>.) */
		DllExport void DeleteMapVertSet (int mp, BitArray set, BitArray *fdel=NULL);
		/*! \remarks This method is available in release 4.0 and later
		only.\n\n
		This method deletes each isolated vertex (un-referenced by any face)
		for the all active maps. */
		DllExport void DeleteIsoMapVerts ();	//	 do all active maps
		/*! \remarks This method is available in release 4.0 and later
		only.\n\n
		This method deletes each isolated vertex (un-referenced by any face)
		for the specified channel.
		\par Parameters:
		<b>int mp=-1</b>\n\n
		Specifies the channel. See
		<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_mapping_channel_index_values.html">List of
		Mapping Channel Index Values</a>. The default value of -1 indicates to
		do all active maps. */
		DllExport void DeleteIsoMapVerts (int mp);
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Deallocates the texture or vertex color vertices for the specified
		channel of this mesh.
		\par Parameters:
		<b>int mp</b>\n\n
		Specifies the channel. See
		<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_mapping_channel_index_values.html">List of
		Mapping Channel Index Values</a>. */
		DllExport void freeMapVerts (int mp);
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Deallocates the texture or vertex color faces for the specified channel
		of this mesh.
		\par Parameters:
		<b>int mp</b>\n\n
		Specifies the channel. See
		<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_mapping_channel_index_values.html">List of
		Mapping Channel Index Values</a>. */
		DllExport void freeMapFaces (int mp);
		/*! \remarks This method is available in release 4.0 and later
		only.\n\n
		This method returns the map for the specified map channel.
		\par Parameters:
		<b>int mp</b>\n\n
		The map channel.
		\par Default Implementation:
		<b>{ return (mp\<0) ? hmaps[-1-mp] : maps[mp]; }</b> */
		MeshMap & Map(int mp) { return (mp<0) ? hmaps[-1-mp] : maps[mp]; }

		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Sets the number of channels of vertex data used by the mesh.
		\par Parameters:
		<b>int ct</b>\n\n
		The number of elements of vertex data to set.\n\n
		<b>BOOL keep=FALSE</b>\n\n
		If TRUE any old vertex data is kept; otherwise it is discarded. */
		DllExport void setNumVData (int ct, BOOL keep=FALSE);
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Returns the number of vertex data channels maintained by this mesh. */
		int getNumVData () const { return vdSupport.GetSize(); }

		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Returns TRUE if the specified channel of vertex data is available for
		this mesh; otherwise FALSE.
		\par Parameters:
		<b>int vd</b>\n\n
		The vertex data channel. See
		<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_vertex_data_index_options.html">List of Vertex
		Data Index Options</a>. */
		DllExport BOOL vDataSupport (int vd) const;
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Sets if the specified channel of vertex data is supported by this mesh.
		\par Parameters:
		<b>int vd</b>\n\n
		The vertex data channel. See
		<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_vertex_data_index_options.html">List of Vertex
		Data Index Options</a>.\n\n
		<b>BOOL support=TRUE</b>\n\n
		TRUE to indicate the channel is supported; FALSE to indicate it's not.
		If TRUE is specified then <b>numVerts</b> elements are allocated (if
		needed). If FALSE is specified the data for the channel is freed. */
		DllExport void setVDataSupport (int vd, BOOL support=TRUE);
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Returns a pointer to the vertex data for the specified channel or NULL
		if the channel is not supported. If supported then the size of this
		array is <b>numVerts</b>.
		\par Parameters:
		<b>int vd</b>\n\n
		The vertex data channel. See
		<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_vertex_data_index_options.html">List of Vertex
		Data Index Options</a>. */
		void *vertexData (int vd) const { return vDataSupport(vd) ? vData[vd].data : NULL; }
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Returns a pointer to the floating point vertex data for the specified
		channel of this mesh or NULL if the channel is not supported. If
		supported then the size of this array is <b>numVerts</b>.
		\par Parameters:
		<b>int vd</b>\n\n
		The vertex data channel. See
		<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_vertex_data_index_options.html">List of Vertex
		Data Index Options</a>. */
		float *vertexFloat (int vd) const { return (float *) vertexData (vd); }
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Deallocates the vertex data for the specified chanel.
		\par Parameters:
		<b>int vd</b>\n\n
		The vertex data channel. See
		<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_vertex_data_index_options.html">List of Vertex
		Data Index Options</a>. */
		DllExport void freeVData (int vd);
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Deallocates the vertex data from all the channels and sets the number
		of supported channels to 0. */
		DllExport void freeAllVData ();

		// Two specific vertex scalars.
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Returns a pointer to the floating point vertex weight data. */
		float *getVertexWeights () { return vertexFloat(VDATA_WEIGHT); }
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Sets the channel support for the vertex weights channel
		(<b>VDATA_WEIGHT</b>). */
		void SupportVertexWeights () { setVDataSupport (VDATA_WEIGHT); }
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Clears (deallocates) the vertex weights channel data. */
		void ClearVertexWeights() { setVDataSupport (VDATA_WEIGHT, FALSE); }
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Deallocates the vertex weights channel data (same as
		<b>ClearVertexWeights()</b> above). */
		void freeVertexWeights () { freeVData (VDATA_WEIGHT); }
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Returns a pointer to the floating point vertex selection weights data.
		*/
		float *getVSelectionWeights () { return vertexFloat(VDATA_SELECT); }
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Sets the channel support for the vertex weights channel
		(<b>VDATA_SELECT</b>). */
		void SupportVSelectionWeights () { setVDataSupport (VDATA_SELECT); }
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Clears (deallocates) the vertex selection weights channel data. */
		void ClearVSelectionWeights() { setVDataSupport (VDATA_SELECT, FALSE); }
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Deallocates the vertex selection weights channel data (same as
		<b>ClearVSelectionWeights()</b> above). */
		void freeVSelectionWeights () { freeVData (VDATA_SELECT); }

		// these flags are restricted to 4 bits and force the topology (strips & edges)
		// to be invalidated when they change.  Used by primitives with smoothing checkboxes
		/*! \remarks This method is available in release 2.0 and later
		only.\n\n
		This method should be called when the user has clicked on the 'Smooth'
		check box in a procedural object. It invalidates the appropriate caches
		of the mesh so the display is updated properly. If this method is not
		called, the internal topology cache might prevent the mesh from
		appearing changed.
		\par Parameters:
		<b>int f</b>\n\n
		Nonzero indicates smoothed; zero unsmoothed. */
		DllExport void  setSmoothFlags(int f);
		/*! \remarks This method is available in release 2.0 and later
		only.\n\n
		Returns the state of the smooth flags. See <b>setSmoothFlags()</b>
		above. */
		DllExport int   getSmoothFlags();

		/*! \remarks Sets a single vertex in the <b>verts</b> array.
		\par Parameters:
		<b>int i</b>\n\n
		A zero based index into the <b>verts</b> array of the vertex to
		store.\n\n
		<b>const Point3 \&xyz</b>\n\n
		Specifies the coordinate of the vertex. */
		void		setVert(int i, const Point3 &xyz)	{ verts[i] = xyz; }
		/*! \remarks Sets a single vertex in the <b>verts</b> array.
		\par Parameters:
		<b>int i</b>\n\n
		A zero based index into the <b>verts</b> array of the vertex to
		store.\n\n
		<b>float x</b>\n\n
		Specifies the X coordinate of the vertex.\n\n
		<b>float y</b>\n\n
		Specifies the Y coordinate of the vertex.\n\n
		<b>float z</b>\n\n
		Specifies the Z coordinate of the vertex. */
		void		setVert(int i, float x, float y, float z)	{ verts[i].x=x; verts[i].y=y; verts[i].z=z; }
		/*! \remarks Sets a single texture vertex in the <b>tVerts</b> array.
		\par Parameters:
		<b>int i</b>\n\n
		A zero based index into the <b>tVerts</b> array of the texture vertex
		to store.\n\n
		<b>const UVVert \&xyz</b>\n\n
		Specifies the coordinate of the vertex. */
		void		setTVert(int i, const UVVert &xyz)	{ tVerts[i] = xyz; }
		/*! \remarks Sets a single texture vertex in the <b>tVerts</b> array.
		\par Parameters:
		<b>int i</b>\n\n
		A zero based index into the <b>tVerts</b> array of the texture vertex
		to store.\n\n
		<b>float x</b>\n\n
		Specifies the X coordinate of the texture vertex.\n\n
		<b>float y</b>\n\n
		Specifies the Y coordinate of the texture vertex.\n\n
		<b>float z</b>\n\n
		Specifies the Z coordinate of the texture vertex. */
		void		setTVert(int i, float x, float y, float z)	{ tVerts[i].x=x; tVerts[i].y=y; tVerts[i].z=z; }
		
		/*! \remarks Sets a single 'rendered' normal in the <b>rVerts</b>
		array of <b>RVertex</b> instances.
		\par Parameters:
		<b>int i</b>\n\n
		A zero based index into the <b>rVerts</b> array of the normal to
		store.\n\n
		<b>const Point3 \&xyz</b>\n\n
		The normal to store in device coordinates. This should be a unit
		vector. */
		DllExport void		setNormal(int i, const Point3 &xyz); 
		/*! \remarks Returns the 'i-th' 'rendered' normal from the
		<b>rVerts</b> array.
		\par Parameters:
		<b>int i</b>\n\n
		A zero based index into the <b>rVerts</b> array of the normal to get.
		*/
		DllExport Point3 &	getNormal(int i) const; // mjm - made const - 2.16.99

		/*! \remarks Sets the 'i-th' face normal.
		\par Parameters:
		<b>int i</b>\n\n
		A zero based index into the face normal array of the normal to
		store.\n\n
		<b>const Point3 \&xyz</b>\n\n
		The face normal to store. This should be a unit vector. */
		void		setFaceNormal(int i, const Point3 &xyz) { faceNormal[i] =  xyz; }
		/*! \remarks Returns the 'i-th' face normal.
		\par Parameters:
		<b>int i</b>\n\n
		Specifies the index of the face normal to retrieve. */
		Point3 &	getFaceNormal(int i) { return faceNormal[i]; }

		/*! \remarks Returns the 'i-th' vertex.
		\par Parameters:
		<b>int i</b>\n\n
		Specifies the index of the vertex to retrieve. */
		Point3 &	getVert(int i)		{ return verts[i];  }
		/*! \remarks Returns a pointer to the 'i-th' vertex.
		\par Parameters:
		<b>int i</b>\n\n
		Specifies the index of the vertex address to retrieve. */
		Point3 *	getVertPtr(int i)	{ return verts+i; }
		/*! \remarks Returns the 'i-th' texture vertex.
		\par Parameters:
		<b>int i</b>\n\n
		Specifies the index of the texture vertex to retrieve. */
		UVVert &	getTVert(int i)		{ return tVerts[i];  }
		/*! \remarks Returns a pointer to the 'i-th' texture vertex.
		\par Parameters:
		<b>int i</b>\n\n
		Specifies the index of the texture vertex address to retrieve. */
		UVVert *	getTVertPtr(int i)	{ return tVerts+i; }
		/*! \remarks This method returns the 'i-th' RVertex.
		\par Parameters:
		<b>int i</b>\n\n
		Specifies the index of the RVertex to retrieve. */
		RVertex &	getRVert(int i)		{ return rVerts[i]; }
		/*! \remarks This method returns a pointer to the 'i-th' RVertex.
		\par Parameters:
		<b>int i</b>\n\n
		Specifies the index of the RVertex to retrieve. */
		RVertex *	getRVertPtr(int i)	{ return rVerts+i; }
		
		/*! \remarks This method is no longer used. */
		void		setMtlIndex(MtlID	i)	{ mtlIndex = i; }
		/*! \remarks This method is no longer used. */
		MtlID		getMtlIndex(void) 		{ return mtlIndex; }

		// Face MtlIndex access methods;
	    /*! \remarks Retrieves the zero based material index of the 'i-th'
	    face.
	    \par Parameters:
	    <b>int i</b>\n\n
	    Specifies the face index. This is the zero based index into the
	    <b>faces</b> array. */
	    DllExport MtlID		getFaceMtlIndex(int i);
		/*! \remarks Sets the material index of the 'i-th' face.
		\par Parameters:
		<b>int i</b>\n\n
		Specifies the face index. This is the zero based index into the
		<b>faces</b> array.\n\n
		<b>MtlID id</b>\n\n
		The material index for the 'i-th' face. */
		DllExport void		setFaceMtlIndex(int i, MtlID id); 	
		
		/*! \remarks This method resolves the normals on the <b>RVertex</b>
		array. If the Mesh already has normals at each vertex, the normal is
		just moved to the <b>RVertex</b> array. See
		Class RVertex and
		Class RNormal.\n\n
		If you are creating a <b>Mesh</b> by hand, after you are done
		specifying all the vertices and faces, this method should be called.
		This allocates the <b>RVertex</b> and <b>RNormal</b> database for the
		<b>Mesh</b>. This will allow you to query the <b>Mesh</b> and ask about
		normals on the vertices. Also, if you deform a Mesh (i.e. take one of
		the vertices and move it), you should call this method again. Actually,
		if you are only moving one normal you only need to smooth the polygons
		that share the vertex. However, there is no method to smooth a subset
		of a <b>Mesh</b>, you either have to do it by hand or call this method
		to smooth the entire <b>Mesh</b>.\n\n
		This method also builds the face normals for the mesh. */
		DllExport void		buildNormals();			// calcs face and vertex normals
		/*! \remarks This method is similar to <b>buildNormals()</b> above,
		but ignores the material index (<b>mtlIndex</b>). In other words, the
		difference between this and <b>buildNormals()</b> is that it doesn't
		look at the <b>mtlIndex</b> of the faces: normals of faces with the
		same smoothing group are averaged regardless. */
		DllExport void 		buildRenderNormals();	// like buildNormals, but ignores mtlIndex
		// checkNormals can be used to build the normals and allocate RVert space 
		// only if necessary.  This is a very cheap call if the normals are already calculated.
		// When illum is FALSE, only the RVerts allocation is checked (since normals aren't
		// needed for non-illum rendering).  When illum is TRUE, normals will also be built, if
		// they aren't already.  So, to make sure normals are built, call this with illum=TRUE.
		/*! \remarks This method can be used to build the normals and allocate
		<b>RVert</b> space only if necessary. This is a very inexpensive call
		if the normals are already calculated. When <b>illum</b> is FALSE, only
		the <b>RVerts</b> allocation is checked (since normals aren't needed
		for non-illum rendering). When <b>illum</b> is TRUE, normals will also
		be built, if they aren't already. So, to make sure normals are built,
		call this with <b>illum=TRUE</b>.
		\par Parameters:
		<b>BOOL illum</b>\n\n
		If TRUE then normals are built. If FALSE then only the <b>RVert</b>
		array is allocated. */
		DllExport void		checkNormals(BOOL illum);

		/*! \remarks Renders this Mesh using the specified graphics window and
		array of materials.\n\n
		Note: If a display routine makes multiple calls to this method you need
		to have called:\n\n
		<b>gw-\>setMaterial(inode-\>Mtls()[0]);</b>\n\n
		before calling <b>Mesh::render()</b>. If you don't then you may get the
		wrong material for material ID 0.
		\par Parameters:
		<b>GraphicsWindow *gw</b>\n\n
		Points to the graphics window to render to.\n\n
		<b>Material *ma</b>\n\n
		The list of materials to use to render the mesh. 
		See Class Material, <a href="class_i_node.html#A_GM_inode_materials">Class INode - Material
		methods</a>.\n\n
		<b>RECT *rp</b>\n\n
		Specifies the rectangular region to render. If the mesh should be
		rendered to the entire viewport pass NULL.\n\n
		<b>int compFlags</b>\n\n
		One or more of the following flags:\n\n
		<b>COMP_TRANSFORM</b>\n\n
		Forces recalculation of the model to screen transformation; otherwise
		attempt to use the cache.\n\n
		<b>COMP_IGN_RECT</b>\n\n
		Forces all polygons to be rendered; otherwise only those intersecting
		the box will be rendered.\n\n
		<b>COMP_LIGHTING</b>\n\n
		Forces re-lighting of all vertices (as when a light moves); otherwise
		only re-light moved vertices\n\n
		<b>COMP_ALL</b>\n\n
		All of the above flags.\n\n
		<b>COMP_OBJSELECTED</b>\n\n
		If this bit is set then the node being displayed by this mesh is
		selected. Certain display flags only activate when this bit is set.\n\n
		<b>int numMat=1</b>\n\n
		The number of materials for the mesh.\n\n
		<b>InterfaceServer *pi = NULL</b>\n\n
		This pointer to an InterfaceServer can be used to get hold of the
		IXTCAccess pointer.\n\n
		And IXTCAccess interface can also be obtained from the object by
		calling <b>Object::GetInterface</b>\n\n
		<b>(IXTCACCESS_INTERFACE_ID)</b>.
		\par Sample Code:
		The following code shows this method being used to render the mesh as
		part of the <b>BaseObject::Display()</b> method:\n\n
		\code
		int SimpleObject::Display(TimeValue t, INode* inode, ViewExp *vpt, int flags)
		{
			if (!OKtoDisplay(t)) 
				return 0;
			GraphicsWindow *gw = vpt->getGW();
			Matrix3 mat = inode->GetObjectTM(t);
			UpdateMesh(t);								  // UpdateMesh() just calls BuildMesh()
			gw->setTransform(mat);
			mesh.render(gw, inode->Mtls(), (flags&USE_DAMAGE_RECT) ? &vpt->GetDammageRect() : NULL, COMP_ALL, inode->NumMtls());
			return(0);
		}
		\endcode */
		DllExport void		render(GraphicsWindow *gw, Material *ma, RECT *rp, int compFlags, int numMat=1, InterfaceServer *pi = NULL);
		/*! \remarks Checks the given HitRecord <b>hr</b> to see if it
		intersects this Mesh object.
		\par Parameters:
		<b>GraphicsWindow *gw</b>\n\n
		Points to the graphics window to check.\n\n
		<b>Material *ma</b>\n\n
		The list of materials for the mesh.\n\n
		<b>HitRegion *hr</b>\n\n
		This describes the properties of a region used for the hit testing. See
		Class HitRegion.\n\n
		<b>int abortOnHit = FALSE</b>\n\n
		If nonzero, the hit testing is complete after any hit; otherwise all
		hits are checked.\n\n
		<b>int numMat=1</b>\n\n
		The number of materials for the mesh.
		\return  TRUE if the item was hit; otherwise FALSE. */
		DllExport BOOL		select(GraphicsWindow *gw, Material *ma, HitRegion *hr, int abortOnHit = FALSE, int numMat=1);
		/*! \remarks Checks to see if there is a snap point near the given
		mouse point.
		\par Parameters:
		<b>GraphicsWindow *gw</b>\n\n
		The graphics window in which to check.\n\n
		<b>SnapInfo *snap</b>\n\n
		This structure describes the snap settings used, and the results of the
		snap test. See <a href="struct_snap_info.html">Structure
		SnapInfo</a>.\n\n
		<b>IPoint2 *p</b>\n\n
		The mouse point to check.\n\n
		<b>Matrix3 \&tm</b>\n\n
		The object transformation matrix. This is the transformation to place
		the object into the world coordinate system.
		\par Sample Code:
		<b>// Checks to see if there is a snap point near the given mouse
		point.</b>\n\n
		\code
		void TestObject::Snap(TimeValue t, INode* inode, SnapInfo *snap, IPoint2 *p, ViewExp *vpt)
		{
		// Grab the object TM
			Matrix3 tm = inode->GetObjectTM(t);
		// Grab the graphics window
			GraphicsWindow *gw = vpt->getGW();
		// Make sure our mesh is up to date
			UpdateMesh(t);		  // UpdateMesh() just calls BuildMesh()
		// Set the transform in the GW
			gw->setTransform(tm);
		// Let the mesh do the work...
			mesh.snap( gw, snap, p, tm );
		}
		\endcode */
		DllExport void		snap(GraphicsWindow *gw, SnapInfo *snap, IPoint2 *p, Matrix3 &tm);
		/*! \remarks This method
		may be called to perform sub-object hit testing on this mesh.
		\par Parameters:
		<b>GraphicsWindow *gw</b>\n\n
		The graphics window associated with the viewport the mesh is being hit tested
		in.\n\n
		<b>Material *ma</b>\n\n
		The list of materials for the mesh. See Class Material, <a href="class_i_node.html#A_GM_inode_materials">Class INode - Material methods</a>.\n\n
		<b>HitRegion *hr</b>\n\n
		This describes the properties of a region used for the hit testing. See
		Class HitRegion.\n\n
		<b>DWORD flags</b>\n\n
		Flags for sub object hit testing. One or more of the following values:\n\n
		<b>SUBHIT_SELONLY</b>\n\n
		Selected only.\n\n
		<b>SUBHIT_UNSELONLY</b>\n\n
		Unselected only.\n\n
		<b>SUBHIT_ABORTONHIT</b>\n\n
		Abort hit testing on the first hit found.\n\n
		<b>SUBHIT_SELSOLID</b>\n\n
		This treats selected items as solid and unselected items as not solid. Treating
		an item as solid means the face will be hit if the mouse is anywhere inside the
		face region and not just over a visible edge.\n\n
		<b>SUBHIT_USEFACESEL</b>\n\n
		When this bit is set, the sel only and unsel only tests will use the faces
		selection when doing a vertex level hit test\n\n
		<b>SUBHIT_VERTS</b>\n\n
		Hit test vertices.\n\n
		<b>SUBHIT_FACES</b>\n\n
		Hit test faces.\n\n
		<b>SUBHIT_EDGES</b>\n\n
		Hit test edges.\n\n
		<b>SubObjHitList\& hitList</b>\n\n
		The results are stored here. See Class SubObjHitList.\n\n
		<b>int numMat=1</b>\n\n
		The number of materials for the mesh.
		\return  TRUE if the item was hit; otherwise FALSE. */
		DllExport BOOL 		SubObjectHitTest(GraphicsWindow *gw, Material *ma, HitRegion *hr,
								DWORD flags, SubObjHitList& hitList, int numMat=1 );

		/*! \remarks Controls the display of surface normals on the mesh
		object.\n\n
		Note that there may be more than one normal per vertex if faces that
		share the vertex are in non-overlapping smoothing groups. In this case,
		all normals associated with the given vertex are drawn.
		\par Parameters:
		<b>int b</b>\n\n
		Nonzero to display the normals; zero to turn off normals display.\n\n
		This can be a combination of <b>MESH_DISP_FACE_NORMALS</b> and
		<b>MESH_DISP_VERTEX_NORMALS</b>. (The arguments may be or'ed together
		to display both.) For backwards compatibility,
		<b>MESH_DISP_FACE_NORMALS</b> is #define'd to be "1", so sending in
		"TRUE" will turn on display of face normals, as before.\n\n
		<b>float sc</b>\n\n
		This specifies the length that should be used (in world units) to
		display the normals. Since all normals start out with length =1 they
		probably would be too small to see unless they were scaled */
		void		displayNormals(int b, float sc)	{ dspNormals = b; if(sc != (float)0.0) norScale = sc; }
		/*! \remarks Controls the display of hidden edges of this mesh object.
		This just sets the Boolean in the mesh that controls whether "hidden"
		edges (for instance the diagonals on the sides of a cube) are
		displayed.
		\par Parameters:
		<b>int b</b>\n\n
		Nonzero to display all the hidden edges; otherwise zero. */
		void		displayAllEdges(int b)	{ dspAllEdges = b; }
		/*! \remarks Computes the bounding box of the Mesh. If surface normals
		are displayed, they are taken into account in the computation of the
		box. The bounding box is stored with the Mesh object, use
		<b>getBoundingBox()</b> to retrieve it. */
		DllExport void		buildBoundingBox(void);
		/*! \remarks Retrieves the bounding box of the mesh object.
		\par Parameters:
		<b>Matrix3 *tm=NULL</b>\n\n
		The optional TM allows the box to be calculated in any space. NOTE:
		This computation will be slower because all the points must be
		transformed.
		\return  The bounding box of the Mesh. */
		DllExport Box3 		getBoundingBox(Matrix3 *tm=NULL); // RB: optional TM allows the box to be calculated in any space.
		                                              // NOTE: this will be slower becuase all the points must be transformed.
		// Cache invalidation
		/*! \remarks Call this method after the geometry of this Mesh has
		changed. It invalidates the bounding box, and tosses out the cached
		normals and edge list. */
		DllExport void 		InvalidateGeomCache();
		/*! \remarks Call this method after you alter vertex or face lists or
		revise edge visibility flags. It will invalidate the edge and strip
		database maintained by the mesh. */
		DllExport void 		InvalidateTopologyCache();
		/*! \remarks Cleans up the allocated arrays. This frees the Faces,
		Verts, TVerts, TVFaces, FaceMtlIndexList and invalidates the geometry
		cache. */
		DllExport void 		FreeAll(); //DS
		DllExport void      ZeroTopologyCache(); // RB set pointers to NULL but don't delete from mem.

		// edge list functions		
		/*! \remarks This method is used internally. */
		DllExport void		EnableEdgeList(int e);
		/*! \remarks This method is used internally. */
		DllExport void     	BuildVisEdgeList();
		/*! \remarks This is used internally. */
		DllExport void 		DrawVisEdgeList(GraphicsWindow *gw, DWORD flags);
		DllExport void 		Draw3DVisEdgeList(GraphicsWindow *gw, DWORD flags);
		/*! \remarks This is used internally. */
		DllExport void		HitTestVisEdgeList(GraphicsWindow *gw, int abortOnHit ); // RB
		/*! \remarks This is used internally. */
		DllExport void		InvalidateEdgeList(); // RB

		// strip functions				
		/*! \remarks This method is available in release 2.0 and later
		only.\n\n
		It builds the strips database inside the mesh. See the method
		<b>BuildStripsAndEdges()</b> below. */
		DllExport BOOL     	BuildStrips();
		/*! \remarks This method is used internally. */
		DllExport void		Stripify(Strip *s, StripData *sd, int vtx);
		DllExport void		Stripify(Strip *s, StripData *sd, int vtx,
									 DWTab	&v,	DWTab	&n,	DWTab	&tv);
		/*! \remarks This method is used internally. */
		DllExport void		getStripVertColor(GraphicsWindow *gw, int cv, int flipped, MtlID mID, DWORD smGroup, Point3 &rgb);
		/*! \remarks This method is used internally. */
		DllExport void		getStripNormal(int cv, MtlID mID, DWORD smGroup, Point3 &nor);
		DllExport int		getStripNormalIndex(int cv, MtlID mID, DWORD smGroup);
		/*! \remarks This method is used internally. */
		DllExport BOOL		getStripTVert(GraphicsWindow *gw, int cv, int ctv, Point3 &uvw, int texNum = 0);
		/*! \remarks This method is used internally. */
		DllExport void 		DrawStrips(GraphicsWindow *gw, Material *ma, int numMat);
		/*! \remarks This method is used internally. */
		DllExport void 		Draw3DStrips(GraphicsWindow *gw, Material *ma, int numMat);
		DllExport void 		Draw3DWireStrips(GraphicsWindow *gw, Material *ma, int numMat);
		/*! \remarks This method invalidates the strips database. */
		DllExport void		InvalidateStrips();

		/*! \remarks This method is available in release 2.0 and later
		only.\n\n
		It builds the strips and edges database inside the mesh. When
		developers create a new mesh for display then this method should be
		called. See the section on Stripping in the Advanced Topics section
		<a href="ms-its:3dsmaxsdk.chm::/meshes_root.html">Working with
		Meshes</a> for details on this method. */
		DllExport void		BuildStripsAndEdges();

		// functions for use in data flow evaluation
		/*! \remarks Makes a copy of the specified channels of the specified Mesh
		object's data structures (but not all the data in these structures) into this
		Mesh. For example the <b>verts, tVerts, tvFaces</b>, ... are not copied.
		\par Parameters:
		<b>Mesh *amesh</b>\n\n
		Specifies the source Mesh to copy.\n\n
		<b>ChannelMask channels</b>\n\n 
		Specifies the channels to copy. See \ref ObjectChannels. */
      DllExport void       ShallowCopy(Mesh *amesh, ChannelMask channels);
		/*! \remarks Makes a complete copy of the specified channels of the specified
		Mesh object (its data structures and all the data in these structures) into
		this Mesh.
		\par Parameters:
		<b>Mesh *amesh</b>\n\n
		Specifies the source Mesh to copy.\n\n
		<b>ChannelMask channels</b>\n\n
		Specifies the channels to copy. See \ref ObjectChannels. */
      DllExport void       DeepCopy(Mesh *amesh, ChannelMask channels);
		/*! \remarks This method replaces the specified channels with newly allocated
		copies.
		\par Parameters:
		<b>ULONG_PTRchannels</b>\n\n
		Specifies the channels to copy. See \ref ObjectChannels. */
      DllExport void       NewAndCopyChannels(ChannelMask channels);
		/*! \remarks Release the memory associated with the specified channels. For
		example if the TOPO_CHANNEL is specified the faces are freed, if the
		GEOM_CHANNEL is specified the vertices are freed, etc.
		\par Parameters:
		<b>ChannelMask channels</b>\n\n
		Specifies the channels to free. Channels not specified are left intact. See \ref ObjectChannels.\n\n
		<b>int zeroOthers=1</b>\n\n
		If nonzero then the various pointers are set to NULL and their counts are
		zeroed. For example <b>faces, verts, tVerts</b>, and <b>tvFace</b> are set to
		NULL and <b>numFaces, numVerts</b> and <b>numTVerts</b> are set to 0. If this
		is passed as 0, these pointers and counts are left unaltered. */
      DllExport void       FreeChannels(ChannelMask channels, int zeroOthers=1);

		// Mesh flags
		/*! \remarks Sets the state of the mesh flags.
		\par Parameters:
		<b>DWORD f</b>\n\n
		Specifies the flags to set. See
		<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_mesh_flags.html">List of Mesh Flags</a>. */
		void		SetFlag(DWORD f) { flags |= f; }
		/*! \remarks Returns the state of the specified mesh flags. See
		<b>Data Members:</b> above for a list of the mesh flags.
		\par Parameters:
		<b>DWORD f</b>\n\n
		Specifies the flags to retrieve. See
		<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_mesh_flags.html">List of Mesh Flags</a>.\n\n
		  */
		DWORD		GetFlag(DWORD f) { return flags & f; }
		/*! \remarks Sets the state of the specified mesh flags to 0.
		\par Parameters:
		<b>DWORD f</b>\n\n
		Specifies the flags to clear. See
		<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_mesh_flags.html">List of Mesh Flags</a>. */
		void		ClearFlag(DWORD f) { flags &= ~f; }

		// Display flags
		/*! \remarks Sets the state of the display flags. See <b>Data
		Members:</b> above for a list of the display flags.
		\par Parameters:
		<b>DWORD f</b>\n\n
		Specifies the flags to set. */
		void		SetDispFlag(DWORD f) { dispFlags |= f; }
		/*! \remarks Returns the state of the specified display flags. See
		<b>Data Members:</b> above for a list of the display flags.
		\par Parameters:
		<b>DWORD f</b>\n\n
		Specifies the flags to retrieve. */
		DWORD		GetDispFlag(DWORD f) { return dispFlags & f; }
		/*! \remarks Sets the state of the specified display flags to 0.
		\par Parameters:
		<b>DWORD f</b>\n\n
		Specifies the flags to clear. */
		void		ClearDispFlag(DWORD f) { dispFlags &= ~f; }

		// Selection access
		/*! \remarks Retrieves the bits representing the vertex selection
		status. See <b>Data Members:</b> above.
		\return  The vertex selection status. */
		BitArray& 	VertSel() { return vertSel;  }	
		/*! \remarks Retrieves the bits representing the face selection
		status. See <b>Data Members:</b> above.
		\return  The face selection status. */
		BitArray& 	FaceSel() { return faceSel;  }	

		// Constructs a vertex selection list based on the current selection level.
		/*! \remarks Constructs a vertex selection list based on the current
		selection level. For example if the selection level is at object level
		all the bits are set (vertices, faces and edges). If the selection
		level is at vertex level only the selected vertex bits are set.
		\return  A BitArray reflecting the current selection level.
		\par Sample Code:
		\code
		BitArray sel = mesh->VertexTempSel();
		for ( int i = 0; i < mesh->getNumVerts(); i++ )
		{
			if ( sel[i] ) {
				...
			}
		}
		\endcode */
		DllExport BitArray 	VertexTempSel();

		DllExport IOResult Save(ISave* isave);
		DllExport IOResult Load(ILoad* iload);

		// RB: added so all objects can easily support the GeomObject method of the same name.
		/*! \remarks Calculates the intersection of the specified ray with
		this mesh object. This allows Mesh objects to easily implement the
		<b>Object::IntersectRay()</b> method.
		\par Parameters:
		<b>Ray\& ray</b>\n\n
		Specifies the origin and direction of the ray to intersect with the
		mesh. See Class Ray.\n\n
		<b>float\& at</b>\n\n
		The computed point of intersection on the surface of the mesh.\n\n
		<b>Point3\& norm</b>\n\n
		The face normal at the point of intersection (<b>at</b>).
		\return  Nonzero if the ray intersected the mesh object; otherwise 0.
		Note that this method ignores backfaces when computing the result. */
		DllExport int IntersectRay(Ray& ray, float& at, Point3& norm);
		/*! \remarks This method is available in release 2.0 and later
		only.\n\n
		Calculates the intersection of the specified ray with this mesh object.
		This new version also returns the face index that was intersected and
		the barycentric coordinates of that face.\n\n
		Barycentric coordinates are the coordinates relative to the triangular
		face. The barycentric coordinates of a point <b>p</b> relative to a
		triangle describe that point as a weighted sum of the vertices of the
		triangle. If the barycentric coordinates are <b>b0</b>, <b>b1</b>, and
		<b>b2</b>, then:\n\n
		<b>p = b0*p0 + b1*p1 + b2*p2;</b>\n\n
		where <b>p0</b>, <b>p1</b>, and <b>p2</b> are the vertices of the
		triangle. The <b>Point3</b> returned by this method has the barycentric
		coordinates stored in its three coordinates. These coordinates are
		relative to the triangular face that was intersected. These barycentric
		coordinates can be used to interpolate any quantity whose value is
		known at the vertices of the triangle.
		\par Parameters:
		<b>Ray\& ray</b>\n\n
		Specifies the origin and direction of the ray to intersect with the
		mesh. See Class Ray.\n\n
		<b>float\& at</b>\n\n
		The computed point of intersection on the surface of the mesh.\n\n
		<b>Point3\& norm</b>\n\n
		The face normal at the point of intersection (<b>at</b>).\n\n
		<b>DWORD \&fi</b>\n\n
		The face index of the face that was intersected is returned here.\n\n
		<b>Point3 \&bary</b>\n\n
		The barycentric coordinates of the face that was hit.
		\return  Nonzero if the ray intersected the mesh object; otherwise 0.
		*/
		DllExport int IntersectRay(Ray& ray, float& at, Point3& norm, DWORD &fi, Point3 &bary);

		// RB: I couldn't resist adding these <g>
		/*! \remarks Performs a boolean union operation.
		\par Parameters:
		<b>Mesh \&mesh</b>\n\n
		Specifies the mesh to use as the other operand of the boolean
		operation.
		\return  A new Mesh resulting from the boolean operation. If the
		operation fails an empty Mesh is returned. */
		DllExport Mesh operator+(Mesh &mesh);  // Union
		/*! \remarks Performs a boolean difference operation.
		\par Parameters:
		<b>Mesh \&mesh</b>\n\n
		Specifies the mesh to use as the other operand of the boolean
		operation.
		\return  A new Mesh resulting from the boolean operation. If the
		operation fails an empty Mesh is returned. */
		DllExport Mesh operator-(Mesh &mesh);  // Difference
		/*! \remarks Performs a boolean intersection operation.
		\par Parameters:
		<b>Mesh \&mesh</b>\n\n
		Specifies the mesh to use as the other operand of the boolean
		operation.
		\return  A new Mesh resulting from the boolean operation. If the
		operation fails an empty Mesh is returned. */
		DllExport Mesh operator*(Mesh &mesh);  // Intersection
		DllExport void MyDebugPrint ();

		/*! \remarks This method is used internally. */
		DllExport void WeldCollinear(BitArray &set);

		/*! \remarks Allows this Mesh to be reduced in complexity by reducing
		the number of faces based on a surface normal threshold. Adjacent faces
		whose difference in surface normal angle falls below the specified
		threshold will be collapsed into a single triangle. The Mesh may also
		have its edge visibility set based on a surface normal threshold.
		\par Parameters:
		<b>float normThresh</b>\n\n
		When the angle between adjacent surface normals is less than this value
		the optimization is performed. This angle is specified in radians.\n\n
		<b>float edgeThresh</b>\n\n
		When the angle between adjacent surface normals is less than this value
		the auto edge is performed (if the <b>OPTIMIZE_AUTOEDGE</b> flag is
		set). This angle is specified in radians.\n\n
		<b>float bias</b>\n\n
		When optimizing mesh objects, as the optimization increases, you can
		get lots of long skinny 'degenerate' triangles (that cause rendering
		artifacts). Increasing the bias parameter keeps triangles from becoming
		degenerate. The range of values is from 0 to 1 (where 0 turns bias
		off). Values close to 1 reduce the amount of optimization in favor of
		maintaining equilateral triangles.\n\n
		<b>float maxEdge</b>\n\n
		This parameter is available in release 2.0 and later only.\n\n
		This will prevent the optimize function from creating edges longer than
		this value. If this parameter is \<=0 no limit is placed on the length
		of the edges.\n\n
		<b>DWORD flags</b>\n\n
		These flags control the optimization. Specify zero or more of the
		following values:\n\n
		<b>OPTIMIZE_SAVEMATBOUNDRIES</b>\n\n
		Specifies that faces won't be collapsed across a material boundary.\n\n
		<b>OPTIMIZE_SAVESMOOTHBOUNDRIES</b>\n\n
		Specifies that faces won't be collapsed across a dissimilar smoothing
		group boundary.\n\n
		<b>OPTIMIZE_AUTOEDGE</b>\n\n
		Specifies that the edge visibility should be set automatically based on
		the angle between adjacent surface normals. This will only set edges as
		invisible - it will not set edges as visible.\n\n
		<b>MeshOpProgress *prog=NULL</b>\n\n
		A callback used for lengthy optimize operations. See
		Class MeshOpProgress. */
		DllExport void Optimize(
			float normThresh, float edgeThresh, 
			float bias, float maxEdge, DWORD flags, 
			MeshOpProgress *prog=NULL);

		/*! \remarks This
		method may be called to map this Mesh with UVW mapping coordinates.
		\par Parameters:
		<b>int type</b>\n\n
		The mapping type. One of the following values:\n\n
		<b>MAP_PLANAR</b>\n\n
		<b>MAP_CYLINDRICAL</b>\n\n
		<b>MAP_SPHERICAL</b>\n\n
		<b>MAP_BALL</b>\n\n
		<b>MAP_BOX</b>\n\n
		<b>float utile</b>\n\n
		Number of tiles in the U direction.\n\n
		<b>float vtile</b>\n\n
		Number of tiles in the V direction.\n\n
		<b>float wtile</b>\n\n
		Number of tiles in the W direction.\n\n
		<b>int uflip</b>\n\n
		If nonzero the U values are mirrored.\n\n
		<b>int vflip</b>\n\n
		If nonzero the V values are mirrored.\n\n
		<b>int wflip</b>\n\n
		If nonzero the W values are mirrored.\n\n
		<b>int cap</b>\n\n
		This is used with <b>MAP_CYLINDRICAL</b>. If nonzero, then any face normal that
		is pointing more vertically than horizontally will be mapped using planar
		coordinates.\n\n
		<b>const Matrix3 \&tm</b>\n\n
		This defines the mapping space. As each point is mapped, it is multiplied by
		this matrix, and then it is mapped.\n\n
		<b>int channel=1</b>\n\n
		This parameter is available in release 2.0 and later only.\n\n
		This indicates which channel the mapping is applied to -- <b>channel==1</b>
		corresponds to the original texture mapping channel. */
		DllExport void ApplyUVWMap(int type,
			float utile, float vtile, float wtile,
			int uflip, int vflip, int wflip, int cap,
			const Matrix3 &tm, int channel=1);
		DllExport void ApplyMapper (UVWMapper & map, int channel=1);

		/*! \remarks Flips the surface normal of the 'i-th' face (this just
		rearranges the indices for the face structure). This also makes sure
		the edge flags are rearranged as well. If there are UV coordinates
		these are rearranged appropriately.
		\par Parameters:
		<b>int i</b>\n\n
		The index of the face to flip. */
		DllExport void FlipNormal(int i);
		/*! \remarks Unifies the surfaces normals of this Mesh. This may be
		for selected faces, or the entire mesh.
		\par Parameters:
		<b>BOOL selOnly</b>\n\n
		If TRUE only the selected faces are unified. */
		DllExport void UnifyNormals(BOOL selOnly);
		/*! \remarks Performs an auto smooth on the mesh, setting the
		smoothing groups based on the surface normals.
		\par Parameters:
		<b>float angle</b>\n\n
		The minimum angle between surface normals for smoothing to be applied,
		in radians.\n\n
		<b>BOOL useSel</b>\n\n
		If TRUE only the selected faces are smoothed.\n\n
		<b>BOOL preventIndirectSmoothing=FALSE</b>\n\n
		This parameter is available in release 2.0 and later only.\n\n
		TRUE to turn on; FALSE to leave off. This matches the option in the
		Smooth Modifier UI -- use this to prevent smoothing 'leaks" when using
		this method. If you use this method, and portions of the mesh that
		should <b>not</b> be smoothed become smoothed, then try this option to
		see if it will correct the problem. Note that the problem it corrects
		is rare, and that checking this slows the Auto Smooth process. */
		DllExport void AutoSmooth(float angle,BOOL useSel,BOOL preventIndirectSmoothing=FALSE);

		/*! \remarks This method is used internally. */
		DllExport Edge *MakeEdgeList(int *edgeCount, int flagdbls=0);
		/*! \remarks This method removes faces from the face list with the
		<b>FACE_WORK</b> flag set.
		\return  The number of faces deleted. */
		DllExport int DeleteFlaggedFaces(); // deletes all faces with FACE_WORK flag set
				
		
		// deletes all seleted elements of the current selection level
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Deletes all seleted elements of the current selection level. */
		DllExport void DeleteSelected();	
		
		// Deletes vertices as specified by the bit array
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Deletes the vertices as specified by the BitArray.
		\par Parameters:
		<b>BitArray set</b>\n\n
		Set of bits to indicate the vertices to delete. */
		DllExport void DeleteVertSet(BitArray set);
		
		// Deletes faces as specified by the bit array. If isoVert is non
		// null then it will be setup to flag vertices that were isolated
		// by the face deletetion. This set can then be passed to
		// DeleteVertSet to delete isolated vertices.
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Deletes faces as specified by the BitArray.
		\par Parameters:
		<b>BitArray set</b>\n\n
		Set of bits to indicate the faces to delete.\n\n
		<b>BitArray *isoVert=NULL</b>\n\n
		If non NULL then this method will be setup to flag vertices that were
		isolated by the face deletion. This set can then be passed to
		<b>DeleteVertSet()</b> to delete isolated vertices. */
		DllExport void DeleteFaceSet(BitArray set, BitArray *isoVert=NULL);

		// Returns TRUE if an equivalent face already exists.
		/*! \remarks This method may be called to determine if an equivalent
		face already exists.
		\par Parameters:
		<b>DWORD v0</b>\n\n
		Index of the first vertex.\n\n
		<b>DWORD v1</b>\n\n
		Index of the second vertex.\n\n
		<b>DWORD v2</b>\n\n
		Index of the third vertex.
		\return  TRUE if an equivalent face already exists; otherwise FALSE. */
		DllExport BOOL DoesFaceExist(DWORD v0, DWORD v1, DWORD v2);

		// Removes faces that have two or more equal indices.
		// Returns TRUE if any degenerate faces were found
		/*! \remarks Removes faces that have two or more equal indices.
		\return  TRUE if any degenerate faces were found; otherwise FALSE. */
		DllExport BOOL RemoveDegenerateFaces();

		// Removes faces that have indices that are out of range
		// Returns TRUE if any illegal faces were found
		/*! \remarks Removes faces that have indices that are out of range
		\return  TRUE if any illegal faces were found; otherwise FALSE. */
		DllExport BOOL RemoveIllegalFaces();

		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		This method returns the normal of the specified face. If <b>nrmlize</b>
		is TRUE, it makes this normal unit length. Otherwise, it's the edge
		cross-product length, which is actually 2 times the area of the face.
		\par Parameters:
		<b>DWORD fi</b>\n\n
		Specifies the face whose normal is returned.\n\n
		<b>BOOL nrmlize=FALSE</b>\n\n
		Use TRUE to make the normal unit length. */
		DllExport Point3 FaceNormal (DWORD fi, BOOL nrmlize=FALSE);
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		This method returns the center of the specified face.
		\par Parameters:
		<b>DWORD fi</b>\n\n
		Specifies the face whose center is returned. */
		DllExport Point3 FaceCenter (DWORD fi);
		/*! \remarks Returns the angle between two face surface normals in
		radians.
		\par Parameters:
		<b>DWORD f0</b>\n\n
		Index of the first face.\n\n
		<b>DWORD f1</b>\n\n
		Index of the second face. */
		DllExport float AngleBetweenFaces(DWORD f0, DWORD f1);

		// Compute the barycentric coords of a point in the plane of
		// a face relative to that face.
		/*! \remarks Computes and returns the barycentric coordinates of a
		point in the plane of a face relative to that face.
		\par Parameters:
		<b>DWORD face</b>\n\n
		The index of the face to check.\n\n
		<b>Point3 p</b>\n\n
		The input point.
		\return  The point <b>p</b> barycentric coordinates. If the point
		<b>p</b> is inside the face the returned values will sum to one. Note:
		If the face (or set of 3 points) is degenerate, ie if it has a zero
		length normal vector ((p1-p0)^(p2-p0)), the methods return
		Point3(-1,1,1). */
		DllExport Point3 BaryCoords(DWORD face, Point3 p);

		// Some edge operations
		/*! \remarks This method is available in release 2.5 and later
		only.\n\n
		Divides the edge, creating a new point. The face directly using this
		edge (face edge/3) is also divided in two, and other faces using the
		edge may optionally be split to use the split edges.
		\par Parameters:
		<b>DWORD edge</b>\n\n
		The edge to divide.\n\n
		<b>float prop</b>\n\n
		The proportion along the edge to make the division. An edge can be
		expressed as ff*3+ee, where ff is a face using this edge and ee
		represents which pair of vertices the edge is between,
		faces[ff]-\>v[ee] and faces[ff]-\>v[(ee+1)%3]. The new point is created
		at (1-prop) times the first vertex plus prop times the second. prop may
		vary from 0 to 1. prop=.5 gives the same result that DivideEdge (DWORD
		edge) would generate.\n\n
		<b>bool visDiag1=TRUE</b>\n\n
		Indicates whether the "diagonal" used to split the primary face this
		edge is on (edge/3) is visible or not.\n\n
		<b>bool fixNeighbors=TRUE</b>\n\n
		Indicates whether other faces using this edge should also be split to
		use the two new edges, or if they should be left as they were. In a
		typical mesh, there is one other face using this edge, the face on the
		"other side" of the edge. If fixNeighbors is FALSE, the "other side" in
		this case would still use the original edge, while the face on this
		side would be split to use the two new edges. This would create a
		"hole" in the mesh.\n\n
		<b>bool visDiag2=TRUE</b>\n\n
		Indicates whether the "diagonals" used to split other faces using this
		edge are visible or not. This argument is not used if fixneighbors is
		FALSE. */
		DllExport void DivideEdge(DWORD edge, float prop=.5f, bool visDiag1=TRUE,
			bool fixNeighbors=TRUE, bool visDiag2=TRUE);
		/*! \remarks This method is available in release 2.5 and later
		only.\n\n
		Cuts a face into three faces, arranged as a quad and a triangle, by
		slicing from a point on one edge to a point on another.
		\par Parameters:
		<b>DWORD face</b>\n\n
		The face to be divided.\n\n
		<b>DWORD e1, e2</b>\n\n
		The index of edges to be cut. For instance, if you wanted to cut from
		the edge between v[0] and v[1], to the edge between v[2] and v[0], you
		would use e1=0 and e2=2.\n\n
		<b>float prop1=.5f</b>\n\n
		The proportion along edge e1 to start cutting.\n\n
		<b>float prop2=.5f</b>\n\n
		The proportion along edge e2 to stop cutting.\n\n
		<b>bool fixNeighbors=TRUE</b>\n\n
		Indicates whether faces also using the cut edges should be split to use
		the new, subdivided edges.\n\n
		<b>bool split=FALSE</b>\n\n
		Indicates that the triangle and quad created by this action should use
		different vertices. If TRUE, the vertices created by the cut are
		duplicated, with one set being used for faces on one side and the other
		set being used by faces on the other side. */
		DllExport void DivideFace(DWORD face, DWORD e1, DWORD e2, 
			float prop1=.5f, float prop2=.5f, bool fixNeighbors=TRUE, bool split=FALSE);
		/*! \remarks This method is available in release 2.5 and later
		only.\n\n
		Turns an edge. The quadrilateral formed by the triangles on either side
		of this edge essentially has its diagonal switched.
		\par Parameters:
		<b>DWORD edge</b>\n\n
		The edge to be turned.\n\n
		<b>DWORD *otherEdge</b>\n\n
		If non-NULL, this should point to a variable in which the index of the
		"other side" of this edge should be stored. In essence, the two sides
		of an edge used by two faces, f1 and f2, are stored in two different
		locations, f1*3+e1 and f2*3+e2, where e1, e2 are 0, 1, or 2. This
		argument is provided so you have easy access to the other side of the
		edge, if desired, to make easy selection or visibility changes to the
		edge or other changes to the faces on both sides. */
		DllExport void TurnEdge (DWORD edge, DWORD *otherEdge=NULL);

		// Tessellation
		/*! \remarks		Tesselates the mesh (or only the selected faces) using the face/center method.
		This means each face is subdivided by lines from a new vertex at the center to
		the original vertices.
		\par Parameters:
		<b>BOOL ignoreSel=FALSE</b>\n\n
		If TRUE the entire mesh is tesselated; otherwise only the selected faces. */
		DllExport void FaceCenterTessellate(BOOL ignoreSel=FALSE, MeshOpProgress *mop=NULL);
		/*! \remarks		Edge tesselates the mesh using the specified tension parameter. This method can
		operate on the entire mesh or only the selection set. Edge tesselation means
		that faces are internally subdivided, with additional faces generated from a
		new vertex in the middle of each face edge.
		\par Parameters:
		<b>float tens</b>\n\n
		The tension setting. This value can range from -100.0 to 100.0. This value
		matches the parameter in the Editable Mesh user interface when tessellating
		faces.\n\n
		<b>BOOL ignoreSel=FALSE</b>\n\n
		If TRUE the entire mesh is tesselated; otherwise only the selected faces. */
		DllExport void EdgeTessellate(float tens,BOOL ignoreSel=FALSE, MeshOpProgress *mop=NULL);

		// Extrudes selected faces. Note that this is just a topological
		// change. The new extruded faces do not change position but
		// are left on top of the original faces.
		// If doFace is FALSE then selected edges are extruded.
		/*! \remarks This method is available in release 2.0 and later
		only.\n\n
		Extrudes the selected faces. Note that this is just a topological
		change. The new extruded faces do not change position but are left on
		top of the original faces.
		\par Parameters:
		<b>BOOL doFace=TRUE</b>\n\n
		If TRUE the faces are extruded. If FALSE then the selected edges are
		extruded. */
		DllExport void ExtrudeFaces(BOOL doFace=TRUE);

		// Indents selected faces, in a manner consistent with the outlining used in Bevel.
		// Added by SteveA for Shiva, 6/98
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Indents the selected faces, in a manner consistent with the outlining
		used in Bevel.
		\par Parameters:
		<b>float amount</b>\n\n
		The amount to indent. */
		DllExport void IndentSelFaces (float amount);

		// Splits verts specified in bitarray so that they are only
		// used by a single face
		/*! \remarks This method is available in release 2.0 and later
		only.\n\n
		Splits the vertices specified in the BitArray so that they are only
		used by a single face.
		\par Parameters:
		<b>BitArray set</b>\n\n
		This array of bits, one per vertex in the mesh. If the bit is set, the
		corresponding vertex in the mesh is copied as required so it is only
		used by a single face. If the bit is not set the vertex is ignored. */
		DllExport void BreakVerts(BitArray set);

		// Deletes verts that aren't used by any faces
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		This method returns a BitArray (of size <b>numVerts</b>), where
		isolated verts are selected. */
		DllExport BitArray GetIsoVerts ();
		/*! \remarks This method is available in release 2.0 and later
		only.\n\n
		Deletes the vertices that aren't used by any faces. */
		DllExport void DeleteIsoVerts ();

		// Clone faces (and verts used by those faces)
		/*! \remarks This method is available in release 2.0 and later
		only.\n\n
		Creates a copy of the faces and verts used by those faces as specified
		by the <b>BitArray</b> passed. If texture faces and vertex color faces
		are present they are cloned as well.
		\par Parameters:
		<b>BitArray fset</b>\n\n
		There is one bit in this array for each face in the mesh. If the bit is
		set, the corresponding face in the mesh is cloned. If the bit is zero
		the face is not cloned. */
		DllExport void CloneFaces(BitArray fset);
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		Sets bits for all faces in the same polygon with face <b>f</b>. Faces
		already selected in <b>set</b> will not be processed -- so if <b>f</b>
		is "set", nothing happens.\n\n
		The definition of a polygon is all faces sharing invisible edges with
		edge angles below "thresh".
		\par Parameters:
		<b>DWORD f</b>\n\n
		Specifies which face to evaluate -- the zero based index into the
		<b>faces</b> array.\n\n
		<b>BitArray \&set</b>\n\n
		Specifies which faces are not processed. One bit for each face with set
		bits not considered.\n\n
		<b>float thresh</b>\n\n
		The angle in radians which is the threshold for defining a polygon. A
		polygon is all faces sharing invisible edges with edge angles below
		this angle.\n\n
		<b>BOOL ignoreVisEdges</b>\n\n
		If TRUE, the edge visibility is ignored but the threshhold is still
		relevant.\n\n
		<b>AdjFaceList *af=NULL</b>\n\n
		This adjacent face list can be passed if there's one handy; otherwise a
		new one will be computed by the method. */
		DllExport void PolyFromFace (DWORD f, BitArray &set, float thresh, BOOL ignoreVisEdges, AdjFaceList *af=NULL);
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		This method sets bits for all faces in the same "element", or connected
		component, with face <b>f</b>. Faces already selected in <b>set</b>
		will be considered "walls" for this processing and will not be
		evaluated.
		\par Parameters:
		<b>DWORD f</b>\n\n
		Specifies which face to evaluate -- the zero based index into the
		<b>faces</b> array.\n\n
		<b>BitArray \&set</b>\n\n
		Specifies which faces are considered as barriers to the element and are
		not processed. One bit for each face with set bits not considered.\n\n
		<b>AdjFaceList *af=NULL</b>\n\n
		This adjacent face list can be passed if there's one handy; otherwise a
		new one will be computed by the method. */
		DllExport void ElementFromFace (DWORD f, BitArray &set, AdjFaceList *af=NULL);
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		When faces are deleted in Edit or Editable Mesh, we often want to be
		able to delete the verts that are isolated by this action. This method
		generates the list of verts that are used only by the specified set of
		faces.
		\par Parameters:
		<b>BitArray \& fset</b>\n\n
		This method finds those vertices used only by the faces indicated in
		this BitArray.\n\n
		<b>BitArray \& vset</b>\n\n
		This BitArray is completely overwritten with the result, and will be
		set to the right size (numVerts) if needed. */
		DllExport void FindVertsUsedOnlyByFaces (BitArray & fset, BitArray & vset);
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		This method fills in a BitArray with the edges in the mesh that are
		"open" or "one-sided". (This is the same as the Edit Mesh "Select Open
		Edges" function.)
		\par Parameters:
		<b>BitArray \& edges</b>\n\n
		This BitArray will be set to size numFaces*3, and only the open edge
		bits will be set. */
		DllExport void FindOpenEdges (BitArray & edges);
		/*! \remarks This method is available in release 3.0 and later
		only.\n\n
		This method calculates, for each vertex, the sum of the angles of this
		vertex's corner in each face it's on. So for instance, a point lying in
		the middle of a grid would always have vertex angle 2*PI, whereas a
		corner of a box would only have 3*PI/2.
		\par Parameters:
		<b>float *vang</b>\n\n
		This should be a pointer to an array of size numVerts. The vertex angle
		for each vertex is put in this array (in radians).\n\n
		<b>BitArray *set=NULL</b>\n\n
		If non-NULL, this points to a BitArray describing which vertices to
		find angles of. If only some bits are set, some computation time is
		saved. If NULL, all vertices' angles are found. */
		DllExport void FindVertexAngles (float *vang, BitArray *set=NULL);

		// used by the renderer
		/*! \remarks Sets the mesh render data hung off this Mesh. This method
		and <b>GetRenderData()</b> allow the renderer to 'hang' data on a mesh.
		This data can be anything the renderer wants. The data will
		automatically be deleted when the mesh is deleted via the
		<b>DeleteThis()</b> method.
		\par Parameters:
		<b>MeshRenderData *p</b>\n\n
		See Class MeshRenderData. */
		void  SetRenderData(MeshRenderData *p) {renderData = p; } 
		/*! \remarks Returns the mesh render data hung off this Mesh. See
		Class MeshRenderData. This
		method and <b>SetRenderData()</b> allow the renderer to 'hang' data on
		a mesh. This data can be anything the renderer wants. The data will
		automatically be deleted when the mesh is deleted via the
		<b>DeleteThis()</b> method. */
		MeshRenderData * GetRenderData() { return renderData; }

		// Quick access to specified normal interface.
		DllExport void ClearSpecifiedNormals ();
		DllExport MeshNormalSpec *GetSpecifiedNormals ();
		DllExport void SpecifyNormals ();

		// Copy only vertices and faces - no maps, selection, per-vertex data, etc.
		DllExport void CopyBasics (const Mesh & from);

		// --- from InterfaceServer
		DllExport BaseInterface* GetInterface(Interface_ID id);

      // TO DO: OLP This should probably be put in an interface.
      //! \brief Check if a face's cache is invalid
      /*!   The function first checks if the whole mesh or the face have been flagged as
         cache invalid (MESH_CACHEINVALID). If none of these is true, it then cycles through the face's
         vertices and returns true as soon as it finds a vertex flagged with MESH_CACHEINVALID.
         \pre faceIndex must be a valid index in the faces (f) array.
         \param[in] faceIndex The index of the face for which you want to determine cache validity
         \return true if the face is invalid, else false.
      */
      DllExport bool isFaceCacheInvalid(int faceIndex);

      //! \brief Invalidates a single vertex's cache indicating it needs to be rebuilt.
      /*!   The function flags the specified vertex with the MESH_CACHEINVALID flag, telling the rebuild
         functions that this vertex, the faces surrounding it and the other vertices of those faces
         need to have their normals recomputed. It also sets normalsBuilt to 0 to trigger normals' rebuild.
         This function should be called when one to several vertices have been modified. If all or almost
         all vertices of the mesh have been invalidated, it's more efficient to call InvalidateGeomCache() instead.
         \pre vertex must be a valid index in the vertex (v) array.
         \param[in] vertex The index of the vertex that needs to be invalidated.
      */
      DllExport void InvalidateVertexCache(int vertex);

      //! \brief Holds flags specific to the corresponding vertex in the verts array.
      long *      vFlags;

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
		DllExport bool NeedGWCacheRebuilt(GraphicsWindow *gw,Material *ma, int numMat);
		/*! \brief This builds the graphics window cached mesh 
		\par Parameters:
		GraphicsWindow *gw the active graphics window \n
		Material *ma the material aray assigned to the mesh \n
		int numMat the number of materials in the material array \n
		BOOL threaded whether when building the cache it can use additional threads.  This is needed since the system may be creating many meshes at the same time\n
		*/
		DllExport void BuildGWCache(GraphicsWindow *gw,Material *ma, int numMat,BOOL threaded);


		
	};

// Mapping types passed to ApplyUVWMap()
#define MAP_PLANAR			 0
#define MAP_CYLINDRICAL		 1
#define MAP_SPHERICAL		 2
#define MAP_BALL			 3
#define MAP_BOX				 4
#define MAP_FACE			 5
#define MAP_ACAD_PLANAR		 6
#define MAP_ACAD_BOX         7
#define MAP_ACAD_CYLINDRICAL 8
#define MAP_ACAD_SPHERICAL   9


// Optimize flags
#define OPTIMIZE_SAVEMATBOUNDRIES		(1<<0)
#define OPTIMIZE_SAVESMOOTHBOUNDRIES	(1<<1)
#define OPTIMIZE_AUTOEDGE				(1<<2)

/*! \remarks This is used internally. */
DllExport void setUseVisEdge(int b);
/*! \remarks This is used internally. */
DllExport int getUseVisEdge();

#define SMALL_VERTEX_DOTS	0
#define LARGE_VERTEX_DOTS	1

// CAL-05/07/03: new vertex dot types with radius from 2 to 7 (release 6.0)
#define VERTEX_DOT2	0		// equivalent to SMALL_VERTEX_DOTS
#define VERTEX_DOT3	1		// equivalent to LARGE_VERTEX_DOTS
#define VERTEX_DOT4	2
#define VERTEX_DOT5	3
#define VERTEX_DOT6	4
#define VERTEX_DOT7	5

// CAL-05/07/03: new handle box types with radius from 2 to 7 (release 6.0)
#define HANDLE_BOX2	0
#define HANDLE_BOX3	1
#define HANDLE_BOX4	2
#define HANDLE_BOX5	3
#define HANDLE_BOX6	4
#define HANDLE_BOX7	5

// CAL-05/07/03: get vertex dot marker from vertex dot type
#define VERTEX_DOT_MARKER(vtype) (MarkerType)(vtype - VERTEX_DOT2 + DOT2_MRKR)

// CAL-05/07/03: get vertex dot marker from vertex dot type
#define HANDLE_BOX_MARKER(htype) (MarkerType)(htype - HANDLE_BOX2 + BOX2_MRKR)

DllExport void setUseVertexDots(int b);
DllExport int getUseVertexDots();

DllExport void setVertexDotType(int t);
DllExport int getVertexDotType();

DllExport void setHandleBoxType(int t);
DllExport int getHandleBoxType();

DllExport void setDisplayBackFaceVertices(int b);
DllExport int getDisplayBackFaceVertices();

// Steve Anderson - new for Max 5.0 - we have a better way of evaluating
// vertex normals now, weighting the contribution of each face by the angle
// the face makes at that vertex, but users might want to access the old way.
// This is controlled by the "Use Legacy R4 Vertex Normals" checkbox
// in the General Preferences dialog.
class VertexNormalsCallback: public MaxHeapOperators {
public:
	virtual ~VertexNormalsCallback() {;}
	virtual void SetUseFaceAngles (bool value) = 0;
};

class VertexNormalsControl: public MaxHeapOperators {
	bool mUseFaceAngles;
	Tab<VertexNormalsCallback *> mCallbacks;
public:
	DllExport VertexNormalsControl () : mUseFaceAngles(true) { }

	DllExport void RegisterCallback (VertexNormalsCallback *pCallback);
	DllExport void UnregisterCallback (VertexNormalsCallback *pCallback);

	DllExport void SetUseFaceAngles (bool value);
	DllExport bool GetUseFaceAngles ();
};

DllExport VertexNormalsControl *GetVertexNormalsControl ();

// a callback to update progress UI while doing a
// lengthy operation to a mesh
/*! \sa  Class Mesh.\n\n
\par Description:
A callback used while doing a lengthy operation to a mesh. A developer creates
an instance of this class and passes a pointer to it into the
<b>CalcBoolOp()</b> or <b>Optimize()</b> function.  */
class MeshOpProgress: public MaxHeapOperators {
	public:
		/*! \remarks Destructor. */
		virtual ~MeshOpProgress() {;}

		// called once with the total increments
		/*! \remarks This method is called once with the total number of increments.
		\par Parameters:
		<b>int total</b>\n\n
		The total number of increments. */
		virtual void Init(int total)=0;

		// Called to update progress. % done = p/total
		/*! This method is called over and over with a new value for <b>p</b>.
		The percentage complete is <b>p/total</b>.
		\par Parameters:
		<b>int p</b>\n\n
		The number completed so far.
		\return  If TRUE processing will continue. If FALSE processing is
		aborted. */
		virtual BOOL Progress(int p)=0;
	};

// Boolean operations for meshes:
#define MESHBOOL_UNION 				1
#define MESHBOOL_INTERSECTION  		2
#define MESHBOOL_DIFFERENCE 		3

//
// mesh = mesh1 op mesh2
// If tm1 or tm2 are non-NULL, the points of the corresponding
// mesh will be transformed by these tm before the bool op
// The mesh will be transformed back by either Inverse(tm1) or
// Inverse(tm2) depending whichInv (0=>tm1, 1=>tm2)
// unless whichInv is -1 in which case it will not be transformed
// back.
//
/*! <b>Matrix3 *tm2 = NULL, int whichInv = 0, int weld = TRUE);</b> \remarks
Note: This method is still in the SDK, but it is now obselete. Calls to
<b>CalcBoolOp()</b> should be replaced with calls to the new <b>MNMesh</b> form
of Boolean. Please see the method <b>MNMesh::MakeBoolean</b> in
Class MNMesh for details.\n\n
This function stores the result of a boolean operation between <b>mesh1</b> and
<b>mesh2</b> into <b>mesh</b>. This operation may be a union, intersection or
difference. If <b>tm1</b> or <b>tm2</b> are non-NULL, the points of the
corresponding mesh will be transformed by these matrices before the boolean
operation. The mesh will be transformed back by either <b>Inverse(tm1)</b> or
<b>Inverse(tm2)</b> depending on <b>whichInv</b> (a value of 0 will use
<b>tm1</b>, a value of 1 will use <b>tm2</b>, unless <b>whichInv</b> is -1 in
which case it will not be transformed back).
\par Parameters:
<b>Mesh \&mesh</b>\n\n
The result of the boolean operation is stored here. <b>mesh = mesh1 op
mesh2</b>.\n\n
<b>Mesh \&mesh1</b>\n\n
The first operand.\n\n
<b>Mesh \&mesh2</b>\n\n
The second operand.\n\n
<b>int op</b>\n\n
The boolean operation. One of the following values:\n\n
<b>MESHBOOL_UNION</b>\n\n
<b>MESHBOOL_INTERSECTION</b>\n\n
<b>MESHBOOL_DIFFERENCE</b>\n\n
<b>MeshOpProgress *prog = NULL</b>\n\n
A callback to display a progress. See
Class MeshOpProgress.\n\n
<b>Matrix3 *tm1 = NULL</b>\n\n
If non-NULL then the points of mesh1 will transformed by this matrix before the
boolean operation.\n\n
<b>Matrix3 *tm2 = NULL</b>\n\n
If non-NULL then the points of mesh2 will transformed by this matrix before the
boolean operation.\n\n
<b>int whichInv = 0</b>\n\n
If 0, the resulting mesh will be transformed by Inverse(tm1). If 1, the
resulting mesh will be transformed by Inverse(tm2). If -1, the mesh will not be
transformed back.\n\n
<b>int weld = TRUE</b>\n\n
If TRUE, the vertices of the resulting mesh are welded.
\return  Nonzero if the operation completed successfully; otherwise zero. */
DllExport int CalcBoolOp(
	Mesh &mesh, Mesh &mesh1, Mesh &mesh2, int op,
	MeshOpProgress *prog = NULL,
	Matrix3 *tm1 = NULL,
	Matrix3 *tm2 = NULL,
	int whichInv = 0,
	int weld = TRUE);


// Combines two meshes. The matrix and whichInv parameters have
// the same meaning as they do for the CalcBoolOp above.
/*! \remarks This function is available in release 2.0 and later only.\n\n
This function may be used to simply combine two meshes into one.
\par Parameters:
<b>Mesh \&mesh</b>\n\n
The result of the combine operation is stored here. <b>mesh =
mesh1+mesh2</b>.\n\n
<b>Mesh \&mesh1</b>\n\n
The first operand.\n\n
<b>Mesh \&mesh2</b>\n\n
The second operand.\n\n
<b>Matrix3 *tm1 = NULL</b>\n\n
If non-NULL then the points of mesh1 will transformed by this matrix before the
boolean operation.\n\n
<b>Matrix3 *tm2 = NULL</b>\n\n
If non-NULL then the points of mesh2 will transformed by this matrix before the
boolean operation.\n\n
<b>int whichInv=0</b>\n\n
If 0, the resulting mesh will be transformed by Inverse(tm1). If 1, the
resulting mesh will be transformed by Inverse(tm2). If -1, the mesh will not be
transformed back. */
DllExport void CombineMeshes(
		Mesh &mesh,Mesh &mesh1,Mesh &mesh2,
		Matrix3 *tm1=NULL, Matrix3 *tm2=NULL, int whichInv=0);

// Slices a single mesh.  The Point3 N and the float offset define a
// slicing plane (by DotProd (N,X) = offset).  Default behavior is to
// split faces that cross the plane, producing 1-2 new faces on each side
// and a new vert in the middle of each edge crossing the plane.  split
// means to add 2 different but coincident points to the top and bottom
// sets of faces, splitting the mesh into two meshes.  remove means to
// delete all faces & verts below the plane.
DllExport void SliceMesh (Mesh & mesh,
						  Point3 N, float off, bool split=FALSE, bool remove=FALSE);

// Handy utilities to go with meshes:

// Translates map type into 3d location -> uvw coord mapper:
/*! \sa  Class Mesh, Class Matrix3, Class Point3.\n\n
\par Description:
This class is available in release 3.0 and later only.\n\n
Prior to release 3.0, developers could implement <b>Object::ApplyUVWMap()</b>
in their objects, but didn't have access to the algorithm 3ds Max uses
internally to turn the mapping types (MAP_BOX, MAP_PLANE, etc) into an actual
vertex-to-mapping-coordinate function. This class now makes this available.\n\n
The constructors for the class initialize the data members with information
about the mapping desired. The main method, <b>MapPoint()</b>, maps a point in
object space into the UVW map defined by this mapper.\n\n
Note: <b>typedef Point3 UVVert;</b>
\par Data Members:
<b>int type;</b>\n\n
The mapping type. One of the following values:\n\n
<b>MAP_PLANAR</b>\n\n
<b>MAP_CYLINDRICAL</b>\n\n
<b>MAP_SPHERICAL</b>\n\n
<b>MAP_BALL</b>\n\n
<b>MAP_BOX</b>\n\n
<b>MAP_FACE</b>\n\n
<b>int cap;</b>\n\n
This is used with <b>MAP_CYLINDRICAL</b>. If nonzero, then any face normal that
is pointing more vertically than horizontally will be mapped using planar
coordinates.\n\n
<b>float utile;</b>\n\n
Number of tiles in the U direction.\n\n
<b>float vtile;</b>\n\n
Number of tiles in the V direction.\n\n
<b>float wtile;</b>\n\n
Number of tiles in the W direction.\n\n
<b>int uflip;</b>\n\n
If nonzero the U values are mirrored.\n\n
<b>int vflip</b>\n\n
If nonzero the V values are mirrored.\n\n
<b>int wflip;</b>\n\n
If nonzero the W values are mirrored.\n\n
<b>Matrix3 tm;</b>\n\n
This defines the mapping space. As each point is mapped, it is multiplied by
this matrix, and then it is mapped.  */
class UVWMapper: public MaxHeapOperators {
public:
	int     type, cap;
	float   utile, vtile, wtile;
	int     uflip, vflip, wflip;
	Matrix3 tm;

	/*! \remarks Constructor. The data members are initialized as follows:\n\n
	<b> type = MAP_BOX;</b>\n\n
	<b> utile = 1.0f;</b>\n\n
	<b> vtile = 1.0f;</b>\n\n
	<b> wtile = 1.0f;</b>\n\n
	<b> uflip = 0;</b>\n\n
	<b> vflip = 0;</b>\n\n
	<b> wflip = 0;</b>\n\n
	<b> cap = 0;</b>\n\n
	<b> tm.IdentityMatrix();</b> */
	DllExport UVWMapper();
	/*! \remarks Constructor. The data members are initialized to the values
	passed. */
	DllExport UVWMapper(int type, const Matrix3 &tm, int cap=FALSE,
		float utile=1.0f, float vtile=1.0f, float wtile=1.0f,
		int uflip=FALSE, int vflip=FALSE, int wflip=FALSE);
	/*! \remarks Constructor. The data members are initialized from the
	UVWMapper passed. */
	DllExport UVWMapper(UVWMapper& m);

	/*! \remarks This method maps a point in object space into the UVW map
	defined by this mapper. This gives the UVW coordinates for the specified
	point according to this mapper's mapping scheme.
	\par Parameters:
	<b>Point3 p</b>\n\n
	The location of a vertex, i.e. the point being mapped. This point should
	NOT be transformed by the UVWMapper's tm, as this happens internally.\n\n
	<b>const Point3 \& norm</b>\n\n
	The direction of the surface normal at p. This information is only required
	for types MAP_BOX or MAP_CYLINDRICAL. See the method <b>NormalMatters()</b>
	below.\n\n
	<b>int *nan=NULL</b>\n\n
	If non-NULL, this points to an int which should be set to FALSE if this
	mapping is good for all faces using this vertex, or TRUE if different faces
	should have different mapping coordinates. This is generally set to TRUE
	more often than absolutely necessary to make sure nothing is missed.
	\return  The mapped point. */
	DllExport UVVert MapPoint(Point3 p, const Point3 & norm, int *nan=NULL);
	/*! \remarks Applies the UVWMap's tile and flip parameters to the given
	UVVert, and returns the result..
	\par Parameters:
	<b>UVVert uvw</b>\n\n
	The input UVVert.
	\return  The modified UVVert. */
	DllExport UVVert TileFlip (UVVert uvw);
	/*! \remarks This method indicates which direction the given vector
	"chiefly points", after vector transformation by the UVWMapper's transform.
	\par Parameters:
	<b>const Point3 \& n</b>\n\n
	The input vector whose main axis is determined.
	\return  One of the following values:\n\n
	<b>0</b>: tm.VectorTransform(n) points mainly in the +x direction.\n\n
	<b>1</b>: tm.VectorTransform(n) points mainly in the +y direction.\n\n
	<b>2</b>: tm.VectorTransform(n) points mainly in the +z direction.\n\n
	<b>3</b>: tm.VectorTransform(n) points mainly in the -x direction.\n\n
	<b>4</b>: tm.VectorTransform(n) points mainly in the -y direction.\n\n
	<b>5</b>: tm.VectorTransform(n) points mainly in the -z direction. */
	DllExport int MainAxis (const Point3 & n);
	/*! \remarks This method lets you know whether the current mapping type
	uses the normal information. If FALSE, it doesn't matter what value you
	pass as a normal to MapPoint. If TRUE, the MainAxis of the normal is used
	to determine the mapping. */
	bool NormalMatters () { return ((type==MAP_BOX || type==MAP_ACAD_BOX)||((type==MAP_CYLINDRICAL || type==MAP_ACAD_CYLINDRICAL )&&cap)) ? TRUE:FALSE; }
};

DllExport Mesh * CreateNewMesh();

#endif // _MESH_H_
