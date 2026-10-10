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

#include <BRepGraphSupInc_StoreRegistry.hxx>

#include <Standard_OutOfRange.hxx>

#include <limits>
#include <mutex>
#include <utility>

namespace
{
std::atomic<uint64_t> THE_NEXT_STORE_ID{1};
std::atomic<bool>     THE_STORE_ID_EXHAUSTED{false};

uint32_t allocateStoreId()
{
  Standard_OutOfRange_Raise_if(THE_STORE_ID_EXHAUSTED.load(),
                               "BRepGraphSupInc_StoreRegistry - StoreId overflow");
  const uint64_t aStoreId = THE_NEXT_STORE_ID.fetch_add(1);
  if (aStoreId > std::numeric_limits<uint32_t>::max())
  {
    THE_STORE_ID_EXHAUSTED.store(true);
    Standard_OutOfRange_Raise_if(true, "BRepGraphSupInc_StoreRegistry - StoreId overflow");
  }
  return static_cast<uint32_t>(aStoreId);
}
} // namespace

//=================================================================================================

BRepGraphSupInc_StoreRegistry::~BRepGraphSupInc_StoreRegistry()
{
  std::unique_lock<std::shared_mutex> aLock(myMutex);
  for (const occ::handle<BRepGraphSupInc_Store>& aStore : myStores)
  {
    if (!aStore.IsNull())
    {
      aStore->unbindRegistry(aStore->StoreId());
    }
  }
}

//=================================================================================================

BRepGraphSupInc_StoreRegistry::BRepGraphSupInc_StoreRegistry(
  BRepGraphSupInc_StoreRegistry&& theOther) noexcept
{
  std::unique_lock<std::shared_mutex> aLock(theOther.myMutex);
  myStores        = std::move(theOther.myStores);
  myGUIDToSlot    = std::move(theOther.myGUIDToSlot);
  myStoreIdToSlot = std::move(theOther.myStoreIdToSlot);
}

//=================================================================================================

BRepGraphSupInc_StoreRegistry& BRepGraphSupInc_StoreRegistry::operator=(
  BRepGraphSupInc_StoreRegistry&& theOther) noexcept
{
  if (this != &theOther)
  {
    std::unique_lock<std::shared_mutex> aThisLock(myMutex, std::defer_lock);
    std::unique_lock<std::shared_mutex> anOtherLock(theOther.myMutex, std::defer_lock);
    std::lock(aThisLock, anOtherLock);

    for (const occ::handle<BRepGraphSupInc_Store>& aStore : myStores)
    {
      if (!aStore.IsNull())
      {
        aStore->unbindRegistry(aStore->StoreId());
      }
    }
    myStores        = std::move(theOther.myStores);
    myGUIDToSlot    = std::move(theOther.myGUIDToSlot);
    myStoreIdToSlot = std::move(theOther.myStoreIdToSlot);
  }
  return *this;
}

//=================================================================================================

bool BRepGraphSupInc_StoreRegistry::RegisterStore(
  const occ::handle<BRepGraphSupInc_Store>& theStore)
{
  if (theStore.IsNull() || theStore->ID() == Standard_GUID())
  {
    return false;
  }

  std::unique_lock<std::shared_mutex> aLock(myMutex);
  if (myGUIDToSlot.Seek(theStore->ID()) != nullptr)
  {
    return false;
  }

  Standard_OutOfRange_Raise_if(myStores.Size() > std::numeric_limits<uint32_t>::max(),
                               "BRepGraphSupInc_StoreRegistry - too many stores");
  const uint32_t aStoreId = allocateStoreId();
  if (!theStore->bindRegistry(aStoreId))
  {
    return false;
  }
  const uint32_t aSlot = static_cast<uint32_t>(myStores.Size());
  myStores.Append(theStore);
  myGUIDToSlot.Bind(theStore->ID(), aSlot);
  myStoreIdToSlot.Bind(aStoreId, aSlot);
  return true;
}

//=================================================================================================

bool BRepGraphSupInc_StoreRegistry::registerCompactedStore(
  const occ::handle<BRepGraphSupInc_Store>& theStore,
  const uint32_t                            theStoreId)
{
  if (theStore.IsNull() || theStore->ID() == Standard_GUID() || theStoreId == 0)
  {
    return false;
  }

  std::unique_lock<std::shared_mutex> aLock(myMutex);
  if (myGUIDToSlot.Seek(theStore->ID()) != nullptr || myStoreIdToSlot.Seek(theStoreId) != nullptr
      || !theStore->bindRegistry(theStoreId))
  {
    return false;
  }
  const uint32_t aSlot = static_cast<uint32_t>(myStores.Size());
  myStores.Append(theStore);
  myGUIDToSlot.Bind(theStore->ID(), aSlot);
  myStoreIdToSlot.Bind(theStoreId, aSlot);
  return true;
}

//=================================================================================================

occ::handle<BRepGraphSupInc_Store> BRepGraphSupInc_StoreRegistry::FindStore(
  const uint32_t theStoreId) const
{
  std::shared_lock<std::shared_mutex> aLock(myMutex);
  const uint32_t*                     aSlot = myStoreIdToSlot.Seek(theStoreId);
  return aSlot == nullptr || static_cast<size_t>(*aSlot) >= myStores.Size()
           ? occ::handle<BRepGraphSupInc_Store>()
           : myStores.Value(static_cast<size_t>(*aSlot));
}

//=================================================================================================

uint32_t BRepGraphSupInc_StoreRegistry::StoreId(const Standard_GUID& theGUID) const
{
  const occ::handle<BRepGraphSupInc_Store> aStore = FindStore(theGUID);
  return aStore.IsNull() ? 0 : aStore->StoreId();
}

//=================================================================================================

occ::handle<BRepGraphSupInc_Store> BRepGraphSupInc_StoreRegistry::FindStore(
  const Standard_GUID& theGUID) const
{
  std::shared_lock<std::shared_mutex> aLock(myMutex);
  const uint32_t*                     aSlot = myGUIDToSlot.Seek(theGUID);
  if (aSlot == nullptr || static_cast<size_t>(*aSlot) >= myStores.Size())
  {
    return occ::handle<BRepGraphSupInc_Store>();
  }
  return myStores.Value(static_cast<size_t>(*aSlot));
}

//=================================================================================================

bool BRepGraphSupInc_StoreRegistry::UnregisterStore(const Standard_GUID& theGUID)
{
  std::unique_lock<std::shared_mutex> aLock(myMutex);
  const uint32_t*                     aSlotPtr = myGUIDToSlot.Seek(theGUID);
  if (aSlotPtr == nullptr)
  {
    return false;
  }

  const uint32_t aSlot = *aSlotPtr;
  if (static_cast<size_t>(aSlot) >= myStores.Size())
  {
    return false;
  }

  const occ::handle<BRepGraphSupInc_Store> aStore = myStores.Value(static_cast<size_t>(aSlot));
  if (aStore.IsNull() || !aStore->IsEmpty())
  {
    return false;
  }

  const uint32_t aLastSlot = static_cast<uint32_t>(myStores.Size() - 1);
  if (aSlot != aLastSlot)
  {
    const occ::handle<BRepGraphSupInc_Store>& aLastStore =
      myStores.Value(static_cast<size_t>(aLastSlot));
    myStores.ChangeValue(static_cast<size_t>(aSlot))  = aLastStore;
    myGUIDToSlot.ChangeFind(aLastStore->ID())         = aSlot;
    myStoreIdToSlot.ChangeFind(aLastStore->StoreId()) = aSlot;
  }
  myStores.EraseLast();
  myGUIDToSlot.UnBind(theGUID);
  myStoreIdToSlot.UnBind(aStore->StoreId());
  aStore->unbindRegistry(aStore->StoreId());
  return true;
}

//=================================================================================================

void BRepGraphSupInc_StoreRegistry::ClearAll() noexcept
{
  NCollection_LinearVector<occ::handle<BRepGraphSupInc_Store>> aStores;
  {
    std::shared_lock<std::shared_mutex> aLock(myMutex);
    aStores = myStores;
  }

  for (const occ::handle<BRepGraphSupInc_Store>& aStore : aStores)
  {
    if (!aStore.IsNull())
    {
      aStore->Clear();
    }
  }
}

//=================================================================================================

uint32_t BRepGraphSupInc_StoreRegistry::Count() const
{
  std::shared_lock<std::shared_mutex> aLock(myMutex);
  return static_cast<uint32_t>(myStores.Size());
}
