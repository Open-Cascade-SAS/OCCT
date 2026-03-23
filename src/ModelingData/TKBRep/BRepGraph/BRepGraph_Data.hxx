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
#include <BRepGraphInc_Storage.hxx>

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
//!
//! Definition data lives in myIncStorage (incidence-table entity vectors).
//! The Defs reference in each TopoKindData points directly to the
//! corresponding incidence entity vector — no separate copy.
struct BRepGraph_Data
{
  //! Groups definition reference, usage, and UID vectors for a single topology kind.
  //! Defs is a reference to the incidence storage vector (sole source of truth).
  template <typename DefT, typename UsageT>
  struct TopoKindData
  {
    NCollection_Vector<DefT>&         Defs;     //!< Reference to incidence entity vector
    NCollection_Vector<UsageT>        Usages;
    NCollection_Vector<BRepGraph_UID> UIDs;

    TopoKindData(NCollection_Vector<DefT>&                  theDefsRef,
                 int                                        theUsageCap,
                 const Handle(NCollection_BaseAllocator)& theAlloc)
        : Defs(theDefsRef),
          Usages(theUsageCap, theAlloc),
          UIDs(theUsageCap, theAlloc) {}

    //! Clear usage and UID vectors. Defs (reference to incidence storage) is not
    //! cleared here — incidence storage owns the entity vectors and clears them separately.
    void Clear() { Usages.Clear(); UIDs.Clear(); }
  };

  Handle(NCollection_BaseAllocator) myAllocator;

  //! Incidence-table storage — sole source of truth for definition data.
  //! MUST be declared before TopoKindData members so references are valid.
  BRepGraphInc_Storage myIncStorage;

  //! Topology kind data (8 kinds). Defs reference incidence storage vectors.
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
        mySolids(myIncStorage.Solids, 16, myAllocator),
        myShells(myIncStorage.Shells, 16, myAllocator),
        myFaces(myIncStorage.Faces, 128, myAllocator),
        myWires(myIncStorage.Wires, 128, myAllocator),
        myEdges(myIncStorage.Edges, 256, myAllocator),
        myVertices(myIncStorage.Vertices, 256, myAllocator),
        myCompounds(myIncStorage.Compounds, 16, myAllocator),
        myCompSolids(myIncStorage.CompSolids, 16, myAllocator),
        myTShapeToDefId(100, myAllocator)
  {
  }

  //! Constructor with custom allocator.
  explicit BRepGraph_Data(const Handle(NCollection_BaseAllocator)& theAlloc)
      : myAllocator(!theAlloc.IsNull() ? theAlloc
                                       : NCollection_BaseAllocator::CommonBaseAllocator()),
        mySolids(myIncStorage.Solids, 16, myAllocator),
        myShells(myIncStorage.Shells, 16, myAllocator),
        myFaces(myIncStorage.Faces, 128, myAllocator),
        myWires(myIncStorage.Wires, 128, myAllocator),
        myEdges(myIncStorage.Edges, 256, myAllocator),
        myVertices(myIncStorage.Vertices, 256, myAllocator),
        myCompounds(myIncStorage.Compounds, 16, myAllocator),
        myCompSolids(myIncStorage.CompSolids, 16, myAllocator),
        myTShapeToDefId(100, myAllocator)
  {
  }
};

#endif // _BRepGraph_Data_HeaderFile
