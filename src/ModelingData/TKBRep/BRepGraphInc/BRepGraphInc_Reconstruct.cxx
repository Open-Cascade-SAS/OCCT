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

#include <BRepGraphInc_Reconstruct.hxx>
#include <BRepGraphInc_Storage.hxx>

#include <BRep_Builder.hxx>
#include <BRep_CurveOnClosedSurface.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_TFace.hxx>
#include <NCollection_List.hxx>
#include <TopoDS.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>

//=================================================================================================

TopoDS_Shape BRepGraphInc_Reconstruct::Node(const BRepGraphInc_Storage& theStorage,
                                            const BRepGraph_NodeId      theNode)
{
  Cache aCache;
  return Node(theStorage, theNode, aCache);
}

//=================================================================================================

TopoDS_Shape BRepGraphInc_Reconstruct::Node(const BRepGraphInc_Storage& theStorage,
                                            const BRepGraph_NodeId      theNode,
                                            Cache&                      theCache)
{
  if (!theNode.IsValid())
    return TopoDS_Shape();

  // Check cache first.
  const TopoDS_Shape* aCached = theCache.Seek(theNode);
  if (aCached != nullptr)
    return *aCached;

  BRep_Builder aBB;
  TopoDS_Shape aResult;

  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Vertex: {
      const BRepGraphInc::VertexEntity& aVtx = theStorage.Vertex(BRepGraph_VertexId(theNode.Index));
      TopoDS_Vertex                     aNewVtx;
      aBB.MakeVertex(aNewVtx, aVtx.Point, aVtx.Tolerance);
      aResult = aNewVtx;
      break;
    }

    case BRepGraph_NodeId::Kind::Edge: {
      const BRepGraphInc::EdgeEntity& anEdge = theStorage.Edge(BRepGraph_EdgeId(theNode.Index));
      TopoDS_Edge                     aNewEdge;
      if (anEdge.IsDegenerate)
      {
        aBB.MakeEdge(aNewEdge);
        aBB.Degenerated(aNewEdge, true);
      }
      else if (anEdge.Curve3DRepId.IsValid())
      {
        const occ::handle<Geom_Curve>& aCurve3d = theStorage.Curve3DRep(anEdge.Curve3DRepId).Curve;
        if (!aCurve3d.IsNull())
          aBB.MakeEdge(aNewEdge, aCurve3d, TopLoc_Location(), anEdge.Tolerance);
        else
          aBB.MakeEdge(aNewEdge);
      }
      else
      {
        aBB.MakeEdge(aNewEdge);
      }
      aBB.Range(aNewEdge, anEdge.ParamFirst, anEdge.ParamLast);
      aBB.SameParameter(aNewEdge, anEdge.SameParameter);
      aBB.SameRange(aNewEdge, anEdge.SameRange);

      if (anEdge.StartVertexRefId.IsValid())
      {
        const BRepGraphInc::VertexRefEntry& aStartVR =
          theStorage.VertexRefEntry(anEdge.StartVertexRefId);
        TopoDS_Shape aStartVtx = Node(theStorage, aStartVR.VertexEntityId, theCache);
        if (!aStartVtx.IsNull())
        {
          aStartVtx.Orientation(TopAbs_FORWARD);
          if (!aStartVR.LocalLocation.IsIdentity())
            aStartVtx.Location(aStartVR.LocalLocation);
          aBB.Add(aNewEdge, aStartVtx);
        }
      }
      if (anEdge.EndVertexRefId.IsValid())
      {
        const BRepGraphInc::VertexRefEntry& anEndVR =
          theStorage.VertexRefEntry(anEdge.EndVertexRefId);
        TopoDS_Shape anEndVtx = Node(theStorage, anEndVR.VertexEntityId, theCache);
        if (!anEndVtx.IsNull())
        {
          anEndVtx.Orientation(TopAbs_REVERSED);
          if (!anEndVR.LocalLocation.IsIdentity())
            anEndVtx.Location(anEndVR.LocalLocation);
          aBB.Add(aNewEdge, anEndVtx);
        }
      }
      for (int i = 0; i < anEdge.InternalVertexRefIds.Length(); ++i)
      {
        const BRepGraphInc::VertexRefEntry& aVR =
          theStorage.VertexRefEntry(anEdge.InternalVertexRefIds.Value(i));
        TopoDS_Shape aVtx = Node(theStorage, aVR.VertexEntityId, theCache);
        if (!aVtx.IsNull())
        {
          aVtx.Orientation(aVR.Orientation);
          if (!aVR.LocalLocation.IsIdentity())
            aVtx.Location(aVR.LocalLocation);
          aBB.Add(aNewEdge, aVtx);
        }
      }
      // Attach Polygon3D discretization.
      if (anEdge.Polygon3DRepId.IsValid())
      {
        const occ::handle<Poly_Polygon3D>& aPolygon3D =
          theStorage.Polygon3DRep(anEdge.Polygon3DRepId).Polygon;
        if (!aPolygon3D.IsNull())
          aBB.UpdateEdge(aNewEdge, aPolygon3D, TopLoc_Location());
      }
      if (anEdge.IsClosed)
        aNewEdge.Closed(true);
      aResult = aNewEdge;
      break;
    }

    case BRepGraph_NodeId::Kind::Wire: {
      const BRepGraphInc::WireEntity& aWire = theStorage.Wire(BRepGraph_WireId(theNode.Index));
      TopoDS_Wire                     aNewWire;
      aBB.MakeWire(aNewWire);
      for (int aRefOrd = 0; aRefOrd < aWire.CoEdgeRefIds.Length(); ++aRefOrd)
      {
        const BRepGraphInc::CoEdgeRefEntry& aCoEdgeRef =
          theStorage.CoEdgeRefEntry(aWire.CoEdgeRefIds.Value(aRefOrd));
        if (aCoEdgeRef.IsRemoved
            || !aCoEdgeRef.CoEdgeEntityId.IsValid(theStorage.NbCoEdges()))
          continue;
        const BRepGraphInc::CoEdgeEntity& aCoEdge =
          theStorage.CoEdge(BRepGraph_CoEdgeId(aCoEdgeRef.CoEdgeEntityId.Index));
        if (aCoEdge.IsRemoved || !aCoEdge.EdgeEntityId.IsValid(theStorage.NbEdges()))
          continue;
        TopoDS_Shape anEdge = Node(theStorage, aCoEdge.EdgeEntityId, theCache);
        if (!anEdge.IsNull())
        {
          anEdge.Orientation(aCoEdge.Sense);
          if (!aCoEdgeRef.LocalLocation.IsIdentity())
            anEdge.Location(aCoEdgeRef.LocalLocation);
          aBB.Add(aNewWire, anEdge);
        }
      }
      if (aWire.IsClosed)
        aNewWire.Closed(true);
      aResult = aNewWire;
      break;
    }

    case BRepGraph_NodeId::Kind::Face: {
      aResult = FaceWithCache(theStorage, theNode.Index, theCache);
      break;
    }

    case BRepGraph_NodeId::Kind::Shell: {
      const BRepGraphInc::ShellEntity& aShell = theStorage.Shell(BRepGraph_ShellId(theNode.Index));
      TopoDS_Shell                     aNewShell;
      aBB.MakeShell(aNewShell);
      for (int aRefOrd = 0; aRefOrd < aShell.FaceRefIds.Length(); ++aRefOrd)
      {
        const BRepGraphInc::FaceRefEntry& aRef = theStorage.FaceRefEntry(aShell.FaceRefIds.Value(aRefOrd));
        if (aRef.IsRemoved || !aRef.FaceEntityId.IsValid(theStorage.NbFaces()))
          continue;
        TopoDS_Shape aFace = FaceWithCache(theStorage, aRef.FaceEntityId.Index, theCache);
        if (!aFace.IsNull())
        {
          aFace.Orientation(aRef.Orientation);
          if (!aRef.LocalLocation.IsIdentity())
            aFace.Location(aRef.LocalLocation);
          aBB.Add(aNewShell, aFace);
        }
      }
      // Reconstruct free children (wires, edges) attached directly to the shell.
      for (int i = 0; i < aShell.FreeChildRefIds.Length(); ++i)
      {
        const BRepGraphInc::ChildRefEntry& aRef =
          theStorage.ChildRefEntry(aShell.FreeChildRefIds.Value(i));
        TopoDS_Shape aChild = Node(theStorage, aRef.ChildEntityId, theCache);
        if (!aChild.IsNull())
        {
          aChild.Orientation(aRef.Orientation);
          if (!aRef.LocalLocation.IsIdentity())
            aChild.Location(aRef.LocalLocation);
          aBB.Add(aNewShell, aChild);
        }
      }
      if (aShell.IsClosed)
        aNewShell.Closed(true);
      aResult = aNewShell;
      break;
    }

    case BRepGraph_NodeId::Kind::Solid: {
      const BRepGraphInc::SolidEntity& aSolid = theStorage.Solid(BRepGraph_SolidId(theNode.Index));
      TopoDS_Solid                     aNewSolid;
      aBB.MakeSolid(aNewSolid);
      for (int aRefOrd = 0; aRefOrd < aSolid.ShellRefIds.Length(); ++aRefOrd)
      {
        const BRepGraphInc::ShellRefEntry& aShellRef =
          theStorage.ShellRefEntry(aSolid.ShellRefIds.Value(aRefOrd));
        if (aShellRef.IsRemoved
            || !aShellRef.ShellEntityId.IsValid(theStorage.NbShells()))
          continue;
        TopoDS_Shape                  aShell    = Node(theStorage, aShellRef.ShellEntityId, theCache);
        if (!aShell.IsNull())
        {
          aShell.Orientation(aShellRef.Orientation);
          if (!aShellRef.LocalLocation.IsIdentity())
            aShell.Location(aShellRef.LocalLocation);
          aBB.Add(aNewSolid, aShell);
        }
      }
      // Free children of the solid (edges, vertices).
      for (int i = 0; i < aSolid.FreeChildRefIds.Length(); ++i)
      {
        const BRepGraphInc::ChildRefEntry& aCR =
          theStorage.ChildRefEntry(aSolid.FreeChildRefIds.Value(i));
        TopoDS_Shape aChild = Node(theStorage, aCR.ChildEntityId, theCache);
        if (!aChild.IsNull())
        {
          aChild.Orientation(aCR.Orientation);
          if (!aCR.LocalLocation.IsIdentity())
            aChild.Location(aCR.LocalLocation);
          aBB.Add(aNewSolid, aChild);
        }
      }
      aResult = aNewSolid;
      break;
    }

    case BRepGraph_NodeId::Kind::Compound: {
      const BRepGraphInc::CompoundEntity& aComp = theStorage.Compound(BRepGraph_CompoundId(theNode.Index));
      TopoDS_Compound aNewComp;
      aBB.MakeCompound(aNewComp);
      for (int aRefOrd = 0; aRefOrd < aComp.ChildRefIds.Length(); ++aRefOrd)
      {
        const BRepGraphInc::ChildRefEntry& aRef = theStorage.ChildRefEntry(aComp.ChildRefIds.Value(aRefOrd));
        if (aRef.IsRemoved || !aRef.ChildEntityId.IsValid())
          continue;
        TopoDS_Shape                  aChild = Node(theStorage, aRef.ChildEntityId, theCache);
        if (!aChild.IsNull())
        {
          aChild.Orientation(aRef.Orientation);
          if (!aRef.LocalLocation.IsIdentity())
            aChild.Location(aRef.LocalLocation);
          aBB.Add(aNewComp, aChild);
        }
      }
      aResult = aNewComp;
      break;
    }

    case BRepGraph_NodeId::Kind::CompSolid: {
      const BRepGraphInc::CompSolidEntity& aCS = theStorage.CompSolid(BRepGraph_CompSolidId(theNode.Index));
      TopoDS_CompSolid aNewCS;
      aBB.MakeCompSolid(aNewCS);
      for (int aRefOrd = 0; aRefOrd < aCS.SolidRefIds.Length(); ++aRefOrd)
      {
        const BRepGraphInc::SolidRefEntry& aRef = theStorage.SolidRefEntry(aCS.SolidRefIds.Value(aRefOrd));
        if (aRef.IsRemoved
            || !aRef.SolidEntityId.IsValid(theStorage.NbSolids()))
          continue;
        TopoDS_Shape                  aSolid = Node(theStorage, aRef.SolidEntityId, theCache);
        if (!aSolid.IsNull())
        {
          aSolid.Orientation(aRef.Orientation);
          if (!aRef.LocalLocation.IsIdentity())
            aSolid.Location(aRef.LocalLocation);
          aBB.Add(aNewCS, aSolid);
        }
      }
      aResult = aNewCS;
      break;
    }

    // CoEdge is not a standalone TopoDS shape - it is an edge-face binding.
    // Product and Occurrence are assembly-level entities without direct TopoDS equivalents.
    case BRepGraph_NodeId::Kind::CoEdge:
    case BRepGraph_NodeId::Kind::Product:
    case BRepGraph_NodeId::Kind::Occurrence:
      return TopoDS_Shape();
  }

  theCache.Bind(theNode, aResult);
  return aResult;
}

//=================================================================================================

TopoDS_Shape BRepGraphInc_Reconstruct::FaceWithCache(const BRepGraphInc_Storage& theStorage,
                                                     const int                   theFaceIdx,
                                                     Cache&                      theCache)
{
  if (theFaceIdx < 0 || theFaceIdx >= theStorage.NbFaces())
    return TopoDS_Shape();

  // Check cache first - 1 NodeId = 1 TShape.
  BRepGraph_NodeId    aFaceNodeId = BRepGraph_NodeId::Face(theFaceIdx);
  const TopoDS_Shape* aCachedFace = theCache.Seek(aFaceNodeId);
  if (aCachedFace != nullptr)
    return *aCachedFace;

  BRep_Builder                    aBB;
  const BRepGraphInc::FaceEntity& aFace = theStorage.Face(BRepGraph_FaceId(theFaceIdx));

  // Resolve surface from rep storage (may be null for bare topology faces).
  occ::handle<Geom_Surface> aFaceSurface;
  if (aFace.SurfaceRepId.IsValid())
    aFaceSurface = theStorage.SurfaceRep(aFace.SurfaceRepId).Surface;

  TopoDS_Face aNewFace;
  if (!aFaceSurface.IsNull())
    aBB.MakeFace(aNewFace, aFaceSurface, TopLoc_Location(), aFace.Tolerance);
  else
    aBB.MakeFace(aNewFace);

  // Attach triangulations.
  if (!aFace.TriangulationRepIds.IsEmpty())
  {
    NCollection_List<occ::handle<Poly_Triangulation>> aTriList;
    occ::handle<Poly_Triangulation>                   anActiveTri;
    for (int aTriIdx = 0; aTriIdx < aFace.TriangulationRepIds.Length(); ++aTriIdx)
    {
      const BRepGraph_TriangulationRepId aTriRepId = aFace.TriangulationRepIds.Value(aTriIdx);
      if (!aTriRepId.IsValid())
        continue;
      const occ::handle<Poly_Triangulation>& aTri =
        theStorage.TriangulationRep(aTriRepId).Triangulation;
      if (!aTri.IsNull())
      {
        aTriList.Append(aTri);
        if (aTriIdx == aFace.ActiveTriangulationIndex)
          anActiveTri = aTri;
      }
    }
    if (!aTriList.IsEmpty())
    {
      const occ::handle<BRep_TFace>& aTFace = occ::down_cast<BRep_TFace>(aNewFace.TShape());
      if (!aTFace.IsNull())
        aTFace->Triangulations(aTriList, anActiveTri);
    }
  }

  // Helper: get or build edge from cache.
  const auto aGetOrBuildEdge = [&](const int theEdgeIdx) -> TopoDS_Edge {
    BRepGraph_NodeId    anEdgeId = BRepGraph_NodeId::Edge(theEdgeIdx);
    const TopoDS_Shape* aCached  = theCache.Seek(anEdgeId);
    if (aCached != nullptr)
      return TopoDS::Edge(*aCached);

    const BRepGraphInc::EdgeEntity& anEdge = theStorage.Edge(BRepGraph_EdgeId(theEdgeIdx));
    TopoDS_Edge                     aNewEdge;
    if (anEdge.IsDegenerate)
    {
      aBB.MakeEdge(aNewEdge);
      aBB.Degenerated(aNewEdge, true);
    }
    else if (anEdge.Curve3DRepId.IsValid())
    {
      const occ::handle<Geom_Curve>& aCurve3d = theStorage.Curve3DRep(anEdge.Curve3DRepId).Curve;
      if (!aCurve3d.IsNull())
        aBB.MakeEdge(aNewEdge, aCurve3d, TopLoc_Location(), anEdge.Tolerance);
      else
        aBB.MakeEdge(aNewEdge);
    }
    else
    {
      aBB.MakeEdge(aNewEdge);
    }
    aBB.Range(aNewEdge, anEdge.ParamFirst, anEdge.ParamLast);
    aBB.SameParameter(aNewEdge, anEdge.SameParameter);
    aBB.SameRange(aNewEdge, anEdge.SameRange);

    // Vertices (also cached).
    const auto aGetOrBuildVertex = [&](const int theVtxIdx) -> TopoDS_Shape {
      if (theVtxIdx < 0)
        return TopoDS_Shape();
      BRepGraph_NodeId    aVtxId     = BRepGraph_NodeId::Vertex(theVtxIdx);
      const TopoDS_Shape* aVtxCached = theCache.Seek(aVtxId);
      if (aVtxCached != nullptr)
        return *aVtxCached;
      const BRepGraphInc::VertexEntity& aVtx = theStorage.Vertex(BRepGraph_VertexId(theVtxIdx));
      TopoDS_Vertex                     aNewVtx;
      aBB.MakeVertex(aNewVtx, aVtx.Point, aVtx.Tolerance);
      theCache.Bind(aVtxId, aNewVtx);
      return aNewVtx;
    };

    if (anEdge.StartVertexRefId.IsValid())
    {
      const BRepGraphInc::VertexRefEntry& aStartVR =
        theStorage.VertexRefEntry(anEdge.StartVertexRefId);
      TopoDS_Shape aStartVtx = aGetOrBuildVertex(aStartVR.VertexEntityId.Index);
      if (!aStartVtx.IsNull())
      {
        aStartVtx.Orientation(TopAbs_FORWARD);
        if (!aStartVR.LocalLocation.IsIdentity())
          aStartVtx.Location(aStartVR.LocalLocation);
        aBB.Add(aNewEdge, aStartVtx);
      }
    }
    if (anEdge.EndVertexRefId.IsValid())
    {
      const BRepGraphInc::VertexRefEntry& anEndVR =
        theStorage.VertexRefEntry(anEdge.EndVertexRefId);
      TopoDS_Shape anEndVtx = aGetOrBuildVertex(anEndVR.VertexEntityId.Index);
      if (!anEndVtx.IsNull())
      {
        anEndVtx.Orientation(TopAbs_REVERSED);
        if (!anEndVR.LocalLocation.IsIdentity())
          anEndVtx.Location(anEndVR.LocalLocation);
        aBB.Add(aNewEdge, anEndVtx);
      }
    }
    for (int anIntIdx = 0; anIntIdx < anEdge.InternalVertexRefIds.Length(); ++anIntIdx)
    {
      const BRepGraphInc::VertexRefEntry& aVR =
        theStorage.VertexRefEntry(anEdge.InternalVertexRefIds.Value(anIntIdx));
      TopoDS_Shape aVtx = aGetOrBuildVertex(aVR.VertexEntityId.Index);
      if (!aVtx.IsNull())
      {
        aVtx.Orientation(aVR.Orientation);
        if (!aVR.LocalLocation.IsIdentity())
          aVtx.Location(aVR.LocalLocation);
        aBB.Add(aNewEdge, aVtx);
      }
    }

    // Polygon3D.
    if (anEdge.Polygon3DRepId.IsValid())
    {
      const occ::handle<Poly_Polygon3D>& aPolygon3D =
        theStorage.Polygon3DRep(anEdge.Polygon3DRepId).Polygon;
      if (!aPolygon3D.IsNull())
        aBB.UpdateEdge(aNewEdge, aPolygon3D, TopLoc_Location());
    }

    // Regularities.
    for (int aRegIdx = 0; aRegIdx < anEdge.Regularities.Length(); ++aRegIdx)
    {
      const BRepGraphInc::EdgeEntity::RegularityEntry& aRegEntry =
        anEdge.Regularities.Value(aRegIdx);
      const BRepGraph_NodeId& aFaceId1    = aRegEntry.FaceEntity1;
      const BRepGraph_NodeId& aFaceId2    = aRegEntry.FaceEntity2;
      const TopoDS_Shape*     aFaceShape1 = theCache.Seek(aFaceId1);
      const TopoDS_Shape*     aFaceShape2 = theCache.Seek(aFaceId2);
      if (aFaceShape1 != nullptr && aFaceShape2 != nullptr)
      {
        aBB.Continuity(aNewEdge,
                       TopoDS::Face(*aFaceShape1),
                       TopoDS::Face(*aFaceShape2),
                       aRegEntry.Continuity);
      }
    }

    if (anEdge.IsClosed)
      aNewEdge.Closed(true);

    theCache.Bind(anEdgeId, aNewEdge);
    return aNewEdge;
  };

  // Build wires for this face.
  // Wire TShape is cached (1 NodeId = 1 TShape); PCurve attachment is per-face.
  // theWireLocation is the wire's LocalLocation within the face (WireRef.LocalLocation),
  // needed to compute the correct CurveRepresentation location for PCurve binding.
  const auto aBuildWireForFace = [&](BRepGraph_WireId       theWireId,
                                     const TopLoc_Location& theWireLocation) -> TopoDS_Wire {
    const BRepGraphInc::WireEntity& aWire       = theStorage.Wire(theWireId);
    BRepGraph_NodeId                aWireNodeId = theWireId;

    // Get or create wire TShape.
    const TopoDS_Shape* aCachedWire = theCache.Seek(aWireNodeId);
    TopoDS_Wire         aNewWire;
    if (aCachedWire != nullptr)
    {
      aNewWire = TopoDS::Wire(*aCachedWire);
    }
    else
    {
      aBB.MakeWire(aNewWire);
      // Add edges in original storage order (preserving input shape order).
      for (int aRefOrd = 0; aRefOrd < aWire.CoEdgeRefIds.Length(); ++aRefOrd)
      {
        const BRepGraphInc::CoEdgeRefEntry& aCoEdgeRef =
          theStorage.CoEdgeRefEntry(aWire.CoEdgeRefIds.Value(aRefOrd));
        if (aCoEdgeRef.IsRemoved
            || !aCoEdgeRef.CoEdgeEntityId.IsValid(theStorage.NbCoEdges()))
          continue;
        const BRepGraphInc::CoEdgeEntity& aCoEdge =
          theStorage.CoEdge(BRepGraph_CoEdgeId(aCoEdgeRef.CoEdgeEntityId.Index));
        if (aCoEdge.IsRemoved || !aCoEdge.EdgeEntityId.IsValid(theStorage.NbEdges()))
          continue;
        TopoDS_Edge anEdge = aGetOrBuildEdge(aCoEdge.EdgeEntityId.Index);
        anEdge.Orientation(aCoEdge.Sense);
        if (!aCoEdgeRef.LocalLocation.IsIdentity())
          anEdge.Location(aCoEdgeRef.LocalLocation);
        aBB.Add(aNewWire, anEdge);
      }
      theCache.Bind(aWireNodeId, aNewWire);
    }
    // Apply closure flag after all edges are added (BRep_Builder::Add may reset it).
    if (aWire.IsClosed)
    {
      aNewWire.Closed(true);
    }

    // Attach PCurves/polygons for THIS face context onto shared edge TShapes.
    // Each CoEdge carries its PCurve directly - no need to scan edge.PCurves by face.
    // The edge must temporarily carry its composed face-hierarchy location so that
    // BRep_Builder::UpdateEdge computes the correct CurveRepresentation storage key.
    // Without this, PCurves stored with Identity location become unfindable when
    // wire/edge instance locations within the face are non-Identity.
    for (int aRefOrd = 0; aRefOrd < aWire.CoEdgeRefIds.Length(); ++aRefOrd)
    {
      const BRepGraphInc::CoEdgeRefEntry& aCoEdgeRef =
        theStorage.CoEdgeRefEntry(aWire.CoEdgeRefIds.Value(aRefOrd));
      if (aCoEdgeRef.IsRemoved
          || !aCoEdgeRef.CoEdgeEntityId.IsValid(theStorage.NbCoEdges()))
        continue;
      const BRepGraphInc::CoEdgeEntity& aCoEdge =
        theStorage.CoEdge(BRepGraph_CoEdgeId(aCoEdgeRef.CoEdgeEntityId.Index));
      if (aCoEdge.IsRemoved || !aCoEdge.EdgeEntityId.IsValid(theStorage.NbEdges()))
        continue;
      TopoDS_Edge                     anEdge    = aGetOrBuildEdge(aCoEdge.EdgeEntityId.Index);
      const BRepGraphInc::EdgeEntity& anEdgeEnt = theStorage.Edge(aCoEdge.EdgeEntityId);

      // Compute composed edge location within the face TShape hierarchy.
      // This is wire-in-face Location * edge-in-wire Location.
      const TopLoc_Location aEdgeInFaceLoc = theWireLocation * aCoEdgeRef.LocalLocation;

      // Temporarily apply composed location to the bare cached edge before UpdateEdge.
      // UpdateEdge computes: stored_loc = L.Predivided(E.Location()) = L * E.Loc^-1.
      // With L = Identity and E.Loc = aEdgeInFaceLoc:
      //   stored_loc = aEdgeInFaceLoc^-1
      // Later, BRep_Tool search computes the same loc from face/wire/edge context. OK
      if (!aEdgeInFaceLoc.IsIdentity())
        anEdge.Location(aEdgeInFaceLoc);

      // Collect PCurve(s): primary from this coedge, seam from paired coedge.
      occ::handle<Geom2d_Curve> aPC1, aPC2;
      double                    aPCFirst = 0.0, aPCLast = 0.0;
      gp_Pnt2d                  aUV1, aUV2;
      bool                      aHasUV          = false;
      GeomAbs_Shape             aSeamContinuity = GeomAbs_C0;

      if (aCoEdge.Curve2DRepId.IsValid())
      {
        aPC1     = theStorage.Curve2DRep(aCoEdge.Curve2DRepId).Curve;
        aPCFirst = aCoEdge.ParamFirst;
        aPCLast  = aCoEdge.ParamLast;
        aUV1     = aCoEdge.UV1;
        aUV2     = aCoEdge.UV2;
        aHasUV   = true;
      }

      // For seam edges, get the paired coedge's PCurve.
      if (aCoEdge.SeamPairId.IsValid())
      {
        const BRepGraphInc::CoEdgeEntity& aSeamCoEdge = theStorage.CoEdge(aCoEdge.SeamPairId);
        if (aSeamCoEdge.Curve2DRepId.IsValid())
        {
          aPC2            = theStorage.Curve2DRep(aSeamCoEdge.Curve2DRepId).Curve;
          aSeamContinuity = aSeamCoEdge.SeamContinuity;
          if (aPC1.IsNull())
          {
            aPCFirst = aSeamCoEdge.ParamFirst;
            aPCLast  = aSeamCoEdge.ParamLast;
          }
        }
      }

      if (!aPC1.IsNull() && !aPC2.IsNull())
      {
        if (aHasUV)
          aBB.UpdateEdge(anEdge,
                         aPC1,
                         aPC2,
                         aFaceSurface,
                         TopLoc_Location(),
                         anEdgeEnt.Tolerance,
                         aUV1,
                         aUV2);
        else
          aBB.UpdateEdge(anEdge, aPC1, aPC2, aFaceSurface, TopLoc_Location(), anEdgeEnt.Tolerance);
        aBB.Range(anEdge, aFaceSurface, TopLoc_Location(), aPCFirst, aPCLast);

        // Restore seam continuity (UpdateEdge creates CurveOnClosedSurface with C0).
        if (aSeamContinuity != GeomAbs_C0)
        {
          // The stored CurveRepresentation location matches the edge's current location.
          const TopLoc_Location aCRLoc =
            aEdgeInFaceLoc.IsIdentity() ? TopLoc_Location() : aEdgeInFaceLoc.Inverted();
          const occ::handle<BRep_TEdge>& aTEdge = occ::down_cast<BRep_TEdge>(anEdge.TShape());
          if (!aTEdge.IsNull())
          {
            for (occ::handle<BRep_CurveRepresentation>& aCR : aTEdge->ChangeCurves())
            {
              if (!aCR.IsNull() && aCR->IsCurveOnClosedSurface()
                  && aCR->IsCurveOnSurface(aFaceSurface, aCRLoc))
              {
                occ::down_cast<BRep_CurveOnClosedSurface>(aCR)->Continuity(aSeamContinuity);
                break;
              }
            }
          }
        }
      }
      else if (!aPC1.IsNull())
      {
        if (aHasUV)
          aBB.UpdateEdge(anEdge,
                         aPC1,
                         aFaceSurface,
                         TopLoc_Location(),
                         anEdgeEnt.Tolerance,
                         aUV1,
                         aUV2);
        else
          aBB.UpdateEdge(anEdge, aPC1, aFaceSurface, TopLoc_Location(), anEdgeEnt.Tolerance);
        aBB.Range(anEdge, aFaceSurface, TopLoc_Location(), aPCFirst, aPCLast);
      }
      else if (!aPC2.IsNull())
      {
        aBB.UpdateEdge(anEdge, aPC2, aFaceSurface, TopLoc_Location(), anEdgeEnt.Tolerance);
        aBB.Range(anEdge, aFaceSurface, TopLoc_Location(), aPCFirst, aPCLast);
      }

      // Attach PolygonOnSurface from CoEdge.
      if (aCoEdge.Polygon2DRepId.IsValid())
      {
        const occ::handle<Poly_Polygon2D>& aPolyOnSurf =
          theStorage.Polygon2DRep(aCoEdge.Polygon2DRepId).Polygon;
        if (!aPolyOnSurf.IsNull())
          aBB.UpdateEdge(anEdge, aPolyOnSurf, aFaceSurface, TopLoc_Location());
      }

      // Attach PolygonOnTriangulation from CoEdge.
      for (int aPolyTriIdx = 0; aPolyTriIdx < aCoEdge.PolygonOnTriRepIds.Length(); ++aPolyTriIdx)
      {
        const BRepGraph_PolygonOnTriRepId aPolyOnTriRepId =
          aCoEdge.PolygonOnTriRepIds.Value(aPolyTriIdx);
        if (!aPolyOnTriRepId.IsValid())
          continue;
        const BRepGraphInc::PolygonOnTriRep& aPolyOnTriRep =
          theStorage.PolygonOnTriRep(aPolyOnTriRepId);
        if (aPolyOnTriRep.Polygon.IsNull() || !aPolyOnTriRep.TriangulationRepId.IsValid())
          continue;
        const occ::handle<Poly_Triangulation>& aTri =
          theStorage.TriangulationRep(aPolyOnTriRep.TriangulationRepId).Triangulation;
        if (!aTri.IsNull())
          aBB.UpdateEdge(anEdge, aPolyOnTriRep.Polygon, aTri, TopLoc_Location());
      }

      // Reset temporary edge location after all UpdateEdge calls.
      if (!aEdgeInFaceLoc.IsIdentity())
        anEdge.Location(TopLoc_Location());
    }

    return aNewWire;
  };

  // Add wires to face: outer first, then inner.
  // Wire orientation must be applied before adding to face.
  for (int aRefOrd = 0; aRefOrd < aFace.WireRefIds.Length(); ++aRefOrd)
  {
    const BRepGraphInc::WireRefEntry& aWireRef = theStorage.WireRefEntry(aFace.WireRefIds.Value(aRefOrd));
    if (aWireRef.IsRemoved || !aWireRef.WireEntityId.IsValid(theStorage.NbWires())
        || !aWireRef.IsOuter)
      continue;
    TopoDS_Wire aWire = aBuildWireForFace(aWireRef.WireEntityId, aWireRef.LocalLocation);
    aWire.Orientation(aWireRef.Orientation);
    if (!aWireRef.LocalLocation.IsIdentity())
      aWire.Location(aWireRef.LocalLocation);
    aBB.Add(aNewFace, aWire);
    break;
  }
  for (int aRefOrd = 0; aRefOrd < aFace.WireRefIds.Length(); ++aRefOrd)
  {
    const BRepGraphInc::WireRefEntry& aWireRef = theStorage.WireRefEntry(aFace.WireRefIds.Value(aRefOrd));
    if (aWireRef.IsRemoved || !aWireRef.WireEntityId.IsValid(theStorage.NbWires())
        || aWireRef.IsOuter)
      continue;
    TopoDS_Wire aWire = aBuildWireForFace(aWireRef.WireEntityId, aWireRef.LocalLocation);
    aWire.Orientation(aWireRef.Orientation);
    if (!aWireRef.LocalLocation.IsIdentity())
      aWire.Location(aWireRef.LocalLocation);
    aBB.Add(aNewFace, aWire);
  }

  // Add direct INTERNAL/EXTERNAL vertex children.
  for (int aRefIdx = 0; aRefIdx < theStorage.NbVertexRefs(); ++aRefIdx)
  {
    const BRepGraphInc::VertexRefEntry& aVR = theStorage.VertexRefEntry(BRepGraph_VertexRefId(aRefIdx));
    if (aVR.ParentId != aFaceNodeId || aVR.IsRemoved)
      continue;
    if (!aVR.VertexEntityId.IsValid())
      continue;
    const BRepGraphInc::VertexEntity& aVtxEnt    = theStorage.Vertex(aVR.VertexEntityId);
    BRepGraph_NodeId                  aVtxId     = aVR.VertexEntityId;
    const TopoDS_Shape*               aVtxCached = theCache.Seek(aVtxId);
    TopoDS_Shape                      aVtxShape;
    if (aVtxCached != nullptr)
    {
      aVtxShape = *aVtxCached;
    }
    else
    {
      TopoDS_Vertex aNewVtx;
      aBB.MakeVertex(aNewVtx, aVtxEnt.Point, aVtxEnt.Tolerance);
      theCache.Bind(aVtxId, aNewVtx);
      aVtxShape = aNewVtx;
    }
    aVtxShape.Orientation(aVR.Orientation);
    if (!aVR.LocalLocation.IsIdentity())
      aVtxShape.Location(aVR.LocalLocation);
    aBB.Add(aNewFace, aVtxShape);
  }

  // Restore vertex point representations now that all edges and this face are cached.
  // UpdateVertex modifies TShape in-place, so cached vertex shapes are updated.
  for (int aWireRefOrd = 0; aWireRefOrd < aFace.WireRefIds.Length(); ++aWireRefOrd)
  {
    const BRepGraphInc::WireRefEntry& aWireRef = theStorage.WireRefEntry(aFace.WireRefIds.Value(aWireRefOrd));
    if (aWireRef.IsRemoved || !aWireRef.WireEntityId.IsValid(theStorage.NbWires()))
      continue;
    const BRepGraphInc::WireEntity& aWireEnt = theStorage.Wire(aWireRef.WireEntityId);
    for (int aCoEdgeRefOrd = 0; aCoEdgeRefOrd < aWireEnt.CoEdgeRefIds.Length(); ++aCoEdgeRefOrd)
    {
      const BRepGraphInc::CoEdgeRefEntry& aCoEdgeRef =
        theStorage.CoEdgeRefEntry(aWireEnt.CoEdgeRefIds.Value(aCoEdgeRefOrd));
      if (aCoEdgeRef.IsRemoved
          || !aCoEdgeRef.CoEdgeEntityId.IsValid(theStorage.NbCoEdges()))
        continue;
      const BRepGraphInc::CoEdgeEntity& aCoEdge =
        theStorage.CoEdge(BRepGraph_CoEdgeId(aCoEdgeRef.CoEdgeEntityId.Index));
      const BRepGraphInc::EdgeEntity& anEdgeEnt              = theStorage.Edge(aCoEdge.EdgeEntityId);
      const auto aRestoreVertexPointReps = [&](BRepGraph_VertexId theVtxId) {
        if (!theVtxId.IsValid())
          return;
        const BRepGraphInc::VertexEntity& aVtx = theStorage.Vertex(theVtxId);
        if (aVtx.PointsOnCurve.IsEmpty() && aVtx.PointsOnSurface.IsEmpty())
          return;
        const TopoDS_Shape* aVtxCached = theCache.Seek(theVtxId);
        if (aVtxCached == nullptr || aVtxCached->IsNull())
          return;
        TopoDS_Vertex aVtxShape = TopoDS::Vertex(*aVtxCached);
        for (int aPOCIdx = 0; aPOCIdx < aVtx.PointsOnCurve.Length(); ++aPOCIdx)
        {
          const BRepGraphInc::VertexEntity::PointOnCurveEntry& aPOC =
            aVtx.PointsOnCurve.Value(aPOCIdx);
          const TopoDS_Shape* anEdgeCached = theCache.Seek(aPOC.EdgeEntityId);
          if (anEdgeCached != nullptr && !anEdgeCached->IsNull())
            aBB.UpdateVertex(aVtxShape,
                             aPOC.Parameter,
                             TopoDS::Edge(*anEdgeCached),
                             aVtx.Tolerance);
        }
        for (int aPOSIdx = 0; aPOSIdx < aVtx.PointsOnSurface.Length(); ++aPOSIdx)
        {
          const BRepGraphInc::VertexEntity::PointOnSurfaceEntry& aPOS =
            aVtx.PointsOnSurface.Value(aPOSIdx);
          const TopoDS_Shape* aFaceCached = theCache.Seek(aPOS.FaceEntityId);
          if (aFaceCached != nullptr && !aFaceCached->IsNull())
            aBB.UpdateVertex(aVtxShape,
                             aPOS.ParameterU,
                             aPOS.ParameterV,
                             TopoDS::Face(*aFaceCached),
                             aVtx.Tolerance);
        }
      };
      if (anEdgeEnt.StartVertexRefId.IsValid())
        aRestoreVertexPointReps(theStorage.VertexRefEntry(anEdgeEnt.StartVertexRefId).VertexEntityId);
      if (anEdgeEnt.EndVertexRefId.IsValid())
        aRestoreVertexPointReps(theStorage.VertexRefEntry(anEdgeEnt.EndVertexRefId).VertexEntityId);
    }
  }

  // NaturalRestriction must be set AFTER wires are added
  // (BRep_Builder::Add may reset the flag).
  if (aFace.NaturalRestriction)
    aBB.NaturalRestriction(aNewFace, true);

  aNewFace.Orientation(TopAbs_FORWARD);
  theCache.Bind(aFaceNodeId, aNewFace);
  return aNewFace;
}
