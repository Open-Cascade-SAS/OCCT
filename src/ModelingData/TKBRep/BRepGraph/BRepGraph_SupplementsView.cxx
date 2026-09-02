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

#include <BRepGraph_SupplementsView.hxx>

#include <BRepGraph_Data.hxx>
#include <BRepGraph_LayerRegistry.hxx>
#include <BRepGraph_LayerSupplementRegistry.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraphSupInc_TopologyStore.hxx>
#include <Standard_ProgramError.hxx>

//=================================================================================================

bool BRepGraph::SupplementsView::RegisterStore(const occ::handle<BRepGraphSupInc_Store>& theStore)
{
  return storage().ChangeRegistry().RegisterStore(theStore);
}

//=================================================================================================

occ::handle<BRepGraphSupInc_Store> BRepGraph::SupplementsView::FindStore(
  const Standard_GUID& theID) const
{
  return storage().Registry().FindStore(theID);
}

//=================================================================================================

bool BRepGraph::SupplementsView::UnregisterStore(const Standard_GUID& theID)
{
  return storage().ChangeRegistry().UnregisterStore(theID);
}

//=================================================================================================

BRepGraphSupInc_DefinitionId BRepGraph::SupplementsView::FindDefinitionByUID(
  const BRepGraphSupInc_ItemUID& theUID) const
{
  return storage().FindDefinitionByUID(theUID);
}

//=================================================================================================

bool BRepGraph::SupplementsView::Has(const BRepGraphSupInc_ItemUID& theUID) const
{
  return FindDefinitionByUID(theUID).IsValid();
}

//=================================================================================================

bool BRepGraph::SupplementsView::Remove(const BRepGraphSupInc_ItemUID& theUID)
{
  const occ::handle<BRepGraphSupInc_Store> aStore = storage().Registry().FindStore(theUID.StoreId);
  if (!aStore.IsNull() && aStore->ID() == BRepGraphSupInc_TopologyStore::GetID())
  {
    const occ::handle<BRepGraphSupInc_TopologyStore> aTopology =
      occ::down_cast<BRepGraphSupInc_TopologyStore>(FindStore(BRepGraphSupInc_TopologyStore::GetID()));
    return aTopology.IsNull() ? false : RemoveAttachment(aTopology->FindByUID(theUID));
  }
  if (!storage().Remove(theUID))
  {
    return false;
  }
  myGraph->layerSupplementRegistry().DispatchOnSupplementRemoved(theUID);
  return true;
}

//=================================================================================================

BRepGraphSupInc_TopologyId BRepGraph::SupplementsView::Attach(
  const BRepGraph_NodeId theOwner,
  const BRepGraphSupInc::TopologyAttachmentKind theKind,
  const TopoDS_Shape& theShape)
{
  if (!theOwner.IsValid() || theShape.IsNull() || myGraph->Topo().Gen().IsRemoved(theOwner))
  {
    return BRepGraphSupInc_TopologyId();
  }
  const BRepGraphSupInc_TopologyId anID = storage().AddTopology(theOwner, theKind, theShape);
  if (anID.IsValid())
  {
    myGraph->markModified(theOwner);
  }
  return anID;
}

//=================================================================================================

const NCollection_LinearVector<BRepGraphSupInc_TopologyId>& BRepGraph::SupplementsView::Attachments(
  const BRepGraph_NodeId theOwner) const
{
  const occ::handle<BRepGraphSupInc_TopologyStore> aTopology = occ::down_cast<BRepGraphSupInc_TopologyStore>(
    FindStore(BRepGraphSupInc_TopologyStore::GetID()));
  static const NCollection_LinearVector<BRepGraphSupInc_TopologyId> THE_EMPTY;
  return aTopology.IsNull() ? THE_EMPTY : aTopology->AttachedTo(theOwner);
}

//=================================================================================================

const BRepGraphSupInc::TopologyDef* BRepGraph::SupplementsView::Attachment(
  const BRepGraphSupInc_TopologyId theID) const
{
  const occ::handle<BRepGraphSupInc_TopologyStore> aTopology = occ::down_cast<BRepGraphSupInc_TopologyStore>(
    FindStore(BRepGraphSupInc_TopologyStore::GetID()));
  return aTopology.IsNull() ? nullptr : aTopology->Find(theID);
}

//=================================================================================================

const TopoDS_Shape* BRepGraph::SupplementsView::AttachmentShape(
  const BRepGraphSupInc::TopologyDef& theAttachment) const
{
  return &theAttachment.Shape;
}

//=================================================================================================

bool BRepGraph::SupplementsView::RemoveAttachment(const BRepGraphSupInc_TopologyId theID)
{
  const BRepGraphSupInc::TopologyDef* anAttachment = Attachment(theID);
  if (anAttachment == nullptr)
  {
    return false;
  }
  const BRepGraph_NodeId anOwner = anAttachment->Owner;
  const occ::handle<BRepGraphSupInc_TopologyStore> aTopology = occ::down_cast<BRepGraphSupInc_TopologyStore>(
    FindStore(BRepGraphSupInc_TopologyStore::GetID()));
  const BRepGraphSupInc_ItemUID anAttachmentUID = aTopology->ItemUID(theID);
  if (!storage().RemoveTopology(theID))
  {
    return false;
  }
  myGraph->markModified(anOwner);
  myGraph->layerSupplementRegistry().DispatchOnSupplementRemoved(anAttachmentUID);
  return true;
}

//=================================================================================================

void BRepGraph::SupplementsView::RemoveOwnerAttachments(const BRepGraph_NodeId theOwner)
{
  const occ::handle<BRepGraphSupInc_TopologyStore> aTopology = occ::down_cast<BRepGraphSupInc_TopologyStore>(
    FindStore(BRepGraphSupInc_TopologyStore::GetID()));
  if (!aTopology.IsNull())
  {
    const NCollection_LinearVector<BRepGraphSupInc_TopologyId> anAttachments =
      aTopology->AttachedTo(theOwner);
    NCollection_LinearVector<BRepGraphSupInc_ItemUID> anAttachmentUIDs;
    anAttachmentUIDs.Reserve(anAttachments.Size());
    for (const BRepGraphSupInc_TopologyId anAttachment : anAttachments)
    {
      anAttachmentUIDs.Append(aTopology->ItemUID(anAttachment));
    }
    storage().RemoveTopologyOwner(theOwner);
    for (const BRepGraphSupInc_ItemUID& anAttachmentUID : anAttachmentUIDs)
    {
      myGraph->markModified(theOwner);
      myGraph->layerSupplementRegistry().DispatchOnSupplementRemoved(anAttachmentUID);
    }
  }
  if (!theOwner.IsRemoved(*myGraph))
  {
    myGraph->markModified(theOwner);
  }
}

//=================================================================================================

void BRepGraph::SupplementsView::ReplaceOwnerAttachments(const BRepGraph_NodeId theOldOwner,
                                                           const BRepGraph_NodeId theNewOwner)
{
  const occ::handle<BRepGraphSupInc_TopologyStore> aTopology =
    occ::down_cast<BRepGraphSupInc_TopologyStore>(
      FindStore(BRepGraphSupInc_TopologyStore::GetID()));
  NCollection_LinearVector<BRepGraphSupInc_ItemUID> anAttachmentUIDs;
  if (!aTopology.IsNull())
  {
    const NCollection_LinearVector<BRepGraphSupInc_TopologyId> anAttachments =
      aTopology->AttachedTo(theOldOwner);
    anAttachmentUIDs.Reserve(anAttachments.Size());
    for (const BRepGraphSupInc_TopologyId anAttachment : anAttachments)
    {
      anAttachmentUIDs.Append(aTopology->ItemUID(anAttachment));
    }
  }
  storage().ReplaceTopologyOwner(theOldOwner, theNewOwner);
  for (const BRepGraphSupInc_ItemUID& anAttachmentUID : anAttachmentUIDs)
  {
    if (!aTopology->FindByUID(anAttachmentUID).IsValid())
    {
      myGraph->layerSupplementRegistry().DispatchOnSupplementRemoved(anAttachmentUID);
    }
  }
  if (!theOldOwner.IsRemoved(*myGraph))
  {
    myGraph->markModified(theOldOwner);
  }
  if (!theNewOwner.IsRemoved(*myGraph))
  {
    myGraph->markModified(theNewOwner);
  }
}

//=================================================================================================

void BRepGraph::SupplementsView::Compact()
{
  storage().Compact();
}

//=================================================================================================

BRepGraphSupInc_Storage& BRepGraph::SupplementsView::storage() const
{
  if (myGraph == nullptr || !myGraph->IsValid())
  {
    throw Standard_ProgramError("BRepGraph::Supplements(): view is detached from graph");
  }
  return myGraph->supplementStorage();
}
