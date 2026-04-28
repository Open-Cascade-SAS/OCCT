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
#include <BRepGraphInc_ReverseIndex.hxx>
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

//! True if no other active coedge of theEdge references theFace.
bool isLastCoEdgeOfEdgeOnFace(const BRepGraphInc_Storage& theStorage,
                              const BRepGraph_EdgeId      theEdge,
                              const BRepGraph_FaceId      theFace,
                              const BRepGraph_CoEdgeId    theExcluding)
{
  const NCollection_DynamicArray<BRepGraph_CoEdgeId>* aCoEdges =
    theStorage.ReverseIndex().CoEdgesOfEdge(theEdge);
  if (aCoEdges == nullptr)
    return true;
  for (const BRepGraph_CoEdgeId& aCoEdgeId : *aCoEdges)
  {
    if (aCoEdgeId == theExcluding)
      continue;
    if (!aCoEdgeId.IsValid(theStorage.NbCoEdges()))
      continue;
    const BRepGraphInc::CoEdgeDef& aCoEdge = theStorage.CoEdge(aCoEdgeId);
    if (!aCoEdge.IsRemoved && aCoEdge.FaceDefId == theFace)
      return false;
  }
  return true;
}

} // namespace

//=================================================================================================

void BRepGraph::EditorView::VertexOps::SetPoint(const BRepGraph_VertexId theVertex,
                                                const gp_Pnt&            thePoint)
{
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
  BRepGraphInc::CoEdgeDef& aDef = myGraph->myData->myIncStorage.ChangeCoEdge(theCoEdge);
  aDef.ParamFirst               = theFirst;
  aDef.ParamLast                = theLast;
  myGraph->markModified(theCoEdge);
}

//=================================================================================================

void BRepGraph::EditorView::CoEdgeOps::SetParamRange(
  BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef>& theMut,
  double                                       theFirst,
  double                                       theLast)
{
  theMut.Internal().ParamFirst = theFirst;
  theMut.Internal().ParamLast  = theLast;
}

//=================================================================================================

void BRepGraph::EditorView::WireOps::SetIsClosed(const BRepGraph_WireId theWire, bool theIsClosed)
{
  myGraph->myData->myIncStorage.ChangeWire(theWire).IsClosed = theIsClosed;
  myGraph->markModified(theWire);
}

//=================================================================================================

void BRepGraph::EditorView::WireOps::SetIsClosed(BRepGraph_MutGuard<BRepGraphInc::WireDef>& theMut,
                                                 bool theIsClosed)
{
  theMut.Internal().IsClosed = theIsClosed;
}

//=================================================================================================

void BRepGraph::EditorView::FaceOps::SetTolerance(const BRepGraph_FaceId theFace,
                                                  double                 theTolerance)
{
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

void BRepGraph::EditorView::FaceOps::SetNaturalRestriction(const BRepGraph_FaceId theFace,
                                                           bool theNaturalRestriction)
{
  myGraph->myData->myIncStorage.ChangeFace(theFace).NaturalRestriction = theNaturalRestriction;
  myGraph->markModified(theFace);
}

//=================================================================================================

void BRepGraph::EditorView::FaceOps::SetNaturalRestriction(
  BRepGraph_MutGuard<BRepGraphInc::FaceDef>& theMut,
  bool                                       theNaturalRestriction)
{
  theMut.Internal().NaturalRestriction = theNaturalRestriction;
}

//=================================================================================================

void BRepGraph::EditorView::FaceOps::SetTriangulationRep(const BRepGraph_FaceId             theFace,
                                                         const BRepGraph_TriangulationRepId theRep)
{
  myGraph->myData->myIncStorage.ChangeFace(theFace).TriangulationRepId = theRep;
  myGraph->markModified(theFace);
}

//=================================================================================================

void BRepGraph::EditorView::FaceOps::SetRefOrientation(const BRepGraph_FaceRefId theFaceRef,
                                                       TopAbs_Orientation        theOrientation)
{
  myGraph->myData->myIncStorage.ChangeFaceRef(theFaceRef).Orientation = theOrientation;
  myGraph->markRefModified(theFaceRef);
}

//=================================================================================================

void BRepGraph::EditorView::FaceOps::SetRefOrientation(
  BRepGraph_MutGuard<BRepGraphInc::FaceRef>& theMut,
  TopAbs_Orientation                         theOrientation)
{
  theMut.Internal().Orientation = theOrientation;
}

//=================================================================================================

void BRepGraph::EditorView::FaceOps::SetRefLocalLocation(const BRepGraph_FaceRefId theFaceRef,
                                                         const TopLoc_Location&    theLoc)
{
  myGraph->myData->myIncStorage.ChangeFaceRef(theFaceRef).LocalLocation = theLoc;
  myGraph->markRefModified(theFaceRef);
}

//=================================================================================================

void BRepGraph::EditorView::FaceOps::SetRefLocalLocation(
  BRepGraph_MutGuard<BRepGraphInc::FaceRef>& theMut,
  const TopLoc_Location&                     theLoc)
{
  theMut.Internal().LocalLocation = theLoc;
}

//=================================================================================================

void BRepGraph::EditorView::OccurrenceOps::SetRefLocalLocation(
  const BRepGraph_OccurrenceRefId theOccurrenceRef,
  const TopLoc_Location&          theLoc)
{
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

void BRepGraph::EditorView::VertexOps::SetRefLocalLocation(const BRepGraph_VertexRefId theVertexRef,
                                                           const TopLoc_Location&      theLoc)
{
  myGraph->myData->myIncStorage.ChangeVertexRef(theVertexRef).LocalLocation = theLoc;
  myGraph->markRefModified(theVertexRef);
}

//=================================================================================================

void BRepGraph::EditorView::VertexOps::SetRefLocalLocation(
  BRepGraph_MutGuard<BRepGraphInc::VertexRef>& theMut,
  const TopLoc_Location&                       theLoc)
{
  theMut.Internal().LocalLocation = theLoc;
}

//=================================================================================================

void BRepGraph::EditorView::CoEdgeOps::SetRefLocalLocation(const BRepGraph_CoEdgeRefId theCoEdgeRef,
                                                           const TopLoc_Location&      theLoc)
{
  myGraph->myData->myIncStorage.ChangeCoEdgeRef(theCoEdgeRef).LocalLocation = theLoc;
  myGraph->markRefModified(theCoEdgeRef);
}

//=================================================================================================

void BRepGraph::EditorView::CoEdgeOps::SetRefLocalLocation(
  BRepGraph_MutGuard<BRepGraphInc::CoEdgeRef>& theMut,
  const TopLoc_Location&                       theLoc)
{
  theMut.Internal().LocalLocation = theLoc;
}

//=================================================================================================

void BRepGraph::EditorView::WireOps::SetRefLocalLocation(const BRepGraph_WireRefId theWireRef,
                                                         const TopLoc_Location&    theLoc)
{
  myGraph->myData->myIncStorage.ChangeWireRef(theWireRef).LocalLocation = theLoc;
  myGraph->markRefModified(theWireRef);
}

//=================================================================================================

void BRepGraph::EditorView::WireOps::SetRefLocalLocation(
  BRepGraph_MutGuard<BRepGraphInc::WireRef>& theMut,
  const TopLoc_Location&                     theLoc)
{
  theMut.Internal().LocalLocation = theLoc;
}

//=================================================================================================

void BRepGraph::EditorView::ShellOps::SetRefLocalLocation(const BRepGraph_ShellRefId theShellRef,
                                                          const TopLoc_Location&     theLoc)
{
  myGraph->myData->myIncStorage.ChangeShellRef(theShellRef).LocalLocation = theLoc;
  myGraph->markRefModified(theShellRef);
}

//=================================================================================================

void BRepGraph::EditorView::ShellOps::SetRefLocalLocation(
  BRepGraph_MutGuard<BRepGraphInc::ShellRef>& theMut,
  const TopLoc_Location&                      theLoc)
{
  theMut.Internal().LocalLocation = theLoc;
}

//=================================================================================================

void BRepGraph::EditorView::SolidOps::SetRefLocalLocation(const BRepGraph_SolidRefId theSolidRef,
                                                          const TopLoc_Location&     theLoc)
{
  myGraph->myData->myIncStorage.ChangeSolidRef(theSolidRef).LocalLocation = theLoc;
  myGraph->markRefModified(theSolidRef);
}

//=================================================================================================

void BRepGraph::EditorView::SolidOps::SetRefLocalLocation(
  BRepGraph_MutGuard<BRepGraphInc::SolidRef>& theMut,
  const TopLoc_Location&                      theLoc)
{
  theMut.Internal().LocalLocation = theLoc;
}

//=================================================================================================

void BRepGraph::EditorView::GenOps::SetChildRefLocalLocation(const BRepGraph_ChildRefId theChildRef,
                                                             const TopLoc_Location&     theLoc)
{
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
  BRepGraphInc::EdgeDef& aDef = myGraph->myData->myIncStorage.ChangeEdge(theEdge);
  aDef.ParamFirst             = theFirst;
  aDef.ParamLast              = theLast;
  myGraph->markModified(theEdge);
}

//=================================================================================================

void BRepGraph::EditorView::EdgeOps::SetParamRange(
  BRepGraph_MutGuard<BRepGraphInc::EdgeDef>& theMut,
  const double                               theFirst,
  const double                               theLast)
{
  theMut.Internal().ParamFirst = theFirst;
  theMut.Internal().ParamLast  = theLast;
}

//=================================================================================================

void BRepGraph::EditorView::EdgeOps::SetSameParameter(const BRepGraph_EdgeId theEdge,
                                                      const bool             theSameParameter)
{
  myGraph->myData->myIncStorage.ChangeEdge(theEdge).SameParameter = theSameParameter;
  myGraph->markModified(theEdge);
}

//=================================================================================================

void BRepGraph::EditorView::EdgeOps::SetSameParameter(
  BRepGraph_MutGuard<BRepGraphInc::EdgeDef>& theMut,
  const bool                                 theSameParameter)
{
  theMut.Internal().SameParameter = theSameParameter;
}

//=================================================================================================

void BRepGraph::EditorView::EdgeOps::SetSameRange(const BRepGraph_EdgeId theEdge,
                                                  const bool             theSameRange)
{
  myGraph->myData->myIncStorage.ChangeEdge(theEdge).SameRange = theSameRange;
  myGraph->markModified(theEdge);
}

//=================================================================================================

void BRepGraph::EditorView::EdgeOps::SetSameRange(BRepGraph_MutGuard<BRepGraphInc::EdgeDef>& theMut,
                                                  const bool theSameRange)
{
  theMut.Internal().SameRange = theSameRange;
}

//=================================================================================================

void BRepGraph::EditorView::EdgeOps::SetDegenerate(const BRepGraph_EdgeId theEdge,
                                                   const bool             theIsDegenerate)
{
  myGraph->myData->myIncStorage.ChangeEdge(theEdge).IsDegenerate = theIsDegenerate;
  myGraph->markModified(theEdge);
}

//=================================================================================================

void BRepGraph::EditorView::EdgeOps::SetDegenerate(
  BRepGraph_MutGuard<BRepGraphInc::EdgeDef>& theMut,
  const bool                                 theIsDegenerate)
{
  theMut.Internal().IsDegenerate = theIsDegenerate;
}

//=================================================================================================

void BRepGraph::EditorView::EdgeOps::SetCurve3DRepId(const BRepGraph_EdgeId       theEdge,
                                                     const BRepGraph_Curve3DRepId theRep)
{
  myGraph->myData->myIncStorage.ChangeEdge(theEdge).Curve3DRepId = theRep;
  myGraph->markModified(theEdge);
}

//=================================================================================================

void BRepGraph::EditorView::EdgeOps::SetCurve3DRepId(
  BRepGraph_MutGuard<BRepGraphInc::EdgeDef>& theMut,
  const BRepGraph_Curve3DRepId               theRep)
{
  theMut.Internal().Curve3DRepId = theRep;
}

//=================================================================================================

void BRepGraph::EditorView::EdgeOps::SetPolygon3DRepId(const BRepGraph_EdgeId         theEdge,
                                                       const BRepGraph_Polygon3DRepId theRep)
{
  myGraph->myData->myIncStorage.ChangeEdge(theEdge).Polygon3DRepId = theRep;
  myGraph->markModified(theEdge);
}

//=================================================================================================

void BRepGraph::EditorView::EdgeOps::SetPolygon3DRepId(
  BRepGraph_MutGuard<BRepGraphInc::EdgeDef>& theMut,
  const BRepGraph_Polygon3DRepId             theRep)
{
  theMut.Internal().Polygon3DRepId = theRep;
}

//=================================================================================================

void BRepGraph::EditorView::FaceOps::SetTriangulationRep(
  BRepGraph_MutGuard<BRepGraphInc::FaceDef>& theMut,
  const BRepGraph_TriangulationRepId         theRep)
{
  theMut.Internal().TriangulationRepId = theRep;
}

//=================================================================================================

void BRepGraph::EditorView::FaceOps::SetSurfaceRepId(const BRepGraph_FaceId       theFace,
                                                     const BRepGraph_SurfaceRepId theRep)
{
  myGraph->myData->myIncStorage.ChangeFace(theFace).SurfaceRepId = theRep;
  myGraph->markModified(theFace);
}

//=================================================================================================

void BRepGraph::EditorView::FaceOps::SetSurfaceRepId(
  BRepGraph_MutGuard<BRepGraphInc::FaceDef>& theMut,
  const BRepGraph_SurfaceRepId               theRep)
{
  theMut.Internal().SurfaceRepId = theRep;
}

//=================================================================================================

void BRepGraph::EditorView::CoEdgeOps::SetOrientation(const BRepGraph_CoEdgeId theCoEdge,
                                                      const TopAbs_Orientation theOrientation)
{
  myGraph->myData->myIncStorage.ChangeCoEdge(theCoEdge).Orientation = theOrientation;
  myGraph->markModified(theCoEdge);
}

//=================================================================================================

void BRepGraph::EditorView::CoEdgeOps::SetOrientation(
  BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef>& theMut,
  const TopAbs_Orientation                     theOrientation)
{
  theMut.Internal().Orientation = theOrientation;
}

//=================================================================================================

void BRepGraph::EditorView::CoEdgeOps::SetUVBox(const BRepGraph_CoEdgeId theCoEdge,
                                                const gp_Pnt2d&          theUV1,
                                                const gp_Pnt2d&          theUV2)
{
  BRepGraphInc::CoEdgeDef& aDef = myGraph->myData->myIncStorage.ChangeCoEdge(theCoEdge);
  aDef.UV1                      = theUV1;
  aDef.UV2                      = theUV2;
  myGraph->markModified(theCoEdge);
}

//=================================================================================================

void BRepGraph::EditorView::CoEdgeOps::SetUVBox(BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef>& theMut,
                                                const gp_Pnt2d&                              theUV1,
                                                const gp_Pnt2d&                              theUV2)
{
  theMut.Internal().UV1 = theUV1;
  theMut.Internal().UV2 = theUV2;
}

//=================================================================================================

void BRepGraph::EditorView::CoEdgeOps::SetContinuity(const BRepGraph_CoEdgeId theCoEdge,
                                                     const GeomAbs_Shape      theContinuity)
{
  myGraph->myData->myIncStorage.ChangeCoEdge(theCoEdge).Continuity = theContinuity;
  myGraph->markModified(theCoEdge);
}

//=================================================================================================

void BRepGraph::EditorView::CoEdgeOps::SetContinuity(
  BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef>& theMut,
  const GeomAbs_Shape                          theContinuity)
{
  theMut.Internal().Continuity = theContinuity;
}

//=================================================================================================

void BRepGraph::EditorView::CoEdgeOps::SetSeamContinuity(const BRepGraph_CoEdgeId theCoEdge,
                                                         const GeomAbs_Shape      theContinuity)
{
  myGraph->myData->myIncStorage.ChangeCoEdge(theCoEdge).SeamContinuity = theContinuity;
  myGraph->markModified(theCoEdge);
}

//=================================================================================================

void BRepGraph::EditorView::CoEdgeOps::SetSeamContinuity(
  BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef>& theMut,
  const GeomAbs_Shape                          theContinuity)
{
  theMut.Internal().SeamContinuity = theContinuity;
}

//=================================================================================================

void BRepGraph::EditorView::CoEdgeOps::SetCurve2DRepId(const BRepGraph_CoEdgeId     theCoEdge,
                                                       const BRepGraph_Curve2DRepId theRep)
{
  myGraph->myData->myIncStorage.ChangeCoEdge(theCoEdge).Curve2DRepId = theRep;
  myGraph->markModified(theCoEdge);
}

//=================================================================================================

void BRepGraph::EditorView::CoEdgeOps::SetCurve2DRepId(
  BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef>& theMut,
  const BRepGraph_Curve2DRepId                 theRep)
{
  theMut.Internal().Curve2DRepId = theRep;
}

//=================================================================================================

void BRepGraph::EditorView::CoEdgeOps::SetPolygon2DRepId(const BRepGraph_CoEdgeId       theCoEdge,
                                                         const BRepGraph_Polygon2DRepId theRep)
{
  myGraph->myData->myIncStorage.ChangeCoEdge(theCoEdge).Polygon2DRepId = theRep;
  myGraph->markModified(theCoEdge);
}

//=================================================================================================

void BRepGraph::EditorView::CoEdgeOps::SetPolygon2DRepId(
  BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef>& theMut,
  const BRepGraph_Polygon2DRepId               theRep)
{
  theMut.Internal().Polygon2DRepId = theRep;
}

//=================================================================================================

void BRepGraph::EditorView::CoEdgeOps::SetPolygonOnTriRepId(
  const BRepGraph_CoEdgeId          theCoEdge,
  const BRepGraph_PolygonOnTriRepId theRep)
{
  myGraph->myData->myIncStorage.ChangeCoEdge(theCoEdge).PolygonOnTriRepId = theRep;
  myGraph->markModified(theCoEdge);
}

//=================================================================================================

void BRepGraph::EditorView::CoEdgeOps::SetPolygonOnTriRepId(
  BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef>& theMut,
  const BRepGraph_PolygonOnTriRepId            theRep)
{
  theMut.Internal().PolygonOnTriRepId = theRep;
}

//=================================================================================================

void BRepGraph::EditorView::CoEdgeOps::ClearPCurveBinding(const BRepGraph_CoEdgeId theCoEdge)
{
  BRepGraphInc::CoEdgeDef& aDef = myGraph->myData->myIncStorage.ChangeCoEdge(theCoEdge);
  aDef.Curve2DRepId             = BRepGraph_Curve2DRepId();
  aDef.ParamFirst               = 0.0;
  aDef.ParamLast                = 0.0;
  aDef.Continuity               = GeomAbs_C0;
  aDef.UV1                      = gp_Pnt2d();
  aDef.UV2                      = gp_Pnt2d();
  myGraph->markModified(theCoEdge);
}

//=================================================================================================

void BRepGraph::EditorView::CoEdgeOps::ClearPCurveBinding(
  BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef>& theMut)
{
  theMut.Internal().Curve2DRepId = BRepGraph_Curve2DRepId();
  theMut.Internal().ParamFirst   = 0.0;
  theMut.Internal().ParamLast    = 0.0;
  theMut.Internal().Continuity   = GeomAbs_C0;
  theMut.Internal().UV1          = gp_Pnt2d();
  theMut.Internal().UV2          = gp_Pnt2d();
}

//=================================================================================================

void BRepGraph::EditorView::WireOps::SetRefIsOuter(const BRepGraph_WireRefId theWireRef,
                                                   const bool                theIsOuter)
{
  myGraph->myData->myIncStorage.ChangeWireRef(theWireRef).IsOuter = theIsOuter;
  myGraph->markRefModified(theWireRef);
}

//=================================================================================================

void BRepGraph::EditorView::WireOps::SetRefIsOuter(
  BRepGraph_MutGuard<BRepGraphInc::WireRef>& theMut,
  const bool                                 theIsOuter)
{
  theMut.Internal().IsOuter = theIsOuter;
}

//=================================================================================================

void BRepGraph::EditorView::WireOps::SetRefOrientation(const BRepGraph_WireRefId theWireRef,
                                                       const TopAbs_Orientation  theOrientation)
{
  myGraph->myData->myIncStorage.ChangeWireRef(theWireRef).Orientation = theOrientation;
  myGraph->markRefModified(theWireRef);
}

//=================================================================================================

void BRepGraph::EditorView::WireOps::SetRefOrientation(
  BRepGraph_MutGuard<BRepGraphInc::WireRef>& theMut,
  const TopAbs_Orientation                   theOrientation)
{
  theMut.Internal().Orientation = theOrientation;
}

//=================================================================================================

void BRepGraph::EditorView::ShellOps::SetRefOrientation(const BRepGraph_ShellRefId theShellRef,
                                                        const TopAbs_Orientation   theOrientation)
{
  myGraph->myData->myIncStorage.ChangeShellRef(theShellRef).Orientation = theOrientation;
  myGraph->markRefModified(theShellRef);
}

//=================================================================================================

void BRepGraph::EditorView::ShellOps::SetRefOrientation(
  BRepGraph_MutGuard<BRepGraphInc::ShellRef>& theMut,
  const TopAbs_Orientation                    theOrientation)
{
  theMut.Internal().Orientation = theOrientation;
}

//=================================================================================================

void BRepGraph::EditorView::ShellOps::SetIsClosed(const BRepGraph_ShellId theShell,
                                                  const bool              theIsClosed)
{
  myGraph->myData->myIncStorage.ChangeShell(theShell).IsClosed = theIsClosed;
  myGraph->markModified(theShell);
}

//=================================================================================================

void BRepGraph::EditorView::ShellOps::SetIsClosed(
  BRepGraph_MutGuard<BRepGraphInc::ShellDef>& theMut,
  const bool                                  theIsClosed)
{
  theMut.Internal().IsClosed = theIsClosed;
}

//=================================================================================================

void BRepGraph::EditorView::VertexOps::SetTolerance(const BRepGraph_VertexId theVertex,
                                                    const double             theTolerance)
{
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

void BRepGraph::EditorView::VertexOps::SetRefOrientation(const BRepGraph_VertexRefId theVertexRef,
                                                         const TopAbs_Orientation    theOrientation)
{
  myGraph->myData->myIncStorage.ChangeVertexRef(theVertexRef).Orientation = theOrientation;
  myGraph->markRefModified(theVertexRef);
}

//=================================================================================================

void BRepGraph::EditorView::VertexOps::SetRefOrientation(
  BRepGraph_MutGuard<BRepGraphInc::VertexRef>& theMut,
  const TopAbs_Orientation                     theOrientation)
{
  theMut.Internal().Orientation = theOrientation;
}

//=================================================================================================

void BRepGraph::EditorView::EdgeOps::SetIsClosed(const BRepGraph_EdgeId theEdge,
                                                 const bool             theIsClosed)
{
  myGraph->myData->myIncStorage.ChangeEdge(theEdge).IsClosed = theIsClosed;
  myGraph->markModified(theEdge);
}

//=================================================================================================

void BRepGraph::EditorView::EdgeOps::SetIsClosed(BRepGraph_MutGuard<BRepGraphInc::EdgeDef>& theMut,
                                                 const bool theIsClosed)
{
  theMut.Internal().IsClosed = theIsClosed;
}

//=================================================================================================

void BRepGraph::EditorView::CoEdgeOps::SetSeamPairId(const BRepGraph_CoEdgeId theCoEdge,
                                                     const BRepGraph_CoEdgeId theSeamPairId)
{
  myGraph->myData->myIncStorage.ChangeCoEdge(theCoEdge).SeamPairId = theSeamPairId;
  myGraph->markModified(theCoEdge);
}

//=================================================================================================

void BRepGraph::EditorView::CoEdgeOps::SetSeamPairId(
  BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef>& theMut,
  const BRepGraph_CoEdgeId                     theSeamPairId)
{
  theMut.Internal().SeamPairId = theSeamPairId;
}

//=================================================================================================

void BRepGraph::EditorView::SolidOps::SetRefOrientation(const BRepGraph_SolidRefId theSolidRef,
                                                        const TopAbs_Orientation   theOrientation)
{
  myGraph->myData->myIncStorage.ChangeSolidRef(theSolidRef).Orientation = theOrientation;
  myGraph->markRefModified(theSolidRef);
}

//=================================================================================================

void BRepGraph::EditorView::SolidOps::SetRefOrientation(
  BRepGraph_MutGuard<BRepGraphInc::SolidRef>& theMut,
  const TopAbs_Orientation                    theOrientation)
{
  theMut.Internal().Orientation = theOrientation;
}

//=================================================================================================

void BRepGraph::EditorView::OccurrenceOps::SetChildDefId(const BRepGraph_OccurrenceId theOccurrence,
                                                         const BRepGraph_NodeId       theChildDefId)
{
  myGraph->myData->myIncStorage.ChangeOccurrence(theOccurrence).ChildDefId = theChildDefId;
  myGraph->markModified(theOccurrence);
}

//=================================================================================================

void BRepGraph::EditorView::OccurrenceOps::SetChildDefId(
  BRepGraph_MutGuard<BRepGraphInc::OccurrenceDef>& theMut,
  const BRepGraph_NodeId                           theChildDefId)
{
  theMut.Internal().ChildDefId = theChildDefId;
}

//=================================================================================================

void BRepGraph::EditorView::GenOps::SetChildRefOrientation(const BRepGraph_ChildRefId theChildRef,
                                                           const TopAbs_Orientation theOrientation)
{
  myGraph->myData->myIncStorage.ChangeChildRef(theChildRef).Orientation = theOrientation;
  myGraph->markRefModified(theChildRef);
}

//=================================================================================================

void BRepGraph::EditorView::GenOps::SetChildRefOrientation(
  BRepGraph_MutGuard<BRepGraphInc::ChildRef>& theMut,
  const TopAbs_Orientation                    theOrientation)
{
  theMut.Internal().Orientation = theOrientation;
}

//=================================================================================================

void BRepGraph::EditorView::RepOps::SetSurface(const BRepGraph_SurfaceRepId     theRep,
                                               const occ::handle<Geom_Surface>& theSurface)
{
  myGraph->myData->myIncStorage.ChangeSurfaceRep(theRep).Surface = theSurface;
  myGraph->markRepModified(theRep);
}

//=================================================================================================

void BRepGraph::EditorView::RepOps::SetSurface(BRepGraph_MutGuard<BRepGraphInc::SurfaceRep>& theMut,
                                               const occ::handle<Geom_Surface>& theSurface)
{
  theMut.Internal().Surface = theSurface;
}

//=================================================================================================

void BRepGraph::EditorView::RepOps::SetCurve3D(const BRepGraph_Curve3DRepId   theRep,
                                               const occ::handle<Geom_Curve>& theCurve)
{
  myGraph->myData->myIncStorage.ChangeCurve3DRep(theRep).Curve = theCurve;
  myGraph->markRepModified(theRep);
}

//=================================================================================================

void BRepGraph::EditorView::RepOps::SetCurve3D(BRepGraph_MutGuard<BRepGraphInc::Curve3DRep>& theMut,
                                               const occ::handle<Geom_Curve>& theCurve)
{
  theMut.Internal().Curve = theCurve;
}

//=================================================================================================

void BRepGraph::EditorView::RepOps::SetCurve2D(const BRepGraph_Curve2DRepId     theRep,
                                               const occ::handle<Geom2d_Curve>& theCurve)
{
  myGraph->myData->myIncStorage.ChangeCurve2DRep(theRep).Curve = theCurve;
  myGraph->markRepModified(theRep);
}

//=================================================================================================

void BRepGraph::EditorView::RepOps::SetCurve2D(BRepGraph_MutGuard<BRepGraphInc::Curve2DRep>& theMut,
                                               const occ::handle<Geom2d_Curve>& theCurve)
{
  theMut.Internal().Curve = theCurve;
}

//=================================================================================================

void BRepGraph::EditorView::RepOps::SetTriangulation(const BRepGraph_TriangulationRepId     theRep,
                                                     const occ::handle<Poly_Triangulation>& theTri)
{
  myGraph->myData->myIncStorage.ChangeTriangulationRep(theRep).Triangulation = theTri;
  myGraph->markRepModified(theRep);
}

//=================================================================================================

void BRepGraph::EditorView::RepOps::SetTriangulation(
  BRepGraph_MutGuard<BRepGraphInc::TriangulationRep>& theMut,
  const occ::handle<Poly_Triangulation>&              theTri)
{
  theMut.Internal().Triangulation = theTri;
}

//=================================================================================================

void BRepGraph::EditorView::RepOps::SetPolygon3D(const BRepGraph_Polygon3DRepId     theRep,
                                                 const occ::handle<Poly_Polygon3D>& thePolygon)
{
  myGraph->myData->myIncStorage.ChangePolygon3DRep(theRep).Polygon = thePolygon;
  myGraph->markRepModified(theRep);
}

//=================================================================================================

void BRepGraph::EditorView::RepOps::SetPolygon3D(
  BRepGraph_MutGuard<BRepGraphInc::Polygon3DRep>& theMut,
  const occ::handle<Poly_Polygon3D>&              thePolygon)
{
  theMut.Internal().Polygon = thePolygon;
}

//=================================================================================================

void BRepGraph::EditorView::RepOps::SetPolygon2D(const BRepGraph_Polygon2DRepId     theRep,
                                                 const occ::handle<Poly_Polygon2D>& thePolygon)
{
  myGraph->myData->myIncStorage.ChangePolygon2DRep(theRep).Polygon = thePolygon;
  myGraph->markRepModified(theRep);
}

//=================================================================================================

void BRepGraph::EditorView::RepOps::SetPolygon2D(
  BRepGraph_MutGuard<BRepGraphInc::Polygon2DRep>& theMut,
  const occ::handle<Poly_Polygon2D>&              thePolygon)
{
  theMut.Internal().Polygon = thePolygon;
}

//=================================================================================================

void BRepGraph::EditorView::RepOps::SetPolygonOnTri(
  const BRepGraph_PolygonOnTriRepId               theRep,
  const occ::handle<Poly_PolygonOnTriangulation>& thePolygon)
{
  myGraph->myData->myIncStorage.ChangePolygonOnTriRep(theRep).Polygon = thePolygon;
  myGraph->markRepModified(theRep);
}

//=================================================================================================

void BRepGraph::EditorView::RepOps::SetPolygonOnTri(
  BRepGraph_MutGuard<BRepGraphInc::PolygonOnTriRep>& theMut,
  const occ::handle<Poly_PolygonOnTriangulation>&    thePolygon)
{
  theMut.Internal().Polygon = thePolygon;
}

//=================================================================================================

void BRepGraph::EditorView::RepOps::SetPolygonOnTriTriangulationId(
  const BRepGraph_PolygonOnTriRepId  theRep,
  const BRepGraph_TriangulationRepId theTriRep)
{
  myGraph->myData->myIncStorage.ChangePolygonOnTriRep(theRep).TriangulationRepId = theTriRep;
  myGraph->markRepModified(theRep);
}

//=================================================================================================

void BRepGraph::EditorView::RepOps::SetPolygonOnTriTriangulationId(
  BRepGraph_MutGuard<BRepGraphInc::PolygonOnTriRep>& theMut,
  const BRepGraph_TriangulationRepId                 theTriRep)
{
  theMut.Internal().TriangulationRepId = theTriRep;
}

//=================================================================================================

namespace
{
BRepGraph_VertexId vertexFromRef(const BRepGraphInc_Storage& theStorage,
                                 const BRepGraph_VertexRefId theRef)
{
  if (!theRef.IsValid(theStorage.NbVertexRefs()))
    return BRepGraph_VertexId();
  return theStorage.VertexRef(theRef).VertexDefId;
}

bool isLastVertexUsageOnEdge(const BRepGraphInc_Storage& theStorage,
                             const BRepGraph_EdgeId      theEdge,
                             const BRepGraph_VertexId    theVtx,
                             const BRepGraph_VertexRefId theExcludingRef)
{
  if (!theEdge.IsValid(theStorage.NbEdges()) || !theVtx.IsValid())
    return true;
  const BRepGraphInc::EdgeDef& anEdge        = theStorage.Edge(theEdge);
  auto                         refResolvesTo = [&](const BRepGraph_VertexRefId aRefId) -> bool {
    if (!aRefId.IsValid(theStorage.NbVertexRefs()) || aRefId == theExcludingRef)
      return false;
    const BRepGraphInc::VertexRef& aRef = theStorage.VertexRef(aRefId);
    return !aRef.IsRemoved && aRef.VertexDefId == theVtx;
  };
  if (refResolvesTo(anEdge.StartVertexRefId))
    return false;
  if (refResolvesTo(anEdge.EndVertexRefId))
    return false;
  for (const BRepGraph_VertexRefId& anIntRefId : anEdge.InternalVertexRefIds)
  {
    if (refResolvesTo(anIntRefId))
      return false;
  }
  return true;
}

void rebindVertexEdge(BRepGraphInc_Storage&       theStorage,
                      const BRepGraph_VertexId    theOldVtx,
                      const BRepGraph_VertexId    theNewVtx,
                      const BRepGraph_EdgeId      theEdge,
                      const BRepGraph_VertexRefId theExcludingRef)
{
  if (theOldVtx == theNewVtx)
    return;
  BRepGraphInc_ReverseIndex& aRI = theStorage.ChangeReverseIndex();
  if (theOldVtx.IsValid()
      && isLastVertexUsageOnEdge(theStorage, theEdge, theOldVtx, theExcludingRef))
  {
    aRI.UnbindVertexFromEdge(theOldVtx, theEdge);
  }
  if (theNewVtx.IsValid())
    aRI.BindVertexToEdge(theNewVtx, theEdge);
}
} // namespace

void BRepGraph::EditorView::EdgeOps::SetStartVertexRefId(const BRepGraph_EdgeId      theEdge,
                                                         const BRepGraph_VertexRefId theVertexRef)
{
  BRepGraphInc_Storage&  aStorage = myGraph->myData->myIncStorage;
  BRepGraphInc::EdgeDef& anEdge   = aStorage.ChangeEdge(theEdge);
  if (anEdge.StartVertexRefId == theVertexRef)
    return;
  const BRepGraph_VertexId anOldVtx = vertexFromRef(aStorage, anEdge.StartVertexRefId);
  const BRepGraph_VertexId aNewVtx  = vertexFromRef(aStorage, theVertexRef);
  anEdge.StartVertexRefId           = theVertexRef;
  rebindVertexEdge(aStorage, anOldVtx, aNewVtx, theEdge, BRepGraph_VertexRefId());
  myGraph->markModified(theEdge);
}

void BRepGraph::EditorView::EdgeOps::SetStartVertexRefId(
  BRepGraph_MutGuard<BRepGraphInc::EdgeDef>& theMut,
  const BRepGraph_VertexRefId                theVertexRef)
{
  if (theMut->StartVertexRefId == theVertexRef)
    return;
  BRepGraphInc_Storage&    aStorage  = myGraph->myData->myIncStorage;
  const BRepGraph_VertexId anOldVtx  = vertexFromRef(aStorage, theMut->StartVertexRefId);
  const BRepGraph_VertexId aNewVtx   = vertexFromRef(aStorage, theVertexRef);
  theMut.Internal().StartVertexRefId = theVertexRef;
  rebindVertexEdge(aStorage, anOldVtx, aNewVtx, theMut.Id(), BRepGraph_VertexRefId());
}

//=================================================================================================

void BRepGraph::EditorView::EdgeOps::SetEndVertexRefId(const BRepGraph_EdgeId      theEdge,
                                                       const BRepGraph_VertexRefId theVertexRef)
{
  BRepGraphInc_Storage&  aStorage = myGraph->myData->myIncStorage;
  BRepGraphInc::EdgeDef& anEdge   = aStorage.ChangeEdge(theEdge);
  if (anEdge.EndVertexRefId == theVertexRef)
    return;
  const BRepGraph_VertexId anOldVtx = vertexFromRef(aStorage, anEdge.EndVertexRefId);
  const BRepGraph_VertexId aNewVtx  = vertexFromRef(aStorage, theVertexRef);
  anEdge.EndVertexRefId             = theVertexRef;
  rebindVertexEdge(aStorage, anOldVtx, aNewVtx, theEdge, BRepGraph_VertexRefId());
  myGraph->markModified(theEdge);
}

void BRepGraph::EditorView::EdgeOps::SetEndVertexRefId(
  BRepGraph_MutGuard<BRepGraphInc::EdgeDef>& theMut,
  const BRepGraph_VertexRefId                theVertexRef)
{
  if (theMut->EndVertexRefId == theVertexRef)
    return;
  BRepGraphInc_Storage&    aStorage = myGraph->myData->myIncStorage;
  const BRepGraph_VertexId anOldVtx = vertexFromRef(aStorage, theMut->EndVertexRefId);
  const BRepGraph_VertexId aNewVtx  = vertexFromRef(aStorage, theVertexRef);
  theMut.Internal().EndVertexRefId  = theVertexRef;
  rebindVertexEdge(aStorage, anOldVtx, aNewVtx, theMut.Id(), BRepGraph_VertexRefId());
}

//=================================================================================================

namespace
{
//! True if no active CoEdge in theWire (other than theExcluding) references theEdge.
bool isLastCoEdgeOfEdgeInWire(const BRepGraphInc_Storage& theStorage,
                              const BRepGraph_EdgeId      theEdge,
                              const BRepGraph_WireId      theWire,
                              const BRepGraph_CoEdgeId    theExcluding)
{
  if (!theWire.IsValid(theStorage.NbWires()))
    return true;
  const BRepGraphInc::WireDef& aWireDef = theStorage.Wire(theWire);
  for (const BRepGraph_CoEdgeRefId& aRefId : aWireDef.CoEdgeRefIds)
  {
    if (!aRefId.IsValid(theStorage.NbCoEdgeRefs()))
      continue;
    const BRepGraphInc::CoEdgeRef& aRef = theStorage.CoEdgeRef(aRefId);
    if (aRef.IsRemoved || !aRef.CoEdgeDefId.IsValid(theStorage.NbCoEdges()))
      continue;
    if (aRef.CoEdgeDefId == theExcluding)
      continue;
    const BRepGraphInc::CoEdgeDef& aCoEdge = theStorage.CoEdge(aRef.CoEdgeDefId);
    if (!aCoEdge.IsRemoved && aCoEdge.EdgeDefId == theEdge)
      return false;
  }
  return true;
}

//! Apply rev-index updates for CoEdge.EdgeDefId rewrite.
void rebindCoEdgeEdge(BRepGraphInc_Storage&          theStorage,
                      const BRepGraph_CoEdgeId       theCoEdge,
                      const BRepGraphInc::CoEdgeDef& theDef,
                      const BRepGraph_EdgeId         theOldEdge,
                      const BRepGraph_EdgeId         theNewEdge)
{
  if (theOldEdge == theNewEdge)
    return;
  BRepGraphInc_ReverseIndex& aRI = theStorage.ChangeReverseIndex();
  if (theOldEdge.IsValid())
    aRI.UnbindEdgeFromCoEdge(theOldEdge, theCoEdge);
  if (theNewEdge.IsValid())
    aRI.BindEdgeToCoEdge(theNewEdge, theCoEdge);
  // Wires that own this coedge: rebind edge-to-wire only when no other coedge of the
  // same wire still references the old edge.
  const NCollection_DynamicArray<BRepGraph_WireId>* aWires = aRI.WiresOfCoEdge(theCoEdge);
  if (aWires != nullptr)
  {
    for (const BRepGraph_WireId& aWireId : *aWires)
    {
      if (theOldEdge.IsValid()
          && isLastCoEdgeOfEdgeInWire(theStorage, theOldEdge, aWireId, theCoEdge))
      {
        aRI.UnbindEdgeFromWire(theOldEdge, aWireId);
      }
      if (theNewEdge.IsValid())
        aRI.BindEdgeToWire(theNewEdge, aWireId);
    }
  }
  // Face binding follows the (edge, face) pair; if the coedge has a face, rebind.
  if (theDef.FaceDefId.IsValid())
  {
    if (theOldEdge.IsValid()
        && isLastCoEdgeOfEdgeOnFace(theStorage, theOldEdge, theDef.FaceDefId, theCoEdge))
    {
      aRI.UnbindEdgeFromFace(theOldEdge, theDef.FaceDefId);
    }
    if (theNewEdge.IsValid())
      aRI.BindEdgeToFace(theNewEdge, theDef.FaceDefId);
  }
}

//! Apply rev-index updates for CoEdge.FaceDefId rewrite.
void rebindCoEdgeFace(BRepGraphInc_Storage&    theStorage,
                      const BRepGraph_CoEdgeId theCoEdge,
                      const BRepGraph_EdgeId   theEdge,
                      const BRepGraph_FaceId   theOldFace,
                      const BRepGraph_FaceId   theNewFace)
{
  if (theOldFace == theNewFace || !theEdge.IsValid())
    return;
  BRepGraphInc_ReverseIndex& aRI = theStorage.ChangeReverseIndex();
  if (theOldFace.IsValid() && isLastCoEdgeOfEdgeOnFace(theStorage, theEdge, theOldFace, theCoEdge))
  {
    aRI.UnbindEdgeFromFace(theEdge, theOldFace);
  }
  if (theNewFace.IsValid())
    aRI.BindEdgeToFace(theEdge, theNewFace);
}
} // namespace

void BRepGraph::EditorView::CoEdgeOps::SetEdgeDefId(const BRepGraph_CoEdgeId theCoEdge,
                                                    const BRepGraph_EdgeId   theEdge)
{
  BRepGraphInc_Storage&    aStorage = myGraph->myData->myIncStorage;
  BRepGraphInc::CoEdgeDef& aDef     = aStorage.ChangeCoEdge(theCoEdge);
  if (aDef.EdgeDefId == theEdge)
    return;
  const BRepGraph_EdgeId anOldEdge = aDef.EdgeDefId;
  aDef.EdgeDefId                   = theEdge;
  rebindCoEdgeEdge(aStorage, theCoEdge, aDef, anOldEdge, theEdge);
  myGraph->markModified(theCoEdge);
}

void BRepGraph::EditorView::CoEdgeOps::SetEdgeDefId(
  BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef>& theMut,
  const BRepGraph_EdgeId                       theEdge)
{
  if (theMut->EdgeDefId == theEdge)
    return;
  BRepGraphInc_Storage&  aStorage  = myGraph->myData->myIncStorage;
  const BRepGraph_EdgeId anOldEdge = theMut->EdgeDefId;
  theMut.Internal().EdgeDefId      = theEdge;
  rebindCoEdgeEdge(aStorage, theMut.Id(), *theMut, anOldEdge, theEdge);
}

//=================================================================================================

void BRepGraph::EditorView::CoEdgeOps::SetFaceDefId(const BRepGraph_CoEdgeId theCoEdge,
                                                    const BRepGraph_FaceId   theFace)
{
  BRepGraphInc_Storage&    aStorage = myGraph->myData->myIncStorage;
  BRepGraphInc::CoEdgeDef& aDef     = aStorage.ChangeCoEdge(theCoEdge);
  if (aDef.FaceDefId == theFace)
    return;
  const BRepGraph_FaceId anOldFace = aDef.FaceDefId;
  const BRepGraph_EdgeId anEdge    = aDef.EdgeDefId;
  aDef.FaceDefId                   = theFace;
  rebindCoEdgeFace(aStorage, theCoEdge, anEdge, anOldFace, theFace);
  myGraph->markModified(theCoEdge);
}

void BRepGraph::EditorView::CoEdgeOps::SetFaceDefId(
  BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef>& theMut,
  const BRepGraph_FaceId                       theFace)
{
  if (theMut->FaceDefId == theFace)
    return;
  BRepGraphInc_Storage&  aStorage  = myGraph->myData->myIncStorage;
  const BRepGraph_FaceId anOldFace = theMut->FaceDefId;
  const BRepGraph_EdgeId anEdge    = theMut->EdgeDefId;
  theMut.Internal().FaceDefId      = theFace;
  rebindCoEdgeFace(aStorage, theMut.Id(), anEdge, anOldFace, theFace);
}

//=================================================================================================

namespace
{
void rebindVertexRef(BRepGraphInc_Storage&       theStorage,
                     const BRepGraph_NodeId      theParent,
                     const BRepGraph_VertexId    theOldVtx,
                     const BRepGraph_VertexId    theNewVtx,
                     const BRepGraph_VertexRefId theMutatedRef)
{
  if (theOldVtx == theNewVtx)
    return;
  if (theParent.NodeKind != BRepGraph_NodeId::Kind::Edge)
    return;
  const BRepGraph_EdgeId     anEdge(theParent);
  BRepGraphInc_ReverseIndex& aRI = theStorage.ChangeReverseIndex();
  if (theOldVtx.IsValid() && isLastVertexUsageOnEdge(theStorage, anEdge, theOldVtx, theMutatedRef))
  {
    aRI.UnbindVertexFromEdge(theOldVtx, anEdge);
  }
  if (theNewVtx.IsValid())
    aRI.BindVertexToEdge(theNewVtx, anEdge);
}
} // namespace

void BRepGraph::EditorView::VertexOps::SetRefVertexDefId(const BRepGraph_VertexRefId theVertexRef,
                                                         const BRepGraph_VertexId    theVertex)
{
  BRepGraphInc_Storage&    aStorage = myGraph->myData->myIncStorage;
  BRepGraphInc::VertexRef& aRef     = aStorage.ChangeVertexRef(theVertexRef);
  if (aRef.VertexDefId == theVertex)
    return;
  const BRepGraph_VertexId anOldVtx = aRef.VertexDefId;
  const BRepGraph_NodeId   aParent  = aRef.ParentId;
  aRef.VertexDefId                  = theVertex;
  rebindVertexRef(aStorage, aParent, anOldVtx, theVertex, theVertexRef);
  myGraph->markRefModified(theVertexRef);
}

void BRepGraph::EditorView::VertexOps::SetRefVertexDefId(
  BRepGraph_MutGuard<BRepGraphInc::VertexRef>& theMut,
  const BRepGraph_VertexId                     theVertex)
{
  if (theMut->VertexDefId == theVertex)
    return;
  BRepGraphInc_Storage&    aStorage = myGraph->myData->myIncStorage;
  const BRepGraph_VertexId anOldVtx = theMut->VertexDefId;
  const BRepGraph_NodeId   aParent  = theMut->ParentId;
  theMut.Internal().VertexDefId     = theVertex;
  rebindVertexRef(aStorage, aParent, anOldVtx, theVertex, theMut.Id());
}

//=================================================================================================

namespace
{
bool isLastWireUsageOnFace(const BRepGraphInc_Storage& theStorage,
                           const BRepGraph_FaceId      theFace,
                           const BRepGraph_WireId      theWire,
                           const BRepGraph_WireRefId   theExcludingRef)
{
  if (!theFace.IsValid(theStorage.NbFaces()) || !theWire.IsValid())
    return true;
  const BRepGraphInc::FaceDef& aFaceDef = theStorage.Face(theFace);
  for (const BRepGraph_WireRefId& aRefId : aFaceDef.WireRefIds)
  {
    if (!aRefId.IsValid(theStorage.NbWireRefs()) || aRefId == theExcludingRef)
      continue;
    const BRepGraphInc::WireRef& aRef = theStorage.WireRef(aRefId);
    if (!aRef.IsRemoved && aRef.WireDefId == theWire)
      return false;
  }
  return true;
}

void rebindWireRef(BRepGraphInc_Storage&     theStorage,
                   const BRepGraph_NodeId    theParent,
                   const BRepGraph_WireId    theOldWire,
                   const BRepGraph_WireId    theNewWire,
                   const BRepGraph_WireRefId theMutatedRef)
{
  if (theOldWire == theNewWire)
    return;
  if (theParent.NodeKind != BRepGraph_NodeId::Kind::Face)
    return;
  const BRepGraph_FaceId     aFace(theParent);
  BRepGraphInc_ReverseIndex& aRI = theStorage.ChangeReverseIndex();
  if (theOldWire.IsValid() && isLastWireUsageOnFace(theStorage, aFace, theOldWire, theMutatedRef))
  {
    aRI.UnbindWireFromFace(theOldWire, aFace);
  }
  if (theNewWire.IsValid())
    aRI.BindWireToFace(theNewWire, aFace);
}
} // namespace

void BRepGraph::EditorView::WireOps::SetRefWireDefId(const BRepGraph_WireRefId theWireRef,
                                                     const BRepGraph_WireId    theWire)
{
  BRepGraphInc_Storage&  aStorage = myGraph->myData->myIncStorage;
  BRepGraphInc::WireRef& aRef     = aStorage.ChangeWireRef(theWireRef);
  if (aRef.WireDefId == theWire)
    return;
  const BRepGraph_WireId anOldWire = aRef.WireDefId;
  const BRepGraph_NodeId aParent   = aRef.ParentId;
  aRef.WireDefId                   = theWire;
  rebindWireRef(aStorage, aParent, anOldWire, theWire, theWireRef);
  myGraph->markRefModified(theWireRef);
}

void BRepGraph::EditorView::WireOps::SetRefWireDefId(
  BRepGraph_MutGuard<BRepGraphInc::WireRef>& theMut,
  const BRepGraph_WireId                     theWire)
{
  if (theMut->WireDefId == theWire)
    return;
  BRepGraphInc_Storage&  aStorage  = myGraph->myData->myIncStorage;
  const BRepGraph_WireId anOldWire = theMut->WireDefId;
  const BRepGraph_NodeId aParent   = theMut->ParentId;
  theMut.Internal().WireDefId      = theWire;
  rebindWireRef(aStorage, aParent, anOldWire, theWire, theMut.Id());
}

//=================================================================================================

namespace
{
bool isLastFaceUsageOnShell(const BRepGraphInc_Storage& theStorage,
                            const BRepGraph_ShellId     theShell,
                            const BRepGraph_FaceId      theFace,
                            const BRepGraph_FaceRefId   theExcludingRef)
{
  if (!theShell.IsValid(theStorage.NbShells()) || !theFace.IsValid())
    return true;
  const BRepGraphInc::ShellDef& aShellDef = theStorage.Shell(theShell);
  for (const BRepGraph_FaceRefId& aRefId : aShellDef.FaceRefIds)
  {
    if (!aRefId.IsValid(theStorage.NbFaceRefs()) || aRefId == theExcludingRef)
      continue;
    const BRepGraphInc::FaceRef& aRef = theStorage.FaceRef(aRefId);
    if (!aRef.IsRemoved && aRef.FaceDefId == theFace)
      return false;
  }
  return true;
}

void rebindFaceRef(BRepGraphInc_Storage&     theStorage,
                   const BRepGraph_NodeId    theParent,
                   const BRepGraph_FaceId    theOldFace,
                   const BRepGraph_FaceId    theNewFace,
                   const BRepGraph_FaceRefId theMutatedRef)
{
  if (theOldFace == theNewFace)
    return;
  if (theParent.NodeKind != BRepGraph_NodeId::Kind::Shell)
    return;
  const BRepGraph_ShellId    aShell(theParent);
  BRepGraphInc_ReverseIndex& aRI = theStorage.ChangeReverseIndex();
  if (theOldFace.IsValid() && isLastFaceUsageOnShell(theStorage, aShell, theOldFace, theMutatedRef))
  {
    aRI.UnbindFaceFromShell(theOldFace, aShell);
  }
  if (theNewFace.IsValid())
    aRI.BindFaceToShell(theNewFace, aShell);
}
} // namespace

void BRepGraph::EditorView::FaceOps::SetRefFaceDefId(const BRepGraph_FaceRefId theFaceRef,
                                                     const BRepGraph_FaceId    theFace)
{
  BRepGraphInc_Storage&  aStorage = myGraph->myData->myIncStorage;
  BRepGraphInc::FaceRef& aRef     = aStorage.ChangeFaceRef(theFaceRef);
  if (aRef.FaceDefId == theFace)
    return;
  const BRepGraph_FaceId anOldFace = aRef.FaceDefId;
  const BRepGraph_NodeId aParent   = aRef.ParentId;
  aRef.FaceDefId                   = theFace;
  rebindFaceRef(aStorage, aParent, anOldFace, theFace, theFaceRef);
  myGraph->markRefModified(theFaceRef);
}

void BRepGraph::EditorView::FaceOps::SetRefFaceDefId(
  BRepGraph_MutGuard<BRepGraphInc::FaceRef>& theMut,
  const BRepGraph_FaceId                     theFace)
{
  if (theMut->FaceDefId == theFace)
    return;
  BRepGraphInc_Storage&  aStorage  = myGraph->myData->myIncStorage;
  const BRepGraph_FaceId anOldFace = theMut->FaceDefId;
  const BRepGraph_NodeId aParent   = theMut->ParentId;
  theMut.Internal().FaceDefId      = theFace;
  rebindFaceRef(aStorage, aParent, anOldFace, theFace, theMut.Id());
}

//=================================================================================================

namespace
{
bool isLastShellUsageOnSolid(const BRepGraphInc_Storage& theStorage,
                             const BRepGraph_SolidId     theSolid,
                             const BRepGraph_ShellId     theShell,
                             const BRepGraph_ShellRefId  theExcludingRef)
{
  if (!theSolid.IsValid(theStorage.NbSolids()) || !theShell.IsValid())
    return true;
  const BRepGraphInc::SolidDef& aSolidDef = theStorage.Solid(theSolid);
  for (const BRepGraph_ShellRefId& aRefId : aSolidDef.ShellRefIds)
  {
    if (!aRefId.IsValid(theStorage.NbShellRefs()) || aRefId == theExcludingRef)
      continue;
    const BRepGraphInc::ShellRef& aRef = theStorage.ShellRef(aRefId);
    if (!aRef.IsRemoved && aRef.ShellDefId == theShell)
      return false;
  }
  return true;
}

void rebindShellRef(BRepGraphInc_Storage&      theStorage,
                    const BRepGraph_NodeId     theParent,
                    const BRepGraph_ShellId    theOldShell,
                    const BRepGraph_ShellId    theNewShell,
                    const BRepGraph_ShellRefId theMutatedRef)
{
  if (theOldShell == theNewShell)
    return;
  if (theParent.NodeKind != BRepGraph_NodeId::Kind::Solid)
    return;
  const BRepGraph_SolidId    aSolid(theParent);
  BRepGraphInc_ReverseIndex& aRI = theStorage.ChangeReverseIndex();
  if (theOldShell.IsValid()
      && isLastShellUsageOnSolid(theStorage, aSolid, theOldShell, theMutatedRef))
  {
    aRI.UnbindShellFromSolid(theOldShell, aSolid);
  }
  if (theNewShell.IsValid())
    aRI.BindShellToSolid(theNewShell, aSolid);
}
} // namespace

void BRepGraph::EditorView::ShellOps::SetRefShellDefId(const BRepGraph_ShellRefId theShellRef,
                                                       const BRepGraph_ShellId    theShell)
{
  BRepGraphInc_Storage&   aStorage = myGraph->myData->myIncStorage;
  BRepGraphInc::ShellRef& aRef     = aStorage.ChangeShellRef(theShellRef);
  if (aRef.ShellDefId == theShell)
    return;
  const BRepGraph_ShellId anOldShell = aRef.ShellDefId;
  const BRepGraph_NodeId  aParent    = aRef.ParentId;
  aRef.ShellDefId                    = theShell;
  rebindShellRef(aStorage, aParent, anOldShell, theShell, theShellRef);
  myGraph->markRefModified(theShellRef);
}

void BRepGraph::EditorView::ShellOps::SetRefShellDefId(
  BRepGraph_MutGuard<BRepGraphInc::ShellRef>& theMut,
  const BRepGraph_ShellId                     theShell)
{
  if (theMut->ShellDefId == theShell)
    return;
  BRepGraphInc_Storage&   aStorage   = myGraph->myData->myIncStorage;
  const BRepGraph_ShellId anOldShell = theMut->ShellDefId;
  const BRepGraph_NodeId  aParent    = theMut->ParentId;
  theMut.Internal().ShellDefId       = theShell;
  rebindShellRef(aStorage, aParent, anOldShell, theShell, theMut.Id());
}

//=================================================================================================

namespace
{
bool isLastSolidUsageOnCompSolid(const BRepGraphInc_Storage& theStorage,
                                 const BRepGraph_CompSolidId theCompSolid,
                                 const BRepGraph_SolidId     theSolid,
                                 const BRepGraph_SolidRefId  theExcludingRef)
{
  if (!theCompSolid.IsValid(theStorage.NbCompSolids()) || !theSolid.IsValid())
    return true;
  const BRepGraphInc::CompSolidDef& aCSDef = theStorage.CompSolid(theCompSolid);
  for (const BRepGraph_SolidRefId& aRefId : aCSDef.SolidRefIds)
  {
    if (!aRefId.IsValid(theStorage.NbSolidRefs()) || aRefId == theExcludingRef)
      continue;
    const BRepGraphInc::SolidRef& aRef = theStorage.SolidRef(aRefId);
    if (!aRef.IsRemoved && aRef.SolidDefId == theSolid)
      return false;
  }
  return true;
}

void rebindSolidRef(BRepGraphInc_Storage&      theStorage,
                    const BRepGraph_NodeId     theParent,
                    const BRepGraph_SolidId    theOldSolid,
                    const BRepGraph_SolidId    theNewSolid,
                    const BRepGraph_SolidRefId theMutatedRef)
{
  if (theOldSolid == theNewSolid)
    return;
  if (theParent.NodeKind != BRepGraph_NodeId::Kind::CompSolid)
    return;
  const BRepGraph_CompSolidId aCompSolid(theParent);
  BRepGraphInc_ReverseIndex&  aRI = theStorage.ChangeReverseIndex();
  if (theOldSolid.IsValid()
      && isLastSolidUsageOnCompSolid(theStorage, aCompSolid, theOldSolid, theMutatedRef))
  {
    aRI.UnbindSolidFromCompSolid(theOldSolid, aCompSolid);
  }
  if (theNewSolid.IsValid())
    aRI.BindSolidToCompSolid(theNewSolid, aCompSolid);
}
} // namespace

void BRepGraph::EditorView::SolidOps::SetRefSolidDefId(const BRepGraph_SolidRefId theSolidRef,
                                                       const BRepGraph_SolidId    theSolid)
{
  BRepGraphInc_Storage&   aStorage = myGraph->myData->myIncStorage;
  BRepGraphInc::SolidRef& aRef     = aStorage.ChangeSolidRef(theSolidRef);
  if (aRef.SolidDefId == theSolid)
    return;
  const BRepGraph_SolidId anOldSolid = aRef.SolidDefId;
  const BRepGraph_NodeId  aParent    = aRef.ParentId;
  aRef.SolidDefId                    = theSolid;
  rebindSolidRef(aStorage, aParent, anOldSolid, theSolid, theSolidRef);
  myGraph->markRefModified(theSolidRef);
}

void BRepGraph::EditorView::SolidOps::SetRefSolidDefId(
  BRepGraph_MutGuard<BRepGraphInc::SolidRef>& theMut,
  const BRepGraph_SolidId                     theSolid)
{
  if (theMut->SolidDefId == theSolid)
    return;
  BRepGraphInc_Storage&   aStorage   = myGraph->myData->myIncStorage;
  const BRepGraph_SolidId anOldSolid = theMut->SolidDefId;
  const BRepGraph_NodeId  aParent    = theMut->ParentId;
  theMut.Internal().SolidDefId       = theSolid;
  rebindSolidRef(aStorage, aParent, anOldSolid, theSolid, theMut.Id());
}

//=================================================================================================

namespace
{
//! Resolve EdgeDefId reachable through a CoEdgeId (via CoEdgeDef.EdgeDefId).
BRepGraph_EdgeId edgeFromCoEdge(const BRepGraphInc_Storage& theStorage,
                                const BRepGraph_CoEdgeId    theCoEdge)
{
  if (!theCoEdge.IsValid(theStorage.NbCoEdges()))
    return BRepGraph_EdgeId();
  return theStorage.CoEdge(theCoEdge).EdgeDefId;
}

//! True if no active CoEdgeRef in theWire references theEdge except theExcluding.
bool isLastCoEdgeRefOfEdgeInWire(const BRepGraphInc_Storage& theStorage,
                                 const BRepGraph_EdgeId      theEdge,
                                 const BRepGraph_WireId      theWire,
                                 const BRepGraph_CoEdgeRefId theExcluding)
{
  if (!theWire.IsValid(theStorage.NbWires()))
    return true;
  const BRepGraphInc::WireDef& aWireDef = theStorage.Wire(theWire);
  for (const BRepGraph_CoEdgeRefId& aRefId : aWireDef.CoEdgeRefIds)
  {
    if (aRefId == theExcluding)
      continue;
    if (!aRefId.IsValid(theStorage.NbCoEdgeRefs()))
      continue;
    const BRepGraphInc::CoEdgeRef& aRef = theStorage.CoEdgeRef(aRefId);
    if (aRef.IsRemoved || !aRef.CoEdgeDefId.IsValid(theStorage.NbCoEdges()))
      continue;
    const BRepGraphInc::CoEdgeDef& aCoEdge = theStorage.CoEdge(aRef.CoEdgeDefId);
    if (!aCoEdge.IsRemoved && aCoEdge.EdgeDefId == theEdge)
      return false;
  }
  return true;
}

void rebindCoEdgeRef(BRepGraphInc_Storage&       theStorage,
                     const BRepGraph_CoEdgeRefId theRefId,
                     const BRepGraph_NodeId      theParent,
                     const BRepGraph_CoEdgeId    theOldCoEdge,
                     const BRepGraph_CoEdgeId    theNewCoEdge)
{
  if (theOldCoEdge == theNewCoEdge)
    return;
  if (theParent.NodeKind != BRepGraph_NodeId::Kind::Wire)
    return;
  const BRepGraph_WireId     aWire(theParent);
  BRepGraphInc_ReverseIndex& aRI = theStorage.ChangeReverseIndex();
  if (theOldCoEdge.IsValid())
    aRI.UnbindCoEdgeFromWire(theOldCoEdge, aWire);
  if (theNewCoEdge.IsValid())
    aRI.BindCoEdgeToWire(theNewCoEdge, aWire);
  // EdgeToWires is derived from coedge.EdgeDefId x wire (deduplicated). Unbind only
  // when no other CoEdgeRef in the wire still references the old edge.
  const BRepGraph_EdgeId anOldEdge = edgeFromCoEdge(theStorage, theOldCoEdge);
  const BRepGraph_EdgeId aNewEdge  = edgeFromCoEdge(theStorage, theNewCoEdge);
  if (anOldEdge != aNewEdge)
  {
    if (anOldEdge.IsValid() && isLastCoEdgeRefOfEdgeInWire(theStorage, anOldEdge, aWire, theRefId))
    {
      aRI.UnbindEdgeFromWire(anOldEdge, aWire);
    }
    if (aNewEdge.IsValid())
      aRI.BindEdgeToWire(aNewEdge, aWire);
  }
}
} // namespace

void BRepGraph::EditorView::CoEdgeOps::SetRefCoEdgeDefId(const BRepGraph_CoEdgeRefId theCoEdgeRef,
                                                         const BRepGraph_CoEdgeId    theCoEdge)
{
  BRepGraphInc_Storage&    aStorage = myGraph->myData->myIncStorage;
  BRepGraphInc::CoEdgeRef& aRef     = aStorage.ChangeCoEdgeRef(theCoEdgeRef);
  if (aRef.CoEdgeDefId == theCoEdge)
    return;
  const BRepGraph_CoEdgeId anOldCoEdge = aRef.CoEdgeDefId;
  const BRepGraph_NodeId   aParent     = aRef.ParentId;
  aRef.CoEdgeDefId                     = theCoEdge;
  rebindCoEdgeRef(aStorage, theCoEdgeRef, aParent, anOldCoEdge, theCoEdge);
  myGraph->markRefModified(theCoEdgeRef);
}

void BRepGraph::EditorView::CoEdgeOps::SetRefCoEdgeDefId(
  BRepGraph_MutGuard<BRepGraphInc::CoEdgeRef>& theMut,
  const BRepGraph_CoEdgeId                     theCoEdge)
{
  if (theMut->CoEdgeDefId == theCoEdge)
    return;
  BRepGraphInc_Storage&    aStorage    = myGraph->myData->myIncStorage;
  const BRepGraph_CoEdgeId anOldCoEdge = theMut->CoEdgeDefId;
  const BRepGraph_NodeId   aParent     = theMut->ParentId;
  theMut.Internal().CoEdgeDefId        = theCoEdge;
  rebindCoEdgeRef(aStorage, theMut.Id(), aParent, anOldCoEdge, theCoEdge);
}

//=================================================================================================

namespace
{
bool isLastChildUsageOnCompound(const BRepGraphInc_Storage& theStorage,
                                const BRepGraph_CompoundId  theCompound,
                                const BRepGraph_NodeId      theChild,
                                const BRepGraph_ChildRefId  theExcludingRef)
{
  if (!theCompound.IsValid(theStorage.NbCompounds()) || !theChild.IsValid())
    return true;
  const BRepGraphInc::CompoundDef& aCompDef = theStorage.Compound(theCompound);
  for (const BRepGraph_ChildRefId& aRefId : aCompDef.ChildRefIds)
  {
    if (!aRefId.IsValid(theStorage.NbChildRefs()) || aRefId == theExcludingRef)
      continue;
    const BRepGraphInc::ChildRef& aRef = theStorage.ChildRef(aRefId);
    if (!aRef.IsRemoved && aRef.ChildDefId == theChild)
      return false;
  }
  return true;
}

void rebindChildRef(BRepGraphInc_Storage&      theStorage,
                    const BRepGraph_NodeId     theParent,
                    const BRepGraph_NodeId     theOldChild,
                    const BRepGraph_NodeId     theNewChild,
                    const BRepGraph_ChildRefId theMutatedRef)
{
  if (theOldChild == theNewChild)
    return;
  if (theParent.NodeKind != BRepGraph_NodeId::Kind::Compound)
    return;
  const BRepGraph_CompoundId aCompound(theParent);
  BRepGraphInc_ReverseIndex& aRI = theStorage.ChangeReverseIndex();
  if (theOldChild.IsValid()
      && isLastChildUsageOnCompound(theStorage, aCompound, theOldChild, theMutatedRef))
  {
    aRI.UnbindCompoundChild(theOldChild, aCompound);
  }
  if (theNewChild.IsValid())
    aRI.BindCompoundChild(theNewChild, aCompound);
}
} // namespace

void BRepGraph::EditorView::GenOps::SetChildRefChildDefId(const BRepGraph_ChildRefId theChildRef,
                                                          const BRepGraph_NodeId     theChild)
{
  BRepGraphInc_Storage&   aStorage = myGraph->myData->myIncStorage;
  BRepGraphInc::ChildRef& aRef     = aStorage.ChangeChildRef(theChildRef);
  if (aRef.ChildDefId == theChild)
    return;
  const BRepGraph_NodeId anOldChild = aRef.ChildDefId;
  const BRepGraph_NodeId aParent    = aRef.ParentId;
  aRef.ChildDefId                   = theChild;
  rebindChildRef(aStorage, aParent, anOldChild, theChild, theChildRef);
  myGraph->markRefModified(theChildRef);
}

void BRepGraph::EditorView::GenOps::SetChildRefChildDefId(
  BRepGraph_MutGuard<BRepGraphInc::ChildRef>& theMut,
  const BRepGraph_NodeId                      theChild)
{
  if (theMut->ChildDefId == theChild)
    return;
  BRepGraphInc_Storage&  aStorage   = myGraph->myData->myIncStorage;
  const BRepGraph_NodeId anOldChild = theMut->ChildDefId;
  const BRepGraph_NodeId aParent    = theMut->ParentId;
  theMut.Internal().ChildDefId      = theChild;
  rebindChildRef(aStorage, aParent, anOldChild, theChild, theMut.Id());
}

//=================================================================================================

namespace
{
void rebindOccurrenceRef(BRepGraphInc_Storage&        theStorage,
                         const BRepGraph_NodeId       theParent,
                         const BRepGraph_OccurrenceId theOldOcc,
                         const BRepGraph_OccurrenceId theNewOcc)
{
  if (theOldOcc == theNewOcc)
    return;
  if (theParent.NodeKind != BRepGraph_NodeId::Kind::Product)
    return;
  const BRepGraph_ProductId  aProduct(theParent);
  BRepGraphInc_ReverseIndex& aRI = theStorage.ChangeReverseIndex();
  if (theOldOcc.IsValid())
    aRI.UnbindProductOccurrence(theOldOcc, aProduct);
  if (theNewOcc.IsValid())
    aRI.BindProductOccurrence(theNewOcc, aProduct);
}
} // namespace

void BRepGraph::EditorView::OccurrenceOps::SetRefOccurrenceDefId(
  const BRepGraph_OccurrenceRefId theOccurrenceRef,
  const BRepGraph_OccurrenceId    theOccurrence)
{
  BRepGraphInc_Storage&        aStorage = myGraph->myData->myIncStorage;
  BRepGraphInc::OccurrenceRef& aRef     = aStorage.ChangeOccurrenceRef(theOccurrenceRef);
  if (aRef.OccurrenceDefId == theOccurrence)
    return;
  const BRepGraph_OccurrenceId anOldOcc = aRef.OccurrenceDefId;
  const BRepGraph_NodeId       aParent  = aRef.ParentId;
  aRef.OccurrenceDefId                  = theOccurrence;
  rebindOccurrenceRef(aStorage, aParent, anOldOcc, theOccurrence);
  myGraph->markRefModified(theOccurrenceRef);
}

void BRepGraph::EditorView::OccurrenceOps::SetRefOccurrenceDefId(
  BRepGraph_MutGuard<BRepGraphInc::OccurrenceRef>& theMut,
  const BRepGraph_OccurrenceId                     theOccurrence)
{
  if (theMut->OccurrenceDefId == theOccurrence)
    return;
  BRepGraphInc_Storage&        aStorage = myGraph->myData->myIncStorage;
  const BRepGraph_OccurrenceId anOldOcc = theMut->OccurrenceDefId;
  const BRepGraph_NodeId       aParent  = theMut->ParentId;
  theMut.Internal().OccurrenceDefId     = theOccurrence;
  rebindOccurrenceRef(aStorage, aParent, anOldOcc, theOccurrence);
}
