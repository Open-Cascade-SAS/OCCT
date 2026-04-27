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
#include <BRepGraphInc_Storage.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Curve.hxx>
#include <Geom2d_Curve.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_Polygon2D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <gp_Pnt.hxx>

//=================================================================================================

void BRepGraph::EditorView::VertexOps::SetPoint(const BRepGraph_VertexId theVertex,
                                                const gp_Pnt&            thePoint)
{
  myGraph->myData->myIncStorage.ChangeVertex(theVertex).Point = thePoint;
  myGraph->markModified(theVertex);
}

//=================================================================================================

void BRepGraph::EditorView::VertexOps::SetPoint(
  BRepGraph_MutGuard<BRepGraphInc::VertexDef>& theMut,
  const gp_Pnt&                                thePoint)
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

void BRepGraph::EditorView::EdgeOps::SetTolerance(
  BRepGraph_MutGuard<BRepGraphInc::EdgeDef>& theMut,
  double                                     theTolerance)
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
  double                                        theFirst,
  double                                        theLast)
{
  theMut.Internal().ParamFirst = theFirst;
  theMut.Internal().ParamLast  = theLast;
}

//=================================================================================================

void BRepGraph::EditorView::WireOps::SetIsClosed(const BRepGraph_WireId theWire,
                                                 bool                   theIsClosed)
{
  myGraph->myData->myIncStorage.ChangeWire(theWire).IsClosed = theIsClosed;
  myGraph->markModified(theWire);
}

//=================================================================================================

void BRepGraph::EditorView::WireOps::SetIsClosed(
  BRepGraph_MutGuard<BRepGraphInc::WireDef>& theMut,
  bool                                        theIsClosed)
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

void BRepGraph::EditorView::FaceOps::SetTolerance(
  BRepGraph_MutGuard<BRepGraphInc::FaceDef>& theMut,
  double                                     theTolerance)
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
  bool                                        theNaturalRestriction)
{
  theMut.Internal().NaturalRestriction = theNaturalRestriction;
}

//=================================================================================================

void BRepGraph::EditorView::FaceOps::SetTriangulationRep(
  const BRepGraph_FaceId             theFace,
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

void BRepGraph::EditorView::VertexOps::SetRefLocalLocation(
  const BRepGraph_VertexRefId theVertexRef,
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

void BRepGraph::EditorView::CoEdgeOps::SetRefLocalLocation(
  const BRepGraph_CoEdgeRefId theCoEdgeRef,
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

void BRepGraph::EditorView::GenOps::SetChildRefLocalLocation(
  const BRepGraph_ChildRefId theChildRef,
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

void BRepGraph::EditorView::EdgeOps::SetSameRange(
  BRepGraph_MutGuard<BRepGraphInc::EdgeDef>& theMut,
  const bool                                 theSameRange)
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

void BRepGraph::EditorView::CoEdgeOps::SetUVBox(
  BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef>& theMut,
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
                                                         const TopAbs_Orientation theOrientation)
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

void BRepGraph::EditorView::EdgeOps::SetIsClosed(
  BRepGraph_MutGuard<BRepGraphInc::EdgeDef>& theMut,
  const bool                                 theIsClosed)
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
                                                         const BRepGraph_NodeId theChildDefId)
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
                                                           const TopAbs_Orientation   theOrientation)
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

void BRepGraph::EditorView::RepOps::SetSurface(
  BRepGraph_MutGuard<BRepGraphInc::SurfaceRep>& theMut,
  const occ::handle<Geom_Surface>&              theSurface)
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

void BRepGraph::EditorView::RepOps::SetCurve3D(
  BRepGraph_MutGuard<BRepGraphInc::Curve3DRep>& theMut,
  const occ::handle<Geom_Curve>&                theCurve)
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

void BRepGraph::EditorView::RepOps::SetCurve2D(
  BRepGraph_MutGuard<BRepGraphInc::Curve2DRep>& theMut,
  const occ::handle<Geom2d_Curve>&              theCurve)
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
