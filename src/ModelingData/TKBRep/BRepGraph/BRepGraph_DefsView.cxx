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

const BRepGraph_TopoNode::SolidDef& BRepGraph::DefsView::Solid(int theIdx) const
{
  return myGraph->myData->myIncStorage.Solid(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::ShellDef& BRepGraph::DefsView::Shell(int theIdx) const
{
  return myGraph->myData->myIncStorage.Shell(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::FaceDef& BRepGraph::DefsView::Face(int theIdx) const
{
  return myGraph->myData->myIncStorage.Face(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::WireDef& BRepGraph::DefsView::Wire(int theIdx) const
{
  return myGraph->myData->myIncStorage.Wire(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::EdgeDef& BRepGraph::DefsView::Edge(int theIdx) const
{
  return myGraph->myData->myIncStorage.Edge(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::VertexDef& BRepGraph::DefsView::Vertex(int theIdx) const
{
  return myGraph->myData->myIncStorage.Vertex(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::CompoundDef& BRepGraph::DefsView::Compound(int theIdx) const
{
  return myGraph->myData->myIncStorage.Compound(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::CompSolidDef& BRepGraph::DefsView::CompSolid(int theIdx) const
{
  return myGraph->myData->myIncStorage.CompSolid(theIdx);
}

//=================================================================================================

int BRepGraph::DefsView::NbShellFaces(int theShellDefIdx) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (theShellDefIdx < 0 || theShellDefIdx >= aStorage.NbShells())
    return 0;
  return aStorage.Shell(theShellDefIdx).FaceRefs.Length();
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::DefsView::ShellFaceDef(int theShellDefIdx, int theFaceIdx) const
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

const BRepGraph_TopoNode::BaseDef* BRepGraph::DefsView::TopoDef(BRepGraph_NodeId theId) const
{
  return myGraph->TopoDef(theId);
}

//=================================================================================================

size_t BRepGraph::DefsView::NbNodes() const
{
  const BRepGraphInc_Storage& aS = myGraph->myData->myIncStorage;
  return static_cast<size_t>(aS.NbSolids())
         + static_cast<size_t>(aS.NbShells())
         + static_cast<size_t>(aS.NbFaces())
         + static_cast<size_t>(aS.NbWires())
         + static_cast<size_t>(aS.NbEdges())
         + static_cast<size_t>(aS.NbVertices())
         + static_cast<size_t>(aS.NbCompounds())
         + static_cast<size_t>(aS.NbCompSolids());
}

//=================================================================================================

const BRepGraph_TopoNode::EdgeDef::PCurveEntry* BRepGraph::DefsView::FindPCurve(
  BRepGraph_NodeId theEdgeDef,
  BRepGraph_NodeId theFaceDef) const
{
  if (theEdgeDef.NodeKind != BRepGraph_NodeId::Kind::Edge || !theEdgeDef.IsValid())
    return nullptr;

  const BRepGraph_TopoNode::EdgeDef& anEdgeDef =
    myGraph->myData->myIncStorage.Edge(theEdgeDef.Index);
  for (int aPCurveIter = 0; aPCurveIter < anEdgeDef.PCurves.Length(); ++aPCurveIter)
  {
    if (anEdgeDef.PCurves.Value(aPCurveIter).FaceDefId == theFaceDef)
      return &anEdgeDef.PCurves.Value(aPCurveIter);
  }
  return nullptr;
}

//=================================================================================================

const BRepGraph_TopoNode::EdgeDef::PCurveEntry* BRepGraph::DefsView::FindPCurve(
  BRepGraph_NodeId   theEdgeDef,
  BRepGraph_NodeId   theFaceDef,
  TopAbs_Orientation theEdgeOrientation) const
{
  if (theEdgeDef.NodeKind != BRepGraph_NodeId::Kind::Edge || !theEdgeDef.IsValid())
    return nullptr;

  const BRepGraph_TopoNode::EdgeDef& anEdgeDef =
    myGraph->myData->myIncStorage.Edge(theEdgeDef.Index);
  for (int aPCurveIter = 0; aPCurveIter < anEdgeDef.PCurves.Length(); ++aPCurveIter)
  {
    const BRepGraph_TopoNode::EdgeDef::PCurveEntry& aPCEntry = anEdgeDef.PCurves.Value(aPCurveIter);
    if (aPCEntry.FaceDefId == theFaceDef && aPCEntry.EdgeOrientation == theEdgeOrientation)
      return &aPCEntry;
  }
  return nullptr;
}

//=================================================================================================

const BRepGraph_TopoNode::EdgeDef::PCurveEntry* BRepGraph::DefsView::FindPCurve(
  const BRepGraph_PCurveContext& theContext) const
{
  return FindPCurve(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, theContext.EdgeDefIndex),
                    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, theContext.FaceDefIndex),
                    theContext.Orientation);
}

//=================================================================================================

static GeomAdaptor_TransformedCurve buildCurveAdaptorFromDef(
  const BRepGraph_Data&              theData,
  const BRepGraph_TopoNode::EdgeDef& theEdge)
{
  if (!theEdge.Curve3d.IsNull())
  {
    return GeomAdaptor_TransformedCurve(theEdge.Curve3d,
                                        theEdge.ParamFirst,
                                        theEdge.ParamLast,
                                        gp_Trsf());
  }

  if (!theEdge.PCurves.IsEmpty())
  {
    const BRepGraph_TopoNode::EdgeDef::PCurveEntry& aPCE = theEdge.PCurves.First();
    const Handle(Geom2d_Curve)& aPC = aPCE.Curve2d;
    const Handle(Geom_Surface)& aSurf =
      theData.myIncStorage.Face(aPCE.FaceDefId.Index).Surface;

    Handle(Geom2dAdaptor_Curve) aHC2d =
      new Geom2dAdaptor_Curve(aPC, theEdge.ParamFirst, theEdge.ParamLast);
    Handle(GeomAdaptor_Surface)      aHS  = new GeomAdaptor_Surface(aSurf);
    Handle(Adaptor3d_CurveOnSurface) aCOS = new Adaptor3d_CurveOnSurface(aHC2d, aHS);

    GeomAdaptor_TransformedCurve aResult;
    aResult.LoadCurveOnSurface(aCOS);
    return aResult;
  }

  return GeomAdaptor_TransformedCurve();
}

//=================================================================================================

GeomAdaptor_TransformedCurve BRepGraph::DefsView::CurveAdaptor(BRepGraph_NodeId theEdgeDef) const
{
  if (theEdgeDef.NodeKind != BRepGraph_NodeId::Kind::Edge || !theEdgeDef.IsValid())
    return GeomAdaptor_TransformedCurve();

  const BRepGraph_TopoNode::EdgeDef& anEdge = myGraph->myData->myIncStorage.Edge(theEdgeDef.Index);
  return buildCurveAdaptorFromDef(*myGraph->myData, anEdge);
}

//=================================================================================================

GeomAdaptor_TransformedCurve BRepGraph::DefsView::CurveAdaptor(BRepGraph_NodeId  theEdgeDef,
                                                               BRepGraph_NodeId  theEdgeNode) const
{
  (void)theEdgeNode;
  if (theEdgeDef.NodeKind != BRepGraph_NodeId::Kind::Edge || !theEdgeDef.IsValid())
    return GeomAdaptor_TransformedCurve();

  const BRepGraph_TopoNode::EdgeDef& anEdge = myGraph->myData->myIncStorage.Edge(theEdgeDef.Index);
  return buildCurveAdaptorFromDef(*myGraph->myData, anEdge);
}

//=================================================================================================

Handle(Adaptor3d_CurveOnSurface) BRepGraph::DefsView::CurveOnSurfaceAdaptor(
  BRepGraph_NodeId theEdgeDef,
  BRepGraph_NodeId theFaceDef) const
{
  if (theEdgeDef.NodeKind != BRepGraph_NodeId::Kind::Edge || !theEdgeDef.IsValid())
    return Handle(Adaptor3d_CurveOnSurface)();
  if (theFaceDef.NodeKind != BRepGraph_NodeId::Kind::Face || !theFaceDef.IsValid())
    return Handle(Adaptor3d_CurveOnSurface)();

  const BRepGraph_TopoNode::EdgeDef& anEdgeDef =
    myGraph->myData->myIncStorage.Edge(theEdgeDef.Index);
  const BRepGraph_TopoNode::FaceDef& aFaceDef = myGraph->myData->myIncStorage.Face(theFaceDef.Index);

  const BRepGraph_TopoNode::EdgeDef::PCurveEntry* aPCEntry = FindPCurve(theEdgeDef, theFaceDef);
  if (aPCEntry == nullptr || aPCEntry->Curve2d.IsNull())
    return Handle(Adaptor3d_CurveOnSurface)();

  if (aFaceDef.Surface.IsNull())
    return Handle(Adaptor3d_CurveOnSurface)();

  Handle(Geom2dAdaptor_Curve) aHC2d =
    new Geom2dAdaptor_Curve(aPCEntry->Curve2d, anEdgeDef.ParamFirst, anEdgeDef.ParamLast);
  Handle(GeomAdaptor_Surface) aHS = new GeomAdaptor_Surface(aFaceDef.Surface);

  return new Adaptor3d_CurveOnSurface(aHC2d, aHS);
}

//=================================================================================================

Handle(Adaptor3d_CurveOnSurface) BRepGraph::DefsView::CurveOnSurfaceAdaptor(
  BRepGraph_NodeId   theEdgeDef,
  BRepGraph_NodeId   theFaceDef,
  TopAbs_Orientation theEdgeOrientation) const
{
  if (theEdgeDef.NodeKind != BRepGraph_NodeId::Kind::Edge || !theEdgeDef.IsValid())
    return Handle(Adaptor3d_CurveOnSurface)();
  if (theFaceDef.NodeKind != BRepGraph_NodeId::Kind::Face || !theFaceDef.IsValid())
    return Handle(Adaptor3d_CurveOnSurface)();

  const BRepGraph_TopoNode::EdgeDef& anEdgeDef =
    myGraph->myData->myIncStorage.Edge(theEdgeDef.Index);
  const BRepGraph_TopoNode::FaceDef& aFaceDef = myGraph->myData->myIncStorage.Face(theFaceDef.Index);

  const BRepGraph_TopoNode::EdgeDef::PCurveEntry* aPCEntry =
    FindPCurve(theEdgeDef, theFaceDef, theEdgeOrientation);
  if (aPCEntry == nullptr || aPCEntry->Curve2d.IsNull())
    return Handle(Adaptor3d_CurveOnSurface)();

  if (aFaceDef.Surface.IsNull())
    return Handle(Adaptor3d_CurveOnSurface)();

  Handle(Geom2dAdaptor_Curve) aHC2d =
    new Geom2dAdaptor_Curve(aPCEntry->Curve2d, anEdgeDef.ParamFirst, anEdgeDef.ParamLast);
  Handle(GeomAdaptor_Surface) aHS = new GeomAdaptor_Surface(aFaceDef.Surface);

  return new Adaptor3d_CurveOnSurface(aHC2d, aHS);
}

//=================================================================================================

GeomAdaptor_TransformedSurface BRepGraph::DefsView::SurfaceAdaptor(
  BRepGraph_NodeId theFaceDef) const
{
  if (theFaceDef.NodeKind != BRepGraph_NodeId::Kind::Face || !theFaceDef.IsValid())
    return GeomAdaptor_TransformedSurface();

  const BRepGraph_TopoNode::FaceDef& aFaceDef = myGraph->myData->myIncStorage.Face(theFaceDef.Index);
  if (aFaceDef.Surface.IsNull())
    return GeomAdaptor_TransformedSurface();

  return GeomAdaptor_TransformedSurface(aFaceDef.Surface, gp_Trsf());
}

//=================================================================================================

GeomAdaptor_TransformedSurface BRepGraph::DefsView::SurfaceAdaptor(BRepGraph_NodeId theFaceDef,
                                                                   double           theUFirst,
                                                                   double           theULast,
                                                                   double           theVFirst,
                                                                   double           theVLast) const
{
  if (theFaceDef.NodeKind != BRepGraph_NodeId::Kind::Face || !theFaceDef.IsValid())
    return GeomAdaptor_TransformedSurface();

  const BRepGraph_TopoNode::FaceDef& aFaceDef = myGraph->myData->myIncStorage.Face(theFaceDef.Index);
  if (aFaceDef.Surface.IsNull())
    return GeomAdaptor_TransformedSurface();

  return GeomAdaptor_TransformedSurface(aFaceDef.Surface,
                                        theUFirst, theULast,
                                        theVFirst, theVLast,
                                        gp_Trsf());
}
