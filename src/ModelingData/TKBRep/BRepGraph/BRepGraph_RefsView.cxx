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

const NCollection_DynamicArray<BRepGraph_FaceRefId>& BRepGraph::RefsView::FaceOps::IdsOf(
  const BRepGraph_ShellId theShell) const
{
  static const NCollection_DynamicArray<BRepGraph_FaceRefId> anEmpty;
  if (!theShell.IsValid(myGraph->myData->myIncStorage.NbShells()))
    return anEmpty;
  return myGraph->myData->myIncStorage.Shell(theShell).FaceRefIds;
}

//=================================================================================================

const NCollection_DynamicArray<BRepGraph_WireRefId>& BRepGraph::RefsView::WireOps::IdsOf(
  const BRepGraph_FaceId theFace) const
{
  static const NCollection_DynamicArray<BRepGraph_WireRefId> anEmpty;
  if (!theFace.IsValid(myGraph->myData->myIncStorage.NbFaces()))
    return anEmpty;
  return myGraph->myData->myIncStorage.Face(theFace).WireRefIds;
}

//=================================================================================================

const NCollection_DynamicArray<BRepGraph_CoEdgeRefId>& BRepGraph::RefsView::CoEdgeOps::IdsOf(
  const BRepGraph_WireId theWire) const
{
  static const NCollection_DynamicArray<BRepGraph_CoEdgeRefId> anEmpty;
  if (!theWire.IsValid(myGraph->myData->myIncStorage.NbWires()))
    return anEmpty;
  return myGraph->myData->myIncStorage.Wire(theWire).CoEdgeRefIds;
}

//=================================================================================================

const NCollection_DynamicArray<BRepGraph_ShellRefId>& BRepGraph::RefsView::ShellOps::IdsOf(
  const BRepGraph_SolidId theSolid) const
{
  static const NCollection_DynamicArray<BRepGraph_ShellRefId> anEmpty;
  if (!theSolid.IsValid(myGraph->myData->myIncStorage.NbSolids()))
    return anEmpty;
  return myGraph->myData->myIncStorage.Solid(theSolid).ShellRefIds;
}

//=================================================================================================

const NCollection_DynamicArray<BRepGraph_ChildRefId>& BRepGraph::RefsView::ChildOps::IdsOf(
  const BRepGraph_CompoundId theCompound) const
{
  static const NCollection_DynamicArray<BRepGraph_ChildRefId> anEmpty;
  if (!theCompound.IsValid(myGraph->myData->myIncStorage.NbCompounds()))
    return anEmpty;
  return myGraph->myData->myIncStorage.Compound(theCompound).ChildRefIds;
}

//=================================================================================================

const NCollection_DynamicArray<BRepGraph_OccurrenceRefId>& BRepGraph::RefsView::OccurrenceOps::
  IdsOf(const BRepGraph_ProductId theProduct) const
{
  static const NCollection_DynamicArray<BRepGraph_OccurrenceRefId> anEmpty;
  if (!theProduct.IsValid(myGraph->myData->myIncStorage.NbProducts()))
    return anEmpty;
  return myGraph->myData->myIncStorage.Product(theProduct).OccurrenceRefIds;
}

//=================================================================================================

BRepGraph_RefId BRepGraph::RefsView::RefAtStep(const BRepGraph_NodeId theParent,
                                               const int              theStep) const
{
  if (!theParent.IsValid() || theStep < 0)
  {
    return BRepGraph_RefId();
  }

  switch (theParent.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Compound: {
      const BRepGraphInc::CompoundDef& aCompound =
        myGraph->Topo().Compounds().Definition(BRepGraph_CompoundId::FromNodeId(theParent));
      return theStep < aCompound.ChildRefIds.Length() ? aCompound.ChildRefIds.Value(theStep)
                                                      : BRepGraph_RefId();
    }
    case BRepGraph_NodeId::Kind::CompSolid: {
      const BRepGraphInc::CompSolidDef& aCompSolid =
        myGraph->Topo().CompSolids().Definition(BRepGraph_CompSolidId::FromNodeId(theParent));
      return theStep < aCompSolid.SolidRefIds.Length() ? aCompSolid.SolidRefIds.Value(theStep)
                                                       : BRepGraph_RefId();
    }
    case BRepGraph_NodeId::Kind::Solid: {
      const BRepGraphInc::SolidDef& aSolid =
        myGraph->Topo().Solids().Definition(BRepGraph_SolidId::FromNodeId(theParent));
      if (theStep < aSolid.ShellRefIds.Length())
      {
        return aSolid.ShellRefIds.Value(theStep);
      }

      const int aFreeIdx = theStep - aSolid.ShellRefIds.Length();
      return aFreeIdx < aSolid.AuxChildRefIds.Length() ? aSolid.AuxChildRefIds.Value(aFreeIdx)
                                                       : BRepGraph_RefId();
    }
    case BRepGraph_NodeId::Kind::Shell: {
      const BRepGraphInc::ShellDef& aShell =
        myGraph->Topo().Shells().Definition(BRepGraph_ShellId::FromNodeId(theParent));
      if (theStep < aShell.FaceRefIds.Length())
      {
        return aShell.FaceRefIds.Value(theStep);
      }

      const int aFreeIdx = theStep - aShell.FaceRefIds.Length();
      return aFreeIdx < aShell.AuxChildRefIds.Length() ? aShell.AuxChildRefIds.Value(aFreeIdx)
                                                       : BRepGraph_RefId();
    }
    case BRepGraph_NodeId::Kind::Face: {
      const BRepGraphInc::FaceDef& aFace =
        myGraph->Topo().Faces().Definition(BRepGraph_FaceId::FromNodeId(theParent));
      if (theStep < aFace.WireRefIds.Length())
      {
        return aFace.WireRefIds.Value(theStep);
      }

      const int aVertexIdx = theStep - aFace.WireRefIds.Length();
      return aVertexIdx < aFace.VertexRefIds.Length() ? aFace.VertexRefIds.Value(aVertexIdx)
                                                      : BRepGraph_RefId();
    }
    case BRepGraph_NodeId::Kind::Wire: {
      const BRepGraphInc::WireDef& aWire =
        myGraph->Topo().Wires().Definition(BRepGraph_WireId::FromNodeId(theParent));
      return theStep < aWire.CoEdgeRefIds.Length() ? aWire.CoEdgeRefIds.Value(theStep)
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

      const int anInternalIdx = theStep - 2;
      return anInternalIdx < anEdge.InternalVertexRefIds.Length()
               ? anEdge.InternalVertexRefIds.Value(anInternalIdx)
               : BRepGraph_RefId();
    }
    case BRepGraph_NodeId::Kind::Product: {
      const BRepGraphInc::ProductDef& aProduct =
        myGraph->Topo().Products().Definition(BRepGraph_ProductId::FromNodeId(theParent));
      return theStep < aProduct.OccurrenceRefIds.Length() ? aProduct.OccurrenceRefIds.Value(theStep)
                                                          : BRepGraph_RefId();
    }
    case BRepGraph_NodeId::Kind::CoEdge:
    case BRepGraph_NodeId::Kind::Occurrence:
    case BRepGraph_NodeId::Kind::Vertex:
      return BRepGraph_RefId();
  }

  return BRepGraph_RefId();
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::RefsView::ChildNode(const BRepGraph_RefId theRef) const
{
  if (!theRef.IsValid())
  {
    return BRepGraph_NodeId();
  }

  switch (theRef.RefKind)
  {
    case BRepGraph_RefId::Kind::Shell:
      return Shells().Entry(BRepGraph_ShellRefId::FromRefId(theRef)).ShellDefId;
    case BRepGraph_RefId::Kind::Face:
      return Faces().Entry(BRepGraph_FaceRefId::FromRefId(theRef)).FaceDefId;
    case BRepGraph_RefId::Kind::Wire:
      return Wires().Entry(BRepGraph_WireRefId::FromRefId(theRef)).WireDefId;
    case BRepGraph_RefId::Kind::CoEdge:
      return CoEdges().Entry(BRepGraph_CoEdgeRefId::FromRefId(theRef)).CoEdgeDefId;
    case BRepGraph_RefId::Kind::Vertex:
      return Vertices().Entry(BRepGraph_VertexRefId::FromRefId(theRef)).VertexDefId;
    case BRepGraph_RefId::Kind::Solid:
      return Solids().Entry(BRepGraph_SolidRefId::FromRefId(theRef)).SolidDefId;
    case BRepGraph_RefId::Kind::Child:
      return Children().Entry(BRepGraph_ChildRefId::FromRefId(theRef)).ChildDefId;
    case BRepGraph_RefId::Kind::Occurrence:
      return Occurrences().Entry(BRepGraph_OccurrenceRefId::FromRefId(theRef)).OccurrenceDefId;
  }

  return BRepGraph_NodeId();
}

//=================================================================================================

bool BRepGraph::RefsView::IsRemoved(const BRepGraph_RefId theRef) const
{
  if (!theRef.IsValid())
  {
    return true;
  }

  switch (theRef.RefKind)
  {
    case BRepGraph_RefId::Kind::Shell:
      return Shells().Entry(BRepGraph_ShellRefId::FromRefId(theRef)).IsRemoved;
    case BRepGraph_RefId::Kind::Face:
      return Faces().Entry(BRepGraph_FaceRefId::FromRefId(theRef)).IsRemoved;
    case BRepGraph_RefId::Kind::Wire:
      return Wires().Entry(BRepGraph_WireRefId::FromRefId(theRef)).IsRemoved;
    case BRepGraph_RefId::Kind::CoEdge:
      return CoEdges().Entry(BRepGraph_CoEdgeRefId::FromRefId(theRef)).IsRemoved;
    case BRepGraph_RefId::Kind::Vertex:
      return Vertices().Entry(BRepGraph_VertexRefId::FromRefId(theRef)).IsRemoved;
    case BRepGraph_RefId::Kind::Solid:
      return Solids().Entry(BRepGraph_SolidRefId::FromRefId(theRef)).IsRemoved;
    case BRepGraph_RefId::Kind::Child:
      return Children().Entry(BRepGraph_ChildRefId::FromRefId(theRef)).IsRemoved;
    case BRepGraph_RefId::Kind::Occurrence:
      return Occurrences().Entry(BRepGraph_OccurrenceRefId::FromRefId(theRef)).IsRemoved;
  }

  return true;
}

//=================================================================================================

TopLoc_Location BRepGraph::RefsView::LocalLocation(const BRepGraph_RefId theRef) const
{
  if (!theRef.IsValid())
  {
    return TopLoc_Location();
  }

  switch (theRef.RefKind)
  {
    case BRepGraph_RefId::Kind::Shell:
      return Shells().Entry(BRepGraph_ShellRefId::FromRefId(theRef)).LocalLocation;
    case BRepGraph_RefId::Kind::Face:
      return Faces().Entry(BRepGraph_FaceRefId::FromRefId(theRef)).LocalLocation;
    case BRepGraph_RefId::Kind::Wire:
      return Wires().Entry(BRepGraph_WireRefId::FromRefId(theRef)).LocalLocation;
    case BRepGraph_RefId::Kind::CoEdge:
      return CoEdges().Entry(BRepGraph_CoEdgeRefId::FromRefId(theRef)).LocalLocation;
    case BRepGraph_RefId::Kind::Vertex:
      return Vertices().Entry(BRepGraph_VertexRefId::FromRefId(theRef)).LocalLocation;
    case BRepGraph_RefId::Kind::Solid:
      return Solids().Entry(BRepGraph_SolidRefId::FromRefId(theRef)).LocalLocation;
    case BRepGraph_RefId::Kind::Child:
      return Children().Entry(BRepGraph_ChildRefId::FromRefId(theRef)).LocalLocation;
    case BRepGraph_RefId::Kind::Occurrence:
      return TopLoc_Location();
  }

  return TopLoc_Location();
}

//=================================================================================================

TopAbs_Orientation BRepGraph::RefsView::Orientation(const BRepGraph_RefId theRef) const
{
  if (!theRef.IsValid())
  {
    return TopAbs_FORWARD;
  }

  switch (theRef.RefKind)
  {
    case BRepGraph_RefId::Kind::Shell:
      return Shells().Entry(BRepGraph_ShellRefId::FromRefId(theRef)).Orientation;
    case BRepGraph_RefId::Kind::Face:
      return Faces().Entry(BRepGraph_FaceRefId::FromRefId(theRef)).Orientation;
    case BRepGraph_RefId::Kind::Wire:
      return Wires().Entry(BRepGraph_WireRefId::FromRefId(theRef)).Orientation;
    case BRepGraph_RefId::Kind::CoEdge:
      return TopAbs_FORWARD;
    case BRepGraph_RefId::Kind::Vertex:
      return Vertices().Entry(BRepGraph_VertexRefId::FromRefId(theRef)).Orientation;
    case BRepGraph_RefId::Kind::Solid:
      return Solids().Entry(BRepGraph_SolidRefId::FromRefId(theRef)).Orientation;
    case BRepGraph_RefId::Kind::Child:
      return Children().Entry(BRepGraph_ChildRefId::FromRefId(theRef)).Orientation;
    case BRepGraph_RefId::Kind::Occurrence:
      return TopAbs_FORWARD;
  }

  return TopAbs_FORWARD;
}

//=================================================================================================

const NCollection_DynamicArray<BRepGraph_SolidRefId>& BRepGraph::RefsView::SolidOps::IdsOf(
  const BRepGraph_CompSolidId theCompSolid) const
{
  static const NCollection_DynamicArray<BRepGraph_SolidRefId> anEmpty;
  if (!theCompSolid.IsValid(myGraph->myData->myIncStorage.NbCompSolids()))
    return anEmpty;
  return myGraph->myData->myIncStorage.CompSolid(theCompSolid).SolidRefIds;
}

//=================================================================================================
