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

#ifndef _BRepGraph_AnalyzeView_HeaderFile
#define _BRepGraph_AnalyzeView_HeaderFile

#include <BRepGraph.hxx>
#include <BRepGraph_SubGraph.hxx>

#include <functional>
#include <utility>

//! @brief Read-only view for diagnostic and decomposition queries.
//!
//! Delegates to BRepGraph_Analyze static methods, providing free edge
//! detection, missing PCurve analysis, tolerance conflict identification,
//! degenerate wire detection, connected component decomposition, and
//! parallel iteration over face/edge indices in a SubGraph.
//! Obtained via BRepGraph::Analyze().
class BRepGraph::AnalyzeView
{
public:
  //! Detect free (boundary) edges: edges referenced by exactly one face.
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> FreeEdges() const;

  //! All (Edge, Face) pairs missing a PCurveNode.
  [[nodiscard]] Standard_EXPORT NCollection_Vector<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>>
    MissingPCurves() const;

  //! Nodes with tolerance conflicts across shared geometry.
  //! @param[in] theThreshold maximum allowed tolerance spread
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> ToleranceConflicts(
    const double theThreshold) const;

  //! WireNodes with < 2 edges or non-closed outer wires.
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> DegenerateWires() const;

  //! Split into connected components (non-owning SubGraph views).
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_SubGraph> Decompose() const;

  //! Parallel iteration over FaceNode indices in a SubGraph.
  //! @param[in] theSub    SubGraph whose faces to iterate
  //! @param[in] theLambda callback invoked with each face typed id
  Standard_EXPORT void ParallelForEachFace(
    const BRepGraph_SubGraph&                          theSub,
    const std::function<void(const BRepGraph_FaceId)>& theLambda) const;

  //! Parallel iteration over EdgeNode indices in a SubGraph.
  //! @param[in] theSub    SubGraph whose edges to iterate
  //! @param[in] theLambda callback invoked with each edge typed id
  Standard_EXPORT void ParallelForEachEdge(
    const BRepGraph_SubGraph&                          theSub,
    const std::function<void(const BRepGraph_EdgeId)>& theLambda) const;

private:
  friend class BRepGraph;

  explicit AnalyzeView(const BRepGraph* theGraph)
      : myGraph(theGraph)
  {
  }

  const BRepGraph* myGraph;
};

inline BRepGraph::AnalyzeView BRepGraph::Analyze() const
{
  return AnalyzeView(this);
}

#endif // _BRepGraph_AnalyzeView_HeaderFile
