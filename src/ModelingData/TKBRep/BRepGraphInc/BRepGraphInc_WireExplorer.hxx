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
#include <NCollection_Array1.hxx>
#include <NCollection_Vector.hxx>

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
//! higher-level views (BRepGraph::TopoView).
//!
//! Usage with BRepGraphInc_Storage:
//! @code
//!   auto edgeLookup = [&](const int idx) -> const BRepGraphInc::EdgeEntity& {
//!     return aStorage.Edge(idx);
//!   };
//!   auto coedgeLookup = [&](const int idx) -> const BRepGraphInc::CoEdgeEntity& {
//!     return aStorage.CoEdge(idx);
//!   };
//!   auto vtxRefLookup = [&](const BRepGraph_VertexRefId id) -> BRepGraph_VertexId {
//!     return aStorage.VertexRefEntry(id).VertexDefId;
//!   };
//!   BRepGraphInc_WireExplorer anExp(aStorage.Wire(aWireIdx).CoEdgeRefs,
//!                                    coedgeLookup, edgeLookup, vtxRefLookup);
//! @endcode
class BRepGraphInc_WireExplorer
{
public:
  //! Initialize the explorer, reordering coedges by vertex connectivity.
  //! @param[in] theCoEdgeRefs     wire's coedge reference vector
  //! @param[in] theCoEdgeLookup   function to get CoEdgeEntity by index
  //! @param[in] theEdgeLookup     function to get EdgeEntity by index
  //! @param[in] theVtxRefLookup   function to resolve VertexRefId to VertexDefId
  template <typename CoEdgeLookupT, typename EdgeLookupT, typename VertexRefLookupT>
  BRepGraphInc_WireExplorer(const NCollection_Vector<BRepGraphInc::CoEdgeRef>& theCoEdgeRefs,
                            const CoEdgeLookupT&                                theCoEdgeLookup,
                            const EdgeLookupT&                                  theEdgeLookup,
                            const VertexRefLookupT&                             theVtxRefLookup)
      : myCurrent(0)
  {
    buildOrder(theCoEdgeRefs, theCoEdgeLookup, theEdgeLookup, theVtxRefLookup);
  }

  //! Returns true if there are more edges to iterate.
  bool More() const { return myCurrent < myOrder.Length(); }

  //! Advance to the next edge.
  void Next() { ++myCurrent; }

  //! Current coedge reference in connection order.
  const BRepGraphInc::CoEdgeRef& CurrentRef() const { return myOrder.Value(myCurrent); }

  //! Current coedge typed identifier in storage.
  BRepGraph_CoEdgeId CurrentCoEdgeId() const { return CurrentRef().CoEdgeDefId; }

  //! Number of coedges in the ordered sequence.
  int NbEdges() const { return myOrder.Length(); }

  //! Access the ordered coedge refs vector directly.
  const NCollection_Vector<BRepGraphInc::CoEdgeRef>& OrderedRefs() const { return myOrder; }

private:
  //! Resolve the oriented start vertex of an edge: for FORWARD sense returns
  //! the start vertex def id, for REVERSED returns the end vertex def id.
  template <typename VertexRefLookupT>
  static BRepGraph_NodeId orientedStartVertex(const BRepGraphInc::EdgeEntity& theEdge,
                                              const TopAbs_Orientation         theSense,
                                              const VertexRefLookupT&          theVtxRefLookup)
  {
    const BRepGraph_VertexRefId aRefId =
      (theSense == TopAbs_FORWARD) ? theEdge.StartVertexRefId : theEdge.EndVertexRefId;
    if (!aRefId.IsValid())
      return BRepGraph_NodeId();
    return theVtxRefLookup(aRefId);
  }

  //! Resolve the oriented end vertex of an edge: for FORWARD sense returns
  //! the end vertex def id, for REVERSED returns the start vertex def id.
  template <typename VertexRefLookupT>
  static BRepGraph_NodeId orientedEndVertex(const BRepGraphInc::EdgeEntity& theEdge,
                                            const TopAbs_Orientation         theSense,
                                            const VertexRefLookupT&          theVtxRefLookup)
  {
    const BRepGraph_VertexRefId aRefId =
      (theSense == TopAbs_FORWARD) ? theEdge.EndVertexRefId : theEdge.StartVertexRefId;
    if (!aRefId.IsValid())
      return BRepGraph_NodeId();
    return theVtxRefLookup(aRefId);
  }

  //! Build connection-ordered coedge sequence from CoEdgeRefs.
  template <typename CoEdgeLookupT, typename EdgeLookupT, typename VertexRefLookupT>
  void buildOrder(const NCollection_Vector<BRepGraphInc::CoEdgeRef>& theCoEdgeRefs,
                  const CoEdgeLookupT&                               theCoEdgeLookup,
                  const EdgeLookupT&                                 theEdgeLookup,
                  const VertexRefLookupT&                            theVtxRefLookup)
  {
    const int aNbEdges = theCoEdgeRefs.Length();
    if (aNbEdges == 0)
      return;

    // Track which coedges have been placed.
    NCollection_Array1<bool> aUsed(0, aNbEdges - 1);
    aUsed.Init(false);

    // Start with the first coedge.
    myOrder.Append(theCoEdgeRefs.Value(0));
    aUsed(0) = true;

    // Chain coedges by matching end-vertex to start-vertex.
    for (int aPlaced = 1; aPlaced < aNbEdges; ++aPlaced)
    {
      const BRepGraphInc::CoEdgeRef&    aPrevRef    = myOrder.Value(aPlaced - 1);
      const BRepGraphInc::CoEdgeEntity& aPrevCoEdge = theCoEdgeLookup(aPrevRef.CoEdgeDefId.Index);
      const BRepGraph_NodeId            aPrevEnd =
        orientedEndVertex(theEdgeLookup(aPrevCoEdge.EdgeDefId.Index),
                          aPrevCoEdge.Sense,
                          theVtxRefLookup);

      bool aFound = false;
      for (int i = 0; i < aNbEdges; ++i)
      {
        if (aUsed(i))
          continue;
        const BRepGraphInc::CoEdgeRef&    aCandRef    = theCoEdgeRefs.Value(i);
        const BRepGraphInc::CoEdgeEntity& aCandCoEdge = theCoEdgeLookup(aCandRef.CoEdgeDefId.Index);
        const BRepGraph_NodeId            aCandStart =
          orientedStartVertex(theEdgeLookup(aCandCoEdge.EdgeDefId.Index),
                              aCandCoEdge.Sense,
                              theVtxRefLookup);

        if (aPrevEnd.IsValid() && aCandStart.IsValid() && aPrevEnd == aCandStart)
        {
          myOrder.Append(aCandRef);
          aUsed(i) = true;
          aFound = true;
          break;
        }
      }
      if (!aFound)
      {
        // No connected coedge found - append next unused (disconnected wire segment).
        for (int i = 0; i < aNbEdges; ++i)
        {
          if (!aUsed(i))
          {
            myOrder.Append(theCoEdgeRefs.Value(i));
            aUsed(i) = true;
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
