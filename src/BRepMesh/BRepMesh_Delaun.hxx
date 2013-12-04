// Copyright (c) 2013 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _BRepMesh_Delaun_HeaderFile
#define _BRepMesh_Delaun_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>

#include <BRepMesh_CircleTool.hxx>
#include <BRepMesh_Triangle.hxx>
#include <BRepMesh_Edge.hxx>
#include <BRepMesh_MapOfInteger.hxx>
#include <BRepMesh_MapOfIntegerInteger.hxx>
#include <BRepMesh_DataStructureOfDelaun.hxx>
#include <NCollection_Sequence.hxx>

class Bnd_B2d;
class Bnd_Box2d;
class BRepMesh_Array1OfVertexOfDelaun;
class BRepMesh_Vertex;
class TColStd_Array1OfInteger;
class TColStd_SequenceOfInteger;
class TColStd_MapOfInteger;

//! Compute the Delaunay's triangulation with the <br>
//! algorithm of Watson. <br>
class BRepMesh_Delaun  {
public:

  DEFINE_STANDARD_ALLOC

  //! Creates the triangulation with an empty Mesh <br>
  //! data structure. <br>
  Standard_EXPORT BRepMesh_Delaun( BRepMesh_Array1OfVertexOfDelaun& theVertices,
                                   const Standard_Boolean           isPositive = Standard_True );

  //! Creates the triangulation with and existent <br>
  //! Mesh data structure. <br>
  Standard_EXPORT BRepMesh_Delaun( const Handle(BRepMesh_DataStructureOfDelaun)& theOldMesh,
                                   BRepMesh_Array1OfVertexOfDelaun&              theVertices,
                                   const Standard_Boolean                        isPositive = Standard_True );

  //! Creates the triangulation with and existant <br>
  //! Mesh data structure. <br>
  Standard_EXPORT BRepMesh_Delaun( const Handle(BRepMesh_DataStructureOfDelaun)& theOldMesh,
                                   TColStd_Array1OfInteger&                      theVertexIndices,
                                   const Standard_Boolean                        isPositive = Standard_True );

  //! Initializes the triangulation with an array of <br>
  //! vertices. <br>
  Standard_EXPORT void Init( BRepMesh_Array1OfVertexOfDelaun& theVertices );

  //! Removes a vertex from the triangulation. <br>
  Standard_EXPORT void RemoveVertex( const BRepMesh_Vertex& theVertex );

  //! Adds some vertices into the triangulation. <br>
  Standard_EXPORT void AddVertices( BRepMesh_Array1OfVertexOfDelaun& theVertices );

  //! Modify mesh to use the edge. Return True if done. <br>
  Standard_EXPORT Standard_Boolean UseEdge( const Standard_Integer theEdge );

  //! Gives the Mesh data structure. <br>
  Standard_EXPORT const Handle(BRepMesh_DataStructureOfDelaun)& Result() const
  {
    return myMeshData;
  }

  //! Gives the list of frontier edges <br>
  inline Handle(BRepMesh_MapOfInteger) Frontier() const
  {
    return getEdgesByType( BRepMesh_Frontier );
  }

  //! Gives the list of internal edges <br>
  inline Handle(BRepMesh_MapOfInteger) InternalEdges() const
  {
    return getEdgesByType( BRepMesh_Fixed );
  }

  //! Gives the list of free edges used only one time <br>
  inline Handle(BRepMesh_MapOfInteger) FreeEdges() const
  {
    return getEdgesByType( BRepMesh_Free );
  }
  
  //! Gives vertex with the given index <br>
  inline const BRepMesh_Vertex& GetVertex( const Standard_Integer theIndex ) const
  {
    return myMeshData->GetNode( theIndex );
  }

  //! Gives edge with the given index <br>
  inline const BRepMesh_Edge& GetEdge( const Standard_Integer theIndex ) const
  {
    return myMeshData->GetLink( theIndex );
  }

  //! Gives triangle with the given index <br>
  inline const BRepMesh_Triangle& GetTriangle( const Standard_Integer theIndex ) const
  {
    return myMeshData->GetElement( theIndex );
  }

  //! Test is the given triangle contains the given vertex. <br>
  //! If <theEdgeOn> != 0 the vertex lies onto the edge index <br>
  //! returned through this parameter. <br>
  Standard_EXPORT Standard_Boolean Contains( const Standard_Integer theTriangleId,
                                             const BRepMesh_Vertex& theVertex,
                                             Standard_Integer&      theEdgeOn ) const;

private:

  enum IntFlag
  {
    NoIntersection,
    Cross,
    EndPointTouch,
    PointOnEdge,
    Glued,
    Same
  };

  enum ReplaceFlag
  {
    Replace,
    InsertAfter,
    InsertBefore
  };

  typedef NCollection_DataMap<Standard_Integer, NCollection_Map<Standard_Integer> > DataMapOfMap;

  typedef NCollection_Handle<NCollection_Map<Standard_Integer> > HandleOfMapOfInteger;

  //! Add boundig box for edge defined by start & end point to <br>
  //! the given vector of bounding boxes for triangulation edges  <br>
  void fillBndBox( NCollection_Sequence<Bnd_B2d>&   theBoxes,
                   const BRepMesh_Vertex&           theV1,
                   const BRepMesh_Vertex&           theV2 );

  //! Gives the list of edges with type defined by the input parameter. <br>
  //! If the given type is BRepMesh_Free returns list of edges <br>
  //! that have number of connected elements less or equal 1  <br>
  Handle(BRepMesh_MapOfInteger) getEdgesByType( const BRepMesh_DegreeOfFreedom theEdgeType ) const;

  //! Create super mesh and run triangulation procedure <br>
  void perform( Bnd_Box2d&               theBndBox,
                TColStd_Array1OfInteger& theVertexIndices );

  //! Build the super mesh. <br>
  void superMesh( const Bnd_Box2d& theBox );

  //! Computes the triangulation and adds the vertices, <br>
  //! edges and triangles to the Mesh data structure. <br>
  void compute( TColStd_Array1OfInteger& theVertexIndices );

  //! Adjust the mesh on the frontier. <br>
  void frontierAdjust();

  //! Find left polygon of the given edge and call meshPolygon. <br>
  Standard_Boolean meshLeftPolygonOf( const Standard_Integer theEdgeIndex,
                                      const Standard_Boolean isForward,
                                      HandleOfMapOfInteger   theSkipped = NULL );

  //! Find next link starting from the given node and has maximum <br>
  //! angle respect the given reference link. <br>
  //! Each time the next link is found other neighbor links at the pivot <br>
  //! node are marked as leprous and will be excluded from consideration <br>
  //! next time until a hanging end is occured. <br>
  Standard_Integer findNextPolygonLink( const Standard_Integer&              theFirstNode,
                                        const Standard_Integer&              thePivotNode,
                                        const BRepMesh_Vertex&               thePivotVertex,
                                        const gp_Vec2d&                      theRefLinkDir,
                                        const NCollection_Sequence<Bnd_B2d>& theBoxes,
                                        const TColStd_SequenceOfInteger&     thePolygon,
                                        const HandleOfMapOfInteger           theSkipped,
                                        const Standard_Boolean&              isSkipLeprous,
                                        NCollection_Map<Standard_Integer>&   theLeprousLinks,
                                        NCollection_Map<Standard_Integer>&   theDeadLinks,
                                        Standard_Integer&                    theNextPivotNode,
                                        gp_Vec2d&                            theNextLinkDir,
                                        Bnd_B2d&                             theNextLinkBndBox );

  //! Check is the given link intersects the polygon boundaries. <br>
  //! Returns bounding box for the given link trough the <theLinkBndBox> parameter. <br>
  Standard_Boolean checkIntersection( const BRepMesh_Edge&                 theLink,
                                      const TColStd_SequenceOfInteger&     thePolygon,
                                      const NCollection_Sequence<Bnd_B2d>& thePolyBoxes,
                                      const Standard_Boolean               isConsiderEndPointTouch,
                                      const Standard_Boolean               isConsiderPointOnEdge,
                                      const Standard_Boolean               isSkipLastEdge,
                                      Bnd_B2d&                             theLinkBndBox ) const;

  //! Triangulatiion of a closed polygon described by the list <br>
  //! of indexes of its edges in the structure. <br>
  //! (negative index means reversed edge) <br>
  void meshPolygon( TColStd_SequenceOfInteger&     thePolygon,
                    NCollection_Sequence<Bnd_B2d>& thePolyBoxes,
                    HandleOfMapOfInteger           theSkipped = NULL );

  //! Triangulatiion of a closed simple polygon (polygon without glued edges and loops) <br>
  //! described by the list of indexes of its edges in the structure. <br>
  //! (negative index means reversed edge) <br>
  void meshSimplePolygon( TColStd_SequenceOfInteger&     thePolygon,
                          NCollection_Sequence<Bnd_B2d>& thePolyBoxes );

  //! Triangulation of closed polygon containing only three edges.
  inline Standard_Boolean meshElementaryPolygon( const TColStd_SequenceOfInteger& thePolygon );

  //! Creates the triangles beetween the given node <br>
  //! and the given polyline. <br>
  void createTriangles( const Standard_Integer        theVertexIndex,
                        BRepMesh_MapOfIntegerInteger& thePoly );

  //! Add a triangle based on the given oriented edges into mesh <br>
  inline void addTriangle( const Standard_Integer (&theEdgesId)[3],
                           const Standard_Boolean (&theEdgesOri)[3],
                           const Standard_Integer (&theNodesId)[3] );

  //! Deletes the triangle with the given index and <br>
  //! adds the free edges into the map. <br>
  //! When an edge is suppressed more than one time <br>
  //! it is destroyed. <br>
  void deleteTriangle( const Standard_Integer        theIndex,
                       BRepMesh_MapOfIntegerInteger& theLoopEdges );

  //! Returns start and end nodes of the given edge <br>
  //! in respect to its orientation. <br>
  void getOrientedNodes(const BRepMesh_Edge&   theEdge,
                        const Standard_Boolean isForward,
                        Standard_Integer       *theNodes) const;

  //! Processes loop within the given polygon formed by range of its <br>
  //! links specified by start and end link indices. <br>
  void processLoop(const Standard_Integer               theLinkFrom,
                   const Standard_Integer               theLinkTo,
                   const TColStd_SequenceOfInteger&     thePolygon,
                   const NCollection_Sequence<Bnd_B2d>& thePolyBoxes);

  //! Creates new link based on the given nodes and updates the given polygon.
  Standard_Integer createAndReplacePolygonLink(const Standard_Integer         theNodes[],
                                               const gp_Pnt2d                 thePnts [],
                                               const Standard_Integer         theRootIndex,
                                               const ReplaceFlag              theReplaceFlag,
                                               TColStd_SequenceOfInteger&     thePolygon,
                                               NCollection_Sequence<Bnd_B2d>& thePolyBoxes);
  
  //! Creates the triangles on new nodes <br>
  void createTrianglesOnNewVertices( TColStd_Array1OfInteger& theVertexIndices );

  //! Cleanup mesh from the free triangles <br>
  void cleanupMesh();

  //! Goes through the neighbour triangles around the given node started
  //! from the given link, returns TRUE if some triangle has a bounding
  //! frontier edge or FALSE elsewhere.
  //! Stop link is used to prevent cycles.
  //! Previous element Id is used to identify next neighbor element.
  Standard_Boolean isBoundToFrontier(const Standard_Integer theRefNodeId,
                                     const Standard_Integer theRefLinkId,
                                     const Standard_Integer theStopLinkId,
                                     const Standard_Integer thePrevElementId);

  //! Remove internal triangles from the given polygon <br>
  void cleanupPolygon( const TColStd_SequenceOfInteger&     thePolygon,
                       const NCollection_Sequence<Bnd_B2d>& thePolyBoxes );

  //! Checks is the given vertex lies inside the polygon <br>
  Standard_Boolean isVertexInsidePolygon( const Standard_Integer&                     theVertexId,
                                          const NCollection_Vector<Standard_Integer>& thePolygonVertices ) const;

  //! Remove all triangles and edges that are placed <br>
  //! inside the polygon or crossed it. <br>
  void killTrianglesAroundVertex( const Standard_Integer                      theZombieNodeId,
                                  const NCollection_Vector<Standard_Integer>& thePolyVertices,
                                  const NCollection_Map<Standard_Integer>&    thePolyVerticesFindMap,
                                  const TColStd_SequenceOfInteger&            thePolygon,
                                  const NCollection_Sequence<Bnd_B2d>&        thePolyBoxes,
                                  NCollection_Map<Standard_Integer>&          theSurvivedLinks,
                                  BRepMesh_MapOfIntegerInteger&               theLoopEdges );

  //! Checks is the given link crosses the polygon boundary. <br>
  //! If yes, kills its triangles and checks neighbor links on <br>
  //! boundary intersection. Does nothing elsewhere. <br>
  void killTrianglesOnIntersectingLinks( const Standard_Integer&              theLinkToCheckId, 
                                         const BRepMesh_Edge&                 theLinkToCheck,
                                         const Standard_Integer&              theEndPoint,
                                         const TColStd_SequenceOfInteger&     thePolygon,
                                         const NCollection_Sequence<Bnd_B2d>& thePolyBoxes,
                                         NCollection_Map<Standard_Integer>&   theSurvivedLinks,
                                         BRepMesh_MapOfIntegerInteger&        theLoopEdges );

  //! Kill triangles bound to the given link. <br>
  void killLinkTriangles( const Standard_Integer&       theLinkId, 
                          BRepMesh_MapOfIntegerInteger& theLoopEdges );

  //! Calculates distances between the given point <br>
  //! and edges of triangle. <br>
  Standard_Real calculateDist( const gp_XY            theVEdges[3],
                               const gp_XY            thePoints[3],
                               const Standard_Integer theEdgesId[3],
                               const BRepMesh_Vertex& theVertex,
                               Standard_Real          theDistance[3],
                               Standard_Real          theSqModulus[3],
                               Standard_Integer&      theEdgeOn ) const;

  //! Classifies the point in case of coincidence of two vectors. <br>
  //! Returns zero value if point is out of segment and non zero  <br>
  //! value if point is between the first and the second point of segment. <br>
  //! thePoint1       - the start point of a segment (base point) <br>
  //! thePoint2       - the end point of a segment <br>
  //! thePointToCheck - the point to classify <br>
  Standard_Integer classifyPoint( const gp_XY& thePoint1,
                                  const gp_XY& thePoint2,
                                  const gp_XY& thePointToCheck ) const;

  //! Checks intersection between the two segments. <br>
  IntFlag intSegSeg( const BRepMesh_Edge&   theEdge1,
                     const BRepMesh_Edge&   theEdge2,
                     const Standard_Boolean isConsiderEndPointTouch,
                     const Standard_Boolean isConsiderPointOnEdge,
                     gp_Pnt2d&              theIntPnt) const;

  //! Returns area of the loop of the given polygon defined by <br>
  //! indices of its start and end links. <br>
  Standard_Real polyArea( const TColStd_SequenceOfInteger& thePolygon,
                          const Standard_Integer           theStartIndex,
                          const Standard_Integer           theEndIndex ) const;
private:

  Handle(BRepMesh_DataStructureOfDelaun)          myMeshData;
  Standard_Boolean                                myIsPositiveOrientation;
  BRepMesh_CircleTool                             myCircles;
  Standard_Integer                                mySupVert[3];
  BRepMesh_Triangle                               mySupTrian;
};

#endif
