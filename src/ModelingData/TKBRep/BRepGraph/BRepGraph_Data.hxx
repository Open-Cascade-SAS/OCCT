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

#include <BRepGraph_CacheView.hxx>
#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_History.hxx>
#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_PathView.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_UID.hxx>
#include <BRepGraph_UIDsView.hxx>
#include <Standard_GUID.hxx>
#include <BRepGraphInc_Storage.hxx>

#include <TopoDS_Shape.hxx>
#include <TopoDS_TShape.hxx>
#include <TopLoc_Location.hxx>

#include <NCollection_Vector.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_BaseAllocator.hxx>

#include <NCollection_IncAllocator.hxx>

#include <atomic>
#include <shared_mutex>

//! @brief Internal storage for BRepGraph (PIMPL).
//!
//! All topology definition data and UIDs live in myIncStorage.
//! Access via myIncStorage.Edges, myIncStorage.Faces, etc.
struct BRepGraph_Data
{
  occ::handle<NCollection_BaseAllocator> myAllocator;

  //! Incidence-table storage - sole source of truth for all topology data,
  //! original shapes, TShape->NodeId mapping, and UIDs.
  BRepGraphInc_Storage myIncStorage;

  //! UID system.
  std::atomic<size_t> myNextUIDCounter{
    1}; //!< Starts at 1; counter=0 is BRepGraph_UID invalid sentinel.
  std::atomic<uint32_t> myGeneration{0};
  Standard_GUID         myGraphGUID; //!< Random graph identity, generated at Build().

  //! History subsystem.
  BRepGraph_History myHistoryLog;

  bool myIsDone = false;

  //! Root topology NodeIds set by Build()/AppendShape() - one per input shape.
  NCollection_Vector<BRepGraph_NodeId> myRootNodeIds;

  //! When true, markModified() only increments OwnGen + SubtreeGen and appends to
  //! myDeferredModified - no mutex acquisition and no upward propagation.
  std::atomic<bool> myDeferredMode{false};

  //! Propagation wave counter. Incremented at the start of each
  //! markModified() / markRefModified() call. markParentModified()
  //! compares entity.LastPropWave against this to skip already-visited
  //! parents in the same propagation wave (O(1) re-visit guard).
  std::atomic<uint32_t> myPropagationWave{0};

  //! NodeIds accumulated during deferred mode. Processed by EndDeferredInvalidation().
  NCollection_Vector<BRepGraph_NodeId> myDeferredModified;

  //! Gen-validated shape cache entry.
  struct CachedShape
  {
    TopoDS_Shape Shape;
    uint32_t     StoredSubtreeGen = 0;
  };

  //! Thread-safe cache of reconstructed shapes with SubtreeGen validation.
  mutable NCollection_DataMap<BRepGraph_NodeId, CachedShape> myCurrentShapes;
  mutable std::shared_mutex                                  myCurrentShapesMutex;

  //! Lazy reverse lookup index for entity UIDs.
  mutable NCollection_DataMap<BRepGraph_UID, BRepGraph_NodeId> myUIDToNodeId;
  mutable std::shared_mutex                                    myUIDToNodeIdMutex;
  mutable uint32_t                                             myUIDToNodeIdGeneration = 0;
  mutable bool                                                 myUIDToNodeIdDirty      = true;

  //! Lazy reverse lookup index for reference UIDs.
  mutable NCollection_DataMap<BRepGraph_RefUID, BRepGraph_RefId> myRefUIDToRefId;
  mutable std::shared_mutex                                      myRefUIDToRefIdMutex;
  mutable uint32_t                                               myRefUIDToRefIdGeneration = 0;
  mutable bool                                                   myRefUIDToRefIdDirty      = true;

  using ReconstructCache = NCollection_DataMap<BRepGraph_NodeId, TopoDS_Shape>;

  //! Cached view objects (pointers set to owning BRepGraph in its constructor).
  BRepGraph::TopoView    myTopoView{nullptr};
  BRepGraph::UIDsView    myUIDsView{nullptr};
  BRepGraph::PathView    myPathView{nullptr};
  BRepGraph::CacheView   myCacheView{nullptr};
  BRepGraph::RefsView    myRefsView{nullptr};
  BRepGraph::ShapesView  myShapesView{nullptr};
  BRepGraph::BuilderView myBuilderView{nullptr};

  BRepGraph_Data()
      : myAllocator(new NCollection_IncAllocator),
        myIncStorage(myAllocator),
        myCurrentShapes(1, myAllocator),
        myUIDToNodeId(1, myAllocator),
        myRefUIDToRefId(1, myAllocator)
  {
    myHistoryLog.SetAllocator(myAllocator);
  }

  explicit BRepGraph_Data(const occ::handle<NCollection_BaseAllocator>& theAlloc)
      : myAllocator(!theAlloc.IsNull()
                      ? theAlloc
                      : occ::handle<NCollection_BaseAllocator>(new NCollection_IncAllocator)),
        myIncStorage(myAllocator),
        myCurrentShapes(1, myAllocator),
        myUIDToNodeId(1, myAllocator),
        myRefUIDToRefId(1, myAllocator)
  {
    myHistoryLog.SetAllocator(myAllocator);
  }
};

#endif // _BRepGraph_Data_HeaderFile
