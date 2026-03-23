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

#ifndef _BRepGraph_Layer_HeaderFile
#define _BRepGraph_Layer_HeaderFile

#include <BRepGraph_NodeId.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_AsciiString.hxx>

//! @brief Abstract base class for named attribute layers.
//!
//! A layer groups per-node metadata under a unique name with lifecycle callbacks.
//! Layers are registered on BRepGraph and automatically notified when nodes are
//! removed, replaced (sewing/deduplicate), or remapped (compact).
//!
//! Derived layers store domain-specific data (names, colors, materials, etc.)
//! in internal maps keyed by BRepGraph_NodeId. The lifecycle callbacks ensure
//! data consistency across all graph mutations.
//!
//! ## Thread safety
//! Callback dispatch is single-threaded (called from mutation paths).
//! Layers that only provide read access can skip internal locking.
class BRepGraph_Layer : public Standard_Transient
{
public:
  //! Layer identity (unique within a graph).
  virtual const TCollection_AsciiString& Name() const = 0;

  //! Called when a node is soft-removed.
  //! @param[in] theNode        the removed node
  //! @param[in] theReplacement if valid, the node that replaces theNode
  //!            (e.g., sewing edge merge, deduplicate). If invalid, pure deletion.
  //!            Layers should migrate data from theNode to theReplacement when valid.
  virtual void OnNodeRemoved(BRepGraph_NodeId theNode,
                             BRepGraph_NodeId theReplacement) = 0;

  //! Called after Compact with per-kind old->new index maps.
  //! Layer must remap all internal NodeId references.
  //! @param[in] theVertexMap vertex old->new index map
  //! @param[in] theEdgeMap   edge old->new index map
  //! @param[in] theWireMap   wire old->new index map
  //! @param[in] theFaceMap   face old->new index map
  //! @param[in] theShellMap  shell old->new index map
  //! @param[in] theSolidMap  solid old->new index map
  virtual void OnCompact(const NCollection_DataMap<int, int>& theVertexMap,
                         const NCollection_DataMap<int, int>& theEdgeMap,
                         const NCollection_DataMap<int, int>& theWireMap,
                         const NCollection_DataMap<int, int>& theFaceMap,
                         const NCollection_DataMap<int, int>& theShellMap,
                         const NCollection_DataMap<int, int>& theSolidMap) = 0;

  //! Mark all cached values dirty (bulk invalidation).
  virtual void InvalidateAll() = 0;

  //! Clear all stored data.
  virtual void Clear() = 0;

  DEFINE_STANDARD_RTTIEXT(BRepGraph_Layer, Standard_Transient)
};

#endif // _BRepGraph_Layer_HeaderFile
