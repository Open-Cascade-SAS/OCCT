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

#ifndef _BRepGraphAlgo_Sewing_HeaderFile
#define _BRepGraphAlgo_Sewing_HeaderFile

#include <BRepGraph.hxx>

#include <NCollection_Array1.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_KDTree.hxx>
#include <NCollection_Map.hxx>
#include <Standard_DefineAlloc.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Pnt.hxx>

class ExtremaPC_Curve;

#include <utility>

//! @brief Graph-based edge sewing algorithm.
//!
//! BRepGraphAlgo_Sewing takes one or more disconnected shapes (typically
//! individual faces), builds a BRepGraph over them, detects free (boundary)
//! edges, matches geometrically coincident free-edge pairs within the
//! given tolerance, and merges them using BRepGraph generic mutation APIs.
//!
//! The algorithm follows these phases (matching BRepBuilderAPI_Sewing logic):
//! 1. Face Analysis -- build graph, detect degenerate edges
//! 2. Find Free Edges -- boundary edge detection via face-reference counting
//! 3. Vertex Assembling -- merge coincident free-edge vertices
//! 4. Edge Matching -- BBox pre-filter + geometric validation
//! 5. Edge Merging -- pcurve transfer, wire updates, history recording
//! 6. Edge Processing -- tolerance and pcurve consistency
//! 7. Shape Reconstruction -- rebuild affected faces from graph state
//! 8. Edge Regularity -- set continuity on sewn edges
//!
//! ## Typical usage
//! @code
//!   BRepGraphAlgo_Sewing aSewer(1.0e-06);
//!   aSewer.Add(face1);
//!   aSewer.Add(face2);
//!   aSewer.Perform();
//!   if (aSewer.IsDone())
//!   {
//!     const TopoDS_Shape& aResult = aSewer.Result();
//!   }
//! @endcode
class BRepGraphAlgo_Sewing
{
public:
  DEFINE_STANDARD_ALLOC

  //! Construct with a sewing tolerance.
  //! @param[in] theTolerance maximum distance for edge matching (default 1.0e-06)
  Standard_EXPORT explicit BRepGraphAlgo_Sewing(double theTolerance = 1.0e-06);

  //! Add a shape to be sewn.
  //! @param[in] theShape shape to include (Face, Shell, Compound, etc.)
  Standard_EXPORT void Add(const TopoDS_Shape& theShape);

  // --- Configuration ---

  //! Enable/disable face analysis phase.
  Standard_EXPORT void SetFaceAnalysis(bool theVal);

  //! Enable/disable edge cutting at intersection points.
  Standard_EXPORT void SetCutting(bool theVal);

  //! Enable/disable same-parameter mode on sewn edges.
  Standard_EXPORT void SetSameParameterMode(bool theVal);

  //! Enable/disable non-manifold mode (allow >2 faces per edge).
  Standard_EXPORT void SetNonManifoldMode(bool theVal);

  //! Enable/disable parallel execution.
  Standard_EXPORT void SetParallel(bool theVal);

  //! Query face analysis mode.
  Standard_EXPORT bool FaceAnalysis() const;

  //! Query cutting mode.
  Standard_EXPORT bool Cutting() const;

  //! Query same-parameter mode.
  Standard_EXPORT bool SameParameterMode() const;

  //! Query non-manifold mode.
  Standard_EXPORT bool NonManifoldMode() const;

  //! Query parallel mode.
  Standard_EXPORT bool IsParallel() const;

  //! Enable/disable history recording during sewing.
  //! Forwards to BRepGraph::SetHistoryEnabled().
  Standard_EXPORT void SetHistoryMode(bool theVal);

  //! Query history recording mode.
  Standard_EXPORT bool HistoryMode() const;

  // --- Execution ---

  //! Run the sewing algorithm.
  Standard_EXPORT void Perform();

  //! True after successful Perform().
  Standard_EXPORT bool IsDone() const;

  // --- Results ---

  //! The sewn result shape.
  Standard_EXPORT const TopoDS_Shape& Result() const;

  //! Access the internal graph (valid after Perform()).
  Standard_EXPORT const BRepGraph& Graph() const;

  //! Number of free (boundary) edges detected before sewing.
  Standard_EXPORT int NbFreeEdgesBefore() const;

  //! Number of free (boundary) edges remaining after sewing.
  Standard_EXPORT int NbFreeEdgesAfter() const;

  //! Number of edge pairs that were successfully sewn.
  Standard_EXPORT int NbSewnEdges() const;

private:
  // Configuration.
  double myTolerance;
  bool   myFaceAnalysis  = true;
  bool   myCutting       = true;
  bool   mySameParameter = true;
  bool   myNonManifold   = false;
  bool   myIsParallel    = false;
  bool   myHistoryMode   = true;

  // State.
  bool                                 myIsDone = false;
  BRepGraph                            myGraph;
  TopoDS_Shape                         myResult;
  NCollection_Vector<TopoDS_Shape>     myInputShapes;
  NCollection_Array1<BRepGraph_NodeId> myFreeEdgesBefore;
  int                                  myFreeEdgesAfter = 0;
  int                                  mySewnCount      = 0;

  // Per-phase methods.

  //! Phase 1: Build graph and analyze faces.
  //! @param[in] theAllocator arena allocator to propagate into the graph
  void analyzeFaces(const Handle(NCollection_IncAllocator)& theAllocator);

  //! Phase 2: Detect free (boundary) edges.
  void findFreeEdges();

  //! Phase 3: Merge coincident free-edge vertices.
  //! @param[in] theTmpAlloc temporary IncAllocator for scratch collections
  void assembleVertices(const Handle(NCollection_IncAllocator)& theTmpAlloc);

  //! Phase 4a: Detect sewing candidates via BBox overlap.
  //! @param[in] theTmpAlloc temporary IncAllocator for scratch collections
  void detectCandidates(const Handle(NCollection_IncAllocator)& theTmpAlloc);

  //! Phase 4b: Cut edges at intersection points (if myCutting enabled).
  //! @param[in] theTmpAlloc temporary IncAllocator for scratch collections
  void cutAtIntersections(const Handle(NCollection_IncAllocator)& theTmpAlloc);

  //! Phase 5a: Match free-edge pairs using BBox pre-filter + geometric validation.
  //! @param[in] theTmpAlloc temporary IncAllocator for scratch collections
  //! @return sequence of matched (keepEdge, removeEdge) node-id pairs
  NCollection_Vector<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>> matchFreeEdges(
    const Handle(NCollection_IncAllocator)& theTmpAlloc);

  //! Phase 5b: Merge matched edge pairs in the graph.
  //! @param[in] thePairs matched edge pairs
  //! @param[out] theAffectedFaces set of face indices that need reconstruction
  //! @param[out] theSewnEdgeIndices indices of surviving (keep) edges
  void mergeMatchedEdges(
    const NCollection_Vector<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>>& thePairs,
    NCollection_Map<int>&                                                      theAffectedFaces,
    NCollection_Map<int>&                                                      theSewnEdgeIndices);

  //! Phase 6: Process sewn edges (tolerance, pcurve consistency).
  //! @param[in] theSewnEdgeIndices indices of edges that were sewn
  void processEdges(const NCollection_Map<int>& theSewnEdgeIndices);

  //! Phase 7: Reconstruct result shape from graph state.
  //! @param[in] theAffectedFaces set of face indices that were modified
  void reconstructResult(const NCollection_Map<int>& theAffectedFaces);

  //! Phase 8: Set edge regularity on sewn edges.
  void setEdgeRegularity();

  //! Geometric test: can these two edges be merged within tolerance?
  //! Uses 5-point sampling with bidirectional projection.
  //! @param[in] theEdgeA first edge node id
  //! @param[in] theEdgeB second edge node id
  //! @return true if edges are geometrically compatible
  bool areEdgesSewable(BRepGraph_NodeId theEdgeA, BRepGraph_NodeId theEdgeB) const;

  //! Cached variant of areEdgesSewable that reuses pre-computed edgeA data.
  //! This avoids redundant curve adaptor / GCPnts_UniformAbscissa / ExtremaPC_Curve
  //! construction when the same edgeA is compared against multiple edgeB candidates.
  //! @param[in] theEdgeA       first edge node id
  //! @param[in] theEdgeB       second edge node id
  //! @param[in] theSamplePtsA  pre-sampled points on edgeA
  //! @param[in] theExtPCRevA   ExtremaPC_Curve initialized on curveA (reused for reverse pass)
  //! @param[in] theChordA      chord length of edgeA (start-to-end distance)
  //! @return true if edges are geometrically compatible
  bool areEdgesSewable(BRepGraph_NodeId                  theEdgeA,
                       BRepGraph_NodeId                  theEdgeB,
                       const NCollection_Array1<gp_Pnt>& theSamplePtsA,
                       const ExtremaPC_Curve&            theExtPCRevA,
                       double                            theChordA) const;
};

#endif // _BRepGraphAlgo_Sewing_HeaderFile
