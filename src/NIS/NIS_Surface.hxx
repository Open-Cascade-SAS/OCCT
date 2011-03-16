// File:      NIS_Surface.h
// Created:   19.03.08 20:37
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade S.A. 2008

#ifndef NIS_Surface_HeaderFile
#define NIS_Surface_HeaderFile

#include <NIS_InteractiveObject.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <gp_XYZ.hxx>

class Quantity_Color;
class Handle_Poly_Triangulation;
class TopoDS_Shape;

/**
 * This class describes a presentation of a meshed surface.
 */

class NIS_Surface : public NIS_InteractiveObject
{
public:
  /**
   * Constructor
   */
  Standard_EXPORT NIS_Surface(const Handle_Poly_Triangulation& theTri,
                                    const Handle_NCollection_BaseAllocator&
                                                                theAlloc =0L);
  /**
   * Constructor. Creates the presentation of all faces in 'theShape' object.
   * @aparm theShape
   *   Source geometry. It should contain triangulations inside.
   * @param theAlloc
   *   Allocator used for nodes and triangles in this presentation.
   */
  Standard_EXPORT NIS_Surface(const TopoDS_Shape& theShape,
//                                     const Standard_Real theDeflection,
                                    const Handle_NCollection_BaseAllocator&
                                                        theAlloc = 0L);

  /**
   * Destructor
   */
  Standard_EXPORT virtual ~NIS_Surface ();

  /**
   * Query the number of nodes.
   */
  inline Standard_Integer           NNodes      () const { return myNNodes; }

  /**
   * Query the number of triangles.
   */
  inline Standard_Integer           NTriangles  () const { return myNTriangles;}

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
  Standard_EXPORT virtual Handle_NIS_Drawer
                        DefaultDrawer   () const;

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
   * Set the transparency factor.
   * @param theValue
   *   1.0 means full transparency, 0.0 means opaque. Valid quantities are in
   *   this interval.
   */
  Standard_EXPORT void  SetTransparency (const Standard_Real theValue);

  /**
   * Intersect the InteractiveObject geometry with a line/ray.
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
                        Intersect       (const gp_Ax1&       theAxis,
                                         const Standard_Real theOver) const;

  /**
   * Intersect the InteractiveObject geometry with an oriented box.
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

protected:

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
  //! Array of nodes in triangles
  Standard_ShortReal               * mypNodes;
  //! Array of normals (TriNodes)
  Standard_ShortReal               * mypNormals;
  Standard_Integer                 * mypTriangles;
  //! Number of nodes in triangles
  Standard_Integer                 myNNodes;
  Standard_Integer                 myNTriangles;
  Handle_NCollection_BaseAllocator myAlloc;

public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (NIS_Surface)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (NIS_Surface, NIS_InteractiveObject)

#endif
