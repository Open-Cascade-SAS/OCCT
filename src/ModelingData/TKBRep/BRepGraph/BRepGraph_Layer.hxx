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
#include <NCollection_Vector.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_AsciiString.hxx>

//! @brief Abstract base class for named attribute layers.
//!
//! A layer groups per-node metadata under a unique name with lifecycle callbacks.
//! Layers are registered on BRepGraph and automatically notified when nodes are
//! removed, replaced (sewing/deduplicate), remapped (compact), or modified.
//!
//! Derived layers store domain-specific data (names, colors, materials, etc.)
//! in internal maps keyed by BRepGraph_NodeId. The lifecycle callbacks ensure
//! data consistency across all graph mutations.
//!
//! ## Modification Events
//! Layers can subscribe to modification events by overriding SubscribedKinds()
//! to return a non-zero bitmask of Kind values. When a subscribed node kind is
//! modified, OnNodeModified() (immediate mode) or OnNodesModified() (deferred
//! batch mode) is called. Layers with SubscribedKinds() == 0 (default) incur
//! zero dispatch overhead.
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
  virtual void OnNodeRemoved(const BRepGraph_NodeId theNode,
                             const BRepGraph_NodeId theReplacement) = 0;

  //! Called after Compact with a unified old->new remap map.
  //! Layer must remap all internal NodeId references using this map.
  //! The map covers all node kinds (Vertex through CompSolid and future extensions).
  //! Nodes absent from the map were removed during compaction - layers should
  //! drop data associated with those nodes.
  //! @param[in] theRemapMap maps old NodeId to new NodeId for all surviving nodes
  virtual void OnCompact(
    const NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theRemapMap) = 0;

  //! Mark all cached values dirty (bulk invalidation).
  virtual void InvalidateAll() = 0;

  //! Clear all stored data.
  virtual void Clear() = 0;

  // --- Modification event subscription ---

  //! Return a bitmask of BRepGraph_NodeId::Kind values this layer subscribes to.
  //! Only modification events matching subscribed kinds are dispatched.
  //! Default: 0 (no subscription - no modification events received).
  //! Override to receive OnNodeModified/OnNodesModified callbacks.
  //! The returned value must be constant for the lifetime of the layer.
  Standard_EXPORT virtual int SubscribedKinds() const;

  //! Called in immediate (non-deferred) mode after a single node is modified.
  //! Only dispatched if the node's kind matches SubscribedKinds().
  //! Default: no-op.
  //! @param[in] theNode the modified node
  Standard_EXPORT virtual void OnNodeModified(const BRepGraph_NodeId theNode);

  //! Called after EndDeferredInvalidation() with all nodes modified during
  //! the deferred scope. Only dispatched if at least one modified node's kind
  //! matches SubscribedKinds(). The vector may contain nodes of kinds not
  //! subscribed to - layers should filter internally if needed.
  //! Default: no-op.
  //! @param[in] theModifiedNodes all modified, non-removed nodes
  Standard_EXPORT virtual void OnNodesModified(
    const NCollection_Vector<BRepGraph_NodeId>& theModifiedNodes);

  //! Convenience: return bitmask bit for a given Kind.
  static int KindBit(const BRepGraph_NodeId::Kind theKind)
  {
    return 1 << static_cast<int>(theKind);
  }

  DEFINE_STANDARD_RTTIEXT(BRepGraph_Layer, Standard_Transient)
};

#endif // _BRepGraph_Layer_HeaderFile
