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

#include <BRepGraph_GeomView.hxx>
#include <BRepGraph_Data.hxx>

#include <Adaptor3d_CurveOnSurface.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>

//=================================================================================================

int BRepGraph::GeomView::NbSurfaces() const { return myGraph->myData->mySurfaces.Length(); }

//=================================================================================================

int BRepGraph::GeomView::NbCurves() const { return myGraph->myData->myCurves.Length(); }

//=================================================================================================

int BRepGraph::GeomView::NbPCurves() const { return myGraph->myData->myPCurves.Length(); }

//=================================================================================================

const BRepGraph_GeomNode::Surf& BRepGraph::GeomView::Surface(int theIdx) const
{
  return myGraph->myData->mySurfaces.Value(theIdx);
}

//=================================================================================================

const BRepGraph_GeomNode::Curve& BRepGraph::GeomView::Curve(int theIdx) const
{
  return myGraph->myData->myCurves.Value(theIdx);
}

//=================================================================================================

const BRepGraph_GeomNode::PCurve& BRepGraph::GeomView::PCurve(int theIdx) const
{
  return myGraph->myData->myPCurves.Value(theIdx);
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::GeomView::SurfaceOf(BRepGraph_NodeId theFaceDef) const
{
  if (theFaceDef.NodeKind != BRepGraph_NodeId::Kind::Face || !theFaceDef.IsValid())
    return BRepGraph_NodeId();
  return myGraph->myData->myFaceDefs.Value(theFaceDef.Index).SurfNodeId;
}

//=================================================================================================

const NCollection_Vector<BRepGraph_NodeId>& BRepGraph::GeomView::FacesOnSurface(
  BRepGraph_NodeId theSurf) const
{
  static const NCollection_Vector<BRepGraph_NodeId> THE_EMPTY_VEC;
  if (theSurf.NodeKind != BRepGraph_NodeId::Kind::Surface || !theSurf.IsValid()
      || theSurf.Index >= myGraph->myData->mySurfaces.Length())
    return THE_EMPTY_VEC;
  return myGraph->myData->mySurfaces.Value(theSurf.Index).FaceDefUsers;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::GeomView::CurveOf(BRepGraph_NodeId theEdgeDef) const
{
  if (theEdgeDef.NodeKind != BRepGraph_NodeId::Kind::Edge || !theEdgeDef.IsValid())
    return BRepGraph_NodeId();
  return myGraph->myData->myEdgeDefs.Value(theEdgeDef.Index).CurveNodeId;
}

//=================================================================================================

const NCollection_Vector<BRepGraph_NodeId>& BRepGraph::GeomView::EdgesOnCurve(
  BRepGraph_NodeId theCurve) const
{
  static const NCollection_Vector<BRepGraph_NodeId> THE_EMPTY_VEC;
  if (theCurve.NodeKind != BRepGraph_NodeId::Kind::Curve || !theCurve.IsValid()
      || theCurve.Index >= myGraph->myData->myCurves.Length())
    return THE_EMPTY_VEC;
  return myGraph->myData->myCurves.Value(theCurve.Index).EdgeDefUsers;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::GeomView::PCurveOf(BRepGraph_NodeId theEdgeDef,
                                               BRepGraph_NodeId theFaceDef) const
{
  if (theEdgeDef.NodeKind != BRepGraph_NodeId::Kind::Edge || !theEdgeDef.IsValid())
    return BRepGraph_NodeId();

  const BRepGraph_TopoNode::EdgeDef& anEdgeDef =
    myGraph->myData->myEdgeDefs.Value(theEdgeDef.Index);
  for (int aPCurveIter = 0; aPCurveIter < anEdgeDef.PCurves.Length(); ++aPCurveIter)
  {
    if (anEdgeDef.PCurves.Value(aPCurveIter).FaceDefId == theFaceDef)
      return anEdgeDef.PCurves.Value(aPCurveIter).PCurveNodeId;
  }
  return BRepGraph_NodeId();
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::GeomView::PCurveOf(BRepGraph_NodeId   theEdgeDef,
                                               BRepGraph_NodeId   theFaceDef,
                                               TopAbs_Orientation theEdgeOrientation) const
{
  if (theEdgeDef.NodeKind != BRepGraph_NodeId::Kind::Edge || !theEdgeDef.IsValid())
    return BRepGraph_NodeId();

  const BRepGraph_TopoNode::EdgeDef& anEdgeDef =
    myGraph->myData->myEdgeDefs.Value(theEdgeDef.Index);
  for (int aPCurveIter = 0; aPCurveIter < anEdgeDef.PCurves.Length(); ++aPCurveIter)
  {
    const BRepGraph_TopoNode::EdgeDef::PCurveEntry& aPCEntry =
      anEdgeDef.PCurves.Value(aPCurveIter);
    if (aPCEntry.FaceDefId == theFaceDef && aPCEntry.EdgeOrientation == theEdgeOrientation)
      return aPCEntry.PCurveNodeId;
  }
  return BRepGraph_NodeId();
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::GeomView::PCurveOf(const BRepGraph_PCurveContext& theContext) const
{
  return PCurveOf(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, theContext.EdgeDefIndex),
                  BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, theContext.FaceDefIndex),
                  theContext.Orientation);
}

//=================================================================================================

static GeomAdaptor_TransformedCurve buildCurveAdaptor(const BRepGraph_Data&              theData,
                                                      const BRepGraph_TopoNode::EdgeDef& theEdge)
{
  // The graph stores curves from BRep_Tool::Curve(E, first, last) (3-arg),
  // which returns the curve already transformed to global coordinates.
  // No additional transform is needed.

  // 3D curve available.
  if (theEdge.CurveNodeId.IsValid())
  {
    const Handle(Geom_Curve)& aCurve =
      theData.myCurves.Value(theEdge.CurveNodeId.Index).CurveGeom;
    return GeomAdaptor_TransformedCurve(aCurve, theEdge.ParamFirst, theEdge.ParamLast, gp_Trsf());
  }

  // Pcurve-only fallback: compose pcurve + surface.
  // The graph stores pcurves and surfaces from BRep_Tool convenience methods
  // which also include location transforms, so identity Trsf is correct.
  if (!theEdge.PCurves.IsEmpty())
  {
    const BRepGraph_TopoNode::EdgeDef::PCurveEntry& aPCE = theEdge.PCurves.First();
    const Handle(Geom2d_Curve)& aPC =
      theData.myPCurves.Value(aPCE.PCurveNodeId.Index).Curve2d;
    const BRepGraph_NodeId aSurfNodeId =
      theData.myFaceDefs.Value(aPCE.FaceDefId.Index).SurfNodeId;
    const Handle(Geom_Surface)& aSurf =
      theData.mySurfaces.Value(aSurfNodeId.Index).Surface;

    Handle(Geom2dAdaptor_Curve)       aHC2d = new Geom2dAdaptor_Curve(aPC, theEdge.ParamFirst, theEdge.ParamLast);
    Handle(GeomAdaptor_Surface)       aHS   = new GeomAdaptor_Surface(aSurf);
    Handle(Adaptor3d_CurveOnSurface)  aCOS  = new Adaptor3d_CurveOnSurface(aHC2d, aHS);

    GeomAdaptor_TransformedCurve aResult;
    aResult.LoadCurveOnSurface(aCOS);
    return aResult;
  }

  return GeomAdaptor_TransformedCurve(); // empty, edge has no geometry
}

//=================================================================================================

GeomAdaptor_TransformedCurve BRepGraph::GeomView::CurveAdaptor(BRepGraph_NodeId theEdgeDef) const
{
  if (theEdgeDef.NodeKind != BRepGraph_NodeId::Kind::Edge || !theEdgeDef.IsValid())
    return GeomAdaptor_TransformedCurve();

  const BRepGraph_TopoNode::EdgeDef& anEdge = myGraph->myData->myEdgeDefs.Value(theEdgeDef.Index);
  return buildCurveAdaptor(*myGraph->myData, anEdge);
}

//=================================================================================================

GeomAdaptor_TransformedCurve BRepGraph::GeomView::CurveAdaptor(BRepGraph_NodeId  theEdgeDef,
                                                                BRepGraph_UsageId theEdgeUsage) const
{
  (void)theEdgeUsage; // reserved for future use with per-usage location override
  if (theEdgeDef.NodeKind != BRepGraph_NodeId::Kind::Edge || !theEdgeDef.IsValid())
    return GeomAdaptor_TransformedCurve();

  const BRepGraph_TopoNode::EdgeDef& anEdge = myGraph->myData->myEdgeDefs.Value(theEdgeDef.Index);
  return buildCurveAdaptor(*myGraph->myData, anEdge);
}
