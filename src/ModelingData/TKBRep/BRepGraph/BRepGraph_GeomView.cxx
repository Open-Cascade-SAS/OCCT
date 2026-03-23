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
  if (theFaceDef.Kind != BRepGraph_NodeKind::Face || !theFaceDef.IsValid())
    return BRepGraph_NodeId();
  return myGraph->myData->myFaceDefs.Value(theFaceDef.Index).SurfNodeId;
}

//=================================================================================================

const NCollection_Vector<BRepGraph_NodeId>& BRepGraph::GeomView::FacesOnSurface(
  BRepGraph_NodeId theSurf) const
{
  static const NCollection_Vector<BRepGraph_NodeId> THE_EMPTY_VEC;
  if (theSurf.Kind != BRepGraph_NodeKind::Surface || !theSurf.IsValid()
      || theSurf.Index >= myGraph->myData->mySurfaces.Length())
    return THE_EMPTY_VEC;
  return myGraph->myData->mySurfaces.Value(theSurf.Index).FaceDefUsers;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::GeomView::CurveOf(BRepGraph_NodeId theEdgeDef) const
{
  if (theEdgeDef.Kind != BRepGraph_NodeKind::Edge || !theEdgeDef.IsValid())
    return BRepGraph_NodeId();
  return myGraph->myData->myEdgeDefs.Value(theEdgeDef.Index).CurveNodeId;
}

//=================================================================================================

const NCollection_Vector<BRepGraph_NodeId>& BRepGraph::GeomView::EdgesOnCurve(
  BRepGraph_NodeId theCurve) const
{
  static const NCollection_Vector<BRepGraph_NodeId> THE_EMPTY_VEC;
  if (theCurve.Kind != BRepGraph_NodeKind::Curve || !theCurve.IsValid()
      || theCurve.Index >= myGraph->myData->myCurves.Length())
    return THE_EMPTY_VEC;
  return myGraph->myData->myCurves.Value(theCurve.Index).EdgeDefUsers;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::GeomView::PCurveOf(BRepGraph_NodeId theEdgeDef,
                                               BRepGraph_NodeId theFaceDef) const
{
  if (theEdgeDef.Kind != BRepGraph_NodeKind::Edge || !theEdgeDef.IsValid())
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
  if (theEdgeDef.Kind != BRepGraph_NodeKind::Edge || !theEdgeDef.IsValid())
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
