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

#ifndef _BRepGraph_CacheRegistry_HeaderFile
#define _BRepGraph_CacheRegistry_HeaderFile

#include <BRepGraph_Cache.hxx>
#include <BRepGraph_CopyRemap.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_FlatDataMap.hxx>
#include <NCollection_LinearVector.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_GUID.hxx>

#include <functional>
#include <mutex>
#include <shared_mutex>

class BRepGraph;
struct BRepGraph_Data;
class BRepGraph_CacheIterator;

//! @brief GUID-keyed runtime registry of graph cache services.
//!
//! Stores registered cache services in a stable slot array for O(1) slot access
//! and a GUID-to-slot map for lookup by stable public identity. Cache services
//! own their typed transient data; this registry only manages identity and
//! owner binding.
class BRepGraph_CacheRegistry
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BRepGraph_CacheRegistry();

  BRepGraph_CacheRegistry(const BRepGraph_CacheRegistry&)            = delete;
  BRepGraph_CacheRegistry& operator=(const BRepGraph_CacheRegistry&) = delete;

  Standard_EXPORT BRepGraph_CacheRegistry(BRepGraph_CacheRegistry&& theOther) noexcept;
  Standard_EXPORT BRepGraph_CacheRegistry& operator=(BRepGraph_CacheRegistry&& theOther) noexcept;

  //! Register a cache service. Replaces an existing cache with the same GUID.
  //! @param[in] theCache cache service
  //! @return graph-local slot index
  Standard_EXPORT uint32_t RegisterCache(const occ::handle<BRepGraph_Cache>& theCache);

  //! Register a cache service. Short form used by graph-local cache operations.
  //! @param[in] theCache cache service
  //! @return graph-local slot index
  Standard_EXPORT uint32_t Register(const occ::handle<BRepGraph_Cache>& theCache);

  //! Remove a cache service by GUID.
  //! @param[in] theGUID cache identity
  Standard_EXPORT void UnregisterCache(const Standard_GUID& theGUID);

  //! Find a cache service by GUID.
  //! @param[in] theGUID cache identity
  //! @return cache service, or null handle if not found
  [[nodiscard]] Standard_EXPORT occ::handle<BRepGraph_Cache> FindCache(
    const Standard_GUID& theGUID) const;

  //! Typed convenience lookup by cache GUID.
  template <typename T>
  [[nodiscard]] occ::handle<T> FindCache() const
  {
    return Find<T>();
  }

  //! Typed lookup by cache GUID.
  template <typename T>
  [[nodiscard]] occ::handle<T> Find() const
  {
    return occ::down_cast<T>(FindCache(T::GetID()));
  }

  //! Return an existing cache service or create and register a default one.
  //! Template convenience wrapper: extracts GUID and calls ensureCache.
  template <typename T>
  [[nodiscard]] occ::handle<T> Ensure()
  {
    return occ::down_cast<T>(
      ensureCache(T::GetID(), []() -> occ::handle<BRepGraph_Cache> { return new T(); }));
  }

  //! Return current graph-local slot for a GUID.
  //! @param[in] theGUID cache family identity
  //! @param[out] theSlot graph-local slot index
  //! @return true if the cache service is registered
  [[nodiscard]] Standard_EXPORT bool FindSlot(const Standard_GUID& theGUID,
                                              uint32_t&            theSlot) const;

  //! Return current graph-local slot for a cache service.
  //! @param[in] theCache cache service
  //! @param[out] theSlot graph-local slot index
  //! @return true if the cache service is registered
  [[nodiscard]] Standard_EXPORT bool FindSlot(const occ::handle<BRepGraph_Cache>& theCache,
                                              uint32_t&                           theSlot) const;

  //! Return cache service by graph-local slot, or null handle if the slot is out of range.
  //! @param[in] theSlot graph-local cache slot
  [[nodiscard]] Standard_EXPORT occ::handle<BRepGraph_Cache> Cache(uint32_t theSlot) const;

  //! Number of registered cache services.
  [[nodiscard]] uint32_t NbCaches() const
  {
    std::shared_lock<std::shared_mutex> aLock(myMutex);
    return static_cast<uint32_t>(myCaches.Size());
  }

  //! Iterate registered cache services.
  [[nodiscard]] Standard_EXPORT BRepGraph_CacheIterator CacheIter() const;

  //! Clear data in all registered cache services.
  Standard_EXPORT void ClearAll() noexcept;

  //! Ask registered cache services to copy fresh, remappable data into the target graph.
  Standard_EXPORT void CopyFreshCachesTo(
    BRepGraph&                                                         theTargetGraph,
    const NCollection_FlatDataMap<BRepGraph_ItemId, BRepGraph_ItemId>& theItemRemap,
    const BRepGraph_CopyRemap::Mode                                    theMode) const;

  //! Ask registered cache services to copy fresh data using identity mapping.
  Standard_EXPORT void CopyFreshCachesTo(BRepGraph&                       theTargetGraph,
                                         BRepGraph_CopyRemap::MappingKind theMappingKind,
                                         BRepGraph_CopyRemap::Mode        theMode) const;

  //! Unregister all cache services.
  Standard_EXPORT void Clear() noexcept;

private:
  friend class ::BRepGraph;
  friend struct ::BRepGraph_Data;

  //! Attach this registry to graph owner. Propagates context to registered caches.
  Standard_EXPORT void Attach(BRepGraph* theGraph) noexcept;

  //! Clear the graph data binding.
  Standard_EXPORT void Detach() noexcept;

  [[nodiscard]] Standard_EXPORT occ::handle<BRepGraph_Cache> findCacheLocked(
    const Standard_GUID& theGUID) const;

  //! Return an existing cache service or create and register a default one.
  //! Uses double-checked locking: shared lock for fast path (cache exists),
  //! exclusive lock only for creation (rare, first-call only).
  [[nodiscard]] Standard_EXPORT occ::handle<BRepGraph_Cache> ensureCache(
    const Standard_GUID&                                 theGUID,
    const std::function<occ::handle<BRepGraph_Cache>()>& theFactory);

  [[nodiscard]] Standard_EXPORT occ::handle<BRepGraph_Cache> cacheAt(uint32_t theSlot) const;

  Standard_EXPORT uint32_t registerCacheLocked(const occ::handle<BRepGraph_Cache>& theCache);

  Standard_EXPORT void detachAllLocked() noexcept;

  NCollection_LinearVector<occ::handle<BRepGraph_Cache>> myCaches;
  NCollection_DataMap<Standard_GUID, uint32_t>           myGuidToSlot;
  BRepGraph*                                             myGraph = nullptr;
  mutable std::shared_mutex                              myMutex;
};

#include <BRepGraph_CacheIterator.hxx>

#endif // _BRepGraph_CacheRegistry_HeaderFile
