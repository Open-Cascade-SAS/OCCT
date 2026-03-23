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
#include <BRepGraph_UsagesView.hxx>
#include <BRepGraphInc_Storage.hxx>

#include <Adaptor3d_CurveOnSurface.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>

//=================================================================================================

// Helper: true if incidence storage was successfully populated.
static bool hasIncStorage(const BRepGraph_Data& theData)
{
  return theData.myIncStorage.IsDone;
}

//=================================================================================================

int BRepGraph::DefsView::NbSolids() const
{
  if (hasIncStorage(*myGraph->myData))
    return myGraph->myData->myIncStorage.Solids.Length();
  return myGraph->myData->mySolids.Defs.Length();
}

//=================================================================================================

int BRepGraph::DefsView::NbShells() const
{
  if (hasIncStorage(*myGraph->myData))
    return myGraph->myData->myIncStorage.Shells.Length();
  return myGraph->myData->myShells.Defs.Length();
}

//=================================================================================================

int BRepGraph::DefsView::NbFaces() const
{
  if (hasIncStorage(*myGraph->myData))
    return myGraph->myData->myIncStorage.Faces.Length();
  return myGraph->myData->myFaces.Defs.Length();
}

//=================================================================================================

int BRepGraph::DefsView::NbWires() const
{
  if (hasIncStorage(*myGraph->myData))
    return myGraph->myData->myIncStorage.Wires.Length();
  return myGraph->myData->myWires.Defs.Length();
}

//=================================================================================================

int BRepGraph::DefsView::NbEdges() const
{
  if (hasIncStorage(*myGraph->myData))
    return myGraph->myData->myIncStorage.Edges.Length();
  return myGraph->myData->myEdges.Defs.Length();
}

//=================================================================================================

int BRepGraph::DefsView::NbVertices() const
{
  if (hasIncStorage(*myGraph->myData))
    return myGraph->myData->myIncStorage.Vertices.Length();
  return myGraph->myData->myVertices.Defs.Length();
}

//=================================================================================================

int BRepGraph::DefsView::NbCompounds() const
{
  if (hasIncStorage(*myGraph->myData))
    return myGraph->myData->myIncStorage.Compounds.Length();
  return myGraph->myData->myCompounds.Defs.Length();
}

//=================================================================================================

int BRepGraph::DefsView::NbCompSolids() const
{
  if (hasIncStorage(*myGraph->myData))
    return myGraph->myData->myIncStorage.CompSolids.Length();
  return myGraph->myData->myCompSolids.Defs.Length();
}

//=================================================================================================

const BRepGraph_TopoNode::SolidDef& BRepGraph::DefsView::Solid(int theIdx) const
{
  return myGraph->myData->mySolids.Defs.Value(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::ShellDef& BRepGraph::DefsView::Shell(int theIdx) const
{
  return myGraph->myData->myShells.Defs.Value(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::FaceDef& BRepGraph::DefsView::Face(int theIdx) const
{
  return myGraph->myData->myFaces.Defs.Value(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::WireDef& BRepGraph::DefsView::Wire(int theIdx) const
{
  return myGraph->myData->myWires.Defs.Value(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::EdgeDef& BRepGraph::DefsView::Edge(int theIdx) const
{
  return myGraph->myData->myEdges.Defs.Value(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::VertexDef& BRepGraph::DefsView::Vertex(int theIdx) const
{
  return myGraph->myData->myVertices.Defs.Value(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::CompoundDef& BRepGraph::DefsView::Compound(int theIdx) const
{
  return myGraph->myData->myCompounds.Defs.Value(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::CompSolidDef& BRepGraph::DefsView::CompSolid(int theIdx) const
{
  return myGraph->myData->myCompSolids.Defs.Value(theIdx);
}

//=================================================================================================

int BRepGraph::DefsView::NbShellFaces(int theShellDefIdx) const
{
  if (hasIncStorage(*myGraph->myData))
  {
    const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
    if (theShellDefIdx < 0 || theShellDefIdx >= aStorage.Shells.Length())
      return 0;
    return aStorage.Shells.Value(theShellDefIdx).FaceRefs.Length();
  }
  // Legacy fallback via Usage chain.
  const BRepGraph_TopoNode::ShellDef& aShellDef = myGraph->myData->myShells.Defs.Value(theShellDefIdx);
  if (aShellDef.Usages.IsEmpty())
    return 0;
  const BRepGraph_TopoNode::ShellUsage& aSU =
    myGraph->myData->myShells.Usages.Value(aShellDef.Usages.Value(0).Index);
  return aSU.FaceUsages.Length();
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::DefsView::ShellFaceDef(int theShellDefIdx, int theFaceIdx) const
{
  if (hasIncStorage(*myGraph->myData))
  {
    const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
    if (theShellDefIdx < 0 || theShellDefIdx >= aStorage.Shells.Length())
      return BRepGraph_NodeId();
    const BRepGraphInc::ShellEntity& aShell = aStorage.Shells.Value(theShellDefIdx);
    if (theFaceIdx < 0 || theFaceIdx >= aShell.FaceRefs.Length())
      return BRepGraph_NodeId();
    return BRepGraph_NodeId::Face(aShell.FaceRefs.Value(theFaceIdx).FaceIdx);
  }
  // Legacy fallback via Usage chain.
  const BRepGraph_TopoNode::ShellDef& aShellDef = myGraph->myData->myShells.Defs.Value(theShellDefIdx);
  if (aShellDef.Usages.IsEmpty())
    return BRepGraph_NodeId();
  const BRepGraph_TopoNode::ShellUsage& aSU =
    myGraph->myData->myShells.Usages.Value(aShellDef.Usages.Value(0).Index);
  if (theFaceIdx < 0 || theFaceIdx >= aSU.FaceUsages.Length())
    return BRepGraph_NodeId();
  return myGraph->DefOf(aSU.FaceUsages.Value(theFaceIdx));
}

//=================================================================================================

const BRepGraph_TopoNode::BaseDef* BRepGraph::DefsView::TopoDef(BRepGraph_NodeId theId) const
{
  return myGraph->TopoDef(theId);
}

//=================================================================================================

size_t BRepGraph::DefsView::NbNodes() const
{
  if (hasIncStorage(*myGraph->myData))
  {
    const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
    return static_cast<size_t>(aStorage.Solids.Length())
           + static_cast<size_t>(aStorage.Shells.Length())
           + static_cast<size_t>(aStorage.Faces.Length())
           + static_cast<size_t>(aStorage.Wires.Length())
           + static_cast<size_t>(aStorage.Edges.Length())
           + static_cast<size_t>(aStorage.Vertices.Length())
           + static_cast<size_t>(aStorage.Compounds.Length())
           + static_cast<size_t>(aStorage.CompSolids.Length());
  }
  return static_cast<size_t>(myGraph->myData->mySolids.Defs.Length())
         + static_cast<size_t>(myGraph->myData->myShells.Defs.Length())
         + static_cast<size_t>(myGraph->myData->myFaces.Defs.Length())
         + static_cast<size_t>(myGraph->myData->myWires.Defs.Length())
         + static_cast<size_t>(myGraph->myData->myEdges.Defs.Length())
         + static_cast<size_t>(myGraph->myData->myVertices.Defs.Length())
         + static_cast<size_t>(myGraph->myData->myCompounds.Defs.Length())
         + static_cast<size_t>(myGraph->myData->myCompSolids.Defs.Length());
}

//=================================================================================================

const BRepGraph_TopoNode::EdgeDef::PCurveEntry* BRepGraph::DefsView::FindPCurve(
  BRepGraph_NodeId theEdgeDef,
  BRepGraph_NodeId theFaceDef) const
{
  if (theEdgeDef.NodeKind != BRepGraph_NodeId::Kind::Edge || !theEdgeDef.IsValid())
    return nullptr;

  // Read from legacy EdgeDef.PCurves (still authoritative until Phase 5).
  // The incidence EdgeFaceGeoms table has the same data but returns a different type.
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

const BRepGraph_TopoNode::EdgeDef::PCurveEntry* BRepGraph::DefsView::FindPCurve(
  BRepGraph_NodeId   theEdgeDef,
  BRepGraph_NodeId   theFaceDef,
  TopAbs_Orientation theEdgeOrientation) const
{
  if (theEdgeDef.NodeKind != BRepGraph_NodeId::Kind::Edge || !theEdgeDef.IsValid())
    return nullptr;

  // Read from legacy EdgeDef.PCurves (still authoritative until Phase 5).
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

  // Pcurve-only fallback: compose pcurve + surface.
  if (!theEdge.PCurves.IsEmpty())
  {
    const BRepGraph_TopoNode::EdgeDef::PCurveEntry& aPCE = theEdge.PCurves.First();
    const Handle(Geom2d_Curve)& aPC = aPCE.Curve2d;
    const Handle(Geom_Surface)& aSurf =
      theData.myFaces.Defs.Value(aPCE.FaceDefId.Index).Surface;

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

  const BRepGraph_TopoNode::EdgeDef& anEdge = myGraph->myData->myEdges.Defs.Value(theEdgeDef.Index);
  return buildCurveAdaptorFromDef(*myGraph->myData, anEdge);
}

//=================================================================================================

GeomAdaptor_TransformedCurve BRepGraph::DefsView::CurveAdaptor(BRepGraph_NodeId  theEdgeDef,
                                                               BRepGraph_UsageId theEdgeUsage) const
{
  (void)theEdgeUsage;
  if (theEdgeDef.NodeKind != BRepGraph_NodeId::Kind::Edge || !theEdgeDef.IsValid())
    return GeomAdaptor_TransformedCurve();

  const BRepGraph_TopoNode::EdgeDef& anEdge = myGraph->myData->myEdges.Defs.Value(theEdgeDef.Index);
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
    myGraph->myData->myEdges.Defs.Value(theEdgeDef.Index);
  const BRepGraph_TopoNode::FaceDef& aFaceDef = myGraph->myData->myFaces.Defs.Value(theFaceDef.Index);

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
    myGraph->myData->myEdges.Defs.Value(theEdgeDef.Index);
  const BRepGraph_TopoNode::FaceDef& aFaceDef = myGraph->myData->myFaces.Defs.Value(theFaceDef.Index);

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

  const BRepGraph_TopoNode::FaceDef& aFaceDef = myGraph->myData->myFaces.Defs.Value(theFaceDef.Index);
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

  const BRepGraph_TopoNode::FaceDef& aFaceDef = myGraph->myData->myFaces.Defs.Value(theFaceDef.Index);
  if (aFaceDef.Surface.IsNull())
    return GeomAdaptor_TransformedSurface();

  return GeomAdaptor_TransformedSurface(aFaceDef.Surface,
                                        theUFirst, theULast,
                                        theVFirst, theVLast,
                                        gp_Trsf());
}
