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

#ifndef _BRepGraph_CacheView_HeaderFile
#define _BRepGraph_CacheView_HeaderFile

#include <BRepGraph.hxx>
#include <BRepGraph_CacheKindIterator.hxx>

//! @brief Non-const view for managing transient cache values on nodes.
//!
//! This view is the stable public cache API for BRepGraph callers.
//! External code should use Cache() for all cache access.
//! Low-level storage operations such as reserve or cross-graph cache transfer
//! stay internal to graph-maintenance code through the graph's private cache access.
//!
//! Cached values are keyed by BRepGraph_CacheKind descriptors (Handle-based)
//! and stored as Handle(BRepGraph_CacheValue). Each CacheKind carries a
//! Standard_GUID for stable identity and is registered in
//! BRepGraph_CacheKindRegistry which maps GUIDs to dense runtime slot
//! indices for O(1) internal storage lookup.
//!
//! Supports set, get, remove, invalidate, and kind enumeration per node.
//! Cached data is stored centrally in BRepGraph_TransientCache with
//! generation-based freshness tracking via SubtreeGen.
//! Hot-path callers may pre-resolve a cache-kind slot once through
//! BRepGraph_CacheKindRegistry::Register() and then use slot-based overloads
//! to avoid repeated registry locking.
//! Obtained via BRepGraph::Cache().
class BRepGraph::CacheView
{
public:
  //! Attach a cached value to a node.
  //! @param[in] theNode  node to attach the value to
  //! @param[in] theKind  cache kind descriptor identifying the slot
  //! @param[in] theValue cached value to store
  Standard_EXPORT void Set(const BRepGraph_NodeId                   theNode,
                           const occ::handle<BRepGraph_CacheKind>&  theKind,
                           const occ::handle<BRepGraph_CacheValue>& theValue);

  //! Attach a cached value using a pre-resolved cache-kind slot.
  Standard_EXPORT void Set(const BRepGraph_NodeId                   theNode,
                           const int                                theKindSlot,
                           const occ::handle<BRepGraph_CacheValue>& theValue);

  //! Retrieve a cached value from a node.
  //! @param[in] theNode node to query
  //! @param[in] theKind cache kind descriptor identifying the slot
  //! @return cached value, or null handle if not present or stale
  [[nodiscard]] Standard_EXPORT occ::handle<BRepGraph_CacheValue> Get(
    const BRepGraph_NodeId                  theNode,
    const occ::handle<BRepGraph_CacheKind>& theKind) const;

  //! Retrieve a cached value using a pre-resolved cache-kind slot.
  [[nodiscard]] Standard_EXPORT occ::handle<BRepGraph_CacheValue> Get(
    const BRepGraph_NodeId theNode,
    const int              theKindSlot) const;

  //! Check if a non-stale cached value exists on a node.
  //! @param[in] theNode node to query
  //! @param[in] theKind cache kind descriptor identifying the slot
  //! @return true if a current value exists for this node and kind
  [[nodiscard]] Standard_EXPORT bool Has(const BRepGraph_NodeId                  theNode,
                                         const occ::handle<BRepGraph_CacheKind>& theKind) const;

  //! Check if a non-stale cached value exists using a pre-resolved slot.
  [[nodiscard]] Standard_EXPORT bool Has(const BRepGraph_NodeId theNode,
                                         const int              theKindSlot) const;

  //! Remove a cached value from a node.
  //! @param[in] theNode node to remove the value from
  //! @param[in] theKind cache kind descriptor identifying the slot
  //! @return true if a value was actually removed
  Standard_EXPORT bool Remove(const BRepGraph_NodeId                  theNode,
                              const occ::handle<BRepGraph_CacheKind>& theKind);

  //! Remove a cached value using a pre-resolved cache-kind slot.
  Standard_EXPORT bool Remove(const BRepGraph_NodeId theNode, const int theKindSlot);

  //! Invalidate (but do not remove) a cached value on a node.
  //! @param[in] theNode node whose cache entry to invalidate
  //! @param[in] theKind cache kind descriptor identifying the slot
  Standard_EXPORT void Invalidate(const BRepGraph_NodeId                  theNode,
                                  const occ::handle<BRepGraph_CacheKind>& theKind);

  //! Invalidate a cached value using a pre-resolved cache-kind slot.
  Standard_EXPORT void Invalidate(const BRepGraph_NodeId theNode, const int theKindSlot);

  //! Create a zero-allocation iterator over all cache kinds populated on a node.
  //! @param[in] theNode node to query
  Standard_EXPORT BRepGraph_CacheKindIterator<BRepGraph_NodeId> CacheKindIter(
    const BRepGraph_NodeId theNode) const;

  //! Create a zero-allocation iterator over all cache kinds populated on a reference.
  //! @param[in] theRef reference to query
  Standard_EXPORT BRepGraph_CacheKindIterator<BRepGraph_RefId> CacheKindIter(
    const BRepGraph_RefId theRef) const;

  // --- Reference-level cache ---

  //! Attach a cached value to a reference.
  //! @param[in] theRef   reference to attach the value to
  //! @param[in] theKind  cache kind descriptor identifying the slot
  //! @param[in] theValue cached value to store
  Standard_EXPORT void Set(const BRepGraph_RefId                    theRef,
                           const occ::handle<BRepGraph_CacheKind>&  theKind,
                           const occ::handle<BRepGraph_CacheValue>& theValue);

  //! Attach a cached value to a reference using a pre-resolved cache-kind slot.
  Standard_EXPORT void Set(const BRepGraph_RefId                    theRef,
                           const int                                theKindSlot,
                           const occ::handle<BRepGraph_CacheValue>& theValue);

  //! Retrieve a cached value from a reference.
  //! @return cached value, or null handle if not present or stale (OwnGen changed)
  [[nodiscard]] Standard_EXPORT occ::handle<BRepGraph_CacheValue> Get(
    const BRepGraph_RefId                   theRef,
    const occ::handle<BRepGraph_CacheKind>& theKind) const;

  //! Retrieve a cached value from a reference using a pre-resolved cache-kind slot.
  [[nodiscard]] Standard_EXPORT occ::handle<BRepGraph_CacheValue> Get(const BRepGraph_RefId theRef,
                                                                      const int theKindSlot) const;

  //! Check if a non-stale cached value exists on a reference.
  [[nodiscard]] Standard_EXPORT bool Has(const BRepGraph_RefId                   theRef,
                                         const occ::handle<BRepGraph_CacheKind>& theKind) const;

  //! Check if a non-stale cached value exists on a reference using a pre-resolved slot.
  [[nodiscard]] Standard_EXPORT bool Has(const BRepGraph_RefId theRef, const int theKindSlot) const;

  //! Remove a cached value from a reference.
  //! @return true if a value was actually removed
  Standard_EXPORT bool Remove(const BRepGraph_RefId                   theRef,
                              const occ::handle<BRepGraph_CacheKind>& theKind);

  //! Remove a cached value from a reference using a pre-resolved cache-kind slot.
  Standard_EXPORT bool Remove(const BRepGraph_RefId theRef, const int theKindSlot);

  //! Invalidate (but do not remove) a cached value on a reference.
  Standard_EXPORT void Invalidate(const BRepGraph_RefId                   theRef,
                                  const occ::handle<BRepGraph_CacheKind>& theKind);

  //! Invalidate a cached value on a reference using a pre-resolved cache-kind slot.
  Standard_EXPORT void Invalidate(const BRepGraph_RefId theRef, const int theKindSlot);

private:
  friend class BRepGraph;
  friend struct BRepGraph_Data;

  explicit CacheView(BRepGraph* theGraph)
      : myGraph(theGraph)
  {
  }

  BRepGraph* myGraph;
};

#endif // _BRepGraph_CacheView_HeaderFile
