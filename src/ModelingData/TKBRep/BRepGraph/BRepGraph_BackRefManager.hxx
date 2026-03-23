// Copyright (c) 2026 OPEN CASCADE SAS
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

#ifndef _BRepGraph_BackRefManager_HeaderFile
#define _BRepGraph_BackRefManager_HeaderFile

#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_RelEdge.hxx>
#include <Standard_DefineAlloc.hxx>

class BRepGraph;

//! @brief Centralized back-reference maintenance for BRepGraph.
//!
//! All reverse-index updates (Surf.FaceDefUsers, Curve.EdgeDefUsers,
//! myEdgeToWires) are routed through this static-only helper.
//! This eliminates duplicated swap-and-pop logic across Builder,
//! BuilderView, Mutator, and Deduplicate.
class BRepGraph_BackRefManager
{
public:
  DEFINE_STANDARD_ALLOC

  // --- Geometry back-refs: Surface <-> FaceDef ---

  //! Register a face definition as a user of the given surface node.
  //! @param[in,out] theGraph     graph to update
  //! @param[in] theFaceDefId     face definition that references the surface
  //! @param[in] theSurfIdx       index into mySurfaces
  static Standard_EXPORT void BindFaceToSurface(BRepGraph&       theGraph,
                                                 BRepGraph_NodeId theFaceDefId,
                                                 int              theSurfIdx);

  //! Remove a face definition from the user list of the given surface node.
  //! @param[in,out] theGraph     graph to update
  //! @param[in] theFaceDefId     face definition to remove
  //! @param[in] theSurfIdx       index into mySurfaces
  static Standard_EXPORT void UnbindFaceFromSurface(BRepGraph&       theGraph,
                                                     BRepGraph_NodeId theFaceDefId,
                                                     int              theSurfIdx);

  //! Move a face definition from one surface to another.
  //! Equivalent to UnbindFaceFromSurface + BindFaceToSurface.
  //! @param[in,out] theGraph     graph to update
  //! @param[in] theFaceDefId     face definition being rewritten
  //! @param[in] theOldSurfIdx    previous surface index
  //! @param[in] theNewSurfIdx    new surface index
  static Standard_EXPORT void RewriteFaceSurface(BRepGraph&       theGraph,
                                                  BRepGraph_NodeId theFaceDefId,
                                                  int              theOldSurfIdx,
                                                  int              theNewSurfIdx);

  // --- Geometry back-refs: Curve <-> EdgeDef ---

  //! Register an edge definition as a user of the given curve node.
  //! @param[in,out] theGraph     graph to update
  //! @param[in] theEdgeDefId     edge definition that references the curve
  //! @param[in] theCurveIdx      index into myCurves
  static Standard_EXPORT void BindEdgeToCurve(BRepGraph&       theGraph,
                                               BRepGraph_NodeId theEdgeDefId,
                                               int              theCurveIdx);

  //! Remove an edge definition from the user list of the given curve node.
  //! @param[in,out] theGraph     graph to update
  //! @param[in] theEdgeDefId     edge definition to remove
  //! @param[in] theCurveIdx      index into myCurves
  static Standard_EXPORT void UnbindEdgeFromCurve(BRepGraph&       theGraph,
                                                   BRepGraph_NodeId theEdgeDefId,
                                                   int              theCurveIdx);

  //! Move an edge definition from one curve to another.
  //! Equivalent to UnbindEdgeFromCurve + BindEdgeToCurve.
  //! @param[in,out] theGraph     graph to update
  //! @param[in] theEdgeDefId     edge definition being rewritten
  //! @param[in] theOldCurveIdx   previous curve index
  //! @param[in] theNewCurveIdx   new curve index
  static Standard_EXPORT void RewriteEdgeCurve(BRepGraph&       theGraph,
                                                BRepGraph_NodeId theEdgeDefId,
                                                int              theOldCurveIdx,
                                                int              theNewCurveIdx);

  // --- Edge-to-wire back-refs ---

  //! Register an edge definition as belonging to a wire definition.
  //! @param[in,out] theGraph     graph to update
  //! @param[in] theEdgeDefIdx    edge definition index
  //! @param[in] theWireDefIdx    wire definition index
  static Standard_EXPORT void BindEdgeToWire(BRepGraph& theGraph,
                                              int        theEdgeDefIdx,
                                              int        theWireDefIdx);

  //! Remove a wire from the list of wires containing a given edge.
  //! @param[in,out] theGraph     graph to update
  //! @param[in] theEdgeDefIdx    edge definition index
  //! @param[in] theWireDefIdx    wire definition index to remove
  static Standard_EXPORT void UnbindEdgeFromWire(BRepGraph& theGraph,
                                                  int        theEdgeDefIdx,
                                                  int        theWireDefIdx);

  //! Replace an edge in the edge-to-wire reverse map for a specific wire.
  //! Removes theOldEdgeIdx -> theWireDefIdx and adds theNewEdgeIdx -> theWireDefIdx.
  //! @param[in,out] theGraph     graph to update
  //! @param[in] theOldEdgeIdx    old edge definition index
  //! @param[in] theNewEdgeIdx    new edge definition index
  //! @param[in] theWireDefIdx    wire definition index
  static Standard_EXPORT void ReplaceEdgeInWireMap(BRepGraph& theGraph,
                                                    int        theOldEdgeIdx,
                                                    int        theNewEdgeIdx,
                                                    int        theWireDefIdx);

  // --- RelEdge back-refs ---

  //! Add a directed relation edge between two nodes, updating both
  //! outgoing and incoming maps.
  //! @param[in,out] theGraph     graph to update
  //! @param[in] theFrom          source node
  //! @param[in] theTo            target node
  //! @param[in] theKind          semantic kind of the relation edge
  //! @return index of the new edge in the outgoing vector
  static Standard_EXPORT int AddRelEdge(BRepGraph&          theGraph,
                                        BRepGraph_NodeId    theFrom,
                                        BRepGraph_NodeId    theTo,
                                        BRepGraph_RelEdge::Kind theKind);

  //! Remove all relation edges of a given kind between two nodes.
  //! @param[in,out] theGraph     graph to update
  //! @param[in] theFrom          source node
  //! @param[in] theTo            target node
  //! @param[in] theKind          semantic kind to match
  static Standard_EXPORT void RemoveRelEdges(BRepGraph&          theGraph,
                                             BRepGraph_NodeId    theFrom,
                                             BRepGraph_NodeId    theTo,
                                             BRepGraph_RelEdge::Kind theKind);

  //! Remove all relation edges (both outgoing and incoming) for a node.
  //! @param[in,out] theGraph     graph to update
  //! @param[in] theNode          node to clear
  static Standard_EXPORT void ClearRelEdges(BRepGraph& theGraph,
                                            BRepGraph_NodeId theNode);

  // --- Bulk ---

  //! Clear all back-reference containers (geometry back-refs, edge-to-wire map,
  //! and relation edge maps).
  //! @param[in,out] theGraph     graph to clear back-refs for
  static Standard_EXPORT void ClearAll(BRepGraph& theGraph);

  //! Clear all back-references and rebuild them by scanning all definitions.
  //! @param[in,out] theGraph     graph to rebuild back-refs for
  static Standard_EXPORT void RebuildAll(BRepGraph& theGraph);

private:
  BRepGraph_BackRefManager() = delete;
};

#endif // _BRepGraph_BackRefManager_HeaderFile
