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

int BRepGraph::GeomView::NbSurfaces() const
{
  return myGraph->myData->mySurfaces.Nodes.Length();
}

//=================================================================================================

int BRepGraph::GeomView::NbCurves() const
{
  return myGraph->myData->myCurves.Nodes.Length();
}

//=================================================================================================

const BRepGraph_GeomNode::Surf& BRepGraph::GeomView::Surface(int theIdx) const
{
  return myGraph->myData->mySurfaces.Nodes.Value(theIdx);
}

//=================================================================================================

const BRepGraph_GeomNode::Curve& BRepGraph::GeomView::Curve(int theIdx) const
{
  return myGraph->myData->myCurves.Nodes.Value(theIdx);
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::GeomView::SurfaceOf(BRepGraph_NodeId theFaceDef) const
{
  if (theFaceDef.NodeKind != BRepGraph_NodeId::Kind::Face || !theFaceDef.IsValid())
    return BRepGraph_NodeId();
  return myGraph->myData->myFaces.Defs.Value(theFaceDef.Index).SurfNodeId;
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::GeomView::FacesOnSurface(
  BRepGraph_NodeId theSurf) const
{
  NCollection_Vector<BRepGraph_NodeId> aResult;
  if (theSurf.NodeKind != BRepGraph_NodeId::Kind::Surface || !theSurf.IsValid()
      || theSurf.Index >= myGraph->myData->mySurfaces.Nodes.Length())
    return aResult;

  const NCollection_Vector<BRepGraph_TopoNode::FaceDef>& aFaceDefs =
    myGraph->myData->myFaces.Defs;
  for (int aFaceIdx = 0; aFaceIdx < aFaceDefs.Length(); ++aFaceIdx)
  {
    if (aFaceDefs.Value(aFaceIdx).SurfNodeId == theSurf)
      aResult.Append(aFaceDefs.Value(aFaceIdx).Id);
  }
  return aResult;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::GeomView::CurveOf(BRepGraph_NodeId theEdgeDef) const
{
  if (theEdgeDef.NodeKind != BRepGraph_NodeId::Kind::Edge || !theEdgeDef.IsValid())
    return BRepGraph_NodeId();
  return myGraph->myData->myEdges.Defs.Value(theEdgeDef.Index).CurveNodeId;
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::GeomView::EdgesOnCurve(
  BRepGraph_NodeId theCurve) const
{
  NCollection_Vector<BRepGraph_NodeId> aResult;
  if (theCurve.NodeKind != BRepGraph_NodeId::Kind::Curve || !theCurve.IsValid()
      || theCurve.Index >= myGraph->myData->myCurves.Nodes.Length())
    return aResult;

  const NCollection_Vector<BRepGraph_TopoNode::EdgeDef>& anEdgeDefs =
    myGraph->myData->myEdges.Defs;
  for (int anEdgeIdx = 0; anEdgeIdx < anEdgeDefs.Length(); ++anEdgeIdx)
  {
    if (anEdgeDefs.Value(anEdgeIdx).CurveNodeId == theCurve)
      aResult.Append(anEdgeDefs.Value(anEdgeIdx).Id);
  }
  return aResult;
}

//=================================================================================================

const BRepGraph_TopoNode::EdgeDef::PCurveEntry* BRepGraph::GeomView::FindPCurve(
  BRepGraph_NodeId theEdgeDef,
  BRepGraph_NodeId theFaceDef) const
{
  if (theEdgeDef.NodeKind != BRepGraph_NodeId::Kind::Edge || !theEdgeDef.IsValid())
    return nullptr;

  const BRepGraph_TopoNode::EdgeDef& anEdgeDef =
    myGraph->myData->myEdges.Defs.Value(theEdgeDef.Index);
  for (int aPCurveIter = 0; aPCurveIter < anEdgeDef.PCurves.Length(); ++aPCurveIter)
  {
    if (anEdgeDef.PCurves.Value(aPCurveIter).FaceDefId == theFaceDef)
      return &anEdgeDef.PCurves.Value(aPCurveIter);
  }
  return nullptr;
}

//=================================================================================================

const BRepGraph_TopoNode::EdgeDef::PCurveEntry* BRepGraph::GeomView::FindPCurve(
  BRepGraph_NodeId   theEdgeDef,
  BRepGraph_NodeId   theFaceDef,
  TopAbs_Orientation theEdgeOrientation) const
{
  if (theEdgeDef.NodeKind != BRepGraph_NodeId::Kind::Edge || !theEdgeDef.IsValid())
    return nullptr;

  const BRepGraph_TopoNode::EdgeDef& anEdgeDef =
    myGraph->myData->myEdges.Defs.Value(theEdgeDef.Index);
  for (int aPCurveIter = 0; aPCurveIter < anEdgeDef.PCurves.Length(); ++aPCurveIter)
  {
    const BRepGraph_TopoNode::EdgeDef::PCurveEntry& aPCEntry = anEdgeDef.PCurves.Value(aPCurveIter);
    if (aPCEntry.FaceDefId == theFaceDef && aPCEntry.EdgeOrientation == theEdgeOrientation)
      return &aPCEntry;
  }
  return nullptr;
}

//=================================================================================================

const BRepGraph_TopoNode::EdgeDef::PCurveEntry* BRepGraph::GeomView::FindPCurve(
  const BRepGraph_PCurveContext& theContext) const
{
  return FindPCurve(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, theContext.EdgeDefIndex),
                    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, theContext.FaceDefIndex),
                    theContext.Orientation);
}

//=================================================================================================

const BRepGraph_GeomNode::Surf* BRepGraph::GeomView::FaceSurface(int theFaceDefIdx) const
{
  if (theFaceDefIdx < 0 || theFaceDefIdx >= myGraph->myData->myFaces.Defs.Length())
    return nullptr;
  const BRepGraph_TopoNode::FaceDef& aFaceDef = myGraph->myData->myFaces.Defs.Value(theFaceDefIdx);
  if (!aFaceDef.SurfNodeId.IsValid())
    return nullptr;
  const BRepGraph_GeomNode::Surf& aSurf =
    myGraph->myData->mySurfaces.Nodes.Value(aFaceDef.SurfNodeId.Index);
  if (aSurf.Surface.IsNull())
    return nullptr;
  return &aSurf;
}

//=================================================================================================

static GeomAdaptor_TransformedCurve buildCurveAdaptor(const BRepGraph_Data&              theData,
                                                      const BRepGraph_TopoNode::EdgeDef& theEdge)
{
  // The graph stores raw geometry with identity location (offsets absorbed into usages).

  // 3D curve available.
  if (theEdge.CurveNodeId.IsValid())
  {
    const BRepGraph_GeomNode::Curve& aCurveNode =
      theData.myCurves.Nodes.Value(theEdge.CurveNodeId.Index);
    return GeomAdaptor_TransformedCurve(aCurveNode.CurveGeom,
                                        theEdge.ParamFirst,
                                        theEdge.ParamLast,
                                        gp_Trsf());
  }

  // Pcurve-only fallback: compose pcurve + surface.
  if (!theEdge.PCurves.IsEmpty())
  {
    const BRepGraph_TopoNode::EdgeDef::PCurveEntry& aPCE = theEdge.PCurves.First();
    const Handle(Geom2d_Curve)& aPC = aPCE.Curve2d;
    const BRepGraph_NodeId aSurfNodeId = theData.myFaces.Defs.Value(aPCE.FaceDefId.Index).SurfNodeId;
    const Handle(Geom_Surface)& aSurf = theData.mySurfaces.Nodes.Value(aSurfNodeId.Index).Surface;

    Handle(Geom2dAdaptor_Curve) aHC2d =
      new Geom2dAdaptor_Curve(aPC, theEdge.ParamFirst, theEdge.ParamLast);
    Handle(GeomAdaptor_Surface)      aHS  = new GeomAdaptor_Surface(aSurf);
    Handle(Adaptor3d_CurveOnSurface) aCOS = new Adaptor3d_CurveOnSurface(aHC2d, aHS);

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

  const BRepGraph_TopoNode::EdgeDef& anEdge = myGraph->myData->myEdges.Defs.Value(theEdgeDef.Index);
  return buildCurveAdaptor(*myGraph->myData, anEdge);
}

//=================================================================================================

GeomAdaptor_TransformedCurve BRepGraph::GeomView::CurveAdaptor(BRepGraph_NodeId  theEdgeDef,
                                                               BRepGraph_UsageId theEdgeUsage) const
{
  (void)theEdgeUsage; // reserved for future use with per-usage location override
  if (theEdgeDef.NodeKind != BRepGraph_NodeId::Kind::Edge || !theEdgeDef.IsValid())
    return GeomAdaptor_TransformedCurve();

  const BRepGraph_TopoNode::EdgeDef& anEdge = myGraph->myData->myEdges.Defs.Value(theEdgeDef.Index);
  return buildCurveAdaptor(*myGraph->myData, anEdge);
}

//=================================================================================================

Handle(Adaptor3d_CurveOnSurface) BRepGraph::GeomView::CurveOnSurfaceAdaptor(
  BRepGraph_NodeId theEdgeDef,
  BRepGraph_NodeId theFaceDef) const
{
  if (theEdgeDef.NodeKind != BRepGraph_NodeId::Kind::Edge || !theEdgeDef.IsValid())
    return Handle(Adaptor3d_CurveOnSurface)();
  if (theFaceDef.NodeKind != BRepGraph_NodeId::Kind::Face || !theFaceDef.IsValid())
    return Handle(Adaptor3d_CurveOnSurface)();

  const BRepGraph_TopoNode::EdgeDef& anEdgeDef =
    myGraph->myData->myEdges.Defs.Value(theEdgeDef.Index);
  const BRepGraph_TopoNode::FaceDef& aFaceDef = myGraph->myData->myFaces.Defs.Value(theFaceDef.Index);

  // Look up inline PCurveEntry for this edge on this face.
  const BRepGraph_TopoNode::EdgeDef::PCurveEntry* aPCEntry = FindPCurve(theEdgeDef, theFaceDef);
  if (aPCEntry == nullptr || aPCEntry->Curve2d.IsNull())
    return Handle(Adaptor3d_CurveOnSurface)();

  if (!aFaceDef.SurfNodeId.IsValid())
    return Handle(Adaptor3d_CurveOnSurface)();

  const BRepGraph_GeomNode::Surf& aSurfNode =
    myGraph->myData->mySurfaces.Nodes.Value(aFaceDef.SurfNodeId.Index);
  if (aSurfNode.Surface.IsNull())
    return Handle(Adaptor3d_CurveOnSurface)();

  Handle(Geom2dAdaptor_Curve) aHC2d =
    new Geom2dAdaptor_Curve(aPCEntry->Curve2d, anEdgeDef.ParamFirst, anEdgeDef.ParamLast);
  Handle(GeomAdaptor_Surface) aHS = new GeomAdaptor_Surface(aSurfNode.Surface);

  return new Adaptor3d_CurveOnSurface(aHC2d, aHS);
}

//=================================================================================================

Handle(Adaptor3d_CurveOnSurface) BRepGraph::GeomView::CurveOnSurfaceAdaptor(
  BRepGraph_NodeId   theEdgeDef,
  BRepGraph_NodeId   theFaceDef,
  TopAbs_Orientation theEdgeOrientation) const
{
  if (theEdgeDef.NodeKind != BRepGraph_NodeId::Kind::Edge || !theEdgeDef.IsValid())
    return Handle(Adaptor3d_CurveOnSurface)();
  if (theFaceDef.NodeKind != BRepGraph_NodeId::Kind::Face || !theFaceDef.IsValid())
    return Handle(Adaptor3d_CurveOnSurface)();

  const BRepGraph_TopoNode::EdgeDef& anEdgeDef =
    myGraph->myData->myEdges.Defs.Value(theEdgeDef.Index);
  const BRepGraph_TopoNode::FaceDef& aFaceDef = myGraph->myData->myFaces.Defs.Value(theFaceDef.Index);

  // Look up inline PCurveEntry for this edge/face/orientation triple (seam edge support).
  const BRepGraph_TopoNode::EdgeDef::PCurveEntry* aPCEntry =
    FindPCurve(theEdgeDef, theFaceDef, theEdgeOrientation);
  if (aPCEntry == nullptr || aPCEntry->Curve2d.IsNull())
    return Handle(Adaptor3d_CurveOnSurface)();

  if (!aFaceDef.SurfNodeId.IsValid())
    return Handle(Adaptor3d_CurveOnSurface)();

  const BRepGraph_GeomNode::Surf& aSurfNode =
    myGraph->myData->mySurfaces.Nodes.Value(aFaceDef.SurfNodeId.Index);
  if (aSurfNode.Surface.IsNull())
    return Handle(Adaptor3d_CurveOnSurface)();

  Handle(Geom2dAdaptor_Curve) aHC2d =
    new Geom2dAdaptor_Curve(aPCEntry->Curve2d, anEdgeDef.ParamFirst, anEdgeDef.ParamLast);
  Handle(GeomAdaptor_Surface) aHS = new GeomAdaptor_Surface(aSurfNode.Surface);

  return new Adaptor3d_CurveOnSurface(aHC2d, aHS);
}

//=================================================================================================

GeomAdaptor_TransformedSurface BRepGraph::GeomView::SurfaceAdaptor(
  BRepGraph_NodeId theFaceDef) const
{
  if (theFaceDef.NodeKind != BRepGraph_NodeId::Kind::Face || !theFaceDef.IsValid())
    return GeomAdaptor_TransformedSurface();

  const BRepGraph_TopoNode::FaceDef& aFaceDef = myGraph->myData->myFaces.Defs.Value(theFaceDef.Index);
  if (!aFaceDef.SurfNodeId.IsValid())
    return GeomAdaptor_TransformedSurface();

  const BRepGraph_GeomNode::Surf& aSurfNode =
    myGraph->myData->mySurfaces.Nodes.Value(aFaceDef.SurfNodeId.Index);
  if (aSurfNode.Surface.IsNull())
    return GeomAdaptor_TransformedSurface();

  return GeomAdaptor_TransformedSurface(aSurfNode.Surface, gp_Trsf());
}

//=================================================================================================

GeomAdaptor_TransformedSurface BRepGraph::GeomView::SurfaceAdaptor(BRepGraph_NodeId theFaceDef,
                                                                   double           theUFirst,
                                                                   double           theULast,
                                                                   double           theVFirst,
                                                                   double           theVLast) const
{
  if (theFaceDef.NodeKind != BRepGraph_NodeId::Kind::Face || !theFaceDef.IsValid())
    return GeomAdaptor_TransformedSurface();

  const BRepGraph_TopoNode::FaceDef& aFaceDef = myGraph->myData->myFaces.Defs.Value(theFaceDef.Index);
  if (!aFaceDef.SurfNodeId.IsValid())
    return GeomAdaptor_TransformedSurface();

  const BRepGraph_GeomNode::Surf& aSurfNode =
    myGraph->myData->mySurfaces.Nodes.Value(aFaceDef.SurfNodeId.Index);
  if (aSurfNode.Surface.IsNull())
    return GeomAdaptor_TransformedSurface();

  return GeomAdaptor_TransformedSurface(aSurfNode.Surface,
                                        theUFirst,
                                        theULast,
                                        theVFirst,
                                        theVLast,
                                        gp_Trsf());
}
