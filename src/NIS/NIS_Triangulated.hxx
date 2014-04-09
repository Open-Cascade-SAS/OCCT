// Created on: 2007-07-17
// Created by: Alexander GRIGORIEV
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#ifndef NIS_Triangulated_HeaderFile
#define NIS_Triangulated_HeaderFile

#include <NIS_InteractiveObject.hxx>
#include <Quantity_Color.hxx>

class Handle(NIS_TriangulatedDrawer);
class NCollection_BaseAllocator;
class Handle(NCollection_BaseAllocator);
class NIS_TriangulatedDrawer;

/**
 * Interactive object that consists of triangles, lines and polygons without
 * normals. Particularly can be used to render planar 2D shapes.
 *
 * @par 2D and 3D model
 * Vertices are stored in an array of float numbers, 2 or 3 numbers per vertex.
 * The number of dimensions is defined in the constructor, see the parameter
 * 'is2D'. When 2D is defined then for all vertices the Z coordinate is 0.
 * To display planar objects in a plane different from XOY you should subclass
 * this type together with the correponding Drawer and store the transformation
 * parameters. In Drawer subclass either in method BeforeDraw() or in method
 * Draw() you would call glTranslate() or glMultMatrix() so that all vertices
 * should be located in their proper positions.
 *
 * @par Compressed storage
 * For efficient memory utilization, indice (triangles, segments, polygons) are
 * 8-bit, 16-bit or 32-bit numbers. The width of this numeric representation is
 * chosen automatically when the total number of nodes is passed in the
 * constructor or in any Set* method. For example, if this number of nodes is
 * smaller than 256 then 8-bit representation is selected. The choice is stored
 * in 'myIndexType' data member.
 */

class NIS_Triangulated : public NIS_InteractiveObject
{
 protected:
  /**
   * Constants defining the mode (type) of presentation. They allow mixed type,
   * e.g., Triangulation+Line. Line and Segments are not mixable, their mix is
   * treated as Line only.
   */
  enum {
    Type_None          =  0,
    Type_Loop          =  1,  //!< modifier to Line
    Type_Line          =  2,
    Type_Segments      =  4,
    Type_Triangulation =  8,
    Type_Polygons      = 16
  };

 public:
  /**
   * Enumerated type of polygon rendering.
   */
  enum PolygonType {
    Polygon_Default  = 0,  //!< Polygon as LINE, Triangulation as FILL
    Polygon_Line     = 1,  //!< Both Polygon and Triangulation as LINE
    Polygon_Fill     = 2   //!< Both Polygon and Triangulation as FILL
  };

 public:
  // ---------- PUBLIC METHODS ----------


  /**
   * Constructor. Optionally defines the number of nodes that will be allocated
   * (this number may be defined later in methods Set*Prs) as well as the
   * memory allocator where the nodes, lines and triangles will be stored by
   * this instance.
   * @param nNodes
   *   Total number of nodes that will be initialized for this object
   * @param is2D
   *   If true then the nodes will be 2D in plane Z=0, otherwise normal 3D.
   * @param theAlloc
   *   Allocator for internal data
   */
  Standard_EXPORT NIS_Triangulated(const Standard_Integer nNodes = 0,
                                   const Standard_Boolean is2D = Standard_False,
                                   const Handle(NCollection_BaseAllocator)&
                                   theAlloc = 0L);

  /**
   * Define the polygonal presentration.
   * @param nPolygons
   *   Number of separate polygons. If set to 0, polygons are cancelled
   * @param nNodes
   *   Optional: Number of nodes to allocate. If left as 0, the previous nodes
   *   allocation is used, otherwise a new allocation is created.
   */ 
  Standard_EXPORT void              SetPolygonsPrs
                                        (const Standard_Integer nPolygons,
                                         const Standard_Integer nNodes = 0);

  /**
   * Define the triangulated presentration.
   * @param nTriangles
   *   Number of triangles. If set to 0, triangulation is cancelled
   * @param nNodes
   *   Optional: Number of nodes to allocate. If left as 0, the previous nodes
   *   allocation is used, otherwise a new allocation is created.
   */ 
  Standard_EXPORT void              SetTriangulationPrs
                                        (const Standard_Integer nTriangles,
                                         const Standard_Integer nNodes = 0);

  /**
   * Define the line presentration (polygon through points)
   * @param nPoints
   *   Number of nodes defining the line. If set to 0, line is cancelled
   * @param isClosed
   *   True if the polygon is closed, so the segment between the first and
   *   the last points is created automatically.
   * @param nNodes
   *   Optional: Number of nodes to allocate. If left as 0, the previous nodes
   *   allocation is used, otherwise a new allocation is created.
   */ 
  Standard_EXPORT void              SetLinePrs
                                        (const Standard_Integer nPoints,
                                         const Standard_Boolean isClosed,
                                         const Standard_Integer nNodes = 0);

  /**
   * Define the segments presentration. Each segment is defined by 2 nodes
   * @param nSegments
   *   Number of segments. If set to 0, segments presentation is cancelled
   * @param nNodes
   *   Optional: Number of nodes to allocate. If left as 0, the previous nodes
   *   allocation is used, otherwise a new allocation is created.
   */ 
  Standard_EXPORT void              SetSegmentPrs
                                        (const Standard_Integer nSegments,
                                         const Standard_Integer nNodes = 0);

  /**
   * Query if there is Triangulation component in the presentation.
   */
  inline Standard_Boolean           IsTriangulation () const
  { return (myType & Type_Triangulation) != 0; }

  /**
   * Query if there is Polygons component in the presentation.
   */
  inline Standard_Boolean           IsPolygons  () const
  { return (myType & Type_Polygons) != 0; }

  /**
   * Query if there is Line component in the presentation.
   */
  inline Standard_Boolean           IsLine      (Standard_Boolean& isLoop) const
  { isLoop = (myType & Type_Loop) != 0; return (myType & Type_Line) != 0; }

  /**
   * Query if there is Segments component in the presentation.
   */
  inline Standard_Boolean           IsSegments  () const
  { return (myType & Type_Segments) != 0; }

  /**
   * Reset all data memebers and free all allocated memory.
   * Called from the destructor, also can be usedto re-initialize a given
   * Interactive Objects. 
   */
  Standard_EXPORT void              Clear       ();

  /**
   * Destructor.
   */
  Standard_EXPORT virtual ~NIS_Triangulated     ();

  /**
   * Create a default drawer instance.
   */
  Standard_EXPORT virtual NIS_Drawer *
                                    DefaultDrawer (NIS_Drawer *) const;

  /**
   * Define the coordinates of node [ind].
   */
  Standard_EXPORT void              SetNode   (const Standard_Integer  ind,
                                               const gp_XYZ&           thePnt);

  /**
   * Define the coordinates of node [ind]. Z coordinate is assigned to 0.
   */
  Standard_EXPORT void              SetNode   (const Standard_Integer  ind,
                                               const gp_XY&            thePnt);

  /**
   * Define the triangle [ind] by indices of its three nodes.
   */
  Standard_EXPORT void              SetTriangle(const Standard_Integer  ind,
                                                const Standard_Integer  iNode0,
                                                const Standard_Integer  iNode1,
                                                const Standard_Integer  iNode2);

  /**
   * Allocate a single polygon, should be called for each polygon following
   * the call SetPolygonsPrs(). The polygon can be filled by node indices using
   * the method SetPolygonNode().
   * @param ind
   *   Index of the polygon, should be [0..Npolygons-1]
   * @param theSz
   *   Number of points (segments) in the polygon.
   */
  Standard_EXPORT void              SetPolygon (const Standard_Integer  ind,
                                                const Standard_Integer  theSz);

  /**
   * Define the line node by index.
   */
  Standard_EXPORT void              SetLineNode(const Standard_Integer ind,
                                                const Standard_Integer iNode);

  /**
   * Query the number of nodes.
   */
  inline Standard_Integer           NNodes    () const
  { return myNNodes; }

  /**
   * Query the number of triangles.
   */
  inline Standard_Integer           NTriangles() const
  { return myNTriangles; }

  /**
   * Query the number of line points.
   */
  inline Standard_Integer           NLineNodes() const
  { return myNLineNodes; }

  /**
   * Query the number of polygons.
   */
  inline Standard_Integer           NPolygons () const
  { return static_cast<Standard_Integer>(myNPolygons); }

  /**
   * Query the node by its index.
   * @return
   *   pointer to array of 2 or 3 Standard_ShortReal values {X,Y(,Z) coord}
   */
  inline const Standard_ShortReal * Node      (const Standard_Integer ind) const
  { return &mypNodes[ind * myNodeCoord]; }

  /**
   * Define the node of a polygon by index.
   * @param indPoly
   *   Index of the Polygon, should be less than the number of polygons that is
   *   defined in SetPolygonsPrs() and can be returned by NPOlygons().
   * @param ind
   *   Index of the node in the Polygon. Should be less than the parameter theSz
   *   in the corresponding previous SetPolygon() call.
   * @param iNode
   *   Index of the node in the given position of the Polygon. 
   */
  Standard_EXPORT void              SetPolygonNode
                                              (const Standard_Integer indPoly,
                                               const Standard_Integer ind,
                                               const Standard_Integer iNode);

  /**
   * Get the node with index 'ind' from the polygon number 'indPoly'.
   */
  Standard_EXPORT Standard_Integer PolygonNode(const Standard_Integer indPoly,
                                               const Standard_Integer ind)const;

  /**
   * Get the number of nodes for the polygon number 'indPoly'.
   */
  Standard_EXPORT Standard_Integer NPolygonNodes
                                         (const Standard_Integer indPoly)const;

  /**
   * Set the boolean flag defining if the polygons or the triangulation
   * should be drawn. This method does not affect the presentation of
   * Line/Segments.
   * @param isDrawPolygons
   *   True defines that no triangulation is drawn, only polygons are. False
   *   defines that only triangulation is drawn, no polygons.
   */
  Standard_EXPORT void              SetDrawPolygons
                                        (const Standard_Boolean isDrawPolygons);
  /**
   * Set the type of polygon rendering.
   */
  Standard_EXPORT void              SetPolygonType
                                        (const PolygonType      theType);

  /**
   * Set the normal color for presentation.
   * @param theColor
   *   New color to use for the presentation.
   */
  Standard_EXPORT void              SetColor  (const Quantity_Color&  theColor);

  /**
   * Get Normal, Transparent or Hilighted color of the presentation.
   * @param theDrawType
   *   The draw type, for which the color is retrieved.
   */
  Standard_EXPORT Quantity_Color GetColor  
                          (const NIS_Drawer::DrawType theDrawType) const;

  /**
   * Set the color for hilighted presentation.
   * @param theColor
   *   New color to use for the presentation.
   */
  Standard_EXPORT void      SetHilightColor   (const Quantity_Color&  theColor);

  /**
   * Set the color for dynamic hilight presentation.
   * @param theColor
   *   New color to use for the presentation.
   */
  Standard_EXPORT void      SetDynHilightColor(const Quantity_Color&  theColor);

  /**
   * Set the width of line presentations in pixels.
   * @param theWidth
   *   New line width to use for the presentation.
   */
  Standard_EXPORT void      SetLineWidth      (const Standard_Real    theWidth);

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
   * Intersect the InteractiveObject geometry with a line/ray.
   * @param theAxis
   *   The line or ray in 3D space.
   * @param theOver
   *   Half-thickness of the selecting line.
   * @return
   *   If the return value is more than 0.1*RealLast() then no intersection is
   *   detected. Otherwise returns the coordinate of thePnt on the ray. May be
   *   negative.
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

  /**
   * Intersect the InteractiveObject geometry with a selection polygon.
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

  Standard_EXPORT static int tri_line_intersect  (const double      start[3],
                                                  const double      dir[3],
                                                  const float       V0[3],
                                                  const float       V1[3],
                                                  const float       V2[3],
                                                  double            * tInter);

  Standard_EXPORT static int tri2d_line_intersect(const double      start[3],
                                                  const double      dir[3],
                                                  const float       V0[2],
                                                  const float       V1[2],
                                                  const float       V2[2],
                                                  double            * tInter);

  Standard_EXPORT static int seg_line_intersect  (const gp_XYZ&     aStart,
                                                  const gp_XYZ&     aDir,
                                                  const double      over2,
                                                  const float       V0[3],
                                                  const float       V1[3],
                                                  double            * tInter);

  Standard_EXPORT static int seg2d_line_intersect(const gp_XYZ&     aStart,
                                                  const gp_XYZ&     aDir,
                                                  const double      over2,
                                                  const float       V0[2],
                                                  const float       V1[2],
                                                  double            * tInter);

  Standard_EXPORT static int seg_box_intersect  (const Bnd_B3f&    theBox,
                                                 const gp_Pnt      thePnt[2]);

  Standard_EXPORT static int seg_box_included   (const Bnd_B3f&    theBox,
                                                 const gp_Pnt      thePnt[2]);

  Standard_EXPORT static int seg_polygon_intersect
                              (const NCollection_List<gp_XY> &thePolygon,
                               const gp_XY                    thePnt[2]);

  Standard_EXPORT static int seg_polygon_included
                              (const NCollection_List<gp_XY> &thePolygon,
                               const gp_XY                    thePnt[2]);

  Standard_EXPORT static void ComputeBox    (Bnd_B3f&                  theBox,
                                             const Standard_Integer    nNodes,
                                             const Standard_ShortReal* pNodes,
                                             const Standard_Integer    nCoord);

  /**
   * Classification of thePoint with respect to thePolygon.
   * @param thePolygon
   *   the list of vertices of a free-form closed polygon without
   *   self-intersections. The last point should not coincide with the first
   *   point of the list. Any two neighbor points should not be confused.
   * @param thePoint
   *   the point to be classified.
   * @return
   *   Standard_True if thePoint in inside thePolygon or lies on its boundary.
   */
  Standard_EXPORT static Standard_Boolean
                 IsIn (const NCollection_List<gp_XY> &thePolygon,
                       const gp_XY                   &thePoint);

  /**
   * Implements deallocation of the object instance
   */
  Standard_EXPORT virtual void Delete () const; 

 protected:

  /**
   * Allocator-based operator new for dynamic allocations in method Clone()
   */
  DEFINE_STANDARD_ALLOC
  DEFINE_NCOLLECTION_ALLOC

  /**
   * Create a 3D bounding box of the object.
   */
  Standard_EXPORT virtual void computeBox     ();

  /**
   * Create the memory buffer for the declared number of nodes, old nodes
   * are deallocated.
   */
  Standard_EXPORT void         allocateNodes  (const Standard_Integer nNodes);

  /**
   * Get the node pointed by the i-th index in the array.
   */
  Standard_EXPORT gp_Pnt       nodeAtInd      (const Standard_Integer * theArr,
                                               const Standard_Integer i) const;

  /**
   * Get the node pointed by the i-th index in the array.
   */
  Standard_EXPORT float*       nodeArrAtInd   (const Standard_Integer * theArr,
                                               const Standard_Integer i) const;

 protected:
  // ---------- PROTECTED FIELDS ----------

  NCollection_BaseAllocator  * myAlloc; //!< Usually from InteractiveContext
  Standard_Integer           myType;    //!< Combination of Type_* constants
  Standard_ShortReal         *  mypNodes;
  Standard_Integer           *  mypTriangles;
  Standard_Integer           *  mypLines;
  Standard_Integer           ** mypPolygons;
  Standard_Integer           myNNodes;
  Standard_Integer           myNTriangles;
  Standard_Integer           myNLineNodes;
  unsigned int               myNPolygons      : 24;
  Standard_Boolean           myIsDrawPolygons : 1;
  Standard_Boolean           myIsCloned       : 1; //!< How it is allocated
  unsigned int               myIndexType      : 2; //!< 0:8bit, 1:16bit, 2:32bit
  unsigned int               myNodeCoord      : 2; //!< 2 or 3 coordinates
  unsigned int               myPolygonType    : 2;

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (NIS_Triangulated)

  friend class NIS_TriangulatedDrawer;
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (NIS_Triangulated, NIS_InteractiveObject)

#endif
