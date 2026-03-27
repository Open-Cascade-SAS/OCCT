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

#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraph_Builder.hxx>
#include <BRepGraph_Layer.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_TopoView.hxx>

#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <NCollection_PackedMap.hxx>
#include <Poly_Triangulation.hxx>

#include <shared_mutex>

namespace
{

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> collectCompoundChildren(const BRepGraph&       theGraph,
                                                             const BRepGraph_NodeId theCompoundNodeId)
{
  NCollection_Vector<BRepGraph_NodeId> aChildNodes;
  const BRepGraph::RefsView&           aRefs = theGraph.Refs();
  for (int aRefIdx = 0; aRefIdx < aRefs.NbChildRefs(); ++aRefIdx)
  {
    const BRepGraphInc::ChildRefEntry& aRef = aRefs.Child(BRepGraph_ChildRefId(aRefIdx));
    if (aRef.ParentId == theCompoundNodeId && !aRef.IsRemoved && aRef.ChildDefId.IsValid())
      aChildNodes.Append(aRef.ChildDefId);
  }
  return aChildNodes;
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> collectCompSolidChildren(const BRepGraph&       theGraph,
                                                              const BRepGraph_NodeId theCompSolidNodeId)
{
  NCollection_Vector<BRepGraph_NodeId> aChildNodes;
  const BRepGraph::RefsView&           aRefs = theGraph.Refs();
  const int                            aNbSolids = theGraph.Topo().NbSolids();
  for (int aRefIdx = 0; aRefIdx < aRefs.NbSolidRefs(); ++aRefIdx)
  {
    const BRepGraphInc::SolidRefEntry& aRef = aRefs.Solid(BRepGraph_SolidRefId(aRefIdx));
    if (aRef.ParentId == theCompSolidNodeId && !aRef.IsRemoved && aRef.SolidDefId.IsValid(aNbSolids))
      aChildNodes.Append(BRepGraph_NodeId::Solid(aRef.SolidDefId.Index));
  }
  return aChildNodes;
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> collectSolidChildren(const BRepGraph&       theGraph,
                                                          const BRepGraph_NodeId theSolidNodeId)
{
  NCollection_Vector<BRepGraph_NodeId> aChildNodes;
  const BRepGraph::RefsView&           aRefs = theGraph.Refs();
  const int                            aNbShells = theGraph.Topo().NbShells();
  for (int aRefIdx = 0; aRefIdx < aRefs.NbShellRefs(); ++aRefIdx)
  {
    const BRepGraphInc::ShellRefEntry& aRef = aRefs.Shell(BRepGraph_ShellRefId(aRefIdx));
    if (aRef.ParentId == theSolidNodeId && !aRef.IsRemoved && aRef.ShellDefId.IsValid(aNbShells))
      aChildNodes.Append(BRepGraph_NodeId::Shell(aRef.ShellDefId.Index));
  }
  return aChildNodes;
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> collectShellChildren(const BRepGraph&       theGraph,
                                                          const BRepGraph_NodeId theShellNodeId)
{
  NCollection_Vector<BRepGraph_NodeId> aChildNodes;
  const BRepGraph::RefsView&           aRefs = theGraph.Refs();
  const int                            aNbFaces = theGraph.Topo().NbFaces();
  for (int aRefIdx = 0; aRefIdx < aRefs.NbFaceRefs(); ++aRefIdx)
  {
    const BRepGraphInc::FaceRefEntry& aRef = aRefs.Face(BRepGraph_FaceRefId(aRefIdx));
    if (aRef.ParentId == theShellNodeId && !aRef.IsRemoved && aRef.FaceDefId.IsValid(aNbFaces))
      aChildNodes.Append(BRepGraph_NodeId::Face(aRef.FaceDefId.Index));
  }
  return aChildNodes;
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> collectFaceChildren(const BRepGraph&       theGraph,
                                                         const BRepGraph_NodeId theFaceNodeId)
{
  NCollection_Vector<BRepGraph_NodeId> aChildNodes;
  const BRepGraph::RefsView&           aRefs = theGraph.Refs();
  const int                            aNbWires = theGraph.Topo().NbWires();
  for (int aRefIdx = 0; aRefIdx < aRefs.NbWireRefs(); ++aRefIdx)
  {
    const BRepGraphInc::WireRefEntry& aRef = aRefs.Wire(BRepGraph_WireRefId(aRefIdx));
    if (aRef.ParentId == theFaceNodeId && !aRef.IsRemoved && aRef.WireDefId.IsValid(aNbWires))
      aChildNodes.Append(BRepGraph_NodeId::Wire(aRef.WireDefId.Index));
  }
  return aChildNodes;
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> collectWireChildren(const BRepGraph&       theGraph,
                                                         const BRepGraph_NodeId theWireNodeId)
{
  NCollection_Vector<BRepGraph_NodeId> aChildNodes;
  const BRepGraph::TopoView&           aTopo = theGraph.Topo();
  const BRepGraph::RefsView&           aRefs = theGraph.Refs();
  const int                            aNbCoEdges = aTopo.NbCoEdges();
  for (int aRefIdx = 0; aRefIdx < aRefs.NbCoEdgeRefs(); ++aRefIdx)
  {
    const BRepGraphInc::CoEdgeRefEntry& aRef = aRefs.CoEdge(BRepGraph_CoEdgeRefId(aRefIdx));
    if (aRef.ParentId != theWireNodeId || aRef.IsRemoved || !aRef.CoEdgeDefId.IsValid(aNbCoEdges))
      continue;

    const BRepGraphInc::CoEdgeDef& aCoEdge = aTopo.CoEdge(aRef.CoEdgeDefId);
    if (aCoEdge.EdgeDefId.IsValid(aTopo.NbEdges()))
      aChildNodes.Append(BRepGraph_NodeId::Edge(aCoEdge.EdgeDefId.Index));
  }
  return aChildNodes;
}

} // namespace

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddVertex(const gp_Pnt& thePoint,
                                                      const double  theTolerance)
{
  BRepGraphInc::VertexDef& aVtxDef = myGraph->myData->myIncStorage.AppendVertex();
  const int                      aIdx    = myGraph->myData->myIncStorage.NbVertices() - 1;
  aVtxDef.Id                             = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Vertex, aIdx);
  aVtxDef.Point                          = thePoint;
  aVtxDef.Tolerance                      = theTolerance;
  myGraph->allocateUID(aVtxDef.Id);

  return aVtxDef.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddEdge(const BRepGraph_NodeId         theStartVtx,
                                                    const BRepGraph_NodeId         theEndVtx,
                                                    const occ::handle<Geom_Curve>& theCurve,
                                                    const double                   theFirst,
                                                    const double                   theLast,
                                                    const double                   theTolerance)
{
  BRepGraphInc::EdgeDef& anEdgeDef = myGraph->myData->myIncStorage.AppendEdge();
  const int                    aIdx      = myGraph->myData->myIncStorage.NbEdges() - 1;
  anEdgeDef.Id                           = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, aIdx);
  if (theStartVtx.IsValid())
  {
    BRepGraphInc::VertexRefEntry& aStartVtxRef =
      myGraph->myData->myIncStorage.AppendVertexRefEntry();
    const int aStartVtxRefIdx = myGraph->myData->myIncStorage.NbVertexRefs() - 1;
    aStartVtxRef.RefId         = BRepGraph_RefId::Vertex(aStartVtxRefIdx);
    aStartVtxRef.ParentId      = anEdgeDef.Id;
    aStartVtxRef.VertexDefId   = BRepGraph_VertexId::FromNodeId(theStartVtx);
    aStartVtxRef.Orientation   = TopAbs_FORWARD;
    myGraph->allocateRefUID(aStartVtxRef.RefId);
    anEdgeDef.StartVertexRefId = BRepGraph_VertexRefId(aStartVtxRefIdx);
  }
  if (theEndVtx.IsValid())
  {
    BRepGraphInc::VertexRefEntry& anEndVtxRef =
      myGraph->myData->myIncStorage.AppendVertexRefEntry();
    const int anEndVtxRefIdx = myGraph->myData->myIncStorage.NbVertexRefs() - 1;
    anEndVtxRef.RefId         = BRepGraph_RefId::Vertex(anEndVtxRefIdx);
    anEndVtxRef.ParentId      = anEdgeDef.Id;
    anEndVtxRef.VertexDefId   = BRepGraph_VertexId::FromNodeId(theEndVtx);
    anEndVtxRef.Orientation   = TopAbs_REVERSED;
    myGraph->allocateRefUID(anEndVtxRef.RefId);
    anEdgeDef.EndVertexRefId = BRepGraph_VertexRefId(anEndVtxRefIdx);
  }
  anEdgeDef.ParamFirst            = theFirst;
  anEdgeDef.ParamLast             = theLast;
  anEdgeDef.Tolerance             = theTolerance;
  anEdgeDef.SameParameter         = true;
  anEdgeDef.SameRange             = true;
  myGraph->allocateUID(anEdgeDef.Id);

  if (!theCurve.IsNull())
  {
    BRepGraphInc::Curve3DRep& aCurveRep    = myGraph->myData->myIncStorage.AppendCurve3DRep();
    const int                 aCurveRepIdx = myGraph->myData->myIncStorage.NbCurves3D() - 1;
    aCurveRep.Id                           = BRepGraph_RepId::Curve3D(aCurveRepIdx);
    aCurveRep.Curve                        = theCurve;
    anEdgeDef.Curve3DRepId                 = BRepGraph_Curve3DRepId(aCurveRepIdx);
  }

  return anEdgeDef.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddWire(
  const NCollection_Vector<std::pair<BRepGraph_NodeId, TopAbs_Orientation>>& theEdges)
{
  BRepGraphInc::WireDef& aWireDef = myGraph->myData->myIncStorage.AppendWire();
  const int                    aIdx     = myGraph->myData->myIncStorage.NbWires() - 1;
  aWireDef.Id                           = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Wire, aIdx);
  myGraph->allocateUID(aWireDef.Id);

  for (int anEdgeIdx = 0; anEdgeIdx < theEdges.Length(); ++anEdgeIdx)
  {
    const BRepGraph_NodeId   anEdgeDefId = theEdges.Value(anEdgeIdx).first;
    const TopAbs_Orientation anOri       = theEdges.Value(anEdgeIdx).second;

    // Create CoEdge entity for this edge-wire binding.
    BRepGraphInc::CoEdgeDef& aCoEdge    = myGraph->myData->myIncStorage.AppendCoEdge();
    const int                   aCoEdgeIdx = myGraph->myData->myIncStorage.NbCoEdges() - 1;
    aCoEdge.Id        = BRepGraph_NodeId(BRepGraph_NodeId::Kind::CoEdge, aCoEdgeIdx);
    aCoEdge.EdgeDefId = BRepGraph_EdgeId::FromNodeId(anEdgeDefId);
    aCoEdge.Sense     = anOri;
    myGraph->allocateUID(aCoEdge.Id);

    // CoEdgeRefEntry in ref-table.
    BRepGraphInc::CoEdgeRefEntry& aCoEdgeRefEntry = myGraph->myData->myIncStorage.AppendCoEdgeRefEntry();
    const int                     aCoEdgeRefIdx   = myGraph->myData->myIncStorage.NbCoEdgeRefs() - 1;
    aCoEdgeRefEntry.RefId                       = BRepGraph_RefId::CoEdge(aCoEdgeRefIdx);
    aCoEdgeRefEntry.ParentId                    = BRepGraph_NodeId::Wire(aIdx);
    aCoEdgeRefEntry.CoEdgeDefId                 = BRepGraph_CoEdgeId(aCoEdgeIdx);
    myGraph->allocateRefUID(aCoEdgeRefEntry.RefId);
    myGraph->myData->myIncStorage.ChangeWire(BRepGraph_WireId(aIdx)).CoEdgeRefIds.Append(
      BRepGraph_CoEdgeRefId(aCoEdgeRefIdx));

    myGraph->myData->myIncStorage.ChangeReverseIndex().BindEdgeToWire(aCoEdge.EdgeDefId,
                                                                      BRepGraph_WireId(aIdx));
    myGraph->myData->myIncStorage.ChangeReverseIndex().BindEdgeToCoEdge(
      aCoEdge.EdgeDefId,
      BRepGraph_CoEdgeId(aCoEdgeIdx));
    myGraph->myData->myIncStorage.ChangeReverseIndex().BindCoEdgeToWire(
      BRepGraph_CoEdgeId(aCoEdgeIdx),
      BRepGraph_WireId(aIdx));
  }

  // Check closure.
  if (!theEdges.IsEmpty())
  {
    const BRepGraph_NodeId   aFirstEdgeNodeId = theEdges.First().first;
    const TopAbs_Orientation aFirstOri        = theEdges.First().second;
    const BRepGraph_NodeId   aLastEdgeNodeId  = theEdges.Last().first;
    const TopAbs_Orientation aLastOri         = theEdges.Last().second;

    const BRepGraphInc::EdgeDef& aFirstEdge =
      myGraph->myData->myIncStorage.Edge(BRepGraph_EdgeId(aFirstEdgeNodeId.Index));
    const BRepGraphInc::EdgeDef& aLastEdge =
      myGraph->myData->myIncStorage.Edge(BRepGraph_EdgeId(aLastEdgeNodeId.Index));

    const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
    const BRepGraph_VertexRefId aFirstRefId =
      (aFirstOri == TopAbs_FORWARD) ? aFirstEdge.StartVertexRefId : aFirstEdge.EndVertexRefId;
    const BRepGraph_VertexRefId aLastRefId =
      (aLastOri == TopAbs_FORWARD) ? aLastEdge.EndVertexRefId : aLastEdge.StartVertexRefId;
    const BRepGraph_NodeId aFirstVtx =
      aFirstRefId.IsValid()
        ? BRepGraph_NodeId::Vertex(aStorage.VertexRefEntry(aFirstRefId).VertexDefId.Index)
        : BRepGraph_NodeId();
    const BRepGraph_NodeId aLastVtx =
      aLastRefId.IsValid()
        ? BRepGraph_NodeId::Vertex(aStorage.VertexRefEntry(aLastRefId).VertexDefId.Index)
        : BRepGraph_NodeId();

    const bool aIsClosed = aFirstVtx.IsValid() && aLastVtx.IsValid() && aFirstVtx == aLastVtx;
    myGraph->myData->myIncStorage.ChangeWire(BRepGraph_WireId(aIdx)).IsClosed = aIsClosed;
  }

  return myGraph->myData->myIncStorage.Wire(BRepGraph_WireId(aIdx)).Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddFace(
  const occ::handle<Geom_Surface>&            theSurface,
  const BRepGraph_NodeId                      theOuterWire,
  const NCollection_Vector<BRepGraph_NodeId>& theInnerWires,
  const double                                theTolerance)
{
  BRepGraphInc::FaceDef& aFaceDef = myGraph->myData->myIncStorage.AppendFace();
  const int                    aIdx     = myGraph->myData->myIncStorage.NbFaces() - 1;
  aFaceDef.Id                           = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, aIdx);
  aFaceDef.Tolerance                    = theTolerance;
  myGraph->allocateUID(aFaceDef.Id);
  if (!theSurface.IsNull())
  {
    BRepGraphInc::SurfaceRep& aSurfRep    = myGraph->myData->myIncStorage.AppendSurfaceRep();
    const int                 aSurfRepIdx = myGraph->myData->myIncStorage.NbSurfaces() - 1;
    aSurfRep.Id                           = BRepGraph_RepId::Surface(aSurfRepIdx);
    aSurfRep.Surface                      = theSurface;
    aFaceDef.SurfaceRepId                 = BRepGraph_SurfaceRepId(aSurfRepIdx);
  }

  // Link wire refs.
  if (theOuterWire.IsValid())
  {
    BRepGraphInc::WireRefEntry& aWireRefEntry = myGraph->myData->myIncStorage.AppendWireRefEntry();
    const int                   aWireRefIdx   = myGraph->myData->myIncStorage.NbWireRefs() - 1;
    aWireRefEntry.RefId                      = BRepGraph_RefId::Wire(aWireRefIdx);
    aWireRefEntry.ParentId                   = BRepGraph_NodeId::Face(aIdx);
    aWireRefEntry.WireDefId                  = BRepGraph_WireId::FromNodeId(theOuterWire);
    aWireRefEntry.IsOuter                    = true;
    myGraph->allocateRefUID(aWireRefEntry.RefId);
    myGraph->myData->myIncStorage.ChangeFace(BRepGraph_FaceId(aIdx)).WireRefIds.Append(
      BRepGraph_WireRefId(aWireRefIdx));
  }
  for (int aWireIdx = 0; aWireIdx < theInnerWires.Length(); ++aWireIdx)
  {
    const BRepGraph_NodeId& aWireDefId = theInnerWires.Value(aWireIdx);
    if (!aWireDefId.IsValid())
      continue;

    BRepGraphInc::WireRefEntry& aWireRefEntry = myGraph->myData->myIncStorage.AppendWireRefEntry();
    const int                   aWireRefIdx   = myGraph->myData->myIncStorage.NbWireRefs() - 1;
    aWireRefEntry.RefId                      = BRepGraph_RefId::Wire(aWireRefIdx);
    aWireRefEntry.ParentId                   = BRepGraph_NodeId::Face(aIdx);
    aWireRefEntry.WireDefId                  = BRepGraph_WireId::FromNodeId(aWireDefId);
    aWireRefEntry.IsOuter                    = false;
    myGraph->allocateRefUID(aWireRefEntry.RefId);
    myGraph->myData->myIncStorage.ChangeFace(BRepGraph_FaceId(aIdx)).WireRefIds.Append(
      BRepGraph_WireRefId(aWireRefIdx));
  }

  return aFaceDef.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddShell()
{
  BRepGraphInc::ShellDef& aShellDef = myGraph->myData->myIncStorage.AppendShell();
  const int                     aIdx      = myGraph->myData->myIncStorage.NbShells() - 1;
  aShellDef.Id                            = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Shell, aIdx);
  myGraph->allocateUID(aShellDef.Id);

  return aShellDef.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddSolid()
{
  BRepGraphInc::SolidDef& aSolidDef = myGraph->myData->myIncStorage.AppendSolid();
  const int                     aIdx      = myGraph->myData->myIncStorage.NbSolids() - 1;
  aSolidDef.Id                            = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, aIdx);
  myGraph->allocateUID(aSolidDef.Id);

  return aSolidDef.Id;
}

//=================================================================================================

void BRepGraph::BuilderView::AddFaceToShell(const BRepGraph_NodeId   theShellEntity,
                                            const BRepGraph_NodeId   theFaceEntity,
                                            const TopAbs_Orientation theOri)
{
  // Append FaceRef to shell entity.
  if (theShellEntity.Index >= 0 && theShellEntity.Index < myGraph->myData->myIncStorage.NbShells()
      && !myGraph->myData->myIncStorage.Shell(BRepGraph_ShellId(theShellEntity.Index)).IsRemoved
      && theFaceEntity.Index >= 0 && theFaceEntity.Index < myGraph->myData->myIncStorage.NbFaces()
      && !myGraph->myData->myIncStorage.Face(BRepGraph_FaceId(theFaceEntity.Index)).IsRemoved)
  {
    BRepGraphInc::FaceRefEntry& aFREntry = myGraph->myData->myIncStorage.AppendFaceRefEntry();
    const int                   aFRIdx   = myGraph->myData->myIncStorage.NbFaceRefs() - 1;
    aFREntry.RefId                        = BRepGraph_RefId::Face(aFRIdx);
    aFREntry.ParentId                     = theShellEntity;
    aFREntry.FaceDefId                    = BRepGraph_FaceId::FromNodeId(theFaceEntity);
    aFREntry.Orientation                  = theOri;
    myGraph->allocateRefUID(aFREntry.RefId);
    myGraph->myData->myIncStorage.ChangeShell(BRepGraph_ShellId(theShellEntity.Index)).FaceRefIds.Append(
      BRepGraph_FaceRefId(aFRIdx));
  }

  myGraph->markModified(theShellEntity);
}

//=================================================================================================

void BRepGraph::BuilderView::AddShellToSolid(const BRepGraph_NodeId   theSolidEntity,
                                             const BRepGraph_NodeId   theShellEntity,
                                             const TopAbs_Orientation theOri)
{
  // Append ShellRef to solid entity.
  if (theSolidEntity.Index >= 0 && theSolidEntity.Index < myGraph->myData->myIncStorage.NbSolids()
      && !myGraph->myData->myIncStorage.Solid(BRepGraph_SolidId(theSolidEntity.Index)).IsRemoved
      && theShellEntity.Index >= 0 && theShellEntity.Index < myGraph->myData->myIncStorage.NbShells()
      && !myGraph->myData->myIncStorage.Shell(BRepGraph_ShellId(theShellEntity.Index)).IsRemoved)
  {
    BRepGraphInc::ShellRefEntry& aSREntry = myGraph->myData->myIncStorage.AppendShellRefEntry();
    const int                    aSRIdx   = myGraph->myData->myIncStorage.NbShellRefs() - 1;
    aSREntry.RefId                        = BRepGraph_RefId::Shell(aSRIdx);
    aSREntry.ParentId                     = theSolidEntity;
    aSREntry.ShellDefId                   = BRepGraph_ShellId::FromNodeId(theShellEntity);
    aSREntry.Orientation                  = theOri;
    myGraph->allocateRefUID(aSREntry.RefId);
    myGraph->myData->myIncStorage.ChangeSolid(BRepGraph_SolidId(theSolidEntity.Index)).ShellRefIds.Append(
      BRepGraph_ShellRefId(aSRIdx));
  }

  myGraph->markModified(theSolidEntity);
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddCompound(
  const NCollection_Vector<BRepGraph_NodeId>& theChildEntities)
{
  BRepGraphInc::CompoundDef& aCompDef = myGraph->myData->myIncStorage.AppendCompound();
  const int                        aIdx     = myGraph->myData->myIncStorage.NbCompounds() - 1;
  aCompDef.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Compound, aIdx);
  myGraph->allocateUID(aCompDef.Id);

  for (int aChildIdx = 0; aChildIdx < theChildEntities.Length(); ++aChildIdx)
  {
    const BRepGraph_NodeId& aChild = theChildEntities.Value(aChildIdx);
    BRepGraphInc::ChildRefEntry& aCREntry = myGraph->myData->myIncStorage.AppendChildRefEntry();
    const int                    aCRIdx   = myGraph->myData->myIncStorage.NbChildRefs() - 1;
    aCREntry.RefId                        = BRepGraph_RefId::Child(aCRIdx);
    aCREntry.ParentId                     = aCompDef.Id;
    aCREntry.ChildDefId                   = aChild;
    myGraph->allocateRefUID(aCREntry.RefId);
    aCompDef.ChildRefIds.Append(BRepGraph_ChildRefId(aCRIdx));
  }

  return aCompDef.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddCompSolid(
  const NCollection_Vector<BRepGraph_NodeId>& theSolidEntities)
{
  BRepGraphInc::CompSolidDef& aCSolDef = myGraph->myData->myIncStorage.AppendCompSolid();
  const int                         aIdx     = myGraph->myData->myIncStorage.NbCompSolids() - 1;
  aCSolDef.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::CompSolid, aIdx);
  myGraph->allocateUID(aCSolDef.Id);

  for (int aSolIdx = 0; aSolIdx < theSolidEntities.Length(); ++aSolIdx)
  {
    BRepGraphInc::SolidRefEntry& aSREntry = myGraph->myData->myIncStorage.AppendSolidRefEntry();
    const int                    aSRIdx   = myGraph->myData->myIncStorage.NbSolidRefs() - 1;
    aSREntry.RefId                        = BRepGraph_RefId::Solid(aSRIdx);
    aSREntry.ParentId                     = aCSolDef.Id;
    aSREntry.SolidDefId                   = BRepGraph_SolidId::FromNodeId(theSolidEntities.Value(aSolIdx));
    myGraph->allocateRefUID(aSREntry.RefId);
    aCSolDef.SolidRefIds.Append(BRepGraph_SolidRefId(aSRIdx));
  }

  return aCSolDef.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddProduct(const BRepGraph_NodeId theShapeRoot)
{
  BRepGraphInc::ProductDef& aProductDef = myGraph->myData->myIncStorage.AppendProduct();
  const int                       aIdx        = myGraph->myData->myIncStorage.NbProducts() - 1;
  aProductDef.Id                              = BRepGraph_NodeId::Product(aIdx);
  aProductDef.ShapeRootId                     = theShapeRoot;
  myGraph->allocateUID(aProductDef.Id);

  return aProductDef.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddAssemblyProduct()
{
  BRepGraphInc::ProductDef& aProductDef = myGraph->myData->myIncStorage.AppendProduct();
  const int                       aIdx        = myGraph->myData->myIncStorage.NbProducts() - 1;
  aProductDef.Id                              = BRepGraph_NodeId::Product(aIdx);
  // ShapeRootId left invalid - this is an assembly.
  myGraph->allocateUID(aProductDef.Id);

  return aProductDef.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddOccurrence(const BRepGraph_NodeId theParentProduct,
                                                       const BRepGraph_NodeId theReferencedProduct,
                                                       const TopLoc_Location& thePlacement)
{
  // Delegate with no parent occurrence (top-level).
  return AddOccurrence(theParentProduct, theReferencedProduct, thePlacement, BRepGraph_NodeId());
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddOccurrence(const BRepGraph_NodeId theParentProduct,
                                                       const BRepGraph_NodeId theReferencedProduct,
                                                       const TopLoc_Location& thePlacement,
                                                       const BRepGraph_NodeId theParentOccurrence)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  // Validate that both arguments are active Product nodes with valid indices.
  if (theParentProduct.NodeKind != BRepGraph_NodeId::Kind::Product || theParentProduct.Index < 0
      || theParentProduct.Index >= aStorage.NbProducts()
      || aStorage.Product(BRepGraph_ProductId(theParentProduct.Index)).IsRemoved)
    return BRepGraph_NodeId();
  if (theReferencedProduct.NodeKind != BRepGraph_NodeId::Kind::Product
      || theReferencedProduct.Index < 0 || theReferencedProduct.Index >= aStorage.NbProducts()
      || aStorage.Product(BRepGraph_ProductId(theReferencedProduct.Index)).IsRemoved)
    return BRepGraph_NodeId();
  // Prevent self-referencing cycles in the assembly DAG.
  if (theParentProduct.Index == theReferencedProduct.Index)
    return BRepGraph_NodeId();
  // Validate parent occurrence if provided.
  // ParentOccurrenceIdx = -1 when theParentOccurrence is invalid (top-level).
  if (theParentOccurrence.IsValid()
      && (theParentOccurrence.NodeKind != BRepGraph_NodeId::Kind::Occurrence
          || theParentOccurrence.Index < 0 || theParentOccurrence.Index >= aStorage.NbOccurrences()
          || aStorage.Occurrence(BRepGraph_OccurrenceId(theParentOccurrence.Index)).IsRemoved))
    return BRepGraph_NodeId();

  BRepGraphInc::OccurrenceDef& anOccDef = aStorage.AppendOccurrence();
  const int                          anOccIdx = aStorage.NbOccurrences() - 1;
  anOccDef.Id                                 = BRepGraph_NodeId::Occurrence(anOccIdx);
  anOccDef.ProductDefId          = BRepGraph_ProductId::FromNodeId(theReferencedProduct);
  anOccDef.ParentProductDefId    = BRepGraph_ProductId::FromNodeId(theParentProduct);
  anOccDef.ParentOccurrenceDefId = theParentOccurrence.IsValid()
                                     ? BRepGraph_OccurrenceId::FromNodeId(theParentOccurrence)
                                     : BRepGraph_OccurrenceId();
  anOccDef.Placement             = thePlacement;
  myGraph->allocateUID(anOccDef.Id);

  // Add OccurrenceRefEntry to the parent product.
  BRepGraphInc::OccurrenceRefEntry& anOccRefEntry = aStorage.AppendOccurrenceRefEntry();
  const int anOccRefIdx = aStorage.NbOccurrenceRefs() - 1;
  anOccRefEntry.RefId           = BRepGraph_RefId::Occurrence(anOccRefIdx);
  anOccRefEntry.ParentId        = theParentProduct;
  anOccRefEntry.OccurrenceDefId = BRepGraph_OccurrenceId(anOccIdx);
  myGraph->allocateRefUID(anOccRefEntry.RefId);
  aStorage.ChangeProduct(BRepGraph_ProductId(theParentProduct.Index))
    .OccurrenceRefIds.Append(BRepGraph_OccurrenceRefId(anOccRefIdx));

  return anOccDef.Id;
}

//=================================================================================================

void BRepGraph::BuilderView::AppendShape(const TopoDS_Shape& theShape, const bool theParallel)
{
  BRepGraph_Builder::Append(*myGraph, theShape, theParallel);
}

//=================================================================================================

void BRepGraph::BuilderView::RemoveNode(const BRepGraph_NodeId theNode)
{
  RemoveNode(theNode, BRepGraph_NodeId());
}

//=================================================================================================

void BRepGraph::BuilderView::RemoveNode(const BRepGraph_NodeId theNode,
                                        const BRepGraph_NodeId theReplacement)
{
  if (!theNode.IsValid())
    return;

  // When removing an Edge with a replacement, reparent all CoEdges from the
  // removed edge to the replacement edge. This prevents orphaned CoEdges
  // that would be excluded from queries via CoEdgesOfEdge().
  if (theNode.NodeKind == BRepGraph_NodeId::Kind::Edge && theReplacement.IsValid()
      && theReplacement.NodeKind == BRepGraph_NodeId::Kind::Edge)
  {
    BRepGraphInc_Storage&                         aStorage = myGraph->myData->myIncStorage;
    const NCollection_Vector<BRepGraph_CoEdgeId>* aCoEdgeIdxs =
      aStorage.ReverseIndex().CoEdgesOfEdge(BRepGraph_EdgeId(theNode.Index));
    if (aCoEdgeIdxs != nullptr)
    {
      for (int i = 0; i < aCoEdgeIdxs->Length(); ++i)
      {
        const BRepGraph_CoEdgeId    aCoEdgeId = aCoEdgeIdxs->Value(i);
        BRepGraphInc::CoEdgeDef& aCoEdge   = aStorage.ChangeCoEdge(aCoEdgeId);
        if (aCoEdge.EdgeDefId == theNode)
        {
          aStorage.ChangeReverseIndex().UnbindEdgeFromCoEdge(BRepGraph_EdgeId(theNode.Index),
                                                             aCoEdgeId);
          aCoEdge.EdgeDefId = BRepGraph_EdgeId::FromNodeId(theReplacement);
          aStorage.ChangeReverseIndex().BindEdgeToCoEdge(BRepGraph_EdgeId(theReplacement.Index),
                                                         aCoEdgeId);
        }
      }
    }
  }

  // Mark removed on the entity (which is the sole definition store).
  BRepGraphInc::BaseDef* aDef = myGraph->ChangeTopoEntity(theNode);
  if (aDef != nullptr && !aDef->IsRemoved)
  {
    myGraph->myData->myIncStorage.MarkRemoved(theNode);
  }

  BRepGraph_NodeCache* aCache = myGraph->mutableCache(theNode);
  if (aCache != nullptr)
    aCache->InvalidateAll();

  {
    std::unique_lock<std::shared_mutex> aWriteLock(myGraph->myData->myCurrentShapesMutex);
    myGraph->myData->myCurrentShapes.UnBind(theNode);
  }

  // Notify registered layers.
  myGraph->dispatchLayerOnNodeRemoved(theNode, theReplacement);
}

//=================================================================================================

void BRepGraph::BuilderView::RemoveSubgraph(const BRepGraph_NodeId theNode)
{
  RemoveNode(theNode);

  // Traverse children via reference-entry tables.
  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Compound: {
      if (theNode.Index >= 0 && theNode.Index < myGraph->myData->myIncStorage.NbCompounds())
      {
        const NCollection_Vector<BRepGraph_NodeId> aChildNodes = collectCompoundChildren(*myGraph,
                                                                                          theNode);
        for (int i = 0; i < aChildNodes.Length(); ++i)
          RemoveSubgraph(aChildNodes.Value(i));
      }
      break;
    }
    case BRepGraph_NodeId::Kind::CompSolid: {
      if (theNode.Index >= 0 && theNode.Index < myGraph->myData->myIncStorage.NbCompSolids())
      {
        const NCollection_Vector<BRepGraph_NodeId> aChildNodes = collectCompSolidChildren(*myGraph,
                                                                                           theNode);
        for (int i = 0; i < aChildNodes.Length(); ++i)
          RemoveSubgraph(aChildNodes.Value(i));
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Solid: {
      if (theNode.Index >= 0 && theNode.Index < myGraph->myData->myIncStorage.NbSolids())
      {
        const NCollection_Vector<BRepGraph_NodeId> aChildNodes = collectSolidChildren(*myGraph,
                                                                                       theNode);
        for (int i = 0; i < aChildNodes.Length(); ++i)
          RemoveSubgraph(aChildNodes.Value(i));
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Shell: {
      if (theNode.Index >= 0 && theNode.Index < myGraph->myData->myIncStorage.NbShells())
      {
        const NCollection_Vector<BRepGraph_NodeId> aChildNodes = collectShellChildren(*myGraph,
                                                                                       theNode);
        for (int i = 0; i < aChildNodes.Length(); ++i)
          RemoveSubgraph(aChildNodes.Value(i));
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Face: {
      if (theNode.Index >= 0 && theNode.Index < myGraph->myData->myIncStorage.NbFaces())
      {
        const NCollection_Vector<BRepGraph_NodeId> aChildNodes = collectFaceChildren(*myGraph,
                                                                                      theNode);
        for (int i = 0; i < aChildNodes.Length(); ++i)
          RemoveSubgraph(aChildNodes.Value(i));
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Wire: {
      if (theNode.Index >= 0 && theNode.Index < myGraph->myData->myIncStorage.NbWires())
      {
        const NCollection_Vector<BRepGraph_NodeId> aChildNodes = collectWireChildren(*myGraph,
                                                                                      theNode);
        for (int i = 0; i < aChildNodes.Length(); ++i)
          RemoveSubgraph(aChildNodes.Value(i));
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Edge: {
      if (theNode.Index >= 0 && theNode.Index < myGraph->myData->myIncStorage.NbEdges())
      {
        const BRepGraphInc::EdgeDef& anEdge =
          myGraph->myData->myIncStorage.Edge(BRepGraph_EdgeId(theNode.Index));
        const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
        if (anEdge.StartVertexRefId.IsValid())
        {
          const BRepGraph_VertexId aStartVtxId =
            aStorage.VertexRefEntry(anEdge.StartVertexRefId).VertexDefId;
          if (aStartVtxId.IsValid())
            RemoveNode(aStartVtxId);
        }
        if (anEdge.EndVertexRefId.IsValid())
        {
          const BRepGraph_VertexId anEndVtxId =
            aStorage.VertexRefEntry(anEdge.EndVertexRefId).VertexDefId;
          if (anEndVtxId.IsValid())
            RemoveNode(anEndVtxId);
        }
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Product: {
      if (theNode.Index >= 0 && theNode.Index < myGraph->myData->myIncStorage.NbProducts())
      {
        const BRepGraphInc_Storage&         aStorage = myGraph->myData->myIncStorage;
        const BRepGraphInc::ProductDef& aProduct =
          aStorage.Product(BRepGraph_ProductId(theNode.Index));
        // Snapshot occurrence indices before iterating, because RemoveSubgraph(Occurrence)
        // modifies the parent's OccurrenceRefIds via swap-remove.
        NCollection_Vector<int> anOccIndices;
        for (int i = 0; i < aProduct.OccurrenceRefIds.Length(); ++i)
          anOccIndices.Append(
            aStorage.OccurrenceRefEntry(aProduct.OccurrenceRefIds.Value(i)).OccurrenceDefId.Index);
        for (int i = 0; i < anOccIndices.Length(); ++i)
          RemoveSubgraph(BRepGraph_NodeId::Occurrence(anOccIndices.Value(i)));
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Occurrence: {
      // Remove from parent product's OccurrenceRefIds.
      if (theNode.Index >= 0 && theNode.Index < myGraph->myData->myIncStorage.NbOccurrences())
      {
        const BRepGraphInc::OccurrenceDef& anOcc =
          myGraph->myData->myIncStorage.Occurrence(BRepGraph_OccurrenceId(theNode.Index));
        if (anOcc.ParentProductDefId.IsValid()
            && anOcc.ParentProductDefId.Index < myGraph->myData->myIncStorage.NbProducts())
        {
          NCollection_Vector<BRepGraph_OccurrenceRefId>& aRefIds =
            myGraph->myData->myIncStorage.ChangeProduct(anOcc.ParentProductDefId).OccurrenceRefIds;
          for (int i = 0; i < aRefIds.Length(); ++i)
          {
            BRepGraphInc::OccurrenceRefEntry& aRefEntry =
              myGraph->myData->myIncStorage.ChangeOccurrenceRefEntry(aRefIds.Value(i));
            if (aRefEntry.OccurrenceDefId.Index == theNode.Index)
            {
              if (!aRefEntry.IsRemoved)
              {
                myGraph->myData->myIncStorage.MarkRemovedRef(aRefEntry.RefId);
              }
              if (i < aRefIds.Length() - 1)
                aRefIds.ChangeValue(i) = aRefIds.Value(aRefIds.Length() - 1);
              aRefIds.EraseLast();
              myGraph->markModified(anOcc.ParentProductDefId);
              break;
            }
          }
        }
      }
      break;
    }
    default:
      break;
  }
}

//=================================================================================================

BRepGraph_Curve2DRepId BRepGraph::BuilderView::CreateCurve2DRep(
  const occ::handle<Geom2d_Curve>& theCurve2d)
{
  if (theCurve2d.IsNull())
    return BRepGraph_Curve2DRepId();

  BRepGraphInc::Curve2DRep& aRep  = myGraph->myData->myIncStorage.AppendCurve2DRep();
  const int                 anIdx = myGraph->myData->myIncStorage.NbCurves2D() - 1;
  aRep.Id                         = BRepGraph_RepId::Curve2D(anIdx);
  aRep.Curve                      = theCurve2d;
  return BRepGraph_Curve2DRepId(anIdx);
}

//=================================================================================================

void BRepGraph::BuilderView::AddPCurveToEdge(const BRepGraph_NodeId           theEdgeEntity,
                                             const BRepGraph_NodeId           theFaceEntity,
                                             const occ::handle<Geom2d_Curve>& theCurve2d,
                                             const double                     theFirst,
                                             const double                     theLast,
                                             const TopAbs_Orientation         theEdgeOrientation)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  // Create CoEdge entity for the new PCurve binding.
  BRepGraphInc::CoEdgeDef& aCoEdge    = aStorage.AppendCoEdge();
  const int                   aCoEdgeIdx = aStorage.NbCoEdges() - 1;
  aCoEdge.Id                             = BRepGraph_NodeId::CoEdge(aCoEdgeIdx);
  aCoEdge.EdgeDefId                      = BRepGraph_EdgeId::FromNodeId(theEdgeEntity);
  aCoEdge.FaceDefId                      = BRepGraph_FaceId::FromNodeId(theFaceEntity);
  aCoEdge.Sense                          = theEdgeOrientation;
  if (!theCurve2d.IsNull())
  {
    BRepGraphInc::Curve2DRep& aCurve2DRep    = aStorage.AppendCurve2DRep();
    const int                 aCurve2DRepIdx = aStorage.NbCurves2D() - 1;
    aCurve2DRep.Id                           = BRepGraph_RepId::Curve2D(aCurve2DRepIdx);
    aCurve2DRep.Curve                        = theCurve2d;
    aCoEdge.Curve2DRepId                     = BRepGraph_Curve2DRepId(aCurve2DRepIdx);
  }
  aCoEdge.ParamFirst = theFirst;
  aCoEdge.ParamLast  = theLast;

  // Update reverse indices.
  aStorage.ChangeReverseIndex().BindEdgeToCoEdge(BRepGraph_EdgeId(theEdgeEntity.Index),
                                                 BRepGraph_CoEdgeId(aCoEdgeIdx));
  aStorage.ChangeReverseIndex().BindEdgeToFace(BRepGraph_EdgeId(theEdgeEntity.Index),
                                               BRepGraph_FaceId(theFaceEntity.Index));
  myGraph->allocateUID(aCoEdge.Id);

  myGraph->markModified(theEdgeEntity);
}

//=================================================================================================

void BRepGraph::BuilderView::BeginDeferredInvalidation()
{
  myGraph->myData->myDeferredMode = true;
}

//=================================================================================================

void BRepGraph::BuilderView::EndDeferredInvalidation()
{
  if (!myGraph->myData->myDeferredMode)
    return;

  myGraph->myData->myDeferredMode = false;

  // Bulk-clear all cached shapes. Safe because IsModified flags are already
  // set on mutated entities - reconstruction will recompute as needed.
  {
    std::unique_lock<std::shared_mutex> aWriteLock(myGraph->myData->myCurrentShapesMutex);
    myGraph->myData->myCurrentShapes.Clear();
  }

  // Propagate IsModified upward for all modified entities.
  // Single iterative pass per kind: Edge->Wire->Face->Shell->Solid.
  // Skips already-propagated nodes: O(modified) not O(total).
  const BRepGraphInc_ReverseIndex& aRevIdx  = myGraph->myData->myIncStorage.ReverseIndex();
  const int                        aNbEdges = myGraph->myData->myIncStorage.NbEdges();

  const auto aPropagateShellToSolid = [&](const BRepGraph_ShellId theShellId) {
    const NCollection_Vector<BRepGraph_SolidId>* aSolids = aRevIdx.SolidsOfShell(theShellId);
    if (aSolids == nullptr)
      return;

    for (int aSolidIter = 0; aSolidIter < aSolids->Length(); ++aSolidIter)
    {
      myGraph->myData->myIncStorage.ChangeSolid(aSolids->Value(aSolidIter)).IsModified = true;
    }
  };

  const auto aPropagateFaceToShell = [&](const BRepGraph_FaceId theFaceId) {
    const NCollection_Vector<BRepGraph_ShellId>* aShells = aRevIdx.ShellsOfFace(theFaceId);
    if (aShells == nullptr)
      return;

    for (int aShellIter = 0; aShellIter < aShells->Length(); ++aShellIter)
    {
      const BRepGraph_ShellId       aShellId  = aShells->Value(aShellIter);
      BRepGraphInc::ShellDef& aShellDef = myGraph->myData->myIncStorage.ChangeShell(aShellId);
      if (aShellDef.IsModified)
        continue;
      aShellDef.IsModified = true;
      aPropagateShellToSolid(aShellId);
    }
  };

  const auto aPropagateWireToFace = [&](const BRepGraph_WireId theWireId) {
    const NCollection_Vector<BRepGraph_FaceId>* aFaces = aRevIdx.FacesOfWire(theWireId);
    if (aFaces == nullptr)
      return;

    for (int aFaceIter = 0; aFaceIter < aFaces->Length(); ++aFaceIter)
    {
      const BRepGraph_FaceId       aFaceId  = aFaces->Value(aFaceIter);
      BRepGraphInc::FaceDef& aFaceDef = myGraph->myData->myIncStorage.ChangeFace(aFaceId);
      if (aFaceDef.IsModified)
        continue;
      aFaceDef.IsModified = true;
      aPropagateFaceToShell(aFaceId);
    }
  };

  for (int anEdgeIdx = 0; anEdgeIdx < aNbEdges; ++anEdgeIdx)
  {
    const BRepGraphInc::EdgeDef& anEdge =
      myGraph->myData->myIncStorage.Edge(BRepGraph_EdgeId(anEdgeIdx));
    if (anEdge.IsRemoved || !anEdge.IsModified)
      continue;

    const NCollection_Vector<BRepGraph_WireId>* aWires =
      aRevIdx.WiresOfEdge(BRepGraph_EdgeId(anEdgeIdx));
    if (aWires == nullptr)
      continue;

    for (int aWireIter = 0; aWireIter < aWires->Length(); ++aWireIter)
    {
      const BRepGraph_WireId       aWireId  = aWires->Value(aWireIter);
      BRepGraphInc::WireDef& aWireDef = myGraph->myData->myIncStorage.ChangeWire(aWireId);
      if (aWireDef.IsModified)
        continue;
      aWireDef.IsModified = true;
      aPropagateWireToFace(aWireId);
    }
  }

  // Propagate for directly modified wires (not reached via edges above).
  const int aNbWires = myGraph->myData->myIncStorage.NbWires();
  for (int aWireIdx = 0; aWireIdx < aNbWires; ++aWireIdx)
  {
    if (!myGraph->myData->myIncStorage.Wire(BRepGraph_WireId(aWireIdx)).IsModified)
      continue;
    aPropagateWireToFace(BRepGraph_WireId(aWireIdx));
  }

  // Propagate for directly modified faces (not reached via wires above).
  const int aNbFaces = myGraph->myData->myIncStorage.NbFaces();
  for (int aFaceIdx = 0; aFaceIdx < aNbFaces; ++aFaceIdx)
  {
    if (!myGraph->myData->myIncStorage.Face(BRepGraph_FaceId(aFaceIdx)).IsModified)
      continue;
    aPropagateFaceToShell(BRepGraph_FaceId(aFaceIdx));
  }

  // Propagate for directly modified shells (not reached via faces above).
  const int aNbShells = myGraph->myData->myIncStorage.NbShells();
  for (int aShellIdx = 0; aShellIdx < aNbShells; ++aShellIdx)
  {
    if (!myGraph->myData->myIncStorage.Shell(BRepGraph_ShellId(aShellIdx)).IsModified)
      continue;
    aPropagateShellToSolid(BRepGraph_ShellId(aShellIdx));
  }

  // Invalidate UserAttribute caches (UVBounds, BndLib, etc.) for all modified entities.
  // When modification subscribers exist, the collection of modified NodeIds piggybacks
  // on this existing per-kind iteration to avoid a separate sweep pass.
  const int                            aNbVertices  = myGraph->myData->myIncStorage.NbVertices();
  const int                            aNbSolids    = myGraph->myData->myIncStorage.NbSolids();
  const int                            aNbCompounds = myGraph->myData->myIncStorage.NbCompounds();
  const int                            aNbCompSols  = myGraph->myData->myIncStorage.NbCompSolids();
  const bool                           aCollectModified = myGraph->myHasModificationSubscribers;
  NCollection_Vector<BRepGraph_NodeId> aModifiedNodes;
  if (aCollectModified)
  {
    const int aEstimatedModifiedCapacity = aNbVertices + aNbEdges + aNbWires + aNbFaces + aNbShells
                                           + aNbSolids + aNbCompounds + aNbCompSols;
    if (aEstimatedModifiedCapacity > 0)
      aModifiedNodes.SetIncrement(aEstimatedModifiedCapacity);
  }
  int                                  aModifiedKindsMask = 0;

  for (int i = 0; i < aNbVertices; ++i)
  {
    const BRepGraphInc::VertexDef& aVtx =
      myGraph->myData->myIncStorage.Vertex(BRepGraph_VertexId(i));
    if (!aVtx.IsRemoved && aVtx.IsModified)
    {
      myGraph->myData->myIncStorage.ChangeVertex(BRepGraph_VertexId(i)).Cache.InvalidateAll();
      if (aCollectModified)
      {
        aModifiedNodes.Append(BRepGraph_NodeId::Vertex(i));
        aModifiedKindsMask |= BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Vertex);
      }
    }
  }
  for (int i = 0; i < aNbEdges; ++i)
  {
    const BRepGraphInc::EdgeDef& anEdge =
      myGraph->myData->myIncStorage.Edge(BRepGraph_EdgeId(i));
    if (!anEdge.IsRemoved && anEdge.IsModified)
    {
      myGraph->myData->myIncStorage.ChangeEdge(BRepGraph_EdgeId(i)).Cache.InvalidateAll();
      if (aCollectModified)
      {
        aModifiedNodes.Append(BRepGraph_NodeId::Edge(i));
        aModifiedKindsMask |= BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Edge);
      }
    }
  }
  for (int i = 0; i < aNbWires; ++i)
  {
    const BRepGraphInc::WireDef& aWire = myGraph->myData->myIncStorage.Wire(BRepGraph_WireId(i));
    if (!aWire.IsRemoved && aWire.IsModified)
    {
      myGraph->myData->myIncStorage.ChangeWire(BRepGraph_WireId(i)).Cache.InvalidateAll();
      if (aCollectModified)
      {
        aModifiedNodes.Append(BRepGraph_NodeId::Wire(i));
        aModifiedKindsMask |= BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Wire);
      }
    }
  }
  for (int i = 0; i < aNbFaces; ++i)
  {
    const BRepGraphInc::FaceDef& aFace = myGraph->myData->myIncStorage.Face(BRepGraph_FaceId(i));
    if (!aFace.IsRemoved && aFace.IsModified)
    {
      myGraph->myData->myIncStorage.ChangeFace(BRepGraph_FaceId(i)).Cache.InvalidateAll();
      if (aCollectModified)
      {
        aModifiedNodes.Append(BRepGraph_NodeId::Face(i));
        aModifiedKindsMask |= BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Face);
      }
    }
  }
  for (int i = 0; i < aNbShells; ++i)
  {
    const BRepGraphInc::ShellDef& aShell =
      myGraph->myData->myIncStorage.Shell(BRepGraph_ShellId(i));
    if (!aShell.IsRemoved && aShell.IsModified)
    {
      myGraph->myData->myIncStorage.ChangeShell(BRepGraph_ShellId(i)).Cache.InvalidateAll();
      if (aCollectModified)
      {
        aModifiedNodes.Append(BRepGraph_NodeId::Shell(i));
        aModifiedKindsMask |= BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Shell);
      }
    }
  }
  for (int i = 0; i < aNbSolids; ++i)
  {
    const BRepGraphInc::SolidDef& aSolid =
      myGraph->myData->myIncStorage.Solid(BRepGraph_SolidId(i));
    if (!aSolid.IsRemoved && aSolid.IsModified)
    {
      myGraph->myData->myIncStorage.ChangeSolid(BRepGraph_SolidId(i)).Cache.InvalidateAll();
      if (aCollectModified)
      {
        aModifiedNodes.Append(BRepGraph_NodeId::Solid(i));
        aModifiedKindsMask |= BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Solid);
      }
    }
  }
  for (int i = 0; i < aNbCompounds; ++i)
  {
    const BRepGraphInc::CompoundDef& aComp =
      myGraph->myData->myIncStorage.Compound(BRepGraph_CompoundId(i));
    if (!aComp.IsRemoved && aComp.IsModified)
    {
      myGraph->myData->myIncStorage.ChangeCompound(BRepGraph_CompoundId(i)).Cache.InvalidateAll();
      if (aCollectModified)
      {
        aModifiedNodes.Append(BRepGraph_NodeId::Compound(i));
        aModifiedKindsMask |= BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Compound);
      }
    }
  }
  for (int i = 0; i < aNbCompSols; ++i)
  {
    const BRepGraphInc::CompSolidDef& aCS =
      myGraph->myData->myIncStorage.CompSolid(BRepGraph_CompSolidId(i));
    if (!aCS.IsRemoved && aCS.IsModified)
    {
      myGraph->myData->myIncStorage.ChangeCompSolid(BRepGraph_CompSolidId(i)).Cache.InvalidateAll();
      if (aCollectModified)
      {
        aModifiedNodes.Append(BRepGraph_NodeId::CompSolid(i));
        aModifiedKindsMask |= BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::CompSolid);
      }
    }
  }

  // Dispatch batch modification event to subscribing layers.
  if (aCollectModified && !aModifiedNodes.IsEmpty())
    myGraph->dispatchNodesModified(aModifiedNodes, aModifiedKindsMask);
}

//=================================================================================================

bool BRepGraph::BuilderView::IsDeferredMode() const
{
  return myGraph->myData->myDeferredMode;
}

//=================================================================================================

BRepGraph_MutRef<BRepGraphInc::EdgeDef> BRepGraph::BuilderView::MutEdge(
  const BRepGraph_EdgeId theEdge)
{
  return BRepGraph_MutRef<BRepGraphInc::EdgeDef>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeEdge(theEdge),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, theEdge.Index));
}

//=================================================================================================

BRepGraph_MutRef<BRepGraphInc::CoEdgeDef> BRepGraph::BuilderView::MutCoEdge(
  const BRepGraph_CoEdgeId theCoEdge)
{
  return BRepGraph_MutRef<BRepGraphInc::CoEdgeDef>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeCoEdge(theCoEdge),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::CoEdge, theCoEdge.Index));
}

//=================================================================================================

BRepGraph_MutRef<BRepGraphInc::VertexDef> BRepGraph::BuilderView::MutVertex(
  const BRepGraph_VertexId theVertex)
{
  return BRepGraph_MutRef<BRepGraphInc::VertexDef>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeVertex(theVertex),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Vertex, theVertex.Index));
}

//=================================================================================================

BRepGraph_MutRef<BRepGraphInc::WireDef> BRepGraph::BuilderView::MutWire(
  const BRepGraph_WireId theWire)
{
  return BRepGraph_MutRef<BRepGraphInc::WireDef>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeWire(theWire),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Wire, theWire.Index));
}

//=================================================================================================

BRepGraph_MutRef<BRepGraphInc::FaceDef> BRepGraph::BuilderView::MutFace(
  const BRepGraph_FaceId theFace)
{
  return BRepGraph_MutRef<BRepGraphInc::FaceDef>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeFace(theFace),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, theFace.Index));
}

//=================================================================================================

BRepGraph_MutRef<BRepGraphInc::ShellDef> BRepGraph::BuilderView::MutShell(
  const BRepGraph_ShellId theShell)
{
  return BRepGraph_MutRef<BRepGraphInc::ShellDef>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeShell(theShell),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Shell, theShell.Index));
}

//=================================================================================================

BRepGraph_MutRef<BRepGraphInc::SolidDef> BRepGraph::BuilderView::MutSolid(
  const BRepGraph_SolidId theSolid)
{
  return BRepGraph_MutRef<BRepGraphInc::SolidDef>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeSolid(theSolid),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, theSolid.Index));
}

//=================================================================================================

BRepGraph_MutRef<BRepGraphInc::CompoundDef> BRepGraph::BuilderView::MutCompound(
  const BRepGraph_CompoundId theCompound)
{
  return BRepGraph_MutRef<BRepGraphInc::CompoundDef>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeCompound(theCompound),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Compound, theCompound.Index));
}

//=================================================================================================

BRepGraph_MutRef<BRepGraphInc::CompSolidDef> BRepGraph::BuilderView::MutCompSolid(
  const BRepGraph_CompSolidId theCompSolid)
{
  return BRepGraph_MutRef<BRepGraphInc::CompSolidDef>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeCompSolid(theCompSolid),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::CompSolid, theCompSolid.Index));
}

//=================================================================================================

BRepGraph_MutRef<BRepGraphInc::ProductDef> BRepGraph::BuilderView::MutProduct(
  const BRepGraph_ProductId theProduct)
{
  return BRepGraph_MutRef<BRepGraphInc::ProductDef>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeProduct(theProduct),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Product, theProduct.Index));
}

//=================================================================================================

BRepGraph_MutRef<BRepGraphInc::OccurrenceDef> BRepGraph::BuilderView::MutOccurrence(
  const BRepGraph_OccurrenceId theOccurrence)
{
  return BRepGraph_MutRef<BRepGraphInc::OccurrenceDef>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeOccurrence(theOccurrence),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Occurrence, theOccurrence.Index));
}

//=================================================================================================

BRepGraph_MutRefEntry<BRepGraphInc::ShellRefEntry> BRepGraph::BuilderView::MutShellRef(
  const BRepGraph_ShellRefId theShellRef)
{
  return BRepGraph_MutRefEntry<BRepGraphInc::ShellRefEntry>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeShellRefEntry(theShellRef),
    BRepGraph_RefId::Shell(theShellRef.Index));
}

//=================================================================================================

BRepGraph_MutRefEntry<BRepGraphInc::FaceRefEntry> BRepGraph::BuilderView::MutFaceRef(
  const BRepGraph_FaceRefId theFaceRef)
{
  return BRepGraph_MutRefEntry<BRepGraphInc::FaceRefEntry>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeFaceRefEntry(theFaceRef),
    BRepGraph_RefId::Face(theFaceRef.Index));
}

//=================================================================================================

BRepGraph_MutRefEntry<BRepGraphInc::WireRefEntry> BRepGraph::BuilderView::MutWireRef(
  const BRepGraph_WireRefId theWireRef)
{
  return BRepGraph_MutRefEntry<BRepGraphInc::WireRefEntry>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeWireRefEntry(theWireRef),
    BRepGraph_RefId::Wire(theWireRef.Index));
}

//=================================================================================================

BRepGraph_MutRefEntry<BRepGraphInc::CoEdgeRefEntry> BRepGraph::BuilderView::MutCoEdgeRef(
  const BRepGraph_CoEdgeRefId theCoEdgeRef)
{
  return BRepGraph_MutRefEntry<BRepGraphInc::CoEdgeRefEntry>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeCoEdgeRefEntry(theCoEdgeRef),
    BRepGraph_RefId::CoEdge(theCoEdgeRef.Index));
}

//=================================================================================================

BRepGraph_MutRefEntry<BRepGraphInc::VertexRefEntry> BRepGraph::BuilderView::MutVertexRef(
  const BRepGraph_VertexRefId theVertexRef)
{
  return BRepGraph_MutRefEntry<BRepGraphInc::VertexRefEntry>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeVertexRefEntry(theVertexRef),
    BRepGraph_RefId::Vertex(theVertexRef.Index));
}

//=================================================================================================

BRepGraph_MutRefEntry<BRepGraphInc::SolidRefEntry> BRepGraph::BuilderView::MutSolidRef(
  const BRepGraph_SolidRefId theSolidRef)
{
  return BRepGraph_MutRefEntry<BRepGraphInc::SolidRefEntry>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeSolidRefEntry(theSolidRef),
    BRepGraph_RefId::Solid(theSolidRef.Index));
}

//=================================================================================================

BRepGraph_MutRefEntry<BRepGraphInc::ChildRefEntry> BRepGraph::BuilderView::MutChildRef(
  const BRepGraph_ChildRefId theChildRef)
{
  return BRepGraph_MutRefEntry<BRepGraphInc::ChildRefEntry>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeChildRefEntry(theChildRef),
    BRepGraph_RefId::Child(theChildRef.Index));
}

//=================================================================================================

BRepGraph_MutRefEntry<BRepGraphInc::OccurrenceRefEntry> BRepGraph::BuilderView::MutOccurrenceRef(
  const BRepGraph_OccurrenceRefId theOccurrenceRef)
{
  return BRepGraph_MutRefEntry<BRepGraphInc::OccurrenceRefEntry>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeOccurrenceRefEntry(theOccurrenceRef),
    BRepGraph_RefId::Occurrence(theOccurrenceRef.Index));
}
