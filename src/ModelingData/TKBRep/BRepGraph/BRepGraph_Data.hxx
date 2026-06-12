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

#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_CacheRegistry.hxx>
#include <BRepGraph_LayerRegistry.hxx>
#include <BRepGraph_MeshView.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_UIDsView.hxx>
#include <BRepGraphInc_Storage.hxx>

#include <atomic>

class BRepGraph;

//! @brief Internal storage for BRepGraph (PIMPL).
//!
//! All topology definition data and UIDs live in myIncStorage.
//! Access via myIncStorage.Edges, myIncStorage.Faces, etc.
struct BRepGraph_Data
{
  //! Incidence-table storage - sole source of truth for all topology data,
  //! original shapes, TShape->NodeId mapping, UIDs, and UID reverse indexes.
  BRepGraphInc_Storage myIncStorage;

  //! Registered graph layers.
  BRepGraph_LayerRegistry myLayerRegistry;

  //! Registered transient cache services.
  BRepGraph_CacheRegistry myCacheRegistry;

  //! Stable top-level views. Nested views store graph-data context only.
  BRepGraph::TopoView   myTopoView{nullptr};
  BRepGraph::UIDsView   myUIDsView{nullptr};
  BRepGraph::RefsView   myRefsView{nullptr};
  BRepGraph::ShapesView myShapesView{nullptr};
  BRepGraph::EditorView myEditorView{nullptr};
  BRepGraph::MeshView   myMeshView{nullptr};
};

#endif // _BRepGraph_Data_HeaderFile
