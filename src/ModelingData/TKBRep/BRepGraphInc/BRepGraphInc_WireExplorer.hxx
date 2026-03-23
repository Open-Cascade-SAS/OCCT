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

#ifndef _BRepGraphInc_WireExplorer_HeaderFile
#define _BRepGraphInc_WireExplorer_HeaderFile

#include <BRepGraphInc_Entity.hxx>
#include <NCollection_Vector.hxx>

#include <functional>

//! @brief Iterator for traversing wire edges in connection order using graph data.
//!
//! Reorders wire edges by vertex adjacency: the end vertex of each edge
//! matches the start vertex of the next. This is the graph equivalent of
//! BRepTools_WireExplorer, operating on BRepGraphInc entity data.
//!
//! The edges are reordered on construction (O(N^2) worst case for N edges).
//! For most wires this is fast since N is small (4-8 edges typically).
//!
//! Accepts any edge accessor — works with both BRepGraphInc_Storage and
//! higher-level views (BRepGraph::DefsView).
//!
//! Usage with BRepGraphInc_Storage:
//! @code
//!   auto edgeLookup = [&](int idx) -> const BRepGraphInc::EdgeEntity& {
//!     return aStorage.Edge(idx);
//!   };
//!   BRepGraphInc_WireExplorer anExp(aStorage.Wire(aWireIdx).EdgeRefs, edgeLookup);
//! @endcode
//!
//! Usage with BRepGraph::DefsView:
//! @code
//!   auto edgeLookup = [&](int idx) -> const BRepGraphInc::EdgeEntity& {
//!     return aDefs.Edge(idx);
//!   };
//!   BRepGraphInc_WireExplorer anExp(aDefs.Wire(aWireIdx).EdgeRefs, edgeLookup);
//! @endcode
class BRepGraphInc_WireExplorer
{
public:

  //! Edge accessor function type.
  using EdgeLookup = std::function<const BRepGraphInc::EdgeEntity&(int)>;

  //! Initialize the explorer, reordering edges by vertex connectivity.
  //! @param[in] theEdgeRefs  wire's edge reference vector
  //! @param[in] theEdgeLookup  function to get EdgeEntity by index
  BRepGraphInc_WireExplorer(const NCollection_Vector<BRepGraphInc::EdgeRef>& theEdgeRefs,
                            const EdgeLookup&                                theEdgeLookup)
    : myCurrent(0)
  {
    buildOrder(theEdgeRefs, theEdgeLookup);
  }

  //! Returns true if there are more edges to iterate.
  bool More() const { return myCurrent < myOrder.Length(); }

  //! Advance to the next edge.
  void Next() { ++myCurrent; }

  //! Current edge reference (EdgeIdx + Orientation) in connection order.
  const BRepGraphInc::EdgeRef& CurrentRef() const
  {
    return myOrder.Value(myCurrent);
  }

  //! Current edge index in storage.
  int CurrentEdgeIdx() const { return CurrentRef().EdgeIdx; }

  //! Edge orientation in the wire context (FORWARD or REVERSED).
  TopAbs_Orientation Orientation() const { return CurrentRef().Orientation; }

  //! Number of edges in the ordered sequence.
  int NbEdges() const { return myOrder.Length(); }

  //! Access the ordered edge refs vector directly.
  const NCollection_Vector<BRepGraphInc::EdgeRef>& OrderedRefs() const { return myOrder; }

private:

  //! Build connection-ordered edge sequence from EdgeRefs.
  void buildOrder(const NCollection_Vector<BRepGraphInc::EdgeRef>& theEdgeRefs,
                  const EdgeLookup&                                theEdgeLookup)
  {
    const int aNbEdges = theEdgeRefs.Length();
    if (aNbEdges == 0)
      return;

    // Track which edges have been placed.
    NCollection_Vector<bool> aUsed(aNbEdges);
    for (int i = 0; i < aNbEdges; ++i)
      aUsed.SetValue(i, false);

    // Start with the first edge.
    myOrder.Append(theEdgeRefs.Value(0));
    aUsed.SetValue(0, true);

    // Chain edges by matching end-vertex to start-vertex.
    for (int aPlaced = 1; aPlaced < aNbEdges; ++aPlaced)
    {
      const BRepGraphInc::EdgeRef& aPrevRef = myOrder.Value(aPlaced - 1);
      const BRepGraph_NodeId aPrevEnd =
        theEdgeLookup(aPrevRef.EdgeIdx).OrientedEndVertex(aPrevRef.Orientation);

      bool aFound = false;
      for (int i = 0; i < aNbEdges; ++i)
      {
        if (aUsed.Value(i))
          continue;
        const BRepGraphInc::EdgeRef& aCandRef = theEdgeRefs.Value(i);
        const BRepGraph_NodeId aCandStart =
          theEdgeLookup(aCandRef.EdgeIdx).OrientedStartVertex(aCandRef.Orientation);

        if (aPrevEnd.IsValid() && aCandStart.IsValid() && aPrevEnd == aCandStart)
        {
          myOrder.Append(aCandRef);
          aUsed.SetValue(i, true);
          aFound = true;
          break;
        }
      }
      if (!aFound)
      {
        // No connected edge found — append next unused (disconnected wire segment).
        for (int i = 0; i < aNbEdges; ++i)
        {
          if (!aUsed.Value(i))
          {
            myOrder.Append(theEdgeRefs.Value(i));
            aUsed.SetValue(i, true);
            break;
          }
        }
      }
    }
  }

  NCollection_Vector<BRepGraphInc::EdgeRef> myOrder;
  int myCurrent;
};

#endif // _BRepGraphInc_WireExplorer_HeaderFile
