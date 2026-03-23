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

#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraphInc_Storage.hxx>
#include <NCollection_Map.hxx>

#include <Adaptor3d_CurveOnSurface.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>

//=================================================================================================
// Count methods read directly from incidence storage.
// Both Build() and BuilderView::Add*() write to incidence, so counts are always in sync.

int BRepGraph::DefsView::NbSolids() const
{
  return myGraph->myData->myIncStorage.NbSolids();
}

//=================================================================================================

int BRepGraph::DefsView::NbShells() const
{
  return myGraph->myData->myIncStorage.NbShells();
}

//=================================================================================================

int BRepGraph::DefsView::NbFaces() const
{
  return myGraph->myData->myIncStorage.NbFaces();
}

//=================================================================================================

int BRepGraph::DefsView::NbWires() const
{
  return myGraph->myData->myIncStorage.NbWires();
}

//=================================================================================================

int BRepGraph::DefsView::NbEdges() const
{
  return myGraph->myData->myIncStorage.NbEdges();
}

//=================================================================================================

int BRepGraph::DefsView::NbVertices() const
{
  return myGraph->myData->myIncStorage.NbVertices();
}

//=================================================================================================

int BRepGraph::DefsView::NbCompounds() const
{
  return myGraph->myData->myIncStorage.NbCompounds();
}

//=================================================================================================

int BRepGraph::DefsView::NbCompSolids() const
{
  return myGraph->myData->myIncStorage.NbCompSolids();
}

//=================================================================================================

int BRepGraph::DefsView::NbCoEdges() const
{
  return myGraph->myData->myIncStorage.NbCoEdges();
}

//=================================================================================================

int BRepGraph::DefsView::NbActiveVertices() const
{
  return myGraph->myData->myIncStorage.NbActiveVertices();
}

//=================================================================================================

int BRepGraph::DefsView::NbActiveEdges() const
{
  return myGraph->myData->myIncStorage.NbActiveEdges();
}

//=================================================================================================

int BRepGraph::DefsView::NbActiveWires() const
{
  return myGraph->myData->myIncStorage.NbActiveWires();
}

//=================================================================================================

int BRepGraph::DefsView::NbActiveFaces() const
{
  return myGraph->myData->myIncStorage.NbActiveFaces();
}

//=================================================================================================

int BRepGraph::DefsView::NbActiveShells() const
{
  return myGraph->myData->myIncStorage.NbActiveShells();
}

//=================================================================================================

int BRepGraph::DefsView::NbActiveSolids() const
{
  return myGraph->myData->myIncStorage.NbActiveSolids();
}

//=================================================================================================

int BRepGraph::DefsView::NbActiveCompounds() const
{
  return myGraph->myData->myIncStorage.NbActiveCompounds();
}

//=================================================================================================

int BRepGraph::DefsView::NbActiveCompSolids() const
{
  return myGraph->myData->myIncStorage.NbActiveCompSolids();
}

//=================================================================================================

int BRepGraph::DefsView::FaceCountOfEdge(const int theEdgeDefIdx) const
{
  return myGraph->myData->myIncStorage.ReverseIndex().FaceCountOfEdge(theEdgeDefIdx);
}

//=================================================================================================

const NCollection_Vector<int>& BRepGraph::DefsView::CoEdgesOfEdge(const int theEdgeDefIdx) const
{
  return myGraph->myData->myIncStorage.ReverseIndex().CoEdgesOfEdgeRef(theEdgeDefIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::SolidDef& BRepGraph::DefsView::Solid(const int theSolidIdx) const
{
  return myGraph->myData->myIncStorage.Solid(theSolidIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::ShellDef& BRepGraph::DefsView::Shell(const int theShellIdx) const
{
  return myGraph->myData->myIncStorage.Shell(theShellIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::FaceDef& BRepGraph::DefsView::Face(const int theFaceIdx) const
{
  return myGraph->myData->myIncStorage.Face(theFaceIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::WireDef& BRepGraph::DefsView::Wire(const int theWireIdx) const
{
  return myGraph->myData->myIncStorage.Wire(theWireIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::EdgeDef& BRepGraph::DefsView::Edge(const int theEdgeIdx) const
{
  return myGraph->myData->myIncStorage.Edge(theEdgeIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::VertexDef& BRepGraph::DefsView::Vertex(const int theVertexIdx) const
{
  return myGraph->myData->myIncStorage.Vertex(theVertexIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::CompoundDef& BRepGraph::DefsView::Compound(const int theCompoundIdx) const
{
  return myGraph->myData->myIncStorage.Compound(theCompoundIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::CompSolidDef& BRepGraph::DefsView::CompSolid(const int theCompSolidIdx) const
{
  return myGraph->myData->myIncStorage.CompSolid(theCompSolidIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::CoEdgeDef& BRepGraph::DefsView::CoEdge(const int theCoEdgeIdx) const
{
  return myGraph->myData->myIncStorage.CoEdge(theCoEdgeIdx);
}

//=================================================================================================

int BRepGraph::DefsView::NbShellFaces(const int theShellDefIdx) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (theShellDefIdx < 0 || theShellDefIdx >= aStorage.NbShells())
    return 0;
  return aStorage.Shell(theShellDefIdx).FaceRefs.Length();
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::DefsView::ShellFaceDef(const int theShellDefIdx, const int theFaceIdx) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (theShellDefIdx < 0 || theShellDefIdx >= aStorage.NbShells())
    return BRepGraph_NodeId();
  const BRepGraphInc::ShellEntity& aShell = aStorage.Shell(theShellDefIdx);
  if (theFaceIdx < 0 || theFaceIdx >= aShell.FaceRefs.Length())
    return BRepGraph_NodeId();
  return BRepGraph_NodeId::Face(aShell.FaceRefs.Value(theFaceIdx).FaceIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::BaseDef* BRepGraph::DefsView::TopoDef(const BRepGraph_NodeId theId) const
{
  return myGraph->TopoDef(theId);
}

//=================================================================================================

int BRepGraph::DefsView::NbProducts() const
{
  return myGraph->myData->myIncStorage.NbProducts();
}

//=================================================================================================

int BRepGraph::DefsView::NbOccurrences() const
{
  return myGraph->myData->myIncStorage.NbOccurrences();
}

//=================================================================================================

int BRepGraph::DefsView::NbActiveProducts() const
{
  return myGraph->myData->myIncStorage.NbActiveProducts();
}

//=================================================================================================

int BRepGraph::DefsView::NbActiveOccurrences() const
{
  return myGraph->myData->myIncStorage.NbActiveOccurrences();
}

//=================================================================================================

const BRepGraph_TopoNode::ProductDef& BRepGraph::DefsView::Product(const int theProductIdx) const
{
  return myGraph->myData->myIncStorage.Product(theProductIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::OccurrenceDef& BRepGraph::DefsView::Occurrence(const int theOccurrenceIdx) const
{
  return myGraph->myData->myIncStorage.Occurrence(theOccurrenceIdx);
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::DefsView::RootProducts() const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  const int aNbProducts = aStorage.NbProducts();

  // Collect product indices that are referenced by at least one active occurrence.
  // Scan occurrences directly (always up to date, unlike the reverse index which
  // is only rebuilt during BuildReverseIndex).
  NCollection_Map<int> aReferencedProducts;
  for (int anOccIdx = 0; anOccIdx < aStorage.NbOccurrences(); ++anOccIdx)
  {
    const BRepGraphInc::OccurrenceEntity& anOcc = aStorage.Occurrence(anOccIdx);
    if (!anOcc.IsRemoved && anOcc.ProductIdx >= 0)
      aReferencedProducts.Add(anOcc.ProductIdx);
  }

  NCollection_Vector<BRepGraph_NodeId> aRoots;
  for (int aProdIdx = 0; aProdIdx < aNbProducts; ++aProdIdx)
  {
    const BRepGraphInc::ProductEntity& aProduct = aStorage.Product(aProdIdx);
    if (aProduct.IsRemoved)
      continue;
    if (!aReferencedProducts.Contains(aProdIdx))
      aRoots.Append(BRepGraph_NodeId::Product(aProdIdx));
  }
  return aRoots;
}

//=================================================================================================

bool BRepGraph::DefsView::IsAssembly(const int theProductIdx) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (theProductIdx < 0 || theProductIdx >= aStorage.NbProducts())
    return false;
  const BRepGraphInc::ProductEntity& aProduct = aStorage.Product(theProductIdx);
  return !aProduct.ShapeRootId.IsValid();
}

//=================================================================================================

bool BRepGraph::DefsView::IsPart(const int theProductIdx) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (theProductIdx < 0 || theProductIdx >= aStorage.NbProducts())
    return false;
  const BRepGraphInc::ProductEntity& aProduct = aStorage.Product(theProductIdx);
  return aProduct.ShapeRootId.IsValid();
}

//=================================================================================================

int BRepGraph::DefsView::NbComponents(const int theProductIdx) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (theProductIdx < 0 || theProductIdx >= aStorage.NbProducts())
    return 0;
  return aStorage.Product(theProductIdx).OccurrenceRefs.Length();
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::DefsView::Component(const int theProductIdx, const int theComponentIdx) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (theProductIdx < 0 || theProductIdx >= aStorage.NbProducts())
    return BRepGraph_NodeId();
  const BRepGraphInc::ProductEntity& aProduct = aStorage.Product(theProductIdx);
  if (theComponentIdx < 0 || theComponentIdx >= aProduct.OccurrenceRefs.Length())
    return BRepGraph_NodeId();
  return BRepGraph_NodeId::Occurrence(aProduct.OccurrenceRefs.Value(theComponentIdx).OccurrenceIdx);
}

//=================================================================================================

size_t BRepGraph::DefsView::NbNodes() const
{
  const BRepGraphInc_Storage& aS = myGraph->myData->myIncStorage;
  return static_cast<size_t>(aS.NbSolids())
         + static_cast<size_t>(aS.NbShells())
         + static_cast<size_t>(aS.NbFaces())
         + static_cast<size_t>(aS.NbWires())
         + static_cast<size_t>(aS.NbCoEdges())
         + static_cast<size_t>(aS.NbEdges())
         + static_cast<size_t>(aS.NbVertices())
         + static_cast<size_t>(aS.NbCompounds())
         + static_cast<size_t>(aS.NbCompSolids())
         + static_cast<size_t>(aS.NbProducts())
         + static_cast<size_t>(aS.NbOccurrences());
}

//=================================================================================================

int BRepGraph::DefsView::NbSurfaces() const
{
  return myGraph->myData->myIncStorage.NbSurfaces();
}

//=================================================================================================

int BRepGraph::DefsView::NbCurves3D() const
{
  return myGraph->myData->myIncStorage.NbCurves3D();
}

//=================================================================================================

int BRepGraph::DefsView::NbCurves2D() const
{
  return myGraph->myData->myIncStorage.NbCurves2D();
}

//=================================================================================================

int BRepGraph::DefsView::NbTriangulations() const
{
  return myGraph->myData->myIncStorage.NbTriangulations();
}

//=================================================================================================

int BRepGraph::DefsView::NbPolygons3D() const
{
  return myGraph->myData->myIncStorage.NbPolygons3D();
}

//=================================================================================================

const BRepGraphInc::SurfaceRep& BRepGraph::DefsView::SurfaceRep(const int theRepIdx) const
{
  return myGraph->myData->myIncStorage.SurfaceRep(theRepIdx);
}

//=================================================================================================

const BRepGraphInc::Curve3DRep& BRepGraph::DefsView::Curve3DRep(const int theRepIdx) const
{
  return myGraph->myData->myIncStorage.Curve3DRep(theRepIdx);
}

//=================================================================================================

const BRepGraphInc::Curve2DRep& BRepGraph::DefsView::Curve2DRep(const int theRepIdx) const
{
  return myGraph->myData->myIncStorage.Curve2DRep(theRepIdx);
}

//=================================================================================================

const BRepGraphInc::TriangulationRep& BRepGraph::DefsView::TriangulationRep(const int theRepIdx) const
{
  return myGraph->myData->myIncStorage.TriangulationRep(theRepIdx);
}

//=================================================================================================

const BRepGraphInc::Polygon3DRep& BRepGraph::DefsView::Polygon3DRep(const int theRepIdx) const
{
  return myGraph->myData->myIncStorage.Polygon3DRep(theRepIdx);
}

//=================================================================================================

int BRepGraph::DefsView::NbPolygons2D() const
{
  return myGraph->myData->myIncStorage.NbPolygons2D();
}

//=================================================================================================

int BRepGraph::DefsView::NbPolygonsOnTri() const
{
  return myGraph->myData->myIncStorage.NbPolygonsOnTri();
}

//=================================================================================================

const BRepGraphInc::Polygon2DRep& BRepGraph::DefsView::Polygon2DRep(const int theRepIdx) const
{
  return myGraph->myData->myIncStorage.Polygon2DRep(theRepIdx);
}

//=================================================================================================

const BRepGraphInc::PolygonOnTriRep& BRepGraph::DefsView::PolygonOnTriRep(const int theRepIdx) const
{
  return myGraph->myData->myIncStorage.PolygonOnTriRep(theRepIdx);
}

//=================================================================================================

const BRepGraphInc::CoEdgeEntity* BRepGraph::DefsView::FindPCurve(
  const BRepGraph_NodeId theEdgeDef,
  const BRepGraph_NodeId theFaceDef) const
{
  if (theEdgeDef.NodeKind != BRepGraph_NodeId::Kind::Edge || !theEdgeDef.IsValid())
    return nullptr;

  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  const NCollection_Vector<int>& aCoEdgeIdxs =
    aStorage.ReverseIndex().CoEdgesOfEdgeRef(theEdgeDef.Index);
  for (int anIter = 0; anIter < aCoEdgeIdxs.Length(); ++anIter)
  {
    const BRepGraphInc::CoEdgeEntity& aCoEdge = aStorage.CoEdge(aCoEdgeIdxs.Value(anIter));
    if (aCoEdge.EdgeIdx == theEdgeDef.Index && aCoEdge.FaceDefId == theFaceDef)
      return &aCoEdge;
  }
  return nullptr;
}

//=================================================================================================

const BRepGraphInc::CoEdgeEntity* BRepGraph::DefsView::FindPCurve(
  const BRepGraph_NodeId   theEdgeDef,
  const BRepGraph_NodeId   theFaceDef,
  const TopAbs_Orientation theEdgeOrientation) const
{
  if (theEdgeDef.NodeKind != BRepGraph_NodeId::Kind::Edge || !theEdgeDef.IsValid())
    return nullptr;

  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  const NCollection_Vector<int>& aCoEdgeIdxs =
    aStorage.ReverseIndex().CoEdgesOfEdgeRef(theEdgeDef.Index);
  // For non-seam edges (1 CoEdge per face), return the only match.
  // For seam edges (2 CoEdges per face), prefer exact orientation match.
  const BRepGraphInc::CoEdgeEntity* aFirstMatch = nullptr;
  for (int anIter = 0; anIter < aCoEdgeIdxs.Length(); ++anIter)
  {
    const BRepGraphInc::CoEdgeEntity& aCoEdge = aStorage.CoEdge(aCoEdgeIdxs.Value(anIter));
    if (aCoEdge.EdgeIdx != theEdgeDef.Index || aCoEdge.FaceDefId != theFaceDef)
      continue;
    if (aFirstMatch == nullptr)
      aFirstMatch = &aCoEdge;
    if (aCoEdge.Sense == theEdgeOrientation)
      return &aCoEdge;
  }
  return aFirstMatch;
}

//=================================================================================================

const BRepGraphInc::CoEdgeEntity* BRepGraph::DefsView::FindPCurve(
  const BRepGraph_PCurveContext& theContext) const
{
  return FindPCurve(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, theContext.EdgeDefIndex),
                    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, theContext.FaceDefIndex),
                    theContext.Orientation);
}
