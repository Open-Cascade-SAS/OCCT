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
//! Reorders wire coedges by vertex adjacency: the end vertex of each edge
//! matches the start vertex of the next. This is the graph equivalent of
//! BRepTools_WireExplorer, operating on BRepGraphInc entity data.
//!
//! The coedges are reordered on construction (O(N^2) worst case for N coedges).
//! For most wires this is fast since N is small (4-8 edges typically).
//!
//! Accepts any edge/coedge accessor - works with both BRepGraphInc_Storage and
//! higher-level views (BRepGraph::DefsView).
//!
//! Usage with BRepGraphInc_Storage:
//! @code
//!   auto edgeLookup = [&](int idx) -> const BRepGraphInc::EdgeEntity& {
//!     return aStorage.Edge(idx);
//!   };
//!   auto coedgeLookup = [&](int idx) -> const BRepGraphInc::CoEdgeEntity& {
//!     return aStorage.CoEdge(idx);
//!   };
//!   BRepGraphInc_WireExplorer anExp(aStorage.Wire(aWireIdx).CoEdgeRefs,
//!                                    coedgeLookup, edgeLookup);
//! @endcode
class BRepGraphInc_WireExplorer
{
public:
  //! Edge accessor function type.
  using EdgeLookup = std::function<const BRepGraphInc::EdgeEntity&(int)>;

  //! CoEdge accessor function type.
  using CoEdgeLookup = std::function<const BRepGraphInc::CoEdgeEntity&(int)>;

  //! Initialize the explorer, reordering coedges by vertex connectivity.
  //! @param[in] theCoEdgeRefs  wire's coedge reference vector
  //! @param[in] theCoEdgeLookup  function to get CoEdgeEntity by index
  //! @param[in] theEdgeLookup  function to get EdgeEntity by index
  BRepGraphInc_WireExplorer(const NCollection_Vector<BRepGraphInc::CoEdgeRef>& theCoEdgeRefs,
                            const CoEdgeLookup&                                theCoEdgeLookup,
                            const EdgeLookup&                                  theEdgeLookup)
      : myCurrent(0)
  {
    buildOrder(theCoEdgeRefs, theCoEdgeLookup, theEdgeLookup);
  }

  //! Returns true if there are more edges to iterate.
  bool More() const { return myCurrent < myOrder.Length(); }

  //! Advance to the next edge.
  void Next() { ++myCurrent; }

  //! Current coedge reference in connection order.
  const BRepGraphInc::CoEdgeRef& CurrentRef() const { return myOrder.Value(myCurrent); }

  //! Current coedge index in storage.
  int CurrentCoEdgeIdx() const { return CurrentRef().CoEdgeIdx; }

  //! Number of coedges in the ordered sequence.
  int NbEdges() const { return myOrder.Length(); }

  //! Access the ordered coedge refs vector directly.
  const NCollection_Vector<BRepGraphInc::CoEdgeRef>& OrderedRefs() const { return myOrder; }

private:
  //! Build connection-ordered coedge sequence from CoEdgeRefs.
  void buildOrder(const NCollection_Vector<BRepGraphInc::CoEdgeRef>& theCoEdgeRefs,
                  const CoEdgeLookup&                                theCoEdgeLookup,
                  const EdgeLookup&                                  theEdgeLookup)
  {
    const int aNbEdges = theCoEdgeRefs.Length();
    if (aNbEdges == 0)
      return;

    // Track which coedges have been placed.
    NCollection_Vector<bool> aUsed(aNbEdges);
    for (int i = 0; i < aNbEdges; ++i)
      aUsed.SetValue(i, false);

    // Start with the first coedge.
    myOrder.Append(theCoEdgeRefs.Value(0));
    aUsed.SetValue(0, true);

    // Chain coedges by matching end-vertex to start-vertex.
    for (int aPlaced = 1; aPlaced < aNbEdges; ++aPlaced)
    {
      const BRepGraphInc::CoEdgeRef&    aPrevRef    = myOrder.Value(aPlaced - 1);
      const BRepGraphInc::CoEdgeEntity& aPrevCoEdge = theCoEdgeLookup(aPrevRef.CoEdgeIdx);
      const BRepGraph_NodeId            aPrevEnd =
        theEdgeLookup(aPrevCoEdge.EdgeIdx).OrientedEndVertex(aPrevCoEdge.Sense);

      bool aFound = false;
      for (int i = 0; i < aNbEdges; ++i)
      {
        if (aUsed.Value(i))
          continue;
        const BRepGraphInc::CoEdgeRef&    aCandRef    = theCoEdgeRefs.Value(i);
        const BRepGraphInc::CoEdgeEntity& aCandCoEdge = theCoEdgeLookup(aCandRef.CoEdgeIdx);
        const BRepGraph_NodeId            aCandStart =
          theEdgeLookup(aCandCoEdge.EdgeIdx).OrientedStartVertex(aCandCoEdge.Sense);

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
        // No connected coedge found - append next unused (disconnected wire segment).
        for (int i = 0; i < aNbEdges; ++i)
        {
          if (!aUsed.Value(i))
          {
            myOrder.Append(theCoEdgeRefs.Value(i));
            aUsed.SetValue(i, true);
            break;
          }
        }
      }
    }
  }

  NCollection_Vector<BRepGraphInc::CoEdgeRef> myOrder;
  int                                         myCurrent;
};

#endif // _BRepGraphInc_WireExplorer_HeaderFile
