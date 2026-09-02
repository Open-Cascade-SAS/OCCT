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

#include <BRepGraph_LayerSupplement.hxx>

#include <BRepGraph_LayerSupplementRegistry.hxx>
#include <BRepGraph_SupplementsView.hxx>
#include <BRepGraphSupInc_CopyContext.hxx>
#include <Standard_GUID.hxx>

#include <limits>

IMPLEMENT_STANDARD_RTTIEXT(BRepGraph_LayerSupplement, BRepGraph_LayerSupplementBase)

//=================================================================================================

const Standard_GUID& BRepGraph_LayerSupplement::GetID()
{
  static const Standard_GUID THE_ID("870744dc-5845-4c7c-96ee-a042e0ced75a");
  return THE_ID;
}

//=================================================================================================

const Standard_GUID& BRepGraph_LayerSupplement::ID() const
{
  return GetID();
}

//=================================================================================================

const TCollection_AsciiString& BRepGraph_LayerSupplement::Name() const
{
  static const TCollection_AsciiString THE_NAME("BRepGraph_LayerSupplement");
  return THE_NAME;
}

//=================================================================================================

uint32_t BRepGraph_LayerSupplement::Add(const BRepGraphSupInc_Endpoint& theSource,
                                        const BRepGraphSupInc_Endpoint& theTarget,
                                        const Standard_GUID&            theRole)
{
  if (!isValidEndpoint(theSource) || !isValidEndpoint(theTarget) || theRole == Standard_GUID()
      || myNextUID == 0)
  {
    return 0;
  }

  const uint32_t aUID = myNextUID++;
  Reference      aReference;
  aReference.Source = theSource;
  aReference.Target = theTarget;
  aReference.Role   = theRole;
  myReferences.Bind(aUID, aReference);
  addIndex(theSource, aUID);
  if (!(theTarget == theSource))
  {
    addIndex(theTarget, aUID);
  }
  touch();
  return aUID;
}

//=================================================================================================

const BRepGraph_LayerSupplement::Reference* BRepGraph_LayerSupplement::Find(const uint32_t theUID) const
{
  return theUID == 0 ? nullptr : myReferences.Seek(theUID);
}

//=================================================================================================

const NCollection_LinearVector<uint32_t>& BRepGraph_LayerSupplement::Related(
  const BRepGraphSupInc_Endpoint& theEndpoint) const
{
  const NCollection_LinearVector<uint32_t>* aUIDs = myEndpointToUIDs.Seek(theEndpoint);
  return aUIDs != nullptr ? *aUIDs : myEmptyUIDs;
}

//=================================================================================================

bool BRepGraph_LayerSupplement::Remove(const uint32_t theUID)
{
  const Reference* aReference = Find(theUID);
  if (aReference == nullptr)
  {
    return false;
  }

  const BRepGraphSupInc_Endpoint aSource = aReference->Source;
  const BRepGraphSupInc_Endpoint aTarget = aReference->Target;
  removeIndex(aSource, theUID);
  if (!(aTarget == aSource))
  {
    removeIndex(aTarget, theUID);
  }
  myReferences.UnBind(theUID);
  touch();
  return true;
}

//=================================================================================================

void BRepGraph_LayerSupplement::OnItemRemoved(const BRepGraph_ItemId theItem) noexcept
{
  removeByEndpoint(BRepGraphSupInc_Endpoint::Core(theItem));
}

//=================================================================================================

void BRepGraph_LayerSupplement::OnNodeReplaced(const BRepGraph_NodeId theOldNode,
                                                const BRepGraph_NodeId theNewNode) noexcept
{
  if (!theOldNode.IsValid() || !theNewNode.IsValid() || theOldNode.NodeKind != theNewNode.NodeKind)
  {
    return;
  }

  const BRepGraphSupInc_Endpoint anOldEndpoint =
    BRepGraphSupInc_Endpoint::Core(BRepGraph_ItemId(theOldNode));
  const BRepGraphSupInc_Endpoint aNewEndpoint =
    BRepGraphSupInc_Endpoint::Core(BRepGraph_ItemId(theNewNode));
  const NCollection_LinearVector<uint32_t> aUIDs = Related(anOldEndpoint);
  bool                                      isChanged = false;
  for (const uint32_t aUID : aUIDs)
  {
    Reference* aReference = myReferences.ChangeSeek(aUID);
    if (aReference == nullptr)
    {
      continue;
    }
    const BRepGraphSupInc_Endpoint aSource = aReference->Source;
    const BRepGraphSupInc_Endpoint aTarget = aReference->Target;
    if (aSource == anOldEndpoint || aTarget == anOldEndpoint)
    {
      removeIndex(aSource, aUID);
      if (!(aTarget == aSource))
      {
        removeIndex(aTarget, aUID);
      }
      if (aReference->Source == anOldEndpoint)
      {
        aReference->Source = aNewEndpoint;
      }
      if (aReference->Target == anOldEndpoint)
      {
        aReference->Target = aNewEndpoint;
      }
      addIndex(aReference->Source, aUID);
      if (!(aReference->Target == aReference->Source))
      {
        addIndex(aReference->Target, aUID);
      }
      isChanged = true;
    }
  }
  if (isChanged)
  {
    touch();
  }
}

//=================================================================================================

void BRepGraph_LayerSupplement::OnSupplementRemoved(const BRepGraphSupInc_ItemUID& theUID) noexcept
{
  removeByEndpoint(BRepGraphSupInc_Endpoint::Supplemental(theUID));
}

//=================================================================================================

void BRepGraph_LayerSupplement::CopySupplementalTo(
  const BRepGraph_CopyRemap&          theCopy,
  const BRepGraphSupInc_CopyContext& theSupplementCopy) const
{
  occ::handle<BRepGraph_LayerSupplement> aTarget;
  for (NCollection_FlatDataMap<uint32_t, Reference>::Iterator anIt(myReferences); anIt.More(); anIt.Next())
  {
    const Reference& aReference = anIt.Value();
    const BRepGraphSupInc_Endpoint aSource =
      remapEndpoint(aReference.Source, theCopy, theSupplementCopy);
    const BRepGraphSupInc_Endpoint aTargetEndpoint =
      remapEndpoint(aReference.Target, theCopy, theSupplementCopy);
    if (!aSource.IsValid() || !aTargetEndpoint.IsValid())
    {
      continue;
    }
    if (aTarget.IsNull())
    {
      aTarget = theCopy.TargetGraph().LayerSupplementRegistry().Ensure<BRepGraph_LayerSupplement>();
    }
    (void)aTarget->Add(aSource, aTargetEndpoint, aReference.Role);
  }
}

//=================================================================================================

void BRepGraph_LayerSupplement::InvalidateAll() noexcept
{
  touch();
}

//=================================================================================================

void BRepGraph_LayerSupplement::Clear() noexcept
{
  if (myReferences.IsEmpty())
  {
    return;
  }
  myReferences.Clear();
  myEndpointToUIDs.Clear();
  myNextUID = 1;
  touch();
}

//=================================================================================================

bool BRepGraph_LayerSupplement::isValidEndpoint(const BRepGraphSupInc_Endpoint& theEndpoint) const
{
  if (!theEndpoint.IsValid())
  {
    return false;
  }
  switch (theEndpoint.ItemDomain())
  {
    case BRepGraphSupInc_Endpoint::Domain::CoreItem:
    {
      const BRepGraph_ItemId* aCoreItem = theEndpoint.CoreItem();
      return aCoreItem != nullptr
             && (aCoreItem->IsNode() ? !aCoreItem->NodeId().IsRemoved(Graph())
                                     : !aCoreItem->RefId().IsRemoved(Graph()));
    }
    case BRepGraphSupInc_Endpoint::Domain::SupplementalItem:
    {
      const BRepGraphSupInc_ItemUID* aSupplementalItem = theEndpoint.SupplementalItem();
      return aSupplementalItem != nullptr && Graph().Supplements().Has(*aSupplementalItem);
    }
    case BRepGraphSupInc_Endpoint::Domain::None:
      return false;
  }
  return false;
}

//=================================================================================================

void BRepGraph_LayerSupplement::addIndex(const BRepGraphSupInc_Endpoint& theEndpoint,
                                         const uint32_t                   theUID)
{
  NCollection_LinearVector<uint32_t>* aUIDs = myEndpointToUIDs.ChangeSeek(theEndpoint);
  if (aUIDs == nullptr)
  {
    NCollection_LinearVector<uint32_t> anEmpty;
    myEndpointToUIDs.Bind(theEndpoint, anEmpty);
    aUIDs = myEndpointToUIDs.ChangeSeek(theEndpoint);
  }
  aUIDs->Append(theUID);
}

//=================================================================================================

void BRepGraph_LayerSupplement::removeIndex(const BRepGraphSupInc_Endpoint& theEndpoint,
                                            const uint32_t                   theUID) noexcept
{
  NCollection_LinearVector<uint32_t>* aUIDs = myEndpointToUIDs.ChangeSeek(theEndpoint);
  if (aUIDs == nullptr)
  {
    return;
  }
  for (size_t anIndex = 0; anIndex < aUIDs->Size(); ++anIndex)
  {
    if (aUIDs->Value(anIndex) == theUID)
    {
      aUIDs->Erase(anIndex);
      break;
    }
  }
  if (aUIDs->IsEmpty())
  {
    myEndpointToUIDs.UnBind(theEndpoint);
  }
}

//=================================================================================================

void BRepGraph_LayerSupplement::removeByEndpoint(
  const BRepGraphSupInc_Endpoint& theEndpoint) noexcept
{
  if (!theEndpoint.IsValid())
  {
    return;
  }
  const NCollection_LinearVector<uint32_t> aUIDs = Related(theEndpoint);
  bool                                      isChanged = false;
  for (const uint32_t aUID : aUIDs)
  {
    const Reference* aReference = Find(aUID);
    if (aReference == nullptr)
    {
      continue;
    }
    const BRepGraphSupInc_Endpoint aSource = aReference->Source;
    const BRepGraphSupInc_Endpoint aTarget = aReference->Target;
    removeIndex(aSource, aUID);
    if (!(aTarget == aSource))
    {
      removeIndex(aTarget, aUID);
    }
    myReferences.UnBind(aUID);
    isChanged = true;
  }
  if (isChanged)
  {
    touch();
  }
}

//=================================================================================================

BRepGraphSupInc_Endpoint BRepGraph_LayerSupplement::remapEndpoint(
  const BRepGraphSupInc_Endpoint& theEndpoint,
  const BRepGraph_CopyRemap&      theCopy,
  const BRepGraphSupInc_CopyContext& theSupplementCopy)
{
  if (theEndpoint.ItemDomain() == BRepGraphSupInc_Endpoint::Domain::CoreItem)
  {
    const BRepGraph_ItemId* aCoreItem = theEndpoint.CoreItem();
    return aCoreItem != nullptr ? BRepGraphSupInc_Endpoint::Core(theCopy.TargetItemOrInvalid(*aCoreItem))
                                : BRepGraphSupInc_Endpoint();
  }
  if (theEndpoint.ItemDomain() == BRepGraphSupInc_Endpoint::Domain::SupplementalItem)
  {
    const BRepGraphSupInc_ItemUID* aSupplementalItem = theEndpoint.SupplementalItem();
    if (aSupplementalItem == nullptr)
    {
      return BRepGraphSupInc_Endpoint();
    }
    BRepGraphSupInc_ItemUID aTarget = theSupplementCopy.TargetUID(*aSupplementalItem);
    if (!aTarget.IsValid() && theSupplementCopy.CopyMode() == BRepGraphSupInc_CopyContext::Mode::Compact
        && theCopy.TargetGraph().Supplements().Has(*aSupplementalItem))
    {
      aTarget = *aSupplementalItem;
    }
    return BRepGraphSupInc_Endpoint::Supplemental(aTarget);
  }
  return BRepGraphSupInc_Endpoint();
}
