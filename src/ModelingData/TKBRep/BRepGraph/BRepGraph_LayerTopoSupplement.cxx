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

#include <BRepGraph_LayerTopoSupplement.hxx>

#include <BRepGraph_LayerRegistry.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <NCollection_LinearVector.hxx>
#include <Standard_Assert.hxx>
#include <Standard_GUID.hxx>
#include <Standard_ProgramError.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepGraph_LayerTopoSupplement, BRepGraph_Layer)

namespace
{
const TCollection_AsciiString THE_SUPPLEMENT_LAYER_NAME("LayerTopoSupplement");

bool isSupportedOwnerKind(const BRepGraph_NodeId::Kind theKind)
{
  switch (theKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
    case BRepGraph_NodeId::Kind::Edge:
    case BRepGraph_NodeId::Kind::Face:
    case BRepGraph_NodeId::Kind::Shell:
    case BRepGraph_NodeId::Kind::Solid:
    case BRepGraph_NodeId::Kind::CompSolid:
    case BRepGraph_NodeId::Kind::Compound:
      return true;
    case BRepGraph_NodeId::Kind::Wire:
    case BRepGraph_NodeId::Kind::CoEdge:
    case BRepGraph_NodeId::Kind::Product:
    case BRepGraph_NodeId::Kind::Occurrence:
      return false;
  }
  return false;
}

bool isAttachmentKindCompatible(const BRepGraph_NodeId::Kind                        theOwnerKind,
                                const BRepGraph_LayerTopoSupplement::AttachmentKind theKind)
{
  switch (theKind)
  {
    case BRepGraph_LayerTopoSupplement::AttachmentKind::GenericSupplementShape:
      return isSupportedOwnerKind(theOwnerKind);
    case BRepGraph_LayerTopoSupplement::AttachmentKind::VertexSupplementShape:
      return theOwnerKind == BRepGraph_NodeId::Kind::Vertex;
    case BRepGraph_LayerTopoSupplement::AttachmentKind::EdgeInternalVertex:
      return theOwnerKind == BRepGraph_NodeId::Kind::Edge;
    case BRepGraph_LayerTopoSupplement::AttachmentKind::FaceDirectVertex:
      return theOwnerKind == BRepGraph_NodeId::Kind::Face;
    case BRepGraph_LayerTopoSupplement::AttachmentKind::SolidAuxShape:
      return theOwnerKind == BRepGraph_NodeId::Kind::Solid;
    case BRepGraph_LayerTopoSupplement::AttachmentKind::ShellAuxShape:
      return theOwnerKind == BRepGraph_NodeId::Kind::Shell;
    case BRepGraph_LayerTopoSupplement::AttachmentKind::CompSolidAuxShape:
      return theOwnerKind == BRepGraph_NodeId::Kind::CompSolid;
    case BRepGraph_LayerTopoSupplement::AttachmentKind::CompoundAuxShape:
      return theOwnerKind == BRepGraph_NodeId::Kind::Compound;
  }
  return false;
}
} // namespace

//=================================================================================================

const Standard_GUID& BRepGraph_LayerTopoSupplement::GetID()
{
  static const Standard_GUID THE_GUID("bd76a339-9958-4d92-a3de-1296bd67a992");
  return THE_GUID;
}

//=================================================================================================

const Standard_GUID& BRepGraph_LayerTopoSupplement::ID() const
{
  return GetID();
}

//=================================================================================================

const TCollection_AsciiString& BRepGraph_LayerTopoSupplement::Name() const
{
  return THE_SUPPLEMENT_LAYER_NAME;
}

//=================================================================================================

const BRepGraph_LayerTopoSupplement::Entry* BRepGraph_LayerTopoSupplement::FindByUid(
  const uint64_t theUid) const
{
  return myEntries.Seek(theUid);
}

//=================================================================================================

const NCollection_LinearVector<uint64_t>& BRepGraph_LayerTopoSupplement::AttachedTo(
  const BRepGraph_NodeId theOwner) const
{
  const NCollection_LinearVector<uint64_t>* aFound = myOwnerToUids.Seek(theOwner);
  return aFound != nullptr ? *aFound : myEmptyUids;
}

//=================================================================================================

uint64_t BRepGraph_LayerTopoSupplement::AddAttachment(const BRepGraph_NodeId theOwner,
                                                      const AttachmentKind   theKind,
                                                      const TopoDS_Shape&    theShape)
{
  const uint64_t aUid = myNextUid;
  if (!AddAttachmentWithUid(theOwner, aUid, theKind, theShape))
  {
    return 0;
  }
  return aUid;
}

//=================================================================================================

bool BRepGraph_LayerTopoSupplement::AddAttachmentWithUid(const BRepGraph_NodeId theOwner,
                                                         const uint64_t         theUid,
                                                         const AttachmentKind   theKind,
                                                         const TopoDS_Shape&    theShape)
{
  if (!theOwner.IsValid() || !isSupportedOwnerKind(theOwner.NodeKind) || theShape.IsNull()
      || !isAttachmentKindCompatible(theOwner.NodeKind, theKind) || theUid == 0
      || myEntries.IsBound(theUid))
  {
    return false;
  }

  Entry anEntry;
  anEntry.BaseOwner = theOwner;
  anEntry.LocalUid  = theUid;
  anEntry.Kind      = theKind;
  anEntry.Shape     = theShape;
  myEntries.Bind(anEntry.LocalUid, anEntry);

  NCollection_LinearVector<uint64_t>* aOwnerEntries = myOwnerToUids.ChangeSeek(theOwner);
  if (aOwnerEntries == nullptr)
  {
    NCollection_LinearVector<uint64_t> anIds;
    anIds.Append(anEntry.LocalUid);
    myOwnerToUids.Bind(theOwner, anIds);
  }
  else
  {
    aOwnerEntries->Append(anEntry.LocalUid);
  }

  if (theUid >= myNextUid)
  {
    myNextUid = theUid + 1;
  }
  if (BRepGraph* aGraph = AttachedGraph())
  {
    aGraph->Shapes().ClearCached(theOwner);
  }
  touch();
  return true;
}

//=================================================================================================

bool BRepGraph_LayerTopoSupplement::RemoveAttachment(const uint64_t theUid)
{
  Entry* anEntry = myEntries.ChangeSeek(theUid);
  if (anEntry == nullptr)
  {
    return false;
  }

  NCollection_LinearVector<uint64_t>* aOwnerEntries = myOwnerToUids.ChangeSeek(anEntry->BaseOwner);
  if (aOwnerEntries != nullptr)
  {
    for (size_t anIdx = 0; anIdx < aOwnerEntries->Size(); ++anIdx)
    {
      if (aOwnerEntries->Value(anIdx) != theUid)
      {
        continue;
      }

      for (size_t aMoveIdx = anIdx + 1; aMoveIdx < aOwnerEntries->Size(); ++aMoveIdx)
      {
        aOwnerEntries->SetValue(aMoveIdx - 1, aOwnerEntries->Value(aMoveIdx));
      }
      aOwnerEntries->EraseLast();
      break;
    }
    if (aOwnerEntries->IsEmpty())
    {
      myOwnerToUids.UnBind(anEntry->BaseOwner);
    }
  }

  if (BRepGraph* aGraph = AttachedGraph())
  {
    aGraph->Shapes().ClearCached(anEntry->BaseOwner);
  }
  myEntries.UnBind(theUid);
  touch();
  return true;
}

//=================================================================================================

void BRepGraph_LayerTopoSupplement::Validate() const
{
  for (NCollection_DataMap<uint64_t, Entry>::Iterator anIt(myEntries); anIt.More(); anIt.Next())
  {
    const Entry& anEntry = anIt.Value();
    if (anEntry.LocalUid == 0 || !anEntry.BaseOwner.IsValid()
        || !isSupportedOwnerKind(anEntry.BaseOwner.NodeKind) || anEntry.Shape.IsNull()
        || !isAttachmentKindCompatible(anEntry.BaseOwner.NodeKind, anEntry.Kind))
    {
      throw Standard_ProgramError("BRepGraph_LayerTopoSupplement::Validate() - invalid entry");
    }

    const NCollection_LinearVector<uint64_t>* aUids = myOwnerToUids.Seek(anEntry.BaseOwner);
    if (aUids == nullptr)
    {
      throw Standard_ProgramError(
        "BRepGraph_LayerTopoSupplement::Validate() - missing owner index");
    }

    bool isFound = false;
    for (const uint64_t aUid : *aUids)
    {
      if (aUid == anEntry.LocalUid)
      {
        isFound = true;
        break;
      }
    }
    if (!isFound)
    {
      throw Standard_ProgramError(
        "BRepGraph_LayerTopoSupplement::Validate() - missing owner linkage");
    }
  }
}

//=================================================================================================

void BRepGraph_LayerTopoSupplement::OnNodeRemoved(const BRepGraph_NodeId theNode) noexcept
{
  removeOwner(theNode);
}

//=================================================================================================

void BRepGraph_LayerTopoSupplement::OnNodeReplaced(const BRepGraph_NodeId theOldNode,
                                                   const BRepGraph_NodeId theNewNode) noexcept
{
  if (!theOldNode.IsValid())
  {
    return;
  }
  if (!theNewNode.IsValid() || theOldNode.NodeKind != theNewNode.NodeKind)
  {
    removeOwner(theOldNode);
    return;
  }

  NCollection_LinearVector<uint64_t>* anOldList = myOwnerToUids.ChangeSeek(theOldNode);
  if (anOldList == nullptr)
  {
    return;
  }

  NCollection_LinearVector<uint64_t>* aNewList = myOwnerToUids.ChangeSeek(theNewNode);
  if (aNewList == nullptr)
  {
    NCollection_LinearVector<uint64_t> aMoved = *anOldList;
    myOwnerToUids.Bind(theNewNode, aMoved);
    aNewList = myOwnerToUids.ChangeSeek(theNewNode);
  }
  else
  {
    for (const uint64_t aUid : *anOldList)
    {
      aNewList->Append(aUid);
    }
  }

  for (const uint64_t aUid : *anOldList)
  {
    Entry* anEntry = myEntries.ChangeSeek(aUid);
    if (anEntry != nullptr)
    {
      anEntry->BaseOwner = theNewNode;
    }
  }
  myOwnerToUids.UnBind(theOldNode);
  touch();
}

//=================================================================================================

void BRepGraph_LayerTopoSupplement::CopyTo(const BRepGraph_CopyRemap& theCopy) const
{
  if (myEntries.IsEmpty())
  {
    return;
  }

  occ::handle<BRepGraph_LayerTopoSupplement> aTarget =
    theCopy.TargetGraph().LayerRegistry().Ensure<BRepGraph_LayerTopoSupplement>();

  NCollection_LinearVector<Entry> aSourceEntries;
  aSourceEntries.Reserve(myEntries.Extent());
  for (NCollection_DataMap<uint64_t, Entry>::Iterator anIt(myEntries); anIt.More(); anIt.Next())
  {
    aSourceEntries.Append(anIt.Value());
  }
  for (const Entry& anEntry : aSourceEntries)
  {
    const BRepGraph_ItemId aTargetItem = theCopy.TargetItem(BRepGraph_ItemId(anEntry.BaseOwner));
    if (!aTargetItem.IsNode())
    {
      continue;
    }

    const BRepGraph_NodeId aTargetOwner = aTargetItem.NodeId();
    if (!aTargetOwner.IsValid())
    {
      continue;
    }
    const bool hasUidCollision = aTarget->myEntries.IsBound(anEntry.LocalUid);
    const bool wasAdded        = hasUidCollision
                                   ? aTarget->AddAttachment(aTargetOwner, anEntry.Kind, anEntry.Shape) != 0
                                   : aTarget->AddAttachmentWithUid(aTargetOwner,
                                                            anEntry.LocalUid,
                                                            anEntry.Kind,
                                                            anEntry.Shape);
    Standard_ASSERT_RAISE(wasAdded,
                          "BRepGraph_LayerTopoSupplement::CopyTo: failed to copy attachment");
  }
}

//=================================================================================================

void BRepGraph_LayerTopoSupplement::InvalidateAll() noexcept
{
  Clear();
}

//=================================================================================================

void BRepGraph_LayerTopoSupplement::Clear() noexcept
{
  myEntries.Clear();
  myOwnerToUids.Clear();
  myNextUid = 1;
  touch();
}

//=================================================================================================

void BRepGraph_LayerTopoSupplement::removeOwner(const BRepGraph_NodeId theOwner) noexcept
{
  NCollection_LinearVector<uint64_t>* aOwnerEntries = myOwnerToUids.ChangeSeek(theOwner);
  if (aOwnerEntries == nullptr)
  {
    return;
  }

  for (const uint64_t aUid : *aOwnerEntries)
  {
    myEntries.UnBind(aUid);
  }
  myOwnerToUids.UnBind(theOwner);
  touch();
}
