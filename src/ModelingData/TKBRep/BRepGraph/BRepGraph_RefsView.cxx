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

#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_Data.hxx>

#include <shared_mutex>

namespace
{

//=================================================================================================

void appendRefUIDReverseIndex(BRepGraph_Data&             theData,
                              const BRepGraph_RefId::Kind theKind)
{
  const NCollection_Vector<BRepGraph_RefUID>& aUIDs = theData.myIncStorage.RefUIDs(theKind);
  for (int anIdx = 0; anIdx < aUIDs.Length(); ++anIdx)
  {
    const BRepGraph_RefUID aUID = aUIDs.Value(anIdx);
    if (aUID.IsValid())
    {
      theData.myRefUIDToRefId.Bind(aUID, BRepGraph_RefId(theKind, anIdx));
    }
  }
}

//=================================================================================================

void ensureRefUIDReverseIndex(BRepGraph_Data& theData)
{
  const uint32_t aGeneration = theData.myGeneration.load();
  {
    std::shared_lock<std::shared_mutex> aReadLock(theData.myRefUIDToRefIdMutex);
    if (!theData.myRefUIDToRefIdDirty && theData.myRefUIDToRefIdGeneration == aGeneration)
    {
      return;
    }
  }

  std::unique_lock<std::shared_mutex> aWriteLock(theData.myRefUIDToRefIdMutex);
  if (!theData.myRefUIDToRefIdDirty && theData.myRefUIDToRefIdGeneration == aGeneration)
  {
    return;
  }

  theData.myRefUIDToRefId.Clear();
  appendRefUIDReverseIndex(theData, BRepGraph_RefId::Kind::Shell);
  appendRefUIDReverseIndex(theData, BRepGraph_RefId::Kind::Face);
  appendRefUIDReverseIndex(theData, BRepGraph_RefId::Kind::Wire);
  appendRefUIDReverseIndex(theData, BRepGraph_RefId::Kind::CoEdge);
  appendRefUIDReverseIndex(theData, BRepGraph_RefId::Kind::Vertex);
  appendRefUIDReverseIndex(theData, BRepGraph_RefId::Kind::Solid);
  appendRefUIDReverseIndex(theData, BRepGraph_RefId::Kind::Child);
  appendRefUIDReverseIndex(theData, BRepGraph_RefId::Kind::Occurrence);
  theData.myRefUIDToRefIdGeneration = aGeneration;
  theData.myRefUIDToRefIdDirty      = false;
}

} // namespace

//=================================================================================================

int BRepGraph::RefsView::NbShellRefs() const
{
  return myGraph->myData->myIncStorage.NbShellRefs();
}

//=================================================================================================

int BRepGraph::RefsView::NbFaceRefs() const
{
  return myGraph->myData->myIncStorage.NbFaceRefs();
}

//=================================================================================================

int BRepGraph::RefsView::NbWireRefs() const
{
  return myGraph->myData->myIncStorage.NbWireRefs();
}

//=================================================================================================

int BRepGraph::RefsView::NbCoEdgeRefs() const
{
  return myGraph->myData->myIncStorage.NbCoEdgeRefs();
}

//=================================================================================================

int BRepGraph::RefsView::NbVertexRefs() const
{
  return myGraph->myData->myIncStorage.NbVertexRefs();
}

//=================================================================================================

int BRepGraph::RefsView::NbSolidRefs() const
{
  return myGraph->myData->myIncStorage.NbSolidRefs();
}

//=================================================================================================

int BRepGraph::RefsView::NbChildRefs() const
{
  return myGraph->myData->myIncStorage.NbChildRefs();
}

//=================================================================================================

int BRepGraph::RefsView::NbOccurrenceRefs() const
{
  return myGraph->myData->myIncStorage.NbOccurrenceRefs();
}

//=================================================================================================

const BRepGraphInc::ShellRef& BRepGraph::RefsView::Shell(const BRepGraph_ShellRefId theRefId) const
{
  return myGraph->myData->myIncStorage.ShellRef(theRefId);
}

//=================================================================================================

const BRepGraphInc::FaceRef& BRepGraph::RefsView::Face(const BRepGraph_FaceRefId theRefId) const
{
  return myGraph->myData->myIncStorage.FaceRef(theRefId);
}

//=================================================================================================

const BRepGraphInc::WireRef& BRepGraph::RefsView::Wire(const BRepGraph_WireRefId theRefId) const
{
  return myGraph->myData->myIncStorage.WireRef(theRefId);
}

//=================================================================================================

const BRepGraphInc::CoEdgeRef& BRepGraph::RefsView::CoEdge(
  const BRepGraph_CoEdgeRefId theRefId) const
{
  return myGraph->myData->myIncStorage.CoEdgeRef(theRefId);
}

//=================================================================================================

const BRepGraphInc::VertexRef& BRepGraph::RefsView::Vertex(
  const BRepGraph_VertexRefId theRefId) const
{
  return myGraph->myData->myIncStorage.VertexRef(theRefId);
}

//=================================================================================================

const BRepGraphInc::SolidRef& BRepGraph::RefsView::Solid(const BRepGraph_SolidRefId theRefId) const
{
  return myGraph->myData->myIncStorage.SolidRef(theRefId);
}

//=================================================================================================

const BRepGraphInc::ChildRef& BRepGraph::RefsView::Child(const BRepGraph_ChildRefId theRefId) const
{
  return myGraph->myData->myIncStorage.ChildRef(theRefId);
}

//=================================================================================================

const BRepGraphInc::OccurrenceRef& BRepGraph::RefsView::Occurrence(
  const BRepGraph_OccurrenceRefId theRefId) const
{
  return myGraph->myData->myIncStorage.OccurrenceRef(theRefId);
}

//=================================================================================================

BRepGraph_RefUID BRepGraph::RefsView::UIDOf(const BRepGraph_RefId theRefId) const
{
  if (!theRefId.IsValid())
    return BRepGraph_RefUID();

  const NCollection_Vector<BRepGraph_RefUID>& aVec =
    myGraph->myData->myIncStorage.RefUIDs(theRefId.RefKind);
  if (theRefId.Index >= aVec.Length())
    return BRepGraph_RefUID();
  return aVec.Value(theRefId.Index);
}

//=================================================================================================

BRepGraph_RefId BRepGraph::RefsView::RefIdFrom(const BRepGraph_RefUID& theUID) const
{
  if (!theUID.IsValid())
    return BRepGraph_RefId();
  if (theUID.Generation() != myGraph->myData->myGeneration.load())
    return BRepGraph_RefId();

  BRepGraph_Data& aData = *myGraph->myData;
  ensureRefUIDReverseIndex(aData);

  std::shared_lock<std::shared_mutex> aReadLock(aData.myRefUIDToRefIdMutex);
  const BRepGraph_RefId*              aRefId = aData.myRefUIDToRefId.Seek(theUID);
  return aRefId != nullptr ? *aRefId : BRepGraph_RefId();
}

//=================================================================================================

bool BRepGraph::RefsView::Has(const BRepGraph_RefUID& theUID) const
{
  if (!theUID.IsValid())
    return false;
  if (theUID.Generation() != myGraph->myData->myGeneration.load())
    return false;

  BRepGraph_Data& aData = *myGraph->myData;
  ensureRefUIDReverseIndex(aData);

  std::shared_lock<std::shared_mutex> aReadLock(aData.myRefUIDToRefIdMutex);
  return aData.myRefUIDToRefId.Seek(theUID) != nullptr;
}

//=================================================================================================

BRepGraph_VersionStamp BRepGraph::RefsView::StampOf(const BRepGraph_RefId theRefId) const
{
  if (!theRefId.IsValid())
    return BRepGraph_VersionStamp();

  const NCollection_Vector<BRepGraph_RefUID>& aUIDs =
    myGraph->myData->myIncStorage.RefUIDs(theRefId.RefKind);
  if (theRefId.Index >= aUIDs.Length())
    return BRepGraph_VersionStamp();

  const BRepGraphInc::BaseRef& aBase = myGraph->myData->myIncStorage.BaseRef(theRefId);
  if (aBase.IsRemoved)
    return BRepGraph_VersionStamp();

  return BRepGraph_VersionStamp(aUIDs.Value(theRefId.Index),
                                aBase.OwnGen,
                                myGraph->myData->myGeneration.load());
}

//=================================================================================================

bool BRepGraph::RefsView::IsStale(const BRepGraph_VersionStamp& theStamp) const
{
  if (!theStamp.IsValid())
    return true;

  if (!theStamp.IsRefStamp())
    return true;

  if (theStamp.myGeneration != myGraph->myData->myGeneration.load())
    return true;

  const BRepGraph_RefId aRefId = RefIdFrom(theStamp.myRefUID);
  if (!aRefId.IsValid())
    return true;

  const BRepGraph_VersionStamp aCurrent = StampOf(aRefId);
  if (!aCurrent.IsValid())
    return true;

  return aCurrent.myMutationGen != theStamp.myMutationGen;
}

//=================================================================================================

const NCollection_Vector<BRepGraph_FaceRefId>& BRepGraph::RefsView::FaceRefIdsOf(
  const BRepGraph_ShellId theShell) const
{
  static const NCollection_Vector<BRepGraph_FaceRefId> anEmpty;
  if (!theShell.IsValid(myGraph->myData->myIncStorage.NbShells()))
    return anEmpty;
  return myGraph->myData->myIncStorage.Shell(theShell).FaceRefIds;
}

//=================================================================================================

const NCollection_Vector<BRepGraph_WireRefId>& BRepGraph::RefsView::WireRefIdsOf(
  const BRepGraph_FaceId theFace) const
{
  static const NCollection_Vector<BRepGraph_WireRefId> anEmpty;
  if (!theFace.IsValid(myGraph->myData->myIncStorage.NbFaces()))
    return anEmpty;
  return myGraph->myData->myIncStorage.Face(theFace).WireRefIds;
}

//=================================================================================================

const NCollection_Vector<BRepGraph_CoEdgeRefId>& BRepGraph::RefsView::CoEdgeRefIdsOf(
  const BRepGraph_WireId theWire) const
{
  static const NCollection_Vector<BRepGraph_CoEdgeRefId> anEmpty;
  if (!theWire.IsValid(myGraph->myData->myIncStorage.NbWires()))
    return anEmpty;
  return myGraph->myData->myIncStorage.Wire(theWire).CoEdgeRefIds;
}

//=================================================================================================

const NCollection_Vector<BRepGraph_ShellRefId>& BRepGraph::RefsView::ShellRefIdsOf(
  const BRepGraph_SolidId theSolid) const
{
  static const NCollection_Vector<BRepGraph_ShellRefId> anEmpty;
  if (!theSolid.IsValid(myGraph->myData->myIncStorage.NbSolids()))
    return anEmpty;
  return myGraph->myData->myIncStorage.Solid(theSolid).ShellRefIds;
}

//=================================================================================================

const NCollection_Vector<BRepGraph_ChildRefId>& BRepGraph::RefsView::ChildRefIdsOf(
  const BRepGraph_CompoundId theCompound) const
{
  static const NCollection_Vector<BRepGraph_ChildRefId> anEmpty;
  if (!theCompound.IsValid(myGraph->myData->myIncStorage.NbCompounds()))
    return anEmpty;
  return myGraph->myData->myIncStorage.Compound(theCompound).ChildRefIds;
}

//=================================================================================================

const NCollection_Vector<BRepGraph_OccurrenceRefId>& BRepGraph::RefsView::OccurrenceRefIdsOf(
  const BRepGraph_ProductId theProduct) const
{
  static const NCollection_Vector<BRepGraph_OccurrenceRefId> anEmpty;
  if (!theProduct.IsValid(myGraph->myData->myIncStorage.NbProducts()))
    return anEmpty;
  return myGraph->myData->myIncStorage.Product(theProduct).OccurrenceRefIds;
}

//=================================================================================================

const NCollection_Vector<BRepGraph_SolidRefId>& BRepGraph::RefsView::SolidRefIdsOf(
  const BRepGraph_CompSolidId theCompSolid) const
{
  static const NCollection_Vector<BRepGraph_SolidRefId> anEmpty;
  if (!theCompSolid.IsValid(myGraph->myData->myIncStorage.NbCompSolids()))
    return anEmpty;
  return myGraph->myData->myIncStorage.CompSolid(theCompSolid).SolidRefIds;
}

//=================================================================================================
