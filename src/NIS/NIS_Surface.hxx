// Created on: 2008-03-19
// Created by: Alexander GRIGORIEV
// Copyright (c) 2008-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef NIS_Surface_HeaderFile
#define NIS_Surface_HeaderFile

#include <NIS_InteractiveObject.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <gp_XYZ.hxx>

class Quantity_Color;
class Handle(Poly_Triangulation);
class TopoDS_Shape;

/**
 * Presentation of a meshed surface.
 * Consists of 4 arrays: Nodes, Triangles, Normals and Edges. Normals are
 * defined in nodes, so the number of stored normals is strictly the number of
 * nodes. Edges is an array of pointers: each pointer starts an array of
 * node indices that define a single edge (i.e., a polygon that can be closed or
 * open, no matter). The first number in the edge is the number of nodes in it.
 * <p>
 * Instances of this class can be initialized either atomically (setting every
 * node and triangle and edge) or from a TopoDS_Shape object. In side the
 * TopoDS_Shape only triangulations in faces are used; edges are taken from
 * PolygonOnTriangulation also belonging to faces.
 * <p>
 * This class is conceived as replacement of AIS_Shape; both wireframe and
 * shading modes are available for dynamic switching.
 */

class NIS_Surface : public NIS_InteractiveObject
{
public:
  enum DisplayMode {
    Shading,
    Wireframe
  };

  /**
   * Constructor
   */
  Standard_EXPORT NIS_Surface(const Handle(Poly_Triangulation)& theTri,
                              const Handle(NCollection_BaseAllocator)&
                                                                theAlloc =0L);
  /**
   * Constructor. Creates the presentation of all faces in 'theShape' object.
   * @param theShape
   *   Source geometry. It should contain triangulations inside.
   * @param theDeflection
   *   Absolute deflection for meshing 'theShape' if such meshing is needed.
   * @param theAl
   *   Allocator used for nodes and triangles in this presentation.
   */
  Standard_EXPORT NIS_Surface(const TopoDS_Shape& theShape,
                              const Standard_Real theDeflection,
                              const Handle(NCollection_BaseAllocator)& theAl=0L);

  /**
   * Destructor
   */
  Standard_EXPORT virtual ~NIS_Surface ();

  /**
   * Initialize the instance with a TopoDS_Shape. Used in constructor but can
   * be called any time to redefine the geometry.
   */
  Standard_EXPORT void              Init        (const TopoDS_Shape& theShape,
                                                 const Standard_Real theDefl);

  /**
   * Deallocate all internal data structures.
   */
  Standard_EXPORT void              Clear       ();

  /**
   * Query the number of nodes.
   */
  inline Standard_Integer           NNodes      () const { return myNNodes; }

  /**
   * Query the number of triangles.
   */
  inline Standard_Integer           NTriangles  () const { return myNTriangles;}

  /**
   * Query the number of edges for wireframe display.
   */
  inline Standard_Integer           NEdges      () const { return myNEdges; }

  /**
   * Query the node by its index.
   * @return
   *   pointer to array of 3 Standard_ShortReal values (X,Y,Z coordinates)
   */
  inline const Standard_ShortReal*
                        Node            (const Standard_Integer theIndex) const
  {
    return &mypNodes[theIndex * 3];
  }

  /**
   * Query the triangle by its index.
   * @return
   *   pointer to array of 3 Standard_Integer values (nodes 0, 1, 2)
   */
  inline const Standard_Integer*
                        Triangle        (const Standard_Integer theIndex) const
  {
    return &mypTriangles[theIndex * 3];
  }

  /**
   * Access to array of integers that represents an Edge.
   * @return
   *   Pointer to array where the 0th element is the number of nodes in the edge
   *   and the elements starting from the 1st are node indices.
   */
  inline const Standard_Integer*
                        Edge            (const Standard_Integer theIndex) const
  {
    return mypEdges[theIndex];
  }

  /**
   * Query the normal vector at the given triangulation node (by index)
   * @return
   *   pointer to array of 3 Standard_ShortReal values (X,Y,Z coordinates)
   */
  inline const Standard_ShortReal*
                        Normal          (const Standard_Integer theIndex) const
  {
    return &mypNormals[theIndex * 3];
  }

  /**
   * Create a default drawer instance.
   */
  Standard_EXPORT virtual NIS_Drawer *
                        DefaultDrawer   (NIS_Drawer *) const;

  /**
   * Set the normal color for presentation.
   * @param theColor
   *   New color to use for the presentation.
   */
  Standard_EXPORT void  SetColor        (const Quantity_Color& theColor);

  /**
   * Set the color for presentation of the back side of triangles.
   * @param theColor
   *   New color to use for the presentation.
   */
  Standard_EXPORT void  SetBackColor    (const Quantity_Color& theColor);

  /**
   * Set the offset for the presentation.
   * @param theValue
   *   New offset to use for the presentation.
   */
  Standard_EXPORT void  SetPolygonOffset (const Standard_Real theValue);

  /**
   * Set the display mode: Shading or Wireframe.
   * The default mode is Shading.
   */
  Standard_EXPORT void  SetDisplayMode   (const DisplayMode theMode);

  /**
   * Query the current display mode: Shading or Wireframe.
   */
  Standard_EXPORT DisplayMode
                        GetDisplayMode   () const;

  /**
   * Create a copy of theObject except its ID.
   * @param theAll
   *   Allocator where the Dest should store its private data.
   * @param theDest
   *   <tt>[in-out]</tt> The target object where the data are copied. If
   *   passed NULL then the target should be created.
   */
  Standard_EXPORT virtual void
                          Clone (const Handle(NCollection_BaseAllocator)& theAll,
                                 Handle(NIS_InteractiveObject)& theDest) const;

  /**
   * Intersect the surface shading/wireframe geometry with a line/ray.
   * @param theAxis
   *   The line or ray in 3D space.
   * @param theOver
   *   Half-thickness of the selecting line - ignored.
   * @return
   *   If the return value is more than 0.1*RealLast() then no intersection is
   *   detected. Otherwise returns the coordinate of the nearest intersection
   *   on the ray. May be negative.
   */
  Standard_EXPORT virtual Standard_Real
                          Intersect     (const gp_Ax1&       theAxis,
                                         const Standard_Real theOver) const;

  /**
   * Intersect the surface shading/wireframe geometry with an oriented box.
   * @param theBox
   *   3D box of selection
   * @param theTrf
   *   Position/Orientation of the box.
   * @param isFull
   *   True if full inclusion is required, False - if partial.
   * @return
   *   True if the InteractiveObject geometry intersects the box or is inside it
   */
  Standard_EXPORT virtual Standard_Boolean
                             Intersect     (const Bnd_B3f&         theBox,
                                            const gp_Trsf&         theTrf,
                                            const Standard_Boolean isFull)const;

  /**
   * Intersect the surface shading/wireframe geometry with a selection polygon.
   * @param thePolygon
   *   the list of vertices of a free-form closed polygon without
   *   self-intersections. The last point should not coincide with the first
   *   point of the list. Any two neighbor points should not be confused.
   * @param theTrf
   *   Position/Orientation of the polygon.
   * @param isFullIn
   *   True if full inclusion is required, False - if partial.
   * @return
   *   True if the InteractiveObject geometry intersects the polygon or is
   *   inside it
   */
  Standard_EXPORT virtual Standard_Boolean Intersect
                    (const NCollection_List<gp_XY> &thePolygon,
                     const gp_Trsf                 &theTrf,
                     const Standard_Boolean         isFullIn) const;

protected:

  /**
   * Allocator for method Clone().
   */
  Standard_EXPORT NIS_Surface (const Handle(NCollection_BaseAllocator)& theAl); 

  /**
   * Create a 3D bounding box of the object.
   */
  Standard_EXPORT virtual void computeBox     ();

  /**
   * Compute normal to the surface at the node with the given index.
   * Returns true if the vertex is artificial at this node.
   */
  Standard_Boolean computeNormal (Standard_Integer theIndex,
                                  gp_XYZ& theNormal) const;

private:
  Handle(NCollection_BaseAllocator) myAlloc;
  //! Array of nodes in triangles
  Standard_ShortReal               * mypNodes;
  //! Array of normals (TriNodes)
  Standard_ShortReal               * mypNormals;
  //! Array of triangles (node indices)
  Standard_Integer                 * mypTriangles;
  //! Array of edges, each edge is an array of indices prefixed by N nodes
  Standard_Integer                 ** mypEdges;
  //! Number of nodes in triangles
  Standard_Integer                 myNNodes;
  Standard_Integer                 myNTriangles;
  Standard_Integer                 myNEdges;
  Standard_Boolean                 myIsWireframe;

public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (NIS_Surface)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (NIS_Surface, NIS_InteractiveObject)

#endif
