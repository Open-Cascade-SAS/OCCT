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

#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <NCollection_PackedMap.hxx>
#include <Poly_Triangulation.hxx>

#include <shared_mutex>

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddVertexDef(const gp_Pnt& thePoint,
                                                      const double  theTolerance)
{
  BRepGraph_TopoNode::VertexDef& aVtxDef = myGraph->myData->myIncStorage.AppendVertex();
  const int                      aIdx    = myGraph->myData->myIncStorage.NbVertices() - 1;
  aVtxDef.Id                             = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Vertex, aIdx);
  aVtxDef.Point                          = thePoint;
  aVtxDef.Tolerance                      = theTolerance;
  myGraph->allocateUID(aVtxDef.Id);

  return aVtxDef.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddEdgeDef(const BRepGraph_NodeId         theStartVtx,
                                                    const BRepGraph_NodeId         theEndVtx,
                                                    const occ::handle<Geom_Curve>& theCurve,
                                                    const double                   theFirst,
                                                    const double                   theLast,
                                                    const double                   theTolerance)
{
  BRepGraph_TopoNode::EdgeDef& anEdgeDef = myGraph->myData->myIncStorage.AppendEdge();
  const int                    aIdx      = myGraph->myData->myIncStorage.NbEdges() - 1;
  anEdgeDef.Id                           = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, aIdx);
  anEdgeDef.StartVertex.VertexDefId =
    theStartVtx.IsValid() ? BRepGraph_VertexId::FromNodeId(theStartVtx) : BRepGraph_VertexId();
  anEdgeDef.StartVertex.Orientation = TopAbs_FORWARD;
  anEdgeDef.EndVertex.VertexDefId =
    theEndVtx.IsValid() ? BRepGraph_VertexId::FromNodeId(theEndVtx) : BRepGraph_VertexId();
  anEdgeDef.EndVertex.Orientation = TopAbs_REVERSED;
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

BRepGraph_NodeId BRepGraph::BuilderView::AddWireDef(
  const NCollection_Vector<std::pair<BRepGraph_NodeId, TopAbs_Orientation>>& theEdges)
{
  BRepGraph_TopoNode::WireDef& aWireDef = myGraph->myData->myIncStorage.AppendWire();
  const int                    aIdx     = myGraph->myData->myIncStorage.NbWires() - 1;
  aWireDef.Id                           = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Wire, aIdx);
  myGraph->allocateUID(aWireDef.Id);

  for (int anEdgeIdx = 0; anEdgeIdx < theEdges.Length(); ++anEdgeIdx)
  {
    const BRepGraph_NodeId   anEdgeDefId = theEdges.Value(anEdgeIdx).first;
    const TopAbs_Orientation anOri       = theEdges.Value(anEdgeIdx).second;

    // Create CoEdge entity for this edge-wire binding.
    BRepGraphInc::CoEdgeEntity& aCoEdge    = myGraph->myData->myIncStorage.AppendCoEdge();
    const int                   aCoEdgeIdx = myGraph->myData->myIncStorage.NbCoEdges() - 1;
    aCoEdge.Id        = BRepGraph_NodeId(BRepGraph_NodeId::Kind::CoEdge, aCoEdgeIdx);
    aCoEdge.EdgeDefId = BRepGraph_EdgeId::FromNodeId(anEdgeDefId);
    aCoEdge.Sense     = anOri;
    myGraph->allocateUID(aCoEdge.Id);

    // CoEdgeRef on WireEntity.
    BRepGraphInc::CoEdgeRef aCoEdgeRef;
    aCoEdgeRef.CoEdgeDefId = BRepGraph_CoEdgeId(aCoEdgeIdx);
    myGraph->myData->myIncStorage.ChangeWire(BRepGraph_WireId(aIdx)).CoEdgeRefs.Append(aCoEdgeRef);

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
    const BRepGraph_NodeId   aFirstDefId = theEdges.First().first;
    const TopAbs_Orientation aFirstOri   = theEdges.First().second;
    const BRepGraph_NodeId   aLastDefId  = theEdges.Last().first;
    const TopAbs_Orientation aLastOri    = theEdges.Last().second;

    const BRepGraph_TopoNode::EdgeDef& aFirstEdge =
      myGraph->myData->myIncStorage.Edge(BRepGraph_EdgeId(aFirstDefId.Index));
    const BRepGraph_TopoNode::EdgeDef& aLastEdge =
      myGraph->myData->myIncStorage.Edge(BRepGraph_EdgeId(aLastDefId.Index));

    BRepGraph_NodeId aFirstVtx =
      (aFirstOri == TopAbs_FORWARD) ? aFirstEdge.StartVertexDefId() : aFirstEdge.EndVertexDefId();
    BRepGraph_NodeId aLastVtx =
      (aLastOri == TopAbs_FORWARD) ? aLastEdge.EndVertexDefId() : aLastEdge.StartVertexDefId();

    const bool aIsClosed = aFirstVtx.IsValid() && aLastVtx.IsValid() && aFirstVtx == aLastVtx;
    myGraph->myData->myIncStorage.ChangeWire(BRepGraph_WireId(aIdx)).IsClosed = aIsClosed;
  }

  return myGraph->myData->myIncStorage.Wire(BRepGraph_WireId(aIdx)).Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddFaceDef(
  const occ::handle<Geom_Surface>&            theSurface,
  const BRepGraph_NodeId                      theOuterWire,
  const NCollection_Vector<BRepGraph_NodeId>& theInnerWires,
  const double                                theTolerance)
{
  BRepGraph_TopoNode::FaceDef& aFaceDef = myGraph->myData->myIncStorage.AppendFace();
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
    BRepGraphInc::WireRef aWireRef;
    aWireRef.WireDefId = BRepGraph_WireId::FromNodeId(theOuterWire);
    aWireRef.IsOuter   = true;
    myGraph->myData->myIncStorage.ChangeFace(BRepGraph_FaceId(aIdx)).WireRefs.Append(aWireRef);
  }
  for (int aWireIdx = 0; aWireIdx < theInnerWires.Length(); ++aWireIdx)
  {
    const BRepGraph_NodeId& aWireDefId = theInnerWires.Value(aWireIdx);
    if (!aWireDefId.IsValid())
      continue;

    BRepGraphInc::WireRef aWireRef;
    aWireRef.WireDefId = BRepGraph_WireId::FromNodeId(aWireDefId);
    aWireRef.IsOuter   = false;
    myGraph->myData->myIncStorage.ChangeFace(BRepGraph_FaceId(aIdx)).WireRefs.Append(aWireRef);
  }

  return aFaceDef.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddShellDef()
{
  BRepGraph_TopoNode::ShellDef& aShellDef = myGraph->myData->myIncStorage.AppendShell();
  const int                     aIdx      = myGraph->myData->myIncStorage.NbShells() - 1;
  aShellDef.Id                            = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Shell, aIdx);
  myGraph->allocateUID(aShellDef.Id);

  return aShellDef.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddSolidDef()
{
  BRepGraph_TopoNode::SolidDef& aSolidDef = myGraph->myData->myIncStorage.AppendSolid();
  const int                     aIdx      = myGraph->myData->myIncStorage.NbSolids() - 1;
  aSolidDef.Id                            = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, aIdx);
  myGraph->allocateUID(aSolidDef.Id);

  return aSolidDef.Id;
}

//=================================================================================================

void BRepGraph::BuilderView::AddFaceToShell(const BRepGraph_NodeId   theShellDef,
                                            const BRepGraph_NodeId   theFaceDef,
                                            const TopAbs_Orientation theOri)
{
  // Append FaceRef to shell entity.
  if (theShellDef.Index >= 0 && theShellDef.Index < myGraph->myData->myIncStorage.NbShells()
      && !myGraph->myData->myIncStorage.Shell(BRepGraph_ShellId(theShellDef.Index)).IsRemoved
      && theFaceDef.Index >= 0 && theFaceDef.Index < myGraph->myData->myIncStorage.NbFaces()
      && !myGraph->myData->myIncStorage.Face(BRepGraph_FaceId(theFaceDef.Index)).IsRemoved)
  {
    BRepGraphInc::FaceRef aFR;
    aFR.FaceDefId   = BRepGraph_FaceId::FromNodeId(theFaceDef);
    aFR.Orientation = theOri;
    myGraph->myData->myIncStorage.ChangeShell(BRepGraph_ShellId(theShellDef.Index))
      .FaceRefs.Append(aFR);
  }

  myGraph->markModified(theShellDef);
}

//=================================================================================================

void BRepGraph::BuilderView::AddShellToSolid(const BRepGraph_NodeId   theSolidDef,
                                             const BRepGraph_NodeId   theShellDef,
                                             const TopAbs_Orientation theOri)
{
  // Append ShellRef to solid entity.
  if (theSolidDef.Index >= 0 && theSolidDef.Index < myGraph->myData->myIncStorage.NbSolids()
      && !myGraph->myData->myIncStorage.Solid(BRepGraph_SolidId(theSolidDef.Index)).IsRemoved
      && theShellDef.Index >= 0 && theShellDef.Index < myGraph->myData->myIncStorage.NbShells()
      && !myGraph->myData->myIncStorage.Shell(BRepGraph_ShellId(theShellDef.Index)).IsRemoved)
  {
    BRepGraphInc::ShellRef aSR;
    aSR.ShellDefId  = BRepGraph_ShellId::FromNodeId(theShellDef);
    aSR.Orientation = theOri;
    myGraph->myData->myIncStorage.ChangeSolid(BRepGraph_SolidId(theSolidDef.Index))
      .ShellRefs.Append(aSR);
  }

  myGraph->markModified(theSolidDef);
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddCompoundDef(
  const NCollection_Vector<BRepGraph_NodeId>& theChildDefs)
{
  BRepGraph_TopoNode::CompoundDef& aCompDef = myGraph->myData->myIncStorage.AppendCompound();
  const int                        aIdx     = myGraph->myData->myIncStorage.NbCompounds() - 1;
  aCompDef.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Compound, aIdx);
  myGraph->allocateUID(aCompDef.Id);

  for (int aChildIdx = 0; aChildIdx < theChildDefs.Length(); ++aChildIdx)
  {
    const BRepGraph_NodeId& aChild = theChildDefs.Value(aChildIdx);
    BRepGraphInc::ChildRef  aCR;
    aCR.ChildDefId = aChild;
    aCompDef.ChildRefs.Append(aCR);
  }

  return aCompDef.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddCompSolidDef(
  const NCollection_Vector<BRepGraph_NodeId>& theSolidDefs)
{
  BRepGraph_TopoNode::CompSolidDef& aCSolDef = myGraph->myData->myIncStorage.AppendCompSolid();
  const int                         aIdx     = myGraph->myData->myIncStorage.NbCompSolids() - 1;
  aCSolDef.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::CompSolid, aIdx);
  myGraph->allocateUID(aCSolDef.Id);

  for (int aSolIdx = 0; aSolIdx < theSolidDefs.Length(); ++aSolIdx)
  {
    BRepGraphInc::SolidRef aSR;
    aSR.SolidDefId = BRepGraph_SolidId::FromNodeId(theSolidDefs.Value(aSolIdx));
    aCSolDef.SolidRefs.Append(aSR);
  }

  return aCSolDef.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddProduct(const BRepGraph_NodeId theShapeRoot)
{
  BRepGraph_TopoNode::ProductDef& aProductDef = myGraph->myData->myIncStorage.AppendProduct();
  const int                       aIdx        = myGraph->myData->myIncStorage.NbProducts() - 1;
  aProductDef.Id                              = BRepGraph_NodeId::Product(aIdx);
  aProductDef.ShapeRootId                     = theShapeRoot;
  myGraph->allocateUID(aProductDef.Id);

  return aProductDef.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddAssemblyProduct()
{
  BRepGraph_TopoNode::ProductDef& aProductDef = myGraph->myData->myIncStorage.AppendProduct();
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

  BRepGraph_TopoNode::OccurrenceDef& anOccDef = aStorage.AppendOccurrence();
  const int                          anOccIdx = aStorage.NbOccurrences() - 1;
  anOccDef.Id                                 = BRepGraph_NodeId::Occurrence(anOccIdx);
  anOccDef.ProductDefId          = BRepGraph_ProductId::FromNodeId(theReferencedProduct);
  anOccDef.ParentProductDefId    = BRepGraph_ProductId::FromNodeId(theParentProduct);
  anOccDef.ParentOccurrenceDefId = theParentOccurrence.IsValid()
                                     ? BRepGraph_OccurrenceId::FromNodeId(theParentOccurrence)
                                     : BRepGraph_OccurrenceId();
  anOccDef.Placement             = thePlacement;
  myGraph->allocateUID(anOccDef.Id);

  // Add OccurrenceRef to the parent product.
  BRepGraphInc::OccurrenceRef anOccRef;
  anOccRef.OccurrenceDefId = BRepGraph_OccurrenceId(anOccIdx);
  aStorage.ChangeProduct(BRepGraph_ProductId(theParentProduct.Index))
    .OccurrenceRefs.Append(anOccRef);

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
        BRepGraphInc::CoEdgeEntity& aCoEdge   = aStorage.ChangeCoEdge(aCoEdgeId);
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
  BRepGraph_TopoNode::BaseDef* aDef = myGraph->ChangeTopoDef(theNode);
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

  // Traverse children via incidence refs.
  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Compound: {
      if (theNode.Index >= 0 && theNode.Index < myGraph->myData->myIncStorage.NbCompounds())
      {
        const BRepGraphInc::CompoundEntity& aComp =
          myGraph->myData->myIncStorage.Compound(BRepGraph_CompoundId(theNode.Index));
        for (int i = 0; i < aComp.ChildRefs.Length(); ++i)
        {
          const BRepGraphInc::ChildRef& aCR = aComp.ChildRefs.Value(i);
          RemoveSubgraph(aCR.ChildDefId);
        }
      }
      break;
    }
    case BRepGraph_NodeId::Kind::CompSolid: {
      if (theNode.Index >= 0 && theNode.Index < myGraph->myData->myIncStorage.NbCompSolids())
      {
        const BRepGraphInc::CompSolidEntity& aCSol =
          myGraph->myData->myIncStorage.CompSolid(BRepGraph_CompSolidId(theNode.Index));
        for (int i = 0; i < aCSol.SolidRefs.Length(); ++i)
          RemoveSubgraph(aCSol.SolidRefs.Value(i).SolidDefId);
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Solid: {
      if (theNode.Index >= 0 && theNode.Index < myGraph->myData->myIncStorage.NbSolids())
      {
        const BRepGraphInc::SolidEntity& aSolid =
          myGraph->myData->myIncStorage.Solid(BRepGraph_SolidId(theNode.Index));
        for (int i = 0; i < aSolid.ShellRefs.Length(); ++i)
          RemoveSubgraph(aSolid.ShellRefs.Value(i).ShellDefId);
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Shell: {
      if (theNode.Index >= 0 && theNode.Index < myGraph->myData->myIncStorage.NbShells())
      {
        const BRepGraphInc::ShellEntity& aShell =
          myGraph->myData->myIncStorage.Shell(BRepGraph_ShellId(theNode.Index));
        for (int i = 0; i < aShell.FaceRefs.Length(); ++i)
          RemoveSubgraph(aShell.FaceRefs.Value(i).FaceDefId);
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Face: {
      if (theNode.Index >= 0 && theNode.Index < myGraph->myData->myIncStorage.NbFaces())
      {
        const BRepGraphInc::FaceEntity& aFace =
          myGraph->myData->myIncStorage.Face(BRepGraph_FaceId(theNode.Index));
        for (int i = 0; i < aFace.WireRefs.Length(); ++i)
          RemoveSubgraph(aFace.WireRefs.Value(i).WireDefId);
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Wire: {
      if (theNode.Index >= 0 && theNode.Index < myGraph->myData->myIncStorage.NbWires())
      {
        const BRepGraphInc::WireEntity& aWire =
          myGraph->myData->myIncStorage.Wire(BRepGraph_WireId(theNode.Index));
        for (int i = 0; i < aWire.CoEdgeRefs.Length(); ++i)
        {
          const BRepGraphInc::CoEdgeEntity& aCoEdge = myGraph->myData->myIncStorage.CoEdge(
            BRepGraph_CoEdgeId(aWire.CoEdgeRefs.Value(i).CoEdgeDefId));
          RemoveSubgraph(aCoEdge.EdgeDefId);
        }
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Edge: {
      if (theNode.Index >= 0 && theNode.Index < myGraph->myData->myIncStorage.NbEdges())
      {
        const BRepGraphInc::EdgeEntity& anEdge =
          myGraph->myData->myIncStorage.Edge(BRepGraph_EdgeId(theNode.Index));
        if (anEdge.StartVertex.VertexDefId.IsValid())
          RemoveNode(anEdge.StartVertex.VertexDefId);
        if (anEdge.EndVertex.VertexDefId.IsValid())
          RemoveNode(anEdge.EndVertex.VertexDefId);
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Product: {
      if (theNode.Index >= 0 && theNode.Index < myGraph->myData->myIncStorage.NbProducts())
      {
        const BRepGraphInc::ProductEntity& aProduct =
          myGraph->myData->myIncStorage.Product(BRepGraph_ProductId(theNode.Index));
        // Snapshot occurrence indices before iterating, because RemoveSubgraph(Occurrence)
        // modifies the parent's OccurrenceRefs via swap-remove.
        NCollection_Vector<int> anOccIndices;
        for (int i = 0; i < aProduct.OccurrenceRefs.Length(); ++i)
          anOccIndices.Append(aProduct.OccurrenceRefs.Value(i).OccurrenceDefId.Index);
        for (int i = 0; i < anOccIndices.Length(); ++i)
          RemoveSubgraph(BRepGraph_NodeId::Occurrence(anOccIndices.Value(i)));
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Occurrence: {
      // Remove from parent product's OccurrenceRefs.
      if (theNode.Index >= 0 && theNode.Index < myGraph->myData->myIncStorage.NbOccurrences())
      {
        const BRepGraphInc::OccurrenceEntity& anOcc =
          myGraph->myData->myIncStorage.Occurrence(BRepGraph_OccurrenceId(theNode.Index));
        if (anOcc.ParentProductDefId.IsValid()
            && anOcc.ParentProductDefId.Index < myGraph->myData->myIncStorage.NbProducts())
        {
          NCollection_Vector<BRepGraphInc::OccurrenceRef>& aRefs =
            myGraph->myData->myIncStorage.ChangeProduct(anOcc.ParentProductDefId).OccurrenceRefs;
          for (int i = 0; i < aRefs.Length(); ++i)
          {
            if (aRefs.Value(i).OccurrenceDefId.Index == theNode.Index)
            {
              if (i < aRefs.Length() - 1)
                aRefs.ChangeValue(i) = aRefs.Value(aRefs.Length() - 1);
              aRefs.EraseLast();
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

void BRepGraph::BuilderView::AddPCurveToEdge(const BRepGraph_NodeId           theEdgeDef,
                                             const BRepGraph_NodeId           theFaceDef,
                                             const occ::handle<Geom2d_Curve>& theCurve2d,
                                             const double                     theFirst,
                                             const double                     theLast,
                                             const TopAbs_Orientation         theEdgeOrientation)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  // Create CoEdge entity for the new PCurve binding.
  BRepGraphInc::CoEdgeEntity& aCoEdge    = aStorage.AppendCoEdge();
  const int                   aCoEdgeIdx = aStorage.NbCoEdges() - 1;
  aCoEdge.Id                             = BRepGraph_NodeId::CoEdge(aCoEdgeIdx);
  aCoEdge.EdgeDefId                      = BRepGraph_EdgeId::FromNodeId(theEdgeDef);
  aCoEdge.FaceDefId                      = BRepGraph_FaceId::FromNodeId(theFaceDef);
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
  aStorage.ChangeReverseIndex().BindEdgeToCoEdge(BRepGraph_EdgeId(theEdgeDef.Index),
                                                 BRepGraph_CoEdgeId(aCoEdgeIdx));
  aStorage.ChangeReverseIndex().BindEdgeToFace(BRepGraph_EdgeId(theEdgeDef.Index),
                                               BRepGraph_FaceId(theFaceDef.Index));
  myGraph->allocateUID(aCoEdge.Id);

  myGraph->markModified(theEdgeDef);
}
