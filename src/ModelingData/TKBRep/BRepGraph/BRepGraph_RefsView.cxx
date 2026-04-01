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
#include <BRepGraph_RefsIterator.hxx>
#include <NCollection_PackedMap.hxx>

#include <shared_mutex>

namespace
{
constexpr int THE_REFSVIEW_EDGE_VERTEX_REF_BLOCK_SIZE = 4;

} // namespace

//=================================================================================================

int BRepGraph::RefsView::ShellOps::Nb() const
{
  return myGraph->myData->myIncStorage.NbShellRefs();
}

//=================================================================================================

int BRepGraph::RefsView::ShellOps::NbActive() const
{
  return myGraph->myData->myIncStorage.NbActiveShellRefs();
}

//=================================================================================================

int BRepGraph::RefsView::FaceOps::Nb() const
{
  return myGraph->myData->myIncStorage.NbFaceRefs();
}

//=================================================================================================

int BRepGraph::RefsView::FaceOps::NbActive() const
{
  return myGraph->myData->myIncStorage.NbActiveFaceRefs();
}

//=================================================================================================

int BRepGraph::RefsView::WireOps::Nb() const
{
  return myGraph->myData->myIncStorage.NbWireRefs();
}

//=================================================================================================

int BRepGraph::RefsView::WireOps::NbActive() const
{
  return myGraph->myData->myIncStorage.NbActiveWireRefs();
}

//=================================================================================================

int BRepGraph::RefsView::CoEdgeOps::Nb() const
{
  return myGraph->myData->myIncStorage.NbCoEdgeRefs();
}

//=================================================================================================

int BRepGraph::RefsView::CoEdgeOps::NbActive() const
{
  return myGraph->myData->myIncStorage.NbActiveCoEdgeRefs();
}

//=================================================================================================

int BRepGraph::RefsView::VertexOps::Nb() const
{
  return myGraph->myData->myIncStorage.NbVertexRefs();
}

//=================================================================================================

int BRepGraph::RefsView::VertexOps::NbActive() const
{
  return myGraph->myData->myIncStorage.NbActiveVertexRefs();
}

//=================================================================================================

int BRepGraph::RefsView::SolidOps::Nb() const
{
  return myGraph->myData->myIncStorage.NbSolidRefs();
}

//=================================================================================================

int BRepGraph::RefsView::SolidOps::NbActive() const
{
  return myGraph->myData->myIncStorage.NbActiveSolidRefs();
}

//=================================================================================================

int BRepGraph::RefsView::ChildOps::Nb() const
{
  return myGraph->myData->myIncStorage.NbChildRefs();
}

//=================================================================================================

int BRepGraph::RefsView::ChildOps::NbActive() const
{
  return myGraph->myData->myIncStorage.NbActiveChildRefs();
}

//=================================================================================================

int BRepGraph::RefsView::OccurrenceOps::Nb() const
{
  return myGraph->myData->myIncStorage.NbOccurrenceRefs();
}

//=================================================================================================

int BRepGraph::RefsView::OccurrenceOps::NbActive() const
{
  return myGraph->myData->myIncStorage.NbActiveOccurrenceRefs();
}

//=================================================================================================

const BRepGraphInc::ShellRef& BRepGraph::RefsView::ShellOps::Entry(
  const BRepGraph_ShellRefId theRefId) const
{
  return myGraph->myData->myIncStorage.ShellRef(theRefId);
}

//=================================================================================================

const BRepGraphInc::FaceRef& BRepGraph::RefsView::FaceOps::Entry(
  const BRepGraph_FaceRefId theRefId) const
{
  return myGraph->myData->myIncStorage.FaceRef(theRefId);
}

//=================================================================================================

const BRepGraphInc::WireRef& BRepGraph::RefsView::WireOps::Entry(
  const BRepGraph_WireRefId theRefId) const
{
  return myGraph->myData->myIncStorage.WireRef(theRefId);
}

//=================================================================================================

const BRepGraphInc::CoEdgeRef& BRepGraph::RefsView::CoEdgeOps::Entry(
  const BRepGraph_CoEdgeRefId theRefId) const
{
  return myGraph->myData->myIncStorage.CoEdgeRef(theRefId);
}

//=================================================================================================

const BRepGraphInc::VertexRef& BRepGraph::RefsView::VertexOps::Entry(
  const BRepGraph_VertexRefId theRefId) const
{
  return myGraph->myData->myIncStorage.VertexRef(theRefId);
}

//=================================================================================================

const BRepGraphInc::SolidRef& BRepGraph::RefsView::SolidOps::Entry(
  const BRepGraph_SolidRefId theRefId) const
{
  return myGraph->myData->myIncStorage.SolidRef(theRefId);
}

//=================================================================================================

const BRepGraphInc::ChildRef& BRepGraph::RefsView::ChildOps::Entry(
  const BRepGraph_ChildRefId theRefId) const
{
  return myGraph->myData->myIncStorage.ChildRef(theRefId);
}

//=================================================================================================

const BRepGraphInc::OccurrenceRef& BRepGraph::RefsView::OccurrenceOps::Entry(
  const BRepGraph_OccurrenceRefId theRefId) const
{
  return myGraph->myData->myIncStorage.OccurrenceRef(theRefId);
}

//=================================================================================================

const NCollection_Vector<BRepGraph_FaceRefId>& BRepGraph::RefsView::FaceOps::IdsOf(
  const BRepGraph_ShellId theShell) const
{
  static const NCollection_Vector<BRepGraph_FaceRefId> anEmpty;
  if (!theShell.IsValid(myGraph->myData->myIncStorage.NbShells()))
    return anEmpty;
  return myGraph->myData->myIncStorage.Shell(theShell).FaceRefIds;
}

//=================================================================================================

const NCollection_Vector<BRepGraph_WireRefId>& BRepGraph::RefsView::WireOps::IdsOf(
  const BRepGraph_FaceId theFace) const
{
  static const NCollection_Vector<BRepGraph_WireRefId> anEmpty;
  if (!theFace.IsValid(myGraph->myData->myIncStorage.NbFaces()))
    return anEmpty;
  return myGraph->myData->myIncStorage.Face(theFace).WireRefIds;
}

//=================================================================================================

const NCollection_Vector<BRepGraph_CoEdgeRefId>& BRepGraph::RefsView::CoEdgeOps::IdsOf(
  const BRepGraph_WireId theWire) const
{
  static const NCollection_Vector<BRepGraph_CoEdgeRefId> anEmpty;
  if (!theWire.IsValid(myGraph->myData->myIncStorage.NbWires()))
    return anEmpty;
  return myGraph->myData->myIncStorage.Wire(theWire).CoEdgeRefIds;
}

//=================================================================================================

const NCollection_Vector<BRepGraph_ShellRefId>& BRepGraph::RefsView::ShellOps::IdsOf(
  const BRepGraph_SolidId theSolid) const
{
  static const NCollection_Vector<BRepGraph_ShellRefId> anEmpty;
  if (!theSolid.IsValid(myGraph->myData->myIncStorage.NbSolids()))
    return anEmpty;
  return myGraph->myData->myIncStorage.Solid(theSolid).ShellRefIds;
}

//=================================================================================================

const NCollection_Vector<BRepGraph_ChildRefId>& BRepGraph::RefsView::ChildOps::IdsOf(
  const BRepGraph_CompoundId theCompound) const
{
  static const NCollection_Vector<BRepGraph_ChildRefId> anEmpty;
  if (!theCompound.IsValid(myGraph->myData->myIncStorage.NbCompounds()))
    return anEmpty;
  return myGraph->myData->myIncStorage.Compound(theCompound).ChildRefIds;
}

//=================================================================================================

const NCollection_Vector<BRepGraph_OccurrenceRefId>& BRepGraph::RefsView::OccurrenceOps::IdsOf(
  const BRepGraph_ProductId theProduct) const
{
  static const NCollection_Vector<BRepGraph_OccurrenceRefId> anEmpty;
  if (!theProduct.IsValid(myGraph->myData->myIncStorage.NbProducts()))
    return anEmpty;
  return myGraph->myData->myIncStorage.Product(theProduct).OccurrenceRefIds;
}

//=================================================================================================

const NCollection_Vector<BRepGraph_SolidRefId>& BRepGraph::RefsView::SolidOps::IdsOf(
  const BRepGraph_CompSolidId theCompSolid) const
{
  static const NCollection_Vector<BRepGraph_SolidRefId> anEmpty;
  if (!theCompSolid.IsValid(myGraph->myData->myIncStorage.NbCompSolids()))
    return anEmpty;
  return myGraph->myData->myIncStorage.CompSolid(theCompSolid).SolidRefIds;
}

//=================================================================================================

NCollection_Vector<BRepGraph_VertexRefId> BRepGraph::RefsView::VertexOps::IdsOf(
  const BRepGraph_EdgeId                        theEdge,
  const occ::handle<NCollection_BaseAllocator>& theAllocator) const
{
  NCollection_Vector<BRepGraph_VertexRefId> aResult(THE_REFSVIEW_EDGE_VERTEX_REF_BLOCK_SIZE,
                                                    theAllocator);
  const BRepGraphInc_Storage&               aStorage = myGraph->myData->myIncStorage;
  if (!theEdge.IsValid(aStorage.NbEdges()))
    return aResult;

  NCollection_PackedMap<int>   aSeenRefIds;

  for (BRepGraph_RefsVertexOfEdge aRefIt(*myGraph, theEdge); aRefIt.More(); aRefIt.Next())
  {
    const BRepGraph_VertexRefId aRefId = aRefIt.CurrentId();
    if (aSeenRefIds.Add(aRefId.Index))
    {
      aResult.Append(aRefId);
    }
  }
  return aResult;
}

//=================================================================================================
