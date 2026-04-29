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
#include <BRepGraph_ParallelPolicy.hxx>
#include <BRepGraph_LayerParam.hxx>
#include <BRepGraph_LayerRegularity.hxx>
#include <BRepGraphInc_Storage.hxx>

#include <BRep_CurveOn2Surfaces.hxx>
#include <BRep_CurveOnClosedSurface.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <BRep_PointOnCurve.hxx>
#include <BRep_PointOnCurveOnSurface.hxx>
#include <BRep_PointOnSurface.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_TFace.hxx>
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
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>

#include <utility>

// Population pipeline overview:
//
// Phase 1 (sequential): Recursively traverse the TopoDS hierarchy
//   (Compound -> CompSolid -> Solid -> Shell), collecting face contexts
//   into a flat FaceLocalData vector. Registers container entities
//   (Compound, CompSolid, Solid, Shell) with TShape deduplication.
//
// Phase 2 (parallel): Per-face geometry extraction via OSD_Parallel.
//   Extracts surface, triangulations, wires, edges (with PCurves,
//   polygons, vertices) into ExtractedEdge/ExtractedWire/FaceLocalData
//   structs. No storage writes - thread-safe read-only access to TopoDS.
//
// Phase 3 (sequential): Register extracted data into BRepGraphInc_Storage.
//   Creates Face, Wire, Edge, CoEdge, Vertex entities with TShape dedup
//   and representation dedup (getOrCreate*Rep). Also runs optional
//   post-passes: edge regularities (3b) and vertex point reps (3c).
//
// Phase 4 (sequential): Build reverse indices for O(1) upward navigation.

namespace
{

//=================================================================================================

void appendFaceRef(BRepGraphInc_Storage&             theStorage,
                   const BRepGraph_ShellId           theParentShellId,
                   const BRepGraphInc::FaceInstance& theRef)
{
  const BRepGraph_FaceRefId aRefId  = theStorage.AppendFaceRef();
  BRepGraphInc::FaceRef&    anEntry = theStorage.ChangeFaceRef(aRefId);
  anEntry.ParentId                  = theParentShellId;
  anEntry.FaceDefId                 = theRef.DefId;
  anEntry.Orientation               = theRef.Orientation;
  anEntry.LocalLocation             = theRef.Location;
  theStorage.ChangeShell(theParentShellId).FaceRefIds.Append(aRefId);
}

//=================================================================================================

void appendWireRef(BRepGraphInc_Storage&             theStorage,
                   const BRepGraph_FaceId            theParentFaceId,
                   const BRepGraphInc::WireInstance& theRef)
{
  const BRepGraph_WireRefId aRefId  = theStorage.AppendWireRef();
  BRepGraphInc::WireRef&    anEntry = theStorage.ChangeWireRef(aRefId);
  anEntry.ParentId                  = theParentFaceId;
  anEntry.WireDefId                 = theRef.DefId;
  anEntry.IsOuter                   = theRef.IsOuter;
  anEntry.Orientation               = theRef.Orientation;
  anEntry.LocalLocation             = theRef.Location;
  theStorage.ChangeFace(theParentFaceId).WireRefIds.Append(aRefId);
}

//=================================================================================================

void appendCoEdgeRef(BRepGraphInc_Storage&               theStorage,
                     const BRepGraph_WireId              theParentWireId,
                     const BRepGraphInc::CoEdgeInstance& theRef)
{
  const BRepGraph_CoEdgeRefId aRefId  = theStorage.AppendCoEdgeRef();
  BRepGraphInc::CoEdgeRef&    anEntry = theStorage.ChangeCoEdgeRef(aRefId);
  anEntry.ParentId                    = theParentWireId;
  anEntry.CoEdgeDefId                 = theRef.DefId;
  anEntry.LocalLocation               = theRef.Location;
  theStorage.ChangeWire(theParentWireId).CoEdgeRefIds.Append(aRefId);
}

//=================================================================================================

BRepGraph_VertexRefId appendVertexRef(BRepGraphInc_Storage&               theStorage,
                                      const BRepGraph_NodeId              theParentId,
                                      const BRepGraphInc::VertexInstance& theRef)
{
  const BRepGraph_VertexRefId aRefId  = theStorage.AppendVertexRef();
  BRepGraphInc::VertexRef&    anEntry = theStorage.ChangeVertexRef(aRefId);
  anEntry.ParentId                    = theParentId;
  anEntry.VertexDefId                 = theRef.DefId;
  anEntry.Orientation                 = theRef.Orientation;
  anEntry.LocalLocation               = theRef.Location;
  return aRefId;
}

//=================================================================================================

void appendShellRef(BRepGraphInc_Storage&              theStorage,
                    const BRepGraph_SolidId            theParentSolidId,
                    const BRepGraphInc::ShellInstance& theRef)
{
  const BRepGraph_ShellRefId aRefId  = theStorage.AppendShellRef();
  BRepGraphInc::ShellRef&    anEntry = theStorage.ChangeShellRef(aRefId);
  anEntry.ParentId                   = theParentSolidId;
  anEntry.ShellDefId                 = theRef.DefId;
  anEntry.Orientation                = theRef.Orientation;
  anEntry.LocalLocation              = theRef.Location;
  theStorage.ChangeSolid(theParentSolidId).ShellRefIds.Append(aRefId);
}

//=================================================================================================

void appendSolidRef(BRepGraphInc_Storage&              theStorage,
                    const BRepGraph_CompSolidId        theParentCompSolidId,
                    const BRepGraphInc::SolidInstance& theRef)
{
  const BRepGraph_SolidRefId aRefId  = theStorage.AppendSolidRef();
  BRepGraphInc::SolidRef&    anEntry = theStorage.ChangeSolidRef(aRefId);
  anEntry.ParentId                   = theParentCompSolidId;
  anEntry.SolidDefId                 = theRef.DefId;
  anEntry.Orientation                = theRef.Orientation;
  anEntry.LocalLocation              = theRef.Location;
  theStorage.ChangeCompSolid(theParentCompSolidId).SolidRefIds.Append(aRefId);
}

//=================================================================================================

BRepGraph_ChildRefId appendChildRef(BRepGraphInc_Storage&             theStorage,
                                    const BRepGraph_NodeId            theParentId,
                                    const BRepGraphInc::NodeInstance& theRef)
{
  const BRepGraph_ChildRefId aRefId  = theStorage.AppendChildRef();
  BRepGraphInc::ChildRef&    anEntry = theStorage.ChangeChildRef(aRefId);
  anEntry.ParentId                   = theParentId;
  anEntry.ChildDefId                 = theRef.DefId;
  anEntry.Orientation                = theRef.Orientation;
  anEntry.LocalLocation              = theRef.Location;
  if (theParentId.NodeKind == BRepGraph_NodeId::Kind::Compound)
  {
    theStorage.ChangeCompound(BRepGraph_CompoundId(theParentId)).ChildRefIds.Append(aRefId);
  }
  return aRefId;
}

//=================================================================================================

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
  TopoDS_Edge                                       Shape;
  occ::handle<Geom_Curve>                           Curve3d;
  double                                            ParamFirst    = 0.0;
  double                                            ParamLast     = 0.0;
  double                                            Tolerance     = 0.0;
  bool                                              IsDegenerate  = false;
  bool                                              SameParameter = false;
  bool                                              SameRange     = false;
  ExtractedVertex                                   StartVertex;
  ExtractedVertex                                   EndVertex;
  NCollection_DynamicArray<ExtractedInternalVertex> InternalVertices;
  TopAbs_Orientation                                OrientationInWire = TopAbs_FORWARD;
  occ::handle<Geom2d_Curve>                         PCurve2d;
  double                                            PCFirst = 0.0;
  double                                            PCLast  = 0.0;
  occ::handle<Geom2d_Curve>                         PCurve2dReversed;
  double                                            PCFirstReversed  = 0.0;
  double                                            PCLastReversed   = 0.0;
  GeomAbs_Shape                                     PCurveContinuity = GeomAbs_C0;
  gp_Pnt2d                                          PCUV1;
  gp_Pnt2d                                          PCUV2;
  GeomAbs_Shape                                     SeamContinuity = GeomAbs_C0;
  occ::handle<Poly_Polygon3D>                       Polygon3D;
  occ::handle<Poly_Polygon2D>                       PolyOnSurf;
  occ::handle<Poly_Polygon2D>                       PolyOnSurfReversed;
};

//! Per-wire data extracted in parallel phase.
struct ExtractedWire
{
  TopoDS_Wire                             Shape;
  bool                                    IsOuter = false;
  NCollection_DynamicArray<ExtractedEdge> Edges;
};

//! All data extracted from a single face.
struct FaceLocalData
{
  // Phase 1 context.
  TopoDS_Face       Face;
  TopLoc_Location   ParentGlobalLoc;
  BRepGraph_ShellId ParentShellId;

  // Phase 2 extracted geometry.
  occ::handle<Geom_Surface>       Surface;
  occ::handle<Geom_Surface>       OriginalSurface; //!< Pre-transform surface for PCurve matching
  occ::handle<Poly_Triangulation> ActiveTriangulation;
  double                          Tolerance          = 0.0;
  TopAbs_Orientation              Orientation        = TopAbs_FORWARD;
  bool                            NaturalRestriction = false;
  NCollection_DynamicArray<ExtractedWire> Wires;
  NCollection_DynamicArray<ExtractedInternalVertex>
    DirectVertices; //!< INTERNAL/EXTERNAL vertex children
};

//! Extract stored PCurve(s) from edge for a given face's surface.
//! Iterates BRep_TEdge::Curves() directly, avoiding BRep_Tool::CurveOnSurface
//! which can compute phantom PCurves via CurveOnPlane for planar surfaces.
//! Uses multi-pass matching:
//!   Pass 1: exact (Surface, Location) match via IsCurveOnSurface(S, L)
//!   Pass 2: surface-handle-only fallback for TopLoc_Location structural equality bug
//!           (only when a unique CR matches the surface - prevents wrong-context selection)
//!   Pass 3: original (pre-transform) surface handle match when face surface
//!           was transformed via applyRepresentationLocation
//! For seam edges (IsCurveOnClosedSurface), extracts both PCurves + continuity.
//!
//! WARNING: Passes 2-3 are workarounds for TopLoc_Location structural equality
//! issues where an explicit identity datum does not compare equal to a default
//! empty identity. If the upstream TopLoc_Location comparison is fixed, passes
//! 2-3 may become redundant but should remain harmless (pass 1 will match first).
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
static bool extractStoredPCurves(
  const TopoDS_Edge&               theEdge,
  const TopoDS_Face&               theFace,
  occ::handle<Geom2d_Curve>&       thePCurve,
  occ::handle<Geom2d_Curve>&       thePCurve2,
  double&                          theFirst,
  double&                          theLast,
  GeomAbs_Shape&                   theSeamContinuity,
  const occ::handle<Geom_Surface>& theOrigSurface = occ::handle<Geom_Surface>())
{
  TopLoc_Location                  aFaceLoc;
  const occ::handle<Geom_Surface>& aSurf = BRep_Tool::Surface(theFace, aFaceLoc);
  if (aSurf.IsNull())
  {
    return false;
  }
  const occ::handle<BRep_TEdge> aTEdge = occ::down_cast<BRep_TEdge>(theEdge.TShape());
  if (aTEdge.IsNull())
  {
    return false;
  }
  const NCollection_List<occ::handle<BRep_CurveRepresentation>>& aCurves = aTEdge->Curves();
  const bool aReversed = (theEdge.Orientation() == TopAbs_REVERSED);

  // Expected CurveRepresentation location for this face+edge context.
  // This is the same formula used by BRep_Tool::CurveOnSurface internally.
  const TopLoc_Location aExpectedLoc = aFaceLoc.Predivided(theEdge.Location());

  // Lambda to extract PCurve data from a matched CurveRepresentation.
  const auto anExtractFromCR = [&](const occ::handle<BRep_CurveRepresentation>& theCR) -> bool {
    const BRep_GCurve* aGC = static_cast<const BRep_GCurve*>(theCR.get());
    aGC->Range(theFirst, theLast);
    if (aGC->IsCurveOnClosedSurface())
    {
      thePCurve         = aReversed ? aGC->PCurve2() : aGC->PCurve();
      thePCurve2        = aReversed ? aGC->PCurve() : aGC->PCurve2();
      theSeamContinuity = static_cast<const BRep_CurveOnClosedSurface*>(theCR.get())->Continuity();
    }
    else
    {
      thePCurve = aGC->PCurve();
    }
    return true;
  };

  // Pass 1: exact match by (Surface, Location). Correctly distinguishes
  // multiple CurveOnSurface entries for the same surface with different Locations.
  for (const occ::handle<BRep_CurveRepresentation>& aCR : aCurves)
  {
    if (!aCR->IsCurveOnSurface())
    {
      continue;
    }
    if (aCR->IsCurveOnSurface(aSurf, aExpectedLoc))
    {
      return anExtractFromCR(aCR);
    }
  }

  // Pass 1b: match using raw TFace.Location (preserves datum pointers).
  // When Pass 1 fails due to TopLoc_Location structural inequality (the composed
  // face.Location() * TFace.Location() creates a chain with different structure
  // than what was used during CR creation), retry with JUST TFace.Location().
  // The TFace.Location is a fixed TShape property whose datum pointers are shared
  // with the CR's stored location chain, enabling structural match to succeed.
  {
    const TopLoc_Location& aTFaceLoc =
      static_cast<const BRep_TFace*>(theFace.TShape().get())->Location();
    if (!aTFaceLoc.IsIdentity())
    {
      const TopLoc_Location aRawExpectedLoc = aTFaceLoc.Predivided(theEdge.Location());
      for (const occ::handle<BRep_CurveRepresentation>& aCR : aCurves)
      {
        if (!aCR->IsCurveOnSurface())
        {
          continue;
        }
        if (aCR->IsCurveOnSurface(aSurf, aRawExpectedLoc))
        {
          return anExtractFromCR(aCR);
        }
      }
    }
  }

  // Pass 2: fallback to surface-handle-only match.
  // Handles the TopLoc_Location structural equality bug where
  // an explicit identity datum does not compare equal to a default empty identity.
  for (const occ::handle<BRep_CurveRepresentation>& aCR : aCurves)
  {
    if (!aCR->IsCurveOnSurface() || aCR->Surface() != aSurf)
    {
      continue;
    }
    return anExtractFromCR(aCR);
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
    for (const occ::handle<BRep_CurveRepresentation>& aCR : aCurves)
    {
      if (!aCR->IsCurveOnSurface())
      {
        continue;
      }
      if (aCR->IsCurveOnSurface(theOrigSurface, aExpectedLoc))
      {
        return anExtractFromCR(aCR);
      }
    }
    // Pass 3a.5: raw TFace.Location match on original surface.
    {
      const TopLoc_Location& aTFaceLoc =
        static_cast<const BRep_TFace*>(theFace.TShape().get())->Location();
      if (!aTFaceLoc.IsIdentity())
      {
        const TopLoc_Location aRawExpectedLoc = aTFaceLoc.Predivided(theEdge.Location());
        for (const occ::handle<BRep_CurveRepresentation>& aCR : aCurves)
        {
          if (!aCR->IsCurveOnSurface())
          {
            continue;
          }
          if (aCR->IsCurveOnSurface(theOrigSurface, aRawExpectedLoc))
          {
            return anExtractFromCR(aCR);
          }
        }
      }
    }
    // Pass 3b: original surface handle-only match (location structural equality fallback).
    for (const occ::handle<BRep_CurveRepresentation>& aCR : aCurves)
    {
      if (!aCR->IsCurveOnSurface() || aCR->Surface() != theOrigSurface)
      {
        continue;
      }
      return anExtractFromCR(aCR);
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
//! Asserts on unhandled ShapeEnum (e.g., TopAbs_SHAPE).
BRepGraph_NodeId::Kind shapeTypeToNodeKind(TopAbs_ShapeEnum theType)
{
  switch (theType)
  {
    case TopAbs_COMPOUND:
      return BRepGraph_NodeId::Kind::Compound;
    case TopAbs_COMPSOLID:
      return BRepGraph_NodeId::Kind::CompSolid;
    case TopAbs_SOLID:
      return BRepGraph_NodeId::Kind::Solid;
    case TopAbs_SHELL:
      return BRepGraph_NodeId::Kind::Shell;
    case TopAbs_FACE:
      return BRepGraph_NodeId::Kind::Face;
    case TopAbs_WIRE:
      return BRepGraph_NodeId::Kind::Wire;
    case TopAbs_EDGE:
      return BRepGraph_NodeId::Kind::Edge;
    case TopAbs_VERTEX:
      return BRepGraph_NodeId::Kind::Vertex;
    default:
      Standard_ASSERT_VOID(false, "shapeTypeToNodeKind: unhandled ShapeEnum");
      return BRepGraph_NodeId::Kind::Solid; // unreachable in practice
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
  {
    return anExisting;
  }
  return nullptr;
}

//! Register a vertex entity by TShape dedup, or return the existing VertexId.
//! @param[in,out] theStorage   incidence storage
//! @param[in]     theVertex    original TopoDS_Vertex
//! @param[in]     thePoint     3D point (pre-extracted)
//! @param[in]     theTolerance vertex tolerance (pre-extracted)
//! @return typed VertexId, or default-constructed (invalid) if theVertex is null
BRepGraph_VertexId registerOrReuseVertex(BRepGraphInc_Storage& theStorage,
                                         const TopoDS_Vertex&  theVertex,
                                         const gp_Pnt&         thePoint,
                                         const double          theTolerance)
{
  if (theVertex.IsNull())
  {
    return BRepGraph_VertexId();
  }
  const BRepGraph_NodeId* anExisting =
    findExistingNode(theStorage, theVertex, BRepGraph_NodeId::Kind::Vertex);
  if (anExisting != nullptr)
  {
    return BRepGraph_VertexId(*anExisting);
  }

  const BRepGraph_VertexId aVtxId  = theStorage.AppendVertex();
  BRepGraphInc::VertexDef& aVtxEnt = theStorage.ChangeVertex(aVtxId);
  aVtxEnt.Point                    = thePoint;
  aVtxEnt.Tolerance                = theTolerance;
  theStorage.BindTShapeToNode(theVertex.TShape().get(), aVtxId);
  theStorage.BindOriginal(aVtxId, theVertex);
  return aVtxId;
}

//! Convenience overload: extracts point and tolerance from the vertex.
BRepGraph_VertexId registerOrReuseVertex(BRepGraphInc_Storage& theStorage,
                                         const TopoDS_Vertex&  theVertex)
{
  if (theVertex.IsNull())
  {
    return BRepGraph_VertexId();
  }
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
  {
    return theGeom;
  }
  // Do NOT use theCombinedLoc.IsIdentity() as an early return - TopLoc_Location
  // chain composition can structurally cancel to Identity (empty chain) even when
  // the actual repLoc (theShapeLoc^-1 * theCombinedLoc) is non-Identity.
  // This happens when the edge instance location and the CR location on TEdge
  // form inverse pairs that cancel in Multiplied().
  const TopLoc_Location aRepLoc = theShapeLoc.Inverted() * theCombinedLoc;
  if (aRepLoc.IsIdentity())
  {
    return theGeom;
  }
  return occ::down_cast<T>(theGeom->Transformed(aRepLoc.Transformation()));
}

//! Apply representation location to a Polygon3D by transforming its nodes.
static occ::handle<Poly_Polygon3D> applyRepLocationToPolygon3D(
  const occ::handle<Poly_Polygon3D>& thePolygon3D,
  const TopLoc_Location&             theShapeLoc,
  const TopLoc_Location&             theCombinedLoc)
{
  if (thePolygon3D.IsNull())
  {
    return thePolygon3D;
  }
  const TopLoc_Location aRepLoc = theShapeLoc.Inverted() * theCombinedLoc;
  if (aRepLoc.IsIdentity())
  {
    return thePolygon3D;
  }

  const gp_Trsf&                    aTrsf  = aRepLoc.Transformation();
  const NCollection_Array1<gp_Pnt>& aNodes = thePolygon3D->Nodes();
  NCollection_Array1<gp_Pnt>        aNewNodes(aNodes.Lower(), aNodes.Upper());
  for (int aNodeIdx = aNodes.Lower(); aNodeIdx <= aNodes.Upper(); ++aNodeIdx)
  {
    aNewNodes.SetValue(aNodeIdx, aNodes.Value(aNodeIdx).Transformed(aTrsf));
  }
  occ::handle<Poly_Polygon3D> aTransPoly;
  if (thePolygon3D->HasParameters())
  {
    aTransPoly = new Poly_Polygon3D(aNewNodes, thePolygon3D->Parameters());
  }
  else
  {
    aTransPoly = new Poly_Polygon3D(aNewNodes);
  }
  aTransPoly->Deflection(thePolygon3D->Deflection());
  return aTransPoly;
}

//! Deduplication maps for representation entities.
//! Keyed by raw Handle pointer - same underlying geometry object -> same rep entity.
struct RepDedup
{
  NCollection_DataMap<const Geom_Surface*, BRepGraph_SurfaceRepId>             Surfaces;
  NCollection_DataMap<const Geom_Curve*, BRepGraph_Curve3DRepId>               Curves3D;
  NCollection_DataMap<const Geom2d_Curve*, BRepGraph_Curve2DRepId>             Curves2D;
  NCollection_DataMap<const Poly_Triangulation*, BRepGraph_TriangulationRepId> Triangulations;
  NCollection_DataMap<const Poly_Polygon3D*, BRepGraph_Polygon3DRepId>         Polygons3D;
  NCollection_DataMap<const Poly_Polygon2D*, BRepGraph_Polygon2DRepId>         Polygons2D;
  NCollection_DataMap<const Poly_PolygonOnTriangulation*, BRepGraph_PolygonOnTriRepId>
    PolygonsOnTri;
};

//! Create or reuse a SurfaceRep for the given surface handle.
BRepGraph_SurfaceRepId getOrCreateSurfaceRep(BRepGraphInc_Storage&            theStorage,
                                             RepDedup&                        theDedup,
                                             const occ::handle<Geom_Surface>& theSurface)
{
  if (theSurface.IsNull())
  {
    return BRepGraph_SurfaceRepId();
  }
  const Geom_Surface*           aPtr       = theSurface.get();
  const BRepGraph_SurfaceRepId* anExisting = theDedup.Surfaces.Seek(aPtr);
  if (anExisting != nullptr)
  {
    return *anExisting;
  }
  const BRepGraph_SurfaceRepId aRepId = theStorage.AppendSurfaceRep();
  BRepGraphInc::SurfaceRep&    aRep   = theStorage.ChangeSurfaceRep(aRepId);
  aRep.Surface                        = theSurface;
  theDedup.Surfaces.Bind(aPtr, aRepId);
  return aRepId;
}

//! Create or reuse a Curve3DRep for the given 3D curve handle.
BRepGraph_Curve3DRepId getOrCreateCurve3DRep(BRepGraphInc_Storage&          theStorage,
                                             RepDedup&                      theDedup,
                                             const occ::handle<Geom_Curve>& theCurve)
{
  if (theCurve.IsNull())
  {
    return BRepGraph_Curve3DRepId();
  }
  const Geom_Curve*             aPtr       = theCurve.get();
  const BRepGraph_Curve3DRepId* anExisting = theDedup.Curves3D.Seek(aPtr);
  if (anExisting != nullptr)
  {
    return *anExisting;
  }
  const BRepGraph_Curve3DRepId aRepId = theStorage.AppendCurve3DRep();
  BRepGraphInc::Curve3DRep&    aRep   = theStorage.ChangeCurve3DRep(aRepId);
  aRep.Curve                          = theCurve;
  theDedup.Curves3D.Bind(aPtr, aRepId);
  return aRepId;
}

//! Create or reuse a Curve2DRep for the given 2D curve handle.
BRepGraph_Curve2DRepId getOrCreateCurve2DRep(BRepGraphInc_Storage&            theStorage,
                                             RepDedup&                        theDedup,
                                             const occ::handle<Geom2d_Curve>& theCurve)
{
  if (theCurve.IsNull())
  {
    return BRepGraph_Curve2DRepId();
  }
  const Geom2d_Curve*           aPtr       = theCurve.get();
  const BRepGraph_Curve2DRepId* anExisting = theDedup.Curves2D.Seek(aPtr);
  if (anExisting != nullptr)
  {
    return *anExisting;
  }
  const BRepGraph_Curve2DRepId aRepId = theStorage.AppendCurve2DRep();
  BRepGraphInc::Curve2DRep&    aRep   = theStorage.ChangeCurve2DRep(aRepId);
  aRep.Curve                          = theCurve;
  theDedup.Curves2D.Bind(aPtr, aRepId);
  return aRepId;
}

//! Create or reuse a TriangulationRep for the given triangulation handle.
BRepGraph_TriangulationRepId getOrCreateTriangulationRep(
  BRepGraphInc_Storage&                  theStorage,
  RepDedup&                              theDedup,
  const occ::handle<Poly_Triangulation>& theTriangulation)
{
  if (theTriangulation.IsNull())
  {
    return BRepGraph_TriangulationRepId();
  }
  const Poly_Triangulation*           aPtr       = theTriangulation.get();
  const BRepGraph_TriangulationRepId* anExisting = theDedup.Triangulations.Seek(aPtr);
  if (anExisting != nullptr)
  {
    return *anExisting;
  }
  const BRepGraph_TriangulationRepId aRepId = theStorage.AppendTriangulationRep();
  BRepGraphInc::TriangulationRep&    aRep   = theStorage.ChangeTriangulationRep(aRepId);
  aRep.Triangulation                        = theTriangulation;
  theDedup.Triangulations.Bind(aPtr, aRepId);
  return aRepId;
}

//! Create or reuse a Polygon3DRep for the given polygon handle.
BRepGraph_Polygon3DRepId getOrCreatePolygon3DRep(BRepGraphInc_Storage&              theStorage,
                                                 RepDedup&                          theDedup,
                                                 const occ::handle<Poly_Polygon3D>& thePolygon)
{
  if (thePolygon.IsNull())
  {
    return BRepGraph_Polygon3DRepId();
  }
  const Poly_Polygon3D*           aPtr       = thePolygon.get();
  const BRepGraph_Polygon3DRepId* anExisting = theDedup.Polygons3D.Seek(aPtr);
  if (anExisting != nullptr)
  {
    return *anExisting;
  }
  const BRepGraph_Polygon3DRepId aRepId = theStorage.AppendPolygon3DRep();
  BRepGraphInc::Polygon3DRep&    aRep   = theStorage.ChangePolygon3DRep(aRepId);
  aRep.Polygon                          = thePolygon;
  theDedup.Polygons3D.Bind(aPtr, aRepId);
  return aRepId;
}

//! Create or reuse a Polygon2DRep for the given polygon-on-surface handle.
BRepGraph_Polygon2DRepId getOrCreatePolygon2DRep(BRepGraphInc_Storage&              theStorage,
                                                 RepDedup&                          theDedup,
                                                 const occ::handle<Poly_Polygon2D>& thePolygon)
{
  if (thePolygon.IsNull())
  {
    return BRepGraph_Polygon2DRepId();
  }
  const Poly_Polygon2D*           aPtr       = thePolygon.get();
  const BRepGraph_Polygon2DRepId* anExisting = theDedup.Polygons2D.Seek(aPtr);
  if (anExisting != nullptr)
  {
    return *anExisting;
  }
  const BRepGraph_Polygon2DRepId aRepId = theStorage.AppendPolygon2DRep();
  BRepGraphInc::Polygon2DRep&    aRep   = theStorage.ChangePolygon2DRep(aRepId);
  aRep.Polygon                          = thePolygon;
  theDedup.Polygons2D.Bind(aPtr, aRepId);
  return aRepId;
}

//! Create or reuse a PolygonOnTriRep for the given polygon-on-triangulation handle.
//! theTriRepId is the global TriangulationRepId (not face-local).
BRepGraph_PolygonOnTriRepId getOrCreatePolygonOnTriRep(
  BRepGraphInc_Storage&                           theStorage,
  RepDedup&                                       theDedup,
  const occ::handle<Poly_PolygonOnTriangulation>& thePolygon,
  const BRepGraph_TriangulationRepId              theTriRepId)
{
  if (thePolygon.IsNull())
  {
    return BRepGraph_PolygonOnTriRepId();
  }
  const Poly_PolygonOnTriangulation* aPtr       = thePolygon.get();
  const BRepGraph_PolygonOnTriRepId* anExisting = theDedup.PolygonsOnTri.Seek(aPtr);
  if (anExisting != nullptr)
  {
    return *anExisting;
  }
  const BRepGraph_PolygonOnTriRepId aRepId = theStorage.AppendPolygonOnTriRep();
  BRepGraphInc::PolygonOnTriRep&    aRep   = theStorage.ChangePolygonOnTriRep(aRepId);
  aRep.Polygon                             = thePolygon;
  aRep.TriangulationRepId                  = theTriRepId;
  theDedup.PolygonsOnTri.Bind(aPtr, aRepId);
  return aRepId;
}

//! Register an edge entity from pre-extracted data, with TShape dedup.
//! Creates the entity (with vertices) if new, returns the EdgeId in both cases.
BRepGraph_EdgeId registerExtractedEdge(BRepGraphInc_Storage& theStorage,
                                       const ExtractedEdge&  theEdgeData,
                                       RepDedup&             theRepDedup)
{
  const BRepGraph_NodeId* anExisting =
    findExistingNode(theStorage, theEdgeData.Shape, BRepGraph_NodeId::Kind::Edge);
  if (anExisting != nullptr)
  {
    return BRepGraph_EdgeId(*anExisting);
  }

  const BRepGraph_EdgeId anEdgeId  = theStorage.AppendEdge();
  BRepGraphInc::EdgeDef& anEdgeEnt = theStorage.ChangeEdge(anEdgeId);
  anEdgeEnt.Tolerance              = theEdgeData.Tolerance;
  anEdgeEnt.IsDegenerate           = theEdgeData.IsDegenerate;
  anEdgeEnt.SameParameter          = theEdgeData.SameParameter;
  anEdgeEnt.SameRange              = theEdgeData.SameRange;
  anEdgeEnt.IsClosed               = theEdgeData.Shape.Closed();
  anEdgeEnt.ParamFirst             = theEdgeData.ParamFirst;
  anEdgeEnt.ParamLast              = theEdgeData.ParamLast;

  if (!theEdgeData.Curve3d.IsNull())
  {
    anEdgeEnt.Curve3DRepId = getOrCreateCurve3DRep(theStorage, theRepDedup, theEdgeData.Curve3d);
  }

  // Vertex registration (boundary vertices stored as VertexRefId into Ref table).
  // Vertices may be null for infinite edges or degenerate topology.
  if (!theEdgeData.StartVertex.Shape.IsNull())
  {
    BRepGraphInc::VertexInstance aVR;
    aVR.DefId                  = registerOrReuseVertex(theStorage,
                                      theEdgeData.StartVertex.Shape,
                                      theEdgeData.StartVertex.Point,
                                      theEdgeData.StartVertex.Tolerance);
    aVR.Orientation            = TopAbs_FORWARD;
    aVR.Location               = theEdgeData.StartVertex.Shape.Location();
    anEdgeEnt.StartVertexRefId = appendVertexRef(theStorage, anEdgeId, aVR);
  }
  if (!theEdgeData.EndVertex.Shape.IsNull())
  {
    BRepGraphInc::VertexInstance aVR;
    aVR.DefId                = registerOrReuseVertex(theStorage,
                                      theEdgeData.EndVertex.Shape,
                                      theEdgeData.EndVertex.Point,
                                      theEdgeData.EndVertex.Tolerance);
    aVR.Orientation          = TopAbs_REVERSED;
    aVR.Location             = theEdgeData.EndVertex.Shape.Location();
    anEdgeEnt.EndVertexRefId = appendVertexRef(theStorage, anEdgeId, aVR);
  }

  // Register internal/external vertices.
  for (const ExtractedInternalVertex& anIntVtx : theEdgeData.InternalVertices)
  {
    const BRepGraph_VertexId anIntVtxId =
      registerOrReuseVertex(theStorage, anIntVtx.Shape, anIntVtx.Point, anIntVtx.Tolerance);
    if (anIntVtxId.IsValid())
    {
      BRepGraphInc::VertexInstance aVR;
      aVR.DefId       = anIntVtxId;
      aVR.Orientation = anIntVtx.Orientation;
      aVR.Location    = anIntVtx.Shape.Location();
      anEdgeEnt.InternalVertexRefIds.Append(appendVertexRef(theStorage, anEdgeId, aVR));
    }
  }

  theStorage.BindTShapeToNode(theEdgeData.Shape.TShape().get(), anEdgeId);
  theStorage.BindOriginal(anEdgeId, theEdgeData.Shape);
  return anEdgeId;
}

//! Create a NodeInstance from a child shape, resolving its index via TShape lookup.
//! Returns true if the ref was created successfully (child was found in storage).
bool makeAuxChildRef(const BRepGraphInc_Storage& theStorage,
                     const TopoDS_Shape&         theChild,
                     BRepGraphInc::NodeInstance& theRef)
{
  const BRepGraph_NodeId* aChildNodeId = theStorage.FindNodeByTShape(theChild.TShape().get());
  if (aChildNodeId == nullptr)
  {
    return false;
  }
  theRef.DefId       = *aChildNodeId;
  theRef.Orientation = theChild.Orientation();
  theRef.Location    = theChild.Location();
  return true;
}

//! Extract first, last, and internal/external vertices from an edge.
//! Extract start (FORWARD), end (REVERSED), and internal vertices from an edge.
//! TopoDS_Iterator may yield multiple FORWARD or REVERSED vertices (rare but legal,
//! e.g., edges rebuilt by BRepTools_Modifier). In that case the *last* encountered
//! vertex of each orientation becomes the boundary vertex; earlier ones are demoted
//! to the internal list with their original orientation preserved. INTERNAL and
//! EXTERNAL vertices go directly to the internal list.
static void edgeVertices(const TopoDS_Edge&                                 theEdge,
                         TopoDS_Vertex&                                     theFirst,
                         TopoDS_Vertex&                                     theLast,
                         NCollection_DynamicArray<ExtractedInternalVertex>& theInternal)
{
  for (TopoDS_Iterator aVIt(theEdge, false, false); aVIt.More(); aVIt.Next())
  {
    if (aVIt.Value().ShapeType() != TopAbs_VERTEX)
    {
      continue;
    }
    const TopoDS_Vertex aVertex = TopoDS::Vertex(aVIt.Value());
    if (aVertex.Orientation() == TopAbs_FORWARD)
    {
      if (!theFirst.IsNull())
      {
        // Preserve previous FORWARD vertex in internal list.
        ExtractedInternalVertex& anIntVtx = theInternal.Appended();
        anIntVtx.Shape                    = theFirst;
        anIntVtx.Point                    = rawVertexPoint(theFirst);
        anIntVtx.Tolerance                = BRep_Tool::Tolerance(theFirst);
        anIntVtx.Orientation              = TopAbs_FORWARD;
      }
      theFirst = aVertex;
    }
    else if (aVertex.Orientation() == TopAbs_REVERSED)
    {
      if (!theLast.IsNull())
      {
        // Preserve previous REVERSED vertex in internal list.
        ExtractedInternalVertex& anIntVtx = theInternal.Appended();
        anIntVtx.Shape                    = theLast;
        anIntVtx.Point                    = rawVertexPoint(theLast);
        anIntVtx.Tolerance                = BRep_Tool::Tolerance(theLast);
        anIntVtx.Orientation              = TopAbs_REVERSED;
      }
      theLast = aVertex;
    }
    else
    {
      ExtractedInternalVertex& anIntVtx = theInternal.Appended();
      anIntVtx.Shape                    = aVertex;
      anIntVtx.Point                    = rawVertexPoint(aVertex);
      anIntVtx.Tolerance                = BRep_Tool::Tolerance(aVertex);
      anIntVtx.Orientation              = aVertex.Orientation();
    }
  }
  // Note: do NOT copy theFirst<->theLast when one is null.
  // Single-vertex edges (e.g., infinite edges) legitimately have only one vertex.
  // Closed edges already have both FORWARD and REVERSED vertices in the iterator.
}

//! Extract edge geometry and parametric data in a face context.
//! Fills theEdgeData with 3D curve, vertices, PCurves, and polygons.
static void extractEdgeInFace(ExtractedEdge&                   theEdgeData,
                              const TopoDS_Edge&               theEdge,
                              const TopoDS_Face&               theForwardFace,
                              const occ::handle<Geom_Surface>& theFaceSurface,
                              const occ::handle<Geom_Surface>& theOrigSurface)
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
    theEdgeData.Curve3d    = applyRepresentationLocation<Geom_Curve>(theEdgeData.Curve3d,
                                                                  theEdge.Location(),
                                                                  aCurveCombinedLoc);
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

    extractStoredPCurves(aFwdEdge,
                         theForwardFace,
                         theEdgeData.PCurve2d,
                         theEdgeData.PCurve2dReversed,
                         aPCFirst,
                         aPCLast,
                         aSeamContinuity,
                         theOrigSurface);

    theEdgeData.PCFirst          = aPCFirst;
    theEdgeData.PCLast           = aPCLast;
    theEdgeData.PCurveContinuity = BRep_Tool::MaxContinuity(theEdge);
    theEdgeData.SeamContinuity   = aSeamContinuity;

    // When the surface was transformed (TFace.Location != Identity -> theFaceSurface
    // differs from the raw TFace surface), the stored CR may belong to a different face
    // context using the same raw surface. Verify by calling BRep_Tool::CurveOnSurface
    // which correctly handles CurveOnPlane for planar surfaces and properly composes
    // face+edge locations. If BRep_Tool COMPUTED a PCurve (not stored) AND it differs
    // from what we extracted, our stored-only extraction picked a CR from the wrong
    // context. Discard ours so reconstruction doesn't attach the wrong PCurve.
    if (!theEdgeData.PCurve2d.IsNull() && theFaceSurface != theOrigSurface)
    {
      double                    aBTFirst = 0.0, aBTLast = 0.0;
      bool                      aBTIsStored = false;
      occ::handle<Geom2d_Curve> aBTPCurve =
        BRep_Tool::CurveOnSurface(aFwdEdge, theForwardFace, aBTFirst, aBTLast, &aBTIsStored);
      if (!aBTPCurve.IsNull() && !aBTIsStored && aBTPCurve.get() != theEdgeData.PCurve2d.get())
      {
        // BRep_Tool computed a different PCurve (CurveOnPlane) - our stored match
        // is from the wrong face context. Discard it.
        theEdgeData.PCurve2d.Nullify();
        theEdgeData.PCurve2dReversed.Nullify();
        theEdgeData.PCFirst        = 0.0;
        theEdgeData.PCLast         = 0.0;
        aPCFirst                   = 0.0;
        aPCLast                    = 0.0;
        theEdgeData.SeamContinuity = GeomAbs_C0;
      }
    }

    if (!theEdgeData.PCurve2d.IsNull() && !theFaceSurface.IsNull())
    {
      BRep_Tool::UVPoints(aFwdEdge, theForwardFace, theEdgeData.PCUV1, theEdgeData.PCUV2);
    }

    // For seam edges, extract reversed parameter range.
    // The reversed edge accesses PCurve2/PCurve (swapped), so Range gives the same values.
    // Fall back to primary range if extraction fails.
    if (!theEdgeData.PCurve2dReversed.IsNull())
    {
      const TopoDS_Edge         aRevEdge = TopoDS::Edge(theEdge.Oriented(TopAbs_REVERSED));
      occ::handle<Geom2d_Curve> aDummyPC1, aDummyPC2;
      GeomAbs_Shape             aDummyCont = GeomAbs_C0;
      if (!extractStoredPCurves(aRevEdge,
                                theForwardFace,
                                aDummyPC1,
                                aDummyPC2,
                                theEdgeData.PCFirstReversed,
                                theEdgeData.PCLastReversed,
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
    theEdgeData.PolyOnSurf     = BRep_Tool::PolygonOnSurface(aFwdEdge, theForwardFace);
    if (!theEdgeData.PolyOnSurf.IsNull())
    {
      const TopoDS_Edge           aRevEdge = TopoDS::Edge(theEdge.Oriented(TopAbs_REVERSED));
      occ::handle<Poly_Polygon2D> aPolyOnSurfRev =
        BRep_Tool::PolygonOnSurface(aRevEdge, theForwardFace);
      if (!aPolyOnSurfRev.IsNull() && aPolyOnSurfRev != theEdgeData.PolyOnSurf)
      {
        theEdgeData.PolyOnSurfReversed = aPolyOnSurfRev;
      }
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
    theData.Surface         = BRep_Tool::Surface(aFace, aSurfCombinedLoc);
    theData.OriginalSurface = theData.Surface; // save pre-transform handle for PCurve matching
    theData.Surface         = applyRepresentationLocation<Geom_Surface>(theData.Surface,
                                                                aFace.Location(),
                                                                aSurfCombinedLoc);
  }

  // Extract active triangulation only.
  {
    TopLoc_Location aDummyLoc;
    theData.ActiveTriangulation = BRep_Tool::Triangulation(aFace, aDummyLoc);
  }

  theData.Tolerance          = BRep_Tool::Tolerance(aFace);
  theData.Orientation        = aFace.Orientation();
  theData.NaturalRestriction = BRep_Tool::NaturalRestriction(aFace);

  const TopoDS_Face aForwardFace = TopoDS::Face(aFace.Oriented(TopAbs_FORWARD));
  const TopoDS_Wire anOuterWire  = BRepTools::OuterWire(aForwardFace);

  for (TopoDS_Iterator aChildIt(aForwardFace, false, false); aChildIt.More(); aChildIt.Next())
  {
    const TopoDS_Shape& aChild = aChildIt.Value();
    if (aChild.ShapeType() == TopAbs_VERTEX)
    {
      const TopoDS_Vertex&     aVertex  = TopoDS::Vertex(aChild);
      ExtractedInternalVertex& aVtxData = theData.DirectVertices.Appended();
      aVtxData.Shape                    = aVertex;
      aVtxData.Point                    = rawVertexPoint(aVertex);
      aVtxData.Tolerance                = BRep_Tool::Tolerance(aVertex);
      aVtxData.Orientation              = aVertex.Orientation();
      continue;
    }
    if (aChild.ShapeType() != TopAbs_WIRE)
    {
      continue;
    }
    const TopoDS_Wire& aWire = TopoDS::Wire(aChild);

    ExtractedWire aWireData;
    aWireData.Shape   = aWire;
    aWireData.IsOuter = aWire.IsSame(anOuterWire);

    for (TopoDS_Iterator anEdgeIt(aWire, false, false); anEdgeIt.More(); anEdgeIt.Next())
    {
      const TopoDS_Shape& anEdgeShape = anEdgeIt.Value();
      if (anEdgeShape.ShapeType() != TopAbs_EDGE)
      {
        continue;
      }
      ExtractedEdge& anEdgeData = aWireData.Edges.Appended();
      extractEdgeInFace(anEdgeData,
                        TopoDS::Edge(anEdgeShape),
                        aForwardFace,
                        theData.Surface,
                        theData.OriginalSurface);
    }

    theData.Wires.Append(std::move(aWireData));
  }
}

//! Register pre-extracted face data into incidence storage.
//! Uses unified TShapeToNodeId map and populates OriginalShapes.
void registerFaceData(BRepGraphInc_Storage&                          theStorage,
                      const NCollection_DynamicArray<FaceLocalData>& theFaceData,
                      RepDedup&                                      theRepDedup)
{
  for (const FaceLocalData& aData : theFaceData)
  {
    const TopoDS_Face& aCurFace = aData.Face;

    // Create or reuse FaceDef.
    const BRepGraph_NodeId* anExistingFace =
      findExistingNode(theStorage, aCurFace, BRepGraph_NodeId::Kind::Face);

    BRepGraph_FaceId aFaceId;
    bool             aIsNewFaceDef = false;
    if (anExistingFace != nullptr)
    {
      aFaceId = BRepGraph_FaceId(*anExistingFace);
    }
    else
    {
      aIsNewFaceDef                = true;
      aFaceId                      = theStorage.AppendFace();
      BRepGraphInc::FaceDef& aFace = theStorage.ChangeFace(aFaceId);
      aFace.Tolerance              = aData.Tolerance;
      aFace.NaturalRestriction     = aData.NaturalRestriction;
      aFace.SurfaceRepId           = getOrCreateSurfaceRep(theStorage, theRepDedup, aData.Surface);

      if (!aData.ActiveTriangulation.IsNull())
      {
        aFace.TriangulationRepId =
          getOrCreateTriangulationRep(theStorage, theRepDedup, aData.ActiveTriangulation);
      }

      theStorage.BindTShapeToNode(aCurFace.TShape().get(), aFaceId);
      theStorage.BindOriginal(aFaceId, aCurFace);
    }

    // Link face to parent shell (with per-instance location for shared TShapes).
    if (aData.ParentShellId.IsValid())
    {
      BRepGraphInc::FaceInstance aRef;
      aRef.DefId       = aFaceId;
      aRef.Orientation = aData.Orientation;
      aRef.Location    = aCurFace.Location();
      appendFaceRef(theStorage, aData.ParentShellId, aRef);
    }

    // Pre-fetch face entity for triangulation access in edge loop.
    BRepGraphInc::FaceDef&       aFaceMut = theStorage.ChangeFace(aFaceId);
    const BRepGraphInc::FaceDef& aFaceDef = aFaceMut;

    // Process wires - only for newly created face definitions.
    // Shared faces (same TShape referenced multiple times in a shell) must NOT
    // duplicate wire/edge/coedge data on the single FaceDef.
    if (!aIsNewFaceDef)
    {
      continue;
    }

    for (const ExtractedWire& aWireData : aData.Wires)
    {

      // Dedup wire by TShape.
      const BRepGraph_NodeId* anExistingWire =
        findExistingNode(theStorage, aWireData.Shape, BRepGraph_NodeId::Kind::Wire);

      BRepGraph_WireId aWireId;
      bool             aIsNewWireDef = false;

      if (anExistingWire != nullptr)
      {
        aWireId = BRepGraph_WireId(*anExistingWire);
      }
      else
      {
        aWireId = theStorage.AppendWire();
        theStorage.BindTShapeToNode(aWireData.Shape.TShape().get(), aWireId);
        theStorage.BindOriginal(aWireId, aWireData.Shape);
        aIsNewWireDef = true;
      }

      // Link wire to face.
      {
        BRepGraphInc::WireInstance aWireRef;
        aWireRef.DefId       = aWireId;
        aWireRef.Orientation = aWireData.Shape.Orientation();
        aWireRef.Location    = aWireData.Shape.Location();
        aWireRef.IsOuter     = aWireData.IsOuter;
        appendWireRef(theStorage, aFaceId, aWireRef);
      }

      for (const ExtractedEdge& anEdgeData : aWireData.Edges)
      {
        const BRepGraph_EdgeId anEdgeIdx =
          registerExtractedEdge(theStorage, anEdgeData, theRepDedup);

        // Cache mutable edge reference for subsequent PCurve/Polygon appends.
        // Safe: no new edges are appended within this scope.
        BRepGraphInc::EdgeDef& anEdgeMut = theStorage.ChangeEdge(anEdgeIdx);

        // Create CoEdge entity for this edge-face binding and add CoEdgeUsage to wire.
        BRepGraph_CoEdgeId aFwdCoEdgeId;
        BRepGraph_CoEdgeId aSeamCoEdgeId;
        if (aIsNewWireDef)
        {
          // Create the forward CoEdge.
          aFwdCoEdgeId                     = theStorage.AppendCoEdge();
          BRepGraphInc::CoEdgeDef& aCoEdge = theStorage.ChangeCoEdge(aFwdCoEdgeId);
          aCoEdge.EdgeDefId                = anEdgeIdx;
          aCoEdge.FaceDefId                = aFaceId;
          aCoEdge.Orientation              = anEdgeData.OrientationInWire;

          // Populate CoEdge with PCurve and polygon data for this face context.
          if (!anEdgeData.PCurve2d.IsNull())
          {
            aCoEdge.Curve2DRepId =
              getOrCreateCurve2DRep(theStorage, theRepDedup, anEdgeData.PCurve2d);
            aCoEdge.ParamFirst = anEdgeData.PCFirst;
            aCoEdge.ParamLast  = anEdgeData.PCLast;
            aCoEdge.Continuity = anEdgeData.PCurveContinuity;
            aCoEdge.UV1        = anEdgeData.PCUV1;
            aCoEdge.UV2        = anEdgeData.PCUV2;
          }
          aCoEdge.Polygon2DRepId =
            getOrCreatePolygon2DRep(theStorage, theRepDedup, anEdgeData.PolyOnSurf);

          // Handle seam edge: create a second CoEdge for the reversed sense.
          if (!anEdgeData.PCurve2dReversed.IsNull())
          {
            aSeamCoEdgeId                        = theStorage.AppendCoEdge();
            BRepGraphInc::CoEdgeDef& aSeamCoEdge = theStorage.ChangeCoEdge(aSeamCoEdgeId);
            aSeamCoEdge.EdgeDefId                = anEdgeIdx;
            aSeamCoEdge.FaceDefId                = aFaceId;
            aSeamCoEdge.Orientation              = TopAbs_REVERSED;
            aSeamCoEdge.Curve2DRepId =
              getOrCreateCurve2DRep(theStorage, theRepDedup, anEdgeData.PCurve2dReversed);
            aSeamCoEdge.ParamFirst     = anEdgeData.PCFirstReversed;
            aSeamCoEdge.ParamLast      = anEdgeData.PCLastReversed;
            aSeamCoEdge.Continuity     = anEdgeData.PCurveContinuity;
            aSeamCoEdge.SeamContinuity = anEdgeData.SeamContinuity;
            aSeamCoEdge.Polygon2DRepId =
              getOrCreatePolygon2DRep(theStorage, theRepDedup, anEdgeData.PolyOnSurfReversed);

            // Link seam pair.
            // Note: aCoEdge ref may be invalidated by AppendCoEdge, re-fetch.
            theStorage.ChangeCoEdge(aFwdCoEdgeId).SeamPairId = aSeamCoEdgeId;
            aSeamCoEdge.SeamPairId                           = aFwdCoEdgeId;
          }

          // Add CoEdgeUsage to wire with edge-in-wire Location.
          BRepGraphInc::CoEdgeInstance aCoEdgeRef;
          aCoEdgeRef.DefId    = aFwdCoEdgeId;
          aCoEdgeRef.Location = anEdgeData.Shape.Location();
          appendCoEdgeRef(theStorage, aWireId, aCoEdgeRef);

          // Note: seam coedge (if any) is NOT added to wire CoEdgeRefs --
          // it shares the same wire position as the forward coedge.
          // The seam pair is accessible via SeamPairId on the CoEdgeDef.
          // Only the forward coedge ref is in the wire's ordered list.
        }

        // Polygon3D (once per edge).
        if (!anEdgeData.Polygon3D.IsNull() && !anEdgeMut.Polygon3DRepId.IsValid())
        {
          anEdgeMut.Polygon3DRepId =
            getOrCreatePolygon3DRep(theStorage, theRepDedup, anEdgeData.Polygon3D);
        }

        // Polygon-on-triangulation: extract directly into CoEdge entities.
        if (aFwdCoEdgeId.IsValid())
        {
          TopLoc_Location aPolyTriLoc;
          const bool      hasSeamCoEdge = aSeamCoEdgeId.IsValid();
          TopoDS_Edge     aRevEdge;
          if (hasSeamCoEdge)
          {
            aRevEdge = TopoDS::Edge(anEdgeData.Shape.Reversed());
          }

          if (aFaceDef.TriangulationRepId.IsValid())
          {
            const occ::handle<Poly_Triangulation>& aTri =
              theStorage.TriangulationRep(aFaceDef.TriangulationRepId).Triangulation;
            if (!aTri.IsNull())
            {
              occ::handle<Poly_PolygonOnTriangulation> aPolyOnTri =
                BRep_Tool::PolygonOnTriangulation(anEdgeData.Shape, aTri, aPolyTriLoc);
              if (!aPolyOnTri.IsNull())
              {
                BRepGraph_TriangulationRepId aTriRepId = aFaceDef.TriangulationRepId;
                if (!aPolyTriLoc.IsIdentity())
                {
                  const TopLoc_Location aRepLoc =
                    anEdgeData.Shape.Location().Inverted() * aPolyTriLoc;
                  if (!aRepLoc.IsIdentity())
                  {
                    occ::handle<Poly_Triangulation> aTriCopy = aTri->Copy();
                    const gp_Trsf&                  aTrsf    = aRepLoc.Transformation();
                    for (int aNodeIdx = 1; aNodeIdx <= aTriCopy->NbNodes(); ++aNodeIdx)
                    {
                      aTriCopy->SetNode(aNodeIdx, aTriCopy->Node(aNodeIdx).Transformed(aTrsf));
                    }
                    aTriRepId = getOrCreateTriangulationRep(theStorage, theRepDedup, aTriCopy);
                    aFaceMut.TriangulationRepId = aTriRepId;
                  }
                }

                const BRepGraph_PolygonOnTriRepId aPolyOnTriRepId =
                  getOrCreatePolygonOnTriRep(theStorage, theRepDedup, aPolyOnTri, aTriRepId);

                theStorage.ChangeCoEdge(aFwdCoEdgeId).PolygonOnTriRepId = aPolyOnTriRepId;

                // Seam polygon-on-triangulation.
                if (hasSeamCoEdge)
                {
                  occ::handle<Poly_PolygonOnTriangulation> aPolyOnTriRev =
                    BRep_Tool::PolygonOnTriangulation(aRevEdge, aTri, aPolyTriLoc);
                  if (!aPolyOnTriRev.IsNull() && aPolyOnTriRev != aPolyOnTri)
                  {
                    const BRepGraph_PolygonOnTriRepId aSeamPolyRepId =
                      getOrCreatePolygonOnTriRep(theStorage, theRepDedup, aPolyOnTriRev, aTriRepId);

                    theStorage.ChangeCoEdge(aSeamCoEdgeId).PolygonOnTriRepId = aSeamPolyRepId;
                  }
                }
              }
            }
          }
        }
      }

      // Wire closure: copy directly from the original shape.
      if (aIsNewWireDef)
      {
        theStorage.ChangeWire(aWireId).IsClosed = aWireData.Shape.Closed();
      }
    }

    // Register direct vertex children of the face (INTERNAL/EXTERNAL).
    for (const ExtractedInternalVertex& aDirVtx : aData.DirectVertices)
    {
      const BRepGraph_VertexId aVtxId =
        registerOrReuseVertex(theStorage, aDirVtx.Shape, aDirVtx.Point, aDirVtx.Tolerance);
      if (aVtxId.IsValid())
      {
        BRepGraphInc::VertexInstance aVR;
        aVR.DefId       = aVtxId;
        aVR.Orientation = aDirVtx.Orientation;
        aVR.Location    = aDirVtx.Shape.Location();
        theStorage.ChangeFace(aFaceId).VertexRefIds.Append(
          appendVertexRef(theStorage, aFaceId, aVR));
      }
    }
  }
}

//! Recursively traverse a TopoDS hierarchy, registering container entities
//! (Compound, CompSolid, Solid, Shell) and collecting face contexts into theFaceData.
//! Used by Perform() for Phase 1.
void traverseHierarchy(BRepGraphInc_Storage&                    theStorage,
                       NCollection_DynamicArray<FaceLocalData>& theFaceData,
                       RepDedup&                                theRepDedup,
                       const TopoDS_Shape&                      theCurrentShape,
                       const TopLoc_Location&                   theParentGlobalLoc)
{
  if (theCurrentShape.IsNull())
  {
    return;
  }

  switch (theCurrentShape.ShapeType())
  {
    case TopAbs_COMPOUND: {
      const TopoDS_Compound& aCompound = TopoDS::Compound(theCurrentShape);
      if (findExistingNode(theStorage, aCompound, BRepGraph_NodeId::Kind::Compound))
      {
        break;
      }

      const BRepGraph_CompoundId aCompId = theStorage.AppendCompound();
      theStorage.BindTShapeToNode(aCompound.TShape().get(), aCompId);
      theStorage.BindOriginal(aCompId, aCompound);

      const TopLoc_Location aGlobalLoc = theParentGlobalLoc * aCompound.Location();

      for (TopoDS_Iterator aChildIt(aCompound, false, false); aChildIt.More(); aChildIt.Next())
      {
        const TopoDS_Shape&    aChild     = aChildIt.Value();
        BRepGraph_NodeId::Kind aChildKind = shapeTypeToNodeKind(aChild.ShapeType());

        traverseHierarchy(theStorage, theFaceData, theRepDedup, aChild, aGlobalLoc);

        if (aChild.ShapeType() != TopAbs_SHAPE)
        {
          // Resolve child index via TShape lookup (handles dedup correctly).
          // Face indices are deferred (invalid) because faces are registered in Phase 3;
          // resolved in the Phase 3a fixup pass after registerFaceData().
          uint32_t aChildIdx = BRepGraph_NodeId::THE_INVALID_INDEX;
          if (aChild.ShapeType() != TopAbs_FACE)
          {
            const BRepGraph_NodeId* aChildNodeId =
              theStorage.FindNodeByTShape(aChild.TShape().get());
            if (aChildNodeId != nullptr)
            {
              aChildIdx = aChildNodeId->Index;
            }
          }

          BRepGraphInc::NodeInstance aRef;
          aRef.DefId       = BRepGraph_NodeId(aChildKind, aChildIdx);
          aRef.Orientation = aChild.Orientation();
          aRef.Location    = aChild.Location();
          appendChildRef(theStorage, aCompId, aRef);
        }
      }
      break;
    }

    case TopAbs_COMPSOLID: {
      const TopoDS_CompSolid& aCompSolid = TopoDS::CompSolid(theCurrentShape);
      if (findExistingNode(theStorage, aCompSolid, BRepGraph_NodeId::Kind::CompSolid))
      {
        break;
      }

      const BRepGraph_CompSolidId aCSolidId = theStorage.AppendCompSolid();
      theStorage.BindTShapeToNode(aCompSolid.TShape().get(), aCSolidId);
      theStorage.BindOriginal(aCSolidId, aCompSolid);

      const TopLoc_Location aGlobalLoc = theParentGlobalLoc * aCompSolid.Location();

      for (TopoDS_Iterator aChildIt(aCompSolid, false, false); aChildIt.More(); aChildIt.Next())
      {
        if (aChildIt.Value().ShapeType() != TopAbs_SOLID)
        {
          continue;
        }
        traverseHierarchy(theStorage, theFaceData, theRepDedup, aChildIt.Value(), aGlobalLoc);

        const BRepGraph_NodeId* aSolidNodeId =
          theStorage.FindNodeByTShape(aChildIt.Value().TShape().get());
        if (aSolidNodeId == nullptr)
        {
          continue;
        }

        BRepGraphInc::SolidInstance aRef;
        aRef.DefId       = BRepGraph_SolidId::FromNodeId(*aSolidNodeId);
        aRef.Orientation = aChildIt.Value().Orientation();
        aRef.Location    = aChildIt.Value().Location();
        appendSolidRef(theStorage, aCSolidId, aRef);
      }
      break;
    }

    case TopAbs_SOLID: {
      const TopoDS_Solid& aSolid = TopoDS::Solid(theCurrentShape);
      if (findExistingNode(theStorage, aSolid, BRepGraph_NodeId::Kind::Solid))
      {
        break;
      }

      const BRepGraph_SolidId aSolidId = theStorage.AppendSolid();
      theStorage.BindTShapeToNode(aSolid.TShape().get(), aSolidId);
      theStorage.BindOriginal(aSolidId, aSolid);

      const TopLoc_Location aGlobalLoc = theParentGlobalLoc * aSolid.Location();

      for (TopoDS_Iterator aChildIt(aSolid, false, false); aChildIt.More(); aChildIt.Next())
      {
        const TopoDS_Shape& aChild = aChildIt.Value();
        traverseHierarchy(theStorage, theFaceData, theRepDedup, aChild, aGlobalLoc);

        if (aChild.ShapeType() == TopAbs_SHELL)
        {
          const BRepGraph_NodeId* aShellNodeId = theStorage.FindNodeByTShape(aChild.TShape().get());
          if (aShellNodeId == nullptr)
          {
            continue;
          }

          BRepGraphInc::ShellInstance aRef;
          aRef.DefId       = BRepGraph_ShellId::FromNodeId(*aShellNodeId);
          aRef.Orientation = aChild.Orientation();
          aRef.Location    = aChild.Location();
          appendShellRef(theStorage, aSolidId, aRef);
        }
        else if (aChild.ShapeType() == TopAbs_EDGE || aChild.ShapeType() == TopAbs_VERTEX)
        {
          BRepGraphInc::NodeInstance aCR;
          if (makeAuxChildRef(theStorage, aChild, aCR))
          {
            const BRepGraph_ChildRefId aChildRefId = appendChildRef(theStorage, aSolidId, aCR);
            theStorage.ChangeSolid(aSolidId).AuxChildRefIds.Append(aChildRefId);
          }
        }
      }
      break;
    }

    case TopAbs_SHELL: {
      const TopoDS_Shell& aShell = TopoDS::Shell(theCurrentShape);
      if (findExistingNode(theStorage, aShell, BRepGraph_NodeId::Kind::Shell))
      {
        break;
      }

      const BRepGraph_ShellId aShellId  = theStorage.AppendShell();
      BRepGraphInc::ShellDef& aShellEnt = theStorage.ChangeShell(aShellId);
      aShellEnt.IsClosed                = aShell.Closed();
      theStorage.BindTShapeToNode(aShell.TShape().get(), aShellId);
      theStorage.BindOriginal(aShellId, aShell);

      const TopLoc_Location aGlobalLoc = theParentGlobalLoc * aShell.Location();

      for (TopoDS_Iterator aChildIt(aShell, false, false); aChildIt.More(); aChildIt.Next())
      {
        const TopoDS_Shape& aChild = aChildIt.Value();
        if (aChild.ShapeType() == TopAbs_FACE)
        {
          FaceLocalData& aData  = theFaceData.Appended();
          aData.Face            = TopoDS::Face(aChild);
          aData.ParentGlobalLoc = aGlobalLoc;
          aData.ParentShellId   = aShellId;
        }
        else if (aChild.ShapeType() == TopAbs_WIRE || aChild.ShapeType() == TopAbs_EDGE)
        {
          traverseHierarchy(theStorage, theFaceData, theRepDedup, aChild, aGlobalLoc);

          BRepGraphInc::NodeInstance aCR;
          if (makeAuxChildRef(theStorage, aChild, aCR))
          {
            const BRepGraph_ChildRefId aChildRefId = appendChildRef(theStorage, aShellId, aCR);
            theStorage.ChangeShell(aShellId).AuxChildRefIds.Append(aChildRefId);
          }
        }
      }
      break;
    }

    case TopAbs_FACE: {
      FaceLocalData& aData  = theFaceData.Appended();
      aData.Face            = TopoDS::Face(theCurrentShape);
      aData.ParentGlobalLoc = theParentGlobalLoc;
      break;
    }

    case TopAbs_WIRE: {
      const TopoDS_Wire& aWire = TopoDS::Wire(theCurrentShape);
      if (findExistingNode(theStorage, aWire, BRepGraph_NodeId::Kind::Wire))
      {
        break;
      }

      const BRepGraph_WireId aWireId  = theStorage.AppendWire();
      BRepGraphInc::WireDef& aWireEnt = theStorage.ChangeWire(aWireId);
      aWireEnt.IsClosed               = aWire.Closed();
      theStorage.BindTShapeToNode(aWire.TShape().get(), aWireId);
      theStorage.BindOriginal(aWireId, aWire);

      for (TopoDS_Iterator anEdgeIt(aWire, false, false); anEdgeIt.More(); anEdgeIt.Next())
      {
        if (anEdgeIt.Value().ShapeType() != TopAbs_EDGE)
        {
          continue;
        }
        const TopoDS_Edge& anEdge = TopoDS::Edge(anEdgeIt.Value());

        // Recurse to create the edge entity (with dedup).
        traverseHierarchy(theStorage, theFaceData, theRepDedup, anEdge, theParentGlobalLoc);

        // Resolve edge index via TShape lookup (handles dedup correctly).
        const BRepGraph_NodeId* anEdgeNodeId = theStorage.FindNodeByTShape(anEdge.TShape().get());
        if (anEdgeNodeId != nullptr && anEdgeNodeId->NodeKind == BRepGraph_NodeId::Kind::Edge)
        {
          // Create CoEdge for free wire (no face context).
          const BRepGraph_CoEdgeId aCoEdgeId = theStorage.AppendCoEdge();
          BRepGraphInc::CoEdgeDef& aCoEdge   = theStorage.ChangeCoEdge(aCoEdgeId);
          aCoEdge.EdgeDefId                  = BRepGraph_EdgeId::FromNodeId(*anEdgeNodeId);
          aCoEdge.Orientation                = anEdge.Orientation();
          // FaceDefId left invalid for free wires.
          // Curve2d left null for free wires.

          BRepGraphInc::CoEdgeInstance aCoEdgeRef;
          aCoEdgeRef.DefId    = aCoEdgeId;
          aCoEdgeRef.Location = anEdge.Location();
          appendCoEdgeRef(theStorage, aWireId, aCoEdgeRef);
        }
      }
      break;
    }

    case TopAbs_EDGE: {
      const TopoDS_Edge& anEdge = TopoDS::Edge(theCurrentShape);
      if (findExistingNode(theStorage, anEdge, BRepGraph_NodeId::Kind::Edge))
      {
        break;
      }

      const BRepGraph_EdgeId anEdgeId  = theStorage.AppendEdge();
      BRepGraphInc::EdgeDef& anEdgeEnt = theStorage.ChangeEdge(anEdgeId);
      anEdgeEnt.Tolerance              = BRep_Tool::Tolerance(anEdge);
      anEdgeEnt.IsDegenerate           = BRep_Tool::Degenerated(anEdge);
      anEdgeEnt.SameParameter          = BRep_Tool::SameParameter(anEdge);
      anEdgeEnt.SameRange              = BRep_Tool::SameRange(anEdge);
      anEdgeEnt.IsClosed               = anEdge.Closed();

      // Extract 3D curve with representation location applied to definition frame.
      {
        double                  aFirst = 0.0, aLast = 0.0;
        TopLoc_Location         aCurveCombinedLoc;
        occ::handle<Geom_Curve> aCurve3d =
          BRep_Tool::Curve(anEdge, aCurveCombinedLoc, aFirst, aLast);
        anEdgeEnt.ParamFirst = aFirst;
        anEdgeEnt.ParamLast  = aLast;
        aCurve3d =
          applyRepresentationLocation<Geom_Curve>(aCurve3d, anEdge.Location(), aCurveCombinedLoc);
        anEdgeEnt.Curve3DRepId = getOrCreateCurve3DRep(theStorage, theRepDedup, aCurve3d);
      }

      // Extract vertices.
      TopoDS_Vertex                                     aVFirst, aVLast;
      NCollection_DynamicArray<ExtractedInternalVertex> anInternalVerts;
      edgeVertices(anEdge, aVFirst, aVLast, anInternalVerts);

      // Register vertices (using definition-frame points; Location stored on VertexRef).
      // Vertices may be null for infinite edges or degenerate topology.
      if (!aVFirst.IsNull())
      {
        BRepGraphInc::VertexInstance aVR;
        aVR.DefId                  = registerOrReuseVertex(theStorage,
                                          aVFirst,
                                          rawVertexPoint(aVFirst),
                                          BRep_Tool::Tolerance(aVFirst));
        aVR.Orientation            = TopAbs_FORWARD;
        aVR.Location               = aVFirst.Location();
        anEdgeEnt.StartVertexRefId = appendVertexRef(theStorage, anEdgeId, aVR);
      }
      if (!aVLast.IsNull())
      {
        BRepGraphInc::VertexInstance aVR;
        aVR.DefId                = registerOrReuseVertex(theStorage,
                                          aVLast,
                                          rawVertexPoint(aVLast),
                                          BRep_Tool::Tolerance(aVLast));
        aVR.Orientation          = TopAbs_REVERSED;
        aVR.Location             = aVLast.Location();
        anEdgeEnt.EndVertexRefId = appendVertexRef(theStorage, anEdgeId, aVR);
      }

      for (const ExtractedInternalVertex& anIntVtx : anInternalVerts)
      {
        const BRepGraph_VertexId anIntVtxId =
          registerOrReuseVertex(theStorage, anIntVtx.Shape, anIntVtx.Point, anIntVtx.Tolerance);
        if (anIntVtxId.IsValid())
        {
          BRepGraphInc::VertexInstance aVR;
          aVR.DefId       = anIntVtxId;
          aVR.Orientation = anIntVtx.Orientation;
          aVR.Location    = anIntVtx.Shape.Location();
          anEdgeEnt.InternalVertexRefIds.Append(appendVertexRef(theStorage, anEdgeId, aVR));
        }
      }

      // Polygon3D: apply representation location for consistency.
      {
        TopLoc_Location             aPoly3DLoc;
        occ::handle<Poly_Polygon3D> aPolygon3D = BRep_Tool::Polygon3D(anEdge, aPoly3DLoc);
        aPolygon3D = applyRepLocationToPolygon3D(aPolygon3D, anEdge.Location(), aPoly3DLoc);
        anEdgeEnt.Polygon3DRepId = getOrCreatePolygon3DRep(theStorage, theRepDedup, aPolygon3D);
      }

      theStorage.BindTShapeToNode(anEdge.TShape().get(), anEdgeId);
      theStorage.BindOriginal(anEdgeId, anEdge);
      break;
    }

    case TopAbs_VERTEX: {
      registerOrReuseVertex(theStorage, TopoDS::Vertex(theCurrentShape));
      break;
    }

    default:
      break;
  }
}

//! Append a root NodeId to the vector, skipping duplicates.
static void appendUniqueRootNode(NCollection_DynamicArray<BRepGraph_NodeId>& theRoots,
                                 const BRepGraph_NodeId&                     theNodeId)
{
  if (!theNodeId.IsValid())
  {
    return;
  }

  for (const BRepGraph_NodeId& aRoot : theRoots)
  {
    if (aRoot == theNodeId)
    {
      return;
    }
  }
  theRoots.Append(theNodeId);
}

//! Flatten hierarchy containers away for AppendFlattened().
//! Face roots are collected for the parallel face pipeline; standalone
//! wire/edge/vertex roots are registered directly through traverseHierarchy().
void flattenForAppend(BRepGraphInc_Storage&                       theStorage,
                      NCollection_DynamicArray<FaceLocalData>&    theFaceData,
                      RepDedup&                                   theRepDedup,
                      const TopoDS_Shape&                         theCurrentShape,
                      const TopLoc_Location&                      theParentGlobalLoc,
                      NCollection_DynamicArray<BRepGraph_NodeId>* theAppendedRoots)
{
  if (theCurrentShape.IsNull())
  {
    return;
  }

  switch (theCurrentShape.ShapeType())
  {
    case TopAbs_COMPOUND:
    case TopAbs_COMPSOLID:
    case TopAbs_SOLID:
    case TopAbs_SHELL: {
      for (TopoDS_Iterator aChildIt(theCurrentShape, false, false); aChildIt.More();
           aChildIt.Next())
      {
        flattenForAppend(theStorage,
                         theFaceData,
                         theRepDedup,
                         aChildIt.Value(),
                         theParentGlobalLoc * theCurrentShape.Location(),
                         theAppendedRoots);
      }
      break;
    }
    case TopAbs_FACE: {
      FaceLocalData& aData  = theFaceData.Appended();
      aData.Face            = TopoDS::Face(theCurrentShape);
      aData.ParentGlobalLoc = theParentGlobalLoc;
      break;
    }
    case TopAbs_WIRE:
    case TopAbs_EDGE:
    case TopAbs_VERTEX: {
      traverseHierarchy(theStorage, theFaceData, theRepDedup, theCurrentShape, theParentGlobalLoc);
      if (theAppendedRoots != nullptr)
      {
        const BRepGraph_NodeId* aNodeId =
          theStorage.FindNodeByTShape(theCurrentShape.TShape().get());
        if (aNodeId != nullptr)
        {
          appendUniqueRootNode(*theAppendedRoots, *aNodeId);
        }
      }
      break;
    }
    default:
      break;
  }
}

//=================================================================================================

void populateRegularityLayer(BRepGraphInc_Storage&                         theStorage,
                             BRepGraph_LayerRegularity*                    theRegularityLayer,
                             const bool                                    theExtractRegularities,
                             const uint32_t                                theOldNbEdges,
                             const occ::handle<NCollection_BaseAllocator>& theTmpAlloc)
{
  if (theRegularityLayer == nullptr)
  {
    return;
  }

  if (theOldNbEdges == 0)
  {
    theRegularityLayer->Clear();
  }
  if (!theExtractRegularities)
  {
    return;
  }

  // Surface-to-face map covers all faces (new edges may reference old faces).
  NCollection_DataMap<const Geom_Surface*, BRepGraph_FaceId> aSurfToFaceIdx(1, theTmpAlloc);
  const uint32_t                                             aNbFaces = theStorage.NbFaces();
  for (BRepGraph_FaceId aFaceId = BRepGraph_FaceId::Start(); aFaceId.IsValid(aNbFaces); ++aFaceId)
  {
    const TopoDS_Shape* anOrigFace = theStorage.FindOriginal(aFaceId);
    if (anOrigFace == nullptr || anOrigFace->IsNull())
    {
      continue;
    }

    TopLoc_Location           aLoc;
    occ::handle<Geom_Surface> aRawSurf = BRep_Tool::Surface(TopoDS::Face(*anOrigFace), aLoc);
    if (!aRawSurf.IsNull())
    {
      aSurfToFaceIdx.TryBind(aRawSurf.get(), aFaceId);
    }
  }

  // Only process new edges in incremental mode.
  const uint32_t aNbEdges = theStorage.NbEdges();
  for (BRepGraph_EdgeId anEdgeId(theOldNbEdges); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    const TopoDS_Shape* anOrigShape = theStorage.FindOriginal(anEdgeId);
    if (anOrigShape == nullptr || anOrigShape->IsNull())
    {
      continue;
    }

    const TopoDS_Edge&            anEdge = TopoDS::Edge(*anOrigShape);
    const occ::handle<BRep_TEdge> aTEdge = occ::down_cast<BRep_TEdge>(anEdge.TShape());
    if (aTEdge.IsNull())
    {
      continue;
    }

    for (const occ::handle<BRep_CurveRepresentation>& aCRep : aTEdge->Curves())
    {
      if (aCRep.IsNull())
      {
        continue;
      }

      const occ::handle<BRep_CurveOn2Surfaces> aCon2S =
        occ::down_cast<BRep_CurveOn2Surfaces>(aCRep);
      if (aCon2S.IsNull())
      {
        continue;
      }

      const Geom_Surface* aSurf1Ptr = aCon2S->Surface().get();
      const Geom_Surface* aSurf2Ptr = aCon2S->Surface2().get();
      if (aSurf1Ptr == nullptr || aSurf2Ptr == nullptr)
      {
        continue;
      }

      const BRepGraph_FaceId* aFaceIdx1 = aSurfToFaceIdx.Seek(aSurf1Ptr);
      const BRepGraph_FaceId* aFaceIdx2 = aSurfToFaceIdx.Seek(aSurf2Ptr);
      if (aFaceIdx1 == nullptr || aFaceIdx2 == nullptr)
      {
        continue;
      }

      theRegularityLayer->SetRegularity(anEdgeId, *aFaceIdx1, *aFaceIdx2, aCon2S->Continuity());
    }
  }
}

//=================================================================================================

void populateParamLayer(BRepGraphInc_Storage&                         theStorage,
                        BRepGraph_LayerParam*                         theParamLayer,
                        const bool                                    theExtractVertexPointReps,
                        const uint32_t                                theOldNbVertices,
                        const occ::handle<NCollection_BaseAllocator>& theTmpAlloc)
{
  if (theParamLayer == nullptr)
  {
    return;
  }

  if (theOldNbVertices == 0)
  {
    theParamLayer->Clear();
  }
  if (!theExtractVertexPointReps)
  {
    return;
  }

  NCollection_DataMap<const Geom_Curve*, BRepGraph_NodeId> aCurveToEdgeDef(1, theTmpAlloc);
  const uint32_t                                           aNbEdges = theStorage.NbEdges();
  for (BRepGraph_EdgeId anEdgeId = BRepGraph_EdgeId::Start(); anEdgeId.IsValid(aNbEdges);
       ++anEdgeId)
  {
    const TopoDS_Shape* anOrigEdge = theStorage.FindOriginal(anEdgeId);
    if (anOrigEdge == nullptr || anOrigEdge->IsNull())
    {
      continue;
    }

    double                  aFirst = 0.0;
    double                  aLast  = 0.0;
    TopLoc_Location         aLoc;
    occ::handle<Geom_Curve> aRawCurve =
      BRep_Tool::Curve(TopoDS::Edge(*anOrigEdge), aLoc, aFirst, aLast);
    if (!aRawCurve.IsNull())
    {
      aCurveToEdgeDef.TryBind(aRawCurve.get(), anEdgeId);
    }
  }

  NCollection_DataMap<const Geom_Surface*, BRepGraph_NodeId> aSurfToFaceDef(1, theTmpAlloc);
  NCollection_DynamicArray<const Geom_Surface*>              aFaceRawSurfaces(1, theTmpAlloc);
  const uint32_t                                             aNbFaces = theStorage.NbFaces();
  for (BRepGraph_FaceId aFaceId = BRepGraph_FaceId::Start(); aFaceId.IsValid(aNbFaces); ++aFaceId)
  {
    const TopoDS_Shape* anOrigFace  = theStorage.FindOriginal(aFaceId);
    const Geom_Surface* aRawSurfPtr = nullptr;
    if (anOrigFace != nullptr && !anOrigFace->IsNull())
    {
      TopLoc_Location           aLoc;
      occ::handle<Geom_Surface> aRawSurf = BRep_Tool::Surface(TopoDS::Face(*anOrigFace), aLoc);
      if (!aRawSurf.IsNull())
      {
        aSurfToFaceDef.TryBind(aRawSurf.get(), aFaceId);
        aRawSurfPtr = aRawSurf.get();
      }
    }
    aFaceRawSurfaces.Append(aRawSurfPtr);
  }

  NCollection_DataMap<const Geom2d_Curve*, NCollection_DynamicArray<BRepGraph_CoEdgeId>>
                 aPCurveToCoEdges(1, theTmpAlloc);
  const uint32_t aNbCoEdges = theStorage.NbCoEdges();
  for (BRepGraph_CoEdgeId aCoEdgeId(0); aCoEdgeId.IsValid(aNbCoEdges); ++aCoEdgeId)
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge = theStorage.CoEdge(aCoEdgeId);
    if (!aCoEdge.Curve2DRepId.IsValid() || !aCoEdge.FaceDefId.IsValid(theStorage.NbFaces()))
    {
      continue;
    }

    const occ::handle<Geom2d_Curve>& aPCurve = theStorage.Curve2DRep(aCoEdge.Curve2DRepId).Curve;
    if (aPCurve.IsNull())
    {
      continue;
    }

    NCollection_DynamicArray<BRepGraph_CoEdgeId>* aCoEdges =
      aPCurveToCoEdges.ChangeSeek(aPCurve.get());
    if (aCoEdges == nullptr)
    {
      NCollection_DynamicArray<BRepGraph_CoEdgeId> aNewCoEdges(1, theTmpAlloc);
      aNewCoEdges.Append(aCoEdgeId);
      aPCurveToCoEdges.Bind(aPCurve.get(), aNewCoEdges);
    }
    else
    {
      aCoEdges->Append(aCoEdgeId);
    }
  }

  // Only process new vertices in incremental mode.
  const uint32_t aNbVertices = theStorage.NbVertices();
  for (BRepGraph_VertexId aVertexId(theOldNbVertices); aVertexId.IsValid(aNbVertices); ++aVertexId)
  {
    const TopoDS_Shape* aVtxShape = theStorage.FindOriginal(aVertexId);
    if (aVtxShape == nullptr || aVtxShape->IsNull())
    {
      continue;
    }

    const TopoDS_Vertex&             aVertex  = TopoDS::Vertex(*aVtxShape);
    const occ::handle<BRep_TVertex>& aTVertex = occ::down_cast<BRep_TVertex>(aVertex.TShape());
    if (aTVertex.IsNull())
    {
      continue;
    }

    for (const occ::handle<BRep_PointRepresentation>& aPtRep : aTVertex->Points())
    {
      if (aPtRep.IsNull())
      {
        continue;
      }

      if (const occ::handle<BRep_PointOnCurve> aPOC = occ::down_cast<BRep_PointOnCurve>(aPtRep))
      {
        const BRepGraph_NodeId* anEdgeId = aCurveToEdgeDef.Seek(aPOC->Curve().get());
        if (anEdgeId != nullptr)
        {
          theParamLayer->SetPointOnCurve(aVertexId,
                                         BRepGraph_EdgeId::FromNodeId(*anEdgeId),
                                         aPOC->Parameter());
        }
      }
      else if (const occ::handle<BRep_PointOnCurveOnSurface> aPOCS =
                 occ::down_cast<BRep_PointOnCurveOnSurface>(aPtRep))
      {
        const NCollection_DynamicArray<BRepGraph_CoEdgeId>* aCandidates =
          aPCurveToCoEdges.Seek(aPOCS->PCurve().get());
        if (aCandidates == nullptr)
        {
          continue;
        }

        const Geom_Surface* aSurfacePtr = aPOCS->Surface().get();
        BRepGraph_CoEdgeId  aMatchedCoEdge;
        for (const BRepGraph_CoEdgeId& aCoEdgeId : *aCandidates)
        {
          const BRepGraphInc::CoEdgeDef& aCoEdge = theStorage.CoEdge(aCoEdgeId);
          if (!aCoEdge.FaceDefId.IsValidIn(aFaceRawSurfaces))
          {
            continue;
          }
          if (aFaceRawSurfaces.Value(static_cast<size_t>(aCoEdge.FaceDefId.Index)) == aSurfacePtr)
          {
            aMatchedCoEdge = aCoEdgeId;
            break;
          }
        }

        if (aMatchedCoEdge.IsValid())
        {
          theParamLayer->SetPointOnPCurve(aVertexId, aMatchedCoEdge, aPOCS->Parameter());
        }
      }
      else if (const occ::handle<BRep_PointOnSurface> aPOS =
                 occ::down_cast<BRep_PointOnSurface>(aPtRep))
      {
        const BRepGraph_NodeId* aFaceId = aSurfToFaceDef.Seek(aPOS->Surface().get());
        if (aFaceId != nullptr)
        {
          theParamLayer->SetPointOnSurface(aVertexId,
                                           BRepGraph_FaceId::FromNodeId(*aFaceId),
                                           aPOS->Parameter(),
                                           aPOS->Parameter2());
        }
      }
    }
  }
}

//=================================================================================================

void populateOptionalLayers(BRepGraphInc_Storage&                         theStorage,
                            BRepGraph_LayerParam*                         theParamLayer,
                            BRepGraph_LayerRegularity*                    theRegularityLayer,
                            const BRepGraphInc_Populate::Options&         theOptions,
                            const uint32_t                                theOldNbEdges,
                            const uint32_t                                theOldNbVertices,
                            const occ::handle<NCollection_BaseAllocator>& theTmpAlloc)
{
  populateRegularityLayer(theStorage,
                          theRegularityLayer,
                          theOptions.ExtractRegularities,
                          theOldNbEdges,
                          theTmpAlloc);
  populateParamLayer(theStorage,
                     theParamLayer,
                     theOptions.ExtractVertexPointReps,
                     theOldNbVertices,
                     theTmpAlloc);
}

} // anonymous namespace

//=================================================================================================

void BRepGraphInc_Populate::Perform(BRepGraphInc_Storage&      theStorage,
                                    const TopoDS_Shape&        theShape,
                                    const bool                 theParallel,
                                    const Options&             theOptions,
                                    BRepGraph_LayerParam*      theParamLayer,
                                    BRepGraph_LayerRegularity* theRegularityLayer,
                                    const occ::handle<NCollection_BaseAllocator>& theTmpAlloc)
{
  theStorage.Clear();

  if (theShape.IsNull())
  {
    return;
  }

  // Use temporary allocator if provided, else default.
  // Must NOT use the storage's persistent allocator for scratch data.
  const occ::handle<NCollection_BaseAllocator>& aTmpAlloc =
    !theTmpAlloc.IsNull() ? theTmpAlloc : NCollection_BaseAllocator::CommonBaseAllocator();
  const int aParallelWorkers = theParallel ? BRepGraph_ParallelPolicy::WorkerCount() : 1;

  // Phase 1 (sequential): Recursively explore hierarchy, collecting face contexts.
  NCollection_DynamicArray<FaceLocalData> aFaceData(256, aTmpAlloc);
  RepDedup                                aRepDedup;

  traverseHierarchy(theStorage, aFaceData, aRepDedup, theShape, TopLoc_Location());

  // Phase 2 (parallel): Extract per-face geometry/topology.
  BRepGraph_ParallelPolicy::Workload aFaceExtractWork;
  aFaceExtractWork.PrimaryItems = aFaceData.Length();
  const bool isParallelFaceExtraction =
    BRepGraph_ParallelPolicy::ShouldRun(theParallel, aParallelWorkers, aFaceExtractWork);
  OSD_Parallel::For(
    0,
    aFaceData.Length(),
    [&](const int theIndex) { extractFaceData(aFaceData.ChangeValue(theIndex)); },
    !isParallelFaceExtraction);

  // Phase 3 (sequential): Register into storage with deduplication.
  registerFaceData(theStorage, aFaceData, aRepDedup);

  // Phase 3a: Fix compound Face ChildRefs (face indices were unknown during Phase 1,
  // resolved now after registerFaceData). All other child types were resolved
  // immediately in traverseShape via FindNodeByTShape.
  const uint32_t aNbCompounds = theStorage.myCompounds.Nb();
  for (BRepGraph_CompoundId aCompoundId(0); aCompoundId.IsValid(aNbCompounds); ++aCompoundId)
  {
    const BRepGraph_NodeId aParentId(aCompoundId);
    const TopoDS_Shape*    aCompOrig = theStorage.myOriginalShapes.Seek(aParentId);
    if (aCompOrig == nullptr)
    {
      continue;
    }

    uint32_t aRefOrd = 0;
    for (TopoDS_Iterator aChildIt(*aCompOrig, false, false); aChildIt.More(); aChildIt.Next())
    {
      BRepGraphInc::ChildRef* aRef         = nullptr;
      uint32_t                aCurrentOrd  = 0;
      const uint32_t          aNbChildRefs = theStorage.NbChildRefs();
      for (BRepGraph_ChildRefId aChildRefId = BRepGraph_ChildRefId::Start();
           aChildRefId.IsValid(aNbChildRefs);
           ++aChildRefId)
      {
        BRepGraphInc::ChildRef& aCandidate = theStorage.ChangeChildRef(aChildRefId);
        if (aCandidate.ParentId != aParentId || aCandidate.IsRemoved)
        {
          continue;
        }
        if (aCurrentOrd == aRefOrd)
        {
          aRef = &aCandidate;
          break;
        }
        ++aCurrentOrd;
      }

      if (aRef == nullptr)
      {
        break;
      }

      if (!aRef->ChildDefId.IsValid())
      {
        const BRepGraph_NodeId* aNodeId =
          theStorage.myTShapeToNodeId.Seek(aChildIt.Value().TShape().get());
        if (aNodeId != nullptr
            && aNodeId->NodeKind == shapeTypeToNodeKind(aChildIt.Value().ShapeType()))
        {
          aRef->ChildDefId = *aNodeId;
        }
      }
      ++aRefOrd;
    }
  }

  populateOptionalLayers(theStorage,
                         theParamLayer,
                         theRegularityLayer,
                         theOptions,
                         0,
                         0,
                         aTmpAlloc);

  // Build reverse indices.
  theStorage.BuildReverseIndex();

  theStorage.myIsDone = true;
}

//=================================================================================================

void BRepGraphInc_Populate::AppendFlattened(
  BRepGraphInc_Storage&                         theStorage,
  const TopoDS_Shape&                           theShape,
  const bool                                    theParallel,
  NCollection_DynamicArray<BRepGraph_NodeId>&   theAppendedRoots,
  const Options&                                theOptions,
  BRepGraph_LayerParam*                         theParamLayer,
  BRepGraph_LayerRegularity*                    theRegularityLayer,
  const occ::handle<NCollection_BaseAllocator>& theTmpAlloc)
{
  if (theShape.IsNull())
  {
    return;
  }

  // Use temporary allocator if provided, else default.
  // Must NOT use the storage's persistent allocator for scratch data.
  const occ::handle<NCollection_BaseAllocator>& aTmpAlloc =
    !theTmpAlloc.IsNull() ? theTmpAlloc : NCollection_BaseAllocator::CommonBaseAllocator();
  const int aParallelWorkers = theParallel ? BRepGraph_ParallelPolicy::WorkerCount() : 1;

  // Snapshot entity counts before appending, for incremental updates.
  const uint32_t anOldNbEdges      = theStorage.NbEdges();
  const uint32_t anOldNbWires      = theStorage.NbWires();
  const uint32_t anOldNbFaces      = theStorage.NbFaces();
  const uint32_t anOldNbShells     = theStorage.NbShells();
  const uint32_t anOldNbSolids     = theStorage.NbSolids();
  const uint32_t anOldNbCompounds  = theStorage.NbCompounds();
  const uint32_t anOldNbCompSolids = theStorage.NbCompSolids();
  const uint32_t anOldNbVertices   = theStorage.NbVertices();
  const uint32_t anOldNbChildRefs  = theStorage.NbChildRefs();
  const uint32_t anOldNbSolidRefs  = theStorage.NbSolidRefs();

  // Collect face contexts by flattening hierarchy.
  NCollection_DynamicArray<FaceLocalData> aFaceData(256, aTmpAlloc);
  RepDedup                                aRepDedup;

  flattenForAppend(theStorage,
                   aFaceData,
                   aRepDedup,
                   theShape,
                   TopLoc_Location(),
                   &theAppendedRoots);

  // Parallel face extraction.
  BRepGraph_ParallelPolicy::Workload aFaceExtractWork;
  aFaceExtractWork.PrimaryItems = aFaceData.Length();
  const bool isParallelFaceExtraction =
    BRepGraph_ParallelPolicy::ShouldRun(theParallel, aParallelWorkers, aFaceExtractWork);
  OSD_Parallel::For(
    0,
    aFaceData.Length(),
    [&](const int theIndex) { extractFaceData(aFaceData.ChangeValue(theIndex)); },
    !isParallelFaceExtraction);

  // Sequential registration (reuses existing dedup maps).
  registerFaceData(theStorage, aFaceData, aRepDedup);

  for (const FaceLocalData& aFaceDataElem : aFaceData)
  {
    const BRepGraph_NodeId* aFaceNodeId =
      theStorage.FindNodeByTShape(aFaceDataElem.Face.TShape().get());
    if (aFaceNodeId != nullptr)
    {
      appendUniqueRootNode(theAppendedRoots, *aFaceNodeId);
    }
  }

  populateOptionalLayers(theStorage,
                         theParamLayer,
                         theRegularityLayer,
                         theOptions,
                         anOldNbEdges,
                         anOldNbVertices,
                         aTmpAlloc);

  // Incrementally update reverse indices for newly appended entities only.
  theStorage.BuildDeltaReverseIndex(anOldNbEdges,
                                    anOldNbWires,
                                    anOldNbFaces,
                                    anOldNbShells,
                                    anOldNbSolids,
                                    anOldNbCompounds,
                                    anOldNbCompSolids,
                                    anOldNbChildRefs,
                                    anOldNbSolidRefs);

  theStorage.myIsDone = true;
}

//=================================================================================================

void BRepGraphInc_Populate::Append(BRepGraphInc_Storage&                         theStorage,
                                   const TopoDS_Shape&                           theShape,
                                   const bool                                    theParallel,
                                   const Options&                                theOptions,
                                   BRepGraph_LayerParam*                         theParamLayer,
                                   BRepGraph_LayerRegularity*                    theRegularityLayer,
                                   const occ::handle<NCollection_BaseAllocator>& theTmpAlloc)
{
  if (theShape.IsNull())
  {
    return;
  }

  const occ::handle<NCollection_BaseAllocator>& aTmpAlloc =
    !theTmpAlloc.IsNull() ? theTmpAlloc : NCollection_BaseAllocator::CommonBaseAllocator();
  const int aParallelWorkers = theParallel ? BRepGraph_ParallelPolicy::WorkerCount() : 1;

  // Snapshot entity counts before appending, for incremental updates.
  const uint32_t anOldNbEdges      = theStorage.NbEdges();
  const uint32_t anOldNbWires      = theStorage.NbWires();
  const uint32_t anOldNbFaces      = theStorage.NbFaces();
  const uint32_t anOldNbShells     = theStorage.NbShells();
  const uint32_t anOldNbSolids     = theStorage.NbSolids();
  const uint32_t anOldNbVertices   = theStorage.NbVertices();
  const uint32_t anOldNbCompounds  = theStorage.NbCompounds();
  const uint32_t anOldNbCompSolids = theStorage.NbCompSolids();
  const uint32_t anOldNbChildRefs  = theStorage.NbChildRefs();
  const uint32_t anOldNbSolidRefs  = theStorage.NbSolidRefs();

  // Phase 1 (sequential): Traverse the full hierarchy.
  // Existing shapes are deduplicated via findExistingNode; only new shapes are added.
  NCollection_DynamicArray<FaceLocalData> aFaceData(256, aTmpAlloc);
  RepDedup                                aRepDedup;
  traverseHierarchy(theStorage, aFaceData, aRepDedup, theShape, TopLoc_Location());

  // Phase 2 (parallel): Per-face geometry extraction.
  BRepGraph_ParallelPolicy::Workload aFaceExtractWork;
  aFaceExtractWork.PrimaryItems = aFaceData.Length();
  const bool isParallelFaceExtraction =
    BRepGraph_ParallelPolicy::ShouldRun(theParallel, aParallelWorkers, aFaceExtractWork);
  OSD_Parallel::For(
    0,
    aFaceData.Length(),
    [&](const int theIndex) { extractFaceData(aFaceData.ChangeValue(theIndex)); },
    !isParallelFaceExtraction);

  // Phase 3 (sequential): Register into storage with deduplication.
  registerFaceData(theStorage, aFaceData, aRepDedup);

  // Phase 3a: Fix compound ChildRef linkages in NEWLY APPENDED compounds only.
  // Pre-existing compounds are not re-processed - Append assumes complete subgraph
  // hierarchies with no cross-references to existing containers.
  //
  // Cost is O(total children across new compounds). Earlier versions rescanned every
  // ChildRef in the storage for each child (O(children x total refs)); the new compound's
  // own ChildRefIds vector is already in order, so we iterate it directly.
  for (BRepGraph_CompoundId aCompoundId(anOldNbCompounds);
       aCompoundId.IsValid(theStorage.myCompounds.Nb());
       ++aCompoundId)
  {
    const BRepGraph_NodeId aCompoundNode = BRepGraph_NodeId(aCompoundId);
    const TopoDS_Shape*    aCompOrig     = theStorage.myOriginalShapes.Seek(aCompoundNode);
    if (aCompOrig == nullptr)
    {
      continue;
    }

    const BRepGraphInc::CompoundDef& aCompDef     = theStorage.Compound(aCompoundId);
    const size_t                     aNbOwnedRefs = aCompDef.ChildRefIds.Size();

    size_t aRefOrd = 0;
    for (TopoDS_Iterator aChildIt(*aCompOrig, false, false); aChildIt.More(); aChildIt.Next())
    {
      if (aRefOrd >= aNbOwnedRefs)
      {
        break;
      }
      const BRepGraph_ChildRefId aOwnedRefId = aCompDef.ChildRefIds.Value(aRefOrd);
      BRepGraphInc::ChildRef&    aRef        = theStorage.ChangeChildRef(aOwnedRefId);

      if (aRef.IsRemoved || aRef.ParentId != aCompoundNode)
      {
        ++aRefOrd;
        continue;
      }

      if (!aRef.ChildDefId.IsValid())
      {
        const BRepGraph_NodeId* aNodeId =
          theStorage.myTShapeToNodeId.Seek(aChildIt.Value().TShape().get());
        if (aNodeId != nullptr
            && aNodeId->NodeKind == shapeTypeToNodeKind(aChildIt.Value().ShapeType()))
        {
          aRef.ChildDefId = *aNodeId;
        }
      }
      ++aRefOrd;
    }
  }

  populateOptionalLayers(theStorage,
                         theParamLayer,
                         theRegularityLayer,
                         theOptions,
                         anOldNbEdges,
                         anOldNbVertices,
                         aTmpAlloc);

  // Incrementally update reverse indices for newly appended entities only.
  theStorage.BuildDeltaReverseIndex(anOldNbEdges,
                                    anOldNbWires,
                                    anOldNbFaces,
                                    anOldNbShells,
                                    anOldNbSolids,
                                    anOldNbCompounds,
                                    anOldNbCompSolids,
                                    anOldNbChildRefs,
                                    anOldNbSolidRefs);

  theStorage.myIsDone = true;
}
