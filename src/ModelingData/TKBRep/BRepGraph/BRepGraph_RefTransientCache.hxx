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

#ifndef _BRepGraph_RefTransientCache_HeaderFile
#define _BRepGraph_RefTransientCache_HeaderFile

#include <BRepGraph_RefId.hxx>
#include <BRepGraph_TransientCache.hxx>

#include <NCollection_Vector.hxx>

#include <atomic>
#include <shared_mutex>

//! @brief Centralized transient cache for algorithm-computed per-reference values.
//!
//! Symmetric counterpart of BRepGraph_TransientCache, keyed by BRepGraph_RefId
//! instead of BRepGraph_NodeId.  Freshness is tracked via BaseRef::OwnGen rather
//! than BaseDef::SubtreeGen, because references do not own subtrees.
//!
//! Shares the same BRepGraph_CacheKind descriptors and BRepGraph_CacheKindRegistry
//! as the node cache; the same kind GUID can address values in both caches.
//!
//! ## OwnGen-based freshness
//! Each stored slot records OwnGen at write time. On read, if the stored OwnGen
//! differs from the reference's current OwnGen the cached value is considered stale.
//!
//! ## Lifecycle
//! NOT a Layer. Cleared on Build() and Compact(). No explicit removal callback -
//! stale data is auto-detected by OwnGen mismatch.
//!
//! ## Thread safety
//! After Reserve(), Get() and Set() for in-range indices bypass the mutex entirely.
//! Out-of-range access falls back to mutex-protected vector growth.
class BRepGraph_RefTransientCache
{
public:
  //! Number of BRepGraph_RefId::Kind enum values (Shell..Occurrence = 0..7).
  static constexpr int THE_REF_KIND_COUNT = 8;

  //! Default number of cache-kind slots reserved after Build().
  static constexpr int THE_DEFAULT_RESERVED_KIND_COUNT = 16;

  //! Per-slot storage: cached value handle + OwnGen stamp.
  struct CacheSlot
  {
    occ::handle<BRepGraph_CacheValue> Value;
    uint32_t                          StoredOwnGen = 0;
  };

  //! Store a cached value for a reference and cache kind.
  //! @pre Reserve() must have been called for lock-free parallel access
  //!      on in-range entity indices; out-of-range access falls back to mutex.
  Standard_EXPORT void Set(const BRepGraph_RefId                    theRef,
                           const occ::handle<BRepGraph_CacheKind>&  theKind,
                           const occ::handle<BRepGraph_CacheValue>& theValue,
                           const uint32_t                           theCurrentOwnGen);

  //! Store a cached value using a pre-resolved kind slot index.
  //! Bypasses BRepGraph_CacheKindRegistry lookup - use in hot parallel paths.
  //! @param[in] theKindSlot  slot from BRepGraph_CacheKindRegistry::Register()
  Standard_EXPORT void Set(const BRepGraph_RefId                    theRef,
                           const int                                theKindSlot,
                           const occ::handle<BRepGraph_CacheValue>& theValue,
                           const uint32_t                           theCurrentOwnGen);

  //! Retrieve a cached value for a reference and cache kind.
  //! Returns null handle if no value is stored or if OwnGen has changed.
  [[nodiscard]] Standard_EXPORT occ::handle<BRepGraph_CacheValue> Get(
    const BRepGraph_RefId                   theRef,
    const occ::handle<BRepGraph_CacheKind>& theKind,
    const uint32_t                          theCurrentOwnGen) const;

  //! Retrieve a cached value using a pre-resolved kind slot index.
  [[nodiscard]] Standard_EXPORT occ::handle<BRepGraph_CacheValue> Get(
    const BRepGraph_RefId theRef,
    const int             theKindSlot,
    const uint32_t        theCurrentOwnGen) const;

  //! Remove a cached value for a reference and cache kind.
  [[nodiscard]] Standard_EXPORT bool Remove(const BRepGraph_RefId                   theRef,
                                            const occ::handle<BRepGraph_CacheKind>& theKind);

  //! Remove a cached value using a pre-resolved cache-kind slot.
  [[nodiscard]] Standard_EXPORT bool Remove(const BRepGraph_RefId theRef, const int theKindSlot);

  //! Collect fresh cache-kind slot indices for a reference (zero heap allocation).
  //! Used internally by CacheView::CacheKindIterator.
  //! @param[in]  theRef     reference to query
  //! @param[in]  theCurrentOwnGen freshness stamp to match
  //! @param[out] theSlots   output array (caller-allocated, must hold
  //! THE_DEFAULT_RESERVED_KIND_COUNT)
  //! @return number of populated slots written to theSlots
  Standard_EXPORT int CollectCacheKindSlots(const BRepGraph_RefId theRef,
                                            const uint32_t        theCurrentOwnGen,
                                            int                   theSlots[]) const;

  //! Pre-allocate storage for lock-free parallel access.
  Standard_EXPORT void Reserve(const int theKindCount, const int theCounts[THE_REF_KIND_COUNT]);

  //! True if Reserve() has been called and storage is pre-allocated.
  [[nodiscard]] bool IsReserved() const noexcept
  {
    return myIsReserved.load(std::memory_order_acquire);
  }

  //! Clear all cached data. Called on Build() and Compact().
  Standard_EXPORT void Clear() noexcept;

  //! Move constructor: transfers data, creates fresh mutex.
  BRepGraph_RefTransientCache(BRepGraph_RefTransientCache&& theOther) noexcept
      : myKinds(std::move(theOther.myKinds)),
        myIsReserved(theOther.myIsReserved.load(std::memory_order_relaxed))
  {
    theOther.myIsReserved.store(false, std::memory_order_relaxed);
  }

  //! Move assignment: transfers data, mutex stays local.
  BRepGraph_RefTransientCache& operator=(BRepGraph_RefTransientCache&& theOther) noexcept
  {
    if (this != &theOther)
    {
      myKinds = std::move(theOther.myKinds);
      myIsReserved.store(theOther.myIsReserved.load(std::memory_order_relaxed),
                         std::memory_order_relaxed);
      theOther.myIsReserved.store(false, std::memory_order_relaxed);
    }
    return *this;
  }

  BRepGraph_RefTransientCache()                                              = default;
  BRepGraph_RefTransientCache(const BRepGraph_RefTransientCache&)            = delete;
  BRepGraph_RefTransientCache& operator=(const BRepGraph_RefTransientCache&) = delete;

private:
  //! Per-ref-kind dense vector of cache slots.
  struct RefKindStore
  {
    NCollection_Vector<CacheSlot> mySlots;
  };

  //! Per-cache-kind storage: one ref-kind store per reference kind.
  struct CacheKindSlot
  {
    RefKindStore myRefKinds[THE_REF_KIND_COUNT];
  };

  //! Ensure myKinds has capacity for the given cache-kind slot.
  void ensureKind(const int theKindSlot);

  //! Access slot (mutable) - grows vector if needed.
  CacheSlot& changeSlot(const BRepGraph_RefId theRef, const int theKindSlot);

  //! Access slot (const) - returns nullptr if out of range.
  const CacheSlot* seekSlot(const BRepGraph_RefId theRef, const int theKindSlot) const;

  //! Outer vector indexed by cache-kind slot.
  NCollection_Vector<CacheKindSlot> myKinds;

  //! True after Reserve() - enables lock-free access for in-range slots.
  std::atomic<bool> myIsReserved{false};

  //! Protects structural modifications (vector growth) during concurrent access.
  mutable std::shared_mutex myMutex;
};

#endif // _BRepGraph_RefTransientCache_HeaderFile
