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

#include <BRepGraphInc_Populate.hxx>
#include <BRepGraphInc_Storage.hxx>

#include <BRep_CurveOn2Surfaces.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <BRep_PointOnCurve.hxx>
#include <BRep_PointOnCurveOnSurface.hxx>
#include <BRep_PointOnSurface.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_TVertex.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <OSD_Parallel.hxx>
#include <Poly_Polygon2D.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <TopAbs.hxx>
#include <TopoDS.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>

namespace
{

//! Per-vertex data extracted from TopoDS in parallel phase.
struct ExtractedVertex
{
  TopoDS_Vertex Shape;
  gp_Pnt        Point;
  double        Tolerance = 0.0;
};

//! Internal/external vertex extracted from an edge.
struct ExtractedInternalVertex
{
  TopoDS_Vertex      Shape;
  gp_Pnt             Point;
  double             Tolerance = 0.0;
  TopAbs_Orientation Orientation = TopAbs_INTERNAL;
};

//! Per-edge data extracted from TopoDS in parallel phase.
struct ExtractedEdge
{
  TopoDS_Edge          Shape;
  occ::handle<Geom_Curve>   Curve3d;
  double               ParamFirst    = 0.0;
  double               ParamLast     = 0.0;
  double               Tolerance     = 0.0;
  bool                 IsDegenerate  = false;
  bool                 SameParameter = false;
  bool                 SameRange     = false;
  ExtractedVertex      StartVertex;
  ExtractedVertex      EndVertex;
  NCollection_Vector<ExtractedInternalVertex> InternalVertices;
  TopAbs_Orientation   OrientationInWire = TopAbs_FORWARD;
  occ::handle<Geom2d_Curve> PCurve2d;
  double               PCFirst = 0.0;
  double               PCLast  = 0.0;
  occ::handle<Geom2d_Curve> PCurve2dReversed;
  double               PCFirstReversed  = 0.0;
  double               PCLastReversed   = 0.0;
  GeomAbs_Shape        PCurveContinuity = GeomAbs_C0;
  gp_Pnt2d             PCUV1;
  gp_Pnt2d             PCUV2;
  occ::handle<Poly_Polygon3D> Polygon3D;
  occ::handle<Poly_Polygon2D> PolyOnSurf;
  occ::handle<Poly_Polygon2D> PolyOnSurfReversed;
};

//! Per-wire data extracted in parallel phase.
struct ExtractedWire
{
  TopoDS_Wire                       Shape;
  bool                              IsOuter = false;
  NCollection_Vector<ExtractedEdge> Edges;
};

//! All data extracted from a single face.
struct FaceLocalData
{
  // Phase 1 context.
  TopoDS_Face     Face;
  TopLoc_Location ParentGlobalLoc;
  int             ParentShellIdx = -1;

  // Phase 2 extracted geometry.
  occ::handle<Geom_Surface> Surface;
  NCollection_Vector<occ::handle<Poly_Triangulation>> Triangulations;
  int                ActiveTriangulationIndex = -1;
  double             Tolerance          = 0.0;
  TopAbs_Orientation Orientation        = TopAbs_FORWARD;
  bool               NaturalRestriction = false;
  NCollection_Vector<ExtractedWire> Wires;
  NCollection_Vector<ExtractedInternalVertex> DirectVertices; //!< INTERNAL/EXTERNAL vertex children
};

//! Extract first, last, and internal/external vertices from an edge.
void edgeVertices(const TopoDS_Edge&                        theEdge,
                  TopoDS_Vertex&                            theFirst,
                  TopoDS_Vertex&                            theLast,
                  NCollection_Vector<ExtractedInternalVertex>& theInternal)
{
  for (TopoDS_Iterator aVIt(theEdge, false); aVIt.More(); aVIt.Next())
  {
    if (aVIt.Value().ShapeType() != TopAbs_VERTEX)
      continue;
    const TopoDS_Vertex aVertex = TopoDS::Vertex(aVIt.Value());
    if (aVertex.Orientation() == TopAbs_FORWARD)
      theFirst = aVertex;
    else if (aVertex.Orientation() == TopAbs_REVERSED)
      theLast = aVertex;
    else
    {
      ExtractedInternalVertex anIntVtx;
      anIntVtx.Shape       = aVertex;
      anIntVtx.Point       = BRep_Tool::Pnt(aVertex);
      anIntVtx.Tolerance   = BRep_Tool::Tolerance(aVertex);
      anIntVtx.Orientation = aVertex.Orientation();
      theInternal.Append(anIntVtx);
    }
  }
  if (theFirst.IsNull())
    theFirst = theLast;
  if (theLast.IsNull())
    theLast = theFirst;
}

//! Extract per-face geometry/topology data from TopoDS.
void extractFaceData(FaceLocalData& theData)
{
  const TopoDS_Face& aFace = theData.Face;

  // Extract surface with its representation location baked in.
  // BRep_Tool::Surface(face, loc) returns TFace->Surface() and sets
  // loc = face.Location() * TFace->Location()  (see BRep_TFace.hxx).
  // Factor out the topological location to isolate TFace->Location(),
  // then apply it to the surface so stored geometry is in the definition frame.
  TopLoc_Location aSurfCombinedLoc;
  theData.Surface = BRep_Tool::Surface(aFace, aSurfCombinedLoc);
  if (!aSurfCombinedLoc.IsIdentity() && !theData.Surface.IsNull())
  {
    const TopLoc_Location aSurfRepLoc = aFace.Location().Inverted() * aSurfCombinedLoc;
    if (!aSurfRepLoc.IsIdentity())
    {
      theData.Surface = occ::down_cast<Geom_Surface>(
        theData.Surface->Transformed(aSurfRepLoc.Transformation()));
    }
  }

  // Extract triangulations.
  {
    TopLoc_Location aTriLoc;
    const NCollection_List<occ::handle<Poly_Triangulation>>& aTriList =
      BRep_Tool::Triangulations(aFace, aTriLoc);
    occ::handle<Poly_Triangulation> anActiveTri;
    {
      TopLoc_Location aDummyLoc;
      anActiveTri = BRep_Tool::Triangulation(aFace, aDummyLoc);
    }
    int aTriIdx = 0;
    for (NCollection_List<occ::handle<Poly_Triangulation>>::Iterator aTriIt(aTriList);
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

  for (TopoDS_Iterator aChildIt(aForwardFace); aChildIt.More(); aChildIt.Next())
  {
    if (aChildIt.Value().ShapeType() == TopAbs_VERTEX)
    {
      const TopoDS_Vertex& aVertex = TopoDS::Vertex(aChildIt.Value());
      ExtractedInternalVertex aVtxData;
      aVtxData.Shape       = aVertex;
      aVtxData.Point       = BRep_Tool::Pnt(aVertex);
      aVtxData.Tolerance   = BRep_Tool::Tolerance(aVertex);
      aVtxData.Orientation = aVertex.Orientation();
      theData.DirectVertices.Append(aVtxData);
      continue;
    }
    if (aChildIt.Value().ShapeType() != TopAbs_WIRE)
      continue;
    const TopoDS_Wire& aWire = TopoDS::Wire(aChildIt.Value());

    ExtractedWire aWireData;
    aWireData.Shape   = aWire;
    aWireData.IsOuter = aWire.IsSame(anOuterWire);

    for (TopoDS_Iterator anEdgeIt(aWire); anEdgeIt.More(); anEdgeIt.Next())
    {
      if (anEdgeIt.Value().ShapeType() != TopAbs_EDGE)
        continue;
      const TopoDS_Edge& anEdge = TopoDS::Edge(anEdgeIt.Value());

      ExtractedEdge anEdgeData;
      anEdgeData.Shape             = anEdge;
      anEdgeData.Tolerance         = BRep_Tool::Tolerance(anEdge);
      anEdgeData.IsDegenerate      = BRep_Tool::Degenerated(anEdge);
      anEdgeData.SameParameter     = BRep_Tool::SameParameter(anEdge);
      anEdgeData.SameRange         = BRep_Tool::SameRange(anEdge);
      anEdgeData.OrientationInWire = anEdge.Orientation();

      double aFirst = 0.0, aLast = 0.0;
      TopLoc_Location aCurveCombinedLoc;
      anEdgeData.Curve3d    = BRep_Tool::Curve(anEdge, aCurveCombinedLoc, aFirst, aLast);
      anEdgeData.ParamFirst = aFirst;
      anEdgeData.ParamLast  = aLast;

      // Apply representation location to 3D curve.
      // BRep_Tool::Curve(edge, loc, ...) returns BRep_Curve3D->Curve3D() and sets
      // loc = edge.Location() * BRep_Curve3D->Location()  (see BRep_GCurve).
      // Factor out the topological location to isolate the curve representation
      // location, then apply it so stored geometry is in the definition frame.
      if (!aCurveCombinedLoc.IsIdentity() && !anEdgeData.Curve3d.IsNull())
      {
        const TopLoc_Location aCurveRepLoc = anEdge.Location().Inverted() * aCurveCombinedLoc;
        if (!aCurveRepLoc.IsIdentity())
        {
          anEdgeData.Curve3d = occ::down_cast<Geom_Curve>(
            anEdgeData.Curve3d->Transformed(aCurveRepLoc.Transformation()));
        }
      }

      TopoDS_Vertex aVFirst, aVLast;
      edgeVertices(anEdge, aVFirst, aVLast, anEdgeData.InternalVertices);

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

      if (!anEdgeData.PCurve2d.IsNull() && !theData.Surface.IsNull())
        BRep_Tool::UVPoints(anEdge, aForwardFace, anEdgeData.PCUV1, anEdgeData.PCUV2);

      // Polygon3D: apply representation location to polygon nodes.
      {
        TopLoc_Location aPoly3DLoc;
        anEdgeData.Polygon3D = BRep_Tool::Polygon3D(anEdge, aPoly3DLoc);
        if (!aPoly3DLoc.IsIdentity() && !anEdgeData.Polygon3D.IsNull())
        {
          const TopLoc_Location aPolyRepLoc = anEdge.Location().Inverted() * aPoly3DLoc;
          if (!aPolyRepLoc.IsIdentity())
          {
            const gp_Trsf& aTrsf = aPolyRepLoc.Transformation();
            const NCollection_Array1<gp_Pnt>& aNodes = anEdgeData.Polygon3D->Nodes();
            NCollection_Array1<gp_Pnt> aNewNodes(aNodes.Lower(), aNodes.Upper());
            for (int aNodeIdx = aNodes.Lower(); aNodeIdx <= aNodes.Upper(); ++aNodeIdx)
              aNewNodes.SetValue(aNodeIdx, aNodes.Value(aNodeIdx).Transformed(aTrsf));
            occ::handle<Poly_Polygon3D> aTransPoly;
            if (anEdgeData.Polygon3D->HasParameters())
              aTransPoly = new Poly_Polygon3D(aNewNodes, anEdgeData.Polygon3D->Parameters());
            else
              aTransPoly = new Poly_Polygon3D(aNewNodes);
            aTransPoly->Deflection(anEdgeData.Polygon3D->Deflection());
            anEdgeData.Polygon3D = aTransPoly;
          }
        }
      }

      // PolygonOnSurface.
      {
        anEdgeData.PolyOnSurf = BRep_Tool::PolygonOnSurface(anEdge, aForwardFace);
        if (!anEdgeData.PolyOnSurf.IsNull())
        {
          TopoDS_Edge aReversedEdge = TopoDS::Edge(anEdge.Reversed());
          occ::handle<Poly_Polygon2D> aPolyOnSurfRev =
            BRep_Tool::PolygonOnSurface(aReversedEdge, aForwardFace);
          if (!aPolyOnSurfRev.IsNull() && aPolyOnSurfRev != anEdgeData.PolyOnSurf)
            anEdgeData.PolyOnSurfReversed = aPolyOnSurfRev;
        }
      }

      // Second PCurve for seam edges (edge closed on face = two distinct PCurves).
      // Use BRep_Tool::IsClosed rather than Handle pointer comparison to avoid
      // false positives: CurveOnSurface may return distinct Handle objects for
      // FORWARD/REVERSED edges even on non-seam planar faces.
      if (!anEdgeData.PCurve2d.IsNull()
          && BRep_Tool::IsClosed(anEdge, aForwardFace))
      {
        double aPCFirstRev = 0.0, aPCLastRev = 0.0;
        TopoDS_Edge aReversedEdge = TopoDS::Edge(anEdge.Reversed());
        occ::handle<Geom2d_Curve> aPC2 =
          BRep_Tool::CurveOnSurface(aReversedEdge, aForwardFace, aPCFirstRev, aPCLastRev);
        if (!aPC2.IsNull())
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

//! Register pre-extracted face data into incidence storage.
//! Uses unified TShapeToNodeId map and populates OriginalShapes.
void registerFaceData(BRepGraphInc_Storage&                    theStorage,
                      const NCollection_Vector<FaceLocalData>& theFaceData)
{
  for (int aFaceDataIdx = 0; aFaceDataIdx < theFaceData.Length(); ++aFaceDataIdx)
  {
    const FaceLocalData& aData    = theFaceData.Value(aFaceDataIdx);
    const TopoDS_Face&   aCurFace = aData.Face;

    // Create or reuse FaceEntity.
    const TopoDS_TShape*     aFaceTShapeKey = aCurFace.TShape().get();
    const BRepGraph_NodeId*  anExistingFace = theStorage.FindNodeByTShape(aFaceTShapeKey);

    int aFaceIdx = -1;
    if (anExistingFace != nullptr && anExistingFace->NodeKind == BRepGraph_NodeId::Kind::Face)
    {
      aFaceIdx = anExistingFace->Index;
    }
    else
    {
      BRepGraphInc::FaceEntity& aFace = theStorage.AppendFace();
      aFaceIdx = theStorage.NbFaces() - 1;
      aFace.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, aFaceIdx);
      aFace.Tolerance          = aData.Tolerance;
      aFace.NaturalRestriction = aData.NaturalRestriction;
      aFace.Surface            = aData.Surface;

      for (int aTriIdx = 0; aTriIdx < aData.Triangulations.Length(); ++aTriIdx)
        aFace.Triangulations.Append(aData.Triangulations.Value(aTriIdx));
      aFace.ActiveTriangulationIndex = aData.ActiveTriangulationIndex;

      theStorage.BindTShapeToNode(aFaceTShapeKey, aFace.Id);
      theStorage.BindOriginal(aFace.Id, aCurFace);
    }

    // Link face to parent shell.
    if (aData.ParentShellIdx >= 0)
    {
      BRepGraphInc::FaceRef aRef;
      aRef.FaceIdx     = aFaceIdx;
      aRef.Orientation = aData.Orientation;
      theStorage.ChangeShell(aData.ParentShellIdx).FaceRefs.Append(aRef);
    }

    // Pre-fetch face entity for triangulation access in edge loop.
    const BRepGraphInc::FaceEntity& aFaceDef = theStorage.Face(aFaceIdx);

    // Process wires.
    for (int aWireIter = 0; aWireIter < aData.Wires.Length(); ++aWireIter)
    {
      const ExtractedWire& aWireData = aData.Wires.Value(aWireIter);

      // Dedup wire by TShape.
      const TopoDS_TShape*    aWireTShapeKey = aWireData.Shape.TShape().get();
      const BRepGraph_NodeId* anExistingWire = theStorage.FindNodeByTShape(aWireTShapeKey);

      int  aWireIdx      = -1;
      bool aIsNewWireDef = false;

      if (anExistingWire != nullptr && anExistingWire->NodeKind == BRepGraph_NodeId::Kind::Wire)
      {
        aWireIdx = anExistingWire->Index;
      }
      else
      {
        BRepGraphInc::WireEntity& aWire = theStorage.AppendWire();
        aWireIdx = theStorage.NbWires() - 1;
        aWire.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Wire, aWireIdx);
        theStorage.BindTShapeToNode(aWireTShapeKey, aWire.Id);
        theStorage.BindOriginal(aWire.Id, aWireData.Shape);
        aIsNewWireDef = true;
      }

      // Link wire to face.
      {
        BRepGraphInc::WireRef aWireRef;
        aWireRef.WireIdx     = aWireIdx;
        aWireRef.IsOuter     = aWireData.IsOuter;
        aWireRef.Orientation = aWireData.Shape.Orientation();
        theStorage.ChangeFace(aFaceIdx).WireRefs.Append(aWireRef);
      }

      for (int anEdgeIter = 0; anEdgeIter < aWireData.Edges.Length(); ++anEdgeIter)
      {
        const ExtractedEdge& anEdgeData = aWireData.Edges.Value(anEdgeIter);
        const TopoDS_Edge&   anEdge     = anEdgeData.Shape;

        // Dedup edge by TShape.
        const TopoDS_TShape*    anEdgeTShapeKey = anEdge.TShape().get();
        const BRepGraph_NodeId* anExistingEdge  = theStorage.FindNodeByTShape(anEdgeTShapeKey);

        int anEdgeIdx = -1;

        if (anExistingEdge != nullptr && anExistingEdge->NodeKind == BRepGraph_NodeId::Kind::Edge)
        {
          anEdgeIdx = anExistingEdge->Index;
        }
        else
        {
          BRepGraphInc::EdgeEntity& anEdgeEnt = theStorage.AppendEdge();
          anEdgeIdx = theStorage.NbEdges() - 1;
          anEdgeEnt.Id            = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, anEdgeIdx);
          anEdgeEnt.Tolerance     = anEdgeData.Tolerance;
          anEdgeEnt.IsDegenerate  = anEdgeData.IsDegenerate;
          anEdgeEnt.SameParameter = anEdgeData.SameParameter;
          anEdgeEnt.SameRange     = anEdgeData.SameRange;
          anEdgeEnt.ParamFirst    = anEdgeData.ParamFirst;
          anEdgeEnt.ParamLast     = anEdgeData.ParamLast;

          if (!anEdgeData.Curve3d.IsNull())
            anEdgeEnt.Curve3d = anEdgeData.Curve3d;

          // Vertex registration.
          auto processVertex = [&](const ExtractedVertex& theVtxData) -> int {
            if (theVtxData.Shape.IsNull())
              return -1;
            const TopoDS_TShape*    aVTShapeKey   = theVtxData.Shape.TShape().get();
            const BRepGraph_NodeId* anExistingVtx = theStorage.FindNodeByTShape(aVTShapeKey);
            if (anExistingVtx != nullptr
                && anExistingVtx->NodeKind == BRepGraph_NodeId::Kind::Vertex)
              return anExistingVtx->Index;

            BRepGraphInc::VertexEntity& aVtxEnt = theStorage.AppendVertex();
            int aVtxIdx = theStorage.NbVertices() - 1;
            aVtxEnt.Id        = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Vertex, aVtxIdx);
            aVtxEnt.Point     = theVtxData.Point;
            aVtxEnt.Tolerance = theVtxData.Tolerance;
            theStorage.BindTShapeToNode(aVTShapeKey, aVtxEnt.Id);
            theStorage.BindOriginal(aVtxEnt.Id, theVtxData.Shape);
            return aVtxIdx;
          };

          anEdgeEnt.StartVertexIdx = processVertex(anEdgeData.StartVertex);
          anEdgeEnt.EndVertexIdx   = processVertex(anEdgeData.EndVertex);

          // Register internal/external vertices.
          for (int anIntIdx = 0; anIntIdx < anEdgeData.InternalVertices.Length(); ++anIntIdx)
          {
            const ExtractedInternalVertex& anIntVtx = anEdgeData.InternalVertices.Value(anIntIdx);
            ExtractedVertex anExtVtx;
            anExtVtx.Shape     = anIntVtx.Shape;
            anExtVtx.Point     = anIntVtx.Point;
            anExtVtx.Tolerance = anIntVtx.Tolerance;
            int anIntVtxIdx = processVertex(anExtVtx);
            if (anIntVtxIdx >= 0)
            {
              BRepGraphInc::VertexRef aVR;
              aVR.VertexIdx   = anIntVtxIdx;
              aVR.Orientation = anIntVtx.Orientation;
              anEdgeEnt.InternalVertices.Append(aVR);
            }
          }

          theStorage.BindTShapeToNode(anEdgeTShapeKey, anEdgeEnt.Id);
          theStorage.BindOriginal(anEdgeEnt.Id, anEdge);
        }

        // Cache mutable edge reference for subsequent PCurve/Polygon appends.
        // Safe: no new edges are appended within this scope.
        BRepGraphInc::EdgeEntity& anEdgeMut = theStorage.ChangeEdge(anEdgeIdx);

        // Edge -> Wire: add edge ref to wire (only for new wire defs).
        if (aIsNewWireDef)
        {
          BRepGraphInc::EdgeRef anEdgeRef;
          anEdgeRef.EdgeIdx     = anEdgeIdx;
          anEdgeRef.Orientation = anEdgeData.OrientationInWire;
          theStorage.ChangeWire(aWireIdx).EdgeRefs.Append(anEdgeRef);
        }

        // Skip if PCurves for this (edge, face) pair already stored.
        // For seam edges, both forward and reversed PCurves are captured on
        // first encounter; the second occurrence (opposite orientation) must
        // not duplicate them, as that causes orientation-dependent swaps
        // during reconstruction.
        bool aHasPCurveForFace = false;
        for (int aPCIdx = 0; aPCIdx < anEdgeMut.PCurves.Length(); ++aPCIdx)
        {
          if (anEdgeMut.PCurves.Value(aPCIdx).FaceDefId.Index == aFaceIdx)
          {
            aHasPCurveForFace = true;
            break;
          }
        }

        if (!aHasPCurveForFace)
        {
          // Inline PCurve: forward.
          if (!anEdgeData.PCurve2d.IsNull())
          {
            BRepGraphInc::EdgeEntity::PCurveEntry aPCEntry;
            aPCEntry.Curve2d         = anEdgeData.PCurve2d;
            aPCEntry.FaceDefId       = BRepGraph_NodeId::Face(aFaceIdx);
            aPCEntry.ParamFirst      = anEdgeData.PCFirst;
            aPCEntry.ParamLast       = anEdgeData.PCLast;
            aPCEntry.Continuity      = anEdgeData.PCurveContinuity;
            aPCEntry.UV1             = anEdgeData.PCUV1;
            aPCEntry.UV2             = anEdgeData.PCUV2;
            aPCEntry.EdgeOrientation = anEdgeData.OrientationInWire;
            anEdgeMut.PCurves.Append(aPCEntry);

            if (!anEdgeData.PolyOnSurf.IsNull())
            {
              BRepGraphInc::EdgeEntity::PolyOnSurfEntry aPolyEntry;
              aPolyEntry.Polygon2D       = anEdgeData.PolyOnSurf;
              aPolyEntry.FaceDefId       = BRepGraph_NodeId::Face(aFaceIdx);
              aPolyEntry.EdgeOrientation = anEdgeData.OrientationInWire;
              anEdgeMut.PolygonsOnSurf.Append(aPolyEntry);
            }
          }

          // Inline PCurve: reversed (seam).
          if (!anEdgeData.PCurve2dReversed.IsNull())
          {
            BRepGraphInc::EdgeEntity::PCurveEntry aPCEntry;
            aPCEntry.Curve2d         = anEdgeData.PCurve2dReversed;
            aPCEntry.FaceDefId       = BRepGraph_NodeId::Face(aFaceIdx);
            aPCEntry.ParamFirst      = anEdgeData.PCFirstReversed;
            aPCEntry.ParamLast       = anEdgeData.PCLastReversed;
            aPCEntry.Continuity      = anEdgeData.PCurveContinuity;
            aPCEntry.EdgeOrientation = TopAbs::Reverse(anEdgeData.OrientationInWire);
            anEdgeMut.PCurves.Append(aPCEntry);

            if (!anEdgeData.PolyOnSurfReversed.IsNull())
            {
              BRepGraphInc::EdgeEntity::PolyOnSurfEntry aPolyEntry;
              aPolyEntry.Polygon2D       = anEdgeData.PolyOnSurfReversed;
              aPolyEntry.FaceDefId       = BRepGraph_NodeId::Face(aFaceIdx);
              aPolyEntry.EdgeOrientation = TopAbs::Reverse(anEdgeData.OrientationInWire);
              anEdgeMut.PolygonsOnSurf.Append(aPolyEntry);
            }
          }
        }

        // Polygon3D (once per edge).
        if (!anEdgeData.Polygon3D.IsNull() && anEdgeMut.Polygon3D.IsNull())
          anEdgeMut.Polygon3D = anEdgeData.Polygon3D;

        // Inline PolygonsOnTri: polygon-on-triangulation.
        // Same face-dedup guard as PCurves above: skip if already stored.
        bool aHasPolyOnTriForFace = false;
        for (int aPTIdx = 0; aPTIdx < anEdgeMut.PolygonsOnTri.Length(); ++aPTIdx)
        {
          if (anEdgeMut.PolygonsOnTri.Value(aPTIdx).FaceDefId.Index == aFaceIdx)
          {
            aHasPolyOnTriForFace = true;
            break;
          }
        }

        if (!aHasPolyOnTriForFace)
        {
          // If the representation location is non-identity, the triangulation nodes
          // must be transformed. We create a copy of the triangulation with transformed
          // nodes and append it to the face's triangulation list.
          // Capture initial count: appended transformed copies must not be re-iterated.
          const int aNbOrigTris = aFaceDef.Triangulations.Length();
          for (int aTriIdx = 0; aTriIdx < aNbOrigTris; ++aTriIdx)
          {
            const occ::handle<Poly_Triangulation>& aTri = aFaceDef.Triangulations.Value(aTriIdx);
            if (aTri.IsNull())
              continue;

            TopLoc_Location aPolyTriLoc;
            occ::handle<Poly_PolygonOnTriangulation> aPolyOnTri =
              BRep_Tool::PolygonOnTriangulation(anEdgeData.Shape, aTri, aPolyTriLoc);
            if (aPolyOnTri.IsNull())
              continue;

            // Extract representation location by factoring out edge topological location.
            int aEffTriIdx = aTriIdx;
            if (!aPolyTriLoc.IsIdentity())
            {
              const TopLoc_Location aRepLoc =
                anEdgeData.Shape.Location().Inverted() * aPolyTriLoc;
              if (!aRepLoc.IsIdentity())
              {
                // Create a transformed copy of the triangulation and append it.
                occ::handle<Poly_Triangulation> aTriCopy = aTri->Copy();
                const gp_Trsf& aTrsf = aRepLoc.Transformation();
                for (int aNodeIdx = 1; aNodeIdx <= aTriCopy->NbNodes(); ++aNodeIdx)
                  aTriCopy->SetNode(aNodeIdx, aTriCopy->Node(aNodeIdx).Transformed(aTrsf));
                aEffTriIdx = theStorage.ChangeFace(aFaceIdx).Triangulations.Length();
                theStorage.ChangeFace(aFaceIdx).Triangulations.Append(aTriCopy);
              }
            }

            BRepGraphInc::EdgeEntity::PolyOnTriEntry aPolyTriEntry;
            aPolyTriEntry.Polygon            = aPolyOnTri;
            aPolyTriEntry.FaceDefId          = BRepGraph_NodeId::Face(aFaceIdx);
            aPolyTriEntry.TriangulationIndex = aEffTriIdx;
            aPolyTriEntry.EdgeOrientation    = TopAbs_FORWARD;
            anEdgeMut.PolygonsOnTri.Append(aPolyTriEntry);

            // Check for seam polygon-on-triangulation.
            TopoDS_Edge aRevEdge = TopoDS::Edge(anEdgeData.Shape.Reversed());
            occ::handle<Poly_PolygonOnTriangulation> aPolyOnTriRev =
              BRep_Tool::PolygonOnTriangulation(aRevEdge, aTri, aPolyTriLoc);
            if (!aPolyOnTriRev.IsNull() && aPolyOnTriRev != aPolyOnTri)
            {
              BRepGraphInc::EdgeEntity::PolyOnTriEntry aPolyTriRevEntry;
              aPolyTriRevEntry.Polygon            = aPolyOnTriRev;
              aPolyTriRevEntry.FaceDefId          = BRepGraph_NodeId::Face(aFaceIdx);
              aPolyTriRevEntry.TriangulationIndex = aEffTriIdx;
              aPolyTriRevEntry.EdgeOrientation    = TopAbs_REVERSED;
              anEdgeMut.PolygonsOnTri.Append(aPolyTriRevEntry);
            }
          }
        }

      }

      // Set wire closure from the TopoDS wire flag.
      if (aIsNewWireDef)
      {
        theStorage.ChangeWire(aWireIdx).IsClosed = aWireData.Shape.Closed();
      }
    }

    // Register direct vertex children of the face (INTERNAL/EXTERNAL).
    for (int aDVIdx = 0; aDVIdx < aData.DirectVertices.Length(); ++aDVIdx)
    {
      const ExtractedInternalVertex& aDirVtx = aData.DirectVertices.Value(aDVIdx);
      const TopoDS_TShape*    aVTShapeKey   = aDirVtx.Shape.TShape().get();
      const BRepGraph_NodeId* anExistingVtx = theStorage.FindNodeByTShape(aVTShapeKey);
      int aVtxIdx = -1;
      if (anExistingVtx != nullptr
          && anExistingVtx->NodeKind == BRepGraph_NodeId::Kind::Vertex)
      {
        aVtxIdx = anExistingVtx->Index;
      }
      else
      {
        BRepGraphInc::VertexEntity& aVtxEnt = theStorage.AppendVertex();
        aVtxIdx = theStorage.NbVertices() - 1;
        aVtxEnt.Id        = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Vertex, aVtxIdx);
        aVtxEnt.Point     = aDirVtx.Point;
        aVtxEnt.Tolerance = aDirVtx.Tolerance;
        theStorage.BindTShapeToNode(aVTShapeKey, aVtxEnt.Id);
        theStorage.BindOriginal(aVtxEnt.Id, aDirVtx.Shape);
      }
      BRepGraphInc::VertexRef aVR;
      aVR.VertexIdx   = aVtxIdx;
      aVR.Orientation = aDirVtx.Orientation;
      theStorage.ChangeFace(aFaceIdx).VertexRefs.Append(aVR);
    }
  }
}

} // anonymous namespace

//=================================================================================================

void BRepGraphInc_Populate::Perform(
  BRepGraphInc_Storage&                    theStorage,
  const TopoDS_Shape&                      theShape,
  bool                                     theParallel,
  const Options&                           theOptions,
  const occ::handle<NCollection_BaseAllocator>& theTmpAlloc)
{
  theStorage.Clear();

  if (theShape.IsNull())
    return;

  // Use temporary allocator if provided, else default.
  // Must NOT use the storage's persistent allocator for scratch data.
  const occ::handle<NCollection_BaseAllocator>& aTmpAlloc =
    !theTmpAlloc.IsNull() ? theTmpAlloc
                          : NCollection_BaseAllocator::CommonBaseAllocator();

  // Phase 1 (sequential): Recursively explore hierarchy, collecting face contexts.
  NCollection_Vector<FaceLocalData> aFaceData(256, aTmpAlloc);

  std::function<void(const TopoDS_Shape&, int, const TopLoc_Location&)> traverseShape;

  traverseShape = [&](const TopoDS_Shape&    theCurrentShape,
                      int                    /*theParentContainerIdx*/,
                      const TopLoc_Location& theParentGlobalLoc) {
    if (theCurrentShape.IsNull())
      return;

    switch (theCurrentShape.ShapeType())
    {
      case TopAbs_COMPOUND: {
        const TopoDS_Compound& aCompound = TopoDS::Compound(theCurrentShape);

        // Dedup by TShape.
        const TopoDS_TShape*    aCompTShapeKey = aCompound.TShape().get();
        const BRepGraph_NodeId* anExistingComp = theStorage.FindNodeByTShape(aCompTShapeKey);
        if (anExistingComp != nullptr
            && anExistingComp->NodeKind == BRepGraph_NodeId::Kind::Compound)
          break; // already registered

        BRepGraphInc::CompoundEntity& aCompEnt = theStorage.myCompounds.Appended();
        int aCompIdx = theStorage.myCompounds.Length() - 1;
        aCompEnt.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Compound, aCompIdx);
        theStorage.BindTShapeToNode(aCompTShapeKey, aCompEnt.Id);
        theStorage.BindOriginal(aCompEnt.Id, aCompound);

        const TopLoc_Location aGlobalLoc = theParentGlobalLoc * aCompound.Location();

        for (TopoDS_Iterator aChildIt(aCompound); aChildIt.More(); aChildIt.Next())
        {
          const TopoDS_Shape& aChild = aChildIt.Value();
          int aChildKind = -1;

          switch (aChild.ShapeType())
          {
            case TopAbs_COMPOUND:  aChildKind = static_cast<int>(BRepGraph_NodeId::Kind::Compound);  break;
            case TopAbs_COMPSOLID: aChildKind = static_cast<int>(BRepGraph_NodeId::Kind::CompSolid); break;
            case TopAbs_SOLID:     aChildKind = static_cast<int>(BRepGraph_NodeId::Kind::Solid);     break;
            case TopAbs_SHELL:     aChildKind = static_cast<int>(BRepGraph_NodeId::Kind::Shell);     break;
            case TopAbs_FACE:      aChildKind = static_cast<int>(BRepGraph_NodeId::Kind::Face);      break;
            case TopAbs_WIRE:      aChildKind = static_cast<int>(BRepGraph_NodeId::Kind::Wire);      break;
            case TopAbs_EDGE:      aChildKind = static_cast<int>(BRepGraph_NodeId::Kind::Edge);      break;
            case TopAbs_VERTEX:    aChildKind = static_cast<int>(BRepGraph_NodeId::Kind::Vertex);    break;
            default: break;
          }

          traverseShape(aChild, -1, aGlobalLoc);

          if (aChildKind >= 0)
          {
            // Resolve child index via TShape lookup (handles dedup correctly).
            // Face indices are deferred (-1) because faces are registered in Phase 3.
            int aChildIdx = -1;
            if (aChild.ShapeType() != TopAbs_FACE)
            {
              const BRepGraph_NodeId* aChildNodeId =
                theStorage.FindNodeByTShape(aChild.TShape().get());
              if (aChildNodeId != nullptr)
                aChildIdx = aChildNodeId->Index;
            }

            BRepGraphInc::ChildRef aRef;
            aRef.Kind          = aChildKind;
            aRef.ChildIdx      = aChildIdx;
            aRef.Orientation   = aChild.Orientation();
            aRef.LocalLocation = aChild.Location();
            theStorage.myCompounds.ChangeValue(aCompIdx).ChildRefs.Append(aRef);
          }
        }
        break;
      }

      case TopAbs_COMPSOLID: {
        const TopoDS_CompSolid& aCompSolid = TopoDS::CompSolid(theCurrentShape);

        // Dedup by TShape.
        const TopoDS_TShape*    aCSTShapeKey = aCompSolid.TShape().get();
        const BRepGraph_NodeId* anExistingCS = theStorage.FindNodeByTShape(aCSTShapeKey);
        if (anExistingCS != nullptr
            && anExistingCS->NodeKind == BRepGraph_NodeId::Kind::CompSolid)
          break;

        BRepGraphInc::CompSolidEntity& aCSolidEnt = theStorage.myCompSolids.Appended();
        int aCSolidIdx = theStorage.myCompSolids.Length() - 1;
        aCSolidEnt.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::CompSolid, aCSolidIdx);
        theStorage.BindTShapeToNode(aCSTShapeKey, aCSolidEnt.Id);
        theStorage.BindOriginal(aCSolidEnt.Id, aCompSolid);

        const TopLoc_Location aGlobalLoc = theParentGlobalLoc * aCompSolid.Location();

        for (TopoDS_Iterator aChildIt(aCompSolid); aChildIt.More(); aChildIt.Next())
        {
          if (aChildIt.Value().ShapeType() != TopAbs_SOLID)
            continue;
          traverseShape(aChildIt.Value(), -1, aGlobalLoc);

          const BRepGraph_NodeId* aSolidNodeId =
            theStorage.FindNodeByTShape(aChildIt.Value().TShape().get());
          if (aSolidNodeId == nullptr)
            continue;

          BRepGraphInc::SolidRef aRef;
          aRef.SolidIdx      = aSolidNodeId->Index;
          aRef.Orientation   = aChildIt.Value().Orientation();
          aRef.LocalLocation = aChildIt.Value().Location();
          theStorage.myCompSolids.ChangeValue(aCSolidIdx).SolidRefs.Append(aRef);
        }
        break;
      }

      case TopAbs_SOLID: {
        const TopoDS_Solid& aSolid = TopoDS::Solid(theCurrentShape);

        // Dedup by TShape.
        const TopoDS_TShape*    aSolidTShapeKey = aSolid.TShape().get();
        const BRepGraph_NodeId* anExistingSolid = theStorage.FindNodeByTShape(aSolidTShapeKey);
        if (anExistingSolid != nullptr
            && anExistingSolid->NodeKind == BRepGraph_NodeId::Kind::Solid)
          break;

        BRepGraphInc::SolidEntity& aSolidEnt = theStorage.mySolids.Appended();
        int aSolidIdx = theStorage.mySolids.Length() - 1;
        aSolidEnt.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, aSolidIdx);
        theStorage.BindTShapeToNode(aSolidTShapeKey, aSolidEnt.Id);
        theStorage.BindOriginal(aSolidEnt.Id, aSolid);

        const TopLoc_Location aGlobalLoc = theParentGlobalLoc * aSolid.Location();

        for (TopoDS_Iterator aChildIt(aSolid); aChildIt.More(); aChildIt.Next())
        {
          const TopoDS_Shape& aChild = aChildIt.Value();
          traverseShape(aChild, -1, aGlobalLoc);

          if (aChild.ShapeType() == TopAbs_SHELL)
          {
            const BRepGraph_NodeId* aShellNodeId =
              theStorage.FindNodeByTShape(aChild.TShape().get());
            if (aShellNodeId == nullptr)
              continue;

            BRepGraphInc::ShellRef aRef;
            aRef.ShellIdx      = aShellNodeId->Index;
            aRef.Orientation   = aChild.Orientation();
            aRef.LocalLocation = aChild.Location();
            theStorage.mySolids.ChangeValue(aSolidIdx).ShellRefs.Append(aRef);
          }
          else if (aChild.ShapeType() == TopAbs_EDGE
                   || aChild.ShapeType() == TopAbs_VERTEX)
          {
            const BRepGraph_NodeId* aChildNodeId =
              theStorage.FindNodeByTShape(aChild.TShape().get());
            if (aChildNodeId == nullptr)
              continue;

            BRepGraphInc::ChildRef aCR;
            aCR.Kind          = static_cast<int>(aChildNodeId->NodeKind);
            aCR.ChildIdx      = aChildNodeId->Index;
            aCR.Orientation   = aChild.Orientation();
            aCR.LocalLocation = aChild.Location();
            theStorage.mySolids.ChangeValue(aSolidIdx).FreeChildRefs.Append(aCR);
          }
        }
        break;
      }

      case TopAbs_SHELL: {
        const TopoDS_Shell& aShell = TopoDS::Shell(theCurrentShape);

        // Dedup by TShape.
        const TopoDS_TShape*    aShellTShapeKey = aShell.TShape().get();
        const BRepGraph_NodeId* anExistingShell = theStorage.FindNodeByTShape(aShellTShapeKey);
        if (anExistingShell != nullptr
            && anExistingShell->NodeKind == BRepGraph_NodeId::Kind::Shell)
          break;

        BRepGraphInc::ShellEntity& aShellEnt = theStorage.myShells.Appended();
        int aShellIdx = theStorage.myShells.Length() - 1;
        aShellEnt.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Shell, aShellIdx);
        theStorage.BindTShapeToNode(aShellTShapeKey, aShellEnt.Id);
        theStorage.BindOriginal(aShellEnt.Id, aShell);

        const TopLoc_Location aGlobalLoc = theParentGlobalLoc * aShell.Location();

        for (TopoDS_Iterator aChildIt(aShell); aChildIt.More(); aChildIt.Next())
        {
          const TopoDS_Shape& aChild = aChildIt.Value();
          if (aChild.ShapeType() == TopAbs_FACE)
          {
            FaceLocalData& aData   = aFaceData.Appended();
            aData.Face             = TopoDS::Face(aChild);
            aData.ParentGlobalLoc  = aGlobalLoc;
            aData.ParentShellIdx   = aShellIdx;
          }
          else if (aChild.ShapeType() == TopAbs_WIRE
                   || aChild.ShapeType() == TopAbs_EDGE)
          {
            traverseShape(aChild, -1, aGlobalLoc);

            const BRepGraph_NodeId* aChildNodeId =
              theStorage.FindNodeByTShape(aChild.TShape().get());
            if (aChildNodeId == nullptr)
              continue;

            BRepGraphInc::ChildRef aCR;
            aCR.Kind          = static_cast<int>(aChildNodeId->NodeKind);
            aCR.ChildIdx      = aChildNodeId->Index;
            aCR.Orientation   = aChild.Orientation();
            aCR.LocalLocation = aChild.Location();
            theStorage.myShells.ChangeValue(aShellIdx).FreeChildRefs.Append(aCR);
          }
        }
        break;
      }

      case TopAbs_FACE: {
        FaceLocalData& aData  = aFaceData.Appended();
        aData.Face            = TopoDS::Face(theCurrentShape);
        aData.ParentGlobalLoc = theParentGlobalLoc;
        aData.ParentShellIdx  = -1;
        break;
      }

      case TopAbs_WIRE: {
        const TopoDS_Wire& aWire = TopoDS::Wire(theCurrentShape);

        // Dedup by TShape.
        const TopoDS_TShape*    aWireTShapeKey = aWire.TShape().get();
        const BRepGraph_NodeId* anExistingWire = theStorage.FindNodeByTShape(aWireTShapeKey);
        if (anExistingWire != nullptr
            && anExistingWire->NodeKind == BRepGraph_NodeId::Kind::Wire)
          break; // already registered

        BRepGraphInc::WireEntity& aWireEnt = theStorage.AppendWire();
        int aWireIdx = theStorage.NbWires() - 1;
        aWireEnt.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Wire, aWireIdx);
        aWireEnt.IsClosed = aWire.Closed();
        theStorage.BindTShapeToNode(aWireTShapeKey, aWireEnt.Id);
        theStorage.BindOriginal(aWireEnt.Id, aWire);

        for (TopoDS_Iterator anEdgeIt(aWire); anEdgeIt.More(); anEdgeIt.Next())
        {
          if (anEdgeIt.Value().ShapeType() != TopAbs_EDGE)
            continue;
          const TopoDS_Edge& anEdge = TopoDS::Edge(anEdgeIt.Value());

          // Recurse to create the edge entity (with dedup).
          traverseShape(anEdge, -1, theParentGlobalLoc);

          // Resolve edge index via TShape lookup (handles dedup correctly).
          const BRepGraph_NodeId* anEdgeNodeId =
            theStorage.FindNodeByTShape(anEdge.TShape().get());
          if (anEdgeNodeId != nullptr
              && anEdgeNodeId->NodeKind == BRepGraph_NodeId::Kind::Edge)
          {
            BRepGraphInc::EdgeRef anEdgeRef;
            anEdgeRef.EdgeIdx     = anEdgeNodeId->Index;
            anEdgeRef.Orientation = anEdge.Orientation();
            theStorage.ChangeWire(aWireIdx).EdgeRefs.Append(anEdgeRef);
          }
        }
        break;
      }

      case TopAbs_EDGE: {
        const TopoDS_Edge& anEdge = TopoDS::Edge(theCurrentShape);

        // Dedup by TShape.
        const TopoDS_TShape*    anEdgeTShapeKey = anEdge.TShape().get();
        const BRepGraph_NodeId* anExistingEdge  = theStorage.FindNodeByTShape(anEdgeTShapeKey);
        if (anExistingEdge != nullptr
            && anExistingEdge->NodeKind == BRepGraph_NodeId::Kind::Edge)
          break; // already registered

        BRepGraphInc::EdgeEntity& anEdgeEnt = theStorage.AppendEdge();
        int anEdgeIdx = theStorage.NbEdges() - 1;
        anEdgeEnt.Id            = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, anEdgeIdx);
        anEdgeEnt.Tolerance     = BRep_Tool::Tolerance(anEdge);
        anEdgeEnt.IsDegenerate  = BRep_Tool::Degenerated(anEdge);
        anEdgeEnt.SameParameter = BRep_Tool::SameParameter(anEdge);
        anEdgeEnt.SameRange     = BRep_Tool::SameRange(anEdge);

        double aFirst = 0.0, aLast = 0.0;
        TopLoc_Location aCurveCombinedLoc;
        anEdgeEnt.Curve3d = BRep_Tool::Curve(anEdge, aCurveCombinedLoc, aFirst, aLast);
        anEdgeEnt.ParamFirst = aFirst;
        anEdgeEnt.ParamLast  = aLast;

        // Apply representation location to 3D curve (same pattern as extractFaceData).
        if (!aCurveCombinedLoc.IsIdentity() && !anEdgeEnt.Curve3d.IsNull())
        {
          const TopLoc_Location aCurveRepLoc = anEdge.Location().Inverted() * aCurveCombinedLoc;
          if (!aCurveRepLoc.IsIdentity())
          {
            anEdgeEnt.Curve3d = occ::down_cast<Geom_Curve>(
              anEdgeEnt.Curve3d->Transformed(aCurveRepLoc.Transformation()));
          }
        }

        // Extract vertices.
        TopoDS_Vertex aVFirst, aVLast;
        NCollection_Vector<ExtractedInternalVertex> anInternalVerts;
        edgeVertices(anEdge, aVFirst, aVLast, anInternalVerts);

        // Register vertices using the same dedup pattern as registerFaceData.
        auto processVertex = [&](const TopoDS_Vertex& theVtx) -> int {
          if (theVtx.IsNull())
            return -1;
          const TopoDS_TShape*    aVTShapeKey   = theVtx.TShape().get();
          const BRepGraph_NodeId* anExistingVtx = theStorage.FindNodeByTShape(aVTShapeKey);
          if (anExistingVtx != nullptr
              && anExistingVtx->NodeKind == BRepGraph_NodeId::Kind::Vertex)
            return anExistingVtx->Index;

          BRepGraphInc::VertexEntity& aVtxEnt = theStorage.AppendVertex();
          int aVtxIdx = theStorage.NbVertices() - 1;
          aVtxEnt.Id        = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Vertex, aVtxIdx);
          aVtxEnt.Point     = BRep_Tool::Pnt(theVtx);
          aVtxEnt.Tolerance = BRep_Tool::Tolerance(theVtx);
          theStorage.BindTShapeToNode(aVTShapeKey, aVtxEnt.Id);
          theStorage.BindOriginal(aVtxEnt.Id, theVtx);
          return aVtxIdx;
        };

        anEdgeEnt.StartVertexIdx = processVertex(aVFirst);
        anEdgeEnt.EndVertexIdx   = processVertex(aVLast);

        for (int anIntIdx = 0; anIntIdx < anInternalVerts.Length(); ++anIntIdx)
        {
          const ExtractedInternalVertex& anIntVtx = anInternalVerts.Value(anIntIdx);
          int anIntVtxIdx = processVertex(anIntVtx.Shape);
          if (anIntVtxIdx >= 0)
          {
            BRepGraphInc::VertexRef aVR;
            aVR.VertexIdx   = anIntVtxIdx;
            aVR.Orientation = anIntVtx.Orientation;
            anEdgeEnt.InternalVertices.Append(aVR);
          }
        }

        // Polygon3D.
        {
          TopLoc_Location aPoly3DLoc;
          anEdgeEnt.Polygon3D = BRep_Tool::Polygon3D(anEdge, aPoly3DLoc);
        }

        theStorage.BindTShapeToNode(anEdgeTShapeKey, anEdgeEnt.Id);
        theStorage.BindOriginal(anEdgeEnt.Id, anEdge);
        break;
      }

      case TopAbs_VERTEX: {
        const TopoDS_Vertex& aVertex = TopoDS::Vertex(theCurrentShape);

        // Dedup by TShape.
        const TopoDS_TShape*    aVTShapeKey   = aVertex.TShape().get();
        const BRepGraph_NodeId* anExistingVtx = theStorage.FindNodeByTShape(aVTShapeKey);
        if (anExistingVtx != nullptr
            && anExistingVtx->NodeKind == BRepGraph_NodeId::Kind::Vertex)
          break; // already registered

        BRepGraphInc::VertexEntity& aVtxEnt = theStorage.AppendVertex();
        int aVtxIdx = theStorage.NbVertices() - 1;
        aVtxEnt.Id        = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Vertex, aVtxIdx);
        aVtxEnt.Point     = BRep_Tool::Pnt(aVertex);
        aVtxEnt.Tolerance = BRep_Tool::Tolerance(aVertex);
        theStorage.BindTShapeToNode(aVTShapeKey, aVtxEnt.Id);
        theStorage.BindOriginal(aVtxEnt.Id, aVertex);
        break;
      }

      default:
        break;
    }
  };

  traverseShape(theShape, -1, TopLoc_Location());

  // Phase 2 (parallel): Extract per-face geometry/topology.
  OSD_Parallel::For(
    0,
    aFaceData.Length(),
    [&](int theIndex) { extractFaceData(aFaceData.ChangeValue(theIndex)); },
    !theParallel);

  // Phase 3 (sequential): Register into storage with deduplication.
  registerFaceData(theStorage, aFaceData);

  // Phase 3a: Fix compound Face ChildRefs (face indices were unknown during Phase 1,
  // resolved now after registerFaceData). All other child types were resolved
  // immediately in traverseShape via FindNodeByTShape.
  for (int aCompIdx = 0; aCompIdx < theStorage.myCompounds.Length(); ++aCompIdx)
  {
    BRepGraphInc::CompoundEntity& aComp = theStorage.myCompounds.ChangeValue(aCompIdx);
    const TopoDS_Shape* aCompOrig = theStorage.myOriginalShapes.Seek(aComp.Id);
    if (aCompOrig == nullptr)
      continue;

    int aCRIdx = 0;
    for (TopoDS_Iterator aChildIt(*aCompOrig); aChildIt.More(); aChildIt.Next())
    {
      if (aCRIdx >= aComp.ChildRefs.Length())
        break;
      BRepGraphInc::ChildRef& aCR = aComp.ChildRefs.ChangeValue(aCRIdx);
      if (aCR.ChildIdx < 0)
      {
        const BRepGraph_NodeId* aNodeId =
          theStorage.myTShapeToNodeId.Seek(aChildIt.Value().TShape().get());
        if (aNodeId != nullptr
            && static_cast<int>(aNodeId->NodeKind) == aCR.Kind)
          aCR.ChildIdx = aNodeId->Index;
      }
      ++aCRIdx;
    }
  }

  // Edge regularities.
  if (theOptions.ExtractRegularities)
  {
    // Map original raw surface pointers to face indices.
    // BRep_CurveOn2Surfaces stores original raw surface pointers, so the lookup
    // must use the same pointers (not the transformed ones in the entity).
    NCollection_DataMap<const Geom_Surface*, int> aSurfToFaceIdx(1, aTmpAlloc);
    for (int i = 0; i < theStorage.myFaces.Length(); ++i)
    {
      const BRepGraphInc::FaceEntity& aFace = theStorage.myFaces.Value(i);
      const TopoDS_Shape* anOrigFace = theStorage.myOriginalShapes.Seek(aFace.Id);
      if (anOrigFace != nullptr && !anOrigFace->IsNull())
      {
        TopLoc_Location aLoc;
        occ::handle<Geom_Surface> aRawSurf =
          BRep_Tool::Surface(TopoDS::Face(*anOrigFace), aLoc);
        if (!aRawSurf.IsNull())
          aSurfToFaceIdx.TryBind(aRawSurf.get(), i);
      }
    }

    OSD_Parallel::For(0, theStorage.myEdges.Length(),
      [&theStorage, &aSurfToFaceIdx](int anEdgeIdx) {
        BRepGraphInc::EdgeEntity& anEdgeEnt = theStorage.myEdges.ChangeValue(anEdgeIdx);

        const TopoDS_Shape* anOrigShape = theStorage.myOriginalShapes.Seek(anEdgeEnt.Id);
        if (anOrigShape == nullptr || anOrigShape->IsNull())
          return;

        const TopoDS_Edge& anEdge = TopoDS::Edge(*anOrigShape);
        const occ::handle<BRep_TEdge> aTEdge = occ::down_cast<BRep_TEdge>(anEdge.TShape());
        if (aTEdge.IsNull())
          return;

        for (const occ::handle<BRep_CurveRepresentation>& aCRep : aTEdge->Curves())
        {
          if (aCRep.IsNull())
            continue;
          const occ::handle<BRep_CurveOn2Surfaces> aCon2S =
            occ::down_cast<BRep_CurveOn2Surfaces>(aCRep);
          if (aCon2S.IsNull())
            continue;

          const Geom_Surface* aSurf1Ptr = aCon2S->Surface().get();
          const Geom_Surface* aSurf2Ptr = aCon2S->Surface2().get();
          if (aSurf1Ptr == nullptr || aSurf2Ptr == nullptr)
            continue;

          const int* aFaceIdx1 = aSurfToFaceIdx.Seek(aSurf1Ptr);
          const int* aFaceIdx2 = aSurfToFaceIdx.Seek(aSurf2Ptr);
          if (aFaceIdx1 == nullptr || aFaceIdx2 == nullptr)
            continue;

          BRepGraphInc::EdgeEntity::RegularityEntry aRegEntry;
          aRegEntry.FaceDef1   = BRepGraph_NodeId::Face(*aFaceIdx1);
          aRegEntry.FaceDef2   = BRepGraph_NodeId::Face(*aFaceIdx2);
          aRegEntry.Continuity = aCon2S->Continuity();
          anEdgeEnt.Regularities.Append(aRegEntry);
        }
      }, !theParallel);
    theStorage.myHasRegularities = true;
  }

  // Vertex point representations.
  if (theOptions.ExtractVertexPointReps)
  {
    // Map original raw curve/surface pointers to edge/face node IDs.
    // BRep_PointRepresentation stores original raw pointers, so the lookup
    // must use the same pointers (not the transformed ones in the entity).
    NCollection_DataMap<const Geom_Curve*, BRepGraph_NodeId> aCurveToEdgeDef(1, aTmpAlloc);
    for (int i = 0; i < theStorage.myEdges.Length(); ++i)
    {
      const BRepGraphInc::EdgeEntity& anEdgeEnt = theStorage.myEdges.Value(i);
      const TopoDS_Shape* anOrigEdge = theStorage.myOriginalShapes.Seek(anEdgeEnt.Id);
      if (anOrigEdge != nullptr && !anOrigEdge->IsNull())
      {
        double aF = 0.0, aL = 0.0;
        TopLoc_Location aLoc;
        occ::handle<Geom_Curve> aRawCurve =
          BRep_Tool::Curve(TopoDS::Edge(*anOrigEdge), aLoc, aF, aL);
        if (!aRawCurve.IsNull())
          aCurveToEdgeDef.TryBind(aRawCurve.get(), anEdgeEnt.Id);
      }
    }

    NCollection_DataMap<const Geom_Surface*, BRepGraph_NodeId> aSurfToFaceDefVtx(1, aTmpAlloc);
    for (int i = 0; i < theStorage.myFaces.Length(); ++i)
    {
      const BRepGraphInc::FaceEntity& aFaceEnt = theStorage.myFaces.Value(i);
      const TopoDS_Shape* anOrigFace = theStorage.myOriginalShapes.Seek(aFaceEnt.Id);
      if (anOrigFace != nullptr && !anOrigFace->IsNull())
      {
        TopLoc_Location aLoc;
        occ::handle<Geom_Surface> aRawSurf =
          BRep_Tool::Surface(TopoDS::Face(*anOrigFace), aLoc);
        if (!aRawSurf.IsNull())
          aSurfToFaceDefVtx.TryBind(aRawSurf.get(), aFaceEnt.Id);
      }
    }

    OSD_Parallel::For(0, theStorage.myVertices.Length(),
      [&theStorage, &aCurveToEdgeDef, &aSurfToFaceDefVtx](int aVtxIdx) {
        BRepGraphInc::VertexEntity& aVtxDef = theStorage.myVertices.ChangeValue(aVtxIdx);
        const TopoDS_Shape* aVtxShape = theStorage.myOriginalShapes.Seek(aVtxDef.Id);
        if (aVtxShape == nullptr || aVtxShape->IsNull())
          return;
        const TopoDS_Vertex& aVertex = TopoDS::Vertex(*aVtxShape);
        const occ::handle<BRep_TVertex>& aTVertex =
          occ::down_cast<BRep_TVertex>(aVertex.TShape());
        if (aTVertex.IsNull())
          return;

        for (const occ::handle<BRep_PointRepresentation>& aPtRep : aTVertex->Points())
        {
          if (aPtRep.IsNull())
            continue;
          if (const occ::handle<BRep_PointOnCurve> aPOC = occ::down_cast<BRep_PointOnCurve>(aPtRep))
          {
            const BRepGraph_NodeId* anEdgeId = aCurveToEdgeDef.Seek(aPOC->Curve().get());
            if (anEdgeId == nullptr)
              continue;
            BRepGraphInc::VertexEntity::PointOnCurveEntry anEntry;
            anEntry.Parameter = aPOC->Parameter();
            anEntry.EdgeDefId = *anEdgeId;
            aVtxDef.PointsOnCurve.Append(anEntry);
          }
          else if (const occ::handle<BRep_PointOnCurveOnSurface> aPOCS =
                     occ::down_cast<BRep_PointOnCurveOnSurface>(aPtRep))
          {
            const BRepGraph_NodeId* aFaceId = aSurfToFaceDefVtx.Seek(aPOCS->Surface().get());
            if (aFaceId == nullptr)
              continue;
            BRepGraphInc::VertexEntity::PointOnPCurveEntry anEntry;
            anEntry.Parameter = aPOCS->Parameter();
            anEntry.FaceDefId = *aFaceId;
            aVtxDef.PointsOnPCurve.Append(anEntry);
          }
          else if (const occ::handle<BRep_PointOnSurface> aPOS =
                     occ::down_cast<BRep_PointOnSurface>(aPtRep))
          {
            const BRepGraph_NodeId* aFaceId = aSurfToFaceDefVtx.Seek(aPOS->Surface().get());
            if (aFaceId == nullptr)
              continue;
            BRepGraphInc::VertexEntity::PointOnSurfaceEntry anEntry;
            anEntry.ParameterU = aPOS->Parameter();
            anEntry.ParameterV = aPOS->Parameter2();
            anEntry.FaceDefId  = *aFaceId;
            aVtxDef.PointsOnSurface.Append(anEntry);
          }
        }
      }, !theParallel);
    theStorage.myHasVertexPointReps = true;
  }

  // Build reverse indices.
  theStorage.BuildReverseIndex();

  theStorage.myIsDone = true;
}

//=================================================================================================

void BRepGraphInc_Populate::Append(
  BRepGraphInc_Storage&                    theStorage,
  const TopoDS_Shape&                      theShape,
  bool                                     theParallel,
  const occ::handle<NCollection_BaseAllocator>& theTmpAlloc)
{
  if (theShape.IsNull())
    return;

  // Use temporary allocator if provided, else default.
  // Must NOT use the storage's persistent allocator for scratch data.
  const occ::handle<NCollection_BaseAllocator>& aTmpAlloc =
    !theTmpAlloc.IsNull() ? theTmpAlloc
                          : NCollection_BaseAllocator::CommonBaseAllocator();

  // Snapshot entity counts before appending, for incremental reverse-index update.
  const int anOldNbEdges  = theStorage.NbEdges();
  const int anOldNbWires  = theStorage.NbWires();
  const int anOldNbFaces  = theStorage.NbFaces();
  const int anOldNbShells = theStorage.NbShells();
  const int anOldNbSolids = theStorage.NbSolids();

  // Collect face contexts by flattening hierarchy.
  NCollection_Vector<FaceLocalData> aFaceData(256, aTmpAlloc);

  std::function<void(const TopoDS_Shape&, const TopLoc_Location&)> traverseShape;
  traverseShape = [&](const TopoDS_Shape&    theCurrentShape,
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
                        theParentGlobalLoc * theCurrentShape.Location());
        }
        break;
      }
      case TopAbs_FACE: {
        FaceLocalData& aData  = aFaceData.Appended();
        aData.Face            = TopoDS::Face(theCurrentShape);
        aData.ParentGlobalLoc = theParentGlobalLoc;
        aData.ParentShellIdx  = -1;
        break;
      }
      default:
        break;
    }
  };

  traverseShape(theShape, TopLoc_Location());

  // Parallel face extraction.
  OSD_Parallel::For(
    0,
    aFaceData.Length(),
    [&](int theIndex) { extractFaceData(aFaceData.ChangeValue(theIndex)); },
    !theParallel);

  // Sequential registration (reuses existing dedup maps).
  registerFaceData(theStorage, aFaceData);

  // Incrementally update reverse indices for newly appended entities only.
  theStorage.BuildDeltaReverseIndex(anOldNbEdges, anOldNbWires, anOldNbFaces,
                                    anOldNbShells, anOldNbSolids);

  theStorage.myIsDone = true;
}
