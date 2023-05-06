// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#ifndef _BRepMesh_Delaun_HeaderFile
#define _BRepMesh_Delaun_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>

#include <BRepMesh_CircleTool.hxx>
#include <BRepMesh_Triangle.hxx>
#include <BRepMesh_Edge.hxx>
#include <IMeshData_Types.hxx>
#include <BRepMesh_DataStructureOfDelaun.hxx>
#include <BRepMesh_GeomTool.hxx>
#include <Message_ProgressRange.hxx>

class Bnd_B2d;
class Bnd_Box2d;
class BRepMesh_Vertex;

//! Compute the Delaunay's triangulation with the algorithm of Watson.
class BRepMesh_Delaun
{
public:

  DEFINE_STANDARD_ALLOC

  //! Creates instance of triangulator, but do not run the algorithm automatically.
  Standard_EXPORT BRepMesh_Delaun (const Handle(BRepMesh_DataStructureOfDelaun)& theOldMesh,
                                   const Standard_Integer                        theCellsCountU,
                                   const Standard_Integer                        theCellsCountV,
                                   const Standard_Boolean                        isFillCircles);

  //! Creates the triangulation with an empty Mesh data structure.
  Standard_EXPORT BRepMesh_Delaun (IMeshData::Array1OfVertexOfDelaun& theVertices);

  //! Creates the triangulation with an existent Mesh data structure.
  Standard_EXPORT BRepMesh_Delaun (const Handle(BRepMesh_DataStructureOfDelaun)& theOldMesh,
                                   IMeshData::Array1OfVertexOfDelaun&            theVertices);

  //! Creates the triangulation with an existant Mesh data structure.
  Standard_EXPORT BRepMesh_Delaun (const Handle(BRepMesh_DataStructureOfDelaun)& theOldMesh,
                                   IMeshData::VectorOfInteger&                   theVertexIndices);

  //! Creates the triangulation with an existant Mesh data structure.
  Standard_EXPORT BRepMesh_Delaun (const Handle (BRepMesh_DataStructureOfDelaun)& theOldMesh,
                                   IMeshData::VectorOfInteger&                    theVertexIndices,
                                   const Standard_Integer                         theCellsCountU,
                                   const Standard_Integer                         theCellsCountV);

  //! Initializes the triangulation with an array of vertices.
  Standard_EXPORT void Init (IMeshData::Array1OfVertexOfDelaun& theVertices);

  //! Forces initialization of circles cell filter using working structure.
  Standard_EXPORT void InitCirclesTool (const Standard_Integer theCellsCountU,
                                        const Standard_Integer theCellsCountV);

  //! Removes a vertex from the triangulation.
  Standard_EXPORT void RemoveVertex (const BRepMesh_Vertex& theVertex);

  //! Adds some vertices into the triangulation.
  Standard_EXPORT void AddVertices (IMeshData::VectorOfInteger&  theVerticesIndices,
                                    const Message_ProgressRange& theRange = Message_ProgressRange());

  //! Modify mesh to use the edge.
  //! @return True if done
  Standard_EXPORT Standard_Boolean UseEdge (const Standard_Integer theEdge);

  //! Gives the Mesh data structure.
  const Handle(BRepMesh_DataStructureOfDelaun)& Result() const
  {
    return myMeshData;
  }

  //! Forces insertion of constraint edges into the base triangulation. 
  void ProcessConstraints()
  {
    insertInternalEdges();

    // Adjustment of meshes to boundary edges
    frontierAdjust();
  }

  //! Gives the list of frontier edges.
  Handle(IMeshData::MapOfInteger) Frontier() const
  {
    return getEdgesByType (BRepMesh_Frontier);
  }

  //! Gives the list of internal edges.
  Handle(IMeshData::MapOfInteger) InternalEdges() const
  {
    return getEdgesByType (BRepMesh_Fixed);
  }

  //! Gives the list of free edges used only one time
  Handle(IMeshData::MapOfInteger) FreeEdges() const
  {
    return getEdgesByType (BRepMesh_Free);
  }

  //! Gives vertex with the given index
  const BRepMesh_Vertex& GetVertex (const Standard_Integer theIndex) const
  {
    return myMeshData->GetNode (theIndex);
  }

  //! Gives edge with the given index
  const BRepMesh_Edge& GetEdge (const Standard_Integer theIndex) const
  {
    return myMeshData->GetLink (theIndex);
  }

  //! Gives triangle with the given index
  const BRepMesh_Triangle& GetTriangle (const Standard_Integer theIndex) const
  {
    return myMeshData->GetElement (theIndex);
  }

  //! Returns tool used to build mesh consistent to Delaunay criteria.
  const BRepMesh_CircleTool& Circles() const
  {
    return myCircles;
  }

  //! Test is the given triangle contains the given vertex.
  //! @param theSqTolerance square tolerance to check closeness to some edge
  //! @param theEdgeOn If it is != 0 the vertex lies onto the edge index
  //!        returned through this parameter.
  Standard_EXPORT Standard_Boolean Contains (const Standard_Integer theTriangleId,
                                             const BRepMesh_Vertex& theVertex,
                                             const Standard_Real    theSqTolerance,
                                             Standard_Integer&      theEdgeOn) const;

  //! Explicitly sets ids of auxiliary vertices used to build mesh and used by 3rd-party algorithms.
  inline void SetAuxVertices (const IMeshData::VectorOfInteger& theSupVert)
  {
    mySupVert = theSupVert;
  }

  //! Destruction of auxiliary triangles containing the given vertices.
  //! Removes auxiliary vertices also.
  //! @param theAuxVertices auxiliary vertices to be cleaned up.
  Standard_EXPORT void RemoveAuxElements ();

private:

  enum ReplaceFlag
  {
    Replace,
    InsertAfter,
    InsertBefore
  };

  typedef NCollection_DataMap<Standard_Integer, IMeshData::MapOfInteger> DataMapOfMap;

  //! Performs initialization of circles cell filter tool.
  void initCirclesTool (const Bnd_Box2d&       theBox,
                        const Standard_Integer theCellsCountU,
                        const Standard_Integer theCellsCountV);

  //! Add bounding box for edge defined by start & end point to
  //! the given vector of bounding boxes for triangulation edges.
  void fillBndBox (IMeshData::SequenceOfBndB2d&  theBoxes,
                   const BRepMesh_Vertex&        theV1,
                   const BRepMesh_Vertex&        theV2);

  //! Gives the list of edges with type defined by the input parameter.
  //! If the given type is BRepMesh_Free returns list of edges
  //! that have number of connected elements less or equal 1.
  Handle(IMeshData::MapOfInteger) getEdgesByType (const BRepMesh_DegreeOfFreedom theEdgeType) const;

  //! Run triangulation procedure.
  void perform (IMeshData::VectorOfInteger& theVertexIndices,
                const Standard_Integer      theCellsCountU = -1,
                const Standard_Integer      theCellsCountV = -1);

  //! Build the super mesh.
  void superMesh (const Bnd_Box2d& theBox);

  //! Computes the triangulation and adds the vertices,
  //! edges and triangles to the Mesh data structure.
  void compute (IMeshData::VectorOfInteger& theVertexIndices);

  //! Adjust the mesh on the frontier.
  void frontierAdjust();

  //! Find left polygon of the given edge and call meshPolygon.
  Standard_Boolean meshLeftPolygonOf(
    const Standard_Integer          theEdgeIndex,
    const Standard_Boolean          isForward,
    Handle(IMeshData::MapOfInteger) theSkipped = NULL);

  //! Find next link starting from the given node and has maximum
  //! angle respect the given reference link.
  //! Each time the next link is found other neighbor links at the pivot
  //! node are marked as leprous and will be excluded from consideration
  //! next time until a hanging end is occurred.
  Standard_Integer findNextPolygonLink (const Standard_Integer&               theFirstNode,
                                        const Standard_Integer&               thePivotNode,
                                        const BRepMesh_Vertex&                thePivotVertex,
                                        const gp_Vec2d&                       theRefLinkDir,
                                        const IMeshData::SequenceOfBndB2d&    theBoxes,
                                        const IMeshData::SequenceOfInteger&   thePolygon,
                                        const Handle(IMeshData::MapOfInteger)& theSkipped,
                                        const Standard_Boolean&               isSkipLeprous,
                                        IMeshData::MapOfInteger&              theLeprousLinks,
                                        IMeshData::MapOfInteger&              theDeadLinks,
                                        Standard_Integer&                     theNextPivotNode,
                                        gp_Vec2d&                             theNextLinkDir,
                                        Bnd_B2d&                              theNextLinkBndBox);

  //! Check is the given link intersects the polygon boundaries.
  //! Returns bounding box for the given link through the theLinkBndBox parameter.
  Standard_Boolean checkIntersection (const BRepMesh_Edge&                theLink,
                                      const IMeshData::SequenceOfInteger& thePolygon,
                                      const IMeshData::SequenceOfBndB2d&  thePolyBoxes,
                                      const Standard_Boolean              isConsiderEndPointTouch,
                                      const Standard_Boolean              isConsiderPointOnEdge,
                                      const Standard_Boolean              isSkipLastEdge,
                                      Bnd_B2d&                            theLinkBndBox) const;

  //! Triangulatiion of a closed polygon described by the list
  //! of indexes of its edges in the structure.
  //! (negative index means reversed edge)
  void meshPolygon (IMeshData::SequenceOfInteger&   thePolygon,
                    IMeshData::SequenceOfBndB2d&    thePolyBoxes,
                    Handle(IMeshData::MapOfInteger) theSkipped = NULL);

  //! Decomposes the given closed simple polygon (polygon without glued edges 
  //! and loops) on two simpler ones by adding new link at the most thin part 
  //! in respect to end point of the first link.
  //! In case if source polygon consists of three links, creates new triangle 
  //! and clears source container.
  //! @param thePolygon source polygon to be decomposed (first part of decomposition).
  //! @param thePolyBoxes bounding boxes corresponded to source polygon's links.
  //! @param thePolygonCut product of decomposition of source polygon (second part of decomposition).
  //! @param thePolyBoxesCut bounding boxes corresponded to resulting polygon's links.
  void decomposeSimplePolygon (
    IMeshData::SequenceOfInteger& thePolygon,
    IMeshData::SequenceOfBndB2d&  thePolyBoxes,
    IMeshData::SequenceOfInteger& thePolygonCut,
    IMeshData::SequenceOfBndB2d&  thePolyBoxesCut);

  //! Triangulation of closed polygon containing only three edges.
  Standard_Boolean meshElementaryPolygon (const IMeshData::SequenceOfInteger& thePolygon);

  //! Creates the triangles between the given node and the given polyline.
  void createTriangles (const Standard_Integer         theVertexIndex,
                        IMeshData::MapOfIntegerInteger& thePoly);

  //! Add a triangle based on the given oriented edges into mesh
  void addTriangle (const Standard_Integer (&theEdgesId)[3],
                    const Standard_Boolean (&theEdgesOri)[3],
                    const Standard_Integer (&theNodesId)[3]);

  //! Deletes the triangle with the given index and adds the free edges into the map.
  //! When an edge is suppressed more than one time it is destroyed.
  void deleteTriangle (const Standard_Integer         theIndex,
                       IMeshData::MapOfIntegerInteger& theLoopEdges);

  //! Returns start and end nodes of the given edge in respect to its orientation.
  void getOrientedNodes (const BRepMesh_Edge&   theEdge,
                         const Standard_Boolean isForward,
                         Standard_Integer*      theNodes) const;

  //! Processes loop within the given polygon formed by range of its
  //! links specified by start and end link indices.
  void processLoop (const Standard_Integer              theLinkFrom,
                    const Standard_Integer              theLinkTo,
                    const IMeshData::SequenceOfInteger& thePolygon,
                    const IMeshData::SequenceOfBndB2d&  thePolyBoxes);

  //! Creates new link based on the given nodes and updates the given polygon.
  Standard_Integer createAndReplacePolygonLink (const Standard_Integer        theNodes[],
                                                const gp_Pnt2d                thePnts [],
                                                const Standard_Integer        theRootIndex,
                                                const ReplaceFlag             theReplaceFlag,
                                                IMeshData::SequenceOfInteger& thePolygon,
                                                IMeshData::SequenceOfBndB2d&  thePolyBoxes);
  
  //! Creates the triangles on new nodes.
  void createTrianglesOnNewVertices (IMeshData::VectorOfInteger&  theVertexIndices,
                                     const Message_ProgressRange& theRange);

  //! Cleanup mesh from the free triangles.
  void cleanupMesh();

  //! Goes through the neighbour triangles around the given node started
  //! from the given link, returns TRUE if some triangle has a bounding
  //! frontier edge or FALSE elsewhere.
  Standard_Boolean isBoundToFrontier (const Standard_Integer theRefNodeId,
                                      const Standard_Integer theRefLinkId);

  //! Remove internal triangles from the given polygon.
  void cleanupPolygon (const IMeshData::SequenceOfInteger& thePolygon,
                       const IMeshData::SequenceOfBndB2d&  thePolyBoxes);

  //! Checks is the given vertex lies inside the polygon.
  Standard_Boolean isVertexInsidePolygon (const Standard_Integer&           theVertexId,
                                          const IMeshData::VectorOfInteger& thePolygonVertices) const;

  //! Remove all triangles and edges that are placed inside the polygon or crossed it.
  void killTrianglesAroundVertex (const Standard_Integer              theZombieNodeId,
                                  const IMeshData::VectorOfInteger&   thePolyVertices,
                                  const IMeshData::MapOfInteger&      thePolyVerticesFindMap,
                                  const IMeshData::SequenceOfInteger& thePolygon,
                                  const IMeshData::SequenceOfBndB2d&  thePolyBoxes,
                                  IMeshData::MapOfInteger&            theSurvivedLinks,
                                  IMeshData::MapOfIntegerInteger&     theLoopEdges);

  //! Checks is the given link crosses the polygon boundary.
  //! If yes, kills its triangles and checks neighbor links on boundary intersection. Does nothing elsewhere.
  void killTrianglesOnIntersectingLinks (const Standard_Integer&              theLinkToCheckId,
                                         const BRepMesh_Edge&                 theLinkToCheck,
                                         const Standard_Integer&              theEndPoint,
                                         const IMeshData::SequenceOfInteger&  thePolygon,
                                         const IMeshData::SequenceOfBndB2d&   thePolyBoxes,
                                         IMeshData::MapOfInteger&             theSurvivedLinks,
                                         IMeshData::MapOfIntegerInteger&      theLoopEdges);

  //! Kill triangles bound to the given link.
  void killLinkTriangles (const Standard_Integer&         theLinkId,
                          IMeshData::MapOfIntegerInteger& theLoopEdges);

  //! Calculates distances between the given point and edges of triangle.
  Standard_Real calculateDist (const gp_XY            theVEdges[3],
                               const gp_XY            thePoints[3],
                               const BRepMesh_Vertex& theVertex,
                               Standard_Real          theDistance[3],
                               Standard_Real          theSqModulus[3],
                               Standard_Integer&      theEdgeOn) const;

  //! Checks intersection between the two segments.
  BRepMesh_GeomTool::IntFlag intSegSeg(
    const BRepMesh_Edge&   theEdge1,
    const BRepMesh_Edge&   theEdge2,
    const Standard_Boolean isConsiderEndPointTouch,
    const Standard_Boolean isConsiderPointOnEdge,
    gp_Pnt2d&              theIntPnt) const;

  //! Returns area of the loop of the given polygon defined by indices of its start and end links.
  Standard_Real polyArea (const IMeshData::SequenceOfInteger& thePolygon,
                          const Standard_Integer              theStartIndex,
                          const Standard_Integer              theEndIndex) const;

  //! Performs insertion of internal edges into mesh.
  void insertInternalEdges();

  //! Checks whether the given vertex id relates to super contour.
  Standard_Boolean isSupVertex (const Standard_Integer theVertexIdx) const
  {
    for (IMeshData::VectorOfInteger::Iterator aIt (mySupVert); aIt.More (); aIt.Next ())
    {
      if (theVertexIdx == aIt.Value ())
      {
        return Standard_True;
      }
    }

    return Standard_False;
  }

private:

  Handle(BRepMesh_DataStructureOfDelaun) myMeshData;
  BRepMesh_CircleTool                    myCircles;
  IMeshData::VectorOfInteger             mySupVert;
  Standard_Boolean                       myInitCircles;
  BRepMesh_Triangle                      mySupTrian;
};

#endif
