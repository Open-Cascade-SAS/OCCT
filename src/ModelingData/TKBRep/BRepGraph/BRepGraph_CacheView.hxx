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

//! @brief Non-const view for managing transient cache values on nodes.
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

  //! Retrieve a cached value from a node.
  //! @param[in] theNode node to query
  //! @param[in] theKind cache kind descriptor identifying the slot
  //! @return cached value, or null handle if not present or stale
  [[nodiscard]] Standard_EXPORT occ::handle<BRepGraph_CacheValue> Get(
    const BRepGraph_NodeId                  theNode,
    const occ::handle<BRepGraph_CacheKind>& theKind) const;

  //! Remove a cached value from a node.
  //! @param[in] theNode node to remove the value from
  //! @param[in] theKind cache kind descriptor identifying the slot
  //! @return true if a value was actually removed
  Standard_EXPORT bool Remove(const BRepGraph_NodeId theNode,
                              const occ::handle<BRepGraph_CacheKind>& theKind);

  //! Invalidate (but do not remove) a cached value on a node.
  //! @param[in] theNode node whose cache entry to invalidate
  //! @param[in] theKind cache kind descriptor identifying the slot
  Standard_EXPORT void Invalidate(const BRepGraph_NodeId theNode,
                                  const occ::handle<BRepGraph_CacheKind>& theKind);

  //! Return all cache kinds populated on a node.
  //! @param[in] theNode node to query
  //! @return vector of cache kind descriptors that have stored values on this node
  [[nodiscard]] Standard_EXPORT NCollection_Vector<occ::handle<BRepGraph_CacheKind>> CacheKinds(
    const BRepGraph_NodeId theNode) const;

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
