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
#include <BRepGraph_GeomNode.hxx>
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
  Handle(NCollection_BaseAllocator) myAllocator;

  //! Definition vectors (indexed by BRepGraph_NodeId.Index).
  NCollection_Vector<BRepGraph_TopoNode::SolidDef>     mySolidDefs;
  NCollection_Vector<BRepGraph_TopoNode::ShellDef>     myShellDefs;
  NCollection_Vector<BRepGraph_TopoNode::FaceDef>      myFaceDefs;
  NCollection_Vector<BRepGraph_TopoNode::WireDef>      myWireDefs;
  NCollection_Vector<BRepGraph_TopoNode::EdgeDef>      myEdgeDefs;
  NCollection_Vector<BRepGraph_TopoNode::VertexDef>    myVertexDefs;
  NCollection_Vector<BRepGraph_TopoNode::CompoundDef>  myCompoundDefs;
  NCollection_Vector<BRepGraph_TopoNode::CompSolidDef> myCompSolidDefs;

  //! Usage vectors (indexed by BRepGraph_UsageId.Index).
  NCollection_Vector<BRepGraph_TopoNode::SolidUsage>     mySolidUsages;
  NCollection_Vector<BRepGraph_TopoNode::ShellUsage>     myShellUsages;
  NCollection_Vector<BRepGraph_TopoNode::FaceUsage>      myFaceUsages;
  NCollection_Vector<BRepGraph_TopoNode::WireUsage>      myWireUsages;
  NCollection_Vector<BRepGraph_TopoNode::EdgeUsage>      myEdgeUsages;
  NCollection_Vector<BRepGraph_TopoNode::VertexUsage>    myVertexUsages;
  NCollection_Vector<BRepGraph_TopoNode::CompoundUsage>  myCompoundUsages;
  NCollection_Vector<BRepGraph_TopoNode::CompSolidUsage> myCompSolidUsages;

  //! Geometry node vectors.
  NCollection_Vector<BRepGraph_GeomNode::Surf>   mySurfaces;
  NCollection_Vector<BRepGraph_GeomNode::Curve>  myCurves;
  NCollection_Vector<BRepGraph_GeomNode::PCurve> myPCurves;

  //! Map-based RelEdge storage.
  NCollection_DataMap<BRepGraph_NodeId,
                      NCollection_Vector<BRepGraph_RelEdge>> myOutRelEdges;
  NCollection_DataMap<BRepGraph_NodeId,
                      NCollection_Vector<BRepGraph_RelEdge>> myInRelEdges;

  //! Geometry deduplication registries.
  NCollection_IndexedDataMap<const Geom_Surface*, int> mySurfRegistry;
  NCollection_IndexedDataMap<const Geom_Curve*, int>   myCurveRegistry;

  //! TShape -> Definition NodeId reverse lookup.
  NCollection_DataMap<const TopoDS_TShape*, BRepGraph_NodeId> myTShapeToDefId;

  //! UID system (always enabled).
  //! Forward maps: per-kind vectors populated eagerly in allocateUID().
  //! Reverse map: single DataMap built lazily on first NodeIdFrom()/Has().
  std::atomic<size_t> myNextUIDCounter{0};
  uint32_t            myGeneration{0};

  NCollection_Vector<BRepGraph_UID> mySolidUIDs;
  NCollection_Vector<BRepGraph_UID> myShellUIDs;
  NCollection_Vector<BRepGraph_UID> myFaceUIDs;
  NCollection_Vector<BRepGraph_UID> myWireUIDs;
  NCollection_Vector<BRepGraph_UID> myEdgeUIDs;
  NCollection_Vector<BRepGraph_UID> myVertexUIDs;
  NCollection_Vector<BRepGraph_UID> myCompoundUIDs;
  NCollection_Vector<BRepGraph_UID> myCompSolidUIDs;
  NCollection_Vector<BRepGraph_UID> mySurfaceUIDs;
  NCollection_Vector<BRepGraph_UID> myCurveUIDs;
  NCollection_Vector<BRepGraph_UID> myPCurveUIDs;

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
        mySolidDefs(16, myAllocator),
        myShellDefs(16, myAllocator),
        myFaceDefs(128, myAllocator),
        myWireDefs(128, myAllocator),
        myEdgeDefs(256, myAllocator),
        myVertexDefs(256, myAllocator),
        myCompoundDefs(16, myAllocator),
        myCompSolidDefs(16, myAllocator),
        mySolidUsages(16, myAllocator),
        myShellUsages(16, myAllocator),
        myFaceUsages(128, myAllocator),
        myWireUsages(128, myAllocator),
        myEdgeUsages(256, myAllocator),
        myVertexUsages(256, myAllocator),
        myCompoundUsages(16, myAllocator),
        myCompSolidUsages(16, myAllocator),
        mySurfaces(64, myAllocator),
        myCurves(64, myAllocator),
        myPCurves(128, myAllocator),
        mySurfRegistry(100, myAllocator),
        myCurveRegistry(100, myAllocator),
        myTShapeToDefId(100, myAllocator),
        mySolidUIDs(16, myAllocator),
        myShellUIDs(16, myAllocator),
        myFaceUIDs(128, myAllocator),
        myWireUIDs(128, myAllocator),
        myEdgeUIDs(256, myAllocator),
        myVertexUIDs(256, myAllocator),
        myCompoundUIDs(16, myAllocator),
        myCompSolidUIDs(16, myAllocator),
        mySurfaceUIDs(64, myAllocator),
        myCurveUIDs(64, myAllocator),
        myPCurveUIDs(128, myAllocator)
  {
  }

  //! Constructor with custom allocator.
  explicit BRepGraph_Data(const Handle(NCollection_BaseAllocator)& theAlloc)
      : myAllocator(!theAlloc.IsNull() ? theAlloc
                                       : NCollection_BaseAllocator::CommonBaseAllocator()),
        mySolidDefs(16, myAllocator),
        myShellDefs(16, myAllocator),
        myFaceDefs(128, myAllocator),
        myWireDefs(128, myAllocator),
        myEdgeDefs(256, myAllocator),
        myVertexDefs(256, myAllocator),
        myCompoundDefs(16, myAllocator),
        myCompSolidDefs(16, myAllocator),
        mySolidUsages(16, myAllocator),
        myShellUsages(16, myAllocator),
        myFaceUsages(128, myAllocator),
        myWireUsages(128, myAllocator),
        myEdgeUsages(256, myAllocator),
        myVertexUsages(256, myAllocator),
        myCompoundUsages(16, myAllocator),
        myCompSolidUsages(16, myAllocator),
        mySurfaces(64, myAllocator),
        myCurves(64, myAllocator),
        myPCurves(128, myAllocator),
        mySurfRegistry(100, myAllocator),
        myCurveRegistry(100, myAllocator),
        myTShapeToDefId(100, myAllocator),
        mySolidUIDs(16, myAllocator),
        myShellUIDs(16, myAllocator),
        myFaceUIDs(128, myAllocator),
        myWireUIDs(128, myAllocator),
        myEdgeUIDs(256, myAllocator),
        myVertexUIDs(256, myAllocator),
        myCompoundUIDs(16, myAllocator),
        myCompSolidUIDs(16, myAllocator),
        mySurfaceUIDs(64, myAllocator),
        myCurveUIDs(64, myAllocator),
        myPCurveUIDs(128, myAllocator)
  {
  }
};

#endif // _BRepGraph_Data_HeaderFile
