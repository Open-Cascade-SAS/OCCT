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

#include <shared_mutex>

//=================================================================================================

uint32_t BRepGraph::RefsView::ShellOps::Nb() const
{
  return myGraph->myData->myIncStorage.NbShellRefs();
}

//=================================================================================================

uint32_t BRepGraph::RefsView::ShellOps::NbActive() const
{
  return myGraph->myData->myIncStorage.NbActiveShellRefs();
}

//=================================================================================================

uint32_t BRepGraph::RefsView::FaceOps::Nb() const
{
  return myGraph->myData->myIncStorage.NbFaceRefs();
}

//=================================================================================================

uint32_t BRepGraph::RefsView::FaceOps::NbActive() const
{
  return myGraph->myData->myIncStorage.NbActiveFaceRefs();
}

//=================================================================================================

uint32_t BRepGraph::RefsView::WireOps::Nb() const
{
  return myGraph->myData->myIncStorage.NbWireRefs();
}

//=================================================================================================

uint32_t BRepGraph::RefsView::WireOps::NbActive() const
{
  return myGraph->myData->myIncStorage.NbActiveWireRefs();
}

//=================================================================================================

uint32_t BRepGraph::RefsView::VertexOps::Nb() const
{
  return myGraph->myData->myIncStorage.NbVertexRefs();
}

//=================================================================================================

uint32_t BRepGraph::RefsView::VertexOps::NbActive() const
{
  return myGraph->myData->myIncStorage.NbActiveVertexRefs();
}

//=================================================================================================

uint32_t BRepGraph::RefsView::SolidOps::Nb() const
{
  return myGraph->myData->myIncStorage.NbSolidRefs();
}

//=================================================================================================

uint32_t BRepGraph::RefsView::SolidOps::NbActive() const
{
  return myGraph->myData->myIncStorage.NbActiveSolidRefs();
}

//=================================================================================================

uint32_t BRepGraph::RefsView::ChildOps::Nb() const
{
  return myGraph->myData->myIncStorage.NbChildRefs();
}

//=================================================================================================

uint32_t BRepGraph::RefsView::ChildOps::NbActive() const
{
  return myGraph->myData->myIncStorage.NbActiveChildRefs();
}

//=================================================================================================

uint32_t BRepGraph::RefsView::OccurrenceOps::Nb() const
{
  return myGraph->myData->myIncStorage.NbOccurrenceRefs();
}

//=================================================================================================

uint32_t BRepGraph::RefsView::OccurrenceOps::NbActive() const
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

const NCollection_LinearVector<BRepGraph_FaceRefId>& BRepGraph::RefsView::FaceOps::IdsOf(
  const BRepGraph_ShellId theShell) const
{
  static const NCollection_LinearVector<BRepGraph_FaceRefId> anEmpty;
  if (!theShell.IsValid(myGraph->myData->myIncStorage.NbShells()))
  {
    return anEmpty;
  }
  return myGraph->Topo().Shells().Relations(theShell).FaceRefIds;
}

//=================================================================================================

const NCollection_LinearVector<BRepGraph_WireRefId>& BRepGraph::RefsView::WireOps::IdsOf(
  const BRepGraph_FaceId theFace) const
{
  static const NCollection_LinearVector<BRepGraph_WireRefId> anEmpty;
  if (!theFace.IsValid(myGraph->myData->myIncStorage.NbFaces()))
  {
    return anEmpty;
  }
  return myGraph->Topo().Faces().Relations(theFace).WireRefIds;
}

//=================================================================================================

const NCollection_LinearVector<BRepGraph_ShellRefId>& BRepGraph::RefsView::ShellOps::IdsOf(
  const BRepGraph_SolidId theSolid) const
{
  static const NCollection_LinearVector<BRepGraph_ShellRefId> anEmpty;
  if (!theSolid.IsValid(myGraph->myData->myIncStorage.NbSolids()))
  {
    return anEmpty;
  }
  return myGraph->Topo().Solids().Relations(theSolid).ShellRefIds;
}

//=================================================================================================

const NCollection_LinearVector<BRepGraph_ChildRefId>& BRepGraph::RefsView::ChildOps::IdsOf(
  const BRepGraph_CompoundId theCompound) const
{
  static const NCollection_LinearVector<BRepGraph_ChildRefId> anEmpty;
  if (!theCompound.IsValid(myGraph->myData->myIncStorage.NbCompounds()))
  {
    return anEmpty;
  }
  return myGraph->Topo().Compounds().Relations(theCompound).ChildRefIds;
}

//=================================================================================================

const NCollection_LinearVector<BRepGraph_ChildRefId>& BRepGraph::RefsView::ChildOps::IdsReferencing(
  const BRepGraph_NodeId theChild) const
{
  return myGraph->myData->myIncStorage.CompoundRefsOfNode(theChild);
}

//=================================================================================================

const NCollection_LinearVector<BRepGraph_OccurrenceRefId>& BRepGraph::RefsView::OccurrenceOps::
  IdsOf(const BRepGraph_ProductId theProduct) const
{
  static const NCollection_LinearVector<BRepGraph_OccurrenceRefId> anEmpty;
  if (!theProduct.IsValid(myGraph->myData->myIncStorage.NbProducts()))
  {
    return anEmpty;
  }
  return myGraph->Topo().Products().Relations(theProduct).OccurrenceRefIds;
}

//=================================================================================================

const NCollection_LinearVector<BRepGraph_OccurrenceRefId>& BRepGraph::RefsView::OccurrenceOps::
  IdsReferencing(const BRepGraph_NodeId theChild) const
{
  return myGraph->myData->myIncStorage.OccurrenceRefsOfNode(theChild);
}

//=================================================================================================

uint32_t BRepGraph::RefsView::GenOps::Nb(const BRepGraph_RefId::Kind theKind) const
{
  const BRepGraphInc_Storage& aS = myGraph->myData->myIncStorage;
  switch (theKind)
  {
    case BRepGraph_RefId::Kind::Shell:
      return aS.NbShellRefs();
    case BRepGraph_RefId::Kind::Face:
      return aS.NbFaceRefs();
    case BRepGraph_RefId::Kind::Wire:
      return aS.NbWireRefs();
    case BRepGraph_RefId::Kind::Vertex:
      return aS.NbVertexRefs();
    case BRepGraph_RefId::Kind::Solid:
      return aS.NbSolidRefs();
    case BRepGraph_RefId::Kind::Child:
      return aS.NbChildRefs();
    case BRepGraph_RefId::Kind::Occurrence:
      return aS.NbOccurrenceRefs();
  }

  return 0;
}

//=================================================================================================

bool BRepGraph::RefsView::GenOps::IsValid(const BRepGraph_RefId theRef) const
{
  if (!theRef.IsValid())
  {
    return false;
  }
  return theRef.Index < Nb(theRef.RefKind);
}

//=================================================================================================

bool BRepGraph::RefsView::GenOps::IsActive(const BRepGraph_RefId theRef) const
{
  return IsValid(theRef) && !theRef.IsRemoved(*myGraph);
}

//=================================================================================================

BRepGraph_RefId BRepGraph::RefsView::GenOps::RefAtStep(const BRepGraph_NodeId theParent,
                                                       const int              theStep) const
{
  if (!myGraph->Topo().Gen().IsActive(theParent) || theStep < 0)
  {
    return BRepGraph_RefId();
  }

  switch (theParent.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Compound: {
      const BRepGraphInc::CompoundRelations& aRel =
        myGraph->Topo().Compounds().Relations(BRepGraph_CompoundId::FromNodeId(theParent));
      return static_cast<size_t>(theStep) < aRel.ChildRefIds.Size()
               ? aRel.ChildRefIds.Value(static_cast<size_t>(theStep))
               : BRepGraph_RefId();
    }
    case BRepGraph_NodeId::Kind::CompSolid: {
      const BRepGraphInc::CompSolidRelations& aRel =
        myGraph->Topo().CompSolids().Relations(BRepGraph_CompSolidId::FromNodeId(theParent));
      return static_cast<size_t>(theStep) < aRel.SolidRefIds.Size()
               ? aRel.SolidRefIds.Value(static_cast<size_t>(theStep))
               : BRepGraph_RefId();
    }
    case BRepGraph_NodeId::Kind::Solid: {
      const BRepGraphInc::SolidRelations& aRel =
        myGraph->Topo().Solids().Relations(BRepGraph_SolidId::FromNodeId(theParent));
      return static_cast<size_t>(theStep) < aRel.ShellRefIds.Size()
               ? aRel.ShellRefIds.Value(static_cast<size_t>(theStep))
               : BRepGraph_RefId();
    }
    case BRepGraph_NodeId::Kind::Shell: {
      const BRepGraphInc::ShellRelations& aRel =
        myGraph->Topo().Shells().Relations(BRepGraph_ShellId::FromNodeId(theParent));
      return static_cast<size_t>(theStep) < aRel.FaceRefIds.Size()
               ? aRel.FaceRefIds.Value(static_cast<size_t>(theStep))
               : BRepGraph_RefId();
    }
    case BRepGraph_NodeId::Kind::Face: {
      const BRepGraphInc::FaceRelations& aRel =
        myGraph->Topo().Faces().Relations(BRepGraph_FaceId::FromNodeId(theParent));
      return static_cast<size_t>(theStep) < aRel.WireRefIds.Size()
               ? aRel.WireRefIds.Value(static_cast<size_t>(theStep))
               : BRepGraph_RefId();
    }
    case BRepGraph_NodeId::Kind::Edge: {
      const BRepGraphInc::EdgeDef& anEdge =
        myGraph->Topo().Edges().Definition(BRepGraph_EdgeId::FromNodeId(theParent));
      if (theStep == 0)
      {
        return anEdge.StartVertexRefId;
      }
      if (theStep == 1)
      {
        return anEdge.EndVertexRefId;
      }
      return BRepGraph_RefId();
    }
    case BRepGraph_NodeId::Kind::Product: {
      const BRepGraphInc::ProductRelations& aRel =
        myGraph->Topo().Products().Relations(BRepGraph_ProductId::FromNodeId(theParent));
      return static_cast<size_t>(theStep) < aRel.OccurrenceRefIds.Size()
               ? aRel.OccurrenceRefIds.Value(static_cast<size_t>(theStep))
               : BRepGraph_RefId();
    }
    case BRepGraph_NodeId::Kind::Wire:
    case BRepGraph_NodeId::Kind::CoEdge:
    case BRepGraph_NodeId::Kind::Occurrence:
    case BRepGraph_NodeId::Kind::Vertex:
      return BRepGraph_RefId();
  }

  return BRepGraph_RefId();
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::RefsView::GenOps::ChildNode(const BRepGraph_RefId theRef) const
{
  if (!IsValid(theRef))
  {
    return BRepGraph_NodeId();
  }

  switch (theRef.RefKind)
  {
    case BRepGraph_RefId::Kind::Shell:
      return myGraph->Refs().Shells().Entry(BRepGraph_ShellRefId::FromRefId(theRef)).ChildShellId;
    case BRepGraph_RefId::Kind::Face:
      return myGraph->Refs().Faces().Entry(BRepGraph_FaceRefId::FromRefId(theRef)).ChildFaceId;
    case BRepGraph_RefId::Kind::Wire:
      return myGraph->Refs().Wires().Entry(BRepGraph_WireRefId::FromRefId(theRef)).ChildWireId;
    case BRepGraph_RefId::Kind::Vertex:
      return myGraph->Refs()
        .Vertices()
        .Entry(BRepGraph_VertexRefId::FromRefId(theRef))
        .ChildVertexId;
    case BRepGraph_RefId::Kind::Solid:
      return myGraph->Refs().Solids().Entry(BRepGraph_SolidRefId::FromRefId(theRef)).ChildSolidId;
    case BRepGraph_RefId::Kind::Child:
      return myGraph->Refs().Children().Entry(BRepGraph_ChildRefId::FromRefId(theRef)).ChildNodeId;
    case BRepGraph_RefId::Kind::Occurrence:
      return myGraph->Refs()
        .Occurrences()
        .Entry(BRepGraph_OccurrenceRefId::FromRefId(theRef))
        .ChildOccurrenceId;
  }

  return BRepGraph_NodeId();
}

//=================================================================================================

bool BRepGraph::RefsView::GenOps::IsRemoved(const BRepGraph_RefId theRef) const
{
  if (!IsValid(theRef))
  {
    return true;
  }

  switch (theRef.RefKind)
  {
    case BRepGraph_RefId::Kind::Shell:
      return myGraph->incStorage().IsRemoved(BRepGraph_ShellRefId::FromRefId(theRef));
    case BRepGraph_RefId::Kind::Face:
      return myGraph->incStorage().IsRemoved(BRepGraph_FaceRefId::FromRefId(theRef));
    case BRepGraph_RefId::Kind::Wire:
      return myGraph->incStorage().IsRemoved(BRepGraph_WireRefId::FromRefId(theRef));
    case BRepGraph_RefId::Kind::Vertex:
      return myGraph->incStorage().IsRemoved(BRepGraph_VertexRefId::FromRefId(theRef));
    case BRepGraph_RefId::Kind::Solid:
      return myGraph->incStorage().IsRemoved(BRepGraph_SolidRefId::FromRefId(theRef));
    case BRepGraph_RefId::Kind::Child:
      return myGraph->incStorage().IsRemoved(BRepGraph_ChildRefId::FromRefId(theRef));
    case BRepGraph_RefId::Kind::Occurrence:
      return myGraph->incStorage().IsRemoved(BRepGraph_OccurrenceRefId::FromRefId(theRef));
  }

  return true;
}

//=================================================================================================

TopLoc_Location BRepGraph::RefsView::GenOps::LocalLocation(const BRepGraph_RefId theRef) const
{
  if (!IsValid(theRef))
  {
    return TopLoc_Location();
  }

  switch (theRef.RefKind)
  {
    case BRepGraph_RefId::Kind::Shell:
    case BRepGraph_RefId::Kind::Face:
    case BRepGraph_RefId::Kind::Wire:
    case BRepGraph_RefId::Kind::Vertex:
    case BRepGraph_RefId::Kind::Solid:
      return TopLoc_Location();
    case BRepGraph_RefId::Kind::Child:
      return myGraph->Refs()
        .Children()
        .Entry(BRepGraph_ChildRefId::FromRefId(theRef))
        .LocalLocation;
    case BRepGraph_RefId::Kind::Occurrence:
      return myGraph->Refs()
        .Occurrences()
        .Entry(BRepGraph_OccurrenceRefId::FromRefId(theRef))
        .LocalLocation;
  }

  return TopLoc_Location();
}

//=================================================================================================

TopAbs_Orientation BRepGraph::RefsView::GenOps::Orientation(const BRepGraph_RefId theRef) const
{
  if (!IsValid(theRef))
  {
    return TopAbs_FORWARD;
  }

  switch (theRef.RefKind)
  {
    case BRepGraph_RefId::Kind::Shell:
      return myGraph->Refs().Shells().Entry(BRepGraph_ShellRefId::FromRefId(theRef)).Orientation;
    case BRepGraph_RefId::Kind::Face:
      return myGraph->Refs().Faces().Entry(BRepGraph_FaceRefId::FromRefId(theRef)).Orientation;
    case BRepGraph_RefId::Kind::Wire:
      return myGraph->Refs().Wires().Entry(BRepGraph_WireRefId::FromRefId(theRef)).Orientation;
    case BRepGraph_RefId::Kind::Vertex:
      return myGraph->Refs().Vertices().Entry(BRepGraph_VertexRefId::FromRefId(theRef)).Orientation;
    case BRepGraph_RefId::Kind::Solid:
      return myGraph->Refs().Solids().Entry(BRepGraph_SolidRefId::FromRefId(theRef)).Orientation;
    case BRepGraph_RefId::Kind::Child:
      return myGraph->Refs().Children().Entry(BRepGraph_ChildRefId::FromRefId(theRef)).Orientation;
    case BRepGraph_RefId::Kind::Occurrence:
      return TopAbs_FORWARD;
  }

  return TopAbs_FORWARD;
}

//=================================================================================================

const NCollection_LinearVector<BRepGraph_SolidRefId>& BRepGraph::RefsView::SolidOps::IdsOf(
  const BRepGraph_CompSolidId theCompSolid) const
{
  static const NCollection_LinearVector<BRepGraph_SolidRefId> anEmpty;
  if (!theCompSolid.IsValid(myGraph->myData->myIncStorage.NbCompSolids()))
  {
    return anEmpty;
  }
  return myGraph->Topo().CompSolids().Relations(theCompSolid).SolidRefIds;
}

//=================================================================================================
