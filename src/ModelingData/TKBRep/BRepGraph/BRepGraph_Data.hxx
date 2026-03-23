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

#ifndef _BRepGraph_Data_HeaderFile
#define _BRepGraph_Data_HeaderFile

#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_RelEdge.hxx>
#include <BRepGraph_UID.hxx>
#include <BRepGraph_AttrRegistry.hxx>
#include <BRepGraph_CachedValue.hxx>
#include <BRepGraph_NodeCache.hxx>
#include <BRepGraph_TopoNode.hxx>
#include <BRepGraph_History.hxx>

#include <TopoDS_Shape.hxx>
#include <TopoDS_TShape.hxx>

#include <NCollection_Vector.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_BaseAllocator.hxx>

#include <atomic>
#include <shared_mutex>

//! @brief Internal storage for BRepGraph (PIMPL).
//!
//! This struct holds all private data members of BRepGraph.
//! It is not part of the public API and must not be included
//! by code outside BRepGraph and its friend classes.
struct BRepGraph_Data
{
  //! Groups definition, usage, and UID vectors for a single topology kind.
  template <typename DefT, typename UsageT>
  struct TopoKindData
  {
    NCollection_Vector<DefT>          Defs;
    NCollection_Vector<UsageT>        Usages;
    NCollection_Vector<BRepGraph_UID> UIDs;

    TopoKindData(int theDefCap, int theUsageCap,
                 const Handle(NCollection_BaseAllocator)& theAlloc)
        : Defs(theDefCap, theAlloc),
          Usages(theUsageCap, theAlloc),
          UIDs(theDefCap, theAlloc) {}

    void Clear() { Defs.Clear(); Usages.Clear(); UIDs.Clear(); }
  };

  Handle(NCollection_BaseAllocator) myAllocator;

  //! Topology kind data (8 kinds), each grouping Defs, Usages, and UIDs.
  TopoKindData<BRepGraph_TopoNode::SolidDef, BRepGraph_TopoNode::SolidUsage>         mySolids;
  TopoKindData<BRepGraph_TopoNode::ShellDef, BRepGraph_TopoNode::ShellUsage>         myShells;
  TopoKindData<BRepGraph_TopoNode::FaceDef, BRepGraph_TopoNode::FaceUsage>           myFaces;
  TopoKindData<BRepGraph_TopoNode::WireDef, BRepGraph_TopoNode::WireUsage>           myWires;
  TopoKindData<BRepGraph_TopoNode::EdgeDef, BRepGraph_TopoNode::EdgeUsage>           myEdges;
  TopoKindData<BRepGraph_TopoNode::VertexDef, BRepGraph_TopoNode::VertexUsage>       myVertices;
  TopoKindData<BRepGraph_TopoNode::CompoundDef, BRepGraph_TopoNode::CompoundUsage>   myCompounds;
  TopoKindData<BRepGraph_TopoNode::CompSolidDef, BRepGraph_TopoNode::CompSolidUsage> myCompSolids;

  //! Map-based RelEdge storage.
  NCollection_DataMap<BRepGraph_NodeId,
                      NCollection_Vector<BRepGraph_RelEdge>> myOutRelEdges;
  NCollection_DataMap<BRepGraph_NodeId,
                      NCollection_Vector<BRepGraph_RelEdge>> myInRelEdges;

  //! TShape -> Definition NodeId reverse lookup.
  NCollection_DataMap<const TopoDS_TShape*, BRepGraph_NodeId> myTShapeToDefId;

  //! UID system (always enabled).
  //! Forward maps: per-kind vectors populated eagerly in allocateUID().
  //! Reverse map: single DataMap built lazily on first NodeIdFrom()/Has().
  std::atomic<size_t> myNextUIDCounter{0};
  uint32_t            myGeneration{0};

  //! Reverse index: edge def index -> wire def indices containing that edge.
  NCollection_DataMap<int, NCollection_Vector<int>> myEdgeToWires;

  //! History subsystem (records, reverse maps, enable/disable toggle).
  BRepGraph_History myHistoryLog;

  bool myIsDone = false;

  //! Shapes from Build().
  NCollection_DataMap<BRepGraph_NodeId, TopoDS_Shape> myOriginalShapes;

  mutable NCollection_DataMap<BRepGraph_NodeId, TopoDS_Shape> myCurrentShapes;
  mutable std::shared_mutex myCurrentShapesMutex;

  //! Shared cache for edge/vertex shapes during multi-face reconstruction.
  using ReconstructCache = NCollection_DataMap<BRepGraph_NodeId, TopoDS_Shape>;

  //! Default constructor with standard capacities.
  BRepGraph_Data()
      : myAllocator(NCollection_BaseAllocator::CommonBaseAllocator()),
        mySolids(16, 16, myAllocator),
        myShells(16, 16, myAllocator),
        myFaces(128, 128, myAllocator),
        myWires(128, 128, myAllocator),
        myEdges(256, 256, myAllocator),
        myVertices(256, 256, myAllocator),
        myCompounds(16, 16, myAllocator),
        myCompSolids(16, 16, myAllocator),
        myTShapeToDefId(100, myAllocator)
  {
  }

  //! Constructor with custom allocator.
  explicit BRepGraph_Data(const Handle(NCollection_BaseAllocator)& theAlloc)
      : myAllocator(!theAlloc.IsNull() ? theAlloc
                                       : NCollection_BaseAllocator::CommonBaseAllocator()),
        mySolids(16, 16, myAllocator),
        myShells(16, 16, myAllocator),
        myFaces(128, 128, myAllocator),
        myWires(128, 128, myAllocator),
        myEdges(256, 256, myAllocator),
        myVertices(256, 256, myAllocator),
        myCompounds(16, 16, myAllocator),
        myCompSolids(16, 16, myAllocator),
        myTShapeToDefId(100, myAllocator)
  {
  }
};

#endif // _BRepGraph_Data_HeaderFile
