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

#include <BRepGraph_LayerDeferred.hxx>

#include <BRepGraph_LayerLock.hxx>
#include <BRepGraph_LayerRegistry.hxx>
#include <Standard_Assert.hxx>
#include <Standard_GUID.hxx>

#include <utility>

IMPLEMENT_STANDARD_RTTIEXT(BRepGraph_LayerDeferred, BRepGraph_Layer)

//=================================================================================================

bool BRepGraph_LayerDeferred::Entry::RepresentationStorage::ContainsKind(
  const RepresentationKind theKind) const
{
  for (size_t aRepresentationIdx = 0; aRepresentationIdx < mySize; ++aRepresentationIdx)
  {
    if (representationPtr(aRepresentationIdx)->Kind == theKind)
    {
      return true;
    }
  }
  return false;
}

//=================================================================================================

void BRepGraph_LayerDeferred::Entry::RepresentationStorage::Append(
  const Representation& theRepresentation)
{
  Standard_ASSERT_RAISE(mySize < THE_MAX_REPRESENTATIONS_PER_ITEM,
                        "Too many deferred representations for one graph item");
  ::new (static_cast<void*>(representationPtr(mySize))) Representation(theRepresentation);
  ++mySize;
}

//=================================================================================================

void BRepGraph_LayerDeferred::Entry::RepresentationStorage::Clear()
{
  for (size_t aRepresentationIdx = 0; aRepresentationIdx < mySize; ++aRepresentationIdx)
  {
    representationPtr(aRepresentationIdx)->~Representation();
  }
  mySize = 0;
}

//=================================================================================================

void BRepGraph_LayerDeferred::Entry::RepresentationStorage::copyFrom(
  const RepresentationStorage& theOther)
{
  for (size_t aRepresentationIdx = 0; aRepresentationIdx < theOther.mySize; ++aRepresentationIdx)
  {
    Append(theOther.Value(aRepresentationIdx));
  }
}

//=================================================================================================

void BRepGraph_LayerDeferred::Entry::RepresentationStorage::moveFrom(
  RepresentationStorage& theOther)
{
  for (size_t aRepresentationIdx = 0; aRepresentationIdx < theOther.mySize; ++aRepresentationIdx)
  {
    ::new (static_cast<void*>(representationPtr(mySize)))
      Representation(std::move(*theOther.representationPtr(aRepresentationIdx)));
    ++mySize;
  }
  theOther.Clear();
}

//=================================================================================================

BRepGraph_LayerDeferred::Entry::RepresentationStorage& BRepGraph_LayerDeferred::Entry::
  RepresentationStorage::operator=(const RepresentationStorage& theOther)
{
  if (this != &theOther)
  {
    Clear();
    copyFrom(theOther);
  }
  return *this;
}

//=================================================================================================

BRepGraph_LayerDeferred::Entry::RepresentationStorage& BRepGraph_LayerDeferred::Entry::
  RepresentationStorage::operator=(RepresentationStorage&& theOther) noexcept
{
  if (this != &theOther)
  {
    Clear();
    moveFrom(theOther);
  }
  return *this;
}

//=================================================================================================

BRepGraph_LayerDeferred::BRepGraph_LayerDeferred()
    : myEntries(1)
{
}

//=================================================================================================

const Standard_GUID& BRepGraph_LayerDeferred::GetID()
{
  static const Standard_GUID THE_ID("9b6555f8-4353-414c-a067-c96a5e47fef7");
  return THE_ID;
}

//=================================================================================================

const Standard_GUID& BRepGraph_LayerDeferred::ID() const
{
  return GetID();
}

//=================================================================================================

const BRepGraph_LayerDeferred::Entry* BRepGraph_LayerDeferred::FindDeferred(
  const BRepGraph_ItemId theItem) const
{
  return myEntries.Seek(theItem);
}

//=================================================================================================

bool BRepGraph_LayerDeferred::HasDeferred(const BRepGraph_ItemId theItem) const
{
  return myEntries.IsBound(theItem);
}

//=================================================================================================

const BRepGraph_LayerDeferred::Entry* BRepGraph_LayerDeferred::FindFirstDeferred(
  const RepresentationKind theKind,
  BRepGraph_ItemId*        theItem) const
{
  for (NCollection_DataMap<BRepGraph_ItemId, Entry>::Iterator anIt(myEntries); anIt.More();
       anIt.Next())
  {
    if (!anIt.Value().Representations.ContainsKind(theKind))
    {
      continue;
    }
    if (theItem != nullptr)
    {
      *theItem = anIt.Key();
    }
    return &anIt.Value();
  }
  return nullptr;
}

//=================================================================================================

void BRepGraph_LayerDeferred::RegisterDeferred(const BRepGraph_ItemId         theItem,
                                               const TCollection_AsciiString& theProvider,
                                               const TCollection_AsciiString& theSourceKey,
                                               const RepresentationKind       theRepresentationKind,
                                               const TCollection_AsciiString& theRepresentationName,
                                               const uint32_t                 theSourceIndex)
{
  Representation aRepresentation;
  aRepresentation.Kind        = theRepresentationKind;
  aRepresentation.Name        = theRepresentationName;
  aRepresentation.SourceIndex = theSourceIndex;
  RegisterDeferredRepresentations(theItem, theProvider, theSourceKey, &aRepresentation, 1);
}

//=================================================================================================

void BRepGraph_LayerDeferred::RegisterDeferredRepresentations(
  const BRepGraph_ItemId         theItem,
  const TCollection_AsciiString& theProvider,
  const TCollection_AsciiString& theSourceKey,
  const Representation*          theRepresentations,
  const size_t                   theNbRepresentations)
{
  if (!theItem.IsValid())
  {
    return;
  }

  if (theRepresentations == nullptr || theNbRepresentations == 0)
  {
    return;
  }

  bool   isChanged = false;
  Entry& anEntry   = myEntries.TryEmplaced(theItem);
  if (!(anEntry.Provider == theProvider))
  {
    anEntry.Provider = theProvider;
    isChanged        = true;
  }
  if (!(anEntry.SourceKey == theSourceKey))
  {
    anEntry.SourceKey = theSourceKey;
    isChanged         = true;
  }

  for (size_t aSrcRepresentationIdx = 0; aSrcRepresentationIdx < theNbRepresentations;
       ++aSrcRepresentationIdx)
  {
    const Representation& aSrcRepresentation = theRepresentations[aSrcRepresentationIdx];
    bool                  isKnown            = false;
    for (size_t aStoredRepresentationIdx = 0;
         aStoredRepresentationIdx < anEntry.Representations.Size();
         ++aStoredRepresentationIdx)
    {
      const Representation& aRepresentation =
        anEntry.Representations.Value(aStoredRepresentationIdx);
      if (aRepresentation.Kind == aSrcRepresentation.Kind
          && aRepresentation.Role == aSrcRepresentation.Role
          && aRepresentation.Name == aSrcRepresentation.Name
          && aRepresentation.SourceIndex == aSrcRepresentation.SourceIndex)
      {
        isKnown = true;
        break;
      }
    }
    if (isKnown)
    {
      continue;
    }

    anEntry.Representations.Append(aSrcRepresentation);
    isChanged = true;
  }

  if (isChanged)
  {
    lockItem(theItem);
    if (myBulkRegistrationDepth != 0)
    {
      myHasBulkChanges = true;
    }
    else
    {
      touch();
    }
  }
}

//=================================================================================================

void BRepGraph_LayerDeferred::RegisterDeferredRepresentationsDirect(
  const BRepGraph_ItemId         theItem,
  const TCollection_AsciiString& theProvider,
  const TCollection_AsciiString& theSourceKey,
  const Representation*          theRepresentations,
  const size_t                   theNbRepresentations)
{
  if (!theItem.IsValid() || theRepresentations == nullptr || theNbRepresentations == 0)
  {
    return;
  }
  Standard_ASSERT_RETURN(
    !myEntries.IsBound(theItem),
    "RegisterDeferredRepresentationsDirect requires an item without deferred representations",
    Standard_VOID_RETURN);

  Entry anEntry;
  anEntry.Provider  = theProvider;
  anEntry.SourceKey = theSourceKey;
  for (size_t aRepresentationIdx = 0; aRepresentationIdx < theNbRepresentations;
       ++aRepresentationIdx)
  {
    anEntry.Representations.Append(theRepresentations[aRepresentationIdx]);
  }

  myEntries.Bind(theItem, std::move(anEntry));
  lockItem(theItem);
  if (myBulkRegistrationDepth != 0)
  {
    myHasBulkChanges = true;
  }
  else
  {
    touch();
  }
}

//=================================================================================================

void BRepGraph_LayerDeferred::UnregisterDeferred(const BRepGraph_ItemId theItem)
{
  if (!theItem.IsValid())
  {
    return;
  }

  if (myEntries.UnBind(theItem))
  {
    unlockItem(theItem);
    touch();
  }
}

//=================================================================================================

void BRepGraph_LayerDeferred::ReserveDeferredItems(const size_t theNbItems)
{
  myEntries.ReSize(myEntries.Extent() + theNbItems);
  if (BRepGraph* aGraph = AttachedGraph())
  {
    occ::handle<BRepGraph_LayerLock> aLockLayer =
      aGraph->LayerRegistry().Ensure<BRepGraph_LayerLock>();
    aLockLayer->ReserveOwners(theNbItems);
  }
}

//=================================================================================================

void BRepGraph_LayerDeferred::BeginBulkRegistration()
{
  if (myBulkRegistrationDepth == 0)
  {
    myHasBulkChanges = false;
  }
  ++myBulkRegistrationDepth;
}

//=================================================================================================

void BRepGraph_LayerDeferred::EndBulkRegistration()
{
  Standard_ASSERT_RAISE(myBulkRegistrationDepth > 0,
                        "BRepGraph_LayerDeferred::EndBulkRegistration without matching begin");
  --myBulkRegistrationDepth;
  if (myBulkRegistrationDepth != 0)
  {
    return;
  }

  const bool hasChanges = myHasBulkChanges;
  myHasBulkChanges      = false;
  if (!hasChanges)
  {
    return;
  }

  touch();
  if (BRepGraph* aGraph = AttachedGraph())
  {
    if (occ::handle<BRepGraph_LayerLock> aLockLayer =
          aGraph->LayerRegistry().FindLayer<BRepGraph_LayerLock>();
        !aLockLayer.IsNull())
    {
      aLockLayer->TouchOwners();
    }
  }
}

//=================================================================================================

const TCollection_AsciiString& BRepGraph_LayerDeferred::Name() const
{
  static const TCollection_AsciiString THE_NAME("Deferred");
  return THE_NAME;
}

//=================================================================================================

void BRepGraph_LayerDeferred::OnNodeRemoved(const BRepGraph_NodeId theNode) noexcept
{
  removeItem(BRepGraph_ItemId(theNode));
}

//=================================================================================================

void BRepGraph_LayerDeferred::OnNodeReplaced(const BRepGraph_NodeId theOldNode,
                                             const BRepGraph_NodeId theNewNode) noexcept
{
  const BRepGraph_ItemId anOldItem(theOldNode);
  const BRepGraph_ItemId aNewItem(theNewNode);
  const Entry*           anOldEntry = myEntries.Seek(anOldItem);
  if (anOldEntry == nullptr)
  {
    return;
  }

  Entry anEntry = *anOldEntry;
  myEntries.UnBind(anOldItem);
  if (Entry* aTarget = myEntries.ChangeSeek(aNewItem))
  {
    for (size_t i = 0; i < anEntry.Representations.Size(); ++i)
    {
      aTarget->Representations.Append(anEntry.Representations.Value(i));
    }
  }
  else
  {
    myEntries.Bind(aNewItem, anEntry);
  }
  touch();
}

//=================================================================================================

void BRepGraph_LayerDeferred::CopyTo(const BRepGraph_CopyRemap& theCopy) const
{
  if (myEntries.IsEmpty())
  {
    return;
  }

  occ::handle<BRepGraph_LayerDeferred> aTarget =
    theCopy.TargetGraph().LayerRegistry().Ensure<BRepGraph_LayerDeferred>();
  aTarget->ReserveDeferredItems(static_cast<size_t>(myEntries.Extent()));
  aTarget->BeginBulkRegistration();
  for (NCollection_DataMap<BRepGraph_ItemId, Entry>::Iterator anIt(myEntries); anIt.More();
       anIt.Next())
  {
    const BRepGraph_ItemId aTargetItem = theCopy.TargetItem(anIt.Key());
    if (!aTargetItem.IsValid())
    {
      continue;
    }

    const Entry&   anEntry = anIt.Value();
    Representation aRepresentations[Entry::RepresentationStorage::THE_MAX_REPRESENTATIONS_PER_ITEM];
    for (size_t aRepresentationIdx = 0; aRepresentationIdx < anEntry.Representations.Size();
         ++aRepresentationIdx)
    {
      aRepresentations[aRepresentationIdx] = anEntry.Representations.Value(aRepresentationIdx);
    }
    aTarget->RegisterDeferredRepresentationsDirect(aTargetItem,
                                                   anEntry.Provider,
                                                   anEntry.SourceKey,
                                                   aRepresentations,
                                                   anEntry.Representations.Size());
  }
  aTarget->EndBulkRegistration();
}

//=================================================================================================

void BRepGraph_LayerDeferred::OnRefRemoved(const BRepGraph_RefId theRef) noexcept
{
  removeItem(BRepGraph_ItemId(theRef));
}

//=================================================================================================

void BRepGraph_LayerDeferred::InvalidateAll() noexcept
{
  Clear();
}

//=================================================================================================

void BRepGraph_LayerDeferred::Clear() noexcept
{
  for (NCollection_DataMap<BRepGraph_ItemId, Entry>::Iterator anIt(myEntries); anIt.More();
       anIt.Next())
  {
    unlockItem(anIt.Key());
  }
  myEntries.Clear(true);
  touch();
}

//=================================================================================================

void BRepGraph_LayerDeferred::removeItem(const BRepGraph_ItemId theItem) noexcept
{
  if (myEntries.UnBind(theItem))
  {
    unlockItem(theItem);
    touch();
  }
}

//=================================================================================================

void BRepGraph_LayerDeferred::lockItem(const BRepGraph_ItemId theItem)
{
  if (BRepGraph* aGraph = AttachedGraph())
  {
    occ::handle<BRepGraph_LayerLock> aLockLayer =
      aGraph->LayerRegistry().Ensure<BRepGraph_LayerLock>();
    const bool isBulk    = myBulkRegistrationDepth != 0;
    const bool isChanged = aLockLayer->SetOwner(theItem, ID(), !isBulk);
    myHasBulkChanges     = myHasBulkChanges || (isBulk && isChanged);
  }
}

//=================================================================================================

void BRepGraph_LayerDeferred::unlockItem(const BRepGraph_ItemId theItem)
{
  if (BRepGraph* aGraph = AttachedGraph())
  {
    if (occ::handle<BRepGraph_LayerLock> aLockLayer =
          aGraph->LayerRegistry().FindLayer<BRepGraph_LayerLock>();
        !aLockLayer.IsNull())
    {
      aLockLayer->UnsetOwner(theItem, ID());
    }
  }
}
