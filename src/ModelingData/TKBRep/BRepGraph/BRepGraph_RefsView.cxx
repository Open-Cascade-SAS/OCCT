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

const BRepGraphInc::ShellRefEntry& BRepGraph::RefsView::Shell(const BRepGraph_ShellRefId theRefId) const
{
  return myGraph->myData->myIncStorage.ShellRefEntry(theRefId);
}

//=================================================================================================

const BRepGraphInc::FaceRefEntry& BRepGraph::RefsView::Face(const BRepGraph_FaceRefId theRefId) const
{
  return myGraph->myData->myIncStorage.FaceRefEntry(theRefId);
}

//=================================================================================================

const BRepGraphInc::WireRefEntry& BRepGraph::RefsView::Wire(const BRepGraph_WireRefId theRefId) const
{
  return myGraph->myData->myIncStorage.WireRefEntry(theRefId);
}

//=================================================================================================

const BRepGraphInc::CoEdgeRefEntry& BRepGraph::RefsView::CoEdge(
  const BRepGraph_CoEdgeRefId theRefId) const
{
  return myGraph->myData->myIncStorage.CoEdgeRefEntry(theRefId);
}

//=================================================================================================

const BRepGraphInc::VertexRefEntry& BRepGraph::RefsView::Vertex(
  const BRepGraph_VertexRefId theRefId) const
{
  return myGraph->myData->myIncStorage.VertexRefEntry(theRefId);
}

//=================================================================================================

const BRepGraphInc::SolidRefEntry& BRepGraph::RefsView::Solid(const BRepGraph_SolidRefId theRefId) const
{
  return myGraph->myData->myIncStorage.SolidRefEntry(theRefId);
}

//=================================================================================================

const BRepGraphInc::ChildRefEntry& BRepGraph::RefsView::Child(const BRepGraph_ChildRefId theRefId) const
{
  return myGraph->myData->myIncStorage.ChildRefEntry(theRefId);
}

//=================================================================================================

const BRepGraphInc::OccurrenceRefEntry& BRepGraph::RefsView::Occurrence(
  const BRepGraph_OccurrenceRefId theRefId) const
{
  return myGraph->myData->myIncStorage.OccurrenceRefEntry(theRefId);
}

//=================================================================================================

BRepGraph_RefUID BRepGraph::RefsView::UIDOf(const BRepGraph_RefId theRefId) const
{
  if (!theRefId.IsValid())
    return BRepGraph_RefUID();

  const NCollection_Vector<BRepGraph_RefUID>& aVec = myGraph->myData->myIncStorage.RefUIDs(
    theRefId.RefKind);
  if (theRefId.Index >= aVec.Length())
    return BRepGraph_RefUID();
  return aVec.Value(theRefId.Index);
}

//=================================================================================================

BRepGraph_RefId BRepGraph::RefsView::RefIdFrom(const BRepGraph_RefUID& theUID) const
{
  if (!theUID.IsValid())
    return BRepGraph_RefId();

  const NCollection_Vector<BRepGraph_RefUID>& aVec = myGraph->myData->myIncStorage.RefUIDs(theUID.Kind());
  for (int i = 0; i < aVec.Length(); ++i)
  {
    if (aVec.Value(i) == theUID)
      return BRepGraph_RefId(theUID.Kind(), i);
  }
  return BRepGraph_RefId();
}

//=================================================================================================

bool BRepGraph::RefsView::Has(const BRepGraph_RefUID& theUID) const
{
  if (!theUID.IsValid())
    return false;
  if (theUID.Generation() != myGraph->myData->myGeneration.load())
    return false;

  const NCollection_Vector<BRepGraph_RefUID>& aVec = myGraph->myData->myIncStorage.RefUIDs(theUID.Kind());
  for (int i = 0; i < aVec.Length(); ++i)
  {
    if (aVec.Value(i) == theUID)
      return true;
  }
  return false;
}

//=================================================================================================

BRepGraph_VersionStamp BRepGraph::RefsView::StampOf(const BRepGraph_RefId theRefId) const
{
  if (!theRefId.IsValid())
    return BRepGraph_VersionStamp();

  const NCollection_Vector<BRepGraph_RefUID>& aUIDs = myGraph->myData->myIncStorage.RefUIDs(
    theRefId.RefKind);
  if (theRefId.Index >= aUIDs.Length())
    return BRepGraph_VersionStamp();

  const BRepGraphInc::BaseRef& aBase = myGraph->myData->myIncStorage.BaseRefEntry(theRefId);
  if (aBase.IsRemoved)
    return BRepGraph_VersionStamp();

  return BRepGraph_VersionStamp(aUIDs.Value(theRefId.Index),
                                aBase.MutationGen,
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

BRepGraph_WireId BRepGraph::RefsView::OuterWireOfFace(const BRepGraph_FaceId theFace) const
{
  if (!theFace.IsValid(myGraph->myData->myIncStorage.NbFaces()))
    return BRepGraph_WireId();

  const BRepGraphInc::FaceEntity& aFaceEnt = myGraph->myData->myIncStorage.Face(theFace);
  for (int i = 0; i < aFaceEnt.WireRefIds.Length(); ++i)
  {
    const BRepGraphInc::WireRefEntry& aWireRef =
      myGraph->myData->myIncStorage.WireRefEntry(aFaceEnt.WireRefIds.Value(i));
    if (!aWireRef.IsRemoved && aWireRef.IsOuter)
      return aWireRef.WireDefId;
  }
  return BRepGraph_WireId();
}
