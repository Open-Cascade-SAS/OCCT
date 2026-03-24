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
#include <BRepGraph_UID.hxx>
#include <BRepGraph_AttrRegistry.hxx>
#include <BRepGraph_NodeCache.hxx>
#include <BRepGraph_TopoNode.hxx>
#include <BRepGraph_History.hxx>
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
  std::atomic<size_t> myNextUIDCounter{0};
  uint32_t            myGeneration{0};

  //! History subsystem.
  BRepGraph_History myHistoryLog;

  bool myIsDone = false;

  //! When true, markModified() only sets IsModified flags --
  //! no mutex acquisition and no upward propagation.
  std::atomic<bool> myDeferredMode{false};

  //! Thread-safe cache of reconstructed shapes.
  mutable NCollection_DataMap<BRepGraph_NodeId, TopoDS_Shape> myCurrentShapes;
  mutable std::shared_mutex                                   myCurrentShapesMutex;

  using ReconstructCache = NCollection_DataMap<BRepGraph_NodeId, TopoDS_Shape>;

  BRepGraph_Data()
      : myAllocator(new NCollection_IncAllocator),
        myIncStorage(myAllocator),
        myCurrentShapes(1, myAllocator)
  {
    myHistoryLog.SetAllocator(myAllocator);
  }

  explicit BRepGraph_Data(const occ::handle<NCollection_BaseAllocator>& theAlloc)
      : myAllocator(!theAlloc.IsNull()
                      ? theAlloc
                      : occ::handle<NCollection_BaseAllocator>(new NCollection_IncAllocator)),
        myIncStorage(myAllocator),
        myCurrentShapes(1, myAllocator)
  {
    myHistoryLog.SetAllocator(myAllocator);
  }
};

#endif // _BRepGraph_Data_HeaderFile
