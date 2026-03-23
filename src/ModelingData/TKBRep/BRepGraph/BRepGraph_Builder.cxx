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

#include <BRepGraph_Builder.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_Data.hxx>

#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <OSD_Parallel.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>

namespace
{

//! Per-vertex data extracted from TopoDS in Phase 2 (parallel).
struct ExtractedVertex
{
  TopoDS_Vertex Shape;
  gp_Pnt        Point;
  double        Tolerance = 0.0;
};

//! Per-edge data extracted from TopoDS in Phase 2 (parallel).
struct ExtractedEdge
{
  TopoDS_Edge          Shape;
  Handle(Geom_Curve)   Curve3d;
  double               ParamFirst   = 0.0;
  double               ParamLast    = 0.0;
  double               Tolerance    = 0.0;
  bool                 IsDegenerate   = false;
  bool                 SameParameter  = false;
  bool                 SameRange      = false;
  ExtractedVertex      StartVertex;
  ExtractedVertex      EndVertex;
  TopAbs_Orientation   OrientationInWire = TopAbs_FORWARD;
  Handle(Geom2d_Curve) PCurve2d;
  double               PCFirst = 0.0;
  double               PCLast  = 0.0;
  Handle(Geom2d_Curve) PCurve2dReversed;  //!< Second PCurve for seam edges
  double               PCFirstReversed = 0.0;
  double               PCLastReversed  = 0.0;
};

//! Per-wire data extracted from TopoDS in Phase 2 (parallel).
struct ExtractedWire
{
  TopoDS_Wire                       Shape;
  bool                              IsOuter = false;
  NCollection_Vector<ExtractedEdge> Edges;
};

//! All data extracted from a single face -- filled in parallel, consumed sequentially.
struct FaceLocalData
{
  // Context from Phase 1 (sequential hierarchy scan).
  TopoDS_Face        Face;
  TopLoc_Location    ParentGlobalLoc;
  BRepGraph_UsageId  ParentShellUsage;

  // Geometry extracted in Phase 2 (parallel).
  Handle(Geom_Surface)                Surface;
  Handle(Poly_Triangulation)          Triangulation;
  double                              Tolerance          = 0.0;
  TopAbs_Orientation                  Orientation        = TopAbs_FORWARD;
  bool                                NaturalRestriction = false;
  NCollection_Vector<ExtractedWire>   Wires;
};

//! Extract per-face geometry/topology data from TopoDS.
void extractFaceData(FaceLocalData& theData)
{
  const TopoDS_Face& aFace = theData.Face;

  theData.Surface            = BRep_Tool::Surface(aFace);
  TopLoc_Location aFaceLoc;
  theData.Triangulation      = BRep_Tool::Triangulation(aFace, aFaceLoc);
  theData.Tolerance          = BRep_Tool::Tolerance(aFace);
  theData.Orientation        = aFace.Orientation();
  theData.NaturalRestriction = BRep_Tool::NaturalRestriction(aFace);

  const TopoDS_Face aForwardFace = TopoDS::Face(aFace.Oriented(TopAbs_FORWARD));
  const TopoDS_Wire anOuterWire  = BRepTools::OuterWire(aForwardFace);

  for (TopoDS_Iterator aWireIt(aForwardFace); aWireIt.More(); aWireIt.Next())
  {
    if (aWireIt.Value().ShapeType() != TopAbs_WIRE)
      continue;
    const TopoDS_Wire& aWire = TopoDS::Wire(aWireIt.Value());

    ExtractedWire aWireData;
    aWireData.Shape   = aWire;
    aWireData.IsOuter = aWire.IsSame(anOuterWire);

    for (BRepTools_WireExplorer anEdgeExp(aWire, aForwardFace); anEdgeExp.More(); anEdgeExp.Next())
    {
      const TopoDS_Edge& anEdge = anEdgeExp.Current();

      ExtractedEdge anEdgeData;
      anEdgeData.Shape             = anEdge;
      anEdgeData.Tolerance         = BRep_Tool::Tolerance(anEdge);
      anEdgeData.IsDegenerate      = BRep_Tool::Degenerated(anEdge);
      anEdgeData.SameParameter     = BRep_Tool::SameParameter(anEdge);
      anEdgeData.SameRange         = BRep_Tool::SameRange(anEdge);
      anEdgeData.OrientationInWire = anEdge.Orientation();

      double aFirst = 0.0, aLast = 0.0;
      anEdgeData.Curve3d    = BRep_Tool::Curve(anEdge, aFirst, aLast);
      anEdgeData.ParamFirst = aFirst;
      anEdgeData.ParamLast  = aLast;

      TopoDS_Vertex aVFirst, aVLast;
      TopExp::Vertices(anEdge, aVFirst, aVLast);

      if (!aVFirst.IsNull())
      {
        anEdgeData.StartVertex.Shape     = aVFirst;
        anEdgeData.StartVertex.Point     = BRep_Tool::Pnt(aVFirst);
        anEdgeData.StartVertex.Tolerance = BRep_Tool::Tolerance(aVFirst);
      }
      if (!aVLast.IsNull())
      {
        anEdgeData.EndVertex.Shape     = aVLast;
        anEdgeData.EndVertex.Point     = BRep_Tool::Pnt(aVLast);
        anEdgeData.EndVertex.Tolerance = BRep_Tool::Tolerance(aVLast);
      }

      double aPCFirst = 0.0, aPCLast = 0.0;
      anEdgeData.PCurve2d = BRep_Tool::CurveOnSurface(anEdge, aForwardFace, aPCFirst, aPCLast);
      anEdgeData.PCFirst  = aPCFirst;
      anEdgeData.PCLast   = aPCLast;

      // Extract second PCurve for seam edges (reversed orientation).
      if (!anEdgeData.PCurve2d.IsNull())
      {
        double aPCFirstRev = 0.0, aPCLastRev = 0.0;
        TopoDS_Edge aReversedEdge = TopoDS::Edge(anEdge.Reversed());
        Handle(Geom2d_Curve) aPC2 =
          BRep_Tool::CurveOnSurface(aReversedEdge, aForwardFace, aPCFirstRev, aPCLastRev);
        if (!aPC2.IsNull() && aPC2 != anEdgeData.PCurve2d)
        {
          anEdgeData.PCurve2dReversed = aPC2;
          anEdgeData.PCFirstReversed  = aPCFirstRev;
          anEdgeData.PCLastReversed   = aPCLastRev;
        }
      }

      aWireData.Edges.Append(anEdgeData);
    }

    theData.Wires.Append(aWireData);
  }
}

} // anonymous namespace

//=================================================================================================

template <typename FaceDataVec>
void BRepGraph_Builder::registerFaceData(BRepGraph&          theGraph,
                                         const FaceDataVec&  theFaceData)
{
  for (int aFaceDataIdx = 0; aFaceDataIdx < theFaceData.Length(); ++aFaceDataIdx)
  {
    const FaceLocalData& aData    = theFaceData.Value(aFaceDataIdx);
    const TopoDS_Face&   aCurFace = aData.Face;

    // Create or reuse FaceDef.
    const TopoDS_TShape*    aFaceTShapeKey    = aCurFace.TShape().get();
    const BRepGraph_NodeId* anExistingFaceDef = theGraph.myData->myTShapeToDefId.Seek(aFaceTShapeKey);

    BRepGraph_NodeId aFaceDefId;
    if (anExistingFaceDef != nullptr)
    {
      aFaceDefId = *anExistingFaceDef;
    }
    else
    {
      BRepGraph_TopoNode::FaceDef& aFaceDef = theGraph.myData->myFaceDefs.Appended();
      const int aFaceDefIdx = theGraph.myData->myFaceDefs.Length() - 1;
      aFaceDef.Id                 = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, aFaceDefIdx);
      aFaceDef.Tolerance          = aData.Tolerance;
      aFaceDef.NaturalRestriction = aData.NaturalRestriction;
      theGraph.allocateUID(aFaceDef.Id);

      aFaceDef.SurfNodeId = theGraph.registerSurface(aData.Surface, aData.Triangulation);
      if (aFaceDef.SurfNodeId.IsValid())
      {
        theGraph.myData->mySurfaces.ChangeValue(aFaceDef.SurfNodeId.Index).FaceDefUsers.Append(aFaceDef.Id);
      }

      theGraph.myData->myTShapeToDefId.Bind(aFaceTShapeKey, aFaceDef.Id);
      theGraph.myData->myOriginalShapes.Bind(aFaceDef.Id, aCurFace);
      aFaceDefId = aFaceDef.Id;
    }

    // Always create FaceUsage.
    BRepGraph_TopoNode::FaceUsage& aFaceUsage = theGraph.myData->myFaceUsages.Appended();
    const int aFaceUsageIdx = theGraph.myData->myFaceUsages.Length() - 1;
    aFaceUsage.UsageId        = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Face, aFaceUsageIdx);
    aFaceUsage.DefId          = aFaceDefId;
    aFaceUsage.LocalLocation  = aCurFace.Location();
    aFaceUsage.GlobalLocation = aData.ParentGlobalLoc * aCurFace.Location();
    aFaceUsage.Orientation    = aData.Orientation;
    aFaceUsage.ParentUsage    = aData.ParentShellUsage;
    theGraph.myData->myFaceDefs.ChangeValue(aFaceDefId.Index).Usages.Append(aFaceUsage.UsageId);

    // Link to parent shell usage.
    if (aData.ParentShellUsage.IsValid())
    {
      theGraph.myData->myShellUsages.ChangeValue(aData.ParentShellUsage.Index)
        .FaceUsages.Append(aFaceUsage.UsageId);
    }

    // Process wires.
    for (int aWireIter = 0; aWireIter < aData.Wires.Length(); ++aWireIter)
    {
      const ExtractedWire& aWireData = aData.Wires.Value(aWireIter);

      // Dedup wire definition by TShape.
      const TopoDS_TShape*    aWireTShapeKey    = aWireData.Shape.TShape().get();
      const BRepGraph_NodeId* anExistingWireDef = theGraph.myData->myTShapeToDefId.Seek(aWireTShapeKey);

      BRepGraph_NodeId aWireDefId;
      int              aWireDefIdx = -1;
      bool             aIsNewWireDef = false;

      if (anExistingWireDef != nullptr)
      {
        aWireDefId  = *anExistingWireDef;
        aWireDefIdx = aWireDefId.Index;
      }
      else
      {
        BRepGraph_TopoNode::WireDef& aWireDef = theGraph.myData->myWireDefs.Appended();
        aWireDefIdx = theGraph.myData->myWireDefs.Length() - 1;
        aWireDef.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Wire, aWireDefIdx);
        theGraph.allocateUID(aWireDef.Id);
        theGraph.myData->myOriginalShapes.Bind(aWireDef.Id, aWireData.Shape);
        theGraph.myData->myTShapeToDefId.Bind(aWireTShapeKey, aWireDef.Id);
        aWireDefId   = aWireDef.Id;
        aIsNewWireDef = true;
      }

      BRepGraph_TopoNode::WireUsage& aWireUsage = theGraph.myData->myWireUsages.Appended();
      const int aWireUsageIdx = theGraph.myData->myWireUsages.Length() - 1;
      aWireUsage.UsageId        = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Wire, aWireUsageIdx);
      aWireUsage.DefId          = aWireDefId;
      aWireUsage.LocalLocation  = aWireData.Shape.Location();
      aWireUsage.GlobalLocation = aFaceUsage.GlobalLocation * aWireData.Shape.Location();
      aWireUsage.Orientation    = aWireData.Shape.Orientation();
      aWireUsage.ParentUsage    = aFaceUsage.UsageId;
      aWireUsage.OwnerFaceUsage = aFaceUsage.UsageId;
      theGraph.myData->myWireDefs.ChangeValue(aWireDefIdx).Usages.Append(aWireUsage.UsageId);

      if (aWireData.IsOuter)
      {
        aFaceUsage.OuterWireUsage = aWireUsage.UsageId;
      }
      else
      {
        aFaceUsage.InnerWireUsages.Append(aWireUsage.UsageId);
      }

      BRepGraph_NodeId aFirstVertexDefId;
      BRepGraph_NodeId aLastVertexDefId;

      for (int anEdgeIter = 0; anEdgeIter < aWireData.Edges.Length(); ++anEdgeIter)
      {
        const ExtractedEdge& anEdgeData = aWireData.Edges.Value(anEdgeIter);
        const TopoDS_Edge&   anEdge     = anEdgeData.Shape;

        // Dedup edge definition by TShape.
        const TopoDS_TShape*    aTShapeKey          = anEdge.TShape().get();
        const BRepGraph_NodeId* anExistingEdgeDefId = theGraph.myData->myTShapeToDefId.Seek(aTShapeKey);

        BRepGraph_NodeId anEdgeDefId;

        if (anExistingEdgeDefId != nullptr)
        {
          anEdgeDefId = *anExistingEdgeDefId;
        }
        else
        {
          BRepGraph_TopoNode::EdgeDef& anEdgeDef = theGraph.myData->myEdgeDefs.Appended();
          const int anEdgeDefIdx = theGraph.myData->myEdgeDefs.Length() - 1;
          anEdgeDef.Id            = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, anEdgeDefIdx);
          anEdgeDef.Tolerance     = anEdgeData.Tolerance;
          anEdgeDef.IsDegenerate  = anEdgeData.IsDegenerate;
          anEdgeDef.SameParameter = anEdgeData.SameParameter;
          anEdgeDef.SameRange     = anEdgeData.SameRange;
          anEdgeDef.ParamFirst    = anEdgeData.ParamFirst;
          anEdgeDef.ParamLast     = anEdgeData.ParamLast;
          theGraph.allocateUID(anEdgeDef.Id);
          theGraph.myData->myOriginalShapes.Bind(anEdgeDef.Id, anEdge);

          if (!anEdgeData.Curve3d.IsNull())
          {
            anEdgeDef.CurveNodeId = theGraph.registerCurve(anEdgeData.Curve3d);
            if (anEdgeDef.CurveNodeId.IsValid())
            {
              theGraph.myData->myCurves.ChangeValue(anEdgeDef.CurveNodeId.Index)
                .EdgeDefUsers.Append(anEdgeDef.Id);
            }
          }

          // Vertex definitions.
          auto processVertexDef = [&](const ExtractedVertex& theVtxData) -> BRepGraph_NodeId {
            if (theVtxData.Shape.IsNull())
              return BRepGraph_NodeId();

            const TopoDS_TShape*    aVTShapeKey   = theVtxData.Shape.TShape().get();
            const BRepGraph_NodeId* anExistingVtx = theGraph.myData->myTShapeToDefId.Seek(aVTShapeKey);
            if (anExistingVtx != nullptr)
              return *anExistingVtx;

            BRepGraph_TopoNode::VertexDef& aVtxDef = theGraph.myData->myVertexDefs.Appended();
            const int aVtxDefIdx = theGraph.myData->myVertexDefs.Length() - 1;
            aVtxDef.Id        = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Vertex, aVtxDefIdx);
            aVtxDef.Point     = theVtxData.Point;
            aVtxDef.Tolerance = theVtxData.Tolerance;
            theGraph.allocateUID(aVtxDef.Id);
            theGraph.myData->myOriginalShapes.Bind(aVtxDef.Id, theVtxData.Shape);

            theGraph.myData->myTShapeToDefId.Bind(aVTShapeKey, aVtxDef.Id);
            return aVtxDef.Id;
          };

          anEdgeDef.StartVertexDefId = processVertexDef(anEdgeData.StartVertex);
          anEdgeDef.EndVertexDefId   = processVertexDef(anEdgeData.EndVertex);

          theGraph.myData->myTShapeToDefId.Bind(aTShapeKey, anEdgeDef.Id);
          anEdgeDefId = anEdgeDef.Id;
        }

        // Always create EdgeUsage.
        BRepGraph_TopoNode::EdgeUsage& anEdgeUsage = theGraph.myData->myEdgeUsages.Appended();
        const int anEdgeUsageIdx = theGraph.myData->myEdgeUsages.Length() - 1;
        anEdgeUsage.UsageId        = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Edge, anEdgeUsageIdx);
        anEdgeUsage.DefId          = anEdgeDefId;
        anEdgeUsage.LocalLocation  = anEdge.Location();
        anEdgeUsage.GlobalLocation = aWireUsage.GlobalLocation * anEdge.Location();
        anEdgeUsage.Orientation    = anEdgeData.OrientationInWire;
        anEdgeUsage.ParentUsage    = aWireUsage.UsageId;
        theGraph.myData->myEdgeDefs.ChangeValue(anEdgeDefId.Index).Usages.Append(anEdgeUsage.UsageId);

        // Create VertexUsages for this edge usage.
        auto createVertexUsage = [&](BRepGraph_NodeId        theVtxDefId,
                                     const ExtractedVertex&  theVtxData,
                                     const TopLoc_Location&  theEdgeGlobalLoc) -> BRepGraph_UsageId
        {
          if (!theVtxDefId.IsValid())
            return BRepGraph_UsageId();

          BRepGraph_TopoNode::VertexUsage& aVtxUsage = theGraph.myData->myVertexUsages.Appended();
          const int aVtxUsageIdx = theGraph.myData->myVertexUsages.Length() - 1;
          aVtxUsage.UsageId        = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Vertex, aVtxUsageIdx);
          aVtxUsage.DefId          = theVtxDefId;
          aVtxUsage.LocalLocation  = theVtxData.Shape.Location();
          aVtxUsage.GlobalLocation = theEdgeGlobalLoc * theVtxData.Shape.Location();
          aVtxUsage.Orientation    = theVtxData.Shape.Orientation();
          aVtxUsage.ParentUsage    = anEdgeUsage.UsageId;
          aVtxUsage.TransformedPoint = theVtxData.Point;
          theGraph.myData->myVertexDefs.ChangeValue(theVtxDefId.Index).Usages.Append(aVtxUsage.UsageId);
          return aVtxUsage.UsageId;
        };

        const BRepGraph_TopoNode::EdgeDef& anEdgeDefRef =
          theGraph.myData->myEdgeDefs.Value(anEdgeDefId.Index);
        anEdgeUsage.StartVertexUsage = createVertexUsage(
          anEdgeDefRef.StartVertexDefId, anEdgeData.StartVertex, anEdgeUsage.GlobalLocation);
        anEdgeUsage.EndVertexUsage = createVertexUsage(
          anEdgeDefRef.EndVertexDefId, anEdgeData.EndVertex, anEdgeUsage.GlobalLocation);

        aWireUsage.EdgeUsages.Append(anEdgeUsage.UsageId);

        // PCurve from pre-extracted data.
        if (!anEdgeData.PCurve2d.IsNull())
        {
          BRepGraph_NodeId aPCurveId = theGraph.createPCurveNode(
            anEdgeData.PCurve2d, anEdgeDefId, aFaceDefId,
            anEdgeData.PCFirst, anEdgeData.PCLast);

          BRepGraph_TopoNode::EdgeDef::PCurveEntry aPCEntry;
          aPCEntry.PCurveNodeId    = aPCurveId;
          aPCEntry.FaceDefId       = aFaceDefId;
          aPCEntry.EdgeOrientation = TopAbs_FORWARD;
          theGraph.myData->myEdgeDefs.ChangeValue(anEdgeDefId.Index).PCurves.Append(aPCEntry);
        }

        // Second PCurve for seam edges.
        if (!anEdgeData.PCurve2dReversed.IsNull())
        {
          BRepGraph_NodeId aPCurveId2 = theGraph.createPCurveNode(
            anEdgeData.PCurve2dReversed, anEdgeDefId, aFaceDefId,
            anEdgeData.PCFirstReversed, anEdgeData.PCLastReversed);

          BRepGraph_TopoNode::EdgeDef::PCurveEntry aPCEntry2;
          aPCEntry2.PCurveNodeId    = aPCurveId2;
          aPCEntry2.FaceDefId       = aFaceDefId;
          aPCEntry2.EdgeOrientation = TopAbs_REVERSED;
          theGraph.myData->myEdgeDefs.ChangeValue(anEdgeDefId.Index).PCurves.Append(aPCEntry2);
        }

        if (aIsNewWireDef)
        {
          // WireDef::EdgeEntry.
          BRepGraph_TopoNode::WireDef::EdgeEntry aWEEntry;
          aWEEntry.EdgeDefId         = anEdgeDefId;
          aWEEntry.OrientationInWire = anEdgeData.OrientationInWire;
          theGraph.myData->myWireDefs.ChangeValue(aWireDefIdx).OrderedEdges.Append(aWEEntry);

          // Populate edge-to-wire reverse index.
          if (!theGraph.myData->myEdgeToWires.IsBound(anEdgeDefId.Index))
            theGraph.myData->myEdgeToWires.Bind(anEdgeDefId.Index, NCollection_Vector<int>());
          theGraph.myData->myEdgeToWires.ChangeFind(anEdgeDefId.Index).Append(aWireDefIdx);

          // Track first/last vertex for closure check.
          if (!aFirstVertexDefId.IsValid())
          {
            if (anEdgeData.OrientationInWire == TopAbs_FORWARD)
              aFirstVertexDefId = anEdgeDefRef.StartVertexDefId;
            else
              aFirstVertexDefId = anEdgeDefRef.EndVertexDefId;
          }
          {
            if (anEdgeData.OrientationInWire == TopAbs_FORWARD)
              aLastVertexDefId = anEdgeDefRef.EndVertexDefId;
            else
              aLastVertexDefId = anEdgeDefRef.StartVertexDefId;
          }
        }
      }

      if (aIsNewWireDef)
      {
        // Set wire closure.
        theGraph.myData->myWireDefs.ChangeValue(aWireDefIdx).IsClosed =
          aFirstVertexDefId.IsValid() && aLastVertexDefId.IsValid()
          && aFirstVertexDefId == aLastVertexDefId;
      }
    }
  }
}

//=================================================================================================

void BRepGraph_Builder::computeMultiLocatedFlags(BRepGraph& theGraph)
{
  // A surface is multi-located only when its face usages have *different* GlobalLocations.
  for (int aSurfIdx = 0; aSurfIdx < theGraph.myData->mySurfaces.Length(); ++aSurfIdx)
  {
    BRepGraph_GeomNode::Surf& aSurf = theGraph.myData->mySurfaces.ChangeValue(aSurfIdx);
    if (aSurf.FaceDefUsers.Length() <= 1)
    {
      aSurf.IsMultiLocated = false;
      continue;
    }
    bool            aIsMultiLocated = false;
    TopLoc_Location aFirstLoc;
    bool            aFoundFirst = false;
    for (int aFDIdx = 0; aFDIdx < aSurf.FaceDefUsers.Length() && !aIsMultiLocated; ++aFDIdx)
    {
      const BRepGraph_TopoNode::FaceDef& aFaceDef =
        theGraph.myData->myFaceDefs.Value(aSurf.FaceDefUsers.Value(aFDIdx).Index);
      for (int aUIdx = 0; aUIdx < aFaceDef.Usages.Length() && !aIsMultiLocated; ++aUIdx)
      {
        const TopLoc_Location& aLoc =
          theGraph.myData->myFaceUsages.Value(aFaceDef.Usages.Value(aUIdx).Index).GlobalLocation;
        if (!aFoundFirst)
        {
          aFirstLoc   = aLoc;
          aFoundFirst = true;
        }
        else if (aLoc != aFirstLoc)
        {
          aIsMultiLocated = true;
        }
      }
    }
    aSurf.IsMultiLocated = aIsMultiLocated;
  }
  // Similarly for curves.
  for (int aCurveIdx = 0; aCurveIdx < theGraph.myData->myCurves.Length(); ++aCurveIdx)
  {
    BRepGraph_GeomNode::Curve& aCurve = theGraph.myData->myCurves.ChangeValue(aCurveIdx);
    if (aCurve.EdgeDefUsers.Length() <= 1)
    {
      aCurve.IsMultiLocated = false;
      continue;
    }
    bool            aIsMultiLocated = false;
    TopLoc_Location aFirstLoc;
    bool            aFoundFirst = false;
    for (int aEDIdx = 0; aEDIdx < aCurve.EdgeDefUsers.Length() && !aIsMultiLocated; ++aEDIdx)
    {
      const BRepGraph_TopoNode::EdgeDef& anEdgeDef =
        theGraph.myData->myEdgeDefs.Value(aCurve.EdgeDefUsers.Value(aEDIdx).Index);
      for (int aUIdx = 0; aUIdx < anEdgeDef.Usages.Length() && !aIsMultiLocated; ++aUIdx)
      {
        const TopLoc_Location& aLoc =
          theGraph.myData->myEdgeUsages.Value(anEdgeDef.Usages.Value(aUIdx).Index).GlobalLocation;
        if (!aFoundFirst)
        {
          aFirstLoc   = aLoc;
          aFoundFirst = true;
        }
        else if (aLoc != aFirstLoc)
        {
          aIsMultiLocated = true;
        }
      }
    }
    aCurve.IsMultiLocated = aIsMultiLocated;
  }
}

//=================================================================================================

void BRepGraph_Builder::Perform(BRepGraph&          theGraph,
                                const TopoDS_Shape& theShape,
                                bool                theParallel)
{
  // Phase 0: Clear all storage, increment generation.
  theGraph.myData->mySolidDefs.Clear();
  theGraph.myData->myShellDefs.Clear();
  theGraph.myData->myFaceDefs.Clear();
  theGraph.myData->myWireDefs.Clear();
  theGraph.myData->myEdgeDefs.Clear();
  theGraph.myData->myVertexDefs.Clear();
  theGraph.myData->myCompoundDefs.Clear();
  theGraph.myData->myCompSolidDefs.Clear();
  theGraph.myData->mySolidUsages.Clear();
  theGraph.myData->myShellUsages.Clear();
  theGraph.myData->myFaceUsages.Clear();
  theGraph.myData->myWireUsages.Clear();
  theGraph.myData->myEdgeUsages.Clear();
  theGraph.myData->myVertexUsages.Clear();
  theGraph.myData->myCompoundUsages.Clear();
  theGraph.myData->myCompSolidUsages.Clear();
  theGraph.myData->mySurfaces.Clear();
  theGraph.myData->myCurves.Clear();
  theGraph.myData->myPCurves.Clear();
  theGraph.myData->myOutRelEdges.Clear();
  theGraph.myData->myInRelEdges.Clear();
  theGraph.myData->mySurfRegistry.Clear();
  theGraph.myData->myCurveRegistry.Clear();
  theGraph.myData->myTShapeToDefId.Clear();
  theGraph.myData->myNodeToUID.Clear();
  theGraph.myData->myUIDToNodeId.Clear();
  theGraph.myData->myEdgeToWires.Clear();
  theGraph.myData->myHistoryLog.Clear();
  theGraph.myData->myOriginalShapes.Clear();
  theGraph.myData->myCurrentShapes.Clear();
  ++theGraph.myData->myGeneration;
  theGraph.myData->myIsDone = false;

  // Early exit for null shapes.
  if (theShape.IsNull())
    return;

  // Phase 1 (sequential): Recursively explore hierarchy, collecting face contexts.
  NCollection_Vector<FaceLocalData> aFaceData(128, theGraph.myData->myAllocator);

  // Recursive traversal function.
  std::function<BRepGraph_UsageId(const TopoDS_Shape&, BRepGraph_UsageId, const TopLoc_Location&)>
    traverseShape;

  traverseShape = [&](const TopoDS_Shape&    theCurrentShape,
                      BRepGraph_UsageId      theParentUsage,
                      const TopLoc_Location& theParentGlobalLoc) -> BRepGraph_UsageId
  {
    if (theCurrentShape.IsNull())
      return BRepGraph_UsageId();

    switch (theCurrentShape.ShapeType())
    {
      case TopAbs_COMPOUND:
      {
        const TopoDS_Compound& aCompound = TopoDS::Compound(theCurrentShape);

        BRepGraph_TopoNode::CompoundDef& aCompDef = theGraph.myData->myCompoundDefs.Appended();
        const int aCompDefIdx = theGraph.myData->myCompoundDefs.Length() - 1;
        aCompDef.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Compound, aCompDefIdx);
        theGraph.allocateUID(aCompDef.Id);
        theGraph.myData->myOriginalShapes.Bind(aCompDef.Id, aCompound);

        BRepGraph_TopoNode::CompoundUsage& aCompUsage = theGraph.myData->myCompoundUsages.Appended();
        const int aCompUsageIdx = theGraph.myData->myCompoundUsages.Length() - 1;
        aCompUsage.UsageId        = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Compound, aCompUsageIdx);
        aCompUsage.DefId          = aCompDef.Id;
        aCompUsage.LocalLocation  = aCompound.Location();
        aCompUsage.GlobalLocation = theParentGlobalLoc * aCompound.Location();
        aCompUsage.Orientation    = aCompound.Orientation();
        aCompUsage.ParentUsage    = theParentUsage;
        aCompDef.Usages.Append(aCompUsage.UsageId);

        const BRepGraph_UsageId aCompUsageId   = aCompUsage.UsageId;
        const TopLoc_Location   aCompGlobalLoc = aCompUsage.GlobalLocation;

        for (TopoDS_Iterator aChildIt(aCompound); aChildIt.More(); aChildIt.Next())
        {
          BRepGraph_UsageId aChildUsage = traverseShape(
            aChildIt.Value(), aCompUsageId, aCompGlobalLoc);
          if (aChildUsage.IsValid())
          {
            theGraph.myData->myCompoundUsages.ChangeValue(aCompUsageIdx).ChildUsages.Append(aChildUsage);
            theGraph.myData->myCompoundDefs.ChangeValue(aCompDefIdx).ChildDefIds.Append(
              theGraph.DefOf(aChildUsage));
          }
        }

        return aCompUsageId;
      }

      case TopAbs_COMPSOLID:
      {
        const TopoDS_CompSolid& aCompSolid = TopoDS::CompSolid(theCurrentShape);

        BRepGraph_TopoNode::CompSolidDef& aCSolidDef = theGraph.myData->myCompSolidDefs.Appended();
        const int aCSolidDefIdx = theGraph.myData->myCompSolidDefs.Length() - 1;
        aCSolidDef.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::CompSolid, aCSolidDefIdx);
        theGraph.allocateUID(aCSolidDef.Id);
        theGraph.myData->myOriginalShapes.Bind(aCSolidDef.Id, aCompSolid);

        BRepGraph_TopoNode::CompSolidUsage& aCSolidUsage = theGraph.myData->myCompSolidUsages.Appended();
        const int aCSolidUsageIdx = theGraph.myData->myCompSolidUsages.Length() - 1;
        aCSolidUsage.UsageId        = BRepGraph_UsageId(BRepGraph_NodeId::Kind::CompSolid, aCSolidUsageIdx);
        aCSolidUsage.DefId          = aCSolidDef.Id;
        aCSolidUsage.LocalLocation  = aCompSolid.Location();
        aCSolidUsage.GlobalLocation = theParentGlobalLoc * aCompSolid.Location();
        aCSolidUsage.Orientation    = aCompSolid.Orientation();
        aCSolidUsage.ParentUsage    = theParentUsage;
        aCSolidDef.Usages.Append(aCSolidUsage.UsageId);

        const BRepGraph_UsageId aCSolidUsageId   = aCSolidUsage.UsageId;
        const TopLoc_Location   aCSolidGlobalLoc = aCSolidUsage.GlobalLocation;

        for (TopoDS_Iterator aChildIt(aCompSolid); aChildIt.More(); aChildIt.Next())
        {
          if (aChildIt.Value().ShapeType() != TopAbs_SOLID)
            continue;
          BRepGraph_UsageId aChildUsage = traverseShape(
            aChildIt.Value(), aCSolidUsageId, aCSolidGlobalLoc);
          if (aChildUsage.IsValid())
          {
            theGraph.myData->myCompSolidUsages.ChangeValue(aCSolidUsageIdx).SolidUsages.Append(aChildUsage);
            theGraph.myData->myCompSolidDefs.ChangeValue(aCSolidDefIdx).SolidDefIds.Append(
              theGraph.DefOf(aChildUsage));
          }
        }

        return aCSolidUsageId;
      }

      case TopAbs_SOLID:
      {
        const TopoDS_Solid& aSolid = TopoDS::Solid(theCurrentShape);

        BRepGraph_TopoNode::SolidDef& aSolidDef = theGraph.myData->mySolidDefs.Appended();
        const int aSolidDefIdx = theGraph.myData->mySolidDefs.Length() - 1;
        aSolidDef.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, aSolidDefIdx);
        theGraph.allocateUID(aSolidDef.Id);

        BRepGraph_TopoNode::SolidUsage& aSolidUsage = theGraph.myData->mySolidUsages.Appended();
        const int aSolidUsageIdx = theGraph.myData->mySolidUsages.Length() - 1;
        aSolidUsage.UsageId        = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Solid, aSolidUsageIdx);
        aSolidUsage.DefId          = aSolidDef.Id;
        aSolidUsage.LocalLocation  = aSolid.Location();
        aSolidUsage.GlobalLocation = theParentGlobalLoc * aSolid.Location();
        aSolidUsage.Orientation    = aSolid.Orientation();
        aSolidUsage.ParentUsage    = theParentUsage;
        aSolidDef.Usages.Append(aSolidUsage.UsageId);
        theGraph.myData->myOriginalShapes.Bind(aSolidDef.Id, aSolid);

        const BRepGraph_UsageId aSolidUsageId   = aSolidUsage.UsageId;
        const TopLoc_Location   aSolidGlobalLoc = aSolidUsage.GlobalLocation;

        for (TopoDS_Iterator aShellIt(aSolid); aShellIt.More(); aShellIt.Next())
        {
          if (aShellIt.Value().ShapeType() != TopAbs_SHELL)
            continue;
          BRepGraph_UsageId aShellUsageId = traverseShape(
            aShellIt.Value(), aSolidUsageId, aSolidGlobalLoc);
          if (aShellUsageId.IsValid())
            theGraph.myData->mySolidUsages.ChangeValue(aSolidUsageIdx).ShellUsages.Append(aShellUsageId);
        }

        return aSolidUsageId;
      }

      case TopAbs_SHELL:
      {
        const TopoDS_Shell& aShell = TopoDS::Shell(theCurrentShape);

        BRepGraph_TopoNode::ShellDef& aShellDef = theGraph.myData->myShellDefs.Appended();
        const int aShellDefIdx = theGraph.myData->myShellDefs.Length() - 1;
        aShellDef.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Shell, aShellDefIdx);
        theGraph.allocateUID(aShellDef.Id);

        BRepGraph_TopoNode::ShellUsage& aShellUsage = theGraph.myData->myShellUsages.Appended();
        const int aShellUsageIdx = theGraph.myData->myShellUsages.Length() - 1;
        aShellUsage.UsageId        = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Shell, aShellUsageIdx);
        aShellUsage.DefId          = aShellDef.Id;
        aShellUsage.LocalLocation  = aShell.Location();
        aShellUsage.GlobalLocation = theParentGlobalLoc * aShell.Location();
        aShellUsage.Orientation    = aShell.Orientation();
        aShellUsage.ParentUsage    = theParentUsage;
        aShellDef.Usages.Append(aShellUsage.UsageId);
        theGraph.myData->myOriginalShapes.Bind(aShellDef.Id, aShell);

        for (TopoDS_Iterator aFaceIt(aShell); aFaceIt.More(); aFaceIt.Next())
        {
          if (aFaceIt.Value().ShapeType() != TopAbs_FACE)
            continue;
          FaceLocalData& aData   = aFaceData.Appended();
          aData.Face             = TopoDS::Face(aFaceIt.Value());
          aData.ParentGlobalLoc  = aShellUsage.GlobalLocation;
          aData.ParentShellUsage = aShellUsage.UsageId;
        }

        return aShellUsage.UsageId;
      }

      case TopAbs_FACE:
      {
        FaceLocalData& aData  = aFaceData.Appended();
        aData.Face            = TopoDS::Face(theCurrentShape);
        aData.ParentGlobalLoc = theParentGlobalLoc;
        return BRepGraph_UsageId(); // FaceUsage created in Phase 3
      }

      default:
        return BRepGraph_UsageId(); // Ignore Wire/Edge/Vertex at top level
    }
  };

  // Start traversal from the root shape.
  traverseShape(theShape, BRepGraph_UsageId(), TopLoc_Location());

  // Phase 2 (parallel): Extract per-face geometry/topology data.
  OSD_Parallel::For(
    0,
    aFaceData.Length(),
    [&](int theIndex) { extractFaceData(aFaceData.ChangeValue(theIndex)); },
    !theParallel);

  // Pre-size maps.
  const int aNbFacesEst   = aFaceData.Length();
  const int anEstEntities = aNbFacesEst * 10;
  theGraph.myData->myTShapeToDefId.ReSize(anEstEntities);
  if (theGraph.myData->myUIDEnabled)
  {
    theGraph.myData->myNodeToUID.ReSize(anEstEntities);
    theGraph.myData->myUIDToNodeId.ReSize(anEstEntities);
  }

  // Phase 3 (sequential): Register definitions and usages from pre-extracted data.
  registerFaceData(theGraph, aFaceData);

  // Phase 4: Set IsMultiLocated flags.
  computeMultiLocatedFlags(theGraph);

  theGraph.myData->myIsDone = true;
}

//=================================================================================================

void BRepGraph_Builder::Append(BRepGraph&          theGraph,
                               const TopoDS_Shape& theShape,
                               bool                theParallel)
{
  if (theShape.IsNull())
    return;

  // Phase 1 (sequential): Collect face contexts using simplified traversal.
  // Note: Append() intentionally flattens hierarchy to face level only —
  // no Solid/Shell/Compound defs are created. This is by design for
  // incremental face-level operations (e.g. sewing).
  NCollection_Vector<FaceLocalData> aFaceData(128, theGraph.myData->myAllocator);

  std::function<void(const TopoDS_Shape&, BRepGraph_UsageId, const TopLoc_Location&)>
    traverseShape;

  traverseShape = [&](const TopoDS_Shape&    theCurrentShape,
                      BRepGraph_UsageId      theParentUsage,
                      const TopLoc_Location& theParentGlobalLoc)
  {
    if (theCurrentShape.IsNull())
      return;

    switch (theCurrentShape.ShapeType())
    {
      case TopAbs_COMPOUND:
      case TopAbs_COMPSOLID:
      case TopAbs_SOLID:
      case TopAbs_SHELL:
      {
        for (TopoDS_Iterator aChildIt(theCurrentShape); aChildIt.More(); aChildIt.Next())
        {
          traverseShape(aChildIt.Value(), theParentUsage,
                        theParentGlobalLoc * theCurrentShape.Location());
        }
        break;
      }
      case TopAbs_FACE:
      {
        FaceLocalData& aData  = aFaceData.Appended();
        aData.Face            = TopoDS::Face(theCurrentShape);
        aData.ParentGlobalLoc = theParentGlobalLoc;
        break;
      }
      default:
        break;
    }
  };

  traverseShape(theShape, BRepGraph_UsageId(), TopLoc_Location());

  // Phase 2 (parallel): Extract per-face geometry/topology data.
  OSD_Parallel::For(
    0,
    aFaceData.Length(),
    [&](int theIndex) { extractFaceData(aFaceData.ChangeValue(theIndex)); },
    !theParallel);

  // Phase 3 (sequential): Register definitions and usages (reuses existing dedup maps).
  registerFaceData(theGraph, aFaceData);

  theGraph.myData->myIsDone = true;
}
