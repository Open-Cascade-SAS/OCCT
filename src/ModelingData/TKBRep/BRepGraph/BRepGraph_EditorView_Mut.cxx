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

#include <BRepGraph_EditorView.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_Data.hxx>

#include <Standard_ProgramError.hxx>

namespace
{

static bool isNodeIndexInRange(const BRepGraphInc_Storage& theStorage,
                               const BRepGraph_NodeId      theNode)
{
  if (!theNode.IsValid())
  {
    return false;
  }

  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      return BRepGraph_VertexId(theNode).IsValid(theStorage.NbVertices());
    case BRepGraph_NodeId::Kind::Edge:
      return BRepGraph_EdgeId(theNode).IsValid(theStorage.NbEdges());
    case BRepGraph_NodeId::Kind::CoEdge:
      return BRepGraph_CoEdgeId(theNode).IsValid(theStorage.NbCoEdges());
    case BRepGraph_NodeId::Kind::Wire:
      return BRepGraph_WireId(theNode).IsValid(theStorage.NbWires());
    case BRepGraph_NodeId::Kind::Face:
      return BRepGraph_FaceId(theNode).IsValid(theStorage.NbFaces());
    case BRepGraph_NodeId::Kind::Shell:
      return BRepGraph_ShellId(theNode).IsValid(theStorage.NbShells());
    case BRepGraph_NodeId::Kind::Solid:
      return BRepGraph_SolidId(theNode).IsValid(theStorage.NbSolids());
    case BRepGraph_NodeId::Kind::Compound:
      return BRepGraph_CompoundId(theNode).IsValid(theStorage.NbCompounds());
    case BRepGraph_NodeId::Kind::CompSolid:
      return BRepGraph_CompSolidId(theNode).IsValid(theStorage.NbCompSolids());
    case BRepGraph_NodeId::Kind::Product:
      return BRepGraph_ProductId(theNode).IsValid(theStorage.NbProducts());
    case BRepGraph_NodeId::Kind::Occurrence:
      return BRepGraph_OccurrenceId(theNode).IsValid(theStorage.NbOccurrences());
  }

  return false;
}

//==================================================================================================

static const BRepGraphInc::BaseDef* topoEntity(const BRepGraphInc_Storage& theStorage,
                                               const BRepGraph_NodeId      theNode)
{
  if (!isNodeIndexInRange(theStorage, theNode))
  {
    return nullptr;
  }

  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      return &theStorage.Vertex(BRepGraph_VertexId(theNode));
    case BRepGraph_NodeId::Kind::Edge:
      return &theStorage.Edge(BRepGraph_EdgeId(theNode));
    case BRepGraph_NodeId::Kind::CoEdge:
      return &theStorage.CoEdge(BRepGraph_CoEdgeId(theNode));
    case BRepGraph_NodeId::Kind::Wire:
      return &theStorage.Wire(BRepGraph_WireId(theNode));
    case BRepGraph_NodeId::Kind::Face:
      return &theStorage.Face(BRepGraph_FaceId(theNode));
    case BRepGraph_NodeId::Kind::Shell:
      return &theStorage.Shell(BRepGraph_ShellId(theNode));
    case BRepGraph_NodeId::Kind::Solid:
      return &theStorage.Solid(BRepGraph_SolidId(theNode));
    case BRepGraph_NodeId::Kind::Compound:
      return &theStorage.Compound(BRepGraph_CompoundId(theNode));
    case BRepGraph_NodeId::Kind::CompSolid:
      return &theStorage.CompSolid(BRepGraph_CompSolidId(theNode));
    case BRepGraph_NodeId::Kind::Product:
      return &theStorage.Product(BRepGraph_ProductId(theNode));
    case BRepGraph_NodeId::Kind::Occurrence:
      return &theStorage.Occurrence(BRepGraph_OccurrenceId(theNode));
  }

  return nullptr;
}

//==================================================================================================

static bool isRefIndexInRange(const BRepGraphInc_Storage& theStorage,
                              const BRepGraph_RefId       theRefId)
{
  if (!theRefId.IsValid())
  {
    return false;
  }

  switch (theRefId.RefKind)
  {
    case BRepGraph_RefId::Kind::Shell:
      return theRefId.IsValid(theStorage.NbShellRefs());
    case BRepGraph_RefId::Kind::Face:
      return theRefId.IsValid(theStorage.NbFaceRefs());
    case BRepGraph_RefId::Kind::Wire:
      return theRefId.IsValid(theStorage.NbWireRefs());
    case BRepGraph_RefId::Kind::CoEdge:
      return theRefId.IsValid(theStorage.NbCoEdgeRefs());
    case BRepGraph_RefId::Kind::Vertex:
      return theRefId.IsValid(theStorage.NbVertexRefs());
    case BRepGraph_RefId::Kind::Solid:
      return theRefId.IsValid(theStorage.NbSolidRefs());
    case BRepGraph_RefId::Kind::Child:
      return theRefId.IsValid(theStorage.NbChildRefs());
    case BRepGraph_RefId::Kind::Occurrence:
      return theRefId.IsValid(theStorage.NbOccurrenceRefs());
  }

  return false;
}

//==================================================================================================

static bool isRepIndexInRange(const BRepGraphInc_Storage& theStorage,
                              const BRepGraph_RepId       theRepId)
{
  if (!theRepId.IsValid())
  {
    return false;
  }

  switch (theRepId.RepKind)
  {
    case BRepGraph_RepId::Kind::Surface:
      return theRepId.IsValid(theStorage.NbSurfaces());
    case BRepGraph_RepId::Kind::Curve3D:
      return theRepId.IsValid(theStorage.NbCurves3D());
    case BRepGraph_RepId::Kind::Curve2D:
      return theRepId.IsValid(theStorage.NbCurves2D());
    case BRepGraph_RepId::Kind::Triangulation:
      return theRepId.IsValid(theStorage.NbTriangulations());
    case BRepGraph_RepId::Kind::Polygon3D:
      return theRepId.IsValid(theStorage.NbPolygons3D());
    case BRepGraph_RepId::Kind::Polygon2D:
      return theRepId.IsValid(theStorage.NbPolygons2D());
    case BRepGraph_RepId::Kind::PolygonOnTri:
      return theRepId.IsValid(theStorage.NbPolygonsOnTri());
  }

  return false;
}

//==================================================================================================

static bool isActiveNode(const BRepGraphInc_Storage& theStorage, const BRepGraph_NodeId theNode)
{
  const BRepGraphInc::BaseDef* aDef = topoEntity(theStorage, theNode);
  return aDef != nullptr && !aDef->IsRemoved;
}

//==================================================================================================

static bool isActiveRef(const BRepGraphInc_Storage& theStorage, const BRepGraph_RefId theRefId)
{
  if (!isRefIndexInRange(theStorage, theRefId))
  {
    return false;
  }

  switch (theRefId.RefKind)
  {
    case BRepGraph_RefId::Kind::Shell:
      return !theStorage.ShellRef(BRepGraph_ShellRefId(theRefId)).IsRemoved;
    case BRepGraph_RefId::Kind::Face:
      return !theStorage.FaceRef(BRepGraph_FaceRefId(theRefId)).IsRemoved;
    case BRepGraph_RefId::Kind::Wire:
      return !theStorage.WireRef(BRepGraph_WireRefId(theRefId)).IsRemoved;
    case BRepGraph_RefId::Kind::CoEdge:
      return !theStorage.CoEdgeRef(BRepGraph_CoEdgeRefId(theRefId)).IsRemoved;
    case BRepGraph_RefId::Kind::Vertex:
      return !theStorage.VertexRef(BRepGraph_VertexRefId(theRefId)).IsRemoved;
    case BRepGraph_RefId::Kind::Solid:
      return !theStorage.SolidRef(BRepGraph_SolidRefId(theRefId)).IsRemoved;
    case BRepGraph_RefId::Kind::Child:
      return !theStorage.ChildRef(BRepGraph_ChildRefId(theRefId)).IsRemoved;
    case BRepGraph_RefId::Kind::Occurrence:
      return !theStorage.OccurrenceRef(BRepGraph_OccurrenceRefId(theRefId)).IsRemoved;
  }

  return false;
}

//==================================================================================================

static bool isActiveRep(const BRepGraphInc_Storage& theStorage, const BRepGraph_RepId theRepId)
{
  if (!isRepIndexInRange(theStorage, theRepId))
  {
    return false;
  }

  switch (theRepId.RepKind)
  {
    case BRepGraph_RepId::Kind::Surface:
      return !theStorage.SurfaceRep(BRepGraph_SurfaceRepId(theRepId)).IsRemoved;
    case BRepGraph_RepId::Kind::Curve3D:
      return !theStorage.Curve3DRep(BRepGraph_Curve3DRepId(theRepId)).IsRemoved;
    case BRepGraph_RepId::Kind::Curve2D:
      return !theStorage.Curve2DRep(BRepGraph_Curve2DRepId(theRepId)).IsRemoved;
    case BRepGraph_RepId::Kind::Triangulation:
      return !theStorage.TriangulationRep(BRepGraph_TriangulationRepId(theRepId)).IsRemoved;
    case BRepGraph_RepId::Kind::Polygon3D:
      return !theStorage.Polygon3DRep(BRepGraph_Polygon3DRepId(theRepId)).IsRemoved;
    case BRepGraph_RepId::Kind::Polygon2D:
      return !theStorage.Polygon2DRep(BRepGraph_Polygon2DRepId(theRepId)).IsRemoved;
    case BRepGraph_RepId::Kind::PolygonOnTri:
      return !theStorage.PolygonOnTriRep(BRepGraph_PolygonOnTriRepId(theRepId)).IsRemoved;
  }

  return false;
}

//==================================================================================================

static void validateMutableNodeId(const BRepGraphInc_Storage& theStorage [[maybe_unused]],
                                  const BRepGraph_NodeId      theNodeId [[maybe_unused]])
{
  Standard_ProgramError_Raise_if(!isActiveNode(theStorage, theNodeId),
                                 "BRepGraph::EditorView::Mut*(): invalid node id");
}

//==================================================================================================

static void validateMutableRefId(const BRepGraphInc_Storage& theStorage [[maybe_unused]],
                                 const BRepGraph_RefId       theRefId [[maybe_unused]])
{
  Standard_ProgramError_Raise_if(!isActiveRef(theStorage, theRefId),
                                 "BRepGraph::EditorView::Mut*(): invalid reference id");
}

//==================================================================================================

static void validateMutableRepId(const BRepGraphInc_Storage& theStorage [[maybe_unused]],
                                 const BRepGraph_RepId       theRepId [[maybe_unused]])
{
  Standard_ProgramError_Raise_if(!isActiveRep(theStorage, theRepId),
                                 "BRepGraph::EditorView::Mut*(): invalid representation id");
}

} // namespace

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::VertexDef> BRepGraph::EditorView::VertexOps::Mut(
  const BRepGraph_VertexId theVertex)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableNodeId(aStorage, BRepGraph_NodeId(theVertex));
  return BRepGraph_MutGuard<BRepGraphInc::VertexDef>(myGraph,
                                                     &aStorage.ChangeVertex(theVertex),
                                                     theVertex);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::EdgeDef> BRepGraph::EditorView::EdgeOps::Mut(
  const BRepGraph_EdgeId theEdge)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableNodeId(aStorage, BRepGraph_NodeId(theEdge));
  return BRepGraph_MutGuard<BRepGraphInc::EdgeDef>(myGraph, &aStorage.ChangeEdge(theEdge), theEdge);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::WireDef> BRepGraph::EditorView::WireOps::Mut(
  const BRepGraph_WireId theWire)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableNodeId(aStorage, BRepGraph_NodeId(theWire));
  return BRepGraph_MutGuard<BRepGraphInc::WireDef>(myGraph, &aStorage.ChangeWire(theWire), theWire);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::FaceDef> BRepGraph::EditorView::FaceOps::Mut(
  const BRepGraph_FaceId theFace)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableNodeId(aStorage, BRepGraph_NodeId(theFace));
  return BRepGraph_MutGuard<BRepGraphInc::FaceDef>(myGraph, &aStorage.ChangeFace(theFace), theFace);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::ShellDef> BRepGraph::EditorView::ShellOps::Mut(
  const BRepGraph_ShellId theShell)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableNodeId(aStorage, BRepGraph_NodeId(theShell));
  return BRepGraph_MutGuard<BRepGraphInc::ShellDef>(myGraph,
                                                    &aStorage.ChangeShell(theShell),
                                                    theShell);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::SolidDef> BRepGraph::EditorView::SolidOps::Mut(
  const BRepGraph_SolidId theSolid)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableNodeId(aStorage, BRepGraph_NodeId(theSolid));
  return BRepGraph_MutGuard<BRepGraphInc::SolidDef>(myGraph,
                                                    &aStorage.ChangeSolid(theSolid),
                                                    theSolid);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::CompoundDef> BRepGraph::EditorView::CompoundOps::Mut(
  const BRepGraph_CompoundId theCompound)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableNodeId(aStorage, BRepGraph_NodeId(theCompound));
  return BRepGraph_MutGuard<BRepGraphInc::CompoundDef>(myGraph,
                                                       &aStorage.ChangeCompound(theCompound),
                                                       theCompound);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::CompSolidDef> BRepGraph::EditorView::CompSolidOps::Mut(
  const BRepGraph_CompSolidId theCompSolid)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableNodeId(aStorage, BRepGraph_NodeId(theCompSolid));
  return BRepGraph_MutGuard<BRepGraphInc::CompSolidDef>(myGraph,
                                                        &aStorage.ChangeCompSolid(theCompSolid),
                                                        theCompSolid);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef> BRepGraph::EditorView::CoEdgeOps::Mut(
  const BRepGraph_CoEdgeId theCoEdge)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableNodeId(aStorage, BRepGraph_NodeId(theCoEdge));
  return BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef>(myGraph,
                                                     &aStorage.ChangeCoEdge(theCoEdge),
                                                     theCoEdge);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::ProductDef> BRepGraph::EditorView::ProductOps::Mut(
  const BRepGraph_ProductId theProduct)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableNodeId(aStorage, BRepGraph_NodeId(theProduct));
  return BRepGraph_MutGuard<BRepGraphInc::ProductDef>(myGraph,
                                                      &aStorage.ChangeProduct(theProduct),
                                                      theProduct);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::OccurrenceDef> BRepGraph::EditorView::ProductOps::MutOccurrence(
  const BRepGraph_OccurrenceId theOccurrence)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableNodeId(aStorage, BRepGraph_NodeId(theOccurrence));
  return BRepGraph_MutGuard<BRepGraphInc::OccurrenceDef>(myGraph,
                                                         &aStorage.ChangeOccurrence(theOccurrence),
                                                         theOccurrence);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::VertexRef> BRepGraph::EditorView::VertexOps::MutRef(
  const BRepGraph_VertexRefId theVertexRef)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableRefId(aStorage, BRepGraph_RefId(theVertexRef));
  return BRepGraph_MutGuard<BRepGraphInc::VertexRef>(myGraph,
                                                     &aStorage.ChangeVertexRef(theVertexRef),
                                                     theVertexRef);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::ShellRef> BRepGraph::EditorView::ShellOps::MutRef(
  const BRepGraph_ShellRefId theShellRef)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableRefId(aStorage, BRepGraph_RefId(theShellRef));
  return BRepGraph_MutGuard<BRepGraphInc::ShellRef>(myGraph,
                                                    &aStorage.ChangeShellRef(theShellRef),
                                                    theShellRef);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::FaceRef> BRepGraph::EditorView::FaceOps::MutRef(
  const BRepGraph_FaceRefId theFaceRef)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableRefId(aStorage, BRepGraph_RefId(theFaceRef));
  return BRepGraph_MutGuard<BRepGraphInc::FaceRef>(myGraph,
                                                   &aStorage.ChangeFaceRef(theFaceRef),
                                                   theFaceRef);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::WireRef> BRepGraph::EditorView::WireOps::MutRef(
  const BRepGraph_WireRefId theWireRef)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableRefId(aStorage, BRepGraph_RefId(theWireRef));
  return BRepGraph_MutGuard<BRepGraphInc::WireRef>(myGraph,
                                                   &aStorage.ChangeWireRef(theWireRef),
                                                   theWireRef);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::CoEdgeRef> BRepGraph::EditorView::CoEdgeOps::MutRef(
  const BRepGraph_CoEdgeRefId theCoEdgeRef)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableRefId(aStorage, BRepGraph_RefId(theCoEdgeRef));
  return BRepGraph_MutGuard<BRepGraphInc::CoEdgeRef>(myGraph,
                                                     &aStorage.ChangeCoEdgeRef(theCoEdgeRef),
                                                     theCoEdgeRef);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::SolidRef> BRepGraph::EditorView::SolidOps::MutRef(
  const BRepGraph_SolidRefId theSolidRef)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableRefId(aStorage, BRepGraph_RefId(theSolidRef));
  return BRepGraph_MutGuard<BRepGraphInc::SolidRef>(myGraph,
                                                    &aStorage.ChangeSolidRef(theSolidRef),
                                                    theSolidRef);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::ChildRef> BRepGraph::EditorView::GenOps::MutChildRef(
  const BRepGraph_ChildRefId theChildRef)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableRefId(aStorage, BRepGraph_RefId(theChildRef));
  return BRepGraph_MutGuard<BRepGraphInc::ChildRef>(myGraph,
                                                    &aStorage.ChangeChildRef(theChildRef),
                                                    theChildRef);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::OccurrenceRef> BRepGraph::EditorView::ProductOps::MutOccurrenceRef(
  const BRepGraph_OccurrenceRefId theOccurrenceRef)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableRefId(aStorage, BRepGraph_RefId(theOccurrenceRef));
  return BRepGraph_MutGuard<BRepGraphInc::OccurrenceRef>(
    myGraph,
    &aStorage.ChangeOccurrenceRef(theOccurrenceRef),
    theOccurrenceRef);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::SurfaceRep> BRepGraph::EditorView::RepOps::MutSurface(
  const BRepGraph_SurfaceRepId theSurface)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableRepId(aStorage, BRepGraph_RepId(theSurface));
  return BRepGraph_MutGuard<BRepGraphInc::SurfaceRep>(myGraph,
                                                      &aStorage.ChangeSurfaceRep(theSurface),
                                                      theSurface);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::Curve3DRep> BRepGraph::EditorView::RepOps::MutCurve3D(
  const BRepGraph_Curve3DRepId theCurve)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableRepId(aStorage, BRepGraph_RepId(theCurve));
  return BRepGraph_MutGuard<BRepGraphInc::Curve3DRep>(myGraph,
                                                      &aStorage.ChangeCurve3DRep(theCurve),
                                                      theCurve);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::Curve2DRep> BRepGraph::EditorView::RepOps::MutCurve2D(
  const BRepGraph_Curve2DRepId theCurve)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableRepId(aStorage, BRepGraph_RepId(theCurve));
  return BRepGraph_MutGuard<BRepGraphInc::Curve2DRep>(myGraph,
                                                      &aStorage.ChangeCurve2DRep(theCurve),
                                                      theCurve);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::TriangulationRep> BRepGraph::EditorView::RepOps::MutTriangulation(
  const BRepGraph_TriangulationRepId theTriangulation)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableRepId(aStorage, BRepGraph_RepId(theTriangulation));
  return BRepGraph_MutGuard<BRepGraphInc::TriangulationRep>(
    myGraph,
    &aStorage.ChangeTriangulationRep(theTriangulation),
    theTriangulation);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::Polygon3DRep> BRepGraph::EditorView::RepOps::MutPolygon3D(
  const BRepGraph_Polygon3DRepId thePolygon)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableRepId(aStorage, BRepGraph_RepId(thePolygon));
  return BRepGraph_MutGuard<BRepGraphInc::Polygon3DRep>(myGraph,
                                                        &aStorage.ChangePolygon3DRep(thePolygon),
                                                        thePolygon);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::Polygon2DRep> BRepGraph::EditorView::RepOps::MutPolygon2D(
  const BRepGraph_Polygon2DRepId thePolygon)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableRepId(aStorage, BRepGraph_RepId(thePolygon));
  return BRepGraph_MutGuard<BRepGraphInc::Polygon2DRep>(myGraph,
                                                        &aStorage.ChangePolygon2DRep(thePolygon),
                                                        thePolygon);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::PolygonOnTriRep> BRepGraph::EditorView::RepOps::MutPolygonOnTri(
  const BRepGraph_PolygonOnTriRepId thePolygon)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableRepId(aStorage, BRepGraph_RepId(thePolygon));
  return BRepGraph_MutGuard<BRepGraphInc::PolygonOnTriRep>(
    myGraph,
    &aStorage.ChangePolygonOnTriRep(thePolygon),
    thePolygon);
}
