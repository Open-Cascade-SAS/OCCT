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

#include <BRepGraph_BackRefManager.hxx>
#include <BRepGraph_Builder.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraphInc_Populate.hxx>

#include <BRep_CurveOn2Surfaces.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <BRep_PointOnCurve.hxx>
#include <BRep_PointOnCurveOnSurface.hxx>
#include <BRep_PointOnSurface.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <BRep_TVertex.hxx>
#include <BRepTools.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <OSD_Parallel.hxx>
#include <Poly_Polygon2D.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
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
  TopLoc_Location      CurveLocation;
  double               ParamFirst    = 0.0;
  double               ParamLast     = 0.0;
  double               Tolerance     = 0.0;
  bool                 IsDegenerate  = false;
  bool                 SameParameter = false;
  bool                 SameRange     = false;
  ExtractedVertex      StartVertex;
  ExtractedVertex      EndVertex;
  TopAbs_Orientation   OrientationInWire = TopAbs_FORWARD;
  Handle(Geom2d_Curve) PCurve2d;
  double               PCFirst = 0.0;
  double               PCLast  = 0.0;
  Handle(Geom2d_Curve) PCurve2dReversed; //!< Second PCurve for seam edges
  double               PCFirstReversed  = 0.0;
  double               PCLastReversed   = 0.0;
  GeomAbs_Shape        PCurveContinuity = GeomAbs_C0;
  gp_Pnt2d             PCUV1;
  gp_Pnt2d             PCUV2;

  //! Polygon3D (one per edge, not per face).
  Handle(Poly_Polygon3D) Polygon3D;

  //! PolygonOnSurface entries.
  Handle(Poly_Polygon2D) PolyOnSurf;
  Handle(Poly_Polygon2D) PolyOnSurfReversed; //!< For seam edges
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
  TopoDS_Face       Face;
  TopLoc_Location   ParentGlobalLoc;
  BRepGraph_UsageId ParentShellUsage;

  // Geometry extracted in Phase 2 (parallel).
  Handle(Geom_Surface)              Surface;
  TopLoc_Location                   SurfaceLocation;
  NCollection_Vector<Handle(Poly_Triangulation)> Triangulations;
  int                               ActiveTriangulationIndex = -1;
  double                            Tolerance          = 0.0;
  TopAbs_Orientation                Orientation        = TopAbs_FORWARD;
  bool                              NaturalRestriction = false;
  NCollection_Vector<ExtractedWire> Wires;
};

//! Extract first and last vertices from an edge by iterating its direct children.
//! Similar to TopExp::Vertices() but additionally filters non-vertex children
//! and provides graceful fallback for INTERNAL/EXTERNAL oriented vertices.
//! cumOri=false matches TopExp::Vertices default: child orientations are NOT
//! composed with the edge's orientation, so FORWARD/REVERSED children are
//! returned as-is regardless of edge orientation.
void edgeVertices(const TopoDS_Edge& theEdge, TopoDS_Vertex& theFirst, TopoDS_Vertex& theLast)
{
  for (TopoDS_Iterator aVIt(theEdge, false); aVIt.More(); aVIt.Next())
  {
    if (aVIt.Value().ShapeType() != TopAbs_VERTEX)
    {
      continue;
    }

    const TopoDS_Vertex aVertex = TopoDS::Vertex(aVIt.Value());
    if (aVertex.Orientation() == TopAbs_FORWARD)
    {
      theFirst = aVertex;
    }
    else if (aVertex.Orientation() == TopAbs_REVERSED)
    {
      theLast = aVertex;
    }
    else if (theFirst.IsNull())
    {
      theFirst = aVertex;
    }
    else if (theLast.IsNull())
    {
      theLast = aVertex;
    }
  }

  if (theFirst.IsNull())
  {
    theFirst = theLast;
  }
  if (theLast.IsNull())
  {
    theLast = theFirst;
  }
}

//! Extract per-face geometry/topology data from TopoDS.
void extractFaceData(FaceLocalData& theData)
{
  const TopoDS_Face& aFace = theData.Face;

  theData.Surface = BRep_Tool::Surface(aFace, theData.SurfaceLocation);

  // Extract all triangulations.
  {
    TopLoc_Location aTriLoc;
    const NCollection_List<Handle(Poly_Triangulation)>& aTriList =
      BRep_Tool::Triangulations(aFace, aTriLoc);
    Handle(Poly_Triangulation) anActiveTri;
    {
      TopLoc_Location aDummyLoc;
      anActiveTri = BRep_Tool::Triangulation(aFace, aDummyLoc);
    }
    int aTriIdx = 0;
    for (NCollection_List<Handle(Poly_Triangulation)>::Iterator aTriIt(aTriList);
         aTriIt.More(); aTriIt.Next(), ++aTriIdx)
    {
      theData.Triangulations.Append(aTriIt.Value());
      if (!anActiveTri.IsNull() && aTriIt.Value() == anActiveTri)
        theData.ActiveTriangulationIndex = aTriIdx;
    }
  }

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
      anEdgeData.Curve3d    = BRep_Tool::Curve(anEdge, anEdgeData.CurveLocation, aFirst, aLast);
      anEdgeData.ParamFirst = aFirst;
      anEdgeData.ParamLast  = aLast;

      TopoDS_Vertex aVFirst, aVLast;
      edgeVertices(anEdge, aVFirst, aVLast);

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
      anEdgeData.PCurveContinuity = BRep_Tool::MaxContinuity(anEdge);

      // Extract UV points if PCurve exists.
      // UVPoints outputs default gp_Pnt2d on missing data, which is acceptable.
      if (!anEdgeData.PCurve2d.IsNull() && !theData.Surface.IsNull())
      {
        BRep_Tool::UVPoints(anEdge, aForwardFace, anEdgeData.PCUV1, anEdgeData.PCUV2);
      }

      // Extract Polygon3D (shared across all faces, only need once).
      {
        TopLoc_Location aPoly3DLoc;
        anEdgeData.Polygon3D = BRep_Tool::Polygon3D(anEdge, aPoly3DLoc);
      }

      // Extract PolygonOnSurface.
      {
        TopLoc_Location aPolyLoc;
        Handle(Poly_Polygon2D) aPolyOnSurf =
          BRep_Tool::PolygonOnSurface(anEdge, aForwardFace);
        anEdgeData.PolyOnSurf = aPolyOnSurf;
        // Check for seam: reversed orientation polygon.
        if (!aPolyOnSurf.IsNull())
        {
          TopoDS_Edge aReversedEdge2 = TopoDS::Edge(anEdge.Reversed());
          Handle(Poly_Polygon2D) aPolyOnSurfRev =
            BRep_Tool::PolygonOnSurface(aReversedEdge2, aForwardFace);
          if (!aPolyOnSurfRev.IsNull() && aPolyOnSurfRev != aPolyOnSurf)
            anEdgeData.PolyOnSurfReversed = aPolyOnSurfRev;
        }
      }

      // Extract second PCurve for seam edges (reversed orientation).
      if (!anEdgeData.PCurve2d.IsNull())
      {
        double               aPCFirstRev = 0.0, aPCLastRev = 0.0;
        TopoDS_Edge          aReversedEdge = TopoDS::Edge(anEdge.Reversed());
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
void BRepGraph_Builder::registerFaceData(BRepGraph& theGraph, const FaceDataVec& theFaceData)
{
  for (int aFaceDataIdx = 0; aFaceDataIdx < theFaceData.Length(); ++aFaceDataIdx)
  {
    const FaceLocalData& aData    = theFaceData.Value(aFaceDataIdx);
    const TopoDS_Face&   aCurFace = aData.Face;

    // Create or reuse FaceDef.
    const TopoDS_TShape*    aFaceTShapeKey = aCurFace.TShape().get();
    const BRepGraph_NodeId* anExistingFaceDef =
      theGraph.myData->myTShapeToDefId.Seek(aFaceTShapeKey);

    BRepGraph_NodeId aFaceDefId;
    if (anExistingFaceDef != nullptr)
    {
      aFaceDefId = *anExistingFaceDef;
    }
    else
    {
      BRepGraph_TopoNode::FaceDef& aFaceDef    = theGraph.myData->myFaces.Defs.Appended();
      const int                    aFaceDefIdx = theGraph.myData->myFaces.Defs.Length() - 1;
      aFaceDef.Id                 = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, aFaceDefIdx);
      aFaceDef.Tolerance          = aData.Tolerance;
      aFaceDef.NaturalRestriction = aData.NaturalRestriction;
      theGraph.allocateUID(aFaceDef.Id);

      aFaceDef.Surface = aData.Surface;

      // Store triangulations on the FaceDef.
      for (int aTriIdx = 0; aTriIdx < aData.Triangulations.Length(); ++aTriIdx)
        aFaceDef.Triangulations.Append(aData.Triangulations.Value(aTriIdx));
      aFaceDef.ActiveTriangulationIndex = aData.ActiveTriangulationIndex;

      theGraph.myData->myTShapeToDefId.Bind(aFaceTShapeKey, aFaceDef.Id);
      theGraph.myData->myOriginalShapes.Bind(aFaceDef.Id, aCurFace);
      aFaceDefId = aFaceDef.Id;
    }

    // Always create FaceUsage.
    BRepGraph_TopoNode::FaceUsage& aFaceUsage    = theGraph.myData->myFaces.Usages.Appended();
    const int                      aFaceUsageIdx = theGraph.myData->myFaces.Usages.Length() - 1;
    aFaceUsage.UsageId        = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Face, aFaceUsageIdx);
    aFaceUsage.DefId          = aFaceDefId;
    aFaceUsage.LocalLocation  = aCurFace.Location();
    aFaceUsage.GlobalLocation = aData.ParentGlobalLoc * aCurFace.Location();
    aFaceUsage.Orientation    = aData.Orientation;
    aFaceUsage.ParentUsage    = aData.ParentShellUsage;
    theGraph.myData->myFaces.Defs.ChangeValue(aFaceDefId.Index).Usages.Append(aFaceUsage.UsageId);

    // Link to parent shell usage.
    if (aData.ParentShellUsage.IsValid())
    {
      theGraph.myData->myShells.Usages.ChangeValue(aData.ParentShellUsage.Index)
        .FaceUsages.Append(aFaceUsage.UsageId);
    }

    // Process wires.
    for (int aWireIter = 0; aWireIter < aData.Wires.Length(); ++aWireIter)
    {
      const ExtractedWire& aWireData = aData.Wires.Value(aWireIter);

      // Dedup wire definition by TShape.
      const TopoDS_TShape*    aWireTShapeKey = aWireData.Shape.TShape().get();
      const BRepGraph_NodeId* anExistingWireDef =
        theGraph.myData->myTShapeToDefId.Seek(aWireTShapeKey);

      BRepGraph_NodeId aWireDefId;
      int              aWireDefIdx   = -1;
      bool             aIsNewWireDef = false;

      if (anExistingWireDef != nullptr)
      {
        aWireDefId  = *anExistingWireDef;
        aWireDefIdx = aWireDefId.Index;
      }
      else
      {
        BRepGraph_TopoNode::WireDef& aWireDef = theGraph.myData->myWires.Defs.Appended();
        aWireDefIdx                           = theGraph.myData->myWires.Defs.Length() - 1;
        aWireDef.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Wire, aWireDefIdx);
        theGraph.allocateUID(aWireDef.Id);
        theGraph.myData->myOriginalShapes.Bind(aWireDef.Id, aWireData.Shape);
        theGraph.myData->myTShapeToDefId.Bind(aWireTShapeKey, aWireDef.Id);
        aWireDefId    = aWireDef.Id;
        aIsNewWireDef = true;
      }

      BRepGraph_TopoNode::WireUsage& aWireUsage    = theGraph.myData->myWires.Usages.Appended();
      const int                      aWireUsageIdx = theGraph.myData->myWires.Usages.Length() - 1;
      aWireUsage.UsageId        = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Wire, aWireUsageIdx);
      aWireUsage.DefId          = aWireDefId;
      aWireUsage.LocalLocation  = aWireData.Shape.Location();
      aWireUsage.GlobalLocation = aFaceUsage.GlobalLocation * aWireData.Shape.Location();
      aWireUsage.Orientation    = aWireData.Shape.Orientation();
      aWireUsage.ParentUsage    = aFaceUsage.UsageId;
      aWireUsage.OwnerFaceUsage = aFaceUsage.UsageId;
      theGraph.myData->myWires.Defs.ChangeValue(aWireDefIdx).Usages.Append(aWireUsage.UsageId);

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
        const TopoDS_TShape*    aTShapeKey = anEdge.TShape().get();
        const BRepGraph_NodeId* anExistingEdgeDefId =
          theGraph.myData->myTShapeToDefId.Seek(aTShapeKey);

        BRepGraph_NodeId anEdgeDefId;

        if (anExistingEdgeDefId != nullptr)
        {
          anEdgeDefId = *anExistingEdgeDefId;
        }
        else
        {
          BRepGraph_TopoNode::EdgeDef& anEdgeDef    = theGraph.myData->myEdges.Defs.Appended();
          const int                    anEdgeDefIdx = theGraph.myData->myEdges.Defs.Length() - 1;
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
            anEdgeDef.Curve3d = anEdgeData.Curve3d;
          }

          // Vertex definitions.
          auto processVertexDef = [&](const ExtractedVertex& theVtxData) -> BRepGraph_NodeId {
            if (theVtxData.Shape.IsNull())
              return BRepGraph_NodeId();

            const TopoDS_TShape*    aVTShapeKey = theVtxData.Shape.TShape().get();
            const BRepGraph_NodeId* anExistingVtx =
              theGraph.myData->myTShapeToDefId.Seek(aVTShapeKey);
            if (anExistingVtx != nullptr)
              return *anExistingVtx;

            BRepGraph_TopoNode::VertexDef& aVtxDef    = theGraph.myData->myVertices.Defs.Appended();
            const int                      aVtxDefIdx = theGraph.myData->myVertices.Defs.Length() - 1;
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
        BRepGraph_TopoNode::EdgeUsage& anEdgeUsage    = theGraph.myData->myEdges.Usages.Appended();
        const int                      anEdgeUsageIdx = theGraph.myData->myEdges.Usages.Length() - 1;
        anEdgeUsage.UsageId       = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Edge, anEdgeUsageIdx);
        anEdgeUsage.DefId         = anEdgeDefId;
        anEdgeUsage.LocalLocation = anEdge.Location();
        anEdgeUsage.GlobalLocation = aWireUsage.GlobalLocation * anEdge.Location();
        anEdgeUsage.Orientation    = anEdgeData.OrientationInWire;
        anEdgeUsage.ParentUsage    = aWireUsage.UsageId;
        theGraph.myData->myEdges.Defs.ChangeValue(anEdgeDefId.Index)
          .Usages.Append(anEdgeUsage.UsageId);

        // Create VertexUsages for this edge usage.
        auto createVertexUsage = [&](BRepGraph_NodeId       theVtxDefId,
                                     const ExtractedVertex& theVtxData,
                                     const TopLoc_Location& theEdgeGlobalLoc) -> BRepGraph_UsageId {
          if (!theVtxDefId.IsValid())
            return BRepGraph_UsageId();

          BRepGraph_TopoNode::VertexUsage& aVtxUsage = theGraph.myData->myVertices.Usages.Appended();
          const int aVtxUsageIdx                     = theGraph.myData->myVertices.Usages.Length() - 1;
          aVtxUsage.UsageId       = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Vertex, aVtxUsageIdx);
          aVtxUsage.DefId         = theVtxDefId;
          aVtxUsage.LocalLocation = theVtxData.Shape.Location();
          aVtxUsage.GlobalLocation   = theEdgeGlobalLoc * theVtxData.Shape.Location();
          aVtxUsage.Orientation      = theVtxData.Shape.Orientation();
          aVtxUsage.ParentUsage      = anEdgeUsage.UsageId;
          aVtxUsage.TransformedPoint = theVtxData.Point;
          theGraph.myData->myVertices.Defs.ChangeValue(theVtxDefId.Index)
            .Usages.Append(aVtxUsage.UsageId);
          return aVtxUsage.UsageId;
        };

        const BRepGraph_TopoNode::EdgeDef& anEdgeDefRef =
          theGraph.myData->myEdges.Defs.Value(anEdgeDefId.Index);
        anEdgeUsage.StartVertexUsage = createVertexUsage(anEdgeDefRef.StartVertexDefId,
                                                         anEdgeData.StartVertex,
                                                         anEdgeUsage.GlobalLocation);
        anEdgeUsage.EndVertexUsage   = createVertexUsage(anEdgeDefRef.EndVertexDefId,
                                                       anEdgeData.EndVertex,
                                                       anEdgeUsage.GlobalLocation);

        aWireUsage.EdgeUsages.Append(anEdgeUsage.UsageId);

        // PCurve from pre-extracted data.
        if (!anEdgeData.PCurve2d.IsNull())
        {
          BRepGraph_TopoNode::EdgeDef::PCurveEntry aPCEntry;
          aPCEntry.Curve2d         = anEdgeData.PCurve2d;
          aPCEntry.FaceDefId       = aFaceDefId;
          aPCEntry.ParamFirst      = anEdgeData.PCFirst;
          aPCEntry.ParamLast       = anEdgeData.PCLast;
          aPCEntry.Continuity      = anEdgeData.PCurveContinuity;
          aPCEntry.UV1             = anEdgeData.PCUV1;
          aPCEntry.UV2             = anEdgeData.PCUV2;
          aPCEntry.EdgeOrientation = TopAbs_FORWARD;
          theGraph.myData->myEdges.Defs.ChangeValue(anEdgeDefId.Index).PCurves.Append(aPCEntry);
        }

        // Second PCurve for seam edges.
        if (!anEdgeData.PCurve2dReversed.IsNull())
        {
          BRepGraph_TopoNode::EdgeDef::PCurveEntry aPCEntry2;
          aPCEntry2.Curve2d         = anEdgeData.PCurve2dReversed;
          aPCEntry2.FaceDefId       = aFaceDefId;
          aPCEntry2.ParamFirst      = anEdgeData.PCFirstReversed;
          aPCEntry2.ParamLast       = anEdgeData.PCLastReversed;
          aPCEntry2.Continuity      = anEdgeData.PCurveContinuity;
          aPCEntry2.EdgeOrientation = TopAbs_REVERSED;
          theGraph.myData->myEdges.Defs.ChangeValue(anEdgeDefId.Index).PCurves.Append(aPCEntry2);
        }

        // Polygon3D (store inline on edge, once per edge).
        if (!anEdgeData.Polygon3D.IsNull())
        {
          BRepGraph_TopoNode::EdgeDef& anEdgeDefMut =
            theGraph.myData->myEdges.Defs.ChangeValue(anEdgeDefId.Index);
          if (anEdgeDefMut.Polygon3D.IsNull())
          {
            anEdgeDefMut.Polygon3D = anEdgeData.Polygon3D;
          }
        }

        // PolygonOnSurface (per face context, stored inline).
        if (!anEdgeData.PolyOnSurf.IsNull())
        {
          BRepGraph_TopoNode::EdgeDef::PolyOnSurfEntry aPolyEntry;
          aPolyEntry.Polygon2D       = anEdgeData.PolyOnSurf;
          aPolyEntry.FaceDefId       = aFaceDefId;
          aPolyEntry.EdgeOrientation = TopAbs_FORWARD;
          theGraph.myData->myEdges.Defs.ChangeValue(anEdgeDefId.Index)
            .PolygonsOnSurf.Append(aPolyEntry);
        }
        if (!anEdgeData.PolyOnSurfReversed.IsNull())
        {
          BRepGraph_TopoNode::EdgeDef::PolyOnSurfEntry aPolyEntry;
          aPolyEntry.Polygon2D       = anEdgeData.PolyOnSurfReversed;
          aPolyEntry.FaceDefId       = aFaceDefId;
          aPolyEntry.EdgeOrientation = TopAbs_REVERSED;
          theGraph.myData->myEdges.Defs.ChangeValue(anEdgeDefId.Index)
            .PolygonsOnSurf.Append(aPolyEntry);
        }

        // PolygonOnTriangulation (per face triangulation context, stored inline).
        {
          const BRepGraph_TopoNode::FaceDef& aFaceDefRef =
            theGraph.myData->myFaces.Defs.Value(aFaceDefId.Index);
          for (int aTriIdx = 0; aTriIdx < aFaceDefRef.Triangulations.Length(); ++aTriIdx)
          {
            const Handle(Poly_Triangulation)& aTri = aFaceDefRef.Triangulations.Value(aTriIdx);
            if (aTri.IsNull())
              continue;

            TopLoc_Location aPolyTriLoc;
            Handle(Poly_PolygonOnTriangulation) aPolyOnTri =
              BRep_Tool::PolygonOnTriangulation(anEdgeData.Shape, aTri, aPolyTriLoc);
            if (!aPolyOnTri.IsNull())
            {
              BRepGraph_TopoNode::EdgeDef::PolyOnTriEntry aPolyTriEntry;
              aPolyTriEntry.Polygon            = aPolyOnTri;
              aPolyTriEntry.FaceDefId          = aFaceDefId;
              aPolyTriEntry.TriangulationIndex = aTriIdx;
              aPolyTriEntry.EdgeOrientation    = TopAbs_FORWARD;
              theGraph.myData->myEdges.Defs.ChangeValue(anEdgeDefId.Index)
                .PolygonsOnTri.Append(aPolyTriEntry);

              // Check for closed polygon on triangulation (seam edge).
              TopoDS_Edge aRevEdge = TopoDS::Edge(anEdgeData.Shape.Reversed());
              Handle(Poly_PolygonOnTriangulation) aPolyOnTriRev =
                BRep_Tool::PolygonOnTriangulation(aRevEdge, aTri, aPolyTriLoc);
              if (!aPolyOnTriRev.IsNull() && aPolyOnTriRev != aPolyOnTri)
              {
                BRepGraph_TopoNode::EdgeDef::PolyOnTriEntry aPolyTriEntryRev;
                aPolyTriEntryRev.Polygon            = aPolyOnTriRev;
                aPolyTriEntryRev.FaceDefId          = aFaceDefId;
                aPolyTriEntryRev.TriangulationIndex = aTriIdx;
                aPolyTriEntryRev.EdgeOrientation    = TopAbs_REVERSED;
                theGraph.myData->myEdges.Defs.ChangeValue(anEdgeDefId.Index)
                  .PolygonsOnTri.Append(aPolyTriEntryRev);
              }
            }
          }
        }

        if (aIsNewWireDef)
        {
          // Populate edge-to-wire reverse index.
          BRepGraph_BackRefManager::BindEdgeToWire(theGraph, anEdgeDefId.Index, aWireDefIdx);

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
        // Set wire closure on the WireUsage.
        theGraph.myData->myWires.Usages.ChangeValue(aWireUsageIdx).IsClosed =
          aFirstVertexDefId.IsValid() && aLastVertexDefId.IsValid()
          && aFirstVertexDefId == aLastVertexDefId;
      }
    }
  }
}

//=================================================================================================

void BRepGraph_Builder::Perform(BRepGraph& theGraph, const TopoDS_Shape& theShape, bool theParallel)
{
  // Phase 0: Clear all storage, increment generation.
  // Clear all back-reference containers first (rel-edges, edge-to-wires,
  // geometry back-refs) via centralized BackRefManager.
  BRepGraph_BackRefManager::ClearAll(theGraph);

  theGraph.myData->mySolids.Clear();
  theGraph.myData->myShells.Clear();
  theGraph.myData->myFaces.Clear();
  theGraph.myData->myWires.Clear();
  theGraph.myData->myEdges.Clear();
  theGraph.myData->myVertices.Clear();
  theGraph.myData->myCompounds.Clear();
  theGraph.myData->myCompSolids.Clear();
  theGraph.myData->myTShapeToDefId.Clear();
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
                      const TopLoc_Location& theParentGlobalLoc) -> BRepGraph_UsageId {
    if (theCurrentShape.IsNull())
      return BRepGraph_UsageId();

    switch (theCurrentShape.ShapeType())
    {
      case TopAbs_COMPOUND: {
        const TopoDS_Compound& aCompound = TopoDS::Compound(theCurrentShape);

        BRepGraph_TopoNode::CompoundDef& aCompDef    = theGraph.myData->myCompounds.Defs.Appended();
        const int                        aCompDefIdx = theGraph.myData->myCompounds.Defs.Length() - 1;
        aCompDef.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Compound, aCompDefIdx);
        theGraph.allocateUID(aCompDef.Id);
        theGraph.myData->myOriginalShapes.Bind(aCompDef.Id, aCompound);

        BRepGraph_TopoNode::CompoundUsage& aCompUsage =
          theGraph.myData->myCompounds.Usages.Appended();
        const int aCompUsageIdx = theGraph.myData->myCompounds.Usages.Length() - 1;
        aCompUsage.UsageId = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Compound, aCompUsageIdx);
        aCompUsage.DefId   = aCompDef.Id;
        aCompUsage.LocalLocation  = aCompound.Location();
        aCompUsage.GlobalLocation = theParentGlobalLoc * aCompound.Location();
        aCompUsage.Orientation    = aCompound.Orientation();
        aCompUsage.ParentUsage    = theParentUsage;
        aCompDef.Usages.Append(aCompUsage.UsageId);

        const BRepGraph_UsageId aCompUsageId   = aCompUsage.UsageId;
        const TopLoc_Location   aCompGlobalLoc = aCompUsage.GlobalLocation;

        for (TopoDS_Iterator aChildIt(aCompound); aChildIt.More(); aChildIt.Next())
        {
          BRepGraph_UsageId aChildUsage =
            traverseShape(aChildIt.Value(), aCompUsageId, aCompGlobalLoc);
          if (aChildUsage.IsValid())
          {
            theGraph.myData->myCompounds.Usages.ChangeValue(aCompUsageIdx)
              .ChildUsages.Append(aChildUsage);
            theGraph.myData->myCompounds.Defs.ChangeValue(aCompDefIdx)
              .ChildDefIds.Append(theGraph.DefOf(aChildUsage));
          }
        }

        return aCompUsageId;
      }

      case TopAbs_COMPSOLID: {
        const TopoDS_CompSolid& aCompSolid = TopoDS::CompSolid(theCurrentShape);

        BRepGraph_TopoNode::CompSolidDef& aCSolidDef = theGraph.myData->myCompSolids.Defs.Appended();
        const int aCSolidDefIdx = theGraph.myData->myCompSolids.Defs.Length() - 1;
        aCSolidDef.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::CompSolid, aCSolidDefIdx);
        theGraph.allocateUID(aCSolidDef.Id);
        theGraph.myData->myOriginalShapes.Bind(aCSolidDef.Id, aCompSolid);

        BRepGraph_TopoNode::CompSolidUsage& aCSolidUsage =
          theGraph.myData->myCompSolids.Usages.Appended();
        const int aCSolidUsageIdx = theGraph.myData->myCompSolids.Usages.Length() - 1;
        aCSolidUsage.UsageId =
          BRepGraph_UsageId(BRepGraph_NodeId::Kind::CompSolid, aCSolidUsageIdx);
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
          BRepGraph_UsageId aChildUsage =
            traverseShape(aChildIt.Value(), aCSolidUsageId, aCSolidGlobalLoc);
          if (aChildUsage.IsValid())
          {
            theGraph.myData->myCompSolids.Usages.ChangeValue(aCSolidUsageIdx)
              .SolidUsages.Append(aChildUsage);
            theGraph.myData->myCompSolids.Defs.ChangeValue(aCSolidDefIdx)
              .SolidDefIds.Append(theGraph.DefOf(aChildUsage));
          }
        }

        return aCSolidUsageId;
      }

      case TopAbs_SOLID: {
        const TopoDS_Solid& aSolid = TopoDS::Solid(theCurrentShape);

        BRepGraph_TopoNode::SolidDef& aSolidDef    = theGraph.myData->mySolids.Defs.Appended();
        const int                     aSolidDefIdx = theGraph.myData->mySolids.Defs.Length() - 1;
        aSolidDef.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, aSolidDefIdx);
        theGraph.allocateUID(aSolidDef.Id);

        BRepGraph_TopoNode::SolidUsage& aSolidUsage = theGraph.myData->mySolids.Usages.Appended();
        const int aSolidUsageIdx                    = theGraph.myData->mySolids.Usages.Length() - 1;
        aSolidUsage.UsageId = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Solid, aSolidUsageIdx);
        aSolidUsage.DefId   = aSolidDef.Id;
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
          BRepGraph_UsageId aShellUsageId =
            traverseShape(aShellIt.Value(), aSolidUsageId, aSolidGlobalLoc);
          if (aShellUsageId.IsValid())
            theGraph.myData->mySolids.Usages.ChangeValue(aSolidUsageIdx)
              .ShellUsages.Append(aShellUsageId);
        }

        return aSolidUsageId;
      }

      case TopAbs_SHELL: {
        const TopoDS_Shell& aShell = TopoDS::Shell(theCurrentShape);

        BRepGraph_TopoNode::ShellDef& aShellDef    = theGraph.myData->myShells.Defs.Appended();
        const int                     aShellDefIdx = theGraph.myData->myShells.Defs.Length() - 1;
        aShellDef.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Shell, aShellDefIdx);
        theGraph.allocateUID(aShellDef.Id);

        BRepGraph_TopoNode::ShellUsage& aShellUsage = theGraph.myData->myShells.Usages.Appended();
        const int aShellUsageIdx                    = theGraph.myData->myShells.Usages.Length() - 1;
        aShellUsage.UsageId = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Shell, aShellUsageIdx);
        aShellUsage.DefId   = aShellDef.Id;
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

      case TopAbs_FACE: {
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

  // Pre-size TShape dedup map.
  const int aNbFacesEst   = aFaceData.Length();
  const int anEstEntities = aNbFacesEst * 10;
  theGraph.myData->myTShapeToDefId.ReSize(anEstEntities);

  // Phase 3 (sequential): Register definitions and usages from pre-extracted data.
  registerFaceData(theGraph, aFaceData);

  // Phase 3b: Extract vertex point representations (post-pass, needs def IDs).
  // Build reverse-lookup maps: geometry pointer -> owning def NodeId.
  NCollection_DataMap<const Geom_Curve*, BRepGraph_NodeId> aCurveToEdgeDef;
  for (int i = 0; i < theGraph.myData->myEdges.Defs.Length(); ++i)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdge = theGraph.myData->myEdges.Defs.Value(i);
    if (!anEdge.Curve3d.IsNull())
      aCurveToEdgeDef.TryBind(anEdge.Curve3d.get(), anEdge.Id);
  }

  NCollection_DataMap<const Geom_Surface*, BRepGraph_NodeId> aSurfToFaceDef;
  for (int i = 0; i < theGraph.myData->myFaces.Defs.Length(); ++i)
  {
    const BRepGraph_TopoNode::FaceDef& aFace = theGraph.myData->myFaces.Defs.Value(i);
    if (!aFace.Surface.IsNull())
      aSurfToFaceDef.TryBind(aFace.Surface.get(), aFace.Id);
  }

  for (int aVtxIdx = 0; aVtxIdx < theGraph.myData->myVertices.Defs.Length(); ++aVtxIdx)
  {
    BRepGraph_TopoNode::VertexDef& aVtxDef = theGraph.myData->myVertices.Defs.ChangeValue(aVtxIdx);
    const TopoDS_Shape* aVtxShape = theGraph.myData->myOriginalShapes.Seek(aVtxDef.Id);
    if (aVtxShape == nullptr || aVtxShape->IsNull())
      continue;
    const TopoDS_Vertex& aVertex = TopoDS::Vertex(*aVtxShape);
    const Handle(BRep_TVertex)& aTVertex =
      Handle(BRep_TVertex)::DownCast(aVertex.TShape());
    if (aTVertex.IsNull())
      continue;

    for (const Handle(BRep_PointRepresentation)& aPtRep : aTVertex->Points())
    {
      if (aPtRep.IsNull())
        continue;
      if (const Handle(BRep_PointOnCurve) aPOC = Handle(BRep_PointOnCurve)::DownCast(aPtRep))
      {
        const BRepGraph_NodeId* anEdgeId = aCurveToEdgeDef.Seek(aPOC->Curve().get());
        if (anEdgeId == nullptr)
          continue;
        BRepGraph_TopoNode::VertexDef::PointOnCurveEntry anEntry;
        anEntry.Parameter = aPOC->Parameter();
        anEntry.EdgeDefId = *anEdgeId;
        aVtxDef.PointsOnCurve.Append(anEntry);
      }
      else if (const Handle(BRep_PointOnCurveOnSurface) aPOCS =
                 Handle(BRep_PointOnCurveOnSurface)::DownCast(aPtRep))
      {
        const BRepGraph_NodeId* aFaceId = aSurfToFaceDef.Seek(aPOCS->Surface().get());
        if (aFaceId == nullptr)
          continue;
        BRepGraph_TopoNode::VertexDef::PointOnPCurveEntry anEntry;
        anEntry.Parameter = aPOCS->Parameter();
        anEntry.FaceDefId = *aFaceId;
        aVtxDef.PointsOnPCurve.Append(anEntry);
      }
      else if (const Handle(BRep_PointOnSurface) aPOS =
                 Handle(BRep_PointOnSurface)::DownCast(aPtRep))
      {
        const BRepGraph_NodeId* aFaceId = aSurfToFaceDef.Seek(aPOS->Surface().get());
        if (aFaceId == nullptr)
          continue;
        BRepGraph_TopoNode::VertexDef::PointOnSurfaceEntry anEntry;
        anEntry.ParameterU = aPOS->Parameter();
        anEntry.ParameterV = aPOS->Parameter2();
        anEntry.FaceDefId  = *aFaceId;
        aVtxDef.PointsOnSurface.Append(anEntry);
      }
    }
  }

  // Phase 3c: Extract edge regularities (post-pass, needs face defs).
  for (int anEdgeIdx = 0; anEdgeIdx < theGraph.myData->myEdges.Defs.Length(); ++anEdgeIdx)
  {
    BRepGraph_TopoNode::EdgeDef& anEdgeDef = theGraph.myData->myEdges.Defs.ChangeValue(anEdgeIdx);
    const TopoDS_Shape* anEdgeShape = theGraph.myData->myOriginalShapes.Seek(anEdgeDef.Id);
    if (anEdgeShape == nullptr || anEdgeShape->IsNull())
      continue;
    const TopoDS_Edge& anEdge = TopoDS::Edge(*anEdgeShape);
    const Handle(BRep_TEdge)& aTEdge = Handle(BRep_TEdge)::DownCast(anEdge.TShape());
    if (aTEdge.IsNull())
      continue;

    for (const Handle(BRep_CurveRepresentation)& aCRep : aTEdge->Curves())
    {
      if (aCRep.IsNull())
        continue;
      const Handle(BRep_CurveOn2Surfaces) aCon2S =
        Handle(BRep_CurveOn2Surfaces)::DownCast(aCRep);
      if (aCon2S.IsNull())
        continue;

      const Geom_Surface* aSurf1Ptr = aCon2S->Surface().get();
      const Geom_Surface* aSurf2Ptr = aCon2S->Surface2().get();
      if (aSurf1Ptr == nullptr || aSurf2Ptr == nullptr)
        continue;

      // Find face defs that reference each surface by pointer comparison.
      auto findFacesForSurf = [&](const Geom_Surface* theSurfPtr)
        -> NCollection_Vector<BRepGraph_NodeId>
      {
        NCollection_Vector<BRepGraph_NodeId> aResult;
        for (int aFIdx = 0; aFIdx < theGraph.myData->myFaces.Defs.Length(); ++aFIdx)
        {
          const BRepGraph_TopoNode::FaceDef& aFDef = theGraph.myData->myFaces.Defs.Value(aFIdx);
          if (!aFDef.Surface.IsNull() && aFDef.Surface.get() == theSurfPtr)
            aResult.Append(aFDef.Id);
        }
        return aResult;
      };

      NCollection_Vector<BRepGraph_NodeId> aFacesForSurf1 = findFacesForSurf(aSurf1Ptr);
      NCollection_Vector<BRepGraph_NodeId> aFacesForSurf2 = findFacesForSurf(aSurf2Ptr);
      if (aFacesForSurf1.IsEmpty() || aFacesForSurf2.IsEmpty())
        continue;

      // Find adjacent face for each surface: the face that has a PCurve for this edge.
      auto findAdjacentFace = [&](const NCollection_Vector<BRepGraph_NodeId>& theFaceUsers)
        -> BRepGraph_NodeId
      {
        for (int aFIdx = 0; aFIdx < theFaceUsers.Length(); ++aFIdx)
        {
          const BRepGraph_NodeId& aFaceId = theFaceUsers.Value(aFIdx);
          for (int aPCIdx = 0; aPCIdx < anEdgeDef.PCurves.Length(); ++aPCIdx)
          {
            if (anEdgeDef.PCurves.Value(aPCIdx).FaceDefId == aFaceId)
              return aFaceId;
          }
        }
        // Fallback: return first user if no PCurve match found.
        return theFaceUsers.First();
      };

      BRepGraph_TopoNode::EdgeDef::RegularityEntry aRegEntry;
      aRegEntry.FaceDef1   = findAdjacentFace(aFacesForSurf1);
      aRegEntry.FaceDef2   = findAdjacentFace(aFacesForSurf2);
      aRegEntry.Continuity = aCon2S->Continuity();
      anEdgeDef.Regularities.Append(aRegEntry);
    }
  }

  theGraph.myData->myIsDone = true;

  // Dual-write: populate incidence-table storage from the same input shape.
  // During migration, the legacy Def/Usage storage remains authoritative.
  // The incidence storage is derived and used for verification only.
  // After Phase 5, incidence storage becomes the sole authoritative store.
  BRepGraphInc_Populate::Perform(theGraph.myData->myIncStorage, theShape, theParallel);
  if (!theGraph.myData->myIncStorage.IsDone)
  {
    // Incidence population failed — log but do not fail the legacy build.
    // This is a non-fatal condition during the dual-write migration phase.
    theGraph.myData->myIncStorage.Clear();
  }
}

//=================================================================================================

void BRepGraph_Builder::Append(BRepGraph& theGraph, const TopoDS_Shape& theShape, bool theParallel)
{
  if (theShape.IsNull())
    return;

  // Invalidate incidence storage — Append modifies legacy storage only.
  theGraph.myData->myIncStorage.IsDone = false;

  // Phase 1 (sequential): Collect face contexts using simplified traversal.
  // Note: Append() intentionally flattens hierarchy to face level only --
  // no Solid/Shell/Compound defs are created. This is by design for
  // incremental face-level operations (e.g. sewing).
  NCollection_Vector<FaceLocalData> aFaceData(128, theGraph.myData->myAllocator);

  std::function<void(const TopoDS_Shape&, BRepGraph_UsageId, const TopLoc_Location&)> traverseShape;

  traverseShape = [&](const TopoDS_Shape&    theCurrentShape,
                      BRepGraph_UsageId      theParentUsage,
                      const TopLoc_Location& theParentGlobalLoc) {
    if (theCurrentShape.IsNull())
      return;

    switch (theCurrentShape.ShapeType())
    {
      case TopAbs_COMPOUND:
      case TopAbs_COMPSOLID:
      case TopAbs_SOLID:
      case TopAbs_SHELL: {
        for (TopoDS_Iterator aChildIt(theCurrentShape); aChildIt.More(); aChildIt.Next())
        {
          traverseShape(aChildIt.Value(),
                        theParentUsage,
                        theParentGlobalLoc * theCurrentShape.Location());
        }
        break;
      }
      case TopAbs_FACE: {
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
