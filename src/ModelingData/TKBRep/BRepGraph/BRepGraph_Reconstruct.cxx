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

#include <BRepGraph_Reconstruct.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraph_ShapesView.hxx>

#include <BRep_Builder.hxx>
#include <TopoDS.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TopoDS_Compound.hxx>

//=================================================================================================

TopoDS_Shape BRepGraph_Reconstruct::Node(const BRepGraph& theGraph,
                                         BRepGraph_NodeId theNode)
{
  if (!theNode.IsValid())
    return TopoDS_Shape();

  BRep_Builder aBB;

  switch (theNode.Kind)
  {
    case BRepGraph_NodeKind::Vertex: {
      const BRepGraph_TopoNode::VertexDef& aVtxDef = theGraph.myData->myVertexDefs.Value(theNode.Index);
      TopoDS_Vertex aNewVtx;
      aBB.MakeVertex(aNewVtx, aVtxDef.Point, aVtxDef.Tolerance);
      return aNewVtx;
    }
    case BRepGraph_NodeKind::Edge: {
      const BRepGraph_TopoNode::EdgeDef& anEdgeDef = theGraph.myData->myEdgeDefs.Value(theNode.Index);
      TopoDS_Edge aNewEdge;
      if (anEdgeDef.IsDegenerate)
      {
        aBB.MakeEdge(aNewEdge);
        aBB.Degenerated(aNewEdge, true);
      }
      else if (anEdgeDef.CurveNodeId.IsValid())
      {
        const BRepGraph_GeomNode::Curve& aCurveNode =
          theGraph.myData->myCurves.Value(anEdgeDef.CurveNodeId.Index);
        TopLoc_Location aEdgeLoc;
        if (!anEdgeDef.Usages.IsEmpty())
          aEdgeLoc = theGraph.myData->myEdgeUsages.Value(anEdgeDef.Usages.Value(0).Index).LocalLocation;
        aBB.MakeEdge(aNewEdge, aCurveNode.CurveGeom, aEdgeLoc, anEdgeDef.Tolerance);
      }
      else
      {
        aBB.MakeEdge(aNewEdge);
      }
      aBB.Range(aNewEdge, anEdgeDef.ParamFirst, anEdgeDef.ParamLast);
      aBB.SameParameter(aNewEdge, anEdgeDef.SameParameter);
      aBB.SameRange(aNewEdge, anEdgeDef.SameRange);

      if (anEdgeDef.StartVertexDefId.IsValid())
      {
        TopoDS_Shape aStartVtx = theGraph.Shapes().Shape(anEdgeDef.StartVertexDefId);
        if (!aStartVtx.IsNull())
          aBB.Add(aNewEdge, aStartVtx.Oriented(TopAbs_FORWARD));
      }
      if (anEdgeDef.EndVertexDefId.IsValid())
      {
        TopoDS_Shape anEndVtx = theGraph.Shapes().Shape(anEdgeDef.EndVertexDefId);
        if (!anEndVtx.IsNull())
          aBB.Add(aNewEdge, anEndVtx.Oriented(TopAbs_REVERSED));
      }
      return aNewEdge;
    }
    case BRepGraph_NodeKind::Wire: {
      const BRepGraph_TopoNode::WireDef& aWireDef = theGraph.myData->myWireDefs.Value(theNode.Index);
      TopoDS_Wire aNewWire;
      aBB.MakeWire(aNewWire);
      for (int anEdgeIdx = 0; anEdgeIdx < aWireDef.OrderedEdges.Length(); ++anEdgeIdx)
      {
        const BRepGraph_TopoNode::WireDef::EdgeEntry& anEntry =
          aWireDef.OrderedEdges.Value(anEdgeIdx);
        TopoDS_Shape anEdge = theGraph.Shapes().Shape(anEntry.EdgeDefId);
        if (!anEdge.IsNull())
        {
          anEdge.Orientation(anEntry.OrientationInWire);
          aBB.Add(aNewWire, anEdge);
        }
      }
      if (aWireDef.IsClosed)
        aNewWire.Closed(true);
      return aNewWire;
    }
    case BRepGraph_NodeKind::Face: {
      const BRepGraph_TopoNode::FaceDef& aFaceDef = theGraph.myData->myFaceDefs.Value(theNode.Index);
      if (!aFaceDef.SurfNodeId.IsValid())
        return TopoDS_Shape();
      const BRepGraph_GeomNode::Surf& aSurfNode =
        theGraph.myData->mySurfaces.Value(aFaceDef.SurfNodeId.Index);

      TopLoc_Location aFaceGlobalLoc;
      TopAbs_Orientation aFaceOri = TopAbs_FORWARD;
      if (!aFaceDef.Usages.IsEmpty())
      {
        const BRepGraph_TopoNode::FaceUsage& aFaceUsage =
          theGraph.myData->myFaceUsages.Value(aFaceDef.Usages.Value(0).Index);
        aFaceGlobalLoc = aFaceUsage.GlobalLocation;
        aFaceOri = aFaceUsage.Orientation;
      }

      TopoDS_Face aNewFace;
      aBB.MakeFace(aNewFace, aSurfNode.Surface, aFaceGlobalLoc, aFaceDef.Tolerance);

      auto buildWireForFace = [&](BRepGraph_NodeId theWireDefId) -> TopoDS_Wire {
        TopoDS_Wire aNewWire;
        aBB.MakeWire(aNewWire);

        const BRepGraph_TopoNode::WireDef& aWireDef =
          theGraph.myData->myWireDefs.Value(theWireDefId.Index);
        for (int anEdgeIdx = 0; anEdgeIdx < aWireDef.OrderedEdges.Length(); ++anEdgeIdx)
        {
          const BRepGraph_TopoNode::WireDef::EdgeEntry& anEntry =
            aWireDef.OrderedEdges.Value(anEdgeIdx);
          const BRepGraph_TopoNode::EdgeDef& anEdgeDef =
            theGraph.myData->myEdgeDefs.Value(anEntry.EdgeDefId.Index);

          TopoDS_Edge anEdge;
          if (anEdgeDef.IsDegenerate)
          {
            aBB.MakeEdge(anEdge);
            aBB.Degenerated(anEdge, true);
          }
          else if (anEdgeDef.CurveNodeId.IsValid())
          {
            const BRepGraph_GeomNode::Curve& aCurveNode =
              theGraph.myData->myCurves.Value(anEdgeDef.CurveNodeId.Index);
            TopLoc_Location aEdgeLoc;
            if (!anEdgeDef.Usages.IsEmpty())
              aEdgeLoc = theGraph.myData->myEdgeUsages.Value(anEdgeDef.Usages.Value(0).Index).LocalLocation;
            aBB.MakeEdge(anEdge, aCurveNode.CurveGeom, aEdgeLoc, anEdgeDef.Tolerance);
          }
          else
          {
            aBB.MakeEdge(anEdge);
          }
          aBB.Range(anEdge, anEdgeDef.ParamFirst, anEdgeDef.ParamLast);
          aBB.SameParameter(anEdge, anEdgeDef.SameParameter);
          aBB.SameRange(anEdge, anEdgeDef.SameRange);

          if (anEdgeDef.StartVertexDefId.IsValid())
          {
            TopoDS_Shape aStartVtx = theGraph.Shapes().Shape(anEdgeDef.StartVertexDefId);
            if (!aStartVtx.IsNull())
              aBB.Add(anEdge, aStartVtx.Oriented(TopAbs_FORWARD));
          }
          if (anEdgeDef.EndVertexDefId.IsValid())
          {
            TopoDS_Shape anEndVtx = theGraph.Shapes().Shape(anEdgeDef.EndVertexDefId);
            if (!anEndVtx.IsNull())
              aBB.Add(anEdge, anEndVtx.Oriented(TopAbs_REVERSED));
          }

          // Attach PCurve(s) for THIS face context.
          Handle(Geom2d_Curve) aPC1, aPC2;
          double aPCFirst = 0.0, aPCLast = 0.0;
          for (int aPCIdx = 0; aPCIdx < anEdgeDef.PCurves.Length(); ++aPCIdx)
          {
            const BRepGraph_TopoNode::EdgeDef::PCurveEntry& aPCEntry =
              anEdgeDef.PCurves.Value(aPCIdx);
            if (aPCEntry.FaceDefId == aFaceDef.Id)
            {
              const BRepGraph_GeomNode::PCurve& aPCNode =
                theGraph.myData->myPCurves.Value(aPCEntry.PCurveNodeId.Index);
              if (!aPCNode.Curve2d.IsNull())
              {
                if (aPCEntry.EdgeOrientation == TopAbs_FORWARD)
                {
                  aPC1 = aPCNode.Curve2d;
                  aPCFirst = aPCNode.ParamFirst;
                  aPCLast  = aPCNode.ParamLast;
                }
                else
                {
                  aPC2 = aPCNode.Curve2d;
                  if (aPC1.IsNull())
                  {
                    aPCFirst = aPCNode.ParamFirst;
                    aPCLast  = aPCNode.ParamLast;
                  }
                }
              }
            }
          }
          if (!aPC1.IsNull() && !aPC2.IsNull())
          {
            aBB.UpdateEdge(anEdge, aPC1, aPC2,
                           aSurfNode.Surface, aFaceGlobalLoc,
                           anEdgeDef.Tolerance);
            aBB.Range(anEdge, aSurfNode.Surface, aFaceGlobalLoc,
                      aPCFirst, aPCLast);
          }
          else if (!aPC1.IsNull())
          {
            aBB.UpdateEdge(anEdge, aPC1,
                           aSurfNode.Surface, aFaceGlobalLoc,
                           anEdgeDef.Tolerance);
            aBB.Range(anEdge, aSurfNode.Surface, aFaceGlobalLoc,
                      aPCFirst, aPCLast);
          }
          else if (!aPC2.IsNull())
          {
            aBB.UpdateEdge(anEdge, aPC2,
                           aSurfNode.Surface, aFaceGlobalLoc,
                           anEdgeDef.Tolerance);
            aBB.Range(anEdge, aSurfNode.Surface, aFaceGlobalLoc,
                      aPCFirst, aPCLast);
          }

          anEdge.Orientation(anEntry.OrientationInWire);
          aBB.Add(aNewWire, anEdge);
        }

        if (aWireDef.IsClosed)
          aNewWire.Closed(true);
        return aNewWire;
      };

      if (!aFaceDef.Usages.IsEmpty())
      {
        const BRepGraph_TopoNode::FaceUsage& aFaceUsage =
          theGraph.myData->myFaceUsages.Value(aFaceDef.Usages.Value(0).Index);

        if (aFaceUsage.OuterWireUsage.IsValid())
        {
          BRepGraph_NodeId aWireDefId = theGraph.DefOf(aFaceUsage.OuterWireUsage);
          aBB.Add(aNewFace, buildWireForFace(aWireDefId));
        }

        for (int aWireIdx = 0; aWireIdx < aFaceUsage.InnerWireUsages.Length(); ++aWireIdx)
        {
          BRepGraph_NodeId aWireDefId = theGraph.DefOf(aFaceUsage.InnerWireUsages.Value(aWireIdx));
          aBB.Add(aNewFace, buildWireForFace(aWireDefId));
        }
      }

      if (aFaceDef.NaturalRestriction)
        aBB.NaturalRestriction(aNewFace, true);

      aNewFace.Orientation(aFaceOri);
      return aNewFace;
    }
    case BRepGraph_NodeKind::Shell: {
      const BRepGraph_TopoNode::ShellDef& aShellDef = theGraph.myData->myShellDefs.Value(theNode.Index);
      TopoDS_Shell aNewShell;
      aBB.MakeShell(aNewShell);
      if (!aShellDef.Usages.IsEmpty())
      {
        BRepGraph::ReconstructCache anEdgeCache;
        const BRepGraph_TopoNode::ShellUsage& aShellUsage =
          theGraph.myData->myShellUsages.Value(aShellDef.Usages.Value(0).Index);
        for (int aFaceIter = 0; aFaceIter < aShellUsage.FaceUsages.Length(); ++aFaceIter)
        {
          BRepGraph_NodeId aFaceDefId = theGraph.DefOf(aShellUsage.FaceUsages.Value(aFaceIter));
          TopoDS_Shape aFaceShape = FaceWithCache(theGraph, aFaceDefId, anEdgeCache);
          aBB.Add(aNewShell, aFaceShape);
        }
      }
      return aNewShell;
    }
    case BRepGraph_NodeKind::Solid: {
      const BRepGraph_TopoNode::SolidDef& aSolidDef = theGraph.myData->mySolidDefs.Value(theNode.Index);
      TopoDS_Solid aNewSolid;
      aBB.MakeSolid(aNewSolid);
      if (!aSolidDef.Usages.IsEmpty())
      {
        BRepGraph::ReconstructCache anEdgeCache;
        const BRepGraph_TopoNode::SolidUsage& aSolidUsage =
          theGraph.myData->mySolidUsages.Value(aSolidDef.Usages.Value(0).Index);
        for (int aShellIter = 0; aShellIter < aSolidUsage.ShellUsages.Length(); ++aShellIter)
        {
          BRepGraph_NodeId aShellDefId = theGraph.DefOf(aSolidUsage.ShellUsages.Value(aShellIter));
          const BRepGraph_TopoNode::ShellDef& aShellDef2 =
            theGraph.myData->myShellDefs.Value(aShellDefId.Index);
          TopoDS_Shell aNewShell2;
          aBB.MakeShell(aNewShell2);
          if (!aShellDef2.Usages.IsEmpty())
          {
            const BRepGraph_TopoNode::ShellUsage& aShellUsage =
              theGraph.myData->myShellUsages.Value(aShellDef2.Usages.Value(0).Index);
            for (int aFaceIter = 0; aFaceIter < aShellUsage.FaceUsages.Length(); ++aFaceIter)
            {
              BRepGraph_NodeId aFaceDefId =
                theGraph.DefOf(aShellUsage.FaceUsages.Value(aFaceIter));
              TopoDS_Shape aFaceShape = FaceWithCache(theGraph, aFaceDefId, anEdgeCache);
              aBB.Add(aNewShell2, aFaceShape);
            }
          }
          aBB.Add(aNewSolid, aNewShell2);
        }
      }
      return aNewSolid;
    }
    case BRepGraph_NodeKind::Compound: {
      const BRepGraph_TopoNode::CompoundDef& aCompDef =
        theGraph.myData->myCompoundDefs.Value(theNode.Index);
      TopoDS_Compound aNewCompound;
      aBB.MakeCompound(aNewCompound);
      if (!aCompDef.Usages.IsEmpty())
      {
        const BRepGraph_TopoNode::CompoundUsage& aCompUsage =
          theGraph.myData->myCompoundUsages.Value(aCompDef.Usages.Value(0).Index);
        for (int aChildIdx = 0; aChildIdx < aCompUsage.ChildUsages.Length(); ++aChildIdx)
        {
          BRepGraph_NodeId aChildDefId = theGraph.DefOf(aCompUsage.ChildUsages.Value(aChildIdx));
          TopoDS_Shape aChildShape = Node(theGraph, aChildDefId);
          if (!aChildShape.IsNull())
            aBB.Add(aNewCompound, aChildShape);
        }
      }
      return aNewCompound;
    }
    case BRepGraph_NodeKind::CompSolid: {
      const BRepGraph_TopoNode::CompSolidDef& aCSolidDef =
        theGraph.myData->myCompSolidDefs.Value(theNode.Index);
      TopoDS_CompSolid aNewCompSolid;
      aBB.MakeCompSolid(aNewCompSolid);
      if (!aCSolidDef.Usages.IsEmpty())
      {
        const BRepGraph_TopoNode::CompSolidUsage& aCSolidUsage =
          theGraph.myData->myCompSolidUsages.Value(aCSolidDef.Usages.Value(0).Index);
        for (int aSolidIdx = 0; aSolidIdx < aCSolidUsage.SolidUsages.Length(); ++aSolidIdx)
        {
          BRepGraph_NodeId aSolidDefId = theGraph.DefOf(aCSolidUsage.SolidUsages.Value(aSolidIdx));
          TopoDS_Shape aSolidShape = Node(theGraph, aSolidDefId);
          if (!aSolidShape.IsNull())
            aBB.Add(aNewCompSolid, aSolidShape);
        }
      }
      return aNewCompSolid;
    }
    default:
      return TopoDS_Shape();
  }
}

//=================================================================================================

TopoDS_Shape BRepGraph_Reconstruct::FaceWithCache(
  const BRepGraph&  theGraph,
  BRepGraph_NodeId  theNode,
  NCollection_DataMap<BRepGraph_NodeId, TopoDS_Shape>& theEdgeCache)
{
  if (!theNode.IsValid() || theNode.Kind != BRepGraph_NodeKind::Face)
    return Node(theGraph, theNode);

  BRep_Builder aBB;
  const BRepGraph_TopoNode::FaceDef& aFaceDef = theGraph.myData->myFaceDefs.Value(theNode.Index);
  if (!aFaceDef.SurfNodeId.IsValid())
    return TopoDS_Shape();
  const BRepGraph_GeomNode::Surf& aSurfNode =
    theGraph.myData->mySurfaces.Value(aFaceDef.SurfNodeId.Index);

  TopLoc_Location aFaceGlobalLoc;
  TopAbs_Orientation aFaceOri = TopAbs_FORWARD;
  if (!aFaceDef.Usages.IsEmpty())
  {
    const BRepGraph_TopoNode::FaceUsage& aFaceUsage =
      theGraph.myData->myFaceUsages.Value(aFaceDef.Usages.Value(0).Index);
    aFaceGlobalLoc = aFaceUsage.GlobalLocation;
    aFaceOri = aFaceUsage.Orientation;
  }

  TopoDS_Face aNewFace;
  aBB.MakeFace(aNewFace, aSurfNode.Surface, aFaceGlobalLoc, aFaceDef.Tolerance);

  auto getOrBuildEdge = [&](BRepGraph_NodeId theEdgeDefId) -> TopoDS_Edge {
    const TopoDS_Shape* aCached = theEdgeCache.Seek(theEdgeDefId);
    if (aCached != nullptr)
      return TopoDS::Edge(*aCached);

    const BRepGraph_TopoNode::EdgeDef& anEdgeDef =
      theGraph.myData->myEdgeDefs.Value(theEdgeDefId.Index);
    TopoDS_Edge anEdge;
    if (anEdgeDef.IsDegenerate)
    {
      aBB.MakeEdge(anEdge);
      aBB.Degenerated(anEdge, true);
    }
    else if (anEdgeDef.CurveNodeId.IsValid())
    {
      const BRepGraph_GeomNode::Curve& aCurveNode =
        theGraph.myData->myCurves.Value(anEdgeDef.CurveNodeId.Index);
      TopLoc_Location aEdgeLoc;
      if (!anEdgeDef.Usages.IsEmpty())
        aEdgeLoc = theGraph.myData->myEdgeUsages.Value(anEdgeDef.Usages.Value(0).Index).LocalLocation;
      aBB.MakeEdge(anEdge, aCurveNode.CurveGeom, aEdgeLoc, anEdgeDef.Tolerance);
    }
    else
    {
      aBB.MakeEdge(anEdge);
    }
    aBB.Range(anEdge, anEdgeDef.ParamFirst, anEdgeDef.ParamLast);
    aBB.SameParameter(anEdge, anEdgeDef.SameParameter);
    aBB.SameRange(anEdge, anEdgeDef.SameRange);

    // Add vertices (also cached).
    if (anEdgeDef.StartVertexDefId.IsValid())
    {
      const TopoDS_Shape* aVtxCached = theEdgeCache.Seek(anEdgeDef.StartVertexDefId);
      TopoDS_Shape aStartVtx;
      if (aVtxCached != nullptr)
        aStartVtx = *aVtxCached;
      else
      {
        aStartVtx = theGraph.Shapes().Shape(anEdgeDef.StartVertexDefId);
        if (!aStartVtx.IsNull())
          theEdgeCache.Bind(anEdgeDef.StartVertexDefId, aStartVtx);
      }
      if (!aStartVtx.IsNull())
        aBB.Add(anEdge, aStartVtx.Oriented(TopAbs_FORWARD));
    }
    if (anEdgeDef.EndVertexDefId.IsValid())
    {
      const TopoDS_Shape* aVtxCached = theEdgeCache.Seek(anEdgeDef.EndVertexDefId);
      TopoDS_Shape anEndVtx;
      if (aVtxCached != nullptr)
        anEndVtx = *aVtxCached;
      else
      {
        anEndVtx = theGraph.Shapes().Shape(anEdgeDef.EndVertexDefId);
        if (!anEndVtx.IsNull())
          theEdgeCache.Bind(anEdgeDef.EndVertexDefId, anEndVtx);
      }
      if (!anEndVtx.IsNull())
        aBB.Add(anEdge, anEndVtx.Oriented(TopAbs_REVERSED));
    }

    theEdgeCache.Bind(theEdgeDefId, anEdge);
    return anEdge;
  };

  auto buildWireForFace = [&](BRepGraph_NodeId theWireDefId) -> TopoDS_Wire {
    TopoDS_Wire aNewWire;
    aBB.MakeWire(aNewWire);

    const BRepGraph_TopoNode::WireDef& aWireDef =
      theGraph.myData->myWireDefs.Value(theWireDefId.Index);
    for (int anEdgeIdx = 0; anEdgeIdx < aWireDef.OrderedEdges.Length(); ++anEdgeIdx)
    {
      const BRepGraph_TopoNode::WireDef::EdgeEntry& anEntry =
        aWireDef.OrderedEdges.Value(anEdgeIdx);
      TopoDS_Edge anEdge = getOrBuildEdge(anEntry.EdgeDefId);

      // Attach PCurve(s) for THIS face context.
      const BRepGraph_TopoNode::EdgeDef& anEdgeDef =
        theGraph.myData->myEdgeDefs.Value(anEntry.EdgeDefId.Index);
      Handle(Geom2d_Curve) aPC1, aPC2;
      double aPCFirst = 0.0, aPCLast = 0.0;
      for (int aPCIdx = 0; aPCIdx < anEdgeDef.PCurves.Length(); ++aPCIdx)
      {
        const BRepGraph_TopoNode::EdgeDef::PCurveEntry& aPCEntry =
          anEdgeDef.PCurves.Value(aPCIdx);
        if (aPCEntry.FaceDefId == aFaceDef.Id)
        {
          const BRepGraph_GeomNode::PCurve& aPCNode =
            theGraph.myData->myPCurves.Value(aPCEntry.PCurveNodeId.Index);
          if (!aPCNode.Curve2d.IsNull())
          {
            if (aPCEntry.EdgeOrientation == TopAbs_FORWARD)
            {
              aPC1 = aPCNode.Curve2d;
              aPCFirst = aPCNode.ParamFirst;
              aPCLast  = aPCNode.ParamLast;
            }
            else
            {
              aPC2 = aPCNode.Curve2d;
              if (aPC1.IsNull())
              {
                aPCFirst = aPCNode.ParamFirst;
                aPCLast  = aPCNode.ParamLast;
              }
            }
          }
        }
      }
      if (!aPC1.IsNull() && !aPC2.IsNull())
      {
        aBB.UpdateEdge(anEdge, aPC1, aPC2,
                       aSurfNode.Surface, aFaceGlobalLoc,
                       anEdgeDef.Tolerance);
        aBB.Range(anEdge, aSurfNode.Surface, aFaceGlobalLoc,
                  aPCFirst, aPCLast);
      }
      else if (!aPC1.IsNull())
      {
        aBB.UpdateEdge(anEdge, aPC1,
                       aSurfNode.Surface, aFaceGlobalLoc,
                       anEdgeDef.Tolerance);
        aBB.Range(anEdge, aSurfNode.Surface, aFaceGlobalLoc,
                  aPCFirst, aPCLast);
      }
      else if (!aPC2.IsNull())
      {
        aBB.UpdateEdge(anEdge, aPC2,
                       aSurfNode.Surface, aFaceGlobalLoc,
                       anEdgeDef.Tolerance);
        aBB.Range(anEdge, aSurfNode.Surface, aFaceGlobalLoc,
                  aPCFirst, aPCLast);
      }

      anEdge.Orientation(anEntry.OrientationInWire);
      aBB.Add(aNewWire, anEdge);
    }

    if (aWireDef.IsClosed)
      aNewWire.Closed(true);
    return aNewWire;
  };

  if (!aFaceDef.Usages.IsEmpty())
  {
    const BRepGraph_TopoNode::FaceUsage& aFaceUsage =
      theGraph.myData->myFaceUsages.Value(aFaceDef.Usages.Value(0).Index);

    if (aFaceUsage.OuterWireUsage.IsValid())
    {
      BRepGraph_NodeId aWireDefId = theGraph.DefOf(aFaceUsage.OuterWireUsage);
      aBB.Add(aNewFace, buildWireForFace(aWireDefId));
    }

    for (int aWireIdx = 0; aWireIdx < aFaceUsage.InnerWireUsages.Length(); ++aWireIdx)
    {
      BRepGraph_NodeId aWireDefId = theGraph.DefOf(aFaceUsage.InnerWireUsages.Value(aWireIdx));
      aBB.Add(aNewFace, buildWireForFace(aWireDefId));
    }
  }

  aNewFace.Orientation(aFaceOri);
  return aNewFace;
}

//=================================================================================================

TopoDS_Shape BRepGraph_Reconstruct::Usage(const BRepGraph&  theGraph,
                                          BRepGraph_UsageId theUsage)
{
  if (!theUsage.IsValid())
    return TopoDS_Shape();

  BRepGraph_NodeId aDefId = theGraph.DefOf(theUsage);
  TopoDS_Shape aShape = Node(theGraph, aDefId);
  if (aShape.IsNull())
    return aShape;

  // Apply usage-specific location and orientation.
  TopLoc_Location    aUsageLoc;
  TopAbs_Orientation aUsageOri = TopAbs_FORWARD;
  switch (theUsage.Kind)
  {
    case BRepGraph_NodeKind::Vertex:
    {
      const BRepGraph_TopoNode::VertexUsage& aVU = theGraph.myData->myVertexUsages.Value(theUsage.Index);
      aUsageLoc = aVU.GlobalLocation;
      aUsageOri = aVU.Orientation;
      break;
    }
    case BRepGraph_NodeKind::Edge:
    {
      const BRepGraph_TopoNode::EdgeUsage& aEU = theGraph.myData->myEdgeUsages.Value(theUsage.Index);
      aUsageLoc = aEU.GlobalLocation;
      aUsageOri = aEU.Orientation;
      break;
    }
    case BRepGraph_NodeKind::Wire:
    {
      const BRepGraph_TopoNode::WireUsage& aWU = theGraph.myData->myWireUsages.Value(theUsage.Index);
      aUsageLoc = aWU.GlobalLocation;
      aUsageOri = aWU.Orientation;
      break;
    }
    case BRepGraph_NodeKind::Face:
    {
      const BRepGraph_TopoNode::FaceUsage& aFU = theGraph.myData->myFaceUsages.Value(theUsage.Index);
      aUsageLoc = aFU.GlobalLocation;
      aUsageOri = aFU.Orientation;
      break;
    }
    case BRepGraph_NodeKind::Shell:
    {
      const BRepGraph_TopoNode::ShellUsage& aSU = theGraph.myData->myShellUsages.Value(theUsage.Index);
      aUsageLoc = aSU.GlobalLocation;
      aUsageOri = aSU.Orientation;
      break;
    }
    case BRepGraph_NodeKind::Solid:
    {
      const BRepGraph_TopoNode::SolidUsage& aSU = theGraph.myData->mySolidUsages.Value(theUsage.Index);
      aUsageLoc = aSU.GlobalLocation;
      aUsageOri = aSU.Orientation;
      break;
    }
    case BRepGraph_NodeKind::Compound:
    {
      const BRepGraph_TopoNode::CompoundUsage& aCU = theGraph.myData->myCompoundUsages.Value(theUsage.Index);
      aUsageLoc = aCU.GlobalLocation;
      aUsageOri = aCU.Orientation;
      break;
    }
    case BRepGraph_NodeKind::CompSolid:
    {
      const BRepGraph_TopoNode::CompSolidUsage& aCU = theGraph.myData->myCompSolidUsages.Value(theUsage.Index);
      aUsageLoc = aCU.GlobalLocation;
      aUsageOri = aCU.Orientation;
      break;
    }
    default:
      break;
  }

  aShape.Location(aUsageLoc);
  aShape.Orientation(aUsageOri);
  return aShape;
}
