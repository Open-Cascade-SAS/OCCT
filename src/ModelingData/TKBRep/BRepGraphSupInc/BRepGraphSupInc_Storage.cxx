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

#include <BRepGraphSupInc_Storage.hxx>

#include <BRepGraphSupInc_CopyContext.hxx>

#include <NCollection_FlatDataMap.hxx>
#include <Standard_Assert.hxx>

#include <functional>

namespace
{
void collectRegisteredStores(
  const BRepGraphSupInc_StoreRegistry&                          theRegistry,
  NCollection_LinearVector<occ::handle<BRepGraphSupInc_Store>>& theStores)
{
  theStores.Clear();
  theStores.Reserve(theRegistry.Count());
  for (BRepGraphSupInc_StoreRegistry::Iterator anIt(theRegistry); anIt.More(); anIt.Next())
  {
    theStores.Append(anIt.Value());
  }
}

bool orderedStores(const BRepGraphSupInc_StoreRegistry&                          theRegistry,
                   NCollection_LinearVector<occ::handle<BRepGraphSupInc_Store>>& theOrdered)
{
  NCollection_LinearVector<occ::handle<BRepGraphSupInc_Store>> aStores;
  collectRegisteredStores(theRegistry, aStores);
  NCollection_FlatDataMap<Standard_GUID, uint32_t> aStoreIndices;
  NCollection_LinearVector<uint8_t>                aStates;
  aStates.Reserve(aStores.Size());
  for (uint32_t anIndex = 0; anIndex < static_cast<uint32_t>(aStores.Size()); ++anIndex)
  {
    const occ::handle<BRepGraphSupInc_Store>& aStore = aStores.Value(static_cast<size_t>(anIndex));
    if (aStore.IsNull() || !aStoreIndices.Bind(aStore->ID(), anIndex))
    {
      return false;
    }
    aStates.Append(0);
  }

  std::function<bool(uint32_t)> visitStore = [&](const uint32_t theIndex) {
    const uint8_t aState = aStates.Value(static_cast<size_t>(theIndex));
    if (aState == 2)
    {
      return true;
    }
    if (aState == 1)
    {
      return false;
    }
    aStates.ChangeValue(static_cast<size_t>(theIndex)) = 1;
    const occ::handle<BRepGraphSupInc_Store>& aStore = aStores.Value(static_cast<size_t>(theIndex));
    for (NCollection_Sequence<Standard_GUID>::Iterator aDependencyIt(aStore->Dependencies());
         aDependencyIt.More();
         aDependencyIt.Next())
    {
      const uint32_t* aDependencyIndex = aStoreIndices.Seek(aDependencyIt.Value());
      if (aDependencyIndex == nullptr || !visitStore(*aDependencyIndex))
      {
        return false;
      }
    }
    aStates.ChangeValue(static_cast<size_t>(theIndex)) = 2;
    theOrdered.Append(aStore);
    return true;
  };

  theOrdered.Clear();
  theOrdered.Reserve(aStores.Size());
  for (uint32_t anIndex = 0; anIndex < static_cast<uint32_t>(aStores.Size()); ++anIndex)
  {
    if (!visitStore(anIndex))
    {
      theOrdered.Clear();
      return false;
    }
  }
  return true;
}

bool collectStoreDependencyClosure(const BRepGraphSupInc_StoreRegistry&          theRegistry,
                                   const occ::handle<BRepGraphSupInc_Store>&     theStore,
                                   NCollection_FlatDataMap<Standard_GUID, bool>& theSelected)
{
  if (theStore.IsNull())
  {
    return false;
  }
  if (theSelected.IsBound(theStore->ID()))
  {
    return true;
  }
  if (!theSelected.Bind(theStore->ID(), true))
  {
    return false;
  }

  for (NCollection_Sequence<Standard_GUID>::Iterator aDependencyIt(theStore->Dependencies());
       aDependencyIt.More();
       aDependencyIt.Next())
  {
    const occ::handle<BRepGraphSupInc_Store> aDependency =
      theRegistry.FindStore(aDependencyIt.Value());
    if (aDependency.IsNull()
        || !collectStoreDependencyClosure(theRegistry, aDependency, theSelected))
    {
      return false;
    }
  }
  return true;
}

} // namespace

//=================================================================================================

BRepGraphSupInc_DefinitionId BRepGraphSupInc_Storage::FindDefinitionByUID(
  const BRepGraphSupInc_ItemUID& theUID) const
{
  if (!theUID.IsValid())
  {
    return BRepGraphSupInc_DefinitionId();
  }

  const occ::handle<BRepGraphSupInc_Store> aStore = myRegistry.FindStore(theUID.StoreId);
  return aStore.IsNull() ? BRepGraphSupInc_DefinitionId() : aStore->FindDefinitionByUID(theUID);
}

//=================================================================================================

bool BRepGraphSupInc_Storage::Has(const BRepGraphSupInc_ItemUID& theUID) const
{
  return FindDefinitionByUID(theUID).IsValid();
}

//=================================================================================================

bool BRepGraphSupInc_Storage::Remove(const BRepGraphSupInc_ItemUID& theUID)
{
  if (!theUID.IsValid())
  {
    return false;
  }

  const occ::handle<BRepGraphSupInc_Store> aStore = myRegistry.FindStore(theUID.StoreId);
  return !aStore.IsNull() && aStore->Remove(theUID);
}

//=================================================================================================

BRepGraphSupInc_TopologyId BRepGraphSupInc_Storage::AddTopology(
  const BRepGraph_NodeId                        theOwner,
  const BRepGraphSupInc::TopologyAttachmentKind theKind,
  const TopoDS_Shape&                           theShape)
{
  const occ::handle<BRepGraphSupInc_TopologyStore> aStore =
    myRegistry.EnsureStore<BRepGraphSupInc_TopologyStore>();
  return aStore.IsNull() ? BRepGraphSupInc_TopologyId() : aStore->Add(theOwner, theKind, theShape);
}

//=================================================================================================

bool BRepGraphSupInc_Storage::RemoveTopology(const BRepGraphSupInc_TopologyId theID)
{
  const occ::handle<BRepGraphSupInc_TopologyStore> aStore =
    myRegistry.FindStore<BRepGraphSupInc_TopologyStore>();
  return !aStore.IsNull() && aStore->Remove(theID);
}

//=================================================================================================

void BRepGraphSupInc_Storage::RemoveTopologyOwner(const BRepGraph_NodeId theOwner)
{
  const occ::handle<BRepGraphSupInc_TopologyStore> aStore =
    myRegistry.FindStore<BRepGraphSupInc_TopologyStore>();
  if (!aStore.IsNull())
  {
    aStore->RemoveOwner(theOwner);
  }
}

//=================================================================================================

void BRepGraphSupInc_Storage::ReplaceTopologyOwner(const BRepGraph_NodeId theOldOwner,
                                                   const BRepGraph_NodeId theNewOwner)
{
  const occ::handle<BRepGraphSupInc_TopologyStore> aStore =
    myRegistry.FindStore<BRepGraphSupInc_TopologyStore>();
  if (!aStore.IsNull())
  {
    aStore->ReplaceOwner(theOldOwner, theNewOwner);
  }
}

//=================================================================================================

void BRepGraphSupInc_Storage::RemapCoreNodes(
  const NCollection_FlatDataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theNodeMap)
{
  NCollection_LinearVector<occ::handle<BRepGraphSupInc_Store>> aStores;
  collectRegisteredStores(myRegistry, aStores);
  for (const occ::handle<BRepGraphSupInc_Store>& aStore : aStores)
  {
    if (!aStore.IsNull())
    {
      aStore->RemapCoreNodes(theNodeMap);
    }
  }
}

//=================================================================================================

bool BRepGraphSupInc_Storage::CloneCompactedTo(
  BRepGraphSupInc_Storage&                                           theTarget,
  const NCollection_FlatDataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theNodeMap) const
{
  if (this == &theTarget || theTarget.Registry().Count() != 0)
  {
    return false;
  }

  BRepGraphSupInc_Storage                                      aCandidate;
  NCollection_LinearVector<occ::handle<BRepGraphSupInc_Store>> aSourceStores;
  if (!orderedStores(myRegistry, aSourceStores))
  {
    return false;
  }
  for (const occ::handle<BRepGraphSupInc_Store>& aSourceStore : aSourceStores)
  {
    if (aSourceStore.IsNull())
    {
      return false;
    }
    const occ::handle<BRepGraphSupInc_Store> aCompactedStore =
      aSourceStore->CloneForCompaction(theNodeMap);
    if (aCompactedStore.IsNull() || !aSourceStore->IsCompatible(*aCompactedStore)
        || !aCandidate.ChangeRegistry().registerCompactedStore(aCompactedStore,
                                                               aSourceStore->StoreId()))
    {
      return false;
    }
  }
  theTarget = std::move(aCandidate);
  return true;
}

//=================================================================================================

void BRepGraphSupInc_Storage::Clear() noexcept
{
  myRegistry.ClearAll();
}

//=================================================================================================

bool BRepGraphSupInc_Storage::NeedsCompaction() const
{
  NCollection_LinearVector<occ::handle<BRepGraphSupInc_Store>> aStores;
  collectRegisteredStores(myRegistry, aStores);
  for (const occ::handle<BRepGraphSupInc_Store>& aStore : aStores)
  {
    if (!aStore.IsNull() && aStore->NeedsCompaction())
    {
      return true;
    }
  }
  return false;
}

//=================================================================================================

bool BRepGraphSupInc_Storage::IsCopySupported(const BRepGraphSupInc_Storage& theTarget) const
{
  NCollection_LinearVector<occ::handle<BRepGraphSupInc_Store>> aSourceStores;
  if (!orderedStores(myRegistry, aSourceStores))
  {
    return false;
  }
  for (const occ::handle<BRepGraphSupInc_Store>& aStore : aSourceStores)
  {
    if (aStore.IsNull())
    {
      return false;
    }
    const occ::handle<BRepGraphSupInc_Store> aTargetStore =
      theTarget.Registry().FindStore(aStore->ID());
    if (!aTargetStore.IsNull() && !aStore->IsCompatible(*aTargetStore))
    {
      return false;
    }
  }
  return true;
}

//=================================================================================================

bool BRepGraphSupInc_Storage::CopyTo(BRepGraphSupInc_Storage&     theTarget,
                                     BRepGraphSupInc_CopyContext& theContext) const
{
  if (this == &theTarget || &theContext.Source() != this || &theContext.Target() != &theTarget)
  {
    return false;
  }

  if (!IsCopySupported(theTarget))
  {
    return false;
  }

  NCollection_LinearVector<occ::handle<BRepGraphSupInc_Store>> aSourceStores;
  if (!orderedStores(myRegistry, aSourceStores))
  {
    return false;
  }
  for (const occ::handle<BRepGraphSupInc_Store>& aSourceStore : aSourceStores)
  {
    const occ::handle<BRepGraphSupInc_Store> aTargetStore =
      theTarget.Registry().FindStore(aSourceStore->ID());
    if (!aTargetStore.IsNull())
    {
      if (!aSourceStore->IsCompatible(*aTargetStore))
      {
        return false;
      }
      continue;
    }

    const occ::handle<BRepGraphSupInc_Store> aNewStore = aSourceStore->NewEmpty();
    if (aNewStore.IsNull() || !aSourceStore->IsCompatible(*aNewStore))
    {
      return false;
    }
  }
  for (const occ::handle<BRepGraphSupInc_Store>& aSourceStore : aSourceStores)
  {
    if (!theTarget.Registry().FindStore(aSourceStore->ID()).IsNull())
    {
      continue;
    }
    const occ::handle<BRepGraphSupInc_Store> aTargetStore = aSourceStore->NewEmpty();
    if (aTargetStore.IsNull() || !theTarget.ChangeRegistry().RegisterStore(aTargetStore))
    {
      return false;
    }
  }
  for (const occ::handle<BRepGraphSupInc_Store>& aSourceStore : aSourceStores)
  {
    if (!aSourceStore->CopyTo(theContext))
    {
      return false;
    }
  }

  if (theContext.CopyMode() == BRepGraphSupInc_CopyContext::Mode::Compact)
  {
    theTarget.Compact();
  }
  return true;
}

//=================================================================================================

void BRepGraphSupInc_Storage::CopyRecordsForMappedNodesTo(
  BRepGraphSupInc_Storage&     theTarget,
  BRepGraphSupInc_CopyContext& theContext) const
{
  NCollection_LinearVector<occ::handle<BRepGraphSupInc_Store>> aSourceStores;
  Standard_ASSERT_RAISE(orderedStores(myRegistry, aSourceStores),
                        "BRepGraphSupInc_Storage: node-copy preflight was not completed");

  NCollection_FlatDataMap<Standard_GUID, bool> aSelectedStores;
  for (const occ::handle<BRepGraphSupInc_Store>& aSourceStore : aSourceStores)
  {
    Standard_ASSERT_RAISE(
      !aSourceStore->HasRecordsForMappedNodes(theContext)
        || collectStoreDependencyClosure(myRegistry, aSourceStore, aSelectedStores),
      "BRepGraphSupInc_Storage: node-copy preflight was not completed");
  }
  for (const occ::handle<BRepGraphSupInc_Store>& aSourceStore : aSourceStores)
  {
    if (!aSelectedStores.IsBound(aSourceStore->ID()))
    {
      continue;
    }
    if (!theTarget.Registry().FindStore(aSourceStore->ID()).IsNull())
    {
      continue;
    }
    occ::handle<BRepGraphSupInc_Store> aTargetStore =
      theContext.prevalidatedTargetStore(aSourceStore->ID());
    Standard_ASSERT_RAISE(!aTargetStore.IsNull() && aSourceStore->IsCompatible(*aTargetStore)
                            && theTarget.ChangeRegistry().RegisterStore(aTargetStore),
                          "BRepGraphSupInc_Storage: node-copy preflight was not completed");
  }
  for (const occ::handle<BRepGraphSupInc_Store>& aSourceStore : aSourceStores)
  {
    if (aSourceStore->HasRecordsForMappedNodes(theContext))
    {
      aSourceStore->CopyRecordsForMappedNodes(theContext);
    }
  }
}

//=================================================================================================

bool BRepGraphSupInc_Storage::PreflightRecordsForMappedNodesTo(
  const BRepGraphSupInc_Storage& theTarget,
  BRepGraphSupInc_CopyContext&   theContext) const
{
  if (&theContext.Source() != this || &theContext.Target() != &theTarget)
  {
    return false;
  }

  NCollection_LinearVector<occ::handle<BRepGraphSupInc_Store>> aSourceStores;
  if (!orderedStores(myRegistry, aSourceStores))
  {
    return false;
  }
  for (const occ::handle<BRepGraphSupInc_Store>& aSourceStore : aSourceStores)
  {
    const occ::handle<BRepGraphSupInc_Store> aTargetStore =
      theTarget.Registry().FindStore(aSourceStore->ID());
    if (!aTargetStore.IsNull() && !aSourceStore->IsCompatible(*aTargetStore))
    {
      return false;
    }
    if (aTargetStore.IsNull())
    {
      const occ::handle<BRepGraphSupInc_Store> aPrevalidatedStore =
        theContext.prevalidatedTargetStore(aSourceStore->ID());
      if (!aPrevalidatedStore.IsNull())
      {
        if (!aSourceStore->IsCompatible(*aPrevalidatedStore))
        {
          return false;
        }
      }
      else
      {
        const occ::handle<BRepGraphSupInc_Store> aNewStore = aSourceStore->NewEmpty();
        if (aNewStore.IsNull() || !aSourceStore->IsCompatible(*aNewStore)
            || !theContext.addPrevalidatedTargetStore(aSourceStore->ID(), aNewStore))
        {
          return false;
        }
      }
    }
    if (aSourceStore->HasRecordsForMappedNodes(theContext)
        && !aSourceStore->CanCopyRecordsForMappedNodes(theContext))
    {
      return false;
    }
  }
  return true;
}

//=================================================================================================

void BRepGraphSupInc_Storage::Compact()
{
  NCollection_LinearVector<occ::handle<BRepGraphSupInc_Store>> aStores;
  collectRegisteredStores(myRegistry, aStores);
  for (const occ::handle<BRepGraphSupInc_Store>& aStore : aStores)
  {
    if (!aStore.IsNull())
    {
      aStore->Compact();
    }
  }
}
