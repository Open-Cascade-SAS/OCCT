// File:      NIS_Triangulated.hxx
// Created:   17.07.07 08:42
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2007


#ifndef NIS_Triangulated_HeaderFile
#define NIS_Triangulated_HeaderFile

#include <NIS_InteractiveObject.hxx>
#include <Quantity_Color.hxx>

class Handle_NIS_TriangulatedDrawer;
class NCollection_BaseAllocator;
class Handle_NCollection_BaseAllocator;

/**
 * Block of comments describing class NIS_Triangulated
 */

class NIS_Triangulated : public NIS_InteractiveObject
{
 protected:
  // Constants defining the mode (type) of presentation. They allow mixed type,
  // e.g., Triangulation+Line. Line and Segments are not mixable, their mix is
  // treated as Line only.
  enum {
    Type_None          =  0,
    Type_Loop          =  1,  // modifier to Line
    Type_Line          =  2,
    Type_Segments      =  4,
    Type_Triangulation =  8,
    Type_Polygons      = 16
  };

 public:
  // ---------- PUBLIC METHODS ----------


  /**
   * Constructor. Optionally defines the number of nodes that will be allocated
   * (this number may be defined later in methods Set*Prs) as well as the
   * memory allocator where the nodes, lines and triangles will be stored by
   * this instance.
   */
  Standard_EXPORT NIS_Triangulated(const Standard_Integer nNodes = 0,
                                   const Handle_NCollection_BaseAllocator& =0L);

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
  Standard_EXPORT virtual Handle_NIS_Drawer
                                    DefaultDrawer () const;


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
   * the call SetPolygonsPrs().
   * @param ind
   *   Index of the polygon, should be [0..Npolygons-1]
   * @param theSz
   *   Number of points (segments) in the polygon.
   * @return
   *   Pointer to the allocated buffer where you should store the indices
   *   of all polygon nodes in order, total "theSz" integers.
   */
  Standard_EXPORT Standard_Integer* SetPolygon (const Standard_Integer  ind,
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
  { return myNPolygons; }

  /**
   * Query the node by its index.
   * @return
   *   pointer to array of 3 Standard_ShortReal values (X,Y,Z coordinates)
   */
  inline const Standard_ShortReal * Node      (const Standard_Integer ind) const
  { return &mypNodes[ind*3]; }

  /**
   * Query the triangle by its index.
   * @return
   *   pointer to array of 3 Standard_Integer values (nodes 0, 1, 2)
   */
  inline const Standard_Integer *   Triangle  (const Standard_Integer ind) const
  { return &mypTriangles[ind*3]; }

  /**
   * Query the node of line or segments by index in the array of node indices.
   * This method does not make distinction of the presentation type
   * (field myType), so the correct definition of ind is to be done by the
   * caller.
   * @return
   *   pointer to the Integer value representing the index of the node.
   */
  inline const Standard_Integer *   LineNode  (const Standard_Integer ind) const
  { return &mypLines[ind]; }


  /**
   * Query the polygon.
   * @param ind
   *   rank of the polygon [0 .. N-1]
   * @param outInd
   *   <tt>[out]</tt> array of vertex indice
   * @return
   *   number of vertice in the polygon - the dimension of outInd array
   */
  inline const Standard_Integer     Polygon   (const Standard_Integer ind,
                                               Standard_Integer* & outInd) const
  { return * (outInd = mypPolygons[ind])++; }

  /**
   * Set the boolean flag defining if the polygons or the triangulation
   * should be drawn. This method does not affect the presentation of
   * Line/Segments.
   * @param isDrawPolygons
   *   True defines that no triangulation is drawn, only polygons are. False
   *   defines that only triangulation is draw, no polygons.
   * @param isUpdateV
   *   True if all views should be updated, otherwise wait till the next update
   */
  Standard_EXPORT void              SetDrawPolygons
                                        (const Standard_Boolean isDrawPolygons,
                                         const Standard_Boolean isUpdateViews
                                         = Standard_True);
  /**
   * Set the normal color for presentation.
   * @param theColor
   *   New color to use for the presentation.
   * @param isUpdateV
   *   True if all views should be updated, otherwise wait till the next update
   */
  Standard_EXPORT void              SetColor  (const Quantity_Color&  theColor,
                                               const Standard_Boolean isUpdateV
                                               = Standard_True);

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
   * @param isUpdateV
   *   True if all views should be updated, otherwise wait till the next update
   */
  Standard_EXPORT void      SetHilightColor   (const Quantity_Color&  theColor,
                                               const Standard_Boolean isUpdateV
                                               = Standard_True);

  /**
   * Set the color for dynamic hilight presentation.
   * @param theColor
   *   New color to use for the presentation.
   * @param isUpdateV
   *   True if all views should be updated, otherwise wait till the next update
   */
  Standard_EXPORT void      SetDynHilightColor(const Quantity_Color&  theColor,
                                               const Standard_Boolean isUpdateV
                                               = Standard_True);

  /**
   * Set the width of line presentations in pixels.
   * @param theWidth
   *   New line width to use for the presentation.
   * @param isUpdateV
   *   True if all views should be updated, otherwise wait till the next update
   */
  Standard_EXPORT void      SetLineWidth      (const Standard_Real    theWidth,
                                               const Standard_Boolean isUpdateV
                                               = Standard_True);

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
  Standard_EXPORT Standard_Real
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

  Standard_EXPORT static int tri_line_intersect (const double      start[3],
                                                 const double      dir[3],
                                                 const float       V0[3],
                                                 const float       V1[3],
                                                 const float       V2[3],
                                                 double            * tInter);

  Standard_EXPORT static int seg_line_intersect (const gp_XYZ&     aStart,
                                                 const gp_XYZ&     aDir,
                                                 const double      over2,
                                                 const float       V0[3],
                                                 const float       V1[3],
                                                 double            * tInter);

  Standard_EXPORT static int seg_box_intersect  (const Bnd_B3f&    theBox,
                                                 const gp_Pnt      thePnt[2]);

  Standard_EXPORT static int seg_box_included   (const Bnd_B3f&    theBox,
                                                 const gp_Pnt      thePnt[2]);

  Standard_EXPORT static void ComputeBox    (Bnd_B3f&                  theBox,
                                             const Standard_Integer    nNodes,
                                             const Standard_ShortReal* pNodes);

 protected:

  /**
   * Create a 3D bounding box of the object.
   */
  Standard_EXPORT virtual void computeBox     ();

  /**
   * Create the memory buffer for the declared number of nodes, old nodes
   * are deallocated.
   */
  Standard_EXPORT void         allocateNodes  (const Standard_Integer nNodes);

 protected:
  // ---------- PROTECTED FIELDS ----------

  /**
   * Combination of Type_* constants
   */
  Standard_Integer                 myType;
  Standard_ShortReal               *  mypNodes;
  Standard_Integer                 *  mypTriangles;
  Standard_Integer                 *  mypLines;
  Standard_Integer                 ** mypPolygons;
  Standard_Integer                 myNNodes;
  Standard_Integer                 myNTriangles;
  Standard_Integer                 myNPolygons;
  Standard_Integer                 myNLineNodes;
  NCollection_BaseAllocator        * myAlloc;
  Standard_Boolean                 myIsDrawPolygons;

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (NIS_Triangulated)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (NIS_Triangulated, NIS_InteractiveObject)

#endif
