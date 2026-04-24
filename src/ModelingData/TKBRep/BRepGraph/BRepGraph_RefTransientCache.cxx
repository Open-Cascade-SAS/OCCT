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

#include <BRepGraph_RefTransientCache.hxx>

//=================================================================================================

void BRepGraph_RefTransientCache::ensureKind(const int theKindSlot)
{
  if (theKindSlot >= myKinds.Length())
  {
    myKinds.SetValue(theKindSlot, CacheKindSlot());
  }
}

//=================================================================================================

BRepGraph_RefTransientCache::CacheSlot& BRepGraph_RefTransientCache::changeSlot(
  const BRepGraph_RefId theRef,
  const int             theKindSlot)
{
  ensureKind(theKindSlot);
  const int aRefKindIdx = static_cast<int>(theRef.RefKind);
  Standard_ASSERT_VOID(aRefKindIdx >= 0 && aRefKindIdx < THE_REF_KIND_COUNT,
                       "BRepGraph_RefTransientCache: RefKind out of range");
  NCollection_DynamicArray<CacheSlot>& aVec =
    myKinds.ChangeValue(theKindSlot).myRefKinds[aRefKindIdx].mySlots;
  if (theRef.Index >= aVec.Size())
  {
    return aVec.SetValue(static_cast<size_t>(theRef.Index), CacheSlot());
  }
  return aVec.ChangeValue(static_cast<size_t>(theRef.Index));
}

//=================================================================================================

const BRepGraph_RefTransientCache::CacheSlot* BRepGraph_RefTransientCache::seekSlot(
  const BRepGraph_RefId theRef,
  const int             theKindSlot) const
{
  if (theKindSlot < 0 || theKindSlot >= myKinds.Length())
  {
    return nullptr;
  }

  const int aRefKindIdx = static_cast<int>(theRef.RefKind);
  Standard_ASSERT_VOID(aRefKindIdx >= 0 && aRefKindIdx < THE_REF_KIND_COUNT,
                       "BRepGraph_RefTransientCache: RefKind out of range");
  const NCollection_DynamicArray<CacheSlot>& aVec =
    myKinds.Value(theKindSlot).myRefKinds[aRefKindIdx].mySlots;
  if (!theRef.IsValidIn(aVec))
  {
    return nullptr;
  }
  return &aVec.Value(static_cast<size_t>(theRef.Index));
}

//=================================================================================================

void BRepGraph_RefTransientCache::Reserve(const int theKindCount,
                                          const int theCounts[THE_REF_KIND_COUNT])
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
    for (int aRefKindIdx = 0; aRefKindIdx < THE_REF_KIND_COUNT; ++aRefKindIdx)
    {
      const int aCount = theCounts[aRefKindIdx];
      if (aCount > 0 && aCount > aKindSlotData.myRefKinds[aRefKindIdx].mySlots.Length())
      {
        aKindSlotData.myRefKinds[aRefKindIdx].mySlots.SetValue(aCount - 1, CacheSlot());
      }
    }
  }

  myIsReserved.store(true, std::memory_order_release);
}

//=================================================================================================

void BRepGraph_RefTransientCache::Set(const BRepGraph_RefId                    theRef,
                                      const occ::handle<BRepGraph_CacheKind>&  theKind,
                                      const occ::handle<BRepGraph_CacheValue>& theValue,
                                      const uint32_t                           theCurrentOwnGen)
{
  if (!theRef.IsValid() || theKind.IsNull())
  {
    return;
  }

  const int aKindSlot = BRepGraph_CacheKindRegistry::Register(theKind);
  if (aKindSlot < 0)
  {
    return;
  }

  Set(theRef, aKindSlot, theValue, theCurrentOwnGen);
}

//=================================================================================================

void BRepGraph_RefTransientCache::Set(const BRepGraph_RefId                    theRef,
                                      const int                                theKindSlot,
                                      const occ::handle<BRepGraph_CacheValue>& theValue,
                                      const uint32_t                           theCurrentOwnGen)
{
  if (!theRef.IsValid() || theKindSlot < 0)
  {
    return;
  }

  if (myIsReserved.load(std::memory_order_acquire) && theKindSlot < myKinds.Length())
  {
    const int                      aRefKindIdx = static_cast<int>(theRef.RefKind);
    NCollection_DynamicArray<CacheSlot>& aVec =
      myKinds.ChangeValue(theKindSlot).myRefKinds[aRefKindIdx].mySlots;
    if (theRef.Index < aVec.Size())
    {
      CacheSlot& aSlot   = aVec.ChangeValue(static_cast<size_t>(theRef.Index));
      aSlot.Value        = theValue;
      aSlot.StoredOwnGen = theCurrentOwnGen;
      return;
    }
  }

  std::unique_lock<std::shared_mutex> aLock(myMutex);
  CacheSlot&                          aSlot = changeSlot(theRef, theKindSlot);
  aSlot.Value                               = theValue;
  aSlot.StoredOwnGen                        = theCurrentOwnGen;
}

//=================================================================================================

occ::handle<BRepGraph_CacheValue> BRepGraph_RefTransientCache::Get(
  const BRepGraph_RefId                   theRef,
  const occ::handle<BRepGraph_CacheKind>& theKind,
  const uint32_t                          theCurrentOwnGen) const
{
  if (!theRef.IsValid() || theKind.IsNull())
  {
    return occ::handle<BRepGraph_CacheValue>();
  }

  const int aKindSlot = BRepGraph_CacheKindRegistry::FindSlot(theKind->ID());
  if (aKindSlot < 0)
  {
    return occ::handle<BRepGraph_CacheValue>();
  }

  return Get(theRef, aKindSlot, theCurrentOwnGen);
}

//=================================================================================================

occ::handle<BRepGraph_CacheValue> BRepGraph_RefTransientCache::Get(
  const BRepGraph_RefId theRef,
  const int             theKindSlot,
  const uint32_t        theCurrentOwnGen) const
{
  if (!theRef.IsValid() || theKindSlot < 0)
  {
    return occ::handle<BRepGraph_CacheValue>();
  }

  if (myIsReserved.load(std::memory_order_acquire) && theKindSlot < myKinds.Length())
  {
    const int                            aRefKindIdx = static_cast<int>(theRef.RefKind);
    const NCollection_DynamicArray<CacheSlot>& aVec =
      myKinds.Value(theKindSlot).myRefKinds[aRefKindIdx].mySlots;
    if (theRef.Index < aVec.Size())
    {
      const CacheSlot& aSlot = aVec.Value(static_cast<size_t>(theRef.Index));
      if (aSlot.Value.IsNull())
      {
        return occ::handle<BRepGraph_CacheValue>();
      }
      if (aSlot.StoredOwnGen != theCurrentOwnGen)
      {
        return occ::handle<BRepGraph_CacheValue>();
      }
      return aSlot.Value;
    }
  }

  std::shared_lock<std::shared_mutex> aLock(myMutex);
  const CacheSlot*                    aSlot = seekSlot(theRef, theKindSlot);
  if (aSlot == nullptr || aSlot->Value.IsNull())
  {
    return occ::handle<BRepGraph_CacheValue>();
  }
  if (aSlot->StoredOwnGen != theCurrentOwnGen)
  {
    return occ::handle<BRepGraph_CacheValue>();
  }
  return aSlot->Value;
}

//=================================================================================================

bool BRepGraph_RefTransientCache::Remove(const BRepGraph_RefId                   theRef,
                                         const occ::handle<BRepGraph_CacheKind>& theKind)
{
  if (!theRef.IsValid() || theKind.IsNull())
  {
    return false;
  }

  const int aKindSlot = BRepGraph_CacheKindRegistry::FindSlot(theKind->ID());
  if (aKindSlot < 0)
  {
    return false;
  }

  return Remove(theRef, aKindSlot);
}

//=================================================================================================

bool BRepGraph_RefTransientCache::Remove(const BRepGraph_RefId theRef, const int theKindSlot)
{
  if (!theRef.IsValid() || theKindSlot < 0)
  {
    return false;
  }

  std::unique_lock<std::shared_mutex> aLock(myMutex);
  if (theKindSlot >= myKinds.Length())
  {
    return false;
  }

  const int                      aRefKindIdx = static_cast<int>(theRef.RefKind);
  NCollection_DynamicArray<CacheSlot>& aVec =
    myKinds.ChangeValue(theKindSlot).myRefKinds[aRefKindIdx].mySlots;
  if (theRef.Index >= aVec.Size())
  {
    return false;
  }

  CacheSlot& aSlot = aVec.ChangeValue(static_cast<size_t>(theRef.Index));
  if (aSlot.Value.IsNull())
  {
    return false;
  }
  aSlot.Value.Nullify();
  aSlot.StoredOwnGen = 0;
  return true;
}

//=================================================================================================

int BRepGraph_RefTransientCache::CollectCacheKindSlots(const BRepGraph_RefId theRef,
                                                       const uint32_t        theCurrentOwnGen,
                                                       int                   theSlots[]) const
{
  int aCount = 0;
  if (!theRef.IsValid())
  {
    return aCount;
  }

  for (int aKindSlot = 0; aKindSlot < myKinds.Length(); ++aKindSlot)
  {
    const CacheSlot* aSlot = seekSlot(theRef, aKindSlot);
    if (aSlot != nullptr && !aSlot->Value.IsNull() && aSlot->StoredOwnGen == theCurrentOwnGen)
    {
      theSlots[aCount++] = aKindSlot;
    }
  }
  return aCount;
}

//=================================================================================================

void BRepGraph_RefTransientCache::Clear() noexcept
{
  myKinds.Clear();
  myIsReserved.store(false, std::memory_order_relaxed);
}
