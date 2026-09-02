// Copyright (c) 2026 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.

#include <BRepGraphSupInc_TopologyStore.hxx>

#include <BRepGraphSupInc_CopyContext.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_ProgramError.hxx>

#include <limits>

IMPLEMENT_STANDARD_RTTIEXT(BRepGraphSupInc_TopologyStore, BRepGraphSupInc_Store)

//=================================================================================================

const Standard_GUID& BRepGraphSupInc_TopologyStore::GetID()
{
  static const Standard_GUID THE_ID("f0b5cb34-657b-4fdd-a6df-b70c6dc88c6a");
  return THE_ID;
}

//=================================================================================================

const TCollection_AsciiString& BRepGraphSupInc_TopologyStore::Name() const
{
  static const TCollection_AsciiString THE_NAME("BRepGraphSupInc_TopologyStore");
  return THE_NAME;
}

//=================================================================================================

BRepGraphSupInc_DefinitionId BRepGraphSupInc_TopologyStore::FindDefinitionByUID(
  const BRepGraphSupInc_ItemUID& theUID) const
{
  const BRepGraphSupInc_TopologyId anID = FindByUID(theUID);
  return anID.IsValid() ? BRepGraphSupInc_DefinitionId{THE_TOPOLOGY_KIND, anID.Index}
                        : BRepGraphSupInc_DefinitionId();
}

//=================================================================================================

bool BRepGraphSupInc_TopologyStore::Remove(const BRepGraphSupInc_ItemUID& theUID)
{
  return Remove(FindByUID(theUID));
}

//=================================================================================================

void BRepGraphSupInc_TopologyStore::Clear() noexcept
{
  myItems.Clear();
  myOwnerToItems.Clear();
  myUIDToID.Clear();
  myActiveCount = 0;
}

//=================================================================================================

void BRepGraphSupInc_TopologyStore::Compact()
{
  NCollection_LinearVector<BRepGraphSupInc::TopologyDef> anItems;
  anItems.Reserve(myActiveCount);
  for (const BRepGraphSupInc::TopologyDef& anItem : myItems)
  {
    if (!anItem.IsRemoved)
    {
      anItems.Append(anItem);
    }
  }
  myItems = std::move(anItems);
  rebuildIndices();
}

//=================================================================================================

occ::handle<BRepGraphSupInc_Store> BRepGraphSupInc_TopologyStore::CloneForCompaction(
  const NCollection_FlatDataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theNodeMap) const
{
  occ::handle<BRepGraphSupInc_TopologyStore> aResult = new BRepGraphSupInc_TopologyStore();
  for (const BRepGraphSupInc::TopologyDef& anItem : myItems)
  {
    if (anItem.IsRemoved)
    {
      continue;
    }
    const BRepGraph_NodeId* aMappedOwner = theNodeMap.Seek(anItem.Owner);
    if (aMappedOwner != nullptr && aMappedOwner->IsValid())
    {
      (void)aResult->append(*aMappedOwner, anItem.Kind, anItem.Shape, anItem.OwnGen, anItem.UID);
    }
  }
  aResult->myNextCounter = myNextCounter;
  return aResult;
}

//=================================================================================================

bool BRepGraphSupInc_TopologyStore::IsCompatible(const BRepGraphSupInc_Store& theTarget) const
{
  return theTarget.ID() == ID();
}

//=================================================================================================

occ::handle<BRepGraphSupInc_Store> BRepGraphSupInc_TopologyStore::NewEmpty() const
{
  return new BRepGraphSupInc_TopologyStore();
}

//=================================================================================================

bool BRepGraphSupInc_TopologyStore::CopyTo(BRepGraphSupInc_CopyContext& theContext) const
{
  const occ::handle<BRepGraphSupInc_Store> aTargetStore = theContext.TargetStore(ID());
  if (aTargetStore.IsNull() || !IsCompatible(*aTargetStore))
  {
    return false;
  }
  BRepGraphSupInc_TopologyStore& aTarget =
    static_cast<BRepGraphSupInc_TopologyStore&>(*aTargetStore);
  for (const BRepGraphSupInc::TopologyDef& anItem : myItems)
  {
    if (anItem.IsRemoved)
    {
      continue;
    }
    const BRepGraphSupInc_ItemUID aSourceUID = itemUID(THE_TOPOLOGY_KIND, anItem.UID);
    if (theContext.HasTargetUID(aSourceUID))
    {
      continue;
    }
    const BRepGraph_NodeId aTargetOwner = theContext.TargetNode(anItem.Owner);
    if (!aTargetOwner.IsValid())
    {
      return false;
    }
    const BRepGraphSupInc_TopologyId aTargetID =
      aTarget.append(aTargetOwner, anItem.Kind, anItem.Shape, anItem.OwnGen);
    if (!theContext.AddMapping(aSourceUID, aTarget.ItemUID(aTargetID)))
    {
      return false;
    }
  }
  return true;
}

//=================================================================================================

bool BRepGraphSupInc_TopologyStore::HasRecordsForMappedNodes(
  const BRepGraphSupInc_CopyContext& theContext) const
{
  for (const BRepGraphSupInc::TopologyDef& anItem : myItems)
  {
    if (!anItem.IsRemoved && theContext.HasTargetNode(anItem.Owner))
    {
      return true;
    }
  }
  return false;
}

//=================================================================================================

void BRepGraphSupInc_TopologyStore::CopyRecordsForMappedNodes(
  BRepGraphSupInc_CopyContext& theContext) const
{
  const occ::handle<BRepGraphSupInc_Store> aTargetStore = theContext.TargetStore(ID());
  Standard_ASSERT_RAISE(!aTargetStore.IsNull() && IsCompatible(*aTargetStore),
                        "BRepGraphSupInc_TopologyStore: node-copy preflight was not completed");
  BRepGraphSupInc_TopologyStore& aTarget =
    static_cast<BRepGraphSupInc_TopologyStore&>(*aTargetStore);
  const NCollection_LinearVector<BRepGraphSupInc::TopologyDef> anItems = myItems;
  for (const BRepGraphSupInc::TopologyDef& anItem : anItems)
  {
    if (anItem.IsRemoved || !theContext.HasTargetNode(anItem.Owner))
    {
      continue;
    }
    const BRepGraph_NodeId aTargetOwner = theContext.TargetNode(anItem.Owner);
    Standard_ASSERT_RAISE(aTargetOwner.IsValid(),
                          "BRepGraphSupInc_TopologyStore: mapped owner is invalid");
    const BRepGraphSupInc_ItemUID aSourceUID = itemUID(THE_TOPOLOGY_KIND, anItem.UID);
    if (theContext.HasTargetUID(aSourceUID))
    {
      continue;
    }
    const BRepGraphSupInc_TopologyId aTargetID =
      aTarget.append(aTargetOwner, anItem.Kind, anItem.Shape, anItem.OwnGen);
    Standard_ASSERT_RAISE(theContext.AddMapping(aSourceUID, aTarget.ItemUID(aTargetID)),
                          "BRepGraphSupInc_TopologyStore: duplicate node-copy item mapping");
  }
}

//=================================================================================================

BRepGraphSupInc_TopologyId BRepGraphSupInc_TopologyStore::Add(
  const BRepGraph_NodeId                        theOwner,
  const BRepGraphSupInc::TopologyAttachmentKind theKind,
  const TopoDS_Shape&                           theShape)
{
  if (!theOwner.IsValid() || !isCompatible(theOwner.NodeKind, theKind) || theShape.IsNull())
  {
    return BRepGraphSupInc_TopologyId();
  }
  return append(theOwner, theKind, theShape, 0);
}

//=================================================================================================

const BRepGraphSupInc::TopologyDef* BRepGraphSupInc_TopologyStore::Find(
  const BRepGraphSupInc_TopologyId theID) const
{
  if (!theID.IsValid() || static_cast<size_t>(theID.Index) >= myItems.Size())
  {
    return nullptr;
  }
  const BRepGraphSupInc::TopologyDef& anItem = myItems.Value(static_cast<size_t>(theID.Index));
  return anItem.IsRemoved ? nullptr : &anItem;
}

//=================================================================================================

BRepGraphSupInc_TopologyId BRepGraphSupInc_TopologyStore::FindByUID(
  const BRepGraphSupInc_ItemUID& theUID) const
{
  if (theUID.StoreId != StoreId() || theUID.Kind != THE_TOPOLOGY_KIND || theUID.Counter == 0)
  {
    return BRepGraphSupInc_TopologyId();
  }
  const BRepGraphSupInc_TopologyId* anID = myUIDToID.Seek(theUID.Counter);
  return anID == nullptr ? BRepGraphSupInc_TopologyId() : *anID;
}

//=================================================================================================

BRepGraphSupInc_ItemUID BRepGraphSupInc_TopologyStore::ItemUID(
  const BRepGraphSupInc_TopologyId theID) const
{
  const BRepGraphSupInc::TopologyDef* anItem = Find(theID);
  return anItem == nullptr ? BRepGraphSupInc_ItemUID() : itemUID(THE_TOPOLOGY_KIND, anItem->UID);
}

//=================================================================================================

const NCollection_LinearVector<BRepGraphSupInc_TopologyId>& BRepGraphSupInc_TopologyStore::
  AttachedTo(const BRepGraph_NodeId theOwner) const
{
  const NCollection_LinearVector<BRepGraphSupInc_TopologyId>* anItems =
    myOwnerToItems.Seek(theOwner);
  return anItems == nullptr ? myEmptyItems : *anItems;
}

//=================================================================================================

bool BRepGraphSupInc_TopologyStore::Remove(const BRepGraphSupInc_TopologyId theID)
{
  const BRepGraphSupInc::TopologyDef* anItem = Find(theID);
  if (anItem == nullptr)
  {
    return false;
  }
  NCollection_LinearVector<BRepGraphSupInc_TopologyId>* anItems =
    myOwnerToItems.ChangeSeek(anItem->Owner);
  if (anItems != nullptr)
  {
    for (size_t anIndex = 0; anIndex < anItems->Size(); ++anIndex)
    {
      if (anItems->Value(anIndex) == theID)
      {
        anItems->Erase(anIndex);
        break;
      }
    }
    if (anItems->IsEmpty())
    {
      myOwnerToItems.UnBind(anItem->Owner);
    }
  }
  BRepGraphSupInc::TopologyDef& aChanged = myItems.ChangeValue(static_cast<size_t>(theID.Index));
  myUIDToID.UnBind(aChanged.UID);
  aChanged.IsRemoved = true;
  --myActiveCount;
  return true;
}

//=================================================================================================

void BRepGraphSupInc_TopologyStore::RemoveOwner(const BRepGraph_NodeId theOwner)
{
  const NCollection_LinearVector<BRepGraphSupInc_TopologyId>* anItems =
    myOwnerToItems.Seek(theOwner);
  if (anItems == nullptr)
  {
    return;
  }
  const NCollection_LinearVector<BRepGraphSupInc_TopologyId> anOwnerItems = *anItems;
  for (const BRepGraphSupInc_TopologyId anID : anOwnerItems)
  {
    Remove(anID);
  }
}

//=================================================================================================

void BRepGraphSupInc_TopologyStore::ReplaceOwner(const BRepGraph_NodeId theOldOwner,
                                                 const BRepGraph_NodeId theNewOwner)
{
  if (theOldOwner == theNewOwner)
  {
    return;
  }
  if (!theOldOwner.IsValid() || !theNewOwner.IsValid()
      || theOldOwner.NodeKind != theNewOwner.NodeKind)
  {
    RemoveOwner(theOldOwner);
    return;
  }
  const NCollection_LinearVector<BRepGraphSupInc_TopologyId>* anItems =
    myOwnerToItems.Seek(theOldOwner);
  if (anItems == nullptr)
  {
    return;
  }
  const NCollection_LinearVector<BRepGraphSupInc_TopologyId> anOwnerItems = *anItems;
  NCollection_LinearVector<BRepGraphSupInc_TopologyId>*      aNewItems =
    myOwnerToItems.ChangeSeek(theNewOwner);
  if (aNewItems == nullptr)
  {
    myOwnerToItems.Bind(theNewOwner, anOwnerItems);
  }
  else
  {
    for (const BRepGraphSupInc_TopologyId anID : anOwnerItems)
    {
      aNewItems->Append(anID);
    }
  }
  for (const BRepGraphSupInc_TopologyId anID : anOwnerItems)
  {
    myItems.ChangeValue(static_cast<size_t>(anID.Index)).Owner = theNewOwner;
  }
  myOwnerToItems.UnBind(theOldOwner);
}

//=================================================================================================

void BRepGraphSupInc_TopologyStore::RemapCoreNodes(
  const NCollection_FlatDataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theNodeMap)
{
  for (BRepGraphSupInc::TopologyDef& anItem : myItems)
  {
    if (anItem.IsRemoved)
    {
      continue;
    }
    const BRepGraph_NodeId* aNewOwner = theNodeMap.Seek(anItem.Owner);
    if (aNewOwner == nullptr)
    {
      anItem.IsRemoved = true;
      --myActiveCount;
    }
    else
    {
      anItem.Owner = *aNewOwner;
    }
  }
  rebuildIndices();
}

//=================================================================================================

void BRepGraphSupInc_TopologyStore::Validate() const
{
  for (const BRepGraphSupInc::TopologyDef& anItem : myItems)
  {
    if (anItem.IsRemoved)
    {
      continue;
    }
    if (anItem.UID == 0 || !anItem.Owner.IsValid()
        || !isCompatible(anItem.Owner.NodeKind, anItem.Kind) || anItem.Shape.IsNull())
    {
      throw Standard_ProgramError("BRepGraphSupInc_TopologyStore::Validate - invalid entry");
    }
  }
}

//=================================================================================================

bool BRepGraphSupInc_TopologyStore::isSupportedOwner(const BRepGraph_NodeId::Kind theKind)
{
  return theKind == BRepGraph_NodeId::Kind::Vertex || theKind == BRepGraph_NodeId::Kind::Edge
         || theKind == BRepGraph_NodeId::Kind::Face || theKind == BRepGraph_NodeId::Kind::Shell
         || theKind == BRepGraph_NodeId::Kind::Solid || theKind == BRepGraph_NodeId::Kind::CompSolid
         || theKind == BRepGraph_NodeId::Kind::Compound;
}

//=================================================================================================

bool BRepGraphSupInc_TopologyStore::isCompatible(
  const BRepGraph_NodeId::Kind                  theOwnerKind,
  const BRepGraphSupInc::TopologyAttachmentKind theKind)
{
  using Kind = BRepGraphSupInc::TopologyAttachmentKind;
  switch (theKind)
  {
    case Kind::GenericSupplementShape:
      return isSupportedOwner(theOwnerKind);
    case Kind::VertexSupplementShape:
      return theOwnerKind == BRepGraph_NodeId::Kind::Vertex;
    case Kind::EdgeInternalVertex:
      return theOwnerKind == BRepGraph_NodeId::Kind::Edge;
    case Kind::FaceDirectVertex:
      return theOwnerKind == BRepGraph_NodeId::Kind::Face;
    case Kind::SolidAuxShape:
      return theOwnerKind == BRepGraph_NodeId::Kind::Solid;
    case Kind::ShellAuxShape:
      return theOwnerKind == BRepGraph_NodeId::Kind::Shell;
    case Kind::CompSolidAuxShape:
      return theOwnerKind == BRepGraph_NodeId::Kind::CompSolid;
    case Kind::CompoundAuxShape:
      return theOwnerKind == BRepGraph_NodeId::Kind::Compound;
  }
  return false;
}

//=================================================================================================

uint32_t BRepGraphSupInc_TopologyStore::allocateCounter()
{
  Standard_OutOfRange_Raise_if(myNextCounter == 0,
                               "BRepGraphSupInc_TopologyStore - UID counter overflow");
  return myNextCounter++;
}

//=================================================================================================

BRepGraphSupInc_TopologyId BRepGraphSupInc_TopologyStore::append(
  const BRepGraph_NodeId                        theOwner,
  const BRepGraphSupInc::TopologyAttachmentKind theKind,
  const TopoDS_Shape&                           theShape,
  const uint32_t                                theOwnGen,
  const uint32_t                                theUID)
{
  Standard_OutOfRange_Raise_if(myItems.Size() > std::numeric_limits<uint32_t>::max(),
                               "BRepGraphSupInc_TopologyStore - too many entries");
  const BRepGraphSupInc_TopologyId anID{static_cast<uint32_t>(myItems.Size())};
  BRepGraphSupInc::TopologyDef     anItem;
  anItem.UID    = theUID == 0 ? allocateCounter() : theUID;
  anItem.OwnGen = theOwnGen;
  anItem.Owner  = theOwner;
  anItem.Shape  = theShape;
  anItem.Kind   = theKind;
  myItems.Append(anItem);
  NCollection_LinearVector<BRepGraphSupInc_TopologyId>* anItems =
    myOwnerToItems.ChangeSeek(theOwner);
  if (anItems == nullptr)
  {
    NCollection_LinearVector<BRepGraphSupInc_TopologyId> aNewItems;
    aNewItems.Append(anID);
    myOwnerToItems.Bind(theOwner, aNewItems);
  }
  else
  {
    anItems->Append(anID);
  }
  myUIDToID.Bind(anItem.UID, anID);
  ++myActiveCount;
  return anID;
}

//=================================================================================================

void BRepGraphSupInc_TopologyStore::rebuildIndices()
{
  myOwnerToItems.Clear();
  myUIDToID.Clear();
  for (uint32_t anIndex = 0; anIndex < static_cast<uint32_t>(myItems.Size()); ++anIndex)
  {
    const BRepGraphSupInc::TopologyDef& anItem = myItems.Value(static_cast<size_t>(anIndex));
    if (anItem.IsRemoved)
    {
      continue;
    }
    const BRepGraphSupInc_TopologyId                      anID{anIndex};
    NCollection_LinearVector<BRepGraphSupInc_TopologyId>* anItems =
      myOwnerToItems.ChangeSeek(anItem.Owner);
    if (anItems == nullptr)
    {
      NCollection_LinearVector<BRepGraphSupInc_TopologyId> aNewItems;
      aNewItems.Append(anID);
      myOwnerToItems.Bind(anItem.Owner, aNewItems);
    }
    else
    {
      anItems->Append(anID);
    }
    myUIDToID.Bind(anItem.UID, anID);
  }
}
