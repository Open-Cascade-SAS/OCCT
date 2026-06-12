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
#include <BRepGraph_Data.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_LayerRegistry.hxx>
#include <BRepGraphInc_Storage.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Curve.hxx>
#include <Geom2d_Curve.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_Polygon2D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <gp_Pnt.hxx>

namespace
{
bool isValidOccurrenceChildKind(const BRepGraph_NodeId::Kind theNodeKind)
{
  return theNodeKind == BRepGraph_NodeId::Kind::Product
         || BRepGraph_NodeId::IsTopologyKind(theNodeKind);
}

bool isActiveOccurrenceChild(const BRepGraphInc_Storage& theStorage,
                             const BRepGraph_NodeId      theChild)
{
  if (!theChild.IsValid() || !isValidOccurrenceChildKind(theChild.NodeKind))
  {
    return false;
  }

  switch (theChild.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      return BRepGraph_VertexId(theChild).IsValid(theStorage.NbVertices())
             && !theStorage.IsRemoved(BRepGraph_VertexId(theChild));
    case BRepGraph_NodeId::Kind::Edge:
      return BRepGraph_EdgeId(theChild).IsValid(theStorage.NbEdges())
             && !theStorage.IsRemoved(BRepGraph_EdgeId(theChild));
    case BRepGraph_NodeId::Kind::CoEdge:
      return BRepGraph_CoEdgeId(theChild).IsValid(theStorage.NbCoEdges())
             && !theStorage.IsRemoved(BRepGraph_CoEdgeId(theChild));
    case BRepGraph_NodeId::Kind::Wire:
      return BRepGraph_WireId(theChild).IsValid(theStorage.NbWires())
             && !theStorage.IsRemoved(BRepGraph_WireId(theChild));
    case BRepGraph_NodeId::Kind::Face:
      return BRepGraph_FaceId(theChild).IsValid(theStorage.NbFaces())
             && !theStorage.IsRemoved(BRepGraph_FaceId(theChild));
    case BRepGraph_NodeId::Kind::Shell:
      return BRepGraph_ShellId(theChild).IsValid(theStorage.NbShells())
             && !theStorage.IsRemoved(BRepGraph_ShellId(theChild));
    case BRepGraph_NodeId::Kind::Solid:
      return BRepGraph_SolidId(theChild).IsValid(theStorage.NbSolids())
             && !theStorage.IsRemoved(BRepGraph_SolidId(theChild));
    case BRepGraph_NodeId::Kind::Compound:
      return BRepGraph_CompoundId(theChild).IsValid(theStorage.NbCompounds())
             && !theStorage.IsRemoved(BRepGraph_CompoundId(theChild));
    case BRepGraph_NodeId::Kind::CompSolid:
      return BRepGraph_CompSolidId(theChild).IsValid(theStorage.NbCompSolids())
             && !theStorage.IsRemoved(BRepGraph_CompSolidId(theChild));
    case BRepGraph_NodeId::Kind::Product:
      return BRepGraph_ProductId(theChild).IsValid(theStorage.NbProducts())
             && !theStorage.IsRemoved(BRepGraph_ProductId(theChild));
    case BRepGraph_NodeId::Kind::Occurrence:
      return false;
  }

  return false;
}

bool isActiveOccurrence(const BRepGraphInc_Storage&  theStorage,
                        const BRepGraph_OccurrenceId theOccurrence)
{
  return theOccurrence.IsValid(theStorage.NbOccurrences()) && !theStorage.IsRemoved(theOccurrence);
}

void clearCoEdgeFaceScopedRepresentations(BRepGraphInc_Storage&    theStorage,
                                          BRepGraphInc::CoEdgeDef& theCoEdge)
{
  if (theCoEdge.Curve2DRepId.IsValid(theStorage.NbCoEdgeCurves2D()))
  {
    theStorage.MarkRemoved(theCoEdge.Curve2DRepId);
  }
  theCoEdge.Curve2DRepId = BRepGraph_CoEdgeCurve2DRepId();

  if (theCoEdge.Polygon2DRepId.IsValid(theStorage.NbCoEdgePolygons2D()))
  {
    theStorage.MarkRemoved(theCoEdge.Polygon2DRepId);
  }
  theCoEdge.Polygon2DRepId = BRepGraph_CoEdgePolygon2DRepId();

  if (theCoEdge.PolygonOnTriRepId.IsValid(theStorage.NbCoEdgePolygonsOnTri()))
  {
    theStorage.MarkRemoved(theCoEdge.PolygonOnTriRepId);
  }
  theCoEdge.PolygonOnTriRepId = BRepGraph_CoEdgePolygonOnTriRepId();
}

void rebindOccurrenceChild(BRepGraphInc_Storage&        theStorage,
                           const BRepGraph_OccurrenceId theOccurrence,
                           const BRepGraph_NodeId       theOldChild,
                           const BRepGraph_NodeId       theNewChild)
{
  theStorage.RebindOccurrenceChild(theOccurrence, theOldChild, theNewChild);
}

bool occurrenceHasOtherActiveRef(const BRepGraphInc_Storage&     theStorage,
                                 const BRepGraph_OccurrenceId    theOccurrence,
                                 const BRepGraph_OccurrenceRefId theExcludingRef)
{
  if (!theOccurrence.IsValid(theStorage.NbOccurrences()))
  {
    return false;
  }
  for (BRepGraph_OccurrenceRefId aRefId = BRepGraph_OccurrenceRefId::Start();
       aRefId.IsValid(theStorage.NbOccurrenceRefs());
       ++aRefId)
  {
    if (aRefId == theExcludingRef)
    {
      continue;
    }
    const BRepGraphInc::OccurrenceRef& aRef = theStorage.OccurrenceRef(aRefId);
    if (!theStorage.IsRemoved(aRefId) && aRef.ChildOccurrenceId == theOccurrence)
    {
      return true;
    }
  }
  return false;
}

void rebindOccurrenceRefParentProduct(BRepGraphInc_Storage&           theStorage,
                                      const BRepGraph_OccurrenceRefId theOccurrenceRef,
                                      const BRepGraph_OccurrenceId    theOldOccurrence,
                                      const BRepGraph_OccurrenceId    theNewOccurrence)
{
  theStorage.RebindOccurrenceRef(theOccurrenceRef, theOldOccurrence, theNewOccurrence);
}

} // namespace

//=================================================================================================

void BRepGraph::EditorView::VertexOps::SetPoint(const BRepGraph_VertexId theVertex,
                                                const gp_Pnt&            thePoint)
{
  myGraph->Editor().requireUnlocked(theVertex, "BRepGraph::EditorView: locked item");
  myGraph->myData->myIncStorage.ChangeVertex(theVertex).Point = thePoint;
  myGraph->markModified(theVertex);
}

//=================================================================================================

void BRepGraph::EditorView::VertexOps::SetPoint(BRepGraph_MutGuard<BRepGraphInc::VertexDef>& theMut,
                                                const gp_Pnt& thePoint)
{
  theMut.Internal().Point = thePoint;
}

//=================================================================================================

void BRepGraph::EditorView::EdgeOps::SetTolerance(const BRepGraph_EdgeId theEdge,
                                                  double                 theTolerance)
{
  myGraph->Editor().requireUnlocked(theEdge, "BRepGraph::EditorView: locked item");
  myGraph->myData->myIncStorage.ChangeEdge(theEdge).Tolerance = theTolerance;
  myGraph->markModified(theEdge);
}

//=================================================================================================

void BRepGraph::EditorView::EdgeOps::SetTolerance(BRepGraph_MutGuard<BRepGraphInc::EdgeDef>& theMut,
                                                  double theTolerance)
{
  theMut.Internal().Tolerance = theTolerance;
}

//=================================================================================================

void BRepGraph::EditorView::CoEdgeOps::SetParamRange(const BRepGraph_CoEdgeId theCoEdge,
                                                     double                   theFirst,
                                                     double                   theLast)
{
  myGraph->Editor().requireUnlocked(theCoEdge, "BRepGraph::EditorView: locked item");
  BRepGraphInc_Storage&    aStorage = myGraph->myData->myIncStorage;
  BRepGraphInc::CoEdgeDef& aDef     = aStorage.ChangeCoEdge(theCoEdge);
  if (aDef.Curve2DRepId.IsValid() && aDef.Curve2DRepId.IsValid(aStorage.NbCoEdgeCurves2D()))
  {
    BRepGraphInc::CoEdgeCurve2DRep& aUse = aStorage.ChangeCoEdgeCurve2DRep(aDef.Curve2DRepId);
    aUse.ParamFirst                      = theFirst;
    aUse.ParamLast                       = theLast;
  }
  myGraph->markModified(theCoEdge);
}

//=================================================================================================

void BRepGraph::EditorView::CoEdgeOps::SetParamRange(
  BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef>& theMut,
  double                                       theFirst,
  double                                       theLast)
{
  BRepGraphInc_Storage&    aStorage = myGraph->myData->myIncStorage;
  BRepGraphInc::CoEdgeDef& aDef     = theMut.Internal();
  if (aDef.Curve2DRepId.IsValid() && aDef.Curve2DRepId.IsValid(aStorage.NbCoEdgeCurves2D()))
  {
    BRepGraphInc::CoEdgeCurve2DRep& aUse = aStorage.ChangeCoEdgeCurve2DRep(aDef.Curve2DRepId);
    aUse.ParamFirst                      = theFirst;
    aUse.ParamLast                       = theLast;
  }
}

//=================================================================================================

void BRepGraph::EditorView::FaceOps::SetTolerance(const BRepGraph_FaceId theFace,
                                                  double                 theTolerance)
{
  myGraph->Editor().requireUnlocked(theFace, "BRepGraph::EditorView: locked item");
  myGraph->myData->myIncStorage.ChangeFace(theFace).Tolerance = theTolerance;
  myGraph->markModified(theFace);
}

//=================================================================================================

void BRepGraph::EditorView::FaceOps::SetTolerance(BRepGraph_MutGuard<BRepGraphInc::FaceDef>& theMut,
                                                  double theTolerance)
{
  theMut.Internal().Tolerance = theTolerance;
}

//=================================================================================================

void BRepGraph::EditorView::FaceOps::SetRefOrientation(
  const BRepGraph_FaceRefId       theFaceRef,
  BRepGraphInc::ParityOrientation theOrientation)
{
  myGraph->Editor().requireUnlocked(theFaceRef, "BRepGraph::EditorView: locked item");
  myGraph->myData->myIncStorage.ChangeFaceRef(theFaceRef).Orientation = theOrientation;
  myGraph->markRefModified(theFaceRef);
}

//=================================================================================================

void BRepGraph::EditorView::FaceOps::SetRefOrientation(
  BRepGraph_MutGuard<BRepGraphInc::FaceRef>& theMut,
  BRepGraphInc::ParityOrientation            theOrientation)
{
  theMut.Internal().Orientation = theOrientation;
}

//=================================================================================================

void BRepGraph::EditorView::OccurrenceOps::SetRefLocalLocation(
  const BRepGraph_OccurrenceRefId theOccurrenceRef,
  const TopLoc_Location&          theLoc)
{
  myGraph->Editor().requireUnlocked(theOccurrenceRef, "BRepGraph::EditorView: locked item");
  myGraph->myData->myIncStorage.ChangeOccurrenceRef(theOccurrenceRef).LocalLocation = theLoc;
  myGraph->markRefModified(theOccurrenceRef);
}

//=================================================================================================

void BRepGraph::EditorView::OccurrenceOps::SetRefLocalLocation(
  BRepGraph_MutGuard<BRepGraphInc::OccurrenceRef>& theMut,
  const TopLoc_Location&                           theLoc)
{
  theMut.Internal().LocalLocation = theLoc;
}

//=================================================================================================

void BRepGraph::EditorView::GenOps::SetChildRefLocalLocation(const BRepGraph_ChildRefId theChildRef,
                                                             const TopLoc_Location&     theLoc)
{
  myGraph->Editor().requireUnlocked(theChildRef, "BRepGraph::EditorView: locked item");
  myGraph->myData->myIncStorage.ChangeChildRef(theChildRef).LocalLocation = theLoc;
  myGraph->markRefModified(theChildRef);
}

//=================================================================================================

void BRepGraph::EditorView::GenOps::SetChildRefLocalLocation(
  BRepGraph_MutGuard<BRepGraphInc::ChildRef>& theMut,
  const TopLoc_Location&                      theLoc)
{
  theMut.Internal().LocalLocation = theLoc;
}

//=================================================================================================

void BRepGraph::EditorView::EdgeOps::SetParamRange(const BRepGraph_EdgeId theEdge,
                                                   const double           theFirst,
                                                   const double           theLast)
{
  myGraph->Editor().requireUnlocked(theEdge, "BRepGraph::EditorView: locked item");
  BRepGraphInc_Storage&  aStorage = myGraph->myData->myIncStorage;
  BRepGraphInc::EdgeDef& aDef     = aStorage.ChangeEdge(theEdge);
  if (aDef.Curve3DRepId.IsValid() && aDef.Curve3DRepId.IsValid(aStorage.NbEdgeCurves3D()))
  {
    BRepGraphInc::EdgeCurve3DRep& aUse = aStorage.ChangeEdgeCurve3DRep(aDef.Curve3DRepId);
    aUse.ParamFirst                    = theFirst;
    aUse.ParamLast                     = theLast;
  }
  myGraph->markModified(theEdge);
}

//=================================================================================================

void BRepGraph::EditorView::EdgeOps::SetParamRange(
  BRepGraph_MutGuard<BRepGraphInc::EdgeDef>& theMut,
  const double                               theFirst,
  const double                               theLast)
{
  BRepGraphInc_Storage&  aStorage = myGraph->myData->myIncStorage;
  BRepGraphInc::EdgeDef& aDef     = theMut.Internal();
  if (aDef.Curve3DRepId.IsValid() && aDef.Curve3DRepId.IsValid(aStorage.NbEdgeCurves3D()))
  {
    BRepGraphInc::EdgeCurve3DRep& aUse = aStorage.ChangeEdgeCurve3DRep(aDef.Curve3DRepId);
    aUse.ParamFirst                    = theFirst;
    aUse.ParamLast                     = theLast;
  }
}

//=================================================================================================

void BRepGraph::EditorView::CoEdgeOps::SetOrientation(
  const BRepGraph_CoEdgeId              theCoEdge,
  const BRepGraphInc::ParityOrientation theOrientation)
{
  myGraph->Editor().requireUnlocked(theCoEdge, "BRepGraph::EditorView: locked item");
  myGraph->myData->myIncStorage.ChangeCoEdge(theCoEdge).Orientation = theOrientation;
  myGraph->markModified(theCoEdge);
}

//=================================================================================================

void BRepGraph::EditorView::CoEdgeOps::SetOrientation(
  BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef>& theMut,
  const BRepGraphInc::ParityOrientation        theOrientation)
{
  theMut.Internal().Orientation = theOrientation;
}

//=================================================================================================

void BRepGraph::EditorView::CoEdgeOps::ResetPCurveBinding(const BRepGraph_CoEdgeId theCoEdge)
{
  myGraph->Editor().requireUnlocked(theCoEdge, "BRepGraph::EditorView: locked item");
  BRepGraphInc_Storage&              aStorage = myGraph->myData->myIncStorage;
  BRepGraphInc::CoEdgeDef&           aDef     = aStorage.ChangeCoEdge(theCoEdge);
  const BRepGraph_CoEdgeCurve2DRepId anOldId  = aDef.Curve2DRepId;
  aDef.Curve2DRepId                           = BRepGraph_CoEdgeCurve2DRepId();
  if (anOldId.IsValid() && anOldId.IsValid(aStorage.NbCoEdgeCurves2D()))
  {
    BRepGraphInc::CoEdgeCurve2DRep& aUse = aStorage.ChangeCoEdgeCurve2DRep(anOldId);
    aUse.ParamFirst                      = 0.0;
    aUse.ParamLast                       = 0.0;
  }
  myGraph->markModified(theCoEdge);
}

//=================================================================================================

void BRepGraph::EditorView::CoEdgeOps::ResetPCurveBinding(
  BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef>& theMut)
{
  BRepGraphInc_Storage&              aStorage = myGraph->myData->myIncStorage;
  BRepGraphInc::CoEdgeDef&           aDef     = theMut.Internal();
  const BRepGraph_CoEdgeCurve2DRepId anOldId  = aDef.Curve2DRepId;
  aDef.Curve2DRepId                           = BRepGraph_CoEdgeCurve2DRepId();
  if (anOldId.IsValid() && anOldId.IsValid(aStorage.NbCoEdgeCurves2D()))
  {
    BRepGraphInc::CoEdgeCurve2DRep& aUse = aStorage.ChangeCoEdgeCurve2DRep(anOldId);
    aUse.ParamFirst                      = 0.0;
    aUse.ParamLast                       = 0.0;
  }
}

//=================================================================================================

void BRepGraph::EditorView::WireOps::SetRefOrientation(
  const BRepGraph_WireRefId             theWireRef,
  const BRepGraphInc::ParityOrientation theOrientation)
{
  myGraph->Editor().requireUnlocked(theWireRef, "BRepGraph::EditorView: locked item");
  myGraph->myData->myIncStorage.ChangeWireRef(theWireRef).Orientation = theOrientation;
  myGraph->markRefModified(theWireRef);
}

//=================================================================================================

void BRepGraph::EditorView::WireOps::SetRefOrientation(
  BRepGraph_MutGuard<BRepGraphInc::WireRef>& theMut,
  const BRepGraphInc::ParityOrientation      theOrientation)
{
  theMut.Internal().Orientation = theOrientation;
}

//=================================================================================================

void BRepGraph::EditorView::ShellOps::SetRefOrientation(
  const BRepGraph_ShellRefId            theShellRef,
  const BRepGraphInc::ParityOrientation theOrientation)
{
  myGraph->Editor().requireUnlocked(theShellRef, "BRepGraph::EditorView: locked item");
  myGraph->myData->myIncStorage.ChangeShellRef(theShellRef).Orientation = theOrientation;
  myGraph->markRefModified(theShellRef);
}

//=================================================================================================

void BRepGraph::EditorView::ShellOps::SetRefOrientation(
  BRepGraph_MutGuard<BRepGraphInc::ShellRef>& theMut,
  const BRepGraphInc::ParityOrientation       theOrientation)
{
  theMut.Internal().Orientation = theOrientation;
}

//=================================================================================================

void BRepGraph::EditorView::VertexOps::SetTolerance(const BRepGraph_VertexId theVertex,
                                                    const double             theTolerance)
{
  myGraph->Editor().requireUnlocked(theVertex, "BRepGraph::EditorView: locked item");
  myGraph->myData->myIncStorage.ChangeVertex(theVertex).Tolerance = theTolerance;
  myGraph->markModified(theVertex);
}

//=================================================================================================

void BRepGraph::EditorView::VertexOps::SetTolerance(
  BRepGraph_MutGuard<BRepGraphInc::VertexDef>& theMut,
  const double                                 theTolerance)
{
  theMut.Internal().Tolerance = theTolerance;
}

//=================================================================================================

void BRepGraph::EditorView::VertexOps::SetRefOrientation(
  const BRepGraph_VertexRefId           theVertexRef,
  const BRepGraphInc::ParityOrientation theOrientation)
{
  myGraph->Editor().requireUnlocked(theVertexRef, "BRepGraph::EditorView: locked item");
  myGraph->myData->myIncStorage.ChangeVertexRef(theVertexRef).Orientation = theOrientation;
  myGraph->markRefModified(theVertexRef);
}

//=================================================================================================

void BRepGraph::EditorView::VertexOps::SetRefOrientation(
  BRepGraph_MutGuard<BRepGraphInc::VertexRef>& theMut,
  const BRepGraphInc::ParityOrientation        theOrientation)
{
  theMut.Internal().Orientation = theOrientation;
}

//=================================================================================================

void BRepGraph::EditorView::SolidOps::SetRefOrientation(
  const BRepGraph_SolidRefId            theSolidRef,
  const BRepGraphInc::ParityOrientation theOrientation)
{
  myGraph->Editor().requireUnlocked(theSolidRef, "BRepGraph::EditorView: locked item");
  myGraph->myData->myIncStorage.ChangeSolidRef(theSolidRef).Orientation = theOrientation;
  myGraph->markRefModified(theSolidRef);
}

//=================================================================================================

void BRepGraph::EditorView::SolidOps::SetRefOrientation(
  BRepGraph_MutGuard<BRepGraphInc::SolidRef>& theMut,
  const BRepGraphInc::ParityOrientation       theOrientation)
{
  theMut.Internal().Orientation = theOrientation;
}

//=================================================================================================

void BRepGraph::EditorView::OccurrenceOps::SetChildNodeId(
  const BRepGraph_OccurrenceId theOccurrence,
  const BRepGraph_NodeId       theChildNodeId)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveOccurrence(aStorage, theOccurrence))
  {
    return;
  }
  if (!isActiveOccurrenceChild(aStorage, theChildNodeId))
  {
    return;
  }
  myGraph->Editor().requireUnlocked(theOccurrence, "BRepGraph::EditorView: locked item");
  BRepGraphInc::OccurrenceDef& anOcc = aStorage.ChangeOccurrence(theOccurrence);
  if (anOcc.ChildNodeId == theChildNodeId)
  {
    return;
  }
  const BRepGraph_NodeId anOldChild = anOcc.ChildNodeId;
  anOcc.ChildNodeId                 = theChildNodeId;
  rebindOccurrenceChild(aStorage, theOccurrence, anOldChild, theChildNodeId);
  myGraph->markModified(theOccurrence);
}

//=================================================================================================

void BRepGraph::EditorView::OccurrenceOps::SetChildNodeId(
  BRepGraph_MutGuard<BRepGraphInc::OccurrenceDef>& theMut,
  const BRepGraph_NodeId                           theChildNodeId)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveOccurrenceChild(aStorage, theChildNodeId))
  {
    return;
  }
  if (theMut->ChildNodeId == theChildNodeId)
  {
    return;
  }
  const BRepGraph_NodeId anOldChild = theMut->ChildNodeId;
  theMut.Internal().ChildNodeId     = theChildNodeId;
  rebindOccurrenceChild(aStorage, theMut.Id(), anOldChild, theChildNodeId);
}

//=================================================================================================

void BRepGraph::EditorView::GenOps::SetChildRefOrientation(
  const BRepGraph_ChildRefId            theChildRef,
  const BRepGraphInc::ParityOrientation theOrientation)
{
  myGraph->Editor().requireUnlocked(theChildRef, "BRepGraph::EditorView: locked item");
  myGraph->myData->myIncStorage.ChangeChildRef(theChildRef).Orientation = theOrientation;
  myGraph->markRefModified(theChildRef);
}

//=================================================================================================

void BRepGraph::EditorView::GenOps::SetChildRefOrientation(
  BRepGraph_MutGuard<BRepGraphInc::ChildRef>& theMut,
  const BRepGraphInc::ParityOrientation       theOrientation)
{
  theMut.Internal().Orientation = theOrientation;
}

//=================================================================================================

//=================================================================================================

namespace
{
BRepGraph_VertexId vertexFromRef(const BRepGraphInc_Storage& theStorage,
                                 const BRepGraph_VertexRefId theRef)
{
  if (!theRef.IsValid(theStorage.NbVertexRefs()))
  {
    return BRepGraph_VertexId();
  }
  return theStorage.VertexRef(theRef).ChildVertexId;
}

void rebindVertexEdge(BRepGraphInc_Storage&       theStorage,
                      const BRepGraph_VertexId    theOldVtx,
                      const BRepGraph_VertexId    theNewVtx,
                      const BRepGraph_EdgeId      theEdge,
                      const BRepGraph_VertexRefId theExcludingRef)
{
  theStorage.RebindVertexEdge(theOldVtx, theNewVtx, theEdge, theExcludingRef);
}

bool isVertexRefOwnedByActiveEdge(const BRepGraphInc_Storage& theStorage,
                                  const BRepGraph_VertexRefId theVertexRef)
{
  if (!theVertexRef.IsValid())
  {
    return false;
  }
  const BRepGraph_VertexId aVertexId = vertexFromRef(theStorage, theVertexRef);
  if (!aVertexId.IsValid(theStorage.NbVertices()))
  {
    return false;
  }
  for (const BRepGraph_EdgeId& anEdgeId : theStorage.VertexRelations(aVertexId).EdgeIds)
  {
    if (!anEdgeId.IsValid(theStorage.NbEdges()) || theStorage.IsRemoved(anEdgeId))
    {
      continue;
    }
    const BRepGraphInc::EdgeDef& anEdge = theStorage.Edge(anEdgeId);
    if (anEdge.StartVertexRefId == theVertexRef || anEdge.EndVertexRefId == theVertexRef)
    {
      return true;
    }
  }
  return false;
}

bool isUsableUnownedVertexRef(const BRepGraphInc_Storage& theStorage,
                              const BRepGraph_VertexRefId theVertexRef)
{
  return !theVertexRef.IsValid()
         || (theVertexRef.IsValid(theStorage.NbVertexRefs()) && !theStorage.IsRemoved(theVertexRef)
             && !isVertexRefOwnedByActiveEdge(theStorage, theVertexRef));
}

void rebindVertexRef(BRepGraphInc_Storage&       theStorage,
                     const BRepGraph_VertexRefId theVertexRef,
                     const BRepGraph_VertexId    theOldVtx,
                     const BRepGraph_VertexId    theNewVtx)
{
  theStorage.RebindVertexRef(theVertexRef, theOldVtx, theNewVtx);
}
} // namespace

void BRepGraph::EditorView::EdgeOps::SetStartVertexRefId(const BRepGraph_EdgeId      theEdge,
                                                         const BRepGraph_VertexRefId theVertexRef)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  myGraph->Editor().requireUnlocked(theEdge, "BRepGraph::EditorView: locked item");
  BRepGraphInc::EdgeDef& anEdge = aStorage.ChangeEdge(theEdge);
  if (anEdge.StartVertexRefId == theVertexRef)
  {
    return;
  }
  Standard_ASSERT_RETURN(isUsableUnownedVertexRef(aStorage, theVertexRef),
                         "SetStartVertexRefId: vertex ref must be active and unowned",
                         Standard_VOID_RETURN);
  const BRepGraph_VertexRefId anOldRef = anEdge.StartVertexRefId;
  myGraph->Editor().requireUnlocked(anOldRef, "BRepGraph::EditorView: locked item");
  const BRepGraph_VertexId anOldVtx = vertexFromRef(aStorage, anEdge.StartVertexRefId);
  const BRepGraph_VertexId aNewVtx  = vertexFromRef(aStorage, theVertexRef);
  anEdge.StartVertexRefId           = theVertexRef;
  rebindVertexEdge(aStorage, anOldVtx, aNewVtx, theEdge, BRepGraph_VertexRefId());
  myGraph->Editor().Gen().RemoveRef(anOldRef);
  myGraph->markModified(theEdge);
}

void BRepGraph::EditorView::EdgeOps::SetStartVertexRefId(
  BRepGraph_MutGuard<BRepGraphInc::EdgeDef>& theMut,
  const BRepGraph_VertexRefId                theVertexRef)
{
  if (theMut->StartVertexRefId == theVertexRef)
  {
    return;
  }
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  Standard_ASSERT_RETURN(isUsableUnownedVertexRef(aStorage, theVertexRef),
                         "SetStartVertexRefId: vertex ref must be active and unowned",
                         Standard_VOID_RETURN);
  const BRepGraph_VertexRefId anOldRef = theMut->StartVertexRefId;
  myGraph->Editor().requireUnlocked(anOldRef, "BRepGraph::EditorView: locked item");
  const BRepGraph_VertexId anOldVtx  = vertexFromRef(aStorage, theMut->StartVertexRefId);
  const BRepGraph_VertexId aNewVtx   = vertexFromRef(aStorage, theVertexRef);
  theMut.Internal().StartVertexRefId = theVertexRef;
  rebindVertexEdge(aStorage, anOldVtx, aNewVtx, theMut.Id(), BRepGraph_VertexRefId());
  myGraph->Editor().Gen().RemoveRef(anOldRef);
}

//=================================================================================================

void BRepGraph::EditorView::EdgeOps::SetEndVertexRefId(const BRepGraph_EdgeId      theEdge,
                                                       const BRepGraph_VertexRefId theVertexRef)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  myGraph->Editor().requireUnlocked(theEdge, "BRepGraph::EditorView: locked item");
  BRepGraphInc::EdgeDef& anEdge = aStorage.ChangeEdge(theEdge);
  if (anEdge.EndVertexRefId == theVertexRef)
  {
    return;
  }
  Standard_ASSERT_RETURN(isUsableUnownedVertexRef(aStorage, theVertexRef),
                         "SetEndVertexRefId: vertex ref must be active and unowned",
                         Standard_VOID_RETURN);
  const BRepGraph_VertexRefId anOldRef = anEdge.EndVertexRefId;
  myGraph->Editor().requireUnlocked(anOldRef, "BRepGraph::EditorView: locked item");
  const BRepGraph_VertexId anOldVtx = vertexFromRef(aStorage, anEdge.EndVertexRefId);
  const BRepGraph_VertexId aNewVtx  = vertexFromRef(aStorage, theVertexRef);
  anEdge.EndVertexRefId             = theVertexRef;
  rebindVertexEdge(aStorage, anOldVtx, aNewVtx, theEdge, BRepGraph_VertexRefId());
  myGraph->Editor().Gen().RemoveRef(anOldRef);
  myGraph->markModified(theEdge);
}

void BRepGraph::EditorView::EdgeOps::SetEndVertexRefId(
  BRepGraph_MutGuard<BRepGraphInc::EdgeDef>& theMut,
  const BRepGraph_VertexRefId                theVertexRef)
{
  if (theMut->EndVertexRefId == theVertexRef)
  {
    return;
  }
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  Standard_ASSERT_RETURN(isUsableUnownedVertexRef(aStorage, theVertexRef),
                         "SetEndVertexRefId: vertex ref must be active and unowned",
                         Standard_VOID_RETURN);
  const BRepGraph_VertexRefId anOldRef = theMut->EndVertexRefId;
  myGraph->Editor().requireUnlocked(anOldRef, "BRepGraph::EditorView: locked item");
  const BRepGraph_VertexId anOldVtx = vertexFromRef(aStorage, theMut->EndVertexRefId);
  const BRepGraph_VertexId aNewVtx  = vertexFromRef(aStorage, theVertexRef);
  theMut.Internal().EndVertexRefId  = theVertexRef;
  rebindVertexEdge(aStorage, anOldVtx, aNewVtx, theMut.Id(), BRepGraph_VertexRefId());
  myGraph->Editor().Gen().RemoveRef(anOldRef);
}

//=================================================================================================

namespace
{
//! Refresh derived relations after CoEdge.ChildEdgeId rewrite.
void rebindCoEdgeEdge(BRepGraphInc_Storage&    theStorage,
                      const BRepGraph_CoEdgeId theCoEdge,
                      const BRepGraph_EdgeId   theOldEdge,
                      const BRepGraph_EdgeId   theNewEdge)
{
  theStorage.RebindCoEdgeEdge(theCoEdge, theOldEdge, theNewEdge);
}

void rebindWireRef(BRepGraphInc_Storage&     theStorage,
                   const BRepGraph_WireRefId theWireRef,
                   const BRepGraph_WireId    theOldWire,
                   const BRepGraph_WireId    theNewWire)
{
  theStorage.RebindWireRef(theWireRef, theOldWire, theNewWire);
}

void rebindFaceRef(BRepGraphInc_Storage&     theStorage,
                   const BRepGraph_FaceRefId theFaceRef,
                   const BRepGraph_FaceId    theOldFace,
                   const BRepGraph_FaceId    theNewFace)
{
  theStorage.RebindFaceRef(theFaceRef, theOldFace, theNewFace);
}

void rebindShellRef(BRepGraphInc_Storage&      theStorage,
                    const BRepGraph_ShellRefId theShellRef,
                    const BRepGraph_ShellId    theOldShell,
                    const BRepGraph_ShellId    theNewShell)
{
  theStorage.RebindShellRef(theShellRef, theOldShell, theNewShell);
}

void rebindSolidRef(BRepGraphInc_Storage&      theStorage,
                    const BRepGraph_SolidRefId theSolidRef,
                    const BRepGraph_SolidId    theOldSolid,
                    const BRepGraph_SolidId    theNewSolid)
{
  theStorage.RebindSolidRef(theSolidRef, theOldSolid, theNewSolid);
}

void rebindChildRef(BRepGraphInc_Storage&      theStorage,
                    const BRepGraph_ChildRefId theChildRef,
                    const BRepGraph_NodeId     theOldChild,
                    const BRepGraph_NodeId     theNewChild)
{
  theStorage.RebindChildRef(theChildRef, theOldChild, theNewChild);
}

} // namespace

void BRepGraph::EditorView::CoEdgeOps::SetChildEdgeId(const BRepGraph_CoEdgeId theCoEdge,
                                                      const BRepGraph_EdgeId   theEdge)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  myGraph->Editor().requireUnlocked(theCoEdge, "BRepGraph::EditorView: locked item");
  BRepGraphInc::CoEdgeDef& aDef = aStorage.ChangeCoEdge(theCoEdge);
  if (aDef.ChildEdgeId == theEdge)
  {
    return;
  }
  const BRepGraph_EdgeId anOldEdge = aDef.ChildEdgeId;
  aDef.ChildEdgeId                 = theEdge;
  rebindCoEdgeEdge(aStorage, theCoEdge, anOldEdge, theEdge);
  myGraph->markModified(theCoEdge);
}

void BRepGraph::EditorView::CoEdgeOps::SetChildEdgeId(
  BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef>& theMut,
  const BRepGraph_EdgeId                       theEdge)
{
  if (theMut->ChildEdgeId == theEdge)
  {
    return;
  }
  BRepGraphInc_Storage&  aStorage  = myGraph->myData->myIncStorage;
  const BRepGraph_EdgeId anOldEdge = theMut->ChildEdgeId;
  theMut.Internal().ChildEdgeId    = theEdge;
  rebindCoEdgeEdge(aStorage, theMut.Id(), anOldEdge, theEdge);
}

//=================================================================================================

void BRepGraph::EditorView::CoEdgeOps::SetFaceId(const BRepGraph_CoEdgeId theCoEdge,
                                                 const BRepGraph_FaceId   theFace)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  myGraph->Editor().requireUnlocked(theCoEdge, "BRepGraph::EditorView: locked item");
  BRepGraphInc::CoEdgeDef& aDef = aStorage.ChangeCoEdge(theCoEdge);
  if (aDef.FaceId == theFace)
  {
    return;
  }
  aDef.FaceId = theFace;
  clearCoEdgeFaceScopedRepresentations(aStorage, aDef);
  myGraph->markModified(theCoEdge);
}

void BRepGraph::EditorView::CoEdgeOps::SetFaceId(
  BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef>& theMut,
  const BRepGraph_FaceId                       theFace)
{
  if (theMut->FaceId == theFace)
  {
    return;
  }
  BRepGraphInc_Storage&    aStorage = myGraph->myData->myIncStorage;
  BRepGraphInc::CoEdgeDef& aCoEdge  = theMut.Internal();
  aCoEdge.FaceId                    = theFace;
  clearCoEdgeFaceScopedRepresentations(aStorage, aCoEdge);
}

//=================================================================================================

void BRepGraph::EditorView::VertexOps::SetRefChildVertexId(const BRepGraph_VertexRefId theVertexRef,
                                                           const BRepGraph_VertexId    theVertex)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  myGraph->Editor().requireUnlocked(theVertexRef, "BRepGraph::EditorView: locked item");
  BRepGraphInc::VertexRef& aRef = aStorage.ChangeVertexRef(theVertexRef);
  if (aRef.ChildVertexId == theVertex)
  {
    return;
  }
  const BRepGraph_VertexId anOldVtx = aRef.ChildVertexId;
  rebindVertexRef(aStorage, theVertexRef, anOldVtx, theVertex);
  myGraph->markRefModified(theVertexRef);
}

void BRepGraph::EditorView::VertexOps::SetRefChildVertexId(
  BRepGraph_MutGuard<BRepGraphInc::VertexRef>& theMut,
  const BRepGraph_VertexId                     theVertex)
{
  if (theMut->ChildVertexId == theVertex)
  {
    return;
  }
  BRepGraphInc_Storage&    aStorage = myGraph->myData->myIncStorage;
  const BRepGraph_VertexId anOldVtx = theMut->ChildVertexId;
  rebindVertexRef(aStorage, theMut.Id(), anOldVtx, theVertex);
}

//=================================================================================================

void BRepGraph::EditorView::WireOps::SetRefChildWireId(const BRepGraph_WireRefId theWireRef,
                                                       const BRepGraph_WireId    theWire)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  myGraph->Editor().requireUnlocked(theWireRef, "BRepGraph::EditorView: locked item");
  BRepGraphInc::WireRef& aRef = aStorage.ChangeWireRef(theWireRef);
  if (aRef.ChildWireId == theWire)
  {
    return;
  }
  const BRepGraph_WireId anOldWire = aRef.ChildWireId;
  aRef.ChildWireId                 = theWire;
  rebindWireRef(aStorage, theWireRef, anOldWire, theWire);
  myGraph->markRefModified(theWireRef);
}

void BRepGraph::EditorView::WireOps::SetRefChildWireId(
  BRepGraph_MutGuard<BRepGraphInc::WireRef>& theMut,
  const BRepGraph_WireId                     theWire)
{
  if (theMut->ChildWireId == theWire)
  {
    return;
  }
  BRepGraphInc_Storage&  aStorage  = myGraph->myData->myIncStorage;
  const BRepGraph_WireId anOldWire = theMut->ChildWireId;
  theMut.Internal().ChildWireId    = theWire;
  rebindWireRef(aStorage, theMut.Id(), anOldWire, theWire);
}

//=================================================================================================

void BRepGraph::EditorView::FaceOps::SetRefFaceId(const BRepGraph_FaceRefId theFaceRef,
                                                  const BRepGraph_FaceId    theFace)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  myGraph->Editor().requireUnlocked(theFaceRef, "BRepGraph::EditorView: locked item");
  BRepGraphInc::FaceRef& aRef = aStorage.ChangeFaceRef(theFaceRef);
  if (aRef.ChildFaceId == theFace)
  {
    return;
  }
  const BRepGraph_FaceId anOldFace = aRef.ChildFaceId;
  aRef.ChildFaceId                 = theFace;
  rebindFaceRef(aStorage, theFaceRef, anOldFace, theFace);
  myGraph->markRefModified(theFaceRef);
}

void BRepGraph::EditorView::FaceOps::SetRefFaceId(BRepGraph_MutGuard<BRepGraphInc::FaceRef>& theMut,
                                                  const BRepGraph_FaceId theFace)
{
  if (theMut->ChildFaceId == theFace)
  {
    return;
  }
  BRepGraphInc_Storage&  aStorage  = myGraph->myData->myIncStorage;
  const BRepGraph_FaceId anOldFace = theMut->ChildFaceId;
  theMut.Internal().ChildFaceId    = theFace;
  rebindFaceRef(aStorage, theMut.Id(), anOldFace, theFace);
}

//=================================================================================================

void BRepGraph::EditorView::ShellOps::SetRefChildShellId(const BRepGraph_ShellRefId theShellRef,
                                                         const BRepGraph_ShellId    theShell)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  myGraph->Editor().requireUnlocked(theShellRef, "BRepGraph::EditorView: locked item");
  BRepGraphInc::ShellRef& aRef = aStorage.ChangeShellRef(theShellRef);
  if (aRef.ChildShellId == theShell)
  {
    return;
  }
  const BRepGraph_ShellId anOldShell = aRef.ChildShellId;
  aRef.ChildShellId                  = theShell;
  rebindShellRef(aStorage, theShellRef, anOldShell, theShell);
  myGraph->markRefModified(theShellRef);
}

void BRepGraph::EditorView::ShellOps::SetRefChildShellId(
  BRepGraph_MutGuard<BRepGraphInc::ShellRef>& theMut,
  const BRepGraph_ShellId                     theShell)
{
  if (theMut->ChildShellId == theShell)
  {
    return;
  }
  BRepGraphInc_Storage&   aStorage   = myGraph->myData->myIncStorage;
  const BRepGraph_ShellId anOldShell = theMut->ChildShellId;
  theMut.Internal().ChildShellId     = theShell;
  rebindShellRef(aStorage, theMut.Id(), anOldShell, theShell);
}

//=================================================================================================

void BRepGraph::EditorView::SolidOps::SetRefChildSolidId(const BRepGraph_SolidRefId theSolidRef,
                                                         const BRepGraph_SolidId    theSolid)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  myGraph->Editor().requireUnlocked(theSolidRef, "BRepGraph::EditorView: locked item");
  BRepGraphInc::SolidRef& aRef = aStorage.ChangeSolidRef(theSolidRef);
  if (aRef.ChildSolidId == theSolid)
  {
    return;
  }
  const BRepGraph_SolidId anOldSolid = aRef.ChildSolidId;
  aRef.ChildSolidId                  = theSolid;
  rebindSolidRef(aStorage, theSolidRef, anOldSolid, theSolid);
  myGraph->markRefModified(theSolidRef);
}

void BRepGraph::EditorView::SolidOps::SetRefChildSolidId(
  BRepGraph_MutGuard<BRepGraphInc::SolidRef>& theMut,
  const BRepGraph_SolidId                     theSolid)
{
  if (theMut->ChildSolidId == theSolid)
  {
    return;
  }
  BRepGraphInc_Storage&   aStorage   = myGraph->myData->myIncStorage;
  const BRepGraph_SolidId anOldSolid = theMut->ChildSolidId;
  theMut.Internal().ChildSolidId     = theSolid;
  rebindSolidRef(aStorage, theMut.Id(), anOldSolid, theSolid);
}

//=================================================================================================

//=================================================================================================

void BRepGraph::EditorView::GenOps::SetChildRefChildNodeId(const BRepGraph_ChildRefId theChildRef,
                                                           const BRepGraph_NodeId     theChild)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  myGraph->Editor().requireUnlocked(theChildRef, "BRepGraph::EditorView: locked item");
  BRepGraphInc::ChildRef& aRef = aStorage.ChangeChildRef(theChildRef);
  if (aRef.ChildNodeId == theChild)
  {
    return;
  }
  const BRepGraph_NodeId anOldChild = aRef.ChildNodeId;
  aRef.ChildNodeId                  = theChild;
  rebindChildRef(aStorage, theChildRef, anOldChild, theChild);
  myGraph->markRefModified(theChildRef);
}

void BRepGraph::EditorView::GenOps::SetChildRefChildNodeId(
  BRepGraph_MutGuard<BRepGraphInc::ChildRef>& theMut,
  const BRepGraph_NodeId                      theChild)
{
  if (theMut->ChildNodeId == theChild)
  {
    return;
  }
  BRepGraphInc_Storage&  aStorage   = myGraph->myData->myIncStorage;
  const BRepGraph_NodeId anOldChild = theMut->ChildNodeId;
  theMut.Internal().ChildNodeId     = theChild;
  rebindChildRef(aStorage, theMut.Id(), anOldChild, theChild);
}

//=================================================================================================

void BRepGraph::EditorView::OccurrenceOps::SetRefChildOccurrenceId(
  const BRepGraph_OccurrenceRefId theOccurrenceRef,
  const BRepGraph_OccurrenceId    theOccurrence)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  myGraph->Editor().requireUnlocked(theOccurrenceRef, "BRepGraph::EditorView: locked item");
  BRepGraphInc::OccurrenceRef& aRef = aStorage.ChangeOccurrenceRef(theOccurrenceRef);
  if (aRef.ChildOccurrenceId == theOccurrence)
  {
    return;
  }
  Standard_ASSERT_RETURN(!occurrenceHasOtherActiveRef(aStorage, theOccurrence, theOccurrenceRef),
                         "SetRefChildOccurrenceId: occurrence definition is already owned",
                         Standard_VOID_RETURN);
  const BRepGraph_OccurrenceId anOldOccurrence = aRef.ChildOccurrenceId;
  aRef.ChildOccurrenceId                       = theOccurrence;
  rebindOccurrenceRefParentProduct(aStorage, theOccurrenceRef, anOldOccurrence, theOccurrence);
  myGraph->markRefModified(theOccurrenceRef);
}

void BRepGraph::EditorView::OccurrenceOps::SetRefChildOccurrenceId(
  BRepGraph_MutGuard<BRepGraphInc::OccurrenceRef>& theMut,
  const BRepGraph_OccurrenceId                     theOccurrence)
{
  if (theMut->ChildOccurrenceId == theOccurrence)
  {
    return;
  }
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  Standard_ASSERT_RETURN(!occurrenceHasOtherActiveRef(aStorage, theOccurrence, theMut.Id()),
                         "SetRefChildOccurrenceId: occurrence definition is already owned",
                         Standard_VOID_RETURN);
  const BRepGraph_OccurrenceId anOldOccurrence = theMut->ChildOccurrenceId;
  theMut.Internal().ChildOccurrenceId          = theOccurrence;
  rebindOccurrenceRefParentProduct(aStorage, theMut.Id(), anOldOccurrence, theOccurrence);
}

//=================================================================================================
// Owner-scoped EdgeOps setters (Use-based API)
//=================================================================================================

void BRepGraph::EditorView::EdgeOps::SetCurve(const BRepGraph_EdgeId         theEdge,
                                              const occ::handle<Geom_Curve>& theCurve,
                                              const double                   theFirst,
                                              const double                   theLast)
{
  myGraph->Editor().requireUnlocked(theEdge, "BRepGraph::EditorView: locked item");
  BRepGraphInc_Storage&  aStorage = myGraph->myData->myIncStorage;
  BRepGraphInc::EdgeDef& anEdge   = aStorage.ChangeEdge(theEdge);

  // Create or update the EdgeCurve3DRep (new owned use record)
  if (!theCurve.IsNull())
  {
    if (anEdge.Curve3DRepId.IsValid(aStorage.NbEdgeCurves3D()))
    {
      aStorage.SetRemoved(anEdge.Curve3DRepId, false);
      BRepGraphInc::EdgeCurve3DRep& aUse = aStorage.ChangeEdgeCurve3DRep(anEdge.Curve3DRepId);
      aUse.ParentEdgeId                  = theEdge;
      aUse.Curve                         = theCurve;
      aUse.ParamFirst                    = theFirst;
      aUse.ParamLast                     = theLast;
    }
    else
    {
      const BRepGraph_EdgeCurve3DRepId aRepId = aStorage.AppendEdgeCurve3DRep();
      BRepGraphInc::EdgeCurve3DRep&    aUse   = aStorage.ChangeEdgeCurve3DRep(aRepId);
      aUse.ParentEdgeId                       = theEdge;
      aUse.Curve                              = theCurve;
      aUse.ParamFirst                         = theFirst;
      aUse.ParamLast                          = theLast;
      anEdge.Curve3DRepId                     = aRepId;
    }
  }
  else
  {
    aStorage.MarkRemoved(anEdge.Curve3DRepId);
  }

  myGraph->markModified(theEdge);
}

//=================================================================================================

void BRepGraph::EditorView::EdgeOps::ClearCurve(const BRepGraph_EdgeId theEdge)
{
  myGraph->Editor().requireUnlocked(theEdge, "BRepGraph::EditorView: locked item");
  BRepGraphInc_Storage&  aStorage = myGraph->myData->myIncStorage;
  BRepGraphInc::EdgeDef& anEdge   = aStorage.ChangeEdge(theEdge);

  aStorage.MarkRemoved(anEdge.Curve3DRepId);

  myGraph->markModified(theEdge);
}

//=================================================================================================

void BRepGraph::EditorView::EdgeOps::SetPersistentPolygon3D(
  const BRepGraph_EdgeId             theEdge,
  const occ::handle<Poly_Polygon3D>& thePolygon)
{
  myGraph->Editor().requireUnlocked(theEdge, "BRepGraph::EditorView: locked item");
  BRepGraphInc_Storage&  aStorage = myGraph->myData->myIncStorage;
  BRepGraphInc::EdgeDef& anEdge   = aStorage.ChangeEdge(theEdge);

  // Create or update the EdgePolygon3DRep (new owned use record)
  if (!thePolygon.IsNull())
  {
    if (anEdge.Polygon3DRepId.IsValid(aStorage.NbEdgePolygons3D()))
    {
      aStorage.SetRemoved(anEdge.Polygon3DRepId, false);
      BRepGraphInc::EdgePolygon3DRep& aUse = aStorage.ChangeEdgePolygon3DRep(anEdge.Polygon3DRepId);
      aUse.ParentEdgeId                    = theEdge;
      aUse.Polygon                         = thePolygon;
    }
    else
    {
      const BRepGraph_EdgePolygon3DRepId aRepId = aStorage.AppendEdgePolygon3DRep();
      BRepGraphInc::EdgePolygon3DRep&    aUse   = aStorage.ChangeEdgePolygon3DRep(aRepId);
      aUse.ParentEdgeId                         = theEdge;
      aUse.Polygon                              = thePolygon;
      anEdge.Polygon3DRepId                     = aRepId;
    }
  }
  else
  {
    aStorage.MarkRemoved(anEdge.Polygon3DRepId);
  }

  myGraph->markModified(theEdge);
}

//=================================================================================================

void BRepGraph::EditorView::EdgeOps::ClearPersistentPolygon3D(const BRepGraph_EdgeId theEdge)
{
  myGraph->Editor().requireUnlocked(theEdge, "BRepGraph::EditorView: locked item");
  BRepGraphInc_Storage&  aStorage = myGraph->myData->myIncStorage;
  BRepGraphInc::EdgeDef& anEdge   = aStorage.ChangeEdge(theEdge);

  aStorage.MarkRemoved(anEdge.Polygon3DRepId);

  myGraph->markModified(theEdge);
}

//=================================================================================================
// Owner-scoped CoEdgeOps setters (Use-based API)
//=================================================================================================

void BRepGraph::EditorView::CoEdgeOps::SetPCurve(const BRepGraph_CoEdgeId         theCoEdge,
                                                 const occ::handle<Geom2d_Curve>& theCurve2d,
                                                 const double                     theFirst,
                                                 const double                     theLast)
{
  myGraph->Editor().requireUnlocked(theCoEdge, "BRepGraph::EditorView: locked item");
  BRepGraphInc_Storage&    aStorage = myGraph->myData->myIncStorage;
  BRepGraphInc::CoEdgeDef& aCoEdge  = aStorage.ChangeCoEdge(theCoEdge);

  // Create or update the CoEdgeCurve2DRep (new owned use record)
  if (!theCurve2d.IsNull())
  {
    if (aCoEdge.Curve2DRepId.IsValid(aStorage.NbCoEdgeCurves2D()))
    {
      aStorage.SetRemoved(aCoEdge.Curve2DRepId, false);
      BRepGraphInc::CoEdgeCurve2DRep& aUse = aStorage.ChangeCoEdgeCurve2DRep(aCoEdge.Curve2DRepId);
      aUse.ParentCoEdgeId                  = theCoEdge;
      aUse.Curve                           = theCurve2d;
      aUse.ParamFirst                      = theFirst;
      aUse.ParamLast                       = theLast;
    }
    else
    {
      const BRepGraph_CoEdgeCurve2DRepId aRepId = aStorage.AppendCoEdgeCurve2DRep();
      BRepGraphInc::CoEdgeCurve2DRep&    aUse   = aStorage.ChangeCoEdgeCurve2DRep(aRepId);
      aUse.ParentCoEdgeId                       = theCoEdge;
      aUse.Curve                                = theCurve2d;
      aUse.ParamFirst                           = theFirst;
      aUse.ParamLast                            = theLast;
      aCoEdge.Curve2DRepId                      = aRepId;
    }
  }
  else
  {
    aStorage.MarkRemoved(aCoEdge.Curve2DRepId);
  }

  myGraph->markModified(theCoEdge);
}

//=================================================================================================

void BRepGraph::EditorView::CoEdgeOps::ClearPCurve(const BRepGraph_CoEdgeId theCoEdge)
{
  myGraph->Editor().requireUnlocked(theCoEdge, "BRepGraph::EditorView: locked item");
  BRepGraphInc_Storage&    aStorage = myGraph->myData->myIncStorage;
  BRepGraphInc::CoEdgeDef& aCoEdge  = aStorage.ChangeCoEdge(theCoEdge);

  aStorage.MarkRemoved(aCoEdge.Curve2DRepId);

  myGraph->markModified(theCoEdge);
}

//=================================================================================================

void BRepGraph::EditorView::CoEdgeOps::SetPersistentPolygon2D(
  const BRepGraph_CoEdgeId           theCoEdge,
  const occ::handle<Poly_Polygon2D>& thePolygon)
{
  myGraph->Editor().requireUnlocked(theCoEdge, "BRepGraph::EditorView: locked item");
  BRepGraphInc_Storage&    aStorage = myGraph->myData->myIncStorage;
  BRepGraphInc::CoEdgeDef& aCoEdge  = aStorage.ChangeCoEdge(theCoEdge);

  // Create or update the CoEdgePolygon2DRep (new owned use record)
  if (!thePolygon.IsNull())
  {
    if (aCoEdge.Polygon2DRepId.IsValid(aStorage.NbCoEdgePolygons2D()))
    {
      aStorage.SetRemoved(aCoEdge.Polygon2DRepId, false);
      BRepGraphInc::CoEdgePolygon2DRep& aUse =
        aStorage.ChangeCoEdgePolygon2DRep(aCoEdge.Polygon2DRepId);
      aUse.ParentCoEdgeId = theCoEdge;
      aUse.Polygon        = thePolygon;
    }
    else
    {
      const BRepGraph_CoEdgePolygon2DRepId aRepId = aStorage.AppendCoEdgePolygon2DRep();
      BRepGraphInc::CoEdgePolygon2DRep&    aUse   = aStorage.ChangeCoEdgePolygon2DRep(aRepId);
      aUse.ParentCoEdgeId                         = theCoEdge;
      aUse.Polygon                                = thePolygon;
      aCoEdge.Polygon2DRepId                      = aRepId;
    }
  }
  else
  {
    aStorage.MarkRemoved(aCoEdge.Polygon2DRepId);
  }

  myGraph->markModified(theCoEdge);
}

//=================================================================================================

void BRepGraph::EditorView::CoEdgeOps::SetPersistentPolygonOnTri(
  const BRepGraph_CoEdgeId                        theCoEdge,
  const occ::handle<Poly_PolygonOnTriangulation>& thePolygon)
{
  myGraph->Editor().requireUnlocked(theCoEdge, "BRepGraph::EditorView: locked item");
  BRepGraphInc_Storage&    aStorage = myGraph->myData->myIncStorage;
  BRepGraphInc::CoEdgeDef& aCoEdge  = aStorage.ChangeCoEdge(theCoEdge);

  // Create or update the CoEdgePolygonOnTriRep (new owned use record)
  if (!thePolygon.IsNull())
  {
    if (aCoEdge.PolygonOnTriRepId.IsValid(aStorage.NbCoEdgePolygonsOnTri()))
    {
      aStorage.SetRemoved(aCoEdge.PolygonOnTriRepId, false);
      BRepGraphInc::CoEdgePolygonOnTriRep& aUse =
        aStorage.ChangeCoEdgePolygonOnTriRep(aCoEdge.PolygonOnTriRepId);
      aUse.ParentCoEdgeId = theCoEdge;
      aUse.Polygon        = thePolygon;
    }
    else
    {
      const BRepGraph_CoEdgePolygonOnTriRepId aRepId = aStorage.AppendCoEdgePolygonOnTriRep();
      BRepGraphInc::CoEdgePolygonOnTriRep&    aUse   = aStorage.ChangeCoEdgePolygonOnTriRep(aRepId);
      aUse.ParentCoEdgeId                            = theCoEdge;
      aUse.Polygon                                   = thePolygon;
      aCoEdge.PolygonOnTriRepId                      = aRepId;
    }
  }
  else
  {
    aStorage.MarkRemoved(aCoEdge.PolygonOnTriRepId);
  }

  myGraph->markModified(theCoEdge);
}

//=================================================================================================
// Owner-scoped FaceOps setters (Use-based API)
//=================================================================================================

void BRepGraph::EditorView::FaceOps::SetSurface(const BRepGraph_FaceId           theFace,
                                                const occ::handle<Geom_Surface>& theSurface)
{
  myGraph->Editor().requireUnlocked(theFace, "BRepGraph::EditorView: locked item");
  BRepGraphInc_Storage&  aStorage = myGraph->myData->myIncStorage;
  BRepGraphInc::FaceDef& aFace    = aStorage.ChangeFace(theFace);

  // Create or update the FaceSurfaceRep (new owned use record)
  if (!theSurface.IsNull())
  {
    if (aFace.SurfaceRepId.IsValid(aStorage.NbFaceSurfaces()))
    {
      aStorage.SetRemoved(aFace.SurfaceRepId, false);
      BRepGraphInc::FaceSurfaceRep& aUse = aStorage.ChangeFaceSurfaceRep(aFace.SurfaceRepId);
      aUse.ParentFaceId                  = theFace;
      aUse.Surface                       = theSurface;
    }
    else
    {
      const BRepGraph_FaceSurfaceRepId aRepId = aStorage.AppendFaceSurfaceRep();
      BRepGraphInc::FaceSurfaceRep&    aUse   = aStorage.ChangeFaceSurfaceRep(aRepId);
      aUse.ParentFaceId                       = theFace;
      aUse.Surface                            = theSurface;
      aFace.SurfaceRepId                      = aRepId;
    }
  }
  else
  {
    aStorage.MarkRemoved(aFace.SurfaceRepId);
  }

  myGraph->markModified(theFace);
}

//=================================================================================================

void BRepGraph::EditorView::FaceOps::ClearSurface(const BRepGraph_FaceId theFace)
{
  myGraph->Editor().requireUnlocked(theFace, "BRepGraph::EditorView: locked item");
  BRepGraphInc_Storage&  aStorage = myGraph->myData->myIncStorage;
  BRepGraphInc::FaceDef& aFace    = aStorage.ChangeFace(theFace);

  aStorage.MarkRemoved(aFace.SurfaceRepId);

  myGraph->markModified(theFace);
}

//=================================================================================================

void BRepGraph::EditorView::FaceOps::SetPersistentTriangulation(
  const BRepGraph_FaceId                 theFace,
  const occ::handle<Poly_Triangulation>& theTriangulation)
{
  myGraph->Editor().requireUnlocked(theFace, "BRepGraph::EditorView: locked item");
  BRepGraphInc_Storage&  aStorage = myGraph->myData->myIncStorage;
  BRepGraphInc::FaceDef& aFace    = aStorage.ChangeFace(theFace);

  // Create or update the FaceTriangulationRep (new owned use record)
  if (!theTriangulation.IsNull())
  {
    if (aFace.TriangulationRepId.IsValid(aStorage.NbFaceTriangulations()))
    {
      aStorage.SetRemoved(aFace.TriangulationRepId, false);
      BRepGraphInc::FaceTriangulationRep& aUse =
        aStorage.ChangeFaceTriangulationRep(aFace.TriangulationRepId);
      aUse.ParentFaceId  = theFace;
      aUse.Triangulation = theTriangulation;
    }
    else
    {
      const BRepGraph_FaceTriangulationRepId aRepId = aStorage.AppendFaceTriangulationRep();
      BRepGraphInc::FaceTriangulationRep&    aUse   = aStorage.ChangeFaceTriangulationRep(aRepId);
      aUse.ParentFaceId                             = theFace;
      aUse.Triangulation                            = theTriangulation;
      aFace.TriangulationRepId                      = aRepId;
    }
  }
  else
  {
    aStorage.MarkRemoved(aFace.TriangulationRepId);
  }

  myGraph->markModified(theFace);
}

//=================================================================================================

void BRepGraph::EditorView::FaceOps::ClearPersistentTriangulation(const BRepGraph_FaceId theFace)
{
  myGraph->Editor().requireUnlocked(theFace, "BRepGraph::EditorView: locked item");
  BRepGraphInc_Storage&  aStorage = myGraph->myData->myIncStorage;
  BRepGraphInc::FaceDef& aFace    = aStorage.ChangeFace(theFace);

  aStorage.MarkRemoved(aFace.TriangulationRepId);

  myGraph->markModified(theFace);
}
