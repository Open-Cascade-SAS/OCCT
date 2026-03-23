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
  occ::handle<Geom_Surface>                           OriginalSurface; //!< Pre-transform surface for PCurve matching
  NCollection_Vector<occ::handle<Poly_Triangulation>> Triangulations;
  int                                                 ActiveTriangulationIndex = -1;
  double                                              Tolerance                = 0.0;
  TopAbs_Orientation                                  Orientation              = TopAbs_FORWARD;
  bool                                                NaturalRestriction       = false;
  NCollection_Vector<ExtractedWire>                   Wires;
  NCollection_Vector<ExtractedInternalVertex> DirectVertices; //!< INTERNAL/EXTERNAL vertex children
};

//! Extract stored PCurve(s) from edge for a given face's surface.
//! Iterates BRep_TEdge::Curves() directly, avoiding BRep_Tool::CurveOnSurface
//! which can compute phantom PCurves via CurveOnPlane for planar surfaces.
//! Uses multi-pass matching:
//!   Pass 1: exact (Surface, Location) match via IsCurveOnSurface(S, L)
//!   Pass 2: surface-handle-only fallback for TopLoc_Location structural equality bug
//!           (only when a unique CR matches the surface — prevents wrong-context selection)
//! For seam edges (IsCurveOnClosedSurface), extracts both PCurves + continuity.
//! @param[in]  theEdge      edge with context orientation and location
//! @param[in]  theFace      face with context location
//! @param[out] thePCurve    primary PCurve (or null)
//! @param[out] thePCurve2   seam PCurve (or null, only for closed surfaces)
//! @param[out] theFirst     parameter range start
//! @param[out] theLast      parameter range end
//! @param[out] theSeamContinuity  seam continuity (GeomAbs_C0 if non-seam)
//! @param[in]  theOrigSurface  pre-transform surface handle for fallback matching
//!                             when the face's raw TFace surface differs from edge CRs
//! @return true if a stored PCurve was found
bool extractStoredPCurves(const TopoDS_Edge&                theEdge,
                          const TopoDS_Face&                theFace,
                          occ::handle<Geom2d_Curve>&        thePCurve,
                          occ::handle<Geom2d_Curve>&        thePCurve2,
                          double&                           theFirst,
                          double&                           theLast,
                          GeomAbs_Shape&                    theSeamContinuity,
                          const occ::handle<Geom_Surface>&  theOrigSurface
                            = occ::handle<Geom_Surface>())
{
  TopLoc_Location aFaceLoc;
  const occ::handle<Geom_Surface>& aSurf = BRep_Tool::Surface(theFace, aFaceLoc);
  if (aSurf.IsNull())
    return false;
  const occ::handle<BRep_TEdge> aTEdge = occ::down_cast<BRep_TEdge>(theEdge.TShape());
  if (aTEdge.IsNull())
    return false;
  const bool aReversed = (theEdge.Orientation() == TopAbs_REVERSED);

  // Expected CurveRepresentation location for this face+edge context.
  // This is the same formula used by BRep_Tool::CurveOnSurface internally.
  const TopLoc_Location aExpectedLoc = aFaceLoc.Predivided(theEdge.Location());

  // Lambda to extract PCurve data from a matched CurveRepresentation.
  auto extractFromCR = [&](const occ::handle<BRep_CurveRepresentation>& theCR) -> bool {
    const BRep_GCurve* aGC = static_cast<const BRep_GCurve*>(theCR.get());
    aGC->Range(theFirst, theLast);
    if (aGC->IsCurveOnClosedSurface())
    {
      thePCurve  = aReversed ? aGC->PCurve2() : aGC->PCurve();
      thePCurve2 = aReversed ? aGC->PCurve()  : aGC->PCurve2();
      theSeamContinuity =
        static_cast<const BRep_CurveOnClosedSurface*>(theCR.get())->Continuity();
    }
    else
    {
      thePCurve = aGC->PCurve();
    }
    return true;
  };

  // Pass 1: exact match by (Surface, Location). Correctly distinguishes
  // multiple CurveOnSurface entries for the same surface with different Locations.
  for (const auto& aCR : aTEdge->Curves())
  {
    if (!aCR->IsCurveOnSurface())
      continue;
    if (aCR->IsCurveOnSurface(aSurf, aExpectedLoc))
      return extractFromCR(aCR);
  }

  // Pass 2: fallback to surface-handle-only match.
  // Handles the TopLoc_Location structural equality bug where
  // an explicit identity datum does not compare equal to a default empty identity.
  for (const auto& aCR : aTEdge->Curves())
  {
    if (!aCR->IsCurveOnSurface() || aCR->Surface() != aSurf)
      continue;
    return extractFromCR(aCR);
  }


  // Pass 3: match by the original (pre-transform) surface handle.
  // When applyRepresentationLocation creates a new surface via Transformed(),
  // the edge's CRs still reference the OLD surface handle. If the face's raw
  // TFace surface differs from the old surface (e.g., compound children at
  // different locations), passes 1-2 fail. Here we retry with the original
  // surface handle that the edge CRs actually reference.
  if (!theOrigSurface.IsNull() && theOrigSurface != aSurf)
  {
    // Pass 3a: exact (original surface, expected location) match.
    for (const auto& aCR : aTEdge->Curves())
    {
      if (!aCR->IsCurveOnSurface())
        continue;
      if (aCR->IsCurveOnSurface(theOrigSurface, aExpectedLoc))
        return extractFromCR(aCR);
    }
    // Pass 3b: original surface handle-only match (location structural equality fallback).
    for (const auto& aCR : aTEdge->Curves())
    {
      if (!aCR->IsCurveOnSurface() || aCR->Surface() != theOrigSurface)
        continue;
      return extractFromCR(aCR);
    }
  }

  return false;
}

//! Get the raw BRep_TVertex point without applying vertex Location.
//! Stores the point in the TShape-local (definition) frame, consistent
//! with how edge curves and face surfaces are stored after
//! applyRepresentationLocation.
gp_Pnt rawVertexPoint(const TopoDS_Vertex& theVertex)
{
  return static_cast<const BRep_TVertex*>(theVertex.TShape().get())->Pnt();
}

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
                               rawVertexPoint(theVertex),
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
  if (theGeom.IsNull())
    return theGeom;
  // Do NOT use theCombinedLoc.IsIdentity() as an early return — TopLoc_Location
  // chain composition can structurally cancel to Identity (empty chain) even when
  // the actual repLoc (theShapeLoc^-1 * theCombinedLoc) is non-Identity.
  // This happens when the edge instance location and the CR location on TEdge
  // form inverse pairs that cancel in Multiplied().
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
  if (thePolygon3D.IsNull())
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

//! Deduplication maps for representation entities.
//! Keyed by raw Handle pointer — same underlying geometry object → same rep entity.
struct RepDedup
{
  NCollection_DataMap<const Geom_Surface*,       int> Surfaces;
  NCollection_DataMap<const Geom_Curve*,         int> Curves3D;
  NCollection_DataMap<const Geom2d_Curve*,       int> Curves2D;
  NCollection_DataMap<const Poly_Triangulation*, int> Triangulations;
  NCollection_DataMap<const Poly_Polygon3D*,     int> Polygons3D;
  NCollection_DataMap<const Poly_Polygon2D*,     int> Polygons2D;
  NCollection_DataMap<const Poly_PolygonOnTriangulation*, int> PolygonsOnTri;
};

//! Create or reuse a SurfaceRep for the given surface handle.
int getOrCreateSurfaceRep(BRepGraphInc_Storage&           theStorage,
                          RepDedup&                       theDedup,
                          const occ::handle<Geom_Surface>& theSurface)
{
  if (theSurface.IsNull())
    return -1;
  const Geom_Surface* aPtr = theSurface.get();
  const int* anExisting = theDedup.Surfaces.Seek(aPtr);
  if (anExisting != nullptr)
    return *anExisting;
  BRepGraphInc::SurfaceRep& aRep = theStorage.AppendSurfaceRep();
  const int anIdx = theStorage.NbSurfaces() - 1;
  aRep.Id      = BRepGraph_RepId::Surface(anIdx);
  aRep.Surface = theSurface;
  theDedup.Surfaces.Bind(aPtr, anIdx);
  return anIdx;
}

//! Create or reuse a Curve3DRep for the given 3D curve handle.
int getOrCreateCurve3DRep(BRepGraphInc_Storage&         theStorage,
                          RepDedup&                     theDedup,
                          const occ::handle<Geom_Curve>& theCurve)
{
  if (theCurve.IsNull())
    return -1;
  const Geom_Curve* aPtr = theCurve.get();
  const int* anExisting = theDedup.Curves3D.Seek(aPtr);
  if (anExisting != nullptr)
    return *anExisting;
  BRepGraphInc::Curve3DRep& aRep = theStorage.AppendCurve3DRep();
  const int anIdx = theStorage.NbCurves3D() - 1;
  aRep.Id    = BRepGraph_RepId::Curve3D(anIdx);
  aRep.Curve = theCurve;
  theDedup.Curves3D.Bind(aPtr, anIdx);
  return anIdx;
}

//! Create or reuse a Curve2DRep for the given 2D curve handle.
int getOrCreateCurve2DRep(BRepGraphInc_Storage&             theStorage,
                          RepDedup&                         theDedup,
                          const occ::handle<Geom2d_Curve>&  theCurve)
{
  if (theCurve.IsNull())
    return -1;
  const Geom2d_Curve* aPtr = theCurve.get();
  const int* anExisting = theDedup.Curves2D.Seek(aPtr);
  if (anExisting != nullptr)
    return *anExisting;
  BRepGraphInc::Curve2DRep& aRep = theStorage.AppendCurve2DRep();
  const int anIdx = theStorage.NbCurves2D() - 1;
  aRep.Id    = BRepGraph_RepId::Curve2D(anIdx);
  aRep.Curve = theCurve;
  theDedup.Curves2D.Bind(aPtr, anIdx);
  return anIdx;
}

//! Create or reuse a TriangulationRep for the given triangulation handle.
int getOrCreateTriangulationRep(BRepGraphInc_Storage&                  theStorage,
                                RepDedup&                              theDedup,
                                const occ::handle<Poly_Triangulation>& theTriangulation)
{
  if (theTriangulation.IsNull())
    return -1;
  const Poly_Triangulation* aPtr = theTriangulation.get();
  const int* anExisting = theDedup.Triangulations.Seek(aPtr);
  if (anExisting != nullptr)
    return *anExisting;
  BRepGraphInc::TriangulationRep& aRep = theStorage.AppendTriangulationRep();
  const int anIdx = theStorage.NbTriangulations() - 1;
  aRep.Id            = BRepGraph_RepId::Triangulation(anIdx);
  aRep.Triangulation = theTriangulation;
  theDedup.Triangulations.Bind(aPtr, anIdx);
  return anIdx;
}

//! Create or reuse a Polygon3DRep for the given polygon handle.
int getOrCreatePolygon3DRep(BRepGraphInc_Storage&                theStorage,
                            RepDedup&                            theDedup,
                            const occ::handle<Poly_Polygon3D>&  thePolygon)
{
  if (thePolygon.IsNull())
    return -1;
  const Poly_Polygon3D* aPtr = thePolygon.get();
  const int* anExisting = theDedup.Polygons3D.Seek(aPtr);
  if (anExisting != nullptr)
    return *anExisting;
  BRepGraphInc::Polygon3DRep& aRep = theStorage.AppendPolygon3DRep();
  const int anIdx = theStorage.NbPolygons3D() - 1;
  aRep.Id      = BRepGraph_RepId::Polygon3D(anIdx);
  aRep.Polygon = thePolygon;
  theDedup.Polygons3D.Bind(aPtr, anIdx);
  return anIdx;
}

//! Create or reuse a Polygon2DRep for the given polygon-on-surface handle.
int getOrCreatePolygon2DRep(BRepGraphInc_Storage&                theStorage,
                            RepDedup&                            theDedup,
                            const occ::handle<Poly_Polygon2D>&   thePolygon)
{
  if (thePolygon.IsNull())
    return -1;
  const Poly_Polygon2D* aPtr = thePolygon.get();
  const int* anExisting = theDedup.Polygons2D.Seek(aPtr);
  if (anExisting != nullptr)
    return *anExisting;
  BRepGraphInc::Polygon2DRep& aRep = theStorage.AppendPolygon2DRep();
  const int anIdx = theStorage.NbPolygons2D() - 1;
  aRep.Id      = BRepGraph_RepId::Polygon2D(anIdx);
  aRep.Polygon = thePolygon;
  theDedup.Polygons2D.Bind(aPtr, anIdx);
  return anIdx;
}

//! Create or reuse a PolygonOnTriRep for the given polygon-on-triangulation handle.
//! theTriRepIdx is the global TriangulationRep index (not face-local).
int getOrCreatePolygonOnTriRep(BRepGraphInc_Storage&                              theStorage,
                               RepDedup&                                           theDedup,
                               const occ::handle<Poly_PolygonOnTriangulation>&     thePolygon,
                               int                                                 theTriRepIdx)
{
  if (thePolygon.IsNull())
    return -1;
  const Poly_PolygonOnTriangulation* aPtr = thePolygon.get();
  const int* anExisting = theDedup.PolygonsOnTri.Seek(aPtr);
  if (anExisting != nullptr)
    return *anExisting;
  BRepGraphInc::PolygonOnTriRep& aRep = theStorage.AppendPolygonOnTriRep();
  const int anIdx = theStorage.NbPolygonsOnTri() - 1;
  aRep.Id                  = BRepGraph_RepId::PolygonOnTri(anIdx);
  aRep.Polygon             = thePolygon;
  aRep.TriangulationRepIdx = theTriRepIdx;
  theDedup.PolygonsOnTri.Bind(aPtr, anIdx);
  return anIdx;
}

//! Register an edge entity from pre-extracted data, with TShape dedup.
//! Creates the entity (with vertices) if new, returns the index in both cases.
int registerExtractedEdge(BRepGraphInc_Storage& theStorage,
                          const ExtractedEdge&  theEdgeData,
                          RepDedup&             theRepDedup)
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
  anEdgeEnt.IsClosed                  = theEdgeData.Shape.Closed();
  anEdgeEnt.ParamFirst                = theEdgeData.ParamFirst;
  anEdgeEnt.ParamLast                 = theEdgeData.ParamLast;

  if (!theEdgeData.Curve3d.IsNull())
  {
    anEdgeEnt.Curve3DRepIdx = getOrCreateCurve3DRep(theStorage, theRepDedup, theEdgeData.Curve3d);
  }

  // Vertex registration (boundary vertices use VertexRef with Location).
  // Vertices may be null for infinite edges or degenerate topology.
  if (!theEdgeData.StartVertex.Shape.IsNull())
  {
    anEdgeEnt.StartVertex.VertexIdx = registerOrReuseVertex(theStorage,
                                                            theEdgeData.StartVertex.Shape,
                                                            theEdgeData.StartVertex.Point,
                                                            theEdgeData.StartVertex.Tolerance);
    anEdgeEnt.StartVertex.Orientation   = TopAbs_FORWARD;
    anEdgeEnt.StartVertex.LocalLocation = theEdgeData.StartVertex.Shape.Location();
  }
  if (!theEdgeData.EndVertex.Shape.IsNull())
  {
    anEdgeEnt.EndVertex.VertexIdx = registerOrReuseVertex(theStorage,
                                                          theEdgeData.EndVertex.Shape,
                                                          theEdgeData.EndVertex.Point,
                                                          theEdgeData.EndVertex.Tolerance);
    anEdgeEnt.EndVertex.Orientation   = TopAbs_REVERSED;
    anEdgeEnt.EndVertex.LocalLocation = theEdgeData.EndVertex.Shape.Location();
  }

  // Register internal/external vertices.
  for (int anIntIdx = 0; anIntIdx < theEdgeData.InternalVertices.Length(); ++anIntIdx)
  {
    const ExtractedInternalVertex& anIntVtx = theEdgeData.InternalVertices.Value(anIntIdx);
    int                            anIntVtxIdx =
      registerOrReuseVertex(theStorage, anIntVtx.Shape, anIntVtx.Point, anIntVtx.Tolerance);
    if (anIntVtxIdx >= 0)
    {
      BRepGraphInc::VertexRef aVR;
      aVR.VertexIdx      = anIntVtxIdx;
      aVR.Orientation    = anIntVtx.Orientation;
      aVR.LocalLocation  = anIntVtx.Shape.Location();
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
  for (TopoDS_Iterator aVIt(theEdge, false, false); aVIt.More(); aVIt.Next())
  {
    if (aVIt.Value().ShapeType() != TopAbs_VERTEX)
      continue;
    const TopoDS_Vertex aVertex = TopoDS::Vertex(aVIt.Value());
    if (aVertex.Orientation() == TopAbs_FORWARD)
    {
      if (!theFirst.IsNull())
      {
        // Preserve previous FORWARD vertex in internal list.
        ExtractedInternalVertex anIntVtx;
        anIntVtx.Shape       = theFirst;
        anIntVtx.Point       = rawVertexPoint(theFirst);
        anIntVtx.Tolerance   = BRep_Tool::Tolerance(theFirst);
        anIntVtx.Orientation = TopAbs_FORWARD;
        theInternal.Append(anIntVtx);
      }
      theFirst = aVertex;
    }
    else if (aVertex.Orientation() == TopAbs_REVERSED)
    {
      if (!theLast.IsNull())
      {
        // Preserve previous REVERSED vertex in internal list.
        ExtractedInternalVertex anIntVtx;
        anIntVtx.Shape       = theLast;
        anIntVtx.Point       = rawVertexPoint(theLast);
        anIntVtx.Tolerance   = BRep_Tool::Tolerance(theLast);
        anIntVtx.Orientation = TopAbs_REVERSED;
        theInternal.Append(anIntVtx);
      }
      theLast = aVertex;
    }
    else
    {
      ExtractedInternalVertex anIntVtx;
      anIntVtx.Shape       = aVertex;
      anIntVtx.Point       = rawVertexPoint(aVertex);
      anIntVtx.Tolerance   = BRep_Tool::Tolerance(aVertex);
      anIntVtx.Orientation = aVertex.Orientation();
      theInternal.Append(anIntVtx);
    }
  }
  // Note: do NOT copy theFirst↔theLast when one is null.
  // Single-vertex edges (e.g., infinite edges) legitimately have only one vertex.
  // Closed edges already have both FORWARD and REVERSED vertices in the iterator.
}

//! Extract edge geometry and parametric data in a face context.
//! Fills theEdgeData with 3D curve, vertices, PCurves, and polygons.
void extractEdgeInFace(ExtractedEdge&                    theEdgeData,
                       const TopoDS_Edge&                theEdge,
                       const TopoDS_Face&                theForwardFace,
                       const occ::handle<Geom_Surface>&  theFaceSurface,
                       const occ::handle<Geom_Surface>&  theOrigSurface)
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
    theEdgeData.StartVertex.Point     = rawVertexPoint(aVFirst);
    theEdgeData.StartVertex.Tolerance = BRep_Tool::Tolerance(aVFirst);
  }
  if (!aVLast.IsNull())
  {
    theEdgeData.EndVertex.Shape     = aVLast;
    theEdgeData.EndVertex.Point     = rawVertexPoint(aVLast);
    theEdgeData.EndVertex.Tolerance = BRep_Tool::Tolerance(aVLast);
  }

  // Extract stored PCurves directly from BRep_TEdge::Curves(), bypassing
  // BRep_Tool::CurveOnSurface which can fail due to TopLoc_Location structural
  // equality bug and can compute phantom PCurves via CurveOnPlane.
  // Uses FORWARD-oriented edge for consistent PCurve pair ordering.
  {
    const TopoDS_Edge aFwdEdge = TopoDS::Edge(theEdge.Oriented(TopAbs_FORWARD));
    double            aPCFirst = 0.0, aPCLast = 0.0;
    GeomAbs_Shape     aSeamContinuity = GeomAbs_C0;

    extractStoredPCurves(aFwdEdge, theForwardFace,
                         theEdgeData.PCurve2d, theEdgeData.PCurve2dReversed,
                         aPCFirst, aPCLast, aSeamContinuity,
                         theOrigSurface);

    theEdgeData.PCFirst          = aPCFirst;
    theEdgeData.PCLast           = aPCLast;
    theEdgeData.PCurveContinuity = BRep_Tool::MaxContinuity(theEdge);
    theEdgeData.SeamContinuity   = aSeamContinuity;

    // When the surface was transformed (TFace.Location != Identity → theFaceSurface
    // differs from the raw TFace surface), the stored CR may belong to a different face
    // context using the same raw surface. Verify by calling BRep_Tool::CurveOnSurface
    // which correctly handles CurveOnPlane for planar surfaces and properly composes
    // face+edge locations. If BRep_Tool COMPUTED a PCurve (not stored) AND it differs
    // from what we extracted, our stored-only extraction picked a CR from the wrong
    // context. Discard ours so reconstruction doesn't attach the wrong PCurve.
    if (!theEdgeData.PCurve2d.IsNull() && theFaceSurface != theOrigSurface)
    {
      double aBTFirst = 0.0, aBTLast = 0.0;
      bool   aBTIsStored = false;
      occ::handle<Geom2d_Curve> aBTPCurve =
        BRep_Tool::CurveOnSurface(aFwdEdge, theForwardFace, aBTFirst, aBTLast, &aBTIsStored);
      if (!aBTPCurve.IsNull() && !aBTIsStored
          && aBTPCurve.get() != theEdgeData.PCurve2d.get())
      {
        // BRep_Tool computed a different PCurve (CurveOnPlane) — our stored match
        // is from the wrong face context. Discard it.
        theEdgeData.PCurve2d.Nullify();
        theEdgeData.PCurve2dReversed.Nullify();
        theEdgeData.PCFirst = 0.0;
        theEdgeData.PCLast  = 0.0;
        aPCFirst            = 0.0;
        aPCLast             = 0.0;
        theEdgeData.SeamContinuity = GeomAbs_C0;
      }
    }

    if (!theEdgeData.PCurve2d.IsNull() && !theFaceSurface.IsNull())
      BRep_Tool::UVPoints(aFwdEdge, theForwardFace, theEdgeData.PCUV1, theEdgeData.PCUV2);

    // For seam edges, extract reversed parameter range.
    // The reversed edge accesses PCurve2/PCurve (swapped), so Range gives the same values.
    // Fall back to primary range if extraction fails.
    if (!theEdgeData.PCurve2dReversed.IsNull())
    {
      const TopoDS_Edge         aRevEdge = TopoDS::Edge(theEdge.Oriented(TopAbs_REVERSED));
      occ::handle<Geom2d_Curve> aDummyPC1, aDummyPC2;
      GeomAbs_Shape             aDummyCont = GeomAbs_C0;
      if (!extractStoredPCurves(aRevEdge, theForwardFace,
                                aDummyPC1, aDummyPC2,
                                theEdgeData.PCFirstReversed, theEdgeData.PCLastReversed,
                                aDummyCont))
      {
        // Seam edges share the same parameter range; use primary as fallback.
        theEdgeData.PCFirstReversed = aPCFirst;
        theEdgeData.PCLastReversed  = aPCLast;
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
    theData.OriginalSurface = theData.Surface; // save pre-transform handle for PCurve matching
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

  for (TopoDS_Iterator aChildIt(aForwardFace, false, false); aChildIt.More(); aChildIt.Next())
  {
    if (aChildIt.Value().ShapeType() == TopAbs_VERTEX)
    {
      const TopoDS_Vertex&    aVertex = TopoDS::Vertex(aChildIt.Value());
      ExtractedInternalVertex aVtxData;
      aVtxData.Shape       = aVertex;
      aVtxData.Point       = rawVertexPoint(aVertex);
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
                        aForwardFace, theData.Surface, theData.OriginalSurface);
      aWireData.Edges.Append(anEdgeData);
    }

    theData.Wires.Append(aWireData);
  }
}

//! Register pre-extracted face data into incidence storage.
//! Uses unified TShapeToNodeId map and populates OriginalShapes.
void registerFaceData(BRepGraphInc_Storage&                    theStorage,
                      const NCollection_Vector<FaceLocalData>& theFaceData,
                      RepDedup&                                theRepDedup)
{
  for (int aFaceDataIdx = 0; aFaceDataIdx < theFaceData.Length(); ++aFaceDataIdx)
  {
    const FaceLocalData& aData    = theFaceData.Value(aFaceDataIdx);
    const TopoDS_Face&   aCurFace = aData.Face;

    // Create or reuse FaceEntity.
    const BRepGraph_NodeId* anExistingFace =
      findExistingNode(theStorage, aCurFace, BRepGraph_NodeId::Kind::Face);

    int  aFaceIdx      = -1;
    bool aIsNewFaceDef = false;
    if (anExistingFace != nullptr)
    {
      aFaceIdx = anExistingFace->Index;
    }
    else
    {
      aIsNewFaceDef = true;
      BRepGraphInc::FaceEntity& aFace = theStorage.AppendFace();
      aFaceIdx                        = theStorage.NbFaces() - 1;
      aFace.Id                        = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, aFaceIdx);
      aFace.Tolerance                 = aData.Tolerance;
      aFace.NaturalRestriction        = aData.NaturalRestriction;
      aFace.SurfaceRepIdx             = getOrCreateSurfaceRep(theStorage, theRepDedup, aData.Surface);

      for (int aTriIdx = 0; aTriIdx < aData.Triangulations.Length(); ++aTriIdx)
      {
        const auto& aTri = aData.Triangulations.Value(aTriIdx);
        aFace.TriangulationRepIdxs.Append(
          getOrCreateTriangulationRep(theStorage, theRepDedup, aTri));
      }
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

    // Process wires — only for newly created face definitions.
    // Shared faces (same TShape referenced multiple times in a shell) must NOT
    // duplicate wire/edge/coedge data on the single FaceEntity.
    if (!aIsNewFaceDef)
      continue;

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
        aWireRef.WireIdx       = aWireIdx;
        aWireRef.IsOuter       = aWireData.IsOuter;
        aWireRef.Orientation   = aWireData.Shape.Orientation();
        aWireRef.LocalLocation = aWireData.Shape.Location();
        theStorage.ChangeFace(aFaceIdx).WireRefs.Append(aWireRef);
      }


      for (int anEdgeIter = 0; anEdgeIter < aWireData.Edges.Length(); ++anEdgeIter)
      {
        const ExtractedEdge& anEdgeData = aWireData.Edges.Value(anEdgeIter);
        int                  anEdgeIdx  = registerExtractedEdge(theStorage, anEdgeData, theRepDedup);

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
          aCoEdge.Sense         = anEdgeData.OrientationInWire;

          // Populate CoEdge with PCurve and polygon data for this face context.
          if (!anEdgeData.PCurve2d.IsNull())
          {
            aCoEdge.Curve2DRepIdx = getOrCreateCurve2DRep(theStorage, theRepDedup,
                                                          anEdgeData.PCurve2d);
            aCoEdge.ParamFirst = anEdgeData.PCFirst;
            aCoEdge.ParamLast  = anEdgeData.PCLast;
            aCoEdge.Continuity = anEdgeData.PCurveContinuity;
            aCoEdge.UV1        = anEdgeData.PCUV1;
            aCoEdge.UV2        = anEdgeData.PCUV2;
          }
          aCoEdge.Polygon2DRepIdx  = getOrCreatePolygon2DRep(theStorage, theRepDedup,
                                                             anEdgeData.PolyOnSurf);

          // Handle seam edge: create a second CoEdge for the reversed sense.
          if (!anEdgeData.PCurve2dReversed.IsNull())
          {
            BRepGraphInc::CoEdgeEntity& aSeamCoEdge = theStorage.AppendCoEdge();
            aSeamCoEdgeIdx = theStorage.NbCoEdges() - 1;
            aSeamCoEdge.Id        = BRepGraph_NodeId::CoEdge(aSeamCoEdgeIdx);
            aSeamCoEdge.EdgeIdx   = anEdgeIdx;
            aSeamCoEdge.FaceDefId = BRepGraph_NodeId::Face(aFaceIdx);
            aSeamCoEdge.Sense         = TopAbs_REVERSED;
            aSeamCoEdge.Curve2DRepIdx = getOrCreateCurve2DRep(theStorage, theRepDedup,
                                                              anEdgeData.PCurve2dReversed);
            aSeamCoEdge.ParamFirst = anEdgeData.PCFirstReversed;
            aSeamCoEdge.ParamLast  = anEdgeData.PCLastReversed;
            aSeamCoEdge.Continuity = anEdgeData.PCurveContinuity;
            aSeamCoEdge.SeamContinuity = anEdgeData.SeamContinuity;
            aSeamCoEdge.Polygon2DRepIdx  = getOrCreatePolygon2DRep(theStorage, theRepDedup,
                                                                   anEdgeData.PolyOnSurfReversed);

            // Link seam pair.
            // Note: aCoEdge ref may be invalidated by AppendCoEdge, re-fetch.
            theStorage.ChangeCoEdge(aCoEdgeIdx).SeamPairIdx = aSeamCoEdgeIdx;
            aSeamCoEdge.SeamPairIdx = aCoEdgeIdx;
          }

          // Add CoEdgeRef to wire with edge-in-wire Location.
          BRepGraphInc::CoEdgeRef aCoEdgeRef;
          aCoEdgeRef.CoEdgeIdx      = aCoEdgeIdx;
          aCoEdgeRef.LocalLocation  = anEdgeData.Shape.Location();
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

        // Polygon3D (once per edge).
        if (!anEdgeData.Polygon3D.IsNull() && anEdgeMut.Polygon3DRepIdx < 0)
        {
          anEdgeMut.Polygon3DRepIdx = getOrCreatePolygon3DRep(theStorage, theRepDedup,
                                                              anEdgeData.Polygon3D);
        }

        // Polygon-on-triangulation: extract directly into CoEdge entities.
        if (aFwdCoEdgeIdx >= 0)
        {
          const int aNbOrigTris = aFaceDef.TriangulationRepIdxs.Length();
          for (int aTriIdx = 0; aTriIdx < aNbOrigTris; ++aTriIdx)
          {
            const int aTriRepIdxOrig = aFaceDef.TriangulationRepIdxs.Value(aTriIdx);
            if (aTriRepIdxOrig < 0)
              continue;
            const occ::handle<Poly_Triangulation>& aTri =
              theStorage.TriangulationRep(aTriRepIdxOrig).Triangulation;
            if (aTri.IsNull())
              continue;

            TopLoc_Location                          aPolyTriLoc;
            occ::handle<Poly_PolygonOnTriangulation> aPolyOnTri =
              BRep_Tool::PolygonOnTriangulation(anEdgeData.Shape, aTri, aPolyTriLoc);
            if (aPolyOnTri.IsNull())
              continue;

            int aTriRepIdx = aTriRepIdxOrig;
            if (!aPolyTriLoc.IsIdentity())
            {
              const TopLoc_Location aRepLoc = anEdgeData.Shape.Location().Inverted() * aPolyTriLoc;
              if (!aRepLoc.IsIdentity())
              {
                occ::handle<Poly_Triangulation> aTriCopy = aTri->Copy();
                const gp_Trsf&                  aTrsf    = aRepLoc.Transformation();
                for (int aNodeIdx = 1; aNodeIdx <= aTriCopy->NbNodes(); ++aNodeIdx)
                  aTriCopy->SetNode(aNodeIdx, aTriCopy->Node(aNodeIdx).Transformed(aTrsf));
                aTriRepIdx = getOrCreateTriangulationRep(theStorage, theRepDedup, aTriCopy);
                theStorage.ChangeFace(aFaceIdx).TriangulationRepIdxs.Append(aTriRepIdx);
              }
            }

            const int aPolyOnTriRepIdx = getOrCreatePolygonOnTriRep(
              theStorage, theRepDedup, aPolyOnTri, aTriRepIdx);

            theStorage.ChangeCoEdge(aFwdCoEdgeIdx).PolygonOnTriRepIdxs.Append(aPolyOnTriRepIdx);

            // Seam polygon-on-triangulation.
            TopoDS_Edge                              aRevEdge = TopoDS::Edge(anEdgeData.Shape.Reversed());
            occ::handle<Poly_PolygonOnTriangulation> aPolyOnTriRev =
              BRep_Tool::PolygonOnTriangulation(aRevEdge, aTri, aPolyTriLoc);
            if (!aPolyOnTriRev.IsNull() && aPolyOnTriRev != aPolyOnTri && aSeamCoEdgeIdx >= 0)
            {
              const int aSeamPolyRepIdx = getOrCreatePolygonOnTriRep(
                theStorage, theRepDedup, aPolyOnTriRev, aTriRepIdx);

              theStorage.ChangeCoEdge(aSeamCoEdgeIdx).PolygonOnTriRepIdxs.Append(aSeamPolyRepIdx);
            }
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
        aVR.VertexIdx      = aVtxIdx;
        aVR.Orientation    = aDirVtx.Orientation;
        aVR.LocalLocation  = aDirVtx.Shape.Location();
        theStorage.ChangeFace(aFaceIdx).VertexRefs.Append(aVR);
      }
    }
  }
}

} // anonymous namespace

//=================================================================================================

void BRepGraphInc_Populate::Perform(BRepGraphInc_Storage&                         theStorage,
                                    const TopoDS_Shape&                           theShape,
                                    const bool                                    theParallel,
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
  RepDedup aRepDedup;

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

        BRepGraphInc::CompoundEntity& aCompEnt = theStorage.AppendCompound();
        int                           aCompIdx = theStorage.NbCompounds() - 1;
        aCompEnt.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Compound, aCompIdx);
        theStorage.BindTShapeToNode(aCompound.TShape().get(), aCompEnt.Id);
        theStorage.BindOriginal(aCompEnt.Id, aCompound);

        const TopLoc_Location aGlobalLoc = theParentGlobalLoc * aCompound.Location();

        for (TopoDS_Iterator aChildIt(aCompound, false, false); aChildIt.More(); aChildIt.Next())
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

        BRepGraphInc::CompSolidEntity& aCSolidEnt = theStorage.AppendCompSolid();
        int                            aCSolidIdx = theStorage.NbCompSolids() - 1;
        aCSolidEnt.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::CompSolid, aCSolidIdx);
        theStorage.BindTShapeToNode(aCompSolid.TShape().get(), aCSolidEnt.Id);
        theStorage.BindOriginal(aCSolidEnt.Id, aCompSolid);

        const TopLoc_Location aGlobalLoc = theParentGlobalLoc * aCompSolid.Location();

        for (TopoDS_Iterator aChildIt(aCompSolid, false, false); aChildIt.More(); aChildIt.Next())
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

        BRepGraphInc::SolidEntity& aSolidEnt = theStorage.AppendSolid();
        int                        aSolidIdx = theStorage.NbSolids() - 1;
        aSolidEnt.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, aSolidIdx);
        theStorage.BindTShapeToNode(aSolid.TShape().get(), aSolidEnt.Id);
        theStorage.BindOriginal(aSolidEnt.Id, aSolid);

        const TopLoc_Location aGlobalLoc = theParentGlobalLoc * aSolid.Location();

        for (TopoDS_Iterator aChildIt(aSolid, false, false); aChildIt.More(); aChildIt.Next())
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

        BRepGraphInc::ShellEntity& aShellEnt = theStorage.AppendShell();
        int                        aShellIdx = theStorage.NbShells() - 1;
        aShellEnt.Id       = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Shell, aShellIdx);
        aShellEnt.IsClosed = aShell.Closed();
        theStorage.BindTShapeToNode(aShell.TShape().get(), aShellEnt.Id);
        theStorage.BindOriginal(aShellEnt.Id, aShell);

        const TopLoc_Location aGlobalLoc = theParentGlobalLoc * aShell.Location();

        for (TopoDS_Iterator aChildIt(aShell, false, false); aChildIt.More(); aChildIt.Next())
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
            aCoEdgeRef.CoEdgeIdx     = aCoEdgeIdx;
            aCoEdgeRef.LocalLocation = anEdge.Location();
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
        anEdgeEnt.IsClosed      = anEdge.Closed();

        // Extract 3D curve with representation location applied to definition frame.
        {
          double          aFirst = 0.0, aLast = 0.0;
          TopLoc_Location aCurveCombinedLoc;
          occ::handle<Geom_Curve> aCurve3d = BRep_Tool::Curve(anEdge, aCurveCombinedLoc, aFirst, aLast);
          anEdgeEnt.ParamFirst = aFirst;
          anEdgeEnt.ParamLast  = aLast;
          aCurve3d = applyRepresentationLocation<Geom_Curve>(aCurve3d,
                                                             anEdge.Location(),
                                                             aCurveCombinedLoc);
          anEdgeEnt.Curve3DRepIdx = getOrCreateCurve3DRep(theStorage, aRepDedup, aCurve3d);
        }

        // Extract vertices.
        TopoDS_Vertex                               aVFirst, aVLast;
        NCollection_Vector<ExtractedInternalVertex> anInternalVerts;
        edgeVertices(anEdge, aVFirst, aVLast, anInternalVerts);

        // Register vertices (using definition-frame points; Location stored on VertexRef).
        // Vertices may be null for infinite edges or degenerate topology.
        if (!aVFirst.IsNull())
        {
          anEdgeEnt.StartVertex.VertexIdx =
            registerOrReuseVertex(theStorage, aVFirst,
                                  rawVertexPoint(aVFirst), BRep_Tool::Tolerance(aVFirst));
          anEdgeEnt.StartVertex.Orientation   = TopAbs_FORWARD;
          anEdgeEnt.StartVertex.LocalLocation = aVFirst.Location();
        }
        if (!aVLast.IsNull())
        {
          anEdgeEnt.EndVertex.VertexIdx =
            registerOrReuseVertex(theStorage, aVLast,
                                  rawVertexPoint(aVLast), BRep_Tool::Tolerance(aVLast));
          anEdgeEnt.EndVertex.Orientation   = TopAbs_REVERSED;
          anEdgeEnt.EndVertex.LocalLocation = aVLast.Location();
        }

        for (int anIntIdx = 0; anIntIdx < anInternalVerts.Length(); ++anIntIdx)
        {
          const ExtractedInternalVertex& anIntVtx = anInternalVerts.Value(anIntIdx);
          int anIntVtxIdx =
            registerOrReuseVertex(theStorage, anIntVtx.Shape, anIntVtx.Point, anIntVtx.Tolerance);
          if (anIntVtxIdx >= 0)
          {
            BRepGraphInc::VertexRef aVR;
            aVR.VertexIdx     = anIntVtxIdx;
            aVR.Orientation   = anIntVtx.Orientation;
            aVR.LocalLocation = anIntVtx.Shape.Location();
            anEdgeEnt.InternalVertices.Append(aVR);
          }
        }

        // Polygon3D: apply representation location for consistency.
        {
          TopLoc_Location aPoly3DLoc;
          occ::handle<Poly_Polygon3D> aPolygon3D = BRep_Tool::Polygon3D(anEdge, aPoly3DLoc);
          aPolygon3D =
            applyRepLocationToPolygon3D(aPolygon3D, anEdge.Location(), aPoly3DLoc);
          anEdgeEnt.Polygon3DRepIdx = getOrCreatePolygon3DRep(theStorage, aRepDedup,
                                                              aPolygon3D);
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
  registerFaceData(theStorage, aFaceData, aRepDedup);

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
    for (TopoDS_Iterator aChildIt(*aCompOrig, false, false); aChildIt.More(); aChildIt.Next())
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
                                   const bool                                    theParallel,
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
        for (TopoDS_Iterator aChildIt(theCurrentShape, false, false); aChildIt.More(); aChildIt.Next())
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
  RepDedup aRepDedup;
  registerFaceData(theStorage, aFaceData, aRepDedup);

  // Incrementally update reverse indices for newly appended entities only.
  theStorage.BuildDeltaReverseIndex(anOldNbEdges,
                                    anOldNbWires,
                                    anOldNbFaces,
                                    anOldNbShells,
                                    anOldNbSolids);

  theStorage.myIsDone = true;
}
