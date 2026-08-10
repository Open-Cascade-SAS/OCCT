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

#include <BRepGraph_TopoView.hxx>

#include <BRepGraph_Data.hxx>
#include <BRepGraph_DefsIterator.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_RefsIterator.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_ReverseIterator.hxx>
#include <BRepGraphInc_Storage.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <Geom2d_Curve.hxx>
#include <NCollection_LocalArray.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Poly_Triangulation.hxx>

namespace
{

template <typename T>
const NCollection_LinearVector<T>& emptyVector()
{
  static const NCollection_LinearVector<T> THE_EMPTY_VECTOR;
  return THE_EMPTY_VECTOR;
}

} // namespace

// ==========================================================================
// Tool-like grouped helpers.
// ==========================================================================

//=================================================================================================

uint32_t BRepGraph::TopoView::FaceOps::Nb() const
{
  return myGraph->myData->myIncStorage.NbFaces();
}

//=================================================================================================

uint32_t BRepGraph::TopoView::FaceOps::NbActive() const
{
  return myGraph->myData->myIncStorage.NbActiveFaces();
}

//=================================================================================================

const BRepGraphInc::FaceDef& BRepGraph::TopoView::FaceOps::Definition(
  const BRepGraph_FaceId theFace) const
{
  return myGraph->myData->myIncStorage.Face(theFace);
}

//=================================================================================================

const BRepGraphInc::FaceRelations& BRepGraph::TopoView::FaceOps::Relations(
  const BRepGraph_FaceId theFace) const
{
  return myGraph->myData->myIncStorage.FaceRelations(theFace);
}

//=================================================================================================

occ::handle<Geom_Surface> BRepGraph::TopoView::FaceOps::Surface(
  const BRepGraph_FaceId theFace) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theFace.IsValid(aStorage.NbFaces()) || aStorage.IsRemoved(theFace))
  {
    return occ::handle<Geom_Surface>();
  }

  const BRepGraphInc::FaceDef& aFace = aStorage.Face(theFace);
  if (!aFace.SurfaceRepId.IsValid(aStorage.NbFaceSurfaces())
      || aStorage.IsRemoved(aFace.SurfaceRepId))
  {
    return occ::handle<Geom_Surface>();
  }
  return aStorage.FaceSurfaceRep(aFace.SurfaceRepId).Surface;
}

//=================================================================================================

occ::handle<Poly_Triangulation> BRepGraph::TopoView::FaceOps::ActiveTriangulation(
  const BRepGraph_FaceId theFace) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theFace.IsValid(aStorage.NbFaces()) || aStorage.IsRemoved(theFace))
  {
    return occ::handle<Poly_Triangulation>();
  }

  const BRepGraphInc::FaceDef& aFace = aStorage.Face(theFace);
  if (!aFace.TriangulationRepId.IsValid(aStorage.NbFaceTriangulations())
      || aStorage.IsRemoved(aFace.TriangulationRepId))
  {
    return occ::handle<Poly_Triangulation>();
  }
  return aStorage.FaceTriangulationRep(aFace.TriangulationRepId).Triangulation;
}

//=================================================================================================

uint32_t BRepGraph::TopoView::EdgeOps::Nb() const
{
  return myGraph->myData->myIncStorage.NbEdges();
}

//=================================================================================================

uint32_t BRepGraph::TopoView::EdgeOps::NbActive() const
{
  return myGraph->myData->myIncStorage.NbActiveEdges();
}

//=================================================================================================

const BRepGraphInc::EdgeDef& BRepGraph::TopoView::EdgeOps::Definition(
  const BRepGraph_EdgeId theEdge) const
{
  return myGraph->myData->myIncStorage.Edge(theEdge);
}

//=================================================================================================

const BRepGraphInc::EdgeRelations& BRepGraph::TopoView::EdgeOps::Relations(
  const BRepGraph_EdgeId theEdge) const
{
  return myGraph->myData->myIncStorage.EdgeRelations(theEdge);
}

//=================================================================================================

uint32_t BRepGraph::TopoView::EdgeOps::NbFaces(const BRepGraph_EdgeId theEdge) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theEdge.IsValid(aStorage.NbEdges()) || aStorage.IsRemoved(theEdge)
      || aStorage.NbActiveFaces() == 0)
  {
    return 0;
  }

  const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdges =
    aStorage.EdgeRelations(theEdge).CoEdgeIds;
  const size_t aNbCoEdges = aCoEdges.Size();
  if (aNbCoEdges == 0)
  {
    return 0;
  }

  // Inline buffer covers edges with up to 32 coedges without heap allocation.
  // Seam edges (2 coedges, 1 face) and typical manifold edges (2 coedges, 2 faces)
  // are the overwhelmingly common cases.
  NCollection_LocalArray<BRepGraph_FaceId, 32> aSeenBuf(32);
  uint32_t                                     aNbSeen = 0;

  for (size_t aIdx = 0; aIdx < aNbCoEdges; ++aIdx)
  {
    const BRepGraph_CoEdgeId aCoEdgeId = aCoEdges.Value(aIdx);
    if (!aCoEdgeId.IsValid(aStorage.NbCoEdges()) || aStorage.IsRemoved(aCoEdgeId))
    {
      continue;
    }

    const BRepGraph_FaceId aFace = aStorage.CoEdge(aCoEdgeId).FaceId;
    if (!aFace.IsValid(aStorage.NbFaces()) || aStorage.IsRemoved(aFace))
    {
      continue;
    }

    // Linear scan over the small seen set (typically 1-2 entries).
    bool aFound = false;
    for (uint32_t aJ = 0; aJ < aNbSeen; ++aJ)
    {
      if (aSeenBuf[aJ] == aFace)
      {
        aFound = true;
        break;
      }
    }
    if (aFound)
    {
      continue;
    }
    if (aNbSeen >= aSeenBuf.Size())
    {
      aSeenBuf.Reallocate(static_cast<size_t>(aSeenBuf.Size() * 1.5));
    }
    aSeenBuf[aNbSeen++] = aFace;
  }

  return aNbSeen;
}

//=================================================================================================

BRepGraph_WiresOfEdge BRepGraph::TopoView::EdgeOps::WiresOf(const BRepGraph_EdgeId theEdge) const
{
  return BRepGraph_WiresOfEdge(*myGraph, theEdge);
}

//=================================================================================================

BRepGraph_WiresOfEdge BRepGraph::TopoView::EdgeOps::WiresOf(const BRepGraph_EdgeId theEdge,
                                                            const uint32_t theStartIndex) const
{
  return BRepGraph_WiresOfEdge(*myGraph, theEdge, theStartIndex);
}

//=================================================================================================

BRepGraph_FacesOfEdge BRepGraph::TopoView::EdgeOps::FacesOf(const BRepGraph_EdgeId theEdge) const
{
  return BRepGraph_FacesOfEdge(*myGraph, theEdge);
}

//=================================================================================================

BRepGraph_FacesOfEdge BRepGraph::TopoView::EdgeOps::FacesOf(const BRepGraph_EdgeId theEdge,
                                                            const uint32_t theStartIndex) const
{
  return BRepGraph_FacesOfEdge(*myGraph, theEdge, theStartIndex);
}

//=================================================================================================

const NCollection_LinearVector<BRepGraph_CoEdgeId>& BRepGraph::TopoView::EdgeOps::CoEdges(
  const BRepGraph_EdgeId theEdge) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  return theEdge.IsValid(aStorage.NbEdges()) ? aStorage.EdgeRelations(theEdge).CoEdgeIds
                                             : emptyVector<BRepGraph_CoEdgeId>();
}

//=================================================================================================

occ::handle<Geom_Curve> BRepGraph::TopoView::EdgeOps::Curve3D(const BRepGraph_EdgeId theEdge) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theEdge.IsValid(aStorage.NbEdges()) || aStorage.IsRemoved(theEdge))
  {
    return occ::handle<Geom_Curve>();
  }

  const BRepGraphInc::EdgeDef& anEdge = aStorage.Edge(theEdge);
  if (!anEdge.Curve3DRepId.IsValid(aStorage.NbEdgeCurves3D())
      || aStorage.IsRemoved(anEdge.Curve3DRepId))
  {
    return occ::handle<Geom_Curve>();
  }
  return aStorage.EdgeCurve3DRep(anEdge.Curve3DRepId).Curve;
}

//=================================================================================================

uint32_t BRepGraph::TopoView::VertexOps::Nb() const
{
  return myGraph->myData->myIncStorage.NbVertices();
}

//=================================================================================================

uint32_t BRepGraph::TopoView::VertexOps::NbActive() const
{
  return myGraph->myData->myIncStorage.NbActiveVertices();
}

//=================================================================================================

const BRepGraphInc::VertexDef& BRepGraph::TopoView::VertexOps::Definition(
  const BRepGraph_VertexId theVertex) const
{
  return myGraph->myData->myIncStorage.Vertex(theVertex);
}

//=================================================================================================

const BRepGraphInc::VertexRelations& BRepGraph::TopoView::VertexOps::Relations(
  const BRepGraph_VertexId theVertex) const
{
  return myGraph->myData->myIncStorage.VertexRelations(theVertex);
}

//=================================================================================================

const NCollection_LinearVector<BRepGraph_EdgeId>& BRepGraph::TopoView::VertexOps::Edges(
  const BRepGraph_VertexId theVertex) const
{
  static const NCollection_LinearVector<BRepGraph_EdgeId> anEmpty;
  const BRepGraphInc_Storage&                             aStorage = myGraph->myData->myIncStorage;
  return theVertex.IsValid(aStorage.NbVertices()) ? aStorage.VertexRelations(theVertex).EdgeIds
                                                  : anEmpty;
}

//=================================================================================================

uint32_t BRepGraph::TopoView::WireOps::Nb() const
{
  return myGraph->myData->myIncStorage.NbWires();
}

//=================================================================================================

uint32_t BRepGraph::TopoView::WireOps::NbActive() const
{
  return myGraph->myData->myIncStorage.NbActiveWires();
}

//=================================================================================================

const BRepGraphInc::WireDef& BRepGraph::TopoView::WireOps::Definition(
  const BRepGraph_WireId theWire) const
{
  return myGraph->myData->myIncStorage.Wire(theWire);
}

//=================================================================================================

const BRepGraphInc::WireRelations& BRepGraph::TopoView::WireOps::Relations(
  const BRepGraph_WireId theWire) const
{
  return myGraph->myData->myIncStorage.WireRelations(theWire);
}

//=================================================================================================

uint32_t BRepGraph::TopoView::ShellOps::Nb() const
{
  return myGraph->myData->myIncStorage.NbShells();
}

//=================================================================================================

uint32_t BRepGraph::TopoView::ShellOps::NbActive() const
{
  return myGraph->myData->myIncStorage.NbActiveShells();
}

//=================================================================================================

const BRepGraphInc::ShellDef& BRepGraph::TopoView::ShellOps::Definition(
  const BRepGraph_ShellId theShell) const
{
  return myGraph->myData->myIncStorage.Shell(theShell);
}

//=================================================================================================

const BRepGraphInc::ShellRelations& BRepGraph::TopoView::ShellOps::Relations(
  const BRepGraph_ShellId theShell) const
{
  return myGraph->myData->myIncStorage.ShellRelations(theShell);
}

//=================================================================================================

uint32_t BRepGraph::TopoView::SolidOps::Nb() const
{
  return myGraph->myData->myIncStorage.NbSolids();
}

//=================================================================================================

uint32_t BRepGraph::TopoView::SolidOps::NbActive() const
{
  return myGraph->myData->myIncStorage.NbActiveSolids();
}

//=================================================================================================

const BRepGraphInc::SolidDef& BRepGraph::TopoView::SolidOps::Definition(
  const BRepGraph_SolidId theSolid) const
{
  return myGraph->myData->myIncStorage.Solid(theSolid);
}

//=================================================================================================

const BRepGraphInc::SolidRelations& BRepGraph::TopoView::SolidOps::Relations(
  const BRepGraph_SolidId theSolid) const
{
  return myGraph->myData->myIncStorage.SolidRelations(theSolid);
}

//=================================================================================================

uint32_t BRepGraph::TopoView::CoEdgeOps::Nb() const
{
  return myGraph->myData->myIncStorage.NbCoEdges();
}

//=================================================================================================

uint32_t BRepGraph::TopoView::CoEdgeOps::NbActive() const
{
  return myGraph->myData->myIncStorage.NbActiveCoEdges();
}

//=================================================================================================

const BRepGraphInc::CoEdgeDef& BRepGraph::TopoView::CoEdgeOps::Definition(
  const BRepGraph_CoEdgeId theCoEdge) const
{
  return myGraph->myData->myIncStorage.CoEdge(theCoEdge);
}

//=================================================================================================

BRepGraph_EdgeId BRepGraph::TopoView::CoEdgeOps::Edge(const BRepGraph_CoEdgeId theCoEdge) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theCoEdge.IsValid(aStorage.NbCoEdges()))
  {
    return BRepGraph_EdgeId();
  }

  if (aStorage.IsRemoved(theCoEdge))
  {
    return BRepGraph_EdgeId();
  }
  const BRepGraphInc::CoEdgeDef& aCoEdge = aStorage.CoEdge(theCoEdge);
  if (!aCoEdge.ChildEdgeId.IsValid(aStorage.NbEdges()))
  {
    return BRepGraph_EdgeId();
  }
  if (aStorage.IsRemoved(aCoEdge.ChildEdgeId))
  {
    return BRepGraph_EdgeId();
  }
  return aCoEdge.ChildEdgeId;
}

//=================================================================================================

BRepGraph_FaceId BRepGraph::TopoView::CoEdgeOps::Face(const BRepGraph_CoEdgeId theCoEdge) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theCoEdge.IsValid(aStorage.NbCoEdges()))
  {
    return BRepGraph_FaceId();
  }

  if (aStorage.IsRemoved(theCoEdge))
  {
    return BRepGraph_FaceId();
  }
  const BRepGraphInc::CoEdgeDef& aCoEdge = aStorage.CoEdge(theCoEdge);
  if (!aCoEdge.FaceId.IsValid(aStorage.NbFaces()))
  {
    return BRepGraph_FaceId();
  }
  if (aStorage.IsRemoved(aCoEdge.FaceId))
  {
    return BRepGraph_FaceId();
  }
  return aCoEdge.FaceId;
}

//=================================================================================================

BRepGraph_WireId BRepGraph::TopoView::CoEdgeOps::Wire(const BRepGraph_CoEdgeId theCoEdge) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theCoEdge.IsValid(aStorage.NbCoEdges()) || aStorage.IsRemoved(theCoEdge))
  {
    return BRepGraph_WireId();
  }

  const BRepGraph_WireId aWireId = aStorage.CoEdge(theCoEdge).ParentWireId;
  if (!aWireId.IsValid(aStorage.NbWires()) || aStorage.IsRemoved(aWireId))
  {
    return BRepGraph_WireId();
  }
  return aWireId;
}

//=================================================================================================

occ::handle<Geom2d_Curve> BRepGraph::TopoView::CoEdgeOps::Curve2D(
  const BRepGraph_CoEdgeId theCoEdge) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theCoEdge.IsValid(aStorage.NbCoEdges()) || aStorage.IsRemoved(theCoEdge))
  {
    return occ::handle<Geom2d_Curve>();
  }

  const BRepGraphInc::CoEdgeDef& aCoEdge = aStorage.CoEdge(theCoEdge);
  if (!aCoEdge.Curve2DRepId.IsValid(aStorage.NbCoEdgeCurves2D())
      || aStorage.IsRemoved(aCoEdge.Curve2DRepId))
  {
    return occ::handle<Geom2d_Curve>();
  }
  return aStorage.CoEdgeCurve2DRep(aCoEdge.Curve2DRepId).Curve;
}

//=================================================================================================

uint32_t BRepGraph::TopoView::CompoundOps::Nb() const
{
  return myGraph->myData->myIncStorage.NbCompounds();
}

//=================================================================================================

uint32_t BRepGraph::TopoView::CompoundOps::NbActive() const
{
  return myGraph->myData->myIncStorage.NbActiveCompounds();
}

//=================================================================================================

const BRepGraphInc::CompoundDef& BRepGraph::TopoView::CompoundOps::Definition(
  const BRepGraph_CompoundId theCompound) const
{
  return myGraph->myData->myIncStorage.Compound(theCompound);
}

//=================================================================================================

const BRepGraphInc::CompoundRelations& BRepGraph::TopoView::CompoundOps::Relations(
  const BRepGraph_CompoundId theCompound) const
{
  return myGraph->myData->myIncStorage.CompoundRelations(theCompound);
}

//=================================================================================================

uint32_t BRepGraph::TopoView::CompSolidOps::Nb() const
{
  return myGraph->myData->myIncStorage.NbCompSolids();
}

//=================================================================================================

uint32_t BRepGraph::TopoView::CompSolidOps::NbActive() const
{
  return myGraph->incStorage().NbActiveCompSolids();
}

//=================================================================================================

const BRepGraphInc::CompSolidDef& BRepGraph::TopoView::CompSolidOps::Definition(
  const BRepGraph_CompSolidId theCompSolid) const
{
  return myGraph->myData->myIncStorage.CompSolid(theCompSolid);
}

//=================================================================================================

const BRepGraphInc::CompSolidRelations& BRepGraph::TopoView::CompSolidOps::Relations(
  const BRepGraph_CompSolidId theCompSolid) const
{
  return myGraph->myData->myIncStorage.CompSolidRelations(theCompSolid);
}

//=================================================================================================

uint32_t BRepGraph::TopoView::ProductOps::Nb() const
{
  return myGraph->myData->myIncStorage.NbProducts();
}

//=================================================================================================

uint32_t BRepGraph::TopoView::ProductOps::NbActive() const
{
  return myGraph->myData->myIncStorage.NbActiveProducts();
}

//=================================================================================================

const BRepGraphInc::ProductDef& BRepGraph::TopoView::ProductOps::Definition(
  const BRepGraph_ProductId theProduct) const
{
  return myGraph->myData->myIncStorage.Product(theProduct);
}

//=================================================================================================

const BRepGraphInc::ProductRelations& BRepGraph::TopoView::ProductOps::Relations(
  const BRepGraph_ProductId theProduct) const
{
  return myGraph->myData->myIncStorage.ProductRelations(theProduct);
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::TopoView::ProductOps::ShapeRoot(
  const BRepGraph_ProductId theProduct) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theProduct.IsValid(aStorage.NbProducts()))
  {
    return BRepGraph_NodeId();
  }

  if (aStorage.IsRemoved(theProduct))
  {
    return BRepGraph_NodeId();
  }
  // Scan occurrences to find the first with a topology ChildNodeId.
  for (const BRepGraph_OccurrenceRefId& aRefId :
       aStorage.ProductRelations(theProduct).OccurrenceRefIds)
  {
    const BRepGraphInc::OccurrenceRef& aRef = aStorage.OccurrenceRef(aRefId);
    if (aStorage.IsRemoved(aRefId))
    {
      continue;
    }
    const BRepGraphInc::OccurrenceDef& anOccDef = aStorage.Occurrence(aRef.ChildOccurrenceId);
    if (aStorage.IsRemoved(aRef.ChildOccurrenceId))
    {
      continue;
    }
    if (!anOccDef.ChildNodeId.IsValid()
        || BRepGraph_NodeId::IsAssemblyKind(anOccDef.ChildNodeId.NodeKind))
    {
      continue;
    }

    const BRepGraphInc::BaseDef* aRoot = myGraph->Topo().Gen().TopoEntity(anOccDef.ChildNodeId);
    if (aRoot != nullptr && !anOccDef.ChildNodeId.IsRemoved(*myGraph))
    {
      return anOccDef.ChildNodeId;
    }
  }
  return BRepGraph_NodeId();
}

bool BRepGraph::TopoView::ProductOps::IsAssembly(const BRepGraph_ProductId theProduct) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theProduct.IsValid(aStorage.NbProducts()))
  {
    return false;
  }

  if (aStorage.IsRemoved(theProduct))
  {
    return false;
  }
  // Assembly if any active occurrence references a product child.
  for (const BRepGraph_OccurrenceRefId& aRefId :
       aStorage.ProductRelations(theProduct).OccurrenceRefIds)
  {
    const BRepGraphInc::OccurrenceRef& aRef = aStorage.OccurrenceRef(aRefId);
    if (aStorage.IsRemoved(aRefId))
    {
      continue;
    }
    const BRepGraphInc::OccurrenceDef& anOccDef = aStorage.Occurrence(aRef.ChildOccurrenceId);
    if (aStorage.IsRemoved(aRef.ChildOccurrenceId))
    {
      continue;
    }
    if (anOccDef.ChildNodeId.NodeKind == BRepGraph_NodeId::Kind::Product)
    {
      return true;
    }
  }
  return false;
}

//=================================================================================================

bool BRepGraph::TopoView::ProductOps::IsPart(const BRepGraph_ProductId theProduct) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theProduct.IsValid(aStorage.NbProducts()))
  {
    return false;
  }

  if (aStorage.IsRemoved(theProduct))
  {
    return false;
  }
  // Part if any active occurrence references a topology child.
  for (const BRepGraph_OccurrenceRefId& aRefId :
       aStorage.ProductRelations(theProduct).OccurrenceRefIds)
  {
    const BRepGraphInc::OccurrenceRef& aRef = aStorage.OccurrenceRef(aRefId);
    if (aStorage.IsRemoved(aRefId))
    {
      continue;
    }
    const BRepGraphInc::OccurrenceDef& anOccDef = aStorage.Occurrence(aRef.ChildOccurrenceId);
    if (aStorage.IsRemoved(aRef.ChildOccurrenceId))
    {
      continue;
    }
    if (anOccDef.ChildNodeId.IsValid()
        && !BRepGraph_NodeId::IsAssemblyKind(anOccDef.ChildNodeId.NodeKind))
    {
      return true;
    }
  }
  return false;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::TopoView::ProductOps::ShapeRootNode(
  const BRepGraph_ProductId theProduct) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theProduct.IsValid(aStorage.NbProducts()))
  {
    return BRepGraph_NodeId();
  }

  if (aStorage.IsRemoved(theProduct))
  {
    return BRepGraph_NodeId();
  }
  // Find the first occurrence with a topology ChildNodeId.
  for (const BRepGraph_OccurrenceRefId& aRefId :
       aStorage.ProductRelations(theProduct).OccurrenceRefIds)
  {
    const BRepGraphInc::OccurrenceRef& aRef = aStorage.OccurrenceRef(aRefId);
    if (aStorage.IsRemoved(aRefId))
    {
      continue;
    }
    const BRepGraphInc::OccurrenceDef& anOccDef = aStorage.Occurrence(aRef.ChildOccurrenceId);
    if (aStorage.IsRemoved(aRef.ChildOccurrenceId))
    {
      continue;
    }
    if (anOccDef.ChildNodeId.IsValid()
        && !BRepGraph_NodeId::IsAssemblyKind(anOccDef.ChildNodeId.NodeKind))
    {
      return anOccDef.ChildNodeId;
    }
  }
  return BRepGraph_NodeId();
}

//=================================================================================================

uint32_t BRepGraph::TopoView::ProductOps::NbComponents(const BRepGraph_ProductId theProduct) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theProduct.IsValid(aStorage.NbProducts()))
  {
    return 0;
  }

  if (aStorage.IsRemoved(theProduct))
  {
    return 0;
  }
  int aCount = 0;
  for (BRepGraph_RefsOccurrenceOfProduct anOccIt(*myGraph, theProduct); anOccIt.More();
       anOccIt.Next())
  {
    const BRepGraphInc::OccurrenceRef& anOccRef = aStorage.OccurrenceRef(anOccIt.CurrentId());
    if (!aStorage.IsRemoved(anOccRef.ChildOccurrenceId))
    {
      ++aCount;
    }
  }
  return aCount;
}

//=================================================================================================

BRepGraph_OccurrenceId BRepGraph::TopoView::ProductOps::Component(
  const BRepGraph_ProductId theProduct,
  const int                 theComponentIdx) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theProduct.IsValid(aStorage.NbProducts()) || theComponentIdx < 0)
  {
    return BRepGraph_OccurrenceId();
  }

  if (aStorage.IsRemoved(theProduct))
  {
    return BRepGraph_OccurrenceId();
  }
  int anActiveIndex = 0;
  for (BRepGraph_RefsOccurrenceOfProduct anOccIt(*myGraph, theProduct); anOccIt.More();
       anOccIt.Next())
  {
    const BRepGraphInc::OccurrenceRef& anOccRef = aStorage.OccurrenceRef(anOccIt.CurrentId());
    if (aStorage.IsRemoved(anOccRef.ChildOccurrenceId))
    {
      continue;
    }

    if (anActiveIndex == theComponentIdx)
    {
      return anOccRef.ChildOccurrenceId;
    }
    ++anActiveIndex;
  }
  return BRepGraph_OccurrenceId();
}

//=================================================================================================

uint32_t BRepGraph::TopoView::OccurrenceOps::Nb() const
{
  return myGraph->myData->myIncStorage.NbOccurrences();
}

//=================================================================================================

uint32_t BRepGraph::TopoView::OccurrenceOps::NbActive() const
{
  return myGraph->myData->myIncStorage.NbActiveOccurrences();
}

//=================================================================================================

const BRepGraphInc::OccurrenceDef& BRepGraph::TopoView::OccurrenceOps::Definition(
  const BRepGraph_OccurrenceId theOccurrence) const
{
  return myGraph->myData->myIncStorage.Occurrence(theOccurrence);
}

//=================================================================================================

const BRepGraphInc::OccurrenceRelations& BRepGraph::TopoView::OccurrenceOps::Relations(
  const BRepGraph_OccurrenceId theOccurrence) const
{
  return myGraph->myData->myIncStorage.OccurrenceRelations(theOccurrence);
}

//=================================================================================================

BRepGraph_ProductId BRepGraph::TopoView::OccurrenceOps::Product(
  const BRepGraph_OccurrenceId theOccurrence) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theOccurrence.IsValid(aStorage.NbOccurrences()))
  {
    return BRepGraph_ProductId();
  }

  if (aStorage.IsRemoved(theOccurrence))
  {
    return BRepGraph_ProductId();
  }
  const BRepGraphInc::OccurrenceDef& anOccurrence = aStorage.Occurrence(theOccurrence);
  if (!anOccurrence.ChildNodeId.IsValid()
      || anOccurrence.ChildNodeId.NodeKind != BRepGraph_NodeId::Kind::Product)
  {
    return BRepGraph_ProductId();
  }
  const BRepGraph_ProductId aProductId = BRepGraph_ProductId::FromNodeId(anOccurrence.ChildNodeId);
  if (!aProductId.IsValid(aStorage.NbProducts()) || aStorage.IsRemoved(aProductId))
  {
    return BRepGraph_ProductId();
  }
  return aProductId;
}

//=================================================================================================

BRepGraph_ProductId BRepGraph::TopoView::OccurrenceOps::ParentProduct(
  const BRepGraph_OccurrenceId theOccurrence) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theOccurrence.IsValid(aStorage.NbOccurrences()))
  {
    return BRepGraph_ProductId();
  }

  if (aStorage.IsRemoved(theOccurrence))
  {
    return BRepGraph_ProductId();
  }
  for (const BRepGraph_OccurrenceRefId& aRefId :
       aStorage.OccurrenceRelations(theOccurrence).ParentOccurrenceRefIds)
  {
    if (!aRefId.IsValid(aStorage.NbOccurrenceRefs()) || aStorage.IsRemoved(aRefId))
    {
      continue;
    }
    const BRepGraphInc::OccurrenceRef& aRef = aStorage.OccurrenceRef(aRefId);
    if (aRef.ParentProductId.IsValid(aStorage.NbProducts())
        && !aStorage.IsRemoved(aRef.ParentProductId))
    {
      return aRef.ParentProductId;
    }
  }
  return BRepGraph_ProductId();
}

//=================================================================================================

TopLoc_Location BRepGraph::TopoView::OccurrenceOps::OccurrenceLocation(
  const BRepGraph_OccurrenceId theOccurrence) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theOccurrence.IsValid(aStorage.NbOccurrences()))
  {
    return TopLoc_Location();
  }

  for (const BRepGraph_OccurrenceRefId& aRefId :
       aStorage.OccurrenceRelations(theOccurrence).ParentOccurrenceRefIds)
  {
    if (!aRefId.IsValid(aStorage.NbOccurrenceRefs()) || aStorage.IsRemoved(aRefId))
    {
      continue;
    }
    const BRepGraphInc::OccurrenceRef& aRef = aStorage.OccurrenceRef(aRefId);
    return aRef.LocalLocation;
  }
  return TopLoc_Location();
}

//=================================================================================================

uint32_t BRepGraph::TopoView::GeometryOps::NbFaceSurfaces() const
{
  return myGraph->myData->myIncStorage.NbFaceSurfaces();
}

//=================================================================================================

uint32_t BRepGraph::TopoView::GeometryOps::NbEdgeCurves3D() const
{
  return myGraph->myData->myIncStorage.NbEdgeCurves3D();
}

//=================================================================================================

uint32_t BRepGraph::TopoView::GeometryOps::NbCoEdgeCurves2D() const
{
  return myGraph->myData->myIncStorage.NbCoEdgeCurves2D();
}

//=================================================================================================

uint32_t BRepGraph::TopoView::GeometryOps::NbActiveFaceSurfaces() const
{
  return myGraph->myData->myIncStorage.NbActiveFaceSurfaces();
}

//=================================================================================================

uint32_t BRepGraph::TopoView::GeometryOps::NbActiveEdgeCurves3D() const
{
  return myGraph->myData->myIncStorage.NbActiveEdgeCurves3D();
}

//=================================================================================================

uint32_t BRepGraph::TopoView::GeometryOps::NbActiveCoEdgeCurves2D() const
{
  return myGraph->myData->myIncStorage.NbActiveCoEdgeCurves2D();
}

//=================================================================================================

const BRepGraphInc::BaseDef* BRepGraph::TopoView::GenOps::TopoEntity(
  const BRepGraph_NodeId theId) const
{
  return myGraph->topoEntity(theId);
}

//=================================================================================================

const NCollection_LinearVector<BRepGraph_ChildRefId>& BRepGraph::TopoView::GenOps::CompoundRefIds(
  const BRepGraph_NodeId theChild) const
{
  return myGraph->myData->myIncStorage.CompoundRefsOfNode(theChild);
}

//=================================================================================================

const NCollection_LinearVector<BRepGraph_OccurrenceRefId>& BRepGraph::TopoView::GenOps::
  OccurrenceRefIds(const BRepGraph_NodeId theChild) const
{
  return myGraph->myData->myIncStorage.OccurrenceRefsOfNode(theChild);
}

//=================================================================================================

bool BRepGraph::TopoView::GenOps::HasCompoundParents(const BRepGraph_NodeId theNode) const
{
  return myGraph->myData->myIncStorage.HasCompoundParent(theNode);
}

//=================================================================================================

bool BRepGraph::TopoView::GenOps::HasOccurrenceParents(const BRepGraph_NodeId theNode) const
{
  return myGraph->myData->myIncStorage.HasOccurrenceParent(theNode);
}

//=================================================================================================

uint32_t BRepGraph::TopoView::GenOps::NbNodes() const
{
  const BRepGraphInc_Storage& aS = myGraph->myData->myIncStorage;
  return aS.NbSolids() + aS.NbShells() + aS.NbFaces() + aS.NbWires() + aS.NbCoEdges() + aS.NbEdges()
         + aS.NbVertices() + aS.NbCompounds() + aS.NbCompSolids() + aS.NbProducts()
         + aS.NbOccurrences();
}

//=================================================================================================

uint32_t BRepGraph::TopoView::GenOps::Nb(const BRepGraph_NodeId::Kind theKind) const
{
  const BRepGraphInc_Storage& aS = myGraph->myData->myIncStorage;
  switch (theKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      return aS.NbVertices();
    case BRepGraph_NodeId::Kind::Edge:
      return aS.NbEdges();
    case BRepGraph_NodeId::Kind::CoEdge:
      return aS.NbCoEdges();
    case BRepGraph_NodeId::Kind::Wire:
      return aS.NbWires();
    case BRepGraph_NodeId::Kind::Face:
      return aS.NbFaces();
    case BRepGraph_NodeId::Kind::Shell:
      return aS.NbShells();
    case BRepGraph_NodeId::Kind::Solid:
      return aS.NbSolids();
    case BRepGraph_NodeId::Kind::Compound:
      return aS.NbCompounds();
    case BRepGraph_NodeId::Kind::CompSolid:
      return aS.NbCompSolids();
    case BRepGraph_NodeId::Kind::Product:
      return aS.NbProducts();
    case BRepGraph_NodeId::Kind::Occurrence:
      return aS.NbOccurrences();
  }

  return 0;
}

//=================================================================================================

bool BRepGraph::TopoView::GenOps::IsValid(const BRepGraph_NodeId theNode) const
{
  if (!theNode.IsValid())
  {
    return false;
  }
  return theNode.Index < Nb(theNode.NodeKind);
}

//=================================================================================================

bool BRepGraph::TopoView::GenOps::IsActive(const BRepGraph_NodeId theNode) const
{
  return IsValid(theNode) && !theNode.IsRemoved(*myGraph);
}

//=================================================================================================

bool BRepGraph::TopoView::GenOps::IsRemoved(const BRepGraph_NodeId theNode) const
{
  return !IsValid(theNode) || theNode.IsRemoved(*myGraph);
}
