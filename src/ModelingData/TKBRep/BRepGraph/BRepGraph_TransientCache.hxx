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

#include <Standard_GUID.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_AsciiString.hxx>

#include <NCollection_Vector.hxx>

#include <atomic>
#include <functional>
#include <mutex>
#include <shared_mutex>

//! @brief Descriptor of one transient cache family.
//!
//! A cache kind defines stable public identity for one class of transient,
//! recomputable per-node data such as bounding boxes or UV bounds.
//! Instances are process-global descriptors registered in
//! BRepGraph_CacheKindRegistry and referenced from graphs by dense runtime slots.
class BRepGraph_CacheKind : public Standard_Transient
{
public:
  //! Create a cache kind descriptor.
  //! @param[in] theID            stable public GUID identity
  //! @param[in] theName          display-only name
  //! @param[in] theNodeKindsMask optional node-kind applicability mask;
  //!                             0 means "unspecified / unrestricted"
  Standard_EXPORT BRepGraph_CacheKind(const Standard_GUID&           theID,
                                      const TCollection_AsciiString& theName          = TCollection_AsciiString(),
                                      const int                      theNodeKindsMask = 0);

  //! Stable public identity.
  [[nodiscard]] const Standard_GUID& ID() const { return myID; }

  //! Display-only metadata.
  [[nodiscard]] const TCollection_AsciiString& Name() const { return myName; }

  //! Optional node-kind applicability mask.
  [[nodiscard]] int NodeKindsMask() const { return myNodeKindsMask; }

  //! True if this cache kind is applicable to the given node kind.
  //! Cache kinds with NodeKindsMask() == 0 are treated as unrestricted.
  [[nodiscard]] bool SupportsNodeKind(const BRepGraph_NodeId::Kind theKind) const
  {
    return myNodeKindsMask == 0 || (myNodeKindsMask & KindBit(theKind)) != 0;
  }

  //! Convenience: bitmask bit for a given node kind.
  static int KindBit(const BRepGraph_NodeId::Kind theKind)
  {
    return 1 << static_cast<int>(theKind);
  }

  DEFINE_STANDARD_RTTIEXT(BRepGraph_CacheKind, Standard_Transient)

private:
  Standard_GUID           myID;
  TCollection_AsciiString myName;
  int                     myNodeKindsMask = 0;
};

//! @brief Process-global registry of cache kind descriptors.
//!
//! Maps stable GUID identity to dense runtime slot index. Slot indices are an
//! internal storage detail used by BRepGraph_TransientCache for O(1) indexing.
//! The registry is shared across all BRepGraph instances in the current process,
//! so cache-kind GUIDs should be globally unique.
class BRepGraph_CacheKindRegistry
{
public:
  //! Register a cache kind descriptor.
  //! Idempotent: the same GUID always yields the same slot.
  //! Slot assignment is process-global and graph-instance independent.
  //! @return dense runtime slot, or -1 for null input
  [[nodiscard]] Standard_EXPORT static int Register(const occ::handle<BRepGraph_CacheKind>& theKind);

  //! Find slot by GUID. Returns -1 if not found.
  [[nodiscard]] Standard_EXPORT static int FindSlot(const Standard_GUID& theGUID);

  //! Find slot by GUID.
  //! @param[out] theSlot dense runtime slot if found
  //! @return true if the GUID is registered
  Standard_EXPORT static bool FindSlot(const Standard_GUID& theGUID, int& theSlot);

  //! Find descriptor by GUID.
  [[nodiscard]] Standard_EXPORT static occ::handle<BRepGraph_CacheKind> FindKind(
    const Standard_GUID& theGUID);

  //! Find descriptor by slot.
  [[nodiscard]] Standard_EXPORT static occ::handle<BRepGraph_CacheKind> FindKind(const int theSlot);

  //! Check whether a GUID is registered.
  [[nodiscard]] Standard_EXPORT static bool Contains(const Standard_GUID& theGUID);

  //! Check whether a slot is registered.
  [[nodiscard]] Standard_EXPORT static bool Contains(const int theSlot);

  //! Number of registered cache kinds.
  [[nodiscard]] Standard_EXPORT static int NbRegistered();

private:
  BRepGraph_CacheKindRegistry() = delete;
};

//! @brief Abstract base for transient per-node cache values.
//!
//! Inherits from Standard_Transient and is stored via
//! occ::handle<BRepGraph_CacheValue>. This uses OCCT's embedded refcount and is
//! consistent with the Handle pattern used throughout the codebase.
class BRepGraph_CacheValue : public Standard_Transient
{
public:
  //! Mark the cached value as needing recomputation. Lock-free.
  void Invalidate() { myDirty.store(true, std::memory_order_release); }

  //! True if the cached value needs recomputation.
  bool IsDirty() const { return myDirty.load(std::memory_order_acquire); }

  DEFINE_STANDARD_RTTI_INLINE(BRepGraph_CacheValue, Standard_Transient)

protected:
  BRepGraph_CacheValue()
      : myDirty(true)
  {
  }

  //! Subclass calls after successful computation to clear the dirty flag.
  void MarkClean() const { myDirty.store(false, std::memory_order_release); }

  //! Mutex for thread-safe Get() in subclasses.
  mutable std::shared_mutex myMutex;

private:
  mutable std::atomic<bool> myDirty;
};

//! @brief Concrete typed wrapper for a lazily-computed per-node value.
//!
//! @tparam T cached value type (for example double).
template <typename T>
class BRepGraph_TypedCacheValue : public BRepGraph_CacheValue
{
public:
  BRepGraph_TypedCacheValue() = default;

  //! Construct with an initial value (marked clean).
  explicit BRepGraph_TypedCacheValue(const T& theInitial)
      : myValue(theInitial)
  {
    MarkClean();
  }

  //! Get the cached value, computing via theComputer if dirty.
  //! Thread-safe: uses the base class shared_mutex.
  T Get(const std::function<T()>& theComputer) const
  {
    if (!IsDirty())
    {
      std::shared_lock<std::shared_mutex> aLock(myMutex);
      if (!IsDirty())
      {
        return myValue;
      }
    }

    std::unique_lock<std::shared_mutex> aLock(myMutex);
    if (IsDirty())
    {
      myValue = theComputer();
      MarkClean();
    }
    return myValue;
  }

  //! Direct write - stores the value and marks clean.
  void Set(const T& theValue)
  {
    std::unique_lock<std::shared_mutex> aLock(myMutex);
    myValue = theValue;
    MarkClean();
  }

  //! Direct read. Caller must guarantee freshness.
  const T& UncheckedValue() const { return myValue; }

private:
  mutable T myValue{};
};

//! @brief Centralized transient cache for algorithm-computed per-node values.
//!
//! Stores short-lived cached data (BndBox, UVBounds, etc.) in dense per-cache-kind
//! vectors indexed by entity index. O(1) access by direct indexing - no hashing.
//!
//! ## SubtreeGen-based freshness
//! Each stored slot records SubtreeGen at write time. On read, if stored
//! SubtreeGen differs from entity's current SubtreeGen the cached value is
//! considered stale - the caller decides how to handle it.
//!
//! ## Lifecycle
//! NOT a Layer. Cleared on Build() and Compact(). No OnNodeRemoved handling -
//! stale data is auto-detected by SubtreeGen mismatch.
//!
//! ## Thread safety
//! After Reserve(), Get() and Set() for in-range indices bypass the mutex
//! entirely - safe because parallel algorithms access different entity slots.
//! Out-of-range access (entities added after Build) falls back to mutex.
class BRepGraph_TransientCache
{
public:
  //! Number of Kind enum slots to cover (0..11, with gap at 9).
  static constexpr int THE_KIND_COUNT = static_cast<int>(BRepGraph_NodeId::Kind::Occurrence) + 1;

  //! Default number of cache-kind slots reserved after Build().
  static constexpr int THE_DEFAULT_RESERVED_KIND_COUNT = 16;

  //! Per-slot storage: cached value handle + SubtreeGen stamp.
  struct CacheSlot
  {
    occ::handle<BRepGraph_CacheValue> Value;
    uint32_t                          StoredSubtreeGen = 0;
  };

  //! Store a cached value for a node and cache kind.
  //! @pre Reserve() must have been called for lock-free parallel access
  //!      on in-range entity indices; out-of-range access falls back to mutex.
  Standard_EXPORT void Set(const BRepGraph_NodeId                   theNode,
                           const occ::handle<BRepGraph_CacheKind>&  theKind,
                           const occ::handle<BRepGraph_CacheValue>& theValue,
                           const uint32_t                           theCurrentSubtreeGen);

  //! Store a cached value using a pre-resolved kind slot index.
  //! Bypasses BRepGraph_CacheKindRegistry lookup — use in hot parallel paths.
  //! @param[in] theKindSlot  slot from BRepGraph_CacheKindRegistry::Register()
  Standard_EXPORT void Set(const BRepGraph_NodeId                   theNode,
                           const int                                theKindSlot,
                           const occ::handle<BRepGraph_CacheValue>& theValue,
                           const uint32_t                           theCurrentSubtreeGen);

  //! Retrieve a cached value for a node and cache kind.
  //! @pre Reserve() must have been called for lock-free parallel access
  //!      on in-range entity indices; out-of-range access falls back to mutex.
  [[nodiscard]] Standard_EXPORT occ::handle<BRepGraph_CacheValue> Get(
    const BRepGraph_NodeId                  theNode,
    const occ::handle<BRepGraph_CacheKind>& theKind,
    const uint32_t                          theCurrentSubtreeGen) const;

  //! Retrieve a cached value using a pre-resolved kind slot index.
  //! Bypasses BRepGraph_CacheKindRegistry lookup — use in hot parallel paths.
  //! @param[in] theKindSlot  slot from BRepGraph_CacheKindRegistry::Register()
  [[nodiscard]] Standard_EXPORT occ::handle<BRepGraph_CacheValue> Get(
    const BRepGraph_NodeId theNode,
    const int              theKindSlot,
    const uint32_t         theCurrentSubtreeGen) const;

  //! Remove a cached value for a node and cache kind.
  [[nodiscard]] Standard_EXPORT bool Remove(const BRepGraph_NodeId                  theNode,
                                            const occ::handle<BRepGraph_CacheKind>& theKind);

  //! Remove a cached value using a pre-resolved cache-kind slot.
  [[nodiscard]] Standard_EXPORT bool Remove(const BRepGraph_NodeId theNode,
                                            const int              theKindSlot);

  //! True if any cached values are stored for this node (any cache kind).
  [[nodiscard]] Standard_EXPORT bool HasCacheValues(const BRepGraph_NodeId theNode) const;

  //! Return all registered cache kinds that have non-null entries for this node.
  [[nodiscard]] Standard_EXPORT NCollection_Vector<occ::handle<BRepGraph_CacheKind>> CacheKinds(
    const BRepGraph_NodeId theNode) const;

  //! Transfer all cached values from a source cache to a destination node.
  Standard_EXPORT void TransferCacheValues(const BRepGraph_TransientCache& theSrcCache,
                                         const BRepGraph_NodeId          theSrcNode,
                                         const BRepGraph_NodeId          theDstNode,
                                         const uint32_t                  theDstSubtreeGen);

  //! Pre-allocate storage for lock-free parallel access.
  Standard_EXPORT void Reserve(const int theKindCount, const int theCounts[THE_KIND_COUNT]);

  //! True if Reserve() has been called and storage is pre-allocated.
  [[nodiscard]] bool IsReserved() const noexcept
  {
    return myIsReserved.load(std::memory_order_acquire);
  }

  //! Clear all cached data. Called on Build() and Compact().
  Standard_EXPORT void Clear() noexcept;

  //! Move constructor: transfers data, creates fresh mutex.
  BRepGraph_TransientCache(BRepGraph_TransientCache&& theOther) noexcept
      : myKinds(std::move(theOther.myKinds)),
        myIsReserved(theOther.myIsReserved.load(std::memory_order_relaxed))
  {
    theOther.myIsReserved.store(false, std::memory_order_relaxed);
  }

  //! Move assignment: transfers data, mutex stays local.
  BRepGraph_TransientCache& operator=(BRepGraph_TransientCache&& theOther) noexcept
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

  BRepGraph_TransientCache()                                           = default;
  BRepGraph_TransientCache(const BRepGraph_TransientCache&)            = delete;
  BRepGraph_TransientCache& operator=(const BRepGraph_TransientCache&) = delete;

private:
  //! Per-node-kind dense vector of cache slots.
  struct NodeKindStore
  {
    NCollection_Vector<CacheSlot> mySlots;
  };

  //! Per-cache-kind storage: one node-kind store per entity kind.
  struct CacheKindSlot
  {
    NodeKindStore myNodeKinds[THE_KIND_COUNT];
  };

  //! Ensure myKinds has capacity for the given cache-kind slot.
  void ensureKind(const int theKindSlot);

  //! Access slot (mutable) - grows vector if needed.
  CacheSlot& changeSlot(const BRepGraph_NodeId theNode, const int theKindSlot);

  //! Access slot (const) - returns nullptr if out of range.
  const CacheSlot* seekSlot(const BRepGraph_NodeId theNode, const int theKindSlot) const;

  //! Outer vector indexed by cache-kind slot.
  NCollection_Vector<CacheKindSlot> myKinds;

  //! True after Reserve() - enables lock-free access for in-range slots.
  std::atomic<bool> myIsReserved{false};

  //! Protects structural modifications (vector growth) during concurrent access.
  mutable std::shared_mutex myMutex;
};

#endif // _BRepGraph_TransientCache_HeaderFile
