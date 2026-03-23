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

//! Per-vertex data extracted from TopoDS in parallel phase.
struct ExtractedVertex
{
  TopoDS_Vertex Shape;
  gp_Pnt        Point;
  double        Tolerance = 0.0;
};

//! Per-edge data extracted from TopoDS in parallel phase.
struct ExtractedEdge
{
  TopoDS_Edge          Shape;
  Handle(Geom_Curve)   Curve3d;
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
  Handle(Geom2d_Curve) PCurve2dReversed;
  double               PCFirstReversed  = 0.0;
  double               PCLastReversed   = 0.0;
  GeomAbs_Shape        PCurveContinuity = GeomAbs_C0;
  gp_Pnt2d             PCUV1;
  gp_Pnt2d             PCUV2;
  Handle(Poly_Polygon3D) Polygon3D;
  Handle(Poly_Polygon2D) PolyOnSurf;
  Handle(Poly_Polygon2D) PolyOnSurfReversed;
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
  Handle(Geom_Surface) Surface;
  NCollection_Vector<Handle(Poly_Triangulation)> Triangulations;
  int                ActiveTriangulationIndex = -1;
  double             Tolerance          = 0.0;
  TopAbs_Orientation Orientation        = TopAbs_FORWARD;
  bool               NaturalRestriction = false;
  NCollection_Vector<ExtractedWire> Wires;
};

//! Extract first and last vertices from an edge.
void edgeVertices(const TopoDS_Edge& theEdge, TopoDS_Vertex& theFirst, TopoDS_Vertex& theLast)
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
    else if (theFirst.IsNull())
      theFirst = aVertex;
    else if (theLast.IsNull())
      theLast = aVertex;
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

  TopLoc_Location aSurfLoc;
  theData.Surface = BRep_Tool::Surface(aFace, aSurfLoc);

  // Extract triangulations.
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
      TopLoc_Location aCurveLoc;
      anEdgeData.Curve3d    = BRep_Tool::Curve(anEdge, aCurveLoc, aFirst, aLast);
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

      if (!anEdgeData.PCurve2d.IsNull() && !theData.Surface.IsNull())
        BRep_Tool::UVPoints(anEdge, aForwardFace, anEdgeData.PCUV1, anEdgeData.PCUV2);

      // Polygon3D.
      {
        TopLoc_Location aPoly3DLoc;
        anEdgeData.Polygon3D = BRep_Tool::Polygon3D(anEdge, aPoly3DLoc);
      }

      // PolygonOnSurface.
      {
        anEdgeData.PolyOnSurf = BRep_Tool::PolygonOnSurface(anEdge, aForwardFace);
        if (!anEdgeData.PolyOnSurf.IsNull())
        {
          TopoDS_Edge aReversedEdge = TopoDS::Edge(anEdge.Reversed());
          Handle(Poly_Polygon2D) aPolyOnSurfRev =
            BRep_Tool::PolygonOnSurface(aReversedEdge, aForwardFace);
          if (!aPolyOnSurfRev.IsNull() && aPolyOnSurfRev != anEdgeData.PolyOnSurf)
            anEdgeData.PolyOnSurfReversed = aPolyOnSurfRev;
        }
      }

      // Second PCurve for seam edges.
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
    const BRepGraph_NodeId*  anExistingFace = theStorage.TShapeToNodeId.Seek(aFaceTShapeKey);

    int aFaceIdx = -1;
    if (anExistingFace != nullptr && anExistingFace->NodeKind == BRepGraph_NodeId::Kind::Face)
    {
      aFaceIdx = anExistingFace->Index;
    }
    else
    {
      BRepGraphInc::FaceEntity& aFace = theStorage.Faces.Appended();
      aFaceIdx = theStorage.Faces.Length() - 1;
      aFace.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, aFaceIdx);
      aFace.Tolerance          = aData.Tolerance;
      aFace.NaturalRestriction = aData.NaturalRestriction;
      aFace.Surface            = aData.Surface;

      for (int aTriIdx = 0; aTriIdx < aData.Triangulations.Length(); ++aTriIdx)
        aFace.Triangulations.Append(aData.Triangulations.Value(aTriIdx));
      aFace.ActiveTriangulationIndex = aData.ActiveTriangulationIndex;

      theStorage.TShapeToNodeId.Bind(aFaceTShapeKey, aFace.Id);
      theStorage.OriginalShapes.Bind(aFace.Id, aCurFace);
    }

    // Link face to parent shell.
    if (aData.ParentShellIdx >= 0)
    {
      BRepGraphInc::FaceRef aRef;
      aRef.FaceIdx     = aFaceIdx;
      aRef.Orientation = aData.Orientation;
      theStorage.Shells.ChangeValue(aData.ParentShellIdx).FaceRefs.Append(aRef);
    }

    // Process wires.
    for (int aWireIter = 0; aWireIter < aData.Wires.Length(); ++aWireIter)
    {
      const ExtractedWire& aWireData = aData.Wires.Value(aWireIter);

      // Dedup wire by TShape.
      const TopoDS_TShape*    aWireTShapeKey = aWireData.Shape.TShape().get();
      const BRepGraph_NodeId* anExistingWire = theStorage.TShapeToNodeId.Seek(aWireTShapeKey);

      int  aWireIdx      = -1;
      bool aIsNewWireDef = false;

      if (anExistingWire != nullptr && anExistingWire->NodeKind == BRepGraph_NodeId::Kind::Wire)
      {
        aWireIdx = anExistingWire->Index;
      }
      else
      {
        BRepGraphInc::WireEntity& aWire = theStorage.Wires.Appended();
        aWireIdx = theStorage.Wires.Length() - 1;
        aWire.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Wire, aWireIdx);
        theStorage.TShapeToNodeId.Bind(aWireTShapeKey, aWire.Id);
        theStorage.OriginalShapes.Bind(aWire.Id, aWireData.Shape);
        aIsNewWireDef = true;
      }

      // Link wire to face.
      {
        BRepGraphInc::WireRef aWireRef;
        aWireRef.WireIdx     = aWireIdx;
        aWireRef.IsOuter     = aWireData.IsOuter;
        aWireRef.Orientation = aWireData.Shape.Orientation();
        theStorage.Faces.ChangeValue(aFaceIdx).WireRefs.Append(aWireRef);
      }

      int aFirstVertexIdx = -1;
      int aLastVertexIdx  = -1;

      for (int anEdgeIter = 0; anEdgeIter < aWireData.Edges.Length(); ++anEdgeIter)
      {
        const ExtractedEdge& anEdgeData = aWireData.Edges.Value(anEdgeIter);
        const TopoDS_Edge&   anEdge     = anEdgeData.Shape;

        // Dedup edge by TShape.
        const TopoDS_TShape*    anEdgeTShapeKey = anEdge.TShape().get();
        const BRepGraph_NodeId* anExistingEdge  = theStorage.TShapeToNodeId.Seek(anEdgeTShapeKey);

        int anEdgeIdx = -1;

        if (anExistingEdge != nullptr && anExistingEdge->NodeKind == BRepGraph_NodeId::Kind::Edge)
        {
          anEdgeIdx = anExistingEdge->Index;
        }
        else
        {
          BRepGraphInc::EdgeEntity& anEdgeEnt = theStorage.Edges.Appended();
          anEdgeIdx = theStorage.Edges.Length() - 1;
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
            const BRepGraph_NodeId* anExistingVtx = theStorage.TShapeToNodeId.Seek(aVTShapeKey);
            if (anExistingVtx != nullptr
                && anExistingVtx->NodeKind == BRepGraph_NodeId::Kind::Vertex)
              return anExistingVtx->Index;

            BRepGraphInc::VertexEntity& aVtxEnt = theStorage.Vertices.Appended();
            int aVtxIdx = theStorage.Vertices.Length() - 1;
            aVtxEnt.Id        = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Vertex, aVtxIdx);
            aVtxEnt.Point     = theVtxData.Point;
            aVtxEnt.Tolerance = theVtxData.Tolerance;
            theStorage.TShapeToNodeId.Bind(aVTShapeKey, aVtxEnt.Id);
            theStorage.OriginalShapes.Bind(aVtxEnt.Id, theVtxData.Shape);
            return aVtxIdx;
          };

          anEdgeEnt.StartVertexIdx = processVertex(anEdgeData.StartVertex);
          anEdgeEnt.EndVertexIdx   = processVertex(anEdgeData.EndVertex);

          theStorage.TShapeToNodeId.Bind(anEdgeTShapeKey, anEdgeEnt.Id);
          theStorage.OriginalShapes.Bind(anEdgeEnt.Id, anEdge);
        }

        // Edge -> Wire: add edge ref to wire (only for new wire defs).
        if (aIsNewWireDef)
        {
          BRepGraphInc::EdgeRef anEdgeRef;
          anEdgeRef.EdgeIdx     = anEdgeIdx;
          anEdgeRef.Orientation = anEdgeData.OrientationInWire;
          theStorage.Wires.ChangeValue(aWireIdx).EdgeRefs.Append(anEdgeRef);
        }

        // Inline PCurve: forward.
        if (!anEdgeData.PCurve2d.IsNull())
        {
          BRepGraphInc::EdgeEntity& anEdgeMutPC = theStorage.Edges.ChangeValue(anEdgeIdx);
          BRepGraphInc::EdgeEntity::PCurveEntry aPCEntry;
          aPCEntry.Curve2d         = anEdgeData.PCurve2d;
          aPCEntry.FaceDefId       = BRepGraph_NodeId::Face(aFaceIdx);
          aPCEntry.ParamFirst      = anEdgeData.PCFirst;
          aPCEntry.ParamLast       = anEdgeData.PCLast;
          aPCEntry.Continuity      = anEdgeData.PCurveContinuity;
          aPCEntry.UV1             = anEdgeData.PCUV1;
          aPCEntry.UV2             = anEdgeData.PCUV2;
          aPCEntry.EdgeOrientation = TopAbs_FORWARD;
          anEdgeMutPC.PCurves.Append(aPCEntry);

          if (!anEdgeData.PolyOnSurf.IsNull())
          {
            BRepGraphInc::EdgeEntity::PolyOnSurfEntry aPolyEntry;
            aPolyEntry.Polygon2D       = anEdgeData.PolyOnSurf;
            aPolyEntry.FaceDefId       = BRepGraph_NodeId::Face(aFaceIdx);
            aPolyEntry.EdgeOrientation = TopAbs_FORWARD;
            anEdgeMutPC.PolygonsOnSurf.Append(aPolyEntry);
          }
        }

        // Inline PCurve: reversed (seam).
        if (!anEdgeData.PCurve2dReversed.IsNull())
        {
          BRepGraphInc::EdgeEntity& anEdgeMutPC = theStorage.Edges.ChangeValue(anEdgeIdx);
          BRepGraphInc::EdgeEntity::PCurveEntry aPCEntry;
          aPCEntry.Curve2d         = anEdgeData.PCurve2dReversed;
          aPCEntry.FaceDefId       = BRepGraph_NodeId::Face(aFaceIdx);
          aPCEntry.ParamFirst      = anEdgeData.PCFirstReversed;
          aPCEntry.ParamLast       = anEdgeData.PCLastReversed;
          aPCEntry.Continuity      = anEdgeData.PCurveContinuity;
          aPCEntry.EdgeOrientation = TopAbs_REVERSED;
          anEdgeMutPC.PCurves.Append(aPCEntry);

          if (!anEdgeData.PolyOnSurfReversed.IsNull())
          {
            BRepGraphInc::EdgeEntity::PolyOnSurfEntry aPolyEntry;
            aPolyEntry.Polygon2D       = anEdgeData.PolyOnSurfReversed;
            aPolyEntry.FaceDefId       = BRepGraph_NodeId::Face(aFaceIdx);
            aPolyEntry.EdgeOrientation = TopAbs_REVERSED;
            anEdgeMutPC.PolygonsOnSurf.Append(aPolyEntry);
          }
        }

        // Polygon3D (once per edge).
        if (!anEdgeData.Polygon3D.IsNull())
        {
          BRepGraphInc::EdgeEntity& anEdgeMut = theStorage.Edges.ChangeValue(anEdgeIdx);
          if (anEdgeMut.Polygon3D.IsNull())
            anEdgeMut.Polygon3D = anEdgeData.Polygon3D;
        }

        // Inline PolygonsOnTri: polygon-on-triangulation.
        {
          const BRepGraphInc::FaceEntity& aFaceRef = theStorage.Faces.Value(aFaceIdx);
          for (int aTriIdx = 0; aTriIdx < aFaceRef.Triangulations.Length(); ++aTriIdx)
          {
            const Handle(Poly_Triangulation)& aTri = aFaceRef.Triangulations.Value(aTriIdx);
            if (aTri.IsNull())
              continue;

            TopLoc_Location aPolyTriLoc;
            Handle(Poly_PolygonOnTriangulation) aPolyOnTri =
              BRep_Tool::PolygonOnTriangulation(anEdgeData.Shape, aTri, aPolyTriLoc);
            if (!aPolyOnTri.IsNull())
            {
              BRepGraphInc::EdgeEntity& anEdgeMutTri = theStorage.Edges.ChangeValue(anEdgeIdx);
              BRepGraphInc::EdgeEntity::PolyOnTriEntry aPolyTriEntry;
              aPolyTriEntry.Polygon            = aPolyOnTri;
              aPolyTriEntry.FaceDefId          = BRepGraph_NodeId::Face(aFaceIdx);
              aPolyTriEntry.TriangulationIndex = aTriIdx;
              aPolyTriEntry.EdgeOrientation    = TopAbs_FORWARD;
              anEdgeMutTri.PolygonsOnTri.Append(aPolyTriEntry);

              // Check for seam polygon-on-triangulation.
              TopoDS_Edge aRevEdge = TopoDS::Edge(anEdgeData.Shape.Reversed());
              Handle(Poly_PolygonOnTriangulation) aPolyOnTriRev =
                BRep_Tool::PolygonOnTriangulation(aRevEdge, aTri, aPolyTriLoc);
              if (!aPolyOnTriRev.IsNull() && aPolyOnTriRev != aPolyOnTri)
              {
                BRepGraphInc::EdgeEntity& anEdgeMutTriRev = theStorage.Edges.ChangeValue(anEdgeIdx);
                BRepGraphInc::EdgeEntity::PolyOnTriEntry aPolyTriRevEntry;
                aPolyTriRevEntry.Polygon            = aPolyOnTriRev;
                aPolyTriRevEntry.FaceDefId          = BRepGraph_NodeId::Face(aFaceIdx);
                aPolyTriRevEntry.TriangulationIndex = aTriIdx;
                aPolyTriRevEntry.EdgeOrientation    = TopAbs_REVERSED;
                anEdgeMutTriRev.PolygonsOnTri.Append(aPolyTriRevEntry);
              }
            }
          }
        }

        // Track first/last vertex for closure check.
        if (aIsNewWireDef)
        {
          const BRepGraphInc::EdgeEntity& anEdgeRef = theStorage.Edges.Value(anEdgeIdx);
          if (aFirstVertexIdx < 0)
          {
            aFirstVertexIdx = (anEdgeData.OrientationInWire == TopAbs_FORWARD)
              ? anEdgeRef.StartVertexIdx : anEdgeRef.EndVertexIdx;
          }
          aLastVertexIdx = (anEdgeData.OrientationInWire == TopAbs_FORWARD)
            ? anEdgeRef.EndVertexIdx : anEdgeRef.StartVertexIdx;
        }
      }

      // Set wire closure.
      if (aIsNewWireDef)
      {
        theStorage.Wires.ChangeValue(aWireIdx).IsClosed =
          aFirstVertexIdx >= 0 && aLastVertexIdx >= 0 && aFirstVertexIdx == aLastVertexIdx;
      }
    }
  }
}

} // anonymous namespace

//=================================================================================================

void BRepGraphInc_Populate::Perform(BRepGraphInc_Storage& theStorage,
                                    const TopoDS_Shape&   theShape,
                                    bool                  theParallel)
{
  theStorage.Clear();

  if (theShape.IsNull())
    return;

  // Phase 1 (sequential): Recursively explore hierarchy, collecting face contexts.
  NCollection_Vector<FaceLocalData> aFaceData;

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

        BRepGraphInc::CompoundEntity& aCompEnt = theStorage.Compounds.Appended();
        int aCompIdx = theStorage.Compounds.Length() - 1;
        aCompEnt.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Compound, aCompIdx);
        theStorage.OriginalShapes.Bind(aCompEnt.Id, aCompound);

        const TopLoc_Location aGlobalLoc = theParentGlobalLoc * aCompound.Location();

        for (TopoDS_Iterator aChildIt(aCompound); aChildIt.More(); aChildIt.Next())
        {
          const TopoDS_Shape& aChild = aChildIt.Value();
          int aChildKind = -1;
          int aChildIdx  = -1;

          switch (aChild.ShapeType())
          {
            case TopAbs_COMPOUND:
              aChildKind = static_cast<int>(BRepGraph_NodeId::Kind::Compound);
              aChildIdx  = theStorage.Compounds.Length(); // will be appended
              break;
            case TopAbs_COMPSOLID:
              aChildKind = static_cast<int>(BRepGraph_NodeId::Kind::CompSolid);
              aChildIdx  = theStorage.CompSolids.Length();
              break;
            case TopAbs_SOLID:
              aChildKind = static_cast<int>(BRepGraph_NodeId::Kind::Solid);
              aChildIdx  = theStorage.Solids.Length();
              break;
            case TopAbs_SHELL:
              aChildKind = static_cast<int>(BRepGraph_NodeId::Kind::Shell);
              aChildIdx  = theStorage.Shells.Length();
              break;
            case TopAbs_FACE:
              aChildKind = static_cast<int>(BRepGraph_NodeId::Kind::Face);
              aChildIdx  = -1; // face idx not known yet
              break;
            default:
              break;
          }

          traverseShape(aChild, -1, aGlobalLoc);

          if (aChildKind >= 0)
          {
            BRepGraphInc::ChildRef aRef;
            aRef.Kind          = aChildKind;
            aRef.ChildIdx      = aChildIdx;
            aRef.Orientation   = aChild.Orientation();
            aRef.LocalLocation = aChild.Location();
            theStorage.Compounds.ChangeValue(aCompIdx).ChildRefs.Append(aRef);
          }
        }
        break;
      }

      case TopAbs_COMPSOLID: {
        const TopoDS_CompSolid& aCompSolid = TopoDS::CompSolid(theCurrentShape);

        BRepGraphInc::CompSolidEntity& aCSolidEnt = theStorage.CompSolids.Appended();
        int aCSolidIdx = theStorage.CompSolids.Length() - 1;
        aCSolidEnt.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::CompSolid, aCSolidIdx);
        theStorage.OriginalShapes.Bind(aCSolidEnt.Id, aCompSolid);

        const TopLoc_Location aGlobalLoc = theParentGlobalLoc * aCompSolid.Location();

        for (TopoDS_Iterator aChildIt(aCompSolid); aChildIt.More(); aChildIt.Next())
        {
          if (aChildIt.Value().ShapeType() != TopAbs_SOLID)
            continue;
          int aSolidIdx = theStorage.Solids.Length();
          traverseShape(aChildIt.Value(), -1, aGlobalLoc);

          BRepGraphInc::SolidRef aRef;
          aRef.SolidIdx      = aSolidIdx;
          aRef.Orientation   = aChildIt.Value().Orientation();
          aRef.LocalLocation = aChildIt.Value().Location();
          theStorage.CompSolids.ChangeValue(aCSolidIdx).SolidRefs.Append(aRef);
        }
        break;
      }

      case TopAbs_SOLID: {
        const TopoDS_Solid& aSolid = TopoDS::Solid(theCurrentShape);

        BRepGraphInc::SolidEntity& aSolidEnt = theStorage.Solids.Appended();
        int aSolidIdx = theStorage.Solids.Length() - 1;
        aSolidEnt.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, aSolidIdx);
        theStorage.OriginalShapes.Bind(aSolidEnt.Id, aSolid);

        const TopLoc_Location aGlobalLoc = theParentGlobalLoc * aSolid.Location();

        for (TopoDS_Iterator aShellIt(aSolid); aShellIt.More(); aShellIt.Next())
        {
          if (aShellIt.Value().ShapeType() != TopAbs_SHELL)
            continue;
          int aShellIdx = theStorage.Shells.Length();
          traverseShape(aShellIt.Value(), -1, aGlobalLoc);

          BRepGraphInc::ShellRef aRef;
          aRef.ShellIdx      = aShellIdx;
          aRef.Orientation   = aShellIt.Value().Orientation();
          aRef.LocalLocation = aShellIt.Value().Location();
          theStorage.Solids.ChangeValue(aSolidIdx).ShellRefs.Append(aRef);
        }
        break;
      }

      case TopAbs_SHELL: {
        const TopoDS_Shell& aShell = TopoDS::Shell(theCurrentShape);

        BRepGraphInc::ShellEntity& aShellEnt = theStorage.Shells.Appended();
        int aShellIdx = theStorage.Shells.Length() - 1;
        aShellEnt.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Shell, aShellIdx);
        theStorage.OriginalShapes.Bind(aShellEnt.Id, aShell);

        for (TopoDS_Iterator aFaceIt(aShell); aFaceIt.More(); aFaceIt.Next())
        {
          if (aFaceIt.Value().ShapeType() != TopAbs_FACE)
            continue;
          FaceLocalData& aData   = aFaceData.Appended();
          aData.Face             = TopoDS::Face(aFaceIt.Value());
          aData.ParentGlobalLoc  = theParentGlobalLoc * aShell.Location();
          aData.ParentShellIdx   = aShellIdx;
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

  traverseShape(theShape, -1, TopLoc_Location());

  // Phase 2 (parallel): Extract per-face geometry/topology.
  OSD_Parallel::For(
    0,
    aFaceData.Length(),
    [&](int theIndex) { extractFaceData(aFaceData.ChangeValue(theIndex)); },
    !theParallel);

  // Phase 3 (sequential): Register into storage with deduplication.
  registerFaceData(theStorage, aFaceData);

  // Phase 3a: Fix compound face ChildRefs (face indices were unknown during Phase 1).
  // ChildRefs are in the same order as TopoDS_Iterator over the original compound.
  for (int aCompIdx = 0; aCompIdx < theStorage.Compounds.Length(); ++aCompIdx)
  {
    BRepGraphInc::CompoundEntity& aComp = theStorage.Compounds.ChangeValue(aCompIdx);
    const TopoDS_Shape* aCompOrig = theStorage.OriginalShapes.Seek(aComp.Id);
    if (aCompOrig == nullptr)
      continue;

    int aCRIdx = 0;
    for (TopoDS_Iterator aChildIt(*aCompOrig); aChildIt.More(); aChildIt.Next())
    {
      if (aCRIdx >= aComp.ChildRefs.Length())
        break;
      BRepGraphInc::ChildRef& aCR = aComp.ChildRefs.ChangeValue(aCRIdx);
      if (aCR.Kind == static_cast<int>(BRepGraph_NodeId::Kind::Face) && aCR.ChildIdx < 0)
      {
        const BRepGraph_NodeId* aFaceNodeId =
          theStorage.TShapeToNodeId.Seek(aChildIt.Value().TShape().get());
        if (aFaceNodeId != nullptr
            && aFaceNodeId->NodeKind == BRepGraph_NodeId::Kind::Face)
          aCR.ChildIdx = aFaceNodeId->Index;
      }
      ++aCRIdx;
    }
  }

  // Phase 3b: Edge regularities.
  NCollection_DataMap<const Geom_Surface*, int> aSurfToFaceIdx;
  for (int i = 0; i < theStorage.Faces.Length(); ++i)
  {
    const BRepGraphInc::FaceEntity& aFace = theStorage.Faces.Value(i);
    if (!aFace.Surface.IsNull())
      aSurfToFaceIdx.TryBind(aFace.Surface.get(), i);
  }

  for (int anEdgeIdx = 0; anEdgeIdx < theStorage.Edges.Length(); ++anEdgeIdx)
  {
    BRepGraphInc::EdgeEntity& anEdgeEnt = theStorage.Edges.ChangeValue(anEdgeIdx);

    // Find original shape via OriginalShapes map.
    const TopoDS_Shape* anOrigShape = theStorage.OriginalShapes.Seek(anEdgeEnt.Id);
    if (anOrigShape == nullptr || anOrigShape->IsNull())
      continue;

    const TopoDS_Edge& anEdge = TopoDS::Edge(*anOrigShape);
    const Handle(BRep_TEdge) aTEdge = Handle(BRep_TEdge)::DownCast(anEdge.TShape());
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
  }

  // Phase 3c: Vertex point representations.
  {
    NCollection_DataMap<const Geom_Curve*, BRepGraph_NodeId> aCurveToEdgeDef;
    for (int i = 0; i < theStorage.Edges.Length(); ++i)
    {
      const BRepGraphInc::EdgeEntity& anEdge = theStorage.Edges.Value(i);
      if (!anEdge.Curve3d.IsNull())
        aCurveToEdgeDef.TryBind(anEdge.Curve3d.get(), anEdge.Id);
    }

    NCollection_DataMap<const Geom_Surface*, BRepGraph_NodeId> aSurfToFaceDefVtx;
    for (int i = 0; i < theStorage.Faces.Length(); ++i)
    {
      const BRepGraphInc::FaceEntity& aFace = theStorage.Faces.Value(i);
      if (!aFace.Surface.IsNull())
        aSurfToFaceDefVtx.TryBind(aFace.Surface.get(), aFace.Id);
    }

    for (int aVtxIdx = 0; aVtxIdx < theStorage.Vertices.Length(); ++aVtxIdx)
    {
      BRepGraphInc::VertexEntity& aVtxDef = theStorage.Vertices.ChangeValue(aVtxIdx);
      const TopoDS_Shape* aVtxShape = theStorage.OriginalShapes.Seek(aVtxDef.Id);
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
          BRepGraphInc::VertexEntity::PointOnCurveEntry anEntry;
          anEntry.Parameter = aPOC->Parameter();
          anEntry.EdgeDefId = *anEdgeId;
          aVtxDef.PointsOnCurve.Append(anEntry);
        }
        else if (const Handle(BRep_PointOnCurveOnSurface) aPOCS =
                   Handle(BRep_PointOnCurveOnSurface)::DownCast(aPtRep))
        {
          const BRepGraph_NodeId* aFaceId = aSurfToFaceDefVtx.Seek(aPOCS->Surface().get());
          if (aFaceId == nullptr)
            continue;
          BRepGraphInc::VertexEntity::PointOnPCurveEntry anEntry;
          anEntry.Parameter = aPOCS->Parameter();
          anEntry.FaceDefId = *aFaceId;
          aVtxDef.PointsOnPCurve.Append(anEntry);
        }
        else if (const Handle(BRep_PointOnSurface) aPOS =
                   Handle(BRep_PointOnSurface)::DownCast(aPtRep))
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
    }
  }

  // Phase 4: Build reverse indices.
  theStorage.BuildReverseIndex();

  theStorage.IsDone = true;
}

//=================================================================================================

void BRepGraphInc_Populate::Append(BRepGraphInc_Storage& theStorage,
                                   const TopoDS_Shape&   theShape,
                                   bool                  theParallel)
{
  if (theShape.IsNull())
    return;

  // Collect face contexts by flattening hierarchy.
  NCollection_Vector<FaceLocalData> aFaceData;

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

  // Rebuild reverse indices.
  theStorage.BuildReverseIndex();

  theStorage.IsDone = true;
}
