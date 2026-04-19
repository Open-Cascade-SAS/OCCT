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

#ifndef _BRepGraph_WireExplorer_HeaderFile
#define _BRepGraph_WireExplorer_HeaderFile

#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_RefsIterator.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <NCollection_ForwardRange.hxx>
#include <NCollection_LocalArray.hxx>

class BRepGraph;

//! @brief Iterator for traversing wire edges in connection order using graph data.
//! @see BRepGraph class comment "Iterator guide" for choosing between iterator types.
//!
//! Reorders wire coedges by vertex adjacency: the end vertex of each edge
//! matches the start vertex of the next. This is the graph equivalent of
//! BRepTools_WireExplorer, operating on pre-built BRepGraph data.
//!
//! The coedges are reordered on construction (O(N^2) worst case for N coedges).
//! For most wires this is fast since N is small (4-8 edges typically).
//!
//! Internal storage uses NCollection_LocalArray with stack allocation for
//! wires with up to 16 edges (the common case), falling back to heap for larger wires.
//!
//! Usage:
//! @code
//!   BRepGraph_WireExplorer anExp(aGraph, aWireId);
//!   for (; anExp.More(); anExp.Next())
//!   {
//!     const BRepGraph_CoEdgeId aCoEdgeId = anExp.CurrentCoEdgeId();
//!     const BRepGraphInc::CoEdgeDef& aDef = aGraph.Topo().CoEdges().Definition(aCoEdgeId);
//!     // ... use aDef ...
//!   }
//! @endcode
class BRepGraph_WireExplorer
{
public:
  //! Initialize the explorer from a pre-built BRepGraph and wire identifier.
  //! Collects coedge IDs from graph iterators and reorders them by vertex connectivity.
  //! @param[in] theGraph  pre-built BRepGraph (IsDone() == true)
  //! @param[in] theWire   wire definition identifier
  BRepGraph_WireExplorer(const BRepGraph& theGraph, const BRepGraph_WireId theWire)
      : myCurrent(0),
        myLength(0)
  {
    buildOrder(theGraph, theWire);
  }

  //! Returns true if there are more edges to iterate.
  bool More() const { return myCurrent < myLength; }

  //! Advance to the next edge.
  void Next() { ++myCurrent; }

  //! Reset the iterator to the beginning (for re-iteration).
  void Reset() { myCurrent = 0; }

  //! Current coedge definition identifier in connection order.
  BRepGraph_CoEdgeId CurrentCoEdgeId() const { return myOrder[myCurrent]; }

  //! Number of coedges in the ordered sequence.
  int NbEdges() const { return myLength; }

  //! Current coedge identifier (alias for CurrentCoEdgeId(), enables range-for).
  BRepGraph_CoEdgeId Current() const { return CurrentCoEdgeId(); }

  //! Returns an STL-compatible iterator for range-based for loops.
  //! Yields BRepGraph_CoEdgeId values.
  NCollection_ForwardRangeIterator<BRepGraph_WireExplorer> begin()
  {
    return NCollection_ForwardRangeIterator<BRepGraph_WireExplorer>(this);
  }

  //! Returns a sentinel marking the end of iteration.
  NCollection_ForwardRangeSentinel end() const { return NCollection_ForwardRangeSentinel{}; }

private:
  //! Resolve the oriented start vertex of an edge.
  static BRepGraph_NodeId orientedStartVertex(const BRepGraph&             theGraph,
                                              const BRepGraphInc::EdgeDef& theEdge,
                                              const TopAbs_Orientation     theOrientation)
  {
    const BRepGraph_VertexRefId aRefId =
      (theOrientation == TopAbs_FORWARD) ? theEdge.StartVertexRefId : theEdge.EndVertexRefId;
    if (!aRefId.IsValid())
      return BRepGraph_NodeId();
    return theGraph.Refs().Vertices().Entry(aRefId).VertexDefId;
  }

  //! Resolve the oriented end vertex of an edge.
  static BRepGraph_NodeId orientedEndVertex(const BRepGraph&             theGraph,
                                            const BRepGraphInc::EdgeDef& theEdge,
                                            const TopAbs_Orientation     theOrientation)
  {
    const BRepGraph_VertexRefId aRefId =
      (theOrientation == TopAbs_FORWARD) ? theEdge.EndVertexRefId : theEdge.StartVertexRefId;
    if (!aRefId.IsValid())
      return BRepGraph_NodeId();
    return theGraph.Refs().Vertices().Entry(aRefId).VertexDefId;
  }

  //! Recursive backtracking chain: try to extend myOrder from theDepth onward,
  //! picking each unused candidate whose oriented start matches the previous
  //! oriented end. Returns true iff a full chain covering [0, theNbEdges) is built.
  bool chainRecursive(const BRepGraph&                                      theGraph,
                      const NCollection_LocalArray<BRepGraph_CoEdgeId, 16>& theInput,
                      NCollection_LocalArray<bool, 16>&                     theUsed,
                      const int                                             theDepth,
                      const int                                             theNbEdges)
  {
    if (theDepth == theNbEdges)
      return true;

    const BRepGraphInc::CoEdgeDef& aPrevCoEdge =
      theGraph.Topo().CoEdges().Definition(myOrder[theDepth - 1]);
    const BRepGraphInc::EdgeDef& aPrevEdge =
      theGraph.Topo().Edges().Definition(aPrevCoEdge.EdgeDefId);
    const BRepGraph_NodeId aPrevEnd =
      orientedEndVertex(theGraph, aPrevEdge, aPrevCoEdge.Orientation);

    for (int i = 0; i < theNbEdges; ++i)
    {
      if (theUsed[i])
        continue;
      const BRepGraphInc::CoEdgeDef& aCandCoEdge =
        theGraph.Topo().CoEdges().Definition(theInput[i]);
      const BRepGraphInc::EdgeDef& aCandEdge =
        theGraph.Topo().Edges().Definition(aCandCoEdge.EdgeDefId);
      const BRepGraph_NodeId aCandStart =
        orientedStartVertex(theGraph, aCandEdge, aCandCoEdge.Orientation);

      if (!aPrevEnd.IsValid() || !aCandStart.IsValid() || aPrevEnd != aCandStart)
        continue;

      myOrder[theDepth] = theInput[i];
      theUsed[i]        = true;
      if (chainRecursive(theGraph, theInput, theUsed, theDepth + 1, theNbEdges))
        return true;
      theUsed[i] = false;
    }
    return false;
  }

  //! Build connection-ordered coedge sequence from graph data.
  //! Uses greedy depth-first backtracking so that wires with ambiguous
  //! continuations (e.g. cylinder lateral face with a seam pair) still produce
  //! a fully connected chain whenever one exists. For pathologically disconnected
  //! wires, remaining coedges are appended in input order.
  void buildOrder(const BRepGraph& theGraph, const BRepGraph_WireId theWire)
  {
    int aNbEdges = 0;
    for (BRepGraph_RefsCoEdgeOfWire aCountIt(theGraph, theWire); aCountIt.More(); aCountIt.Next())
      ++aNbEdges;

    if (aNbEdges == 0)
      return;

    NCollection_LocalArray<BRepGraph_CoEdgeId, 16> anInput(aNbEdges);
    {
      int anIdx = 0;
      for (BRepGraph_RefsCoEdgeOfWire aCEIt(theGraph, theWire); aCEIt.More(); aCEIt.Next())
      {
        const BRepGraphInc::CoEdgeRef& aCRef = theGraph.Refs().CoEdges().Entry(aCEIt.CurrentId());
        anInput[anIdx++]                     = aCRef.CoEdgeDefId;
      }
    }

    myOrder.Allocate(aNbEdges);
    myLength = aNbEdges;

    NCollection_LocalArray<bool, 16> aUsed(aNbEdges);
    for (int i = 0; i < aNbEdges; ++i)
      aUsed[i] = false;

    myOrder[0] = anInput[0];
    aUsed[0]   = true;

    if (!chainRecursive(theGraph, anInput, aUsed, 1, aNbEdges))
    {
      // Pathologically disconnected wire: append any unused coedges in input order.
      for (int aPlaced = 1; aPlaced < aNbEdges; ++aPlaced)
      {
        for (int i = 0; i < aNbEdges; ++i)
        {
          if (!aUsed[i])
          {
            myOrder[aPlaced] = anInput[i];
            aUsed[i]         = true;
            break;
          }
        }
      }
    }
  }

  NCollection_LocalArray<BRepGraph_CoEdgeId, 16> myOrder;   //!< Ordered coedge IDs (stack for ≤16).
  int                                            myCurrent; //!< Current iteration index.
  int                                            myLength;  //!< Number of coedges.
};

#endif // _BRepGraph_WireExplorer_HeaderFile
