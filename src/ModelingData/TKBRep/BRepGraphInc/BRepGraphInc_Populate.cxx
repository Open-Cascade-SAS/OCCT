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
#include <BRep_CurveOnClosedSurface.hxx>
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
  double             Tolerance   = 0.0;
  TopAbs_Orientation Orientation = TopAbs_INTERNAL;
};

//! Per-edge data extracted from TopoDS in parallel phase.
struct ExtractedEdge
{
  TopoDS_Edge                                 Shape;
  occ::handle<Geom_Curve>                     Curve3d;
  double                                      ParamFirst    = 0.0;
  double                                      ParamLast     = 0.0;
  double                                      Tolerance     = 0.0;
  bool                                        IsDegenerate  = false;
  bool                                        SameParameter = false;
  bool                                        SameRange     = false;
  ExtractedVertex                             StartVertex;
  ExtractedVertex                             EndVertex;
  NCollection_Vector<ExtractedInternalVertex> InternalVertices;
  TopAbs_Orientation                          OrientationInWire = TopAbs_FORWARD;
  occ::handle<Geom2d_Curve>                   PCurve2d;
  double                                      PCFirst = 0.0;
  double                                      PCLast  = 0.0;
  occ::handle<Geom2d_Curve>                   PCurve2dReversed;
  double                                      PCFirstReversed  = 0.0;
  double                                      PCLastReversed   = 0.0;
  GeomAbs_Shape                               PCurveContinuity = GeomAbs_C0;
  gp_Pnt2d                                    PCUV1;
  gp_Pnt2d                                    PCUV2;
  GeomAbs_Shape                               SeamContinuity = GeomAbs_C0;
  occ::handle<Poly_Polygon3D>                 Polygon3D;
  occ::handle<Poly_Polygon2D>                 PolyOnSurf;
  occ::handle<Poly_Polygon2D>                 PolyOnSurfReversed;
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
  occ::handle<Geom_Surface>                           Surface;
  NCollection_Vector<occ::handle<Poly_Triangulation>> Triangulations;
  int                                                 ActiveTriangulationIndex = -1;
  double                                              Tolerance                = 0.0;
  TopAbs_Orientation                                  Orientation              = TopAbs_FORWARD;
  bool                                                NaturalRestriction       = false;
  NCollection_Vector<ExtractedWire>                   Wires;
  NCollection_Vector<ExtractedInternalVertex> DirectVertices; //!< INTERNAL/EXTERNAL vertex children
};

//! Map TopAbs_ShapeEnum to BRepGraph_NodeId::Kind.
//! Returns -1 for TopAbs_SHAPE (unknown/unhandled).
int shapeTypeToNodeKind(TopAbs_ShapeEnum theType)
{
  switch (theType)
  {
    case TopAbs_COMPOUND:
      return static_cast<int>(BRepGraph_NodeId::Kind::Compound);
    case TopAbs_COMPSOLID:
      return static_cast<int>(BRepGraph_NodeId::Kind::CompSolid);
    case TopAbs_SOLID:
      return static_cast<int>(BRepGraph_NodeId::Kind::Solid);
    case TopAbs_SHELL:
      return static_cast<int>(BRepGraph_NodeId::Kind::Shell);
    case TopAbs_FACE:
      return static_cast<int>(BRepGraph_NodeId::Kind::Face);
    case TopAbs_WIRE:
      return static_cast<int>(BRepGraph_NodeId::Kind::Wire);
    case TopAbs_EDGE:
      return static_cast<int>(BRepGraph_NodeId::Kind::Edge);
    case TopAbs_VERTEX:
      return static_cast<int>(BRepGraph_NodeId::Kind::Vertex);
    default:
      return -1;
  }
}

//! Check if a shape's TShape is already registered in storage with the expected kind.
//! Returns the existing NodeId pointer if found, nullptr otherwise.
const BRepGraph_NodeId* findExistingNode(const BRepGraphInc_Storage& theStorage,
                                         const TopoDS_Shape&         theShape,
                                         BRepGraph_NodeId::Kind      theExpectedKind)
{
  const BRepGraph_NodeId* anExisting = theStorage.FindNodeByTShape(theShape.TShape().get());
  if (anExisting != nullptr && anExisting->NodeKind == theExpectedKind)
    return anExisting;
  return nullptr;
}

//! Register a vertex entity by TShape dedup, or return the existing index.
//! @param[in,out] theStorage   incidence storage
//! @param[in]     theVertex    original TopoDS_Vertex
//! @param[in]     thePoint     3D point (pre-extracted)
//! @param[in]     theTolerance vertex tolerance (pre-extracted)
//! @return index into the vertex entity vector, or -1 if theVertex is null
int registerOrReuseVertex(BRepGraphInc_Storage& theStorage,
                          const TopoDS_Vertex&  theVertex,
                          const gp_Pnt&         thePoint,
                          double                theTolerance)
{
  if (theVertex.IsNull())
    return -1;
  const BRepGraph_NodeId* anExisting =
    findExistingNode(theStorage, theVertex, BRepGraph_NodeId::Kind::Vertex);
  if (anExisting != nullptr)
    return anExisting->Index;

  BRepGraphInc::VertexEntity& aVtxEnt = theStorage.AppendVertex();
  int                         aVtxIdx = theStorage.NbVertices() - 1;
  aVtxEnt.Id                          = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Vertex, aVtxIdx);
  aVtxEnt.Point                       = thePoint;
  aVtxEnt.Tolerance                   = theTolerance;
  theStorage.BindTShapeToNode(theVertex.TShape().get(), aVtxEnt.Id);
  theStorage.BindOriginal(aVtxEnt.Id, theVertex);
  return aVtxIdx;
}

//! Convenience overload: extracts point and tolerance from the vertex.
int registerOrReuseVertex(BRepGraphInc_Storage& theStorage, const TopoDS_Vertex& theVertex)
{
  if (theVertex.IsNull())
    return -1;
  return registerOrReuseVertex(theStorage,
                               theVertex,
                               BRep_Tool::Pnt(theVertex),
                               BRep_Tool::Tolerance(theVertex));
}

//! Factor representation location from combined location and apply to geometry.
//! Returns the transformed geometry if the representation location is non-identity,
//! otherwise returns the original unchanged.
//! @tparam T  Geom_Surface or Geom_Curve (must support Transformed())
template <typename T>
occ::handle<T> applyRepresentationLocation(const occ::handle<T>&  theGeom,
                                           const TopLoc_Location& theShapeLoc,
                                           const TopLoc_Location& theCombinedLoc)
{
  if (theCombinedLoc.IsIdentity() || theGeom.IsNull())
    return theGeom;
  const TopLoc_Location aRepLoc = theShapeLoc.Inverted() * theCombinedLoc;
  if (aRepLoc.IsIdentity())
    return theGeom;
  return occ::down_cast<T>(theGeom->Transformed(aRepLoc.Transformation()));
}

//! Apply representation location to a Polygon3D by transforming its nodes.
occ::handle<Poly_Polygon3D> applyRepLocationToPolygon3D(
  const occ::handle<Poly_Polygon3D>& thePolygon3D,
  const TopLoc_Location&             theShapeLoc,
  const TopLoc_Location&             theCombinedLoc)
{
  if (theCombinedLoc.IsIdentity() || thePolygon3D.IsNull())
    return thePolygon3D;
  const TopLoc_Location aRepLoc = theShapeLoc.Inverted() * theCombinedLoc;
  if (aRepLoc.IsIdentity())
    return thePolygon3D;

  const gp_Trsf&                    aTrsf  = aRepLoc.Transformation();
  const NCollection_Array1<gp_Pnt>& aNodes = thePolygon3D->Nodes();
  NCollection_Array1<gp_Pnt>        aNewNodes(aNodes.Lower(), aNodes.Upper());
  for (int aNodeIdx = aNodes.Lower(); aNodeIdx <= aNodes.Upper(); ++aNodeIdx)
    aNewNodes.SetValue(aNodeIdx, aNodes.Value(aNodeIdx).Transformed(aTrsf));
  occ::handle<Poly_Polygon3D> aTransPoly;
  if (thePolygon3D->HasParameters())
    aTransPoly = new Poly_Polygon3D(aNewNodes, thePolygon3D->Parameters());
  else
    aTransPoly = new Poly_Polygon3D(aNewNodes);
  aTransPoly->Deflection(thePolygon3D->Deflection());
  return aTransPoly;
}

//! Append a PCurve entry and optional PolyOnSurf entry to an edge entity.
//! Does nothing if theCurve2d is null.
//! For reversed seam entries, theUV1/theUV2 are typically default (0,0)
//! because BRep_Tool::UVPoints is only called for the forward orientation.
//! Reconstruction reads UVs only from the forward-matching entry.
void appendPCurveAndPolyEntry(BRepGraphInc::EdgeEntity&          theEdge,
                              const occ::handle<Geom2d_Curve>&   theCurve2d,
                              int                                theFaceIdx,
                              double                             theFirst,
                              double                             theLast,
                              GeomAbs_Shape                      theContinuity,
                              const gp_Pnt2d&                    theUV1,
                              const gp_Pnt2d&                    theUV2,
                              TopAbs_Orientation                 theEdgeOri,
                              const occ::handle<Poly_Polygon2D>& thePolyOnSurf)
{
  if (theCurve2d.IsNull())
    return;

  BRepGraphInc::EdgeEntity::PCurveEntry aPCEntry;
  aPCEntry.Curve2d         = theCurve2d;
  aPCEntry.FaceDefId       = BRepGraph_NodeId::Face(theFaceIdx);
  aPCEntry.ParamFirst      = theFirst;
  aPCEntry.ParamLast       = theLast;
  aPCEntry.Continuity      = theContinuity;
  aPCEntry.UV1             = theUV1;
  aPCEntry.UV2             = theUV2;
  aPCEntry.EdgeOrientation = theEdgeOri;
  theEdge.PCurves.Append(aPCEntry);

  if (!thePolyOnSurf.IsNull())
  {
    BRepGraphInc::EdgeEntity::PolyOnSurfEntry aPolyEntry;
    aPolyEntry.Polygon2D       = thePolyOnSurf;
    aPolyEntry.FaceDefId       = BRepGraph_NodeId::Face(theFaceIdx);
    aPolyEntry.EdgeOrientation = theEdgeOri;
    theEdge.PolygonsOnSurf.Append(aPolyEntry);
  }
}

//! Append polygon-on-triangulation entries for one edge in one face context.
//! Handles transformed triangulation copies and seam detection.
void appendPolyOnTriEntries(BRepGraphInc_Storage&     theStorage,
                            BRepGraphInc::EdgeEntity& theEdge,
                            const TopoDS_Edge&        theEdgeShape,
                            int                       theFaceIdx,
                            int                       theNbOrigTris)
{
  const BRepGraphInc::FaceEntity& aFaceDef = theStorage.Face(theFaceIdx);
  for (int aTriIdx = 0; aTriIdx < theNbOrigTris; ++aTriIdx)
  {
    const occ::handle<Poly_Triangulation>& aTri = aFaceDef.Triangulations.Value(aTriIdx);
    if (aTri.IsNull())
      continue;

    TopLoc_Location                          aPolyTriLoc;
    occ::handle<Poly_PolygonOnTriangulation> aPolyOnTri =
      BRep_Tool::PolygonOnTriangulation(theEdgeShape, aTri, aPolyTriLoc);
    if (aPolyOnTri.IsNull())
      continue;

    int aEffTriIdx = aTriIdx;
    if (!aPolyTriLoc.IsIdentity())
    {
      const TopLoc_Location aRepLoc = theEdgeShape.Location().Inverted() * aPolyTriLoc;
      if (!aRepLoc.IsIdentity())
      {
        occ::handle<Poly_Triangulation> aTriCopy = aTri->Copy();
        const gp_Trsf&                  aTrsf    = aRepLoc.Transformation();
        for (int aNodeIdx = 1; aNodeIdx <= aTriCopy->NbNodes(); ++aNodeIdx)
          aTriCopy->SetNode(aNodeIdx, aTriCopy->Node(aNodeIdx).Transformed(aTrsf));
        aEffTriIdx = theStorage.ChangeFace(theFaceIdx).Triangulations.Length();
        theStorage.ChangeFace(theFaceIdx).Triangulations.Append(aTriCopy);
      }
    }

    BRepGraphInc::EdgeEntity::PolyOnTriEntry aPolyTriEntry;
    aPolyTriEntry.Polygon            = aPolyOnTri;
    aPolyTriEntry.FaceDefId          = BRepGraph_NodeId::Face(theFaceIdx);
    aPolyTriEntry.TriangulationIndex = aEffTriIdx;
    aPolyTriEntry.EdgeOrientation    = TopAbs_FORWARD;
    theEdge.PolygonsOnTri.Append(aPolyTriEntry);

    // Check for seam polygon-on-triangulation.
    TopoDS_Edge                              aRevEdge = TopoDS::Edge(theEdgeShape.Reversed());
    occ::handle<Poly_PolygonOnTriangulation> aPolyOnTriRev =
      BRep_Tool::PolygonOnTriangulation(aRevEdge, aTri, aPolyTriLoc);
    if (!aPolyOnTriRev.IsNull() && aPolyOnTriRev != aPolyOnTri)
    {
      BRepGraphInc::EdgeEntity::PolyOnTriEntry aPolyTriRevEntry;
      aPolyTriRevEntry.Polygon            = aPolyOnTriRev;
      aPolyTriRevEntry.FaceDefId          = BRepGraph_NodeId::Face(theFaceIdx);
      aPolyTriRevEntry.TriangulationIndex = aEffTriIdx;
      aPolyTriRevEntry.EdgeOrientation    = TopAbs_REVERSED;
      theEdge.PolygonsOnTri.Append(aPolyTriRevEntry);
    }
  }
}

//! Register an edge entity from pre-extracted data, with TShape dedup.
//! Creates the entity (with vertices) if new, returns the index in both cases.
int registerExtractedEdge(BRepGraphInc_Storage& theStorage, const ExtractedEdge& theEdgeData)
{
  const BRepGraph_NodeId* anExisting =
    findExistingNode(theStorage, theEdgeData.Shape, BRepGraph_NodeId::Kind::Edge);
  if (anExisting != nullptr)
    return anExisting->Index;

  BRepGraphInc::EdgeEntity& anEdgeEnt = theStorage.AppendEdge();
  int                       anEdgeIdx = theStorage.NbEdges() - 1;
  anEdgeEnt.Id                        = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, anEdgeIdx);
  anEdgeEnt.Tolerance                 = theEdgeData.Tolerance;
  anEdgeEnt.IsDegenerate              = theEdgeData.IsDegenerate;
  anEdgeEnt.SameParameter             = theEdgeData.SameParameter;
  anEdgeEnt.SameRange                 = theEdgeData.SameRange;
  anEdgeEnt.ParamFirst                = theEdgeData.ParamFirst;
  anEdgeEnt.ParamLast                 = theEdgeData.ParamLast;

  if (!theEdgeData.Curve3d.IsNull())
    anEdgeEnt.Curve3d = theEdgeData.Curve3d;

  // Vertex registration.
  anEdgeEnt.StartVertexIdx = registerOrReuseVertex(theStorage,
                                                   theEdgeData.StartVertex.Shape,
                                                   theEdgeData.StartVertex.Point,
                                                   theEdgeData.StartVertex.Tolerance);
  anEdgeEnt.EndVertexIdx   = registerOrReuseVertex(theStorage,
                                                 theEdgeData.EndVertex.Shape,
                                                 theEdgeData.EndVertex.Point,
                                                 theEdgeData.EndVertex.Tolerance);

  // Register internal/external vertices.
  for (int anIntIdx = 0; anIntIdx < theEdgeData.InternalVertices.Length(); ++anIntIdx)
  {
    const ExtractedInternalVertex& anIntVtx = theEdgeData.InternalVertices.Value(anIntIdx);
    int                            anIntVtxIdx =
      registerOrReuseVertex(theStorage, anIntVtx.Shape, anIntVtx.Point, anIntVtx.Tolerance);
    if (anIntVtxIdx >= 0)
    {
      BRepGraphInc::VertexRef aVR;
      aVR.VertexIdx   = anIntVtxIdx;
      aVR.Orientation = anIntVtx.Orientation;
      anEdgeEnt.InternalVertices.Append(aVR);
    }
  }

  theStorage.BindTShapeToNode(theEdgeData.Shape.TShape().get(), anEdgeEnt.Id);
  theStorage.BindOriginal(anEdgeEnt.Id, theEdgeData.Shape);
  return anEdgeIdx;
}

//! Create a ChildRef from a child shape, resolving its index via TShape lookup.
//! Returns true if the ref was created successfully (child was found in storage).
bool makeFreeChildRef(const BRepGraphInc_Storage& theStorage,
                      const TopoDS_Shape&         theChild,
                      BRepGraphInc::ChildRef&     theRef)
{
  const BRepGraph_NodeId* aChildNodeId = theStorage.FindNodeByTShape(theChild.TShape().get());
  if (aChildNodeId == nullptr)
    return false;
  theRef.Kind          = static_cast<int>(aChildNodeId->NodeKind);
  theRef.ChildIdx      = aChildNodeId->Index;
  theRef.Orientation   = theChild.Orientation();
  theRef.LocalLocation = theChild.Location();
  return true;
}

//! Extract first, last, and internal/external vertices from an edge.
void edgeVertices(const TopoDS_Edge&                           theEdge,
                  TopoDS_Vertex&                               theFirst,
                  TopoDS_Vertex&                               theLast,
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

//! Extract edge geometry and parametric data in a face context.
//! Fills theEdgeData with 3D curve, vertices, PCurves, and polygons.
void extractEdgeInFace(ExtractedEdge&                    theEdgeData,
                       const TopoDS_Edge&                theEdge,
                       const TopoDS_Face&                theForwardFace,
                       const occ::handle<Geom_Surface>&  theFaceSurface)
{
  theEdgeData.Shape             = theEdge;
  theEdgeData.Tolerance         = BRep_Tool::Tolerance(theEdge);
  theEdgeData.IsDegenerate      = BRep_Tool::Degenerated(theEdge);
  theEdgeData.SameParameter     = BRep_Tool::SameParameter(theEdge);
  theEdgeData.SameRange         = BRep_Tool::SameRange(theEdge);
  theEdgeData.OrientationInWire = theEdge.Orientation();

  // 3D curve with representation location applied to definition frame.
  {
    double          aFirst = 0.0, aLast = 0.0;
    TopLoc_Location aCurveCombinedLoc;
    theEdgeData.Curve3d    = BRep_Tool::Curve(theEdge, aCurveCombinedLoc, aFirst, aLast);
    theEdgeData.ParamFirst = aFirst;
    theEdgeData.ParamLast  = aLast;
    theEdgeData.Curve3d    = applyRepresentationLocation<Geom_Curve>(
      theEdgeData.Curve3d, theEdge.Location(), aCurveCombinedLoc);
  }

  // Vertices: use FORWARD-oriented edge for orientation-independent extraction.
  TopoDS_Vertex aVFirst, aVLast;
  {
    const TopoDS_Edge aFwdEdge = TopoDS::Edge(theEdge.Oriented(TopAbs_FORWARD));
    edgeVertices(aFwdEdge, aVFirst, aVLast, theEdgeData.InternalVertices);
  }

  if (!aVFirst.IsNull())
  {
    theEdgeData.StartVertex.Shape     = aVFirst;
    theEdgeData.StartVertex.Point     = BRep_Tool::Pnt(aVFirst);
    theEdgeData.StartVertex.Tolerance = BRep_Tool::Tolerance(aVFirst);
  }
  if (!aVLast.IsNull())
  {
    theEdgeData.EndVertex.Shape     = aVLast;
    theEdgeData.EndVertex.Point     = BRep_Tool::Pnt(aVLast);
    theEdgeData.EndVertex.Tolerance = BRep_Tool::Tolerance(aVLast);
  }

  // Extract PCurves using the edge always oriented FORWARD to ensure
  // orientation-independent PCurve pair ordering. BRep_Tool::CurveOnSurface
  // returns different PCurves depending on edge orientation for seam edges.
  {
    const TopoDS_Edge aFwdEdge = TopoDS::Edge(theEdge.Oriented(TopAbs_FORWARD));
    double aPCFirst = 0.0, aPCLast = 0.0;
    theEdgeData.PCurve2d = BRep_Tool::CurveOnSurface(aFwdEdge, theForwardFace, aPCFirst, aPCLast);
    theEdgeData.PCFirst  = aPCFirst;
    theEdgeData.PCLast   = aPCLast;
    theEdgeData.PCurveContinuity = BRep_Tool::MaxContinuity(theEdge);

    if (!theEdgeData.PCurve2d.IsNull() && !theFaceSurface.IsNull())
      BRep_Tool::UVPoints(aFwdEdge, theForwardFace, theEdgeData.PCUV1, theEdgeData.PCUV2);

    // Second PCurve for seam edges + seam continuity.
    if (!theEdgeData.PCurve2d.IsNull() && BRep_Tool::IsClosed(aFwdEdge, theForwardFace))
    {
      double aPCFirstRev = 0.0, aPCLastRev = 0.0;
      const TopoDS_Edge aRevEdge = TopoDS::Edge(theEdge.Oriented(TopAbs_REVERSED));
      theEdgeData.PCurve2dReversed =
        BRep_Tool::CurveOnSurface(aRevEdge, theForwardFace, aPCFirstRev, aPCLastRev);
      theEdgeData.PCFirstReversed = aPCFirstRev;
      theEdgeData.PCLastReversed  = aPCLastRev;

      // Extract seam continuity from BRep_CurveOnClosedSurface.
      const occ::handle<BRep_TEdge>& aTEdge =
        occ::down_cast<BRep_TEdge>(theEdge.TShape());
      if (!aTEdge.IsNull())
      {
        TopLoc_Location aFaceLoc;
        const occ::handle<Geom_Surface>& aFaceSurf =
          BRep_Tool::Surface(theForwardFace, aFaceLoc);
        const TopLoc_Location aEdgeLoc = aFaceLoc.Predivided(theEdge.Location());
        for (const occ::handle<BRep_CurveRepresentation>& aCRep : aTEdge->Curves())
        {
          if (!aCRep.IsNull() && aCRep->IsCurveOnClosedSurface()
              && aCRep->IsCurveOnSurface(aFaceSurf, aEdgeLoc))
          {
            theEdgeData.SeamContinuity =
              occ::down_cast<BRep_CurveOnClosedSurface>(aCRep)->Continuity();
            break;
          }
        }
      }
    }
  }

  // Polygon3D with representation location applied.
  {
    TopLoc_Location aPoly3DLoc;
    theEdgeData.Polygon3D = BRep_Tool::Polygon3D(theEdge, aPoly3DLoc);
    theEdgeData.Polygon3D =
      applyRepLocationToPolygon3D(theEdgeData.Polygon3D, theEdge.Location(), aPoly3DLoc);
  }

  // PolygonOnSurface: use FORWARD-oriented edge for consistent ordering.
  {
    const TopoDS_Edge aFwdEdge = TopoDS::Edge(theEdge.Oriented(TopAbs_FORWARD));
    theEdgeData.PolyOnSurf = BRep_Tool::PolygonOnSurface(aFwdEdge, theForwardFace);
    if (!theEdgeData.PolyOnSurf.IsNull())
    {
      const TopoDS_Edge aRevEdge = TopoDS::Edge(theEdge.Oriented(TopAbs_REVERSED));
      occ::handle<Poly_Polygon2D> aPolyOnSurfRev =
        BRep_Tool::PolygonOnSurface(aRevEdge, theForwardFace);
      if (!aPolyOnSurfRev.IsNull() && aPolyOnSurfRev != theEdgeData.PolyOnSurf)
        theEdgeData.PolyOnSurfReversed = aPolyOnSurfRev;
    }
  }
}

//! Extract per-face geometry/topology data from TopoDS.
void extractFaceData(FaceLocalData& theData)
{
  const TopoDS_Face& aFace = theData.Face;

  // Extract surface with representation location applied to definition frame.
  {
    TopLoc_Location aSurfCombinedLoc;
    theData.Surface = BRep_Tool::Surface(aFace, aSurfCombinedLoc);
    theData.Surface = applyRepresentationLocation<Geom_Surface>(theData.Surface,
                                                                aFace.Location(),
                                                                aSurfCombinedLoc);
  }

  // Extract triangulations.
  {
    TopLoc_Location                                          aTriLoc;
    const NCollection_List<occ::handle<Poly_Triangulation>>& aTriList =
      BRep_Tool::Triangulations(aFace, aTriLoc);
    occ::handle<Poly_Triangulation> anActiveTri;
    {
      TopLoc_Location aDummyLoc;
      anActiveTri = BRep_Tool::Triangulation(aFace, aDummyLoc);
    }
    int aTriIdx = 0;
    for (NCollection_List<occ::handle<Poly_Triangulation>>::Iterator aTriIt(aTriList);
         aTriIt.More();
         aTriIt.Next(), ++aTriIdx)
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
      const TopoDS_Vertex&    aVertex = TopoDS::Vertex(aChildIt.Value());
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

    for (TopoDS_Iterator anEdgeIt(aWire, false, false); anEdgeIt.More(); anEdgeIt.Next())
    {
      if (anEdgeIt.Value().ShapeType() != TopAbs_EDGE)
        continue;
      ExtractedEdge anEdgeData;
      extractEdgeInFace(anEdgeData, TopoDS::Edge(anEdgeIt.Value()),
                        aForwardFace, theData.Surface);
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
    const BRepGraph_NodeId* anExistingFace =
      findExistingNode(theStorage, aCurFace, BRepGraph_NodeId::Kind::Face);

    int aFaceIdx = -1;
    if (anExistingFace != nullptr)
    {
      aFaceIdx = anExistingFace->Index;
    }
    else
    {
      BRepGraphInc::FaceEntity& aFace = theStorage.AppendFace();
      aFaceIdx                        = theStorage.NbFaces() - 1;
      aFace.Id                        = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, aFaceIdx);
      aFace.Tolerance                 = aData.Tolerance;
      aFace.NaturalRestriction        = aData.NaturalRestriction;
      aFace.Surface                   = aData.Surface;

      for (int aTriIdx = 0; aTriIdx < aData.Triangulations.Length(); ++aTriIdx)
        aFace.Triangulations.Append(aData.Triangulations.Value(aTriIdx));
      aFace.ActiveTriangulationIndex = aData.ActiveTriangulationIndex;

      theStorage.BindTShapeToNode(aCurFace.TShape().get(), aFace.Id);
      theStorage.BindOriginal(aFace.Id, aCurFace);
    }

    // Link face to parent shell (with per-instance location for shared TShapes).
    if (aData.ParentShellIdx >= 0)
    {
      BRepGraphInc::FaceRef aRef;
      aRef.FaceIdx       = aFaceIdx;
      aRef.Orientation   = aData.Orientation;
      aRef.LocalLocation = aCurFace.Location();
      theStorage.ChangeShell(aData.ParentShellIdx).FaceRefs.Append(aRef);
    }

    // Pre-fetch face entity for triangulation access in edge loop.
    const BRepGraphInc::FaceEntity& aFaceDef = theStorage.Face(aFaceIdx);

    // Process wires.
    for (int aWireIter = 0; aWireIter < aData.Wires.Length(); ++aWireIter)
    {
      const ExtractedWire& aWireData = aData.Wires.Value(aWireIter);

      // Dedup wire by TShape.
      const BRepGraph_NodeId* anExistingWire =
        findExistingNode(theStorage, aWireData.Shape, BRepGraph_NodeId::Kind::Wire);

      int  aWireIdx      = -1;
      bool aIsNewWireDef = false;

      if (anExistingWire != nullptr)
      {
        aWireIdx = anExistingWire->Index;
      }
      else
      {
        BRepGraphInc::WireEntity& aWire = theStorage.AppendWire();
        aWireIdx                        = theStorage.NbWires() - 1;
        aWire.Id                        = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Wire, aWireIdx);
        theStorage.BindTShapeToNode(aWireData.Shape.TShape().get(), aWire.Id);
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
        int                  anEdgeIdx  = registerExtractedEdge(theStorage, anEdgeData);

        // Cache mutable edge reference for subsequent PCurve/Polygon appends.
        // Safe: no new edges are appended within this scope.
        BRepGraphInc::EdgeEntity& anEdgeMut = theStorage.ChangeEdge(anEdgeIdx);

        // Create CoEdge entity for this edge-face binding and add CoEdgeRef to wire.
        int aFwdCoEdgeIdx  = -1;
        int aSeamCoEdgeIdx = -1;
        if (aIsNewWireDef)
        {
          // Create the forward CoEdge.
          BRepGraphInc::CoEdgeEntity& aCoEdge = theStorage.AppendCoEdge();
          aFwdCoEdgeIdx = theStorage.NbCoEdges() - 1;
          const int aCoEdgeIdx = aFwdCoEdgeIdx;
          aCoEdge.Id        = BRepGraph_NodeId::CoEdge(aCoEdgeIdx);
          aCoEdge.EdgeIdx   = anEdgeIdx;
          aCoEdge.FaceDefId = BRepGraph_NodeId::Face(aFaceIdx);
          aCoEdge.Sense     = anEdgeData.OrientationInWire;

          // Populate CoEdge with PCurve and polygon data for this face context.
          if (!anEdgeData.PCurve2d.IsNull())
          {
            aCoEdge.Curve2d    = anEdgeData.PCurve2d;
            aCoEdge.ParamFirst = anEdgeData.PCFirst;
            aCoEdge.ParamLast  = anEdgeData.PCLast;
            aCoEdge.Continuity = anEdgeData.PCurveContinuity;
            aCoEdge.UV1        = anEdgeData.PCUV1;
            aCoEdge.UV2        = anEdgeData.PCUV2;
          }
          aCoEdge.PolygonOnSurf = anEdgeData.PolyOnSurf;

          // Handle seam edge: create a second CoEdge for the reversed sense.
          if (!anEdgeData.PCurve2dReversed.IsNull())
          {
            BRepGraphInc::CoEdgeEntity& aSeamCoEdge = theStorage.AppendCoEdge();
            aSeamCoEdgeIdx = theStorage.NbCoEdges() - 1;
            aSeamCoEdge.Id        = BRepGraph_NodeId::CoEdge(aSeamCoEdgeIdx);
            aSeamCoEdge.EdgeIdx   = anEdgeIdx;
            aSeamCoEdge.FaceDefId = BRepGraph_NodeId::Face(aFaceIdx);
            aSeamCoEdge.Sense     = TopAbs_REVERSED;
            aSeamCoEdge.Curve2d    = anEdgeData.PCurve2dReversed;
            aSeamCoEdge.ParamFirst = anEdgeData.PCFirstReversed;
            aSeamCoEdge.ParamLast  = anEdgeData.PCLastReversed;
            aSeamCoEdge.Continuity = anEdgeData.PCurveContinuity;
            aSeamCoEdge.SeamContinuity = anEdgeData.SeamContinuity;
            aSeamCoEdge.PolygonOnSurf = anEdgeData.PolyOnSurfReversed;

            // Link seam pair.
            // Note: aCoEdge ref may be invalidated by AppendCoEdge, re-fetch.
            theStorage.ChangeCoEdge(aCoEdgeIdx).SeamPairIdx = aSeamCoEdgeIdx;
            aSeamCoEdge.SeamPairIdx = aCoEdgeIdx;
          }

          // Add CoEdgeRef to wire.
          BRepGraphInc::CoEdgeRef aCoEdgeRef;
          aCoEdgeRef.CoEdgeIdx = aCoEdgeIdx;
          theStorage.ChangeWire(aWireIdx).CoEdgeRefs.Append(aCoEdgeRef);

          // For seam edges, add the reversed coedge ref too.
          if (aSeamCoEdgeIdx >= 0)
          {
            BRepGraphInc::CoEdgeRef aSeamRef;
            aSeamRef.CoEdgeIdx = aSeamCoEdgeIdx;
            // Note: seam coedge is NOT added to wire CoEdgeRefs — it shares the same wire position
            // as the forward coedge. The seam pair is accessible via SeamPairIdx.
            // Only the forward coedge ref is in the wire's ordered list.
            (void)aSeamRef;
          }
        }

        // PCurve: first (always stored as FORWARD, orientation-independent).
        // Keep EdgeEntity.PCurves populated for backward compatibility (Phase 4 removal).
        appendPCurveAndPolyEntry(anEdgeMut,
                                 anEdgeData.PCurve2d,
                                 aFaceIdx,
                                 anEdgeData.PCFirst,
                                 anEdgeData.PCLast,
                                 anEdgeData.PCurveContinuity,
                                 anEdgeData.PCUV1,
                                 anEdgeData.PCUV2,
                                 TopAbs_FORWARD,
                                 anEdgeData.PolyOnSurf);

        // PCurve: second (seam, always stored as REVERSED).
        appendPCurveAndPolyEntry(anEdgeMut,
                                 anEdgeData.PCurve2dReversed,
                                 aFaceIdx,
                                 anEdgeData.PCFirstReversed,
                                 anEdgeData.PCLastReversed,
                                 anEdgeData.PCurveContinuity,
                                 gp_Pnt2d(),
                                 gp_Pnt2d(),
                                 TopAbs_REVERSED,
                                 anEdgeData.PolyOnSurfReversed);

        // Store seam continuity on the reversed entry.
        if (!anEdgeData.PCurve2dReversed.IsNull()
            && anEdgeData.SeamContinuity != GeomAbs_C0
            && !anEdgeMut.PCurves.IsEmpty())
        {
          anEdgeMut.PCurves.ChangeValue(anEdgeMut.PCurves.Length() - 1).SeamContinuity =
            anEdgeData.SeamContinuity;
        }

        // Polygon3D (once per edge).
        if (!anEdgeData.Polygon3D.IsNull() && anEdgeMut.Polygon3D.IsNull())
          anEdgeMut.Polygon3D = anEdgeData.Polygon3D;

        // Polygon-on-triangulation (forward + seam reversed).
        appendPolyOnTriEntries(theStorage,
                               anEdgeMut,
                               anEdgeData.Shape,
                               aFaceIdx,
                               aFaceDef.Triangulations.Length());

        // Copy PolyOnTri entries matching this face from EdgeEntity to CoEdge.
        if (aFwdCoEdgeIdx >= 0)
        {
          for (int aPTIdx = 0; aPTIdx < anEdgeMut.PolygonsOnTri.Length(); ++aPTIdx)
          {
            const BRepGraphInc::EdgeEntity::PolyOnTriEntry& aPTEntry =
              anEdgeMut.PolygonsOnTri.Value(aPTIdx);
            if (aPTEntry.FaceDefId.Index != aFaceIdx)
              continue;

            BRepGraphInc::CoEdgeEntity::PolyOnTriEntry aCoEdgePTEntry;
            aCoEdgePTEntry.Polygon            = aPTEntry.Polygon;
            aCoEdgePTEntry.TriangulationIndex = aPTEntry.TriangulationIndex;

            if (aPTEntry.EdgeOrientation == TopAbs_FORWARD)
              theStorage.ChangeCoEdge(aFwdCoEdgeIdx).PolygonsOnTri.Append(aCoEdgePTEntry);
            else if (aSeamCoEdgeIdx >= 0)
              theStorage.ChangeCoEdge(aSeamCoEdgeIdx).PolygonsOnTri.Append(aCoEdgePTEntry);
          }
        }

      }

      // Wire closure: copy directly from the original shape.
      if (aIsNewWireDef)
      {
        theStorage.ChangeWire(aWireIdx).IsClosed = aWireData.Shape.Closed();
      }
    }

    // Register direct vertex children of the face (INTERNAL/EXTERNAL).
    for (int aDVIdx = 0; aDVIdx < aData.DirectVertices.Length(); ++aDVIdx)
    {
      const ExtractedInternalVertex& aDirVtx = aData.DirectVertices.Value(aDVIdx);
      int                            aVtxIdx =
        registerOrReuseVertex(theStorage, aDirVtx.Shape, aDirVtx.Point, aDirVtx.Tolerance);
      if (aVtxIdx >= 0)
      {
        BRepGraphInc::VertexRef aVR;
        aVR.VertexIdx   = aVtxIdx;
        aVR.Orientation = aDirVtx.Orientation;
        theStorage.ChangeFace(aFaceIdx).VertexRefs.Append(aVR);
      }
    }
  }
}

} // anonymous namespace

//=================================================================================================

void BRepGraphInc_Populate::Perform(BRepGraphInc_Storage&                         theStorage,
                                    const TopoDS_Shape&                           theShape,
                                    bool                                          theParallel,
                                    const Options&                                theOptions,
                                    const occ::handle<NCollection_BaseAllocator>& theTmpAlloc)
{
  theStorage.Clear();

  if (theShape.IsNull())
    return;

  // Use temporary allocator if provided, else default.
  // Must NOT use the storage's persistent allocator for scratch data.
  const occ::handle<NCollection_BaseAllocator>& aTmpAlloc =
    !theTmpAlloc.IsNull() ? theTmpAlloc : NCollection_BaseAllocator::CommonBaseAllocator();

  // Phase 1 (sequential): Recursively explore hierarchy, collecting face contexts.
  NCollection_Vector<FaceLocalData> aFaceData(256, aTmpAlloc);

  std::function<void(const TopoDS_Shape&, int, const TopLoc_Location&)> traverseShape;

  traverseShape = [&](const TopoDS_Shape& theCurrentShape,
                      int /*theParentContainerIdx*/,
                      const TopLoc_Location& theParentGlobalLoc) {
    if (theCurrentShape.IsNull())
      return;

    switch (theCurrentShape.ShapeType())
    {
      case TopAbs_COMPOUND: {
        const TopoDS_Compound& aCompound = TopoDS::Compound(theCurrentShape);
        if (findExistingNode(theStorage, aCompound, BRepGraph_NodeId::Kind::Compound))
          break;

        BRepGraphInc::CompoundEntity& aCompEnt = theStorage.myCompounds.Appended();
        int                           aCompIdx = theStorage.myCompounds.Length() - 1;
        aCompEnt.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Compound, aCompIdx);
        theStorage.BindTShapeToNode(aCompound.TShape().get(), aCompEnt.Id);
        theStorage.BindOriginal(aCompEnt.Id, aCompound);

        const TopLoc_Location aGlobalLoc = theParentGlobalLoc * aCompound.Location();

        for (TopoDS_Iterator aChildIt(aCompound); aChildIt.More(); aChildIt.Next())
        {
          const TopoDS_Shape& aChild     = aChildIt.Value();
          int                 aChildKind = shapeTypeToNodeKind(aChild.ShapeType());

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
        if (findExistingNode(theStorage, aCompSolid, BRepGraph_NodeId::Kind::CompSolid))
          break;

        BRepGraphInc::CompSolidEntity& aCSolidEnt = theStorage.myCompSolids.Appended();
        int                            aCSolidIdx = theStorage.myCompSolids.Length() - 1;
        aCSolidEnt.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::CompSolid, aCSolidIdx);
        theStorage.BindTShapeToNode(aCompSolid.TShape().get(), aCSolidEnt.Id);
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
        if (findExistingNode(theStorage, aSolid, BRepGraph_NodeId::Kind::Solid))
          break;

        BRepGraphInc::SolidEntity& aSolidEnt = theStorage.mySolids.Appended();
        int                        aSolidIdx = theStorage.mySolids.Length() - 1;
        aSolidEnt.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, aSolidIdx);
        theStorage.BindTShapeToNode(aSolid.TShape().get(), aSolidEnt.Id);
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
          else if (aChild.ShapeType() == TopAbs_EDGE || aChild.ShapeType() == TopAbs_VERTEX)
          {
            BRepGraphInc::ChildRef aCR;
            if (makeFreeChildRef(theStorage, aChild, aCR))
              theStorage.mySolids.ChangeValue(aSolidIdx).FreeChildRefs.Append(aCR);
          }
        }
        break;
      }

      case TopAbs_SHELL: {
        const TopoDS_Shell& aShell = TopoDS::Shell(theCurrentShape);
        if (findExistingNode(theStorage, aShell, BRepGraph_NodeId::Kind::Shell))
          break;

        BRepGraphInc::ShellEntity& aShellEnt = theStorage.myShells.Appended();
        int                        aShellIdx = theStorage.myShells.Length() - 1;
        aShellEnt.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Shell, aShellIdx);
        theStorage.BindTShapeToNode(aShell.TShape().get(), aShellEnt.Id);
        theStorage.BindOriginal(aShellEnt.Id, aShell);

        const TopLoc_Location aGlobalLoc = theParentGlobalLoc * aShell.Location();

        for (TopoDS_Iterator aChildIt(aShell); aChildIt.More(); aChildIt.Next())
        {
          const TopoDS_Shape& aChild = aChildIt.Value();
          if (aChild.ShapeType() == TopAbs_FACE)
          {
            FaceLocalData& aData  = aFaceData.Appended();
            aData.Face            = TopoDS::Face(aChild);
            aData.ParentGlobalLoc = aGlobalLoc;
            aData.ParentShellIdx  = aShellIdx;
          }
          else if (aChild.ShapeType() == TopAbs_WIRE || aChild.ShapeType() == TopAbs_EDGE)
          {
            traverseShape(aChild, -1, aGlobalLoc);

            BRepGraphInc::ChildRef aCR;
            if (makeFreeChildRef(theStorage, aChild, aCR))
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
        if (findExistingNode(theStorage, aWire, BRepGraph_NodeId::Kind::Wire))
          break;

        BRepGraphInc::WireEntity& aWireEnt = theStorage.AppendWire();
        int                       aWireIdx = theStorage.NbWires() - 1;
        aWireEnt.Id       = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Wire, aWireIdx);
        aWireEnt.IsClosed = aWire.Closed();
        theStorage.BindTShapeToNode(aWire.TShape().get(), aWireEnt.Id);
        theStorage.BindOriginal(aWireEnt.Id, aWire);

        for (TopoDS_Iterator anEdgeIt(aWire, false, false); anEdgeIt.More(); anEdgeIt.Next())
        {
          if (anEdgeIt.Value().ShapeType() != TopAbs_EDGE)
            continue;
          const TopoDS_Edge& anEdge = TopoDS::Edge(anEdgeIt.Value());

          // Recurse to create the edge entity (with dedup).
          traverseShape(anEdge, -1, theParentGlobalLoc);

          // Resolve edge index via TShape lookup (handles dedup correctly).
          const BRepGraph_NodeId* anEdgeNodeId = theStorage.FindNodeByTShape(anEdge.TShape().get());
          if (anEdgeNodeId != nullptr && anEdgeNodeId->NodeKind == BRepGraph_NodeId::Kind::Edge)
          {
            // Create CoEdge for free wire (no face context).
            BRepGraphInc::CoEdgeEntity& aCoEdge = theStorage.AppendCoEdge();
            const int aCoEdgeIdx = theStorage.NbCoEdges() - 1;
            aCoEdge.Id      = BRepGraph_NodeId::CoEdge(aCoEdgeIdx);
            aCoEdge.EdgeIdx = anEdgeNodeId->Index;
            aCoEdge.Sense   = anEdge.Orientation();
            // FaceDefId left invalid for free wires.
            // Curve2d left null for free wires.

            BRepGraphInc::CoEdgeRef aCoEdgeRef;
            aCoEdgeRef.CoEdgeIdx = aCoEdgeIdx;
            theStorage.ChangeWire(aWireIdx).CoEdgeRefs.Append(aCoEdgeRef);
          }
        }
        break;
      }

      case TopAbs_EDGE: {
        const TopoDS_Edge& anEdge = TopoDS::Edge(theCurrentShape);
        if (findExistingNode(theStorage, anEdge, BRepGraph_NodeId::Kind::Edge))
          break;

        BRepGraphInc::EdgeEntity& anEdgeEnt = theStorage.AppendEdge();
        int                       anEdgeIdx = theStorage.NbEdges() - 1;
        anEdgeEnt.Id            = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, anEdgeIdx);
        anEdgeEnt.Tolerance     = BRep_Tool::Tolerance(anEdge);
        anEdgeEnt.IsDegenerate  = BRep_Tool::Degenerated(anEdge);
        anEdgeEnt.SameParameter = BRep_Tool::SameParameter(anEdge);
        anEdgeEnt.SameRange     = BRep_Tool::SameRange(anEdge);

        // Extract 3D curve with representation location applied to definition frame.
        {
          double          aFirst = 0.0, aLast = 0.0;
          TopLoc_Location aCurveCombinedLoc;
          anEdgeEnt.Curve3d    = BRep_Tool::Curve(anEdge, aCurveCombinedLoc, aFirst, aLast);
          anEdgeEnt.ParamFirst = aFirst;
          anEdgeEnt.ParamLast  = aLast;
          anEdgeEnt.Curve3d    = applyRepresentationLocation<Geom_Curve>(anEdgeEnt.Curve3d,
                                                                      anEdge.Location(),
                                                                      aCurveCombinedLoc);
        }

        // Extract vertices.
        TopoDS_Vertex                               aVFirst, aVLast;
        NCollection_Vector<ExtractedInternalVertex> anInternalVerts;
        edgeVertices(anEdge, aVFirst, aVLast, anInternalVerts);

        // Register vertices.
        anEdgeEnt.StartVertexIdx = registerOrReuseVertex(theStorage, aVFirst);
        anEdgeEnt.EndVertexIdx   = registerOrReuseVertex(theStorage, aVLast);

        for (int anIntIdx = 0; anIntIdx < anInternalVerts.Length(); ++anIntIdx)
        {
          const ExtractedInternalVertex& anIntVtx = anInternalVerts.Value(anIntIdx);
          int anIntVtxIdx = registerOrReuseVertex(theStorage, anIntVtx.Shape);
          if (anIntVtxIdx >= 0)
          {
            BRepGraphInc::VertexRef aVR;
            aVR.VertexIdx   = anIntVtxIdx;
            aVR.Orientation = anIntVtx.Orientation;
            anEdgeEnt.InternalVertices.Append(aVR);
          }
        }

        // Polygon3D: apply representation location for consistency.
        {
          TopLoc_Location aPoly3DLoc;
          anEdgeEnt.Polygon3D = BRep_Tool::Polygon3D(anEdge, aPoly3DLoc);
          anEdgeEnt.Polygon3D =
            applyRepLocationToPolygon3D(anEdgeEnt.Polygon3D, anEdge.Location(), aPoly3DLoc);
        }

        theStorage.BindTShapeToNode(anEdge.TShape().get(), anEdgeEnt.Id);
        theStorage.BindOriginal(anEdgeEnt.Id, anEdge);
        break;
      }

      case TopAbs_VERTEX: {
        registerOrReuseVertex(theStorage, TopoDS::Vertex(theCurrentShape));
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
    BRepGraphInc::CompoundEntity& aComp     = theStorage.myCompounds.ChangeValue(aCompIdx);
    const TopoDS_Shape*           aCompOrig = theStorage.myOriginalShapes.Seek(aComp.Id);
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
        if (aNodeId != nullptr && static_cast<int>(aNodeId->NodeKind) == aCR.Kind)
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
      const BRepGraphInc::FaceEntity& aFace      = theStorage.myFaces.Value(i);
      const TopoDS_Shape*             anOrigFace = theStorage.myOriginalShapes.Seek(aFace.Id);
      if (anOrigFace != nullptr && !anOrigFace->IsNull())
      {
        TopLoc_Location           aLoc;
        occ::handle<Geom_Surface> aRawSurf = BRep_Tool::Surface(TopoDS::Face(*anOrigFace), aLoc);
        if (!aRawSurf.IsNull())
          aSurfToFaceIdx.TryBind(aRawSurf.get(), i);
      }
    }

    OSD_Parallel::For(
      0,
      theStorage.myEdges.Length(),
      [&theStorage, &aSurfToFaceIdx](int anEdgeIdx) {
        BRepGraphInc::EdgeEntity& anEdgeEnt = theStorage.myEdges.ChangeValue(anEdgeIdx);

        const TopoDS_Shape* anOrigShape = theStorage.myOriginalShapes.Seek(anEdgeEnt.Id);
        if (anOrigShape == nullptr || anOrigShape->IsNull())
          return;

        const TopoDS_Edge&            anEdge = TopoDS::Edge(*anOrigShape);
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
      },
      !theParallel);
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
      const BRepGraphInc::EdgeEntity& anEdgeEnt  = theStorage.myEdges.Value(i);
      const TopoDS_Shape*             anOrigEdge = theStorage.myOriginalShapes.Seek(anEdgeEnt.Id);
      if (anOrigEdge != nullptr && !anOrigEdge->IsNull())
      {
        double                  aF = 0.0, aL = 0.0;
        TopLoc_Location         aLoc;
        occ::handle<Geom_Curve> aRawCurve =
          BRep_Tool::Curve(TopoDS::Edge(*anOrigEdge), aLoc, aF, aL);
        if (!aRawCurve.IsNull())
          aCurveToEdgeDef.TryBind(aRawCurve.get(), anEdgeEnt.Id);
      }
    }

    NCollection_DataMap<const Geom_Surface*, BRepGraph_NodeId> aSurfToFaceDefVtx(1, aTmpAlloc);
    for (int i = 0; i < theStorage.myFaces.Length(); ++i)
    {
      const BRepGraphInc::FaceEntity& aFaceEnt   = theStorage.myFaces.Value(i);
      const TopoDS_Shape*             anOrigFace = theStorage.myOriginalShapes.Seek(aFaceEnt.Id);
      if (anOrigFace != nullptr && !anOrigFace->IsNull())
      {
        TopLoc_Location           aLoc;
        occ::handle<Geom_Surface> aRawSurf = BRep_Tool::Surface(TopoDS::Face(*anOrigFace), aLoc);
        if (!aRawSurf.IsNull())
          aSurfToFaceDefVtx.TryBind(aRawSurf.get(), aFaceEnt.Id);
      }
    }

    OSD_Parallel::For(
      0,
      theStorage.myVertices.Length(),
      [&theStorage, &aCurveToEdgeDef, &aSurfToFaceDefVtx](int aVtxIdx) {
        BRepGraphInc::VertexEntity& aVtxDef   = theStorage.myVertices.ChangeValue(aVtxIdx);
        const TopoDS_Shape*         aVtxShape = theStorage.myOriginalShapes.Seek(aVtxDef.Id);
        if (aVtxShape == nullptr || aVtxShape->IsNull())
          return;
        const TopoDS_Vertex&             aVertex  = TopoDS::Vertex(*aVtxShape);
        const occ::handle<BRep_TVertex>& aTVertex = occ::down_cast<BRep_TVertex>(aVertex.TShape());
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
      },
      !theParallel);
    theStorage.myHasVertexPointReps = true;
  }

  // Build reverse indices.
  theStorage.BuildReverseIndex();

  theStorage.myIsDone = true;
}

//=================================================================================================

void BRepGraphInc_Populate::Append(BRepGraphInc_Storage&                         theStorage,
                                   const TopoDS_Shape&                           theShape,
                                   bool                                          theParallel,
                                   const occ::handle<NCollection_BaseAllocator>& theTmpAlloc)
{
  if (theShape.IsNull())
    return;

  // Use temporary allocator if provided, else default.
  // Must NOT use the storage's persistent allocator for scratch data.
  const occ::handle<NCollection_BaseAllocator>& aTmpAlloc =
    !theTmpAlloc.IsNull() ? theTmpAlloc : NCollection_BaseAllocator::CommonBaseAllocator();

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
          traverseShape(aChildIt.Value(), theParentGlobalLoc * theCurrentShape.Location());
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
  theStorage.BuildDeltaReverseIndex(anOldNbEdges,
                                    anOldNbWires,
                                    anOldNbFaces,
                                    anOldNbShells,
                                    anOldNbSolids);

  theStorage.myIsDone = true;
}
