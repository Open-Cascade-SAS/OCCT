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

#include <BRepGraph_TransientCache.hxx>

#include <NCollection_DataMap.hxx>
#include <NCollection_Vector.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepGraph_CacheKind, Standard_Transient)

namespace
{

struct BRepGraph_CacheKindRegistryData
{
  NCollection_DataMap<Standard_GUID, int>              GuidToSlot;
  NCollection_Vector<occ::handle<BRepGraph_CacheKind>> Kinds;
  std::shared_mutex                                    Mutex;
};

BRepGraph_CacheKindRegistryData& cacheKindRegistryData()
{
  static BRepGraph_CacheKindRegistryData aData;
  return aData;
}

} // namespace

//=================================================================================================

BRepGraph_CacheKind::BRepGraph_CacheKind(const Standard_GUID&           theID,
                                         const TCollection_AsciiString& theName,
                                         const int                      theNodeKindsMask)
    : myID(theID),
      myName(theName),
      myNodeKindsMask(theNodeKindsMask)
{
}

//=================================================================================================

int BRepGraph_CacheKindRegistry::Register(const occ::handle<BRepGraph_CacheKind>& theKind)
{
  if (theKind.IsNull())
  {
    return -1;
  }

  BRepGraph_CacheKindRegistryData& aData = cacheKindRegistryData();
  {
    std::shared_lock<std::shared_mutex> aReadLock(aData.Mutex);
    const int*                          aSlot = aData.GuidToSlot.Seek(theKind->ID());
    if (aSlot != nullptr)
    {
      return *aSlot;
    }
  }

  std::unique_lock<std::shared_mutex> aWriteLock(aData.Mutex);
  const int*                          aSlot = aData.GuidToSlot.Seek(theKind->ID());
  if (aSlot != nullptr)
  {
    return *aSlot;
  }

  const int aNewSlot = aData.Kinds.Length();
  aData.Kinds.Append(theKind);
  aData.GuidToSlot.Bind(theKind->ID(), aNewSlot);
  return aNewSlot;
}

//=================================================================================================

int BRepGraph_CacheKindRegistry::FindSlot(const Standard_GUID& theGUID)
{
  BRepGraph_CacheKindRegistryData&    aData = cacheKindRegistryData();
  std::shared_lock<std::shared_mutex> aLock(aData.Mutex);
  const int*                          aSlot = aData.GuidToSlot.Seek(theGUID);
  return aSlot != nullptr ? *aSlot : -1;
}

//=================================================================================================

bool BRepGraph_CacheKindRegistry::FindSlot(const Standard_GUID& theGUID, int& theSlot)
{
  theSlot = FindSlot(theGUID);
  return theSlot >= 0;
}

//=================================================================================================

occ::handle<BRepGraph_CacheKind> BRepGraph_CacheKindRegistry::FindKind(const Standard_GUID& theGUID)
{
  const int aSlot = FindSlot(theGUID);
  return aSlot >= 0 ? FindKind(aSlot) : occ::handle<BRepGraph_CacheKind>();
}

//=================================================================================================

occ::handle<BRepGraph_CacheKind> BRepGraph_CacheKindRegistry::FindKind(const int theSlot)
{
  BRepGraph_CacheKindRegistryData&    aData = cacheKindRegistryData();
  std::shared_lock<std::shared_mutex> aLock(aData.Mutex);
  if (theSlot < 0 || theSlot >= aData.Kinds.Length())
  {
    return occ::handle<BRepGraph_CacheKind>();
  }
  return aData.Kinds.Value(theSlot);
}

//=================================================================================================

bool BRepGraph_CacheKindRegistry::Contains(const Standard_GUID& theGUID)
{
  return FindSlot(theGUID) >= 0;
}

//=================================================================================================

bool BRepGraph_CacheKindRegistry::Contains(const int theSlot)
{
  BRepGraph_CacheKindRegistryData&    aData = cacheKindRegistryData();
  std::shared_lock<std::shared_mutex> aLock(aData.Mutex);
  return theSlot >= 0 && theSlot < aData.Kinds.Length() && !aData.Kinds.Value(theSlot).IsNull();
}

//=================================================================================================

int BRepGraph_CacheKindRegistry::NbRegistered()
{
  BRepGraph_CacheKindRegistryData&    aData = cacheKindRegistryData();
  std::shared_lock<std::shared_mutex> aLock(aData.Mutex);
  return aData.Kinds.Length();
}

//=================================================================================================

void BRepGraph_TransientCache::ensureKind(const int theKindSlot)
{
  if (theKindSlot >= myKinds.Length())
  {
    myKinds.SetValue(theKindSlot, CacheKindSlot());
  }
}

//=================================================================================================

BRepGraph_TransientCache::CacheSlot& BRepGraph_TransientCache::changeSlot(
  const BRepGraph_NodeId theNode,
  const int              theKindSlot)
{
  ensureKind(theKindSlot);
  const int aKindIdx = static_cast<int>(theNode.NodeKind);
  Standard_ASSERT_VOID(aKindIdx >= 0 && aKindIdx < THE_KIND_COUNT,
                       "BRepGraph_TransientCache: NodeKind out of range");
  NCollection_Vector<CacheSlot>& aVec =
    myKinds.ChangeValue(theKindSlot).myNodeKinds[aKindIdx].mySlots;
  if (theNode.Index >= aVec.Length())
  {
    return aVec.SetValue(theNode.Index, CacheSlot());
  }
  return aVec.ChangeValue(theNode.Index);
}

//=================================================================================================

const BRepGraph_TransientCache::CacheSlot* BRepGraph_TransientCache::seekSlot(
  const BRepGraph_NodeId theNode,
  const int              theKindSlot) const
{
  if (theKindSlot < 0 || theKindSlot >= myKinds.Length())
  {
    return nullptr;
  }

  const int aKindIdx = static_cast<int>(theNode.NodeKind);
  Standard_ASSERT_VOID(aKindIdx >= 0 && aKindIdx < THE_KIND_COUNT,
                       "BRepGraph_TransientCache: NodeKind out of range");
  const NCollection_Vector<CacheSlot>& aVec =
    myKinds.Value(theKindSlot).myNodeKinds[aKindIdx].mySlots;
  if (theNode.Index >= aVec.Length())
  {
    return nullptr;
  }
  return &aVec.Value(theNode.Index);
}

//=================================================================================================

void BRepGraph_TransientCache::Reserve(const int theKindCount, const int theCounts[THE_KIND_COUNT])
{
  std::unique_lock<std::shared_mutex> aLock(myMutex);

  const int aKindCount = theKindCount > 0 ? theKindCount : 0;
  if (aKindCount > 0)
  {
    ensureKind(aKindCount - 1);
  }

  for (int aKindSlot = 0; aKindSlot < aKindCount; ++aKindSlot)
  {
    CacheKindSlot& aKindSlotData = myKinds.ChangeValue(aKindSlot);
    for (int aKindIdx = 0; aKindIdx < THE_KIND_COUNT; ++aKindIdx)
    {
      const int aCount = theCounts[aKindIdx];
      if (aCount > 0 && aCount > aKindSlotData.myNodeKinds[aKindIdx].mySlots.Length())
      {
        aKindSlotData.myNodeKinds[aKindIdx].mySlots.SetValue(aCount - 1, CacheSlot());
      }
    }
  }

  myIsReserved.store(true, std::memory_order_release);
}

//=================================================================================================

void BRepGraph_TransientCache::Set(const BRepGraph_NodeId                   theNode,
                                   const occ::handle<BRepGraph_CacheKind>&  theKind,
                                   const occ::handle<BRepGraph_CacheValue>& theValue,
                                   const uint32_t                           theCurrentSubtreeGen)
{
  if (!theNode.IsValid() || theKind.IsNull())
  {
    return;
  }

  const int aKindSlot = BRepGraph_CacheKindRegistry::Register(theKind);
  if (aKindSlot < 0)
  {
    return;
  }

  Set(theNode, aKindSlot, theValue, theCurrentSubtreeGen);
}

//=================================================================================================

void BRepGraph_TransientCache::Set(const BRepGraph_NodeId                   theNode,
                                   const int                                theKindSlot,
                                   const occ::handle<BRepGraph_CacheValue>& theValue,
                                   const uint32_t                           theCurrentSubtreeGen)
{
  if (!theNode.IsValid() || theKindSlot < 0)
  {
    return;
  }

  if (myIsReserved.load(std::memory_order_acquire) && theKindSlot < myKinds.Length())
  {
    const int                      aKindIdx = static_cast<int>(theNode.NodeKind);
    NCollection_Vector<CacheSlot>& aVec =
      myKinds.ChangeValue(theKindSlot).myNodeKinds[aKindIdx].mySlots;
    if (theNode.Index < aVec.Length())
    {
      CacheSlot& aSlot       = aVec.ChangeValue(theNode.Index);
      aSlot.Value            = theValue;
      aSlot.StoredSubtreeGen = theCurrentSubtreeGen;
      return;
    }
  }

  std::unique_lock<std::shared_mutex> aLock(myMutex);
  CacheSlot&                          aSlot = changeSlot(theNode, theKindSlot);
  aSlot.Value                               = theValue;
  aSlot.StoredSubtreeGen                    = theCurrentSubtreeGen;
}

//=================================================================================================

occ::handle<BRepGraph_CacheValue> BRepGraph_TransientCache::Get(
  const BRepGraph_NodeId                  theNode,
  const occ::handle<BRepGraph_CacheKind>& theKind,
  const uint32_t                          theCurrentSubtreeGen) const
{
  if (!theNode.IsValid() || theKind.IsNull())
  {
    return occ::handle<BRepGraph_CacheValue>();
  }

  const int aKindSlot = BRepGraph_CacheKindRegistry::FindSlot(theKind->ID());
  if (aKindSlot < 0)
  {
    return occ::handle<BRepGraph_CacheValue>();
  }

  return Get(theNode, aKindSlot, theCurrentSubtreeGen);
}

//=================================================================================================

occ::handle<BRepGraph_CacheValue> BRepGraph_TransientCache::Get(
  const BRepGraph_NodeId theNode,
  const int              theKindSlot,
  const uint32_t         theCurrentSubtreeGen) const
{
  if (!theNode.IsValid() || theKindSlot < 0)
  {
    return occ::handle<BRepGraph_CacheValue>();
  }

  if (myIsReserved.load(std::memory_order_acquire) && theKindSlot < myKinds.Length())
  {
    const int                            aKindIdx = static_cast<int>(theNode.NodeKind);
    const NCollection_Vector<CacheSlot>& aVec =
      myKinds.Value(theKindSlot).myNodeKinds[aKindIdx].mySlots;
    if (theNode.Index < aVec.Length())
    {
      const CacheSlot& aSlot = aVec.Value(theNode.Index);
      if (aSlot.Value.IsNull())
      {
        return occ::handle<BRepGraph_CacheValue>();
      }
      if (aSlot.StoredSubtreeGen != theCurrentSubtreeGen)
      {
        return occ::handle<BRepGraph_CacheValue>();
      }
      return aSlot.Value;
    }
  }

  std::shared_lock<std::shared_mutex> aLock(myMutex);
  const CacheSlot*                    aSlot = seekSlot(theNode, theKindSlot);
  if (aSlot == nullptr || aSlot->Value.IsNull())
  {
    return occ::handle<BRepGraph_CacheValue>();
  }
  if (aSlot->StoredSubtreeGen != theCurrentSubtreeGen)
  {
    return occ::handle<BRepGraph_CacheValue>();
  }
  return aSlot->Value;
}

//=================================================================================================

bool BRepGraph_TransientCache::Remove(const BRepGraph_NodeId                  theNode,
                                      const occ::handle<BRepGraph_CacheKind>& theKind)
{
  if (!theNode.IsValid() || theKind.IsNull())
  {
    return false;
  }

  const int aKindSlot = BRepGraph_CacheKindRegistry::FindSlot(theKind->ID());
  if (aKindSlot < 0)
  {
    return false;
  }

  return Remove(theNode, aKindSlot);
}

//=================================================================================================

bool BRepGraph_TransientCache::Remove(const BRepGraph_NodeId theNode, const int theKindSlot)
{
  if (!theNode.IsValid() || theKindSlot < 0)
  {
    return false;
  }

  std::unique_lock<std::shared_mutex> aLock(myMutex);
  if (theKindSlot >= myKinds.Length())
  {
    return false;
  }

  const int                      aKindIdx = static_cast<int>(theNode.NodeKind);
  NCollection_Vector<CacheSlot>& aVec =
    myKinds.ChangeValue(theKindSlot).myNodeKinds[aKindIdx].mySlots;
  if (theNode.Index >= aVec.Length())
  {
    return false;
  }

  CacheSlot& aSlot = aVec.ChangeValue(theNode.Index);
  if (aSlot.Value.IsNull())
  {
    return false;
  }
  aSlot.Value.Nullify();
  aSlot.StoredSubtreeGen = 0;
  return true;
}

//=================================================================================================

int BRepGraph_TransientCache::CollectCacheKindSlots(const BRepGraph_NodeId theNode,
                                                    const uint32_t         theCurrentSubtreeGen,
                                                    int                    theSlots[]) const
{
  int aCount = 0;
  if (!theNode.IsValid())
  {
    return aCount;
  }

  for (int aKindSlot = 0; aKindSlot < myKinds.Length(); ++aKindSlot)
  {
    const CacheSlot* aSlot = seekSlot(theNode, aKindSlot);
    if (aSlot != nullptr && !aSlot->Value.IsNull()
        && aSlot->StoredSubtreeGen == theCurrentSubtreeGen)
    {
      theSlots[aCount++] = aKindSlot;
    }
  }
  return aCount;
}

//=================================================================================================

void BRepGraph_TransientCache::Clear() noexcept
{
  myKinds.Clear();
  myIsReserved.store(false, std::memory_order_relaxed);
}
