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

#ifndef _BRepGraph_Analyze_HeaderFile
#define _BRepGraph_Analyze_HeaderFile

#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_SubGraph.hxx>
#include <NCollection_Vector.hxx>
#include <Standard_DefineAlloc.hxx>

#include <utility>

class BRepGraph;

//! @brief Static analysis methods for BRepGraph.
//!
//! BRepGraph_Analyze provides a set of diagnostic and decomposition queries
//! over a built BRepGraph.  All methods are static and take the graph
//! (and optionally a SubGraph) by const reference.
//!
//! This class is a friend of BRepGraph and BRepGraph_SubGraph, giving it
//! direct access to private node vectors for efficient traversal.
class BRepGraph_Analyze
{
public:
  DEFINE_STANDARD_ALLOC

  //! Detect free (boundary) edges: edges referenced by exactly one face.
  //! Degenerate edges are excluded.
  //! @param[in] theGraph the graph to analyze
  //! @return vector of NodeIds for free edges
  Standard_EXPORT static NCollection_Vector<BRepGraph_NodeId> FreeEdges(const BRepGraph& theGraph);

  //! All (Edge, Face) pairs missing a PCurveNode.
  //! @param[in] theGraph the graph to analyze
  //! @return vector of (EdgeId, FaceId) pairs with missing pcurves
  Standard_EXPORT static NCollection_Vector<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>>
    MissingPCurves(const BRepGraph& theGraph);

  //! Nodes with tolerance conflicts across shared geometry.
  //! @param[in] theGraph     the graph to analyze
  //! @param[in] theThreshold maximum allowed tolerance spread on a shared curve
  //! @return vector of edge NodeIds involved in tolerance conflicts
  Standard_EXPORT static NCollection_Vector<BRepGraph_NodeId>
    ToleranceConflicts(const BRepGraph& theGraph, double theThreshold);

  //! WireNodes with < 2 edges or non-closed outer wires.
  //! @param[in] theGraph the graph to analyze
  //! @return vector of degenerate wire NodeIds
  Standard_EXPORT static NCollection_Vector<BRepGraph_NodeId>
    DegenerateWires(const BRepGraph& theGraph);

  //! Split into connected components (non-owning SubGraph views).
  //! When no solids but shells exist, each shell becomes one SubGraph.
  //! @param[in] theGraph the graph to decompose
  //! @return vector of SubGraph views over connected components
  Standard_EXPORT static NCollection_Vector<BRepGraph_SubGraph>
    Decompose(const BRepGraph& theGraph);

  //! Parallel iteration over FaceNode indices in a SubGraph.
  //! @param[in] theGraph  the parent graph
  //! @param[in] theSub    SubGraph whose faces to iterate
  //! @param[in] theLambda callback invoked with each face index
  Standard_EXPORT static void ParallelForEachFace(
    const BRepGraph&                        theGraph,
    const BRepGraph_SubGraph&               theSub,
    const std::function<void(int faceIdx)>& theLambda);

  //! Parallel iteration over EdgeNode indices in a SubGraph.
  //! @param[in] theGraph  the parent graph
  //! @param[in] theSub    SubGraph whose edges to iterate
  //! @param[in] theLambda callback invoked with each edge index
  Standard_EXPORT static void ParallelForEachEdge(
    const BRepGraph&                        theGraph,
    const BRepGraph_SubGraph&               theSub,
    const std::function<void(int edgeIdx)>& theLambda);

private:
  BRepGraph_Analyze() = delete;
};

#endif // _BRepGraph_Analyze_HeaderFile
