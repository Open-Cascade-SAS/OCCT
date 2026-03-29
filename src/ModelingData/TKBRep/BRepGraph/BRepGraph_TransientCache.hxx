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

#ifndef _BRepGraph_TransientCache_HeaderFile
#define _BRepGraph_TransientCache_HeaderFile

#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_UserAttribute.hxx>

#include <NCollection_Vector.hxx>

#include <shared_mutex>

//! @brief Centralized transient cache for algorithm-computed per-node attributes.
//!
//! Stores short-lived cached data (BndBox, UVBounds, etc.) in dense per-kind
//! vectors indexed by entity index. O(1) access by direct indexing — no hashing.
//!
//! ## SubtreeGen-based freshness
//! Each stored slot records SubtreeGen at write time. On read, if stored
//! SubtreeGen differs from entity's current SubtreeGen the attribute is
//! considered stale — the caller decides how to handle it.
//!
//! ## Lifecycle
//! NOT a Layer. Cleared on Build() and Compact(). No OnNodeRemoved handling —
//! stale data is auto-detected by SubtreeGen mismatch.
//!
//! ## Thread safety
//! After Reserve(), Get() and Set() for in-range indices bypass the mutex
//! entirely — safe because parallel algorithms access different entity slots.
//! Out-of-range access (entities added after Build) falls back to mutex.
class BRepGraph_TransientCache
{
public:
  //! Number of Kind enum slots to cover (0..11, with gap at 9).
  static constexpr int THE_KIND_COUNT =
    static_cast<int>(BRepGraph_NodeId::Kind::Occurrence) + 1;

  //! Per-slot storage: attribute handle + SubtreeGen stamp.
  struct CacheSlot
  {
    occ::handle<BRepGraph_UserAttribute> Attr;
    uint32_t                             StoredSubtreeGen = 0;
  };

  //! Store an attribute for a node at a given key.
  //! @param[in] theNode          target node
  //! @param[in] theKey           attribute key (from BRepGraph_AttrRegistry)
  //! @param[in] theAttr          attribute handle to store
  //! @param[in] theCurrentSubtreeGen current SubtreeGen of the entity
  void Set(const BRepGraph_NodeId                      theNode,
           const int                                   theKey,
           const occ::handle<BRepGraph_UserAttribute>& theAttr,
           const uint32_t                              theCurrentSubtreeGen);

  //! Retrieve an attribute for a node at a given key.
  //! If stored SubtreeGen differs from theCurrentSubtreeGen, the returned
  //! attribute is considered stale — the caller should check IsDirty()
  //! or recompute as appropriate.
  //! @param[in] theNode              target node
  //! @param[in] theKey               attribute key
  //! @param[in] theCurrentSubtreeGen current SubtreeGen of the entity
  //! @return attribute handle, or null if absent
  [[nodiscard]] occ::handle<BRepGraph_UserAttribute> Get(
    const BRepGraph_NodeId theNode,
    const int              theKey,
    const uint32_t         theCurrentSubtreeGen) const;

  //! Remove an attribute for a node at a given key.
  //! @return true if something was removed
  [[nodiscard]] bool Remove(const BRepGraph_NodeId theNode, const int theKey);

  //! True if any attributes are stored for this node (any key).
  [[nodiscard]] bool HasAttributes(const BRepGraph_NodeId theNode) const;

  //! Return all registered attribute keys that have non-null entries for this node.
  [[nodiscard]] NCollection_Vector<int> AttributeKeys(const BRepGraph_NodeId theNode) const;

  //! Transfer all attributes from a source cache to a destination node in this cache.
  void TransferAttributes(const BRepGraph_TransientCache& theSrcCache,
                          const BRepGraph_NodeId          theSrcNode,
                          const BRepGraph_NodeId          theDstNode,
                          const uint32_t                  theDstSubtreeGen);

  //! Pre-allocate storage for lock-free parallel access.
  //! Must be called after Build() when entity counts are final.
  //! After this call, Get() and Set() for in-range indices skip the mutex.
  //! @param[in] theMaxKey  highest attribute key expected (typically 2-3)
  //! @param[in] theCounts  entity count per kind (indexed by Kind enum)
  void Reserve(const int theMaxKey, const int theCounts[THE_KIND_COUNT]);

  //! True if Reserve() has been called and storage is pre-allocated.
  [[nodiscard]] bool IsReserved() const noexcept { return myIsReserved; }

  //! Clear all cached data. Called on Build() and Compact().
  //! Resets the pre-allocated state.
  void Clear() noexcept;

  //! Move constructor: transfers data, creates fresh mutex.
  BRepGraph_TransientCache(BRepGraph_TransientCache&& theOther) noexcept
      : myKeys(std::move(theOther.myKeys)),
        myIsReserved(theOther.myIsReserved)
  {
    theOther.myIsReserved = false;
  }

  //! Move assignment: transfers data, mutex stays local.
  BRepGraph_TransientCache& operator=(BRepGraph_TransientCache&& theOther) noexcept
  {
    if (this != &theOther)
    {
      myKeys        = std::move(theOther.myKeys);
      myIsReserved  = theOther.myIsReserved;
      theOther.myIsReserved = false;
    }
    return *this;
  }

  BRepGraph_TransientCache()                                           = default;
  BRepGraph_TransientCache(const BRepGraph_TransientCache&)            = delete;
  BRepGraph_TransientCache& operator=(const BRepGraph_TransientCache&) = delete;

private:

  //! Per-kind dense vector of cache slots.
  struct KindStore
  {
    NCollection_Vector<CacheSlot> mySlots;
  };

  //! Per-key storage: one KindStore per entity kind.
  struct KeySlot
  {
    KindStore myKinds[THE_KIND_COUNT];
  };

  //! Ensure myKeys has capacity for the given key index.
  void ensureKey(const int theKey);

  //! Access slot (mutable) — grows vector if needed.
  CacheSlot& changeSlot(const BRepGraph_NodeId theNode, const int theKey);

  //! Access slot (const) — returns nullptr if out of range.
  const CacheSlot* seekSlot(const BRepGraph_NodeId theNode, const int theKey) const;

  //! Outer vector indexed by attribute key (0, 1, 2, ...).
  NCollection_Vector<KeySlot> myKeys;

  //! True after Reserve() — enables lock-free access for in-range slots.
  bool myIsReserved = false;

  //! Protects structural modifications (vector growth) during concurrent access.
  //! Bypassed for in-range access when myIsReserved is true.
  mutable std::shared_mutex myMutex;
};

#endif // _BRepGraph_TransientCache_HeaderFile
