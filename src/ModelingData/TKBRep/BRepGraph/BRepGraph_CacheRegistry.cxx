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

#include <BRepGraph_CacheRegistry.hxx>

#include <BRepGraph.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_ProgramError.hxx>

#include <limits>
#include <mutex>

//=================================================================================================

BRepGraph_CacheRegistry::BRepGraph_CacheRegistry() = default;

//=================================================================================================

BRepGraph_CacheRegistry::BRepGraph_CacheRegistry(BRepGraph_CacheRegistry&& theOther) noexcept
{
  std::unique_lock<std::shared_mutex> aLock(theOther.myMutex);
  myCaches         = std::move(theOther.myCaches);
  myGuidToSlot     = std::move(theOther.myGuidToSlot);
  myGraph          = theOther.myGraph;
  theOther.myGraph = nullptr;
}

//=================================================================================================

BRepGraph_CacheRegistry& BRepGraph_CacheRegistry::operator=(
  BRepGraph_CacheRegistry&& theOther) noexcept
{
  if (this != &theOther)
  {
    std::unique_lock<std::shared_mutex> aThisLock(myMutex, std::defer_lock);
    std::unique_lock<std::shared_mutex> anOtherLock(theOther.myMutex, std::defer_lock);
    std::lock(aThisLock, anOtherLock);

    detachAllLocked();
    myCaches         = std::move(theOther.myCaches);
    myGuidToSlot     = std::move(theOther.myGuidToSlot);
    myGraph          = theOther.myGraph;
    theOther.myGraph = nullptr;
  }
  return *this;
}

//=================================================================================================

void BRepGraph_CacheRegistry::Attach(BRepGraph* theGraph) noexcept
{
  std::unique_lock<std::shared_mutex> aLock(myMutex);
  myGraph = theGraph;
  for (const occ::handle<BRepGraph_Cache>& aCache : myCaches)
  {
    if (!aCache.IsNull())
    {
      aCache->rebindGraph(myGraph);
    }
  }
}

//=================================================================================================

void BRepGraph_CacheRegistry::Detach() noexcept
{
  std::unique_lock<std::shared_mutex> aLock(myMutex);
  detachAllLocked();
  myGraph = nullptr;
}

//=================================================================================================

uint32_t BRepGraph_CacheRegistry::RegisterCache(const occ::handle<BRepGraph_Cache>& theCache)
{
  std::unique_lock<std::shared_mutex> aLock(myMutex);
  return registerCacheLocked(theCache);
}

//=================================================================================================

void BRepGraph_CacheRegistry::UnregisterCache(const Standard_GUID& theGUID)
{
  std::unique_lock<std::shared_mutex> aLock(myMutex);
  const uint32_t*                     aSlotPtr = myGuidToSlot.Seek(theGUID);
  if (aSlotPtr == nullptr)
  {
    return;
  }

  const uint32_t                     aSlot     = *aSlotPtr;
  const uint32_t                     aLastSlot = static_cast<uint32_t>(myCaches.Size() - 1);
  const occ::handle<BRepGraph_Cache> aRemoved  = myCaches.Value(static_cast<size_t>(aSlot));
  if (aSlot != aLastSlot)
  {
    const occ::handle<BRepGraph_Cache>& aLastCache = myCaches.Value(static_cast<size_t>(aLastSlot));
    myCaches.ChangeValue(static_cast<size_t>(aSlot)) = aLastCache;
    myGuidToSlot.ChangeFind(aLastCache->ID())        = aSlot;
  }

  myCaches.EraseLast();
  myGuidToSlot.UnBind(theGUID);
  if (!aRemoved.IsNull())
  {
    aRemoved->detachGraph();
  }
}

//=================================================================================================

occ::handle<BRepGraph_Cache> BRepGraph_CacheRegistry::FindCache(const Standard_GUID& theGUID) const
{
  std::shared_lock<std::shared_mutex> aLock(myMutex);
  return findCacheLocked(theGUID);
}

//=================================================================================================

occ::handle<BRepGraph_Cache> BRepGraph_CacheRegistry::ensureCache(
  const Standard_GUID&                                 theGUID,
  const std::function<occ::handle<BRepGraph_Cache>()>& theFactory)
{
  // Fast path: shared lock for read-only lookup.
  {
    std::shared_lock<std::shared_mutex> aLock(myMutex);
    occ::handle<BRepGraph_Cache>        aCache = findCacheLocked(theGUID);
    if (!aCache.IsNull())
    {
      return aCache;
    }
  }

  // Slow path: exclusive lock for creation.
  {
    std::unique_lock<std::shared_mutex> aLock(myMutex);
    // Re-check after acquiring exclusive lock (another thread may have created it).
    occ::handle<BRepGraph_Cache> aCache = findCacheLocked(theGUID);
    if (aCache.IsNull())
    {
      aCache = theFactory();
      registerCacheLocked(aCache);
    }
    return aCache;
  }
}

//=================================================================================================

bool BRepGraph_CacheRegistry::FindSlot(const Standard_GUID& theGUID, uint32_t& theSlot) const
{
  std::shared_lock<std::shared_mutex> aLock(myMutex);
  const uint32_t*                     aSlot = myGuidToSlot.Seek(theGUID);
  if (aSlot == nullptr)
  {
    return false;
  }
  theSlot = *aSlot;
  return true;
}

//=================================================================================================

occ::handle<BRepGraph_Cache> BRepGraph_CacheRegistry::Cache(const uint32_t theSlot) const
{
  std::shared_lock<std::shared_mutex> aLock(myMutex);
  if (static_cast<size_t>(theSlot) >= myCaches.Size())
  {
    return occ::handle<BRepGraph_Cache>();
  }
  return myCaches.Value(static_cast<size_t>(theSlot));
}

//=================================================================================================

void BRepGraph_CacheRegistry::Clear() noexcept
{
  std::unique_lock<std::shared_mutex> aLock(myMutex);
  detachAllLocked();
  myCaches.Clear();
  myGuidToSlot.Clear();
}

//=================================================================================================

uint32_t BRepGraph_CacheRegistry::Register(const occ::handle<BRepGraph_Cache>& theCache)
{
  return RegisterCache(theCache);
}

//=================================================================================================

bool BRepGraph_CacheRegistry::FindSlot(const occ::handle<BRepGraph_Cache>& theCache,
                                       uint32_t&                           theSlot) const
{
  return !theCache.IsNull() && FindSlot(theCache->ID(), theSlot);
}

//=================================================================================================

BRepGraph_CacheIterator BRepGraph_CacheRegistry::CacheIter() const
{
  return BRepGraph_CacheIterator(*this);
}

//=================================================================================================

void BRepGraph_CacheRegistry::ClearAll() noexcept
{
  for (uint32_t aSlot = 0;; ++aSlot)
  {
    occ::handle<BRepGraph_Cache> aCache = cacheAt(aSlot);
    if (aCache.IsNull())
    {
      return;
    }
    aCache->Clear();
  }
}

//=================================================================================================

void BRepGraph_CacheRegistry::CopyFreshCachesTo(
  BRepGraph&                                                         theTargetGraph,
  const NCollection_FlatDataMap<BRepGraph_ItemId, BRepGraph_ItemId>& theItemRemap,
  const BRepGraph_CopyRemap::Mode                                    theMode) const
{
  BRepGraph* aSourceGraph = nullptr;
  {
    std::shared_lock<std::shared_mutex> aLock(myMutex);
    aSourceGraph = myGraph;
  }
  if (aSourceGraph == nullptr)
  {
    return;
  }

  const BRepGraph_CopyRemap aCopy(*aSourceGraph, theTargetGraph, theItemRemap, theMode);
  for (uint32_t aSlot = 0;; ++aSlot)
  {
    occ::handle<BRepGraph_Cache> aCache = cacheAt(aSlot);
    if (aCache.IsNull())
    {
      return;
    }
    aCache->CopyFreshTo(aCopy);
  }
}

//=================================================================================================

void BRepGraph_CacheRegistry::CopyFreshCachesTo(BRepGraph&                       theTargetGraph,
                                                BRepGraph_CopyRemap::MappingKind theMappingKind,
                                                BRepGraph_CopyRemap::Mode        theMode) const
{
  BRepGraph* aSourceGraph = nullptr;
  {
    std::shared_lock<std::shared_mutex> aLock(myMutex);
    aSourceGraph = myGraph;
  }
  if (aSourceGraph == nullptr)
  {
    return;
  }

  const BRepGraph_CopyRemap aCopy(*aSourceGraph, theTargetGraph, theMappingKind, theMode);
  for (uint32_t aSlot = 0;; ++aSlot)
  {
    occ::handle<BRepGraph_Cache> aCache = cacheAt(aSlot);
    if (aCache.IsNull())
    {
      return;
    }
    aCache->CopyFreshTo(aCopy);
  }
}

//=================================================================================================

occ::handle<BRepGraph_Cache> BRepGraph_CacheRegistry::findCacheLocked(
  const Standard_GUID& theGUID) const
{
  const uint32_t* aSlot = myGuidToSlot.Seek(theGUID);
  if (aSlot == nullptr || static_cast<size_t>(*aSlot) >= myCaches.Size())
  {
    return occ::handle<BRepGraph_Cache>();
  }
  return myCaches.Value(static_cast<size_t>(*aSlot));
}

//=================================================================================================

occ::handle<BRepGraph_Cache> BRepGraph_CacheRegistry::cacheAt(const uint32_t theSlot) const
{
  std::shared_lock<std::shared_mutex> aLock(myMutex);
  if (static_cast<size_t>(theSlot) >= myCaches.Size())
  {
    return occ::handle<BRepGraph_Cache>();
  }
  return myCaches.Value(static_cast<size_t>(theSlot));
}

//=================================================================================================

uint32_t BRepGraph_CacheRegistry::registerCacheLocked(const occ::handle<BRepGraph_Cache>& theCache)
{
  Standard_ProgramError_Raise_if(theCache.IsNull(),
                                 "BRepGraph_CacheRegistry::RegisterCache() - null cache");
  Standard_ProgramError_Raise_if(theCache->myGraph != nullptr && theCache->myGraph != myGraph,
                                 "BRepGraph_CacheRegistry::RegisterCache() - cache is attached "
                                 "to another graph");

  if (uint32_t* aSlot = myGuidToSlot.ChangeSeek(theCache->ID()))
  {
    if (static_cast<size_t>(*aSlot) < myCaches.Size())
    {
      const occ::handle<BRepGraph_Cache>& aPrev = myCaches.Value(static_cast<size_t>(*aSlot));
      if (!aPrev.IsNull() && aPrev.get() != theCache.get())
      {
        aPrev->detachGraph();
      }
      theCache->attachGraph(myGraph);
      myCaches.ChangeValue(static_cast<size_t>(*aSlot)) = theCache;
      return *aSlot;
    }
  }

  Standard_OutOfRange_Raise_if(myCaches.Size() > std::numeric_limits<uint32_t>::max(),
                               "BRepGraph_CacheRegistry - too many registered caches");
  const uint32_t aNewSlot = static_cast<uint32_t>(myCaches.Size());
  theCache->attachGraph(myGraph);
  myCaches.Append(theCache);
  myGuidToSlot.Bind(theCache->ID(), aNewSlot);
  return aNewSlot;
}

//=================================================================================================

void BRepGraph_CacheRegistry::detachAllLocked() noexcept
{
  for (const occ::handle<BRepGraph_Cache>& aCache : myCaches)
  {
    if (!aCache.IsNull())
    {
      aCache->detachGraph();
    }
  }
}
