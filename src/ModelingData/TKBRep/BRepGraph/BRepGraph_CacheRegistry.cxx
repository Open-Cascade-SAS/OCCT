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
#include <utility>

//=================================================================================================

BRepGraph_CacheRegistry::BRepGraph_CacheRegistry() = default;

//=================================================================================================

BRepGraph_CacheRegistry::BRepGraph_CacheRegistry(BRepGraph& theGraph)
{
  Attach(theGraph);
}

//=================================================================================================

BRepGraph_CacheRegistry::~BRepGraph_CacheRegistry()
{
  const bool wasAttached = IsAttached();
  Detach();
  releaseCaches(!wasAttached);
}

//=================================================================================================

BRepGraph_CacheRegistry::BRepGraph_CacheRegistry(BRepGraph_CacheRegistry&& theOther) noexcept
{
  {
    std::unique_lock<std::shared_mutex> aLock(theOther.myMutex);
    myCaches             = std::move(theOther.myCaches);
    myGuidToSlot         = std::move(theOther.myGuidToSlot);
    myGraph.store(theOther.myGraph.exchange(nullptr, std::memory_order_acq_rel),
                  std::memory_order_release);
    myIsExternal         = theOther.myIsExternal;
    theOther.myIsExternal = false;
    for (const occ::handle<BRepGraph_Cache>& aCache : myCaches)
    {
      if (!aCache.IsNull())
      {
        aCache->rebindRegistry(this);
      }
    }
  }
  BRepGraph* aGraph = myGraph.load(std::memory_order_acquire);
  if (myIsExternal && aGraph != nullptr)
  {
    aGraph->replaceExternalCacheRegistry(&theOther, this);
  }
}

//=================================================================================================

BRepGraph_CacheRegistry& BRepGraph_CacheRegistry::operator=(
  BRepGraph_CacheRegistry&& theOther) noexcept
{
  if (this != &theOther)
  {
    const bool wasAttached = IsAttached();
    Detach();
    releaseCaches(!wasAttached);
    std::unique_lock<std::shared_mutex> aThisLock(myMutex, std::defer_lock);
    std::unique_lock<std::shared_mutex> anOtherLock(theOther.myMutex, std::defer_lock);
    std::lock(aThisLock, anOtherLock);

    myCaches              = std::move(theOther.myCaches);
    myGuidToSlot          = std::move(theOther.myGuidToSlot);
    myGraph.store(theOther.myGraph.exchange(nullptr, std::memory_order_acq_rel),
                  std::memory_order_release);
    myIsExternal         = theOther.myIsExternal;
    theOther.myIsExternal = false;
    for (const occ::handle<BRepGraph_Cache>& aCache : myCaches)
    {
      if (!aCache.IsNull())
      {
        aCache->rebindRegistry(this);
      }
    }
    BRepGraph* aGraph = myGraph.load(std::memory_order_acquire);
    if (myIsExternal && aGraph != nullptr)
    {
      aGraph->replaceExternalCacheRegistry(&theOther, this);
    }
  }
  return *this;
}

//=================================================================================================

void BRepGraph_CacheRegistry::Attach(BRepGraph& theGraph)
{
  Standard_ProgramError_Raise_if(this == &theGraph.CacheRegistry(),
                                 "BRepGraph_CacheRegistry::Attach() - graph registry is internal");
  {
    std::shared_lock<std::shared_mutex> aLock(myMutex);
    BRepGraph* aGraph = myGraph.load(std::memory_order_acquire);
    if (aGraph == &theGraph)
    {
      return;
    }
    Standard_ProgramError_Raise_if(
      aGraph != nullptr || myIsDetaching,
      "BRepGraph_CacheRegistry::Attach() - registry is already attached");
  }

  theGraph.registerExternalCacheRegistry(this);

  NCollection_LinearVector<occ::handle<BRepGraph_Cache>> aCaches;
  {
    std::unique_lock<std::shared_mutex> aLock(myMutex);
    myGraph.store(&theGraph, std::memory_order_release);
    myIsExternal = true;
    for (const occ::handle<BRepGraph_Cache>& aCache : myCaches)
    {
      if (!aCache.IsNull())
      {
        aCache->attachGraph(this);
        aCache->beginAttach();
      }
    }
    aCaches = myCaches;
  }
  for (const occ::handle<BRepGraph_Cache>& aCache : aCaches)
  {
    if (!aCache.IsNull())
    {
      aCache->notifyAttached();
    }
  }
}

//=================================================================================================

void BRepGraph_CacheRegistry::attachGraph(BRepGraph* theGraph) noexcept
{
  NCollection_LinearVector<occ::handle<BRepGraph_Cache>> aCaches;
  {
    std::unique_lock<std::shared_mutex> aLock(myMutex);
    myGraph.store(theGraph, std::memory_order_release);
    myIsExternal = false;
    for (const occ::handle<BRepGraph_Cache>& aCache : myCaches)
    {
      if (!aCache.IsNull())
      {
        aCache->attachGraph(this);
        aCache->beginAttach();
      }
    }
    aCaches = myCaches;
  }
  if (theGraph != nullptr)
  {
    for (const occ::handle<BRepGraph_Cache>& aCache : aCaches)
    {
      if (!aCache.IsNull())
      {
        aCache->notifyAttached();
      }
    }
  }
}

//=================================================================================================

void BRepGraph_CacheRegistry::rebindGraph(BRepGraph* theGraph) noexcept
{
  std::unique_lock<std::shared_mutex> aLock(myMutex);
  myGraph.store(theGraph, std::memory_order_release);
  for (const occ::handle<BRepGraph_Cache>& aCache : myCaches)
  {
    if (!aCache.IsNull())
    {
      aCache->rebindRegistry(this);
    }
  }
}

//=================================================================================================

void BRepGraph_CacheRegistry::detachGraphFromOwner(BRepGraph* theGraph) noexcept
{
  NCollection_LinearVector<occ::handle<BRepGraph_Cache>> aCaches;
  {
    std::unique_lock<std::shared_mutex> aLock(myMutex);
    if (myGraph.load(std::memory_order_acquire) != theGraph || myIsDetaching)
    {
      return;
    }
    myIsDetaching = true;
    aCaches       = myCaches;
  }
  for (const occ::handle<BRepGraph_Cache>& aCache : aCaches)
  {
    if (!aCache.IsNull())
    {
      aCache->detachGraph();
    }
  }
  {
    std::unique_lock<std::shared_mutex> aLock(myMutex);
    myGraph.store(nullptr, std::memory_order_release);
    myIsExternal  = false;
    myIsDetaching = false;
  }
}

//=================================================================================================

void BRepGraph_CacheRegistry::Detach() noexcept
{
  BRepGraph*                                             aGraph     = nullptr;
  bool                                                   isExternal = false;
  NCollection_LinearVector<occ::handle<BRepGraph_Cache>> aCaches;
  {
    std::unique_lock<std::shared_mutex> aLock(myMutex);
    aGraph     = myGraph.load(std::memory_order_acquire);
    if (aGraph == nullptr || myIsDetaching)
    {
      return;
    }
    isExternal = myIsExternal;
    myIsDetaching = true;
    aCaches       = myCaches;
  }
  for (const occ::handle<BRepGraph_Cache>& aCache : aCaches)
  {
    if (!aCache.IsNull())
    {
      aCache->detachGraph();
    }
  }
  {
    std::unique_lock<std::shared_mutex> aLock(myMutex);
    myGraph.store(nullptr, std::memory_order_release);
    myIsExternal  = false;
    myIsDetaching = false;
  }
  if (isExternal && aGraph != nullptr)
  {
    aGraph->unregisterExternalCacheRegistry(this);
  }
}

//=================================================================================================

bool BRepGraph_CacheRegistry::IsAttached() const noexcept
{
  return myGraph.load(std::memory_order_acquire) != nullptr;
}

//=================================================================================================

bool BRepGraph_CacheRegistry::IsFor(const BRepGraph& theGraph) const noexcept
{
  return myGraph.load(std::memory_order_acquire) == &theGraph;
}

//=================================================================================================

BRepGraph* BRepGraph_CacheRegistry::AttachedGraph() const noexcept
{
  return myGraph.load(std::memory_order_acquire);
}

//=================================================================================================

uint32_t BRepGraph_CacheRegistry::RegisterCache(const occ::handle<BRepGraph_Cache>& theCache)
{
  occ::handle<BRepGraph_Cache> aRemoved;
  bool                         toAttach = false;
  uint32_t                     aSlot    = 0;
  {
    std::unique_lock<std::shared_mutex> aLock(myMutex);
    aSlot = registerCacheLocked(theCache, aRemoved, toAttach);
  }
  if (!aRemoved.IsNull())
  {
    if (aRemoved->IsAttached())
    {
      aRemoved->detachGraph();
    }
    else
    {
      aRemoved->Clear();
    }
    releaseIfUnregistered(aRemoved);
  }
  if (toAttach)
  {
    theCache->notifyAttached();
  }
  return aSlot;
}

//=================================================================================================

void BRepGraph_CacheRegistry::UnregisterCache(const Standard_GUID& theGUID)
{
  occ::handle<BRepGraph_Cache> aRemoved;
  {
    std::unique_lock<std::shared_mutex> aLock(myMutex);
    const uint32_t*                     aSlotPtr = myGuidToSlot.Seek(theGUID);
    if (aSlotPtr == nullptr)
    {
      return;
    }

    const uint32_t aSlot     = *aSlotPtr;
    const uint32_t aLastSlot = static_cast<uint32_t>(myCaches.Size() - 1);
    aRemoved                 = myCaches.Value(static_cast<size_t>(aSlot));
    if (aSlot != aLastSlot)
    {
      const occ::handle<BRepGraph_Cache>& aLastCache =
        myCaches.Value(static_cast<size_t>(aLastSlot));
      myCaches.ChangeValue(static_cast<size_t>(aSlot)) = aLastCache;
      myGuidToSlot.ChangeFind(aLastCache->ID())        = aSlot;
    }

    myCaches.EraseLast();
    myGuidToSlot.UnBind(theGUID);
  }
  if (!aRemoved.IsNull())
  {
    if (aRemoved->IsAttached())
    {
      aRemoved->detachGraph();
    }
    else
    {
      aRemoved->Clear();
    }
    releaseIfUnregistered(aRemoved);
  }
}

//=================================================================================================

occ::handle<BRepGraph_Cache> BRepGraph_CacheRegistry::FindCache(const Standard_GUID& theGUID) const
{
  return findCache(theGUID);
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

  occ::handle<BRepGraph_Cache> aCache;
  occ::handle<BRepGraph_Cache> aRemoved;
  bool                         toAttach = false;
  {
    std::unique_lock<std::shared_mutex> aLock(myMutex);
    // Re-check after acquiring exclusive lock (another thread may have created it).
    aCache = findCacheLocked(theGUID);
    if (aCache.IsNull())
    {
      aCache = theFactory();
      (void)registerCacheLocked(aCache, aRemoved, toAttach);
    }
  }
  if (toAttach)
  {
    aCache->notifyAttached();
  }
  return aCache;
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
  releaseCaches(true);
}

//=================================================================================================

void BRepGraph_CacheRegistry::releaseCaches(const bool theToClearDetached) noexcept
{
  NCollection_LinearVector<occ::handle<BRepGraph_Cache>> aCaches;
  {
    std::unique_lock<std::shared_mutex> aLock(myMutex);
    aCaches = std::move(myCaches);
    myGuidToSlot.Clear();
  }
  for (const occ::handle<BRepGraph_Cache>& aCache : aCaches)
  {
    if (!aCache.IsNull())
    {
      if (aCache->IsAttached())
      {
        aCache->detachGraph();
      }
      else if (theToClearDetached)
      {
        aCache->clearForRelease();
      }
      releaseIfUnregistered(aCache);
    }
  }
}

//=================================================================================================

void BRepGraph_CacheRegistry::releaseIfUnregistered(
  const occ::handle<BRepGraph_Cache>& theCache) noexcept
{
  if (theCache.IsNull())
  {
    return;
  }
  bool isRegistered = false;
  {
    std::shared_lock<std::shared_mutex> aLock(myMutex);
    const uint32_t* aSlot = myGuidToSlot.Seek(theCache->ID());
    isRegistered = aSlot != nullptr && static_cast<size_t>(*aSlot) < myCaches.Size()
                   && myCaches.Value(static_cast<size_t>(*aSlot)).get() == theCache.get();
  }
  if (!isRegistered)
  {
    theCache->releaseRegistry();
  }
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
  const BRepGraph_CopyRemap::Mode                                    theMode,
  const BRepGraph_CopyRemap::FreshnessPolicy theFreshnessPolicy) const
{
  BRepGraph* aSourceGraph = nullptr;
  {
    std::shared_lock<std::shared_mutex> aLock(myMutex);
    aSourceGraph = myGraph.load(std::memory_order_acquire);
  }
  if (aSourceGraph == nullptr)
  {
    return;
  }

  const BRepGraph_CopyRemap aCopy(*aSourceGraph,
                                  theTargetGraph,
                                  theItemRemap,
                                  theMode,
                                  theFreshnessPolicy);
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

void BRepGraph_CacheRegistry::CopyFreshCachesTo(
  BRepGraph&                                 theTargetGraph,
  const BRepGraph_CopyRemap::MappingKind     theMappingKind,
  const BRepGraph_CopyRemap::Mode            theMode,
  const BRepGraph_CopyRemap::FreshnessPolicy theFreshnessPolicy) const
{
  BRepGraph* aSourceGraph = nullptr;
  {
    std::shared_lock<std::shared_mutex> aLock(myMutex);
    aSourceGraph = myGraph.load(std::memory_order_acquire);
  }
  if (aSourceGraph == nullptr)
  {
    return;
  }

  const BRepGraph_CopyRemap aCopy(*aSourceGraph,
                                  theTargetGraph,
                                  theMappingKind,
                                  theMode,
                                  theFreshnessPolicy);
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

occ::handle<BRepGraph_Cache> BRepGraph_CacheRegistry::findCache(
  const Standard_GUID& theGUID) const
{
  std::shared_lock<std::shared_mutex> aLock(myMutex);
  return findCacheLocked(theGUID);
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

uint32_t BRepGraph_CacheRegistry::registerCacheLocked(
  const occ::handle<BRepGraph_Cache>& theCache,
  occ::handle<BRepGraph_Cache>&       theRemoved,
  bool&                               theToAttach)
{
  Standard_ProgramError_Raise_if(theCache.IsNull(),
                                 "BRepGraph_CacheRegistry::RegisterCache() - null cache");
  const BRepGraph_CacheRegistry* aRegistry =
    theCache->myRegistry.load(std::memory_order_acquire);
  Standard_ProgramError_Raise_if(
    aRegistry != nullptr && aRegistry != this,
                                 "BRepGraph_CacheRegistry::RegisterCache() - cache is attached "
                                 "to another registry");

  if (uint32_t* aSlot = myGuidToSlot.ChangeSeek(theCache->ID()))
  {
    if (static_cast<size_t>(*aSlot) < myCaches.Size())
    {
      const occ::handle<BRepGraph_Cache>& aPrev = myCaches.Value(static_cast<size_t>(*aSlot));
      if (!aPrev.IsNull() && aPrev.get() != theCache.get())
      {
        theRemoved = aPrev;
      }
      if (aPrev.get() != theCache.get())
      {
        theCache->attachGraph(this);
        theToAttach = myGraph.load(std::memory_order_acquire) != nullptr && !myIsDetaching;
        if (theToAttach)
        {
          theCache->beginAttach();
        }
      }
      myCaches.ChangeValue(static_cast<size_t>(*aSlot)) = theCache;
      return *aSlot;
    }
  }

  Standard_OutOfRange_Raise_if(myCaches.Size() > std::numeric_limits<uint32_t>::max(),
                               "BRepGraph_CacheRegistry - too many registered caches");
  const uint32_t aNewSlot = static_cast<uint32_t>(myCaches.Size());
  theCache->attachGraph(this);
  theToAttach = myGraph.load(std::memory_order_acquire) != nullptr && !myIsDetaching;
  if (theToAttach)
  {
    theCache->beginAttach();
  }
  myCaches.Append(theCache);
  myGuidToSlot.Bind(theCache->ID(), aNewSlot);
  return aNewSlot;
}
