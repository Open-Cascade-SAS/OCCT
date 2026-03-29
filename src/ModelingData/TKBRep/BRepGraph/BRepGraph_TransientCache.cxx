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

//=================================================================================================

void BRepGraph_TransientCache::ensureKey(const int theKey)
{
  while (theKey >= myKeys.Length())
  {
    myKeys.Append(KeySlot());
  }
}

//=================================================================================================

BRepGraph_TransientCache::CacheSlot& BRepGraph_TransientCache::changeSlot(
  const BRepGraph_NodeId theNode,
  const int              theKey)
{
  ensureKey(theKey);
  const int                     aKindIdx = static_cast<int>(theNode.NodeKind);
  NCollection_Vector<CacheSlot>& aVec    = myKeys.ChangeValue(theKey).myKinds[aKindIdx].mySlots;
  while (theNode.Index >= aVec.Length())
  {
    aVec.Append(CacheSlot());
  }
  return aVec.ChangeValue(theNode.Index);
}

//=================================================================================================

const BRepGraph_TransientCache::CacheSlot* BRepGraph_TransientCache::seekSlot(
  const BRepGraph_NodeId theNode,
  const int              theKey) const
{
  if (theKey >= myKeys.Length())
    return nullptr;
  const int                           aKindIdx = static_cast<int>(theNode.NodeKind);
  const NCollection_Vector<CacheSlot>& aVec    = myKeys.Value(theKey).myKinds[aKindIdx].mySlots;
  if (theNode.Index >= aVec.Length())
    return nullptr;
  return &aVec.Value(theNode.Index);
}

//=================================================================================================

void BRepGraph_TransientCache::Reserve(const int theMaxKey, const int theCounts[THE_KIND_COUNT])
{
  std::unique_lock<std::shared_mutex> aLock(myMutex);

  // Pre-allocate key slots.
  ensureKey(theMaxKey);

  // Pre-allocate per-kind vectors for each key.
  for (int aKeyIdx = 0; aKeyIdx <= theMaxKey; ++aKeyIdx)
  {
    KeySlot& aKeySlot = myKeys.ChangeValue(aKeyIdx);
    for (int aKindIdx = 0; aKindIdx < THE_KIND_COUNT; ++aKindIdx)
    {
      const int aCount = theCounts[aKindIdx];
      NCollection_Vector<CacheSlot>& aVec = aKeySlot.myKinds[aKindIdx].mySlots;
      while (aVec.Length() < aCount)
      {
        aVec.Append(CacheSlot());
      }
    }
  }

  myIsReserved.store(true, std::memory_order_release);
}

//=================================================================================================

void BRepGraph_TransientCache::Set(const BRepGraph_NodeId                      theNode,
                                   const int                                   theKey,
                                   const occ::handle<BRepGraph_UserAttribute>& theAttr,
                                   const uint32_t                              theCurrentSubtreeGen)
{
  if (!theNode.IsValid() || theKey < 0)
    return;

  // Lock-free fast path: slot is pre-allocated and in range.
  // Acquire on myIsReserved ensures visibility of all Reserve() writes.
  if (myIsReserved.load(std::memory_order_acquire)
      && theKey < myKeys.Length())
  {
    const int                     aKindIdx = static_cast<int>(theNode.NodeKind);
    NCollection_Vector<CacheSlot>& aVec    = myKeys.ChangeValue(theKey).myKinds[aKindIdx].mySlots;
    if (theNode.Index < aVec.Length())
    {
      CacheSlot& aSlot       = aVec.ChangeValue(theNode.Index);
      aSlot.Attr             = theAttr;
      aSlot.StoredSubtreeGen = theCurrentSubtreeGen;
      return;
    }
  }

  // Fallback: acquire exclusive lock and grow as needed.
  std::unique_lock<std::shared_mutex> aLock(myMutex);
  CacheSlot& aSlot        = changeSlot(theNode, theKey);
  aSlot.Attr              = theAttr;
  aSlot.StoredSubtreeGen  = theCurrentSubtreeGen;
}

//=================================================================================================

occ::handle<BRepGraph_UserAttribute> BRepGraph_TransientCache::Get(
  const BRepGraph_NodeId theNode,
  const int              theKey,
  const uint32_t         theCurrentSubtreeGen) const
{
  // Lock-free fast path: slot is pre-allocated and in range.
  // Acquire on myIsReserved ensures visibility of all Reserve() writes.
  if (myIsReserved.load(std::memory_order_acquire)
      && theKey < myKeys.Length())
  {
    const int                           aKindIdx = static_cast<int>(theNode.NodeKind);
    const NCollection_Vector<CacheSlot>& aVec    = myKeys.Value(theKey).myKinds[aKindIdx].mySlots;
    if (theNode.Index < aVec.Length())
    {
      const CacheSlot& aSlot = aVec.Value(theNode.Index);
      if (aSlot.Attr.IsNull())
        return occ::handle<BRepGraph_UserAttribute>();
      // SubtreeGen-based freshness: if the entity's SubtreeGen has changed
      // since the attribute was stored, the cached value is stale.
      // Return null to force recomputation by the caller.
      if (aSlot.StoredSubtreeGen != theCurrentSubtreeGen)
        return occ::handle<BRepGraph_UserAttribute>();
      return aSlot.Attr;
    }
  }

  // Fallback: acquire shared lock.
  std::shared_lock<std::shared_mutex> aLock(myMutex);
  const CacheSlot* aSlot = seekSlot(theNode, theKey);
  if (aSlot == nullptr || aSlot->Attr.IsNull())
    return occ::handle<BRepGraph_UserAttribute>();
  if (aSlot->StoredSubtreeGen != theCurrentSubtreeGen)
    return occ::handle<BRepGraph_UserAttribute>();
  return aSlot->Attr;
}

//=================================================================================================

bool BRepGraph_TransientCache::Remove(const BRepGraph_NodeId theNode, const int theKey)
{
  if (!theNode.IsValid() || theKey < 0)
    return false;
  std::unique_lock<std::shared_mutex> aLock(myMutex);
  if (theKey >= myKeys.Length())
    return false;
  const int                     aKindIdx = static_cast<int>(theNode.NodeKind);
  NCollection_Vector<CacheSlot>& aVec    = myKeys.ChangeValue(theKey).myKinds[aKindIdx].mySlots;
  if (theNode.Index >= aVec.Length())
    return false;
  CacheSlot& aSlot = aVec.ChangeValue(theNode.Index);
  if (aSlot.Attr.IsNull())
    return false;
  aSlot.Attr.Nullify();
  aSlot.StoredSubtreeGen = 0;
  return true;
}

//=================================================================================================

bool BRepGraph_TransientCache::HasAttributes(const BRepGraph_NodeId theNode) const
{
  if (!theNode.IsValid())
    return false;
  for (int aKeyIdx = 0; aKeyIdx < myKeys.Length(); ++aKeyIdx)
  {
    const CacheSlot* aSlot = seekSlot(theNode, aKeyIdx);
    if (aSlot != nullptr && !aSlot->Attr.IsNull())
      return true;
  }
  return false;
}

//=================================================================================================

NCollection_Vector<int> BRepGraph_TransientCache::AttributeKeys(
  const BRepGraph_NodeId theNode) const
{
  NCollection_Vector<int> aKeys;
  if (!theNode.IsValid())
    return aKeys;
  for (int aKeyIdx = 0; aKeyIdx < myKeys.Length(); ++aKeyIdx)
  {
    const CacheSlot* aSlot = seekSlot(theNode, aKeyIdx);
    if (aSlot != nullptr && !aSlot->Attr.IsNull())
      aKeys.Append(aKeyIdx);
  }
  return aKeys;
}

//=================================================================================================

void BRepGraph_TransientCache::TransferAttributes(const BRepGraph_TransientCache& theSrcCache,
                                                  const BRepGraph_NodeId          theSrcNode,
                                                  const BRepGraph_NodeId          theDstNode,
                                                  const uint32_t                  theDstSubtreeGen)
{
  if (!theSrcNode.IsValid() || !theDstNode.IsValid())
    return;
  for (int aKeyIdx = 0; aKeyIdx < theSrcCache.myKeys.Length(); ++aKeyIdx)
  {
    const CacheSlot* aSrcSlot = theSrcCache.seekSlot(theSrcNode, aKeyIdx);
    if (aSrcSlot == nullptr || aSrcSlot->Attr.IsNull())
      continue;
    Set(theDstNode, aKeyIdx, aSrcSlot->Attr, theDstSubtreeGen);
  }
}

//=================================================================================================

void BRepGraph_TransientCache::Clear() noexcept
{
  myKeys.Clear();
  myIsReserved.store(false, std::memory_order_relaxed);
}
