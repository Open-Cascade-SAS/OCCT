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

#include <BRepGraph_Tool.hxx>

#include <Adaptor3d_CurveOnSurface.hxx>
#include "BRepGraph_CacheDerivedState.hxx"
#include "BRepGraph_CacheRegistry.hxx"
#include <BRepGraph_DefsIterator.hxx>
#include <BRepGraph_RefsIterator.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_ReverseIterator.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <GeomProjLib.hxx>
#include <Precision.hxx>
#include <Standard_Failure.hxx>

#include <algorithm>
#include <utility>

namespace
{
bool isValidCurveRange(const occ::handle<Geom_Curve>& theCurve,
                       const double                   theFirst,
                       const double                   theLast)
{
  if (theCurve.IsNull() || Precision::IsInfinite(theFirst) || Precision::IsInfinite(theLast)
      || !(theFirst < theLast))
  {
    return false;
  }

  return theCurve->IsPeriodic()
         || (theCurve->FirstParameter() - theFirst <= Precision::PConfusion()
             && theLast - theCurve->LastParameter() <= Precision::PConfusion());
}

bool isValidCurveRange(const occ::handle<Geom2d_Curve>& theCurve,
                       const double                     theFirst,
                       const double                     theLast,
                       const bool                       theAllowEqual)
{
  if (theCurve.IsNull() || Precision::IsInfinite(theFirst) || Precision::IsInfinite(theLast)
      || (theAllowEqual ? theFirst > theLast : !(theFirst < theLast)))
  {
    return false;
  }

  return theCurve->IsPeriodic()
         || (theCurve->FirstParameter() - theFirst <= Precision::PConfusion()
             && theLast - theCurve->LastParameter() <= Precision::PConfusion());
}

enum class StoredPCurveStatus
{
  Absent,
  Usable,
  Malformed
};

struct StoredPCurveResult
{
  Geom2dAdaptor_Curve Adaptor;
  StoredPCurveStatus  Status = StoredPCurveStatus::Absent;
};

template <typename T>
occ::handle<T> findOrCreateCache(BRepGraph& theGraph)
{
  return theGraph.CacheRegistry().Ensure<T>();
}

template <typename T>
occ::handle<T> findOrCreateCache(const BRepGraph& theGraph)
{
  return const_cast<BRepGraph&>(theGraph).CacheRegistry().Ensure<T>();
}

BRepGraph_FaceId faceOf(const BRepGraph& theGraph, const BRepGraph_FaceRefId theFaceRef)
{
  if (!theFaceRef.IsValid(theGraph.Refs().Faces().Nb())
      || theGraph.Refs().Gen().IsRemoved(theFaceRef))
  {
    return BRepGraph_FaceId();
  }
  const BRepGraph_FaceId aFace = theGraph.Refs().Faces().Entry(theFaceRef).ChildFaceId;
  return aFace.IsValid(theGraph.Topo().Faces().Nb()) && !aFace.IsRemoved(theGraph)
           ? aFace
           : BRepGraph_FaceId();
}

BRepGraph_WireId wireOf(const BRepGraph& theGraph, const BRepGraph_WireRefId theWireRef)
{
  if (!theWireRef.IsValid(theGraph.Refs().Wires().Nb())
      || theGraph.Refs().Gen().IsRemoved(theWireRef))
  {
    return BRepGraph_WireId();
  }
  const BRepGraph_WireId aWire = theGraph.Refs().Wires().Entry(theWireRef).ChildWireId;
  return aWire.IsValid(theGraph.Topo().Wires().Nb()) && !aWire.IsRemoved(theGraph)
           ? aWire
           : BRepGraph_WireId();
}

BRepGraph_ShellId shellOf(const BRepGraph& theGraph, const BRepGraph_ShellRefId theShellRef)
{
  if (!theShellRef.IsValid(theGraph.Refs().Shells().Nb())
      || theGraph.Refs().Gen().IsRemoved(theShellRef))
  {
    return BRepGraph_ShellId();
  }
  const BRepGraph_ShellId aShell = theGraph.Refs().Shells().Entry(theShellRef).ChildShellId;
  return aShell.IsValid(theGraph.Topo().Shells().Nb()) && !aShell.IsRemoved(theGraph)
           ? aShell
           : BRepGraph_ShellId();
}

occ::handle<BRepGraph_CacheDerivedState> derivedStateCache(const BRepGraph& theGraph)
{
  return const_cast<BRepGraph&>(theGraph).CacheRegistry().Ensure<BRepGraph_CacheDerivedState>();
}

enum class CoEdgeLookupContent
{
  Any,
  WithPCurve
};

bool matchesLookupContent(const BRepGraph&               theGraph,
                          const BRepGraphInc::CoEdgeDef& theCoEdge,
                          const CoEdgeLookupContent      theContent)
{
  return theContent == CoEdgeLookupContent::Any
         || (theCoEdge.Curve2DRepId.IsValid(theGraph.Topo().Geometry().NbCoEdgeCurves2D())
             && !theCoEdge.Curve2DRepId.IsRemoved(theGraph));
}

occ::handle<Geom_Plane> planeBasis(const occ::handle<Geom_Surface>& theSurface)
{
  occ::handle<Geom_Plane> aPlane = occ::down_cast<Geom_Plane>(theSurface);
  if (!aPlane.IsNull())
  {
    return aPlane;
  }

  const occ::handle<Geom_RectangularTrimmedSurface> aTrimmed =
    occ::down_cast<Geom_RectangularTrimmedSurface>(theSurface);
  return !aTrimmed.IsNull() ? occ::down_cast<Geom_Plane>(aTrimmed->BasisSurface())
                            : occ::handle<Geom_Plane>();
}

StoredPCurveResult storedCoEdgePCurveAdaptor(const BRepGraph&               theGraph,
                                             const BRepGraphInc_Storage&    theStorage,
                                             const BRepGraphInc::CoEdgeDef& theCoEdge)
{
  if (!theCoEdge.Curve2DRepId.IsValid(theStorage.NbCoEdgeCurves2D())
      || theStorage.IsRemoved(theCoEdge.Curve2DRepId))
  {
    return {};
  }

  const BRepGraphInc::CoEdgeCurve2DRep& aPCurveUse =
    theStorage.CoEdgeCurve2DRep(theCoEdge.Curve2DRepId);
  const bool isDegenerated = theCoEdge.ChildEdgeId.IsValid(theGraph.Topo().Edges().Nb())
                             && !theCoEdge.ChildEdgeId.IsRemoved(theGraph)
                             && BRepGraph_Tool::Edge::Degenerated(theGraph, theCoEdge.ChildEdgeId);
  if (!isValidCurveRange(aPCurveUse.Curve,
                         aPCurveUse.ParamFirst,
                         aPCurveUse.ParamLast,
                         isDegenerated))
  {
    return {Geom2dAdaptor_Curve(), StoredPCurveStatus::Malformed};
  }
  try
  {
    return {Geom2dAdaptor_Curve(aPCurveUse.Curve, aPCurveUse.ParamFirst, aPCurveUse.ParamLast),
            StoredPCurveStatus::Usable};
  }
  catch (const Standard_Failure&)
  {
    return {Geom2dAdaptor_Curve(), StoredPCurveStatus::Malformed};
  }
}

Geom2dAdaptor_Curve projectedPlanarPCurveAdaptor(const BRepGraph&               theGraph,
                                                 const BRepGraphInc::CoEdgeDef& theCoEdge,
                                                 const BRepGraph_FaceId         theFace)
{
  if (!theFace.IsValid(theGraph.Topo().Faces().Nb()) || theFace.IsRemoved(theGraph)
      || !theCoEdge.ChildEdgeId.IsValid(theGraph.Topo().Edges().Nb())
      || theCoEdge.ChildEdgeId.IsRemoved(theGraph))
  {
    return Geom2dAdaptor_Curve();
  }

  const occ::handle<Geom_Surface>& aSurface = BRepGraph_Tool::Face::Surface(theGraph, theFace);
  if (aSurface.IsNull() || !BRepGraph_Tool::Edge::HasCurve(theGraph, theCoEdge.ChildEdgeId))
  {
    return Geom2dAdaptor_Curve();
  }

  const occ::handle<Geom_Plane> aPlane = planeBasis(aSurface);
  if (aPlane.IsNull())
  {
    return Geom2dAdaptor_Curve();
  }

  const occ::handle<Geom_Curve>& aCurve3d =
    BRepGraph_Tool::Edge::Curve(theGraph, theCoEdge.ChildEdgeId);
  if (aCurve3d.IsNull())
  {
    return Geom2dAdaptor_Curve();
  }

  const std::pair<double, double> aRange =
    BRepGraph_Tool::Edge::Range(theGraph, theCoEdge.ChildEdgeId);
  if (!isValidCurveRange(aCurve3d, aRange.first, aRange.second))
  {
    return Geom2dAdaptor_Curve();
  }
  occ::handle<Geom2d_Curve> aProjected;
  try
  {
    aProjected = GeomProjLib::Curve2d(aCurve3d, aRange.first, aRange.second, aPlane);
  }
  catch (const Standard_Failure&)
  {
    return Geom2dAdaptor_Curve();
  }
  return !aProjected.IsNull() ? Geom2dAdaptor_Curve(aProjected, aRange.first, aRange.second)
                              : Geom2dAdaptor_Curve();
}

bool isCoEdgeInFaceContext(const BRepGraph&               theGraph,
                           const BRepGraphInc::CoEdgeDef& theCoEdge,
                           const BRepGraph_FaceId         theFace)
{
  if (!theFace.IsValid(theGraph.Topo().Faces().Nb()) || theFace.IsRemoved(theGraph))
  {
    return false;
  }

  if (theCoEdge.FaceId.IsValid())
  {
    if (!theCoEdge.FaceId.IsValid(theGraph.Topo().Faces().Nb()))
    {
      return false;
    }
    if (!theCoEdge.FaceId.IsRemoved(theGraph))
    {
      return theCoEdge.FaceId == theFace;
    }
  }

  if (!theCoEdge.ParentWireId.IsValid(theGraph.Topo().Wires().Nb())
      || theCoEdge.ParentWireId.IsRemoved(theGraph))
  {
    return false;
  }

  const NCollection_LinearVector<BRepGraph_WireRefId>& aWireRefs =
    theGraph.Topo().Wires().Relations(theCoEdge.ParentWireId).ParentWireRefIds;
  for (const BRepGraph_WireRefId& aWireRefId : aWireRefs)
  {
    if (!aWireRefId.IsValid(theGraph.Refs().Wires().Nb()) || aWireRefId.IsRemoved(theGraph))
    {
      continue;
    }

    if (theGraph.Refs().Wires().Entry(aWireRefId).ParentFaceId == theFace)
    {
      return true;
    }
  }
  return false;
}

BRepGraph_CoEdgeId findCoEdgeId(const BRepGraph&          theGraph,
                                const BRepGraph_EdgeId    theEdge,
                                const BRepGraph_FaceId    theFace,
                                const CoEdgeLookupContent theContent)
{
  if (!theEdge.IsValid(theGraph.Topo().Edges().Nb())
      || !theFace.IsValid(theGraph.Topo().Faces().Nb()))
  {
    return BRepGraph_CoEdgeId();
  }

  for (BRepGraph_CoEdgesOfEdge aCoEdgeIt(theGraph, theGraph.Topo().Edges().CoEdges(theEdge));
       aCoEdgeIt.More();
       aCoEdgeIt.Next())
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge = aCoEdgeIt.Definition();
    if (aCoEdge.ChildEdgeId == theEdge && isCoEdgeInFaceContext(theGraph, aCoEdge, theFace)
        && matchesLookupContent(theGraph, aCoEdge, theContent))
    {
      return aCoEdgeIt.CurrentId();
    }
  }
  return BRepGraph_CoEdgeId();
}

BRepGraph_CoEdgeId findCoEdgeId(const BRepGraph&          theGraph,
                                const BRepGraph_EdgeId    theEdge,
                                const BRepGraph_FaceId    theFace,
                                const TopAbs_Orientation  theOrientation,
                                const CoEdgeLookupContent theContent)
{
  if (!theEdge.IsValid(theGraph.Topo().Edges().Nb())
      || !theFace.IsValid(theGraph.Topo().Faces().Nb()))
  {
    return BRepGraph_CoEdgeId();
  }

  BRepGraph_CoEdgeId aFirstMatch;
  for (BRepGraph_CoEdgesOfEdge aCoEdgeIt(theGraph, theGraph.Topo().Edges().CoEdges(theEdge));
       aCoEdgeIt.More();
       aCoEdgeIt.Next())
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge = aCoEdgeIt.Definition();
    if (aCoEdge.ChildEdgeId != theEdge || !isCoEdgeInFaceContext(theGraph, aCoEdge, theFace)
        || !matchesLookupContent(theGraph, aCoEdge, theContent))
    {
      continue;
    }
    if (!aFirstMatch.IsValid())
    {
      aFirstMatch = aCoEdgeIt.CurrentId();
    }
    if (aCoEdge.Orientation == theOrientation)
    {
      return aCoEdgeIt.CurrentId();
    }
  }
  return aFirstMatch;
}

BRepGraph_CoEdgeId findExactCoEdgeId(const BRepGraph&         theGraph,
                                     const BRepGraph_EdgeId   theEdge,
                                     const BRepGraph_FaceId   theFace,
                                     const TopAbs_Orientation theFaceOrientation,
                                     const TopAbs_Orientation theOrientation)
{
  if (!theEdge.IsValid(theGraph.Topo().Edges().Nb()) || theEdge.IsRemoved(theGraph)
      || !theFace.IsValid(theGraph.Topo().Faces().Nb()) || theFace.IsRemoved(theGraph))
  {
    return BRepGraph_CoEdgeId();
  }

  for (BRepGraph_CoEdgesOfEdge aCoEdgeIt(theGraph, theGraph.Topo().Edges().CoEdges(theEdge));
       aCoEdgeIt.More();
       aCoEdgeIt.Next())
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge = aCoEdgeIt.Definition();
    if (aCoEdge.ChildEdgeId != theEdge || !isCoEdgeInFaceContext(theGraph, aCoEdge, theFace))
    {
      continue;
    }
    const NCollection_LinearVector<BRepGraph_WireRefId>& aWireRefs =
      theGraph.Topo().Faces().Relations(theFace).WireRefIds;
    for (const BRepGraph_WireRefId& aWireRefId : aWireRefs)
    {
      if (!aWireRefId.IsValid(theGraph.Refs().Wires().Nb()) || aWireRefId.IsRemoved(theGraph))
      {
        continue;
      }
      const BRepGraphInc::WireRef& aWireRef = theGraph.Refs().Wires().Entry(aWireRefId);
      const TopAbs_Orientation anOccurrenceOrientation =
        TopAbs::Compose(theFaceOrientation,
                        TopAbs::Compose(aWireRef.Orientation, aCoEdge.Orientation));
      if (aWireRef.ChildWireId == aCoEdge.ParentWireId
          && anOccurrenceOrientation == theOrientation)
      {
        return aCoEdgeIt.CurrentId();
      }
    }
  }
  return BRepGraph_CoEdgeId();
}
} // namespace

BRepGraph_Tool::VertexUsage BRepGraph_Tool::Vertex::Usage(const BRepGraph&            theGraph,
                                                          const BRepGraph_VertexRefId theVertexRef)
{
  if (!theVertexRef.IsValid() || theGraph.Refs().Gen().IsRemoved(theVertexRef))
  {
    return VertexUsage();
  }
  const BRepGraphInc::VertexRef& aRef = theGraph.Refs().Vertices().Entry(theVertexRef);
  if (!aRef.ChildVertexId.IsValid() || aRef.ChildVertexId.IsRemoved(theGraph))
  {
    return VertexUsage();
  }
  return VertexUsage{aRef.ChildVertexId, TopLoc_Location(), aRef.Orientation};
}

//=================================================================================================

gp_Pnt BRepGraph_Tool::Vertex::Pnt(const BRepGraph&                    theGraph,
                                   const BRepGraphInc::VertexInstance& theRef)
{
  const gp_Pnt& aPnt = theGraph.Topo().Vertices().Definition(theRef.DefId).Point;
  if (theRef.Location.IsIdentity())
  {
    return aPnt;
  }
  return aPnt.Transformed(theRef.Location.Transformation());
}

//=================================================================================================

gp_Pnt BRepGraph_Tool::Vertex::Pnt(const BRepGraph& theGraph, const BRepGraph_VertexId theVertex)
{
  return theGraph.Topo().Vertices().Definition(theVertex).Point;
}

//=================================================================================================

gp_Pnt BRepGraph_Tool::Vertex::Pnt(const BRepGraph&            theGraph,
                                   const BRepGraph_VertexRefId theVertexRef)
{
  const VertexUsage aUsage = Usage(theGraph, theVertexRef);
  return aUsage.IsValid() ? Pnt(theGraph, aUsage) : gp_Pnt();
}

//=================================================================================================

double BRepGraph_Tool::Vertex::Tolerance(const BRepGraph&         theGraph,
                                         const BRepGraph_VertexId theVertex)
{
  return theGraph.Topo().Vertices().Definition(theVertex).Tolerance;
}

//=================================================================================================

double BRepGraph_Tool::Vertex::Tolerance(const BRepGraph&            theGraph,
                                         const BRepGraph_VertexRefId theVertexRef)
{
  if (!theVertexRef.IsValid() || theGraph.Refs().Gen().IsRemoved(theVertexRef))
  {
    return 0.0;
  }
  const BRepGraph_VertexId aVertex = theGraph.Refs().Vertices().Entry(theVertexRef).ChildVertexId;
  return aVertex.IsValid() && !aVertex.IsRemoved(theGraph) ? Tolerance(theGraph, aVertex) : 0.0;
}

//=================================================================================================

double BRepGraph_Tool::Edge::Tolerance(const BRepGraph& theGraph, const BRepGraph_EdgeId theEdge)
{
  return theGraph.Topo().Edges().Definition(theEdge).Tolerance;
}

//=================================================================================================

bool BRepGraph_Tool::Edge::Degenerated(const BRepGraph& theGraph, const BRepGraph_EdgeId theEdge)
{
  return derivedStateCache(theGraph)->IsDegenerated(theEdge);
}

//=================================================================================================

bool BRepGraph_Tool::CoEdge::SameParameter(const BRepGraph&         theGraph,
                                           const BRepGraph_CoEdgeId theCoEdge)
{
  return derivedStateCache(theGraph)->SameParameter(theCoEdge);
}

//=================================================================================================

bool BRepGraph_Tool::CoEdge::SameRange(const BRepGraph&         theGraph,
                                       const BRepGraph_CoEdgeId theCoEdge)
{
  return derivedStateCache(theGraph)->SameRange(theCoEdge);
}

//=================================================================================================

bool BRepGraph_Tool::Edge::IsClosed(const BRepGraph& theGraph, const BRepGraph_EdgeId theEdge)
{
  return derivedStateCache(theGraph)->IsClosed(theEdge);
}

//=================================================================================================

std::pair<double, double> BRepGraph_Tool::Edge::Range(const BRepGraph&       theGraph,
                                                      const BRepGraph_EdgeId theEdge)
{
  const BRepGraphInc::EdgeDef& anEdge   = theGraph.Topo().Edges().Definition(theEdge);
  const BRepGraphInc_Storage&  aStorage = theGraph.incStorage();
  double                       aFirst = 0.0, aLast = 0.0;
  if (anEdge.Curve3DRepId.IsValid() && anEdge.Curve3DRepId.IsValid(aStorage.NbEdgeCurves3D())
      && !aStorage.IsRemoved(anEdge.Curve3DRepId))
  {
    const BRepGraphInc::EdgeCurve3DRep& aUse = aStorage.EdgeCurve3DRep(anEdge.Curve3DRepId);
    aFirst                                   = aUse.ParamFirst;
    aLast                                    = aUse.ParamLast;
  }
  return {aFirst, aLast};
}

//=================================================================================================

bool BRepGraph_Tool::Edge::HasCurve(const BRepGraph& theGraph, const BRepGraph_EdgeId theEdge)
{
  const occ::handle<Geom_Curve> aCurve = theGraph.Topo().Edges().Curve3D(theEdge);
  return !aCurve.IsNull();
}

//=================================================================================================

BRepGraph_VertexRefId BRepGraph_Tool::Edge::StartVertexId(const BRepGraph&       theGraph,
                                                          const BRepGraph_EdgeId theEdge)
{
  return theGraph.Topo().Edges().Definition(theEdge).StartVertexRefId;
}

//=================================================================================================

BRepGraph_VertexRefId BRepGraph_Tool::Edge::EndVertexId(const BRepGraph&       theGraph,
                                                        const BRepGraph_EdgeId theEdge)
{
  return theGraph.Topo().Edges().Definition(theEdge).EndVertexRefId;
}

//=================================================================================================

GeomAdaptor_TransformedCurve BRepGraph_Tool::Edge::CurveAdaptor(
  const BRepGraph&                    theGraph,
  const BRepGraphInc::CoEdgeInstance& theRef)
{
  if (!theRef.DefId.IsValid(theGraph.Topo().CoEdges().Nb())
      || theRef.DefId.IsRemoved(theGraph))
  {
    return GeomAdaptor_TransformedCurve();
  }
  const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdges().Definition(theRef.DefId);
  if (!aCoEdge.ChildEdgeId.IsValid(theGraph.Topo().Edges().Nb())
      || aCoEdge.ChildEdgeId.IsRemoved(theGraph))
  {
    return GeomAdaptor_TransformedCurve();
  }
  const BRepGraphInc::EdgeDef&   anEdge  = theGraph.Topo().Edges().Definition(aCoEdge.ChildEdgeId);
  const gp_Trsf& aTrsf = theRef.Location.Transformation();

  // Prefer 3D curve when available.
  if (anEdge.Curve3DRepId.IsValid()
      && anEdge.Curve3DRepId.IsValid(theGraph.incStorage().NbEdgeCurves3D())
      && !theGraph.incStorage().IsRemoved(anEdge.Curve3DRepId))
  {
    const BRepGraphInc::EdgeCurve3DRep& aCurveRep =
      theGraph.incStorage().EdgeCurve3DRep(anEdge.Curve3DRepId);
    const occ::handle<Geom_Curve>& aCurve = aCurveRep.Curve;
    if (!aCurve.IsNull())
    {
      return GeomAdaptor_TransformedCurve(aCurve, aCurveRep.ParamFirst, aCurveRep.ParamLast, aTrsf);
    }
  }

  // Fallback: CurveOnSurface from PCurve + the coedge's stored face.
  const BRepGraph_FaceId aFaceId = aCoEdge.FaceId;
  if (aFaceId.IsValid(theGraph.Topo().Faces().Nb()) && !aFaceId.IsRemoved(theGraph))
  {
    const BRepGraphInc::FaceDef& aFaceDef =
      theGraph.Topo().Faces().Definition(aFaceId);
    if (aFaceDef.SurfaceRepId.IsValid()
        && aFaceDef.SurfaceRepId.IsValid(theGraph.incStorage().NbFaceSurfaces())
        && !theGraph.incStorage().IsRemoved(aFaceDef.SurfaceRepId))
    {
      const BRepGraphInc::FaceSurfaceRep& aSurfaceRep =
        theGraph.incStorage().FaceSurfaceRep(aFaceDef.SurfaceRepId);
      const occ::handle<Geom_Surface>& aSurf = aSurfaceRep.Surface;
      Geom2dAdaptor_Curve              aPCurve =
        BRepGraph_Tool::CoEdge::PCurveAdaptor(theGraph, theRef.DefId);
      if (!aPCurve.Curve().IsNull() && !aSurf.IsNull())
      {
        GeomAdaptor_TransformedCurve aResult;
        aResult.SetTrsf(aTrsf);
        occ::handle<Geom2dAdaptor_Curve> aHC2d =
          new Geom2dAdaptor_Curve(std::move(aPCurve));
        occ::handle<GeomAdaptor_Surface> aHS = new GeomAdaptor_Surface(aSurf);
        occ::handle<Adaptor3d_CurveOnSurface> aCurveOnSurface =
          new Adaptor3d_CurveOnSurface(aHC2d, aHS);
        aResult.LoadCurveOnSurface(aCurveOnSurface);
        return aResult;
      }
    }
  }

  return GeomAdaptor_TransformedCurve();
}

//=================================================================================================

GeomAdaptor_TransformedCurve BRepGraph_Tool::Edge::CurveAdaptor(const BRepGraph&       theGraph,
                                                                const BRepGraph_EdgeId theEdge)
{
  if (!theEdge.IsValid(theGraph.Topo().Edges().Nb()) || theEdge.IsRemoved(theGraph))
  {
    return GeomAdaptor_TransformedCurve();
  }
  const BRepGraphInc::EdgeDef& anEdge = theGraph.Topo().Edges().Definition(theEdge);
  if (!anEdge.Curve3DRepId.IsValid()
      || !anEdge.Curve3DRepId.IsValid(theGraph.incStorage().NbEdgeCurves3D())
      || theGraph.incStorage().IsRemoved(anEdge.Curve3DRepId))
  {
    return GeomAdaptor_TransformedCurve();
  }
  const BRepGraphInc::EdgeCurve3DRep& aCurveRep =
    theGraph.incStorage().EdgeCurve3DRep(anEdge.Curve3DRepId);
  const occ::handle<Geom_Curve>& aCurve = aCurveRep.Curve;
  if (aCurve.IsNull())
  {
    return GeomAdaptor_TransformedCurve();
  }
  return GeomAdaptor_TransformedCurve(aCurve, aCurveRep.ParamFirst, aCurveRep.ParamLast, gp_Trsf());
}

//=================================================================================================

GeomAdaptor_TransformedCurve BRepGraph_Tool::Edge::CurveAdaptor(
  const BRepGraph& theGraph,
  const EdgeUsage& theRef)
{
  GeomAdaptor_TransformedCurve aResult = CurveAdaptor(theGraph, theRef.DefId);
  if (!aResult.GeomCurve().IsNull())
  {
    aResult.SetTrsf(theRef.Location.Transformation());
  }
  return aResult;
}

//=================================================================================================

GeomAdaptor_TransformedCurve BRepGraph_Tool::Edge::CurveOnSurface(
  const BRepGraph& theGraph,
  const EdgeUsage& theEdge,
  const FaceUsage& theFace)
{
  if (!theEdge.DefId.IsValid(theGraph.Topo().Edges().Nb())
      || theEdge.DefId.IsRemoved(theGraph)
      || !theFace.DefId.IsValid(theGraph.Topo().Faces().Nb())
      || theFace.DefId.IsRemoved(theGraph)
      || !theEdge.Location.IsEqual(theFace.Location))
  {
    return GeomAdaptor_TransformedCurve();
  }

  const BRepGraph_CoEdgeId aCoEdge =
    findExactCoEdgeId(theGraph,
                      theEdge.DefId,
                      theFace.DefId,
                      theFace.Orientation,
                      theEdge.Orientation);
  if (!aCoEdge.IsValid(theGraph.Topo().CoEdges().Nb()) || aCoEdge.IsRemoved(theGraph))
  {
    return GeomAdaptor_TransformedCurve();
  }

  const BRepGraphInc::CoEdgeInstance aCoEdgeUsage{
    aCoEdge, theFace.Location, theEdge.Orientation};
  const occ::handle<Adaptor3d_CurveOnSurface> aCurveOnSurface =
    CurveOnSurface(theGraph, aCoEdgeUsage, theFace.DefId);
  if (aCurveOnSurface.IsNull())
  {
    return GeomAdaptor_TransformedCurve();
  }

  GeomAdaptor_TransformedCurve aResult;
  aResult.SetTrsf(theFace.Location.Transformation());
  aResult.LoadCurveOnSurface(aCurveOnSurface);
  return aResult;
}

//=================================================================================================

static const occ::handle<Geom_Curve> THE_NULL_CURVE;

const occ::handle<Geom_Curve>& BRepGraph_Tool::Edge::Curve(const BRepGraph&       theGraph,
                                                           const BRepGraph_EdgeId theEdge)
{
  if (!theEdge.IsValid(theGraph.Topo().Edges().Nb()) || theEdge.IsRemoved(theGraph))
  {
    return THE_NULL_CURVE;
  }
  const BRepGraph_EdgeCurve3DRepId aRepId =
    theGraph.Topo().Edges().Definition(theEdge).Curve3DRepId;
  if (!aRepId.IsValid(theGraph.incStorage().NbEdgeCurves3D())
      || theGraph.incStorage().IsRemoved(aRepId))
  {
    return THE_NULL_CURVE;
  }
  return theGraph.incStorage().EdgeCurve3DRep(aRepId).Curve;
}

//=================================================================================================

occ::handle<Geom_Curve> BRepGraph_Tool::Edge::Curve(const BRepGraph&                    theGraph,
                                                     const BRepGraphInc::CoEdgeInstance& theRef)
{
  if (!theRef.DefId.IsValid(theGraph.Topo().CoEdges().Nb())
      || theRef.DefId.IsRemoved(theGraph))
  {
    return occ::handle<Geom_Curve>();
  }
  const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdges().Definition(theRef.DefId);
  if (!aCoEdge.ChildEdgeId.IsValid(theGraph.Topo().Edges().Nb())
      || aCoEdge.ChildEdgeId.IsRemoved(theGraph))
  {
    return occ::handle<Geom_Curve>();
  }
  const BRepGraphInc::EdgeDef&   anEdge  = theGraph.Topo().Edges().Definition(aCoEdge.ChildEdgeId);
  if (!anEdge.Curve3DRepId.IsValid()
      || !anEdge.Curve3DRepId.IsValid(theGraph.incStorage().NbEdgeCurves3D())
      || theGraph.incStorage().IsRemoved(anEdge.Curve3DRepId))
  {
    return occ::handle<Geom_Curve>();
  }
  const BRepGraphInc::EdgeCurve3DRep& aCurveRep =
    theGraph.incStorage().EdgeCurve3DRep(anEdge.Curve3DRepId);
  const occ::handle<Geom_Curve>& aCurve = aCurveRep.Curve;
  if (aCurve.IsNull() || theRef.Location.IsIdentity())
  {
    return aCurve;
  }
  return occ::down_cast<Geom_Curve>(aCurve->Transformed(theRef.Location.Transformation()));
}

//=================================================================================================

uint32_t BRepGraph_Tool::Vertex::NbEdges(const BRepGraph&         theGraph,
                                         const BRepGraph_VertexId theVertex)
{
  return static_cast<uint32_t>(theGraph.Topo().Vertices().Edges(theVertex).Size());
}

//=================================================================================================

BRepGraph_EdgeId BRepGraph_Tool::Edge::FindByVertices(const BRepGraph&         theGraph,
                                                      const BRepGraph_VertexId theStartVertex,
                                                      const BRepGraph_VertexId theEndVertex,
                                                      const bool theToIgnoreOrientation)
{
  if (!theStartVertex.IsValid(theGraph.Topo().Vertices().Nb())
      || !theEndVertex.IsValid(theGraph.Topo().Vertices().Nb())
      || theStartVertex.IsRemoved(theGraph) || theEndVertex.IsRemoved(theGraph))
  {
    return BRepGraph_EdgeId();
  }

  for (const BRepGraph_EdgeId& anEdgeId : theGraph.Topo().Vertices().Edges(theStartVertex))
  {
    if (!anEdgeId.IsValid(theGraph.Topo().Edges().Nb()) || anEdgeId.IsRemoved(theGraph))
    {
      continue;
    }

    const BRepGraphInc::EdgeDef& anEdge = theGraph.Topo().Edges().Definition(anEdgeId);
    if (!anEdge.StartVertexRefId.IsValid(theGraph.Refs().Vertices().Nb())
        || !anEdge.EndVertexRefId.IsValid(theGraph.Refs().Vertices().Nb())
        || anEdge.StartVertexRefId.IsRemoved(theGraph) || anEdge.EndVertexRefId.IsRemoved(theGraph))
    {
      continue;
    }

    const BRepGraph_VertexId aStart =
      theGraph.Refs().Vertices().Entry(anEdge.StartVertexRefId).ChildVertexId;
    const BRepGraph_VertexId anEnd =
      theGraph.Refs().Vertices().Entry(anEdge.EndVertexRefId).ChildVertexId;
    if (aStart == theStartVertex && anEnd == theEndVertex)
    {
      return anEdgeId;
    }
    if (theToIgnoreOrientation && aStart == theEndVertex && anEnd == theStartVertex)
    {
      return anEdgeId;
    }
  }

  return BRepGraph_EdgeId();
}

//=================================================================================================

BRepGraph_CoEdgeId BRepGraph_Tool::Edge::FindPCurveCoEdgeId(const BRepGraph&       theGraph,
                                                            const BRepGraph_EdgeId theEdge,
                                                            const BRepGraph_FaceId theFace)
{
  return findCoEdgeId(theGraph, theEdge, theFace, CoEdgeLookupContent::WithPCurve);
}

//=================================================================================================

BRepGraph_CoEdgeId BRepGraph_Tool::Edge::FindPCurveCoEdgeId(const BRepGraph&         theGraph,
                                                            const BRepGraph_EdgeId   theEdge,
                                                            const BRepGraph_FaceId   theFace,
                                                            const TopAbs_Orientation theOrientation)
{
  return findCoEdgeId(theGraph, theEdge, theFace, theOrientation, CoEdgeLookupContent::WithPCurve);
}

//=================================================================================================

BRepGraph_CoEdgeId BRepGraph_Tool::Edge::FindCoEdgeId(const BRepGraph&       theGraph,
                                                      const BRepGraph_EdgeId theEdge,
                                                      const BRepGraph_FaceId theFace)
{
  return findCoEdgeId(theGraph, theEdge, theFace, CoEdgeLookupContent::Any);
}

//=================================================================================================

BRepGraph_CoEdgeId BRepGraph_Tool::Edge::FindCoEdgeId(const BRepGraph&         theGraph,
                                                      const BRepGraph_EdgeId   theEdge,
                                                      const BRepGraph_FaceId   theFace,
                                                      const TopAbs_Orientation theOrientation)
{
  return findCoEdgeId(theGraph, theEdge, theFace, theOrientation, CoEdgeLookupContent::Any);
}

//=================================================================================================

uint32_t BRepGraph_Tool::Edge::NbFaces(const BRepGraph& theGraph, const BRepGraph_EdgeId theEdge)
{
  return theGraph.Topo().Edges().NbFaces(theEdge);
}

//=================================================================================================

bool BRepGraph_Tool::Edge::IsManifold(const BRepGraph& theGraph, const BRepGraph_EdgeId theEdge)
{
  return NbFaces(theGraph, theEdge) == 2;
}

//=================================================================================================

bool BRepGraph_Tool::Edge::IsBoundary(const BRepGraph& theGraph, const BRepGraph_EdgeId theEdge)
{
  return NbFaces(theGraph, theEdge) == 1;
}

//=================================================================================================

TopAbs_Orientation BRepGraph_Tool::CoEdge::Orientation(const BRepGraph&         theGraph,
                                                       const BRepGraph_CoEdgeId theCoEdge)
{
  return theGraph.Topo().CoEdges().Definition(theCoEdge).Orientation;
}

//=================================================================================================

bool BRepGraph_Tool::CoEdge::IsReversed(const BRepGraph&         theGraph,
                                        const BRepGraph_CoEdgeId theCoEdge)
{
  return theGraph.Topo().CoEdges().Definition(theCoEdge).Orientation == TopAbs_REVERSED;
}

//=================================================================================================

BRepGraph_EdgeId BRepGraph_Tool::CoEdge::EdgeOf(const BRepGraph&         theGraph,
                                                const BRepGraph_CoEdgeId theCoEdge)
{
  return theGraph.Topo().CoEdges().Definition(theCoEdge).ChildEdgeId;
}

//=================================================================================================

BRepGraph_FaceId BRepGraph_Tool::CoEdge::FaceOf(const BRepGraph&         theGraph,
                                                const BRepGraph_CoEdgeId theCoEdge)
{
  return theGraph.Topo().CoEdges().Definition(theCoEdge).FaceId;
}

//=================================================================================================

BRepGraph_FaceId BRepGraph_Tool::CoEdge::FaceContextOf(const BRepGraph&         theGraph,
                                                       const BRepGraph_CoEdgeId theCoEdge)
{
  const BRepGraphInc_Storage& aStorage = theGraph.incStorage();
  if (!theCoEdge.IsValid(aStorage.NbCoEdges()) || aStorage.IsRemoved(theCoEdge))
  {
    return BRepGraph_FaceId();
  }

  const BRepGraphInc::CoEdgeDef& aCoEdge = aStorage.CoEdge(theCoEdge);
  if (aCoEdge.FaceId.IsValid())
  {
    if (!aCoEdge.FaceId.IsValid(aStorage.NbFaces()))
    {
      return BRepGraph_FaceId();
    }
    if (!aStorage.IsRemoved(aCoEdge.FaceId))
    {
      return aCoEdge.FaceId;
    }
  }

  if (!aCoEdge.ParentWireId.IsValid(aStorage.NbWires()) || aStorage.IsRemoved(aCoEdge.ParentWireId))
  {
    return BRepGraph_FaceId();
  }

  const BRepGraph_FaceId aWireFace = BRepGraph_Tool::Wire::FaceOf(theGraph, aCoEdge.ParentWireId);
  return aWireFace.IsValid(aStorage.NbFaces()) && !aStorage.IsRemoved(aWireFace)
           ? aWireFace
           : BRepGraph_FaceId();
}

//=================================================================================================

bool BRepGraph_Tool::CoEdge::IsInFaceContext(const BRepGraph&         theGraph,
                                              const BRepGraph_CoEdgeId theCoEdge,
                                              const BRepGraph_FaceId   theFace)
{
  const BRepGraphInc_Storage& aStorage = theGraph.incStorage();
  return theCoEdge.IsValid(aStorage.NbCoEdges()) && !aStorage.IsRemoved(theCoEdge)
         && isCoEdgeInFaceContext(theGraph, aStorage.CoEdge(theCoEdge), theFace);
}

//=================================================================================================

BRepGraph_CoEdgeId BRepGraph_Tool::CoEdge::SeamPair(const BRepGraph&         theGraph,
                                                    const BRepGraph_CoEdgeId theCoEdge)
{
  // The seam mate is the sibling CoEdge on the same face with opposite orientation.
  const BRepGraphInc::CoEdgeDef& aDef = theGraph.Topo().CoEdges().Definition(theCoEdge);
  const BRepGraph_FaceId         aFace = FaceContextOf(theGraph, theCoEdge);
  if (!aDef.ChildEdgeId.IsValid() || !aFace.IsValid())
  {
    return {};
  }
  for (BRepGraph_CoEdgesOfEdge anIt(theGraph, theGraph.Topo().Edges().CoEdges(aDef.ChildEdgeId));
       anIt.More();
       anIt.Next())
  {
    const BRepGraph_CoEdgeId aOther = anIt.CurrentId();
    if (aOther == theCoEdge)
    {
      continue;
    }
    const BRepGraphInc::CoEdgeDef& aOtherDef = anIt.Definition();
    if (FaceContextOf(theGraph, aOther) == aFace && aOtherDef.Orientation != aDef.Orientation)
    {
      return aOther;
    }
  }
  return {};
}

//=================================================================================================

bool BRepGraph_Tool::CoEdge::IsSeam(const BRepGraph& theGraph, const BRepGraph_CoEdgeId theCoEdge)
{
  return SeamPair(theGraph, theCoEdge).IsValid();
}

//=================================================================================================

Geom2dAdaptor_Curve BRepGraph_Tool::CoEdge::PCurveAdaptor(
  const BRepGraph&                    theGraph,
  const BRepGraphInc::CoEdgeInstance& theRef)
{
  return PCurveAdaptor(theGraph, theRef.DefId);
}

//=================================================================================================

Geom2dAdaptor_Curve BRepGraph_Tool::CoEdge::PCurveAdaptorInFace(
  const BRepGraph&         theGraph,
  const BRepGraph_CoEdgeId theCoEdge,
  const BRepGraph_FaceId   theFace)
{
  if (!theCoEdge.IsValid(theGraph.Topo().CoEdges().Nb()) || theCoEdge.IsRemoved(theGraph))
  {
    return Geom2dAdaptor_Curve();
  }
  const BRepGraphInc::CoEdgeInstance aUsage{
    theCoEdge, TopLoc_Location(), Orientation(theGraph, theCoEdge)};
  const occ::handle<Adaptor3d_CurveOnSurface> aCurveOnSurface =
    Edge::CurveOnSurface(theGraph, aUsage, theFace);
  if (aCurveOnSurface.IsNull())
  {
    return Geom2dAdaptor_Curve();
  }
  const occ::handle<Geom2dAdaptor_Curve> aPCurve =
    occ::down_cast<Geom2dAdaptor_Curve>(aCurveOnSurface->GetCurve());
  return !aPCurve.IsNull() ? *aPCurve : Geom2dAdaptor_Curve();
}

//=================================================================================================

Geom2dAdaptor_Curve BRepGraph_Tool::CoEdge::PCurveAdaptor(const BRepGraph&         theGraph,
                                                          const BRepGraph_CoEdgeId theCoEdge)
{
  const BRepGraphInc_Storage& aStorage = theGraph.incStorage();
  if (!theCoEdge.IsValid(aStorage.NbCoEdges()) || aStorage.IsRemoved(theCoEdge))
  {
    return Geom2dAdaptor_Curve();
  }

  const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdges().Definition(theCoEdge);

  const StoredPCurveResult aStored = storedCoEdgePCurveAdaptor(theGraph, aStorage, aCoEdge);
  if (aStored.Status == StoredPCurveStatus::Usable)
  {
    return aStored.Adaptor;
  }
  if (aStored.Status == StoredPCurveStatus::Malformed)
  {
    return Geom2dAdaptor_Curve();
  }

  const BRepGraph_FaceId aFace = CoEdge::FaceContextOf(theGraph, theCoEdge);
  return projectedPlanarPCurveAdaptor(theGraph, aCoEdge, aFace);
}

//=================================================================================================

static const occ::handle<Geom2d_Curve> THE_NULL_PCURVE;

const occ::handle<Geom2d_Curve>& BRepGraph_Tool::CoEdge::PCurve(const BRepGraph&         theGraph,
                                                                const BRepGraph_CoEdgeId theCoEdge)
{
  const BRepGraphInc::CoEdgeDef& aCoEdge  = theGraph.Topo().CoEdges().Definition(theCoEdge);
  const BRepGraphInc_Storage&    aStorage = theGraph.incStorage();
  if (!aCoEdge.Curve2DRepId.IsValid() || !aCoEdge.Curve2DRepId.IsValid(aStorage.NbCoEdgeCurves2D())
      || aStorage.IsRemoved(aCoEdge.Curve2DRepId))
  {
    return THE_NULL_PCURVE;
  }
  return aStorage.CoEdgeCurve2DRep(aCoEdge.Curve2DRepId).Curve;
}

//=================================================================================================

bool BRepGraph_Tool::CoEdge::HasPCurve(const BRepGraph&         theGraph,
                                       const BRepGraph_CoEdgeId theCoEdge)
{
  const BRepGraphInc::CoEdgeDef& aCoEdge  = theGraph.Topo().CoEdges().Definition(theCoEdge);
  const BRepGraphInc_Storage&    aStorage = theGraph.incStorage();
  return aCoEdge.Curve2DRepId.IsValid() && aCoEdge.Curve2DRepId.IsValid(aStorage.NbCoEdgeCurves2D())
         && !aStorage.IsRemoved(aCoEdge.Curve2DRepId);
}

//=================================================================================================

std::pair<gp_Pnt2d, gp_Pnt2d> BRepGraph_Tool::CoEdge::UVPoints(const BRepGraph&         theGraph,
                                                               const BRepGraph_CoEdgeId theCoEdge)
{
  if (!theCoEdge.IsValid())
  {
    return {gp_Pnt2d(), gp_Pnt2d()};
  }

  const BRepGraphInc::CoEdgeDef& aCoEdge  = theGraph.Topo().CoEdges().Definition(theCoEdge);
  const BRepGraphInc_Storage&    aStorage = theGraph.incStorage();
  if (aCoEdge.Curve2DRepId.IsValid() && aCoEdge.Curve2DRepId.IsValid(aStorage.NbCoEdgeCurves2D())
      && !aStorage.IsRemoved(aCoEdge.Curve2DRepId))
  {
    const BRepGraphInc::CoEdgeCurve2DRep& aPCUse = aStorage.CoEdgeCurve2DRep(aCoEdge.Curve2DRepId);
    const occ::handle<Geom2d_Curve>&      aCurve = aPCUse.Curve;
    if (!aCurve.IsNull())
    {
      return {aCurve->EvalD0(aPCUse.ParamFirst), aCurve->EvalD0(aPCUse.ParamLast)};
    }
  }

  const Geom2dAdaptor_Curve aPCurve = PCurveAdaptor(theGraph, theCoEdge);
  if (!aPCurve.IsInitialized())
  {
    return {gp_Pnt2d(), gp_Pnt2d()};
  }
  return {aPCurve.Value(aPCurve.FirstParameter()), aPCurve.Value(aPCurve.LastParameter())};
}

//=================================================================================================

std::pair<double, double> BRepGraph_Tool::CoEdge::Range(const BRepGraph&         theGraph,
                                                        const BRepGraph_CoEdgeId theCoEdge)
{
  const BRepGraphInc::CoEdgeDef& aCoEdge  = theGraph.Topo().CoEdges().Definition(theCoEdge);
  const BRepGraphInc_Storage&    aStorage = theGraph.incStorage();
  double                         aFirst = 0.0, aLast = 0.0;
  if (aCoEdge.Curve2DRepId.IsValid() && aCoEdge.Curve2DRepId.IsValid(aStorage.NbCoEdgeCurves2D())
      && !aStorage.IsRemoved(aCoEdge.Curve2DRepId))
  {
    const BRepGraphInc::CoEdgeCurve2DRep& aPCUse = aStorage.CoEdgeCurve2DRep(aCoEdge.Curve2DRepId);
    aFirst                                       = aPCUse.ParamFirst;
    aLast                                        = aPCUse.ParamLast;
  }
  return {aFirst, aLast};
}

//=================================================================================================

bool BRepGraph_Tool::Edge::IsSeamOnFace(const BRepGraph&       theGraph,
                                        const BRepGraph_EdgeId theEdge,
                                        const BRepGraph_FaceId theFace)
{
  if (!theEdge.IsValid() || !theFace.IsValid())
  {
    return false;
  }
  bool               hasOrientation = false;
  TopAbs_Orientation anOrientation  = TopAbs_FORWARD;
  for (BRepGraph_CoEdgesOfEdge anIt(theGraph, theGraph.Topo().Edges().CoEdges(theEdge));
       anIt.More();
       anIt.Next())
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge = anIt.Definition();
    if (!isCoEdgeInFaceContext(theGraph, aCoEdge, theFace))
    {
      continue;
    }
    if (!hasOrientation)
    {
      anOrientation  = aCoEdge.Orientation;
      hasOrientation = true;
      continue;
    }
    if (aCoEdge.Orientation != anOrientation)
    {
      return true;
    }
  }
  return false;
}

//=================================================================================================

BRepGraph_Tool::FaceUsage BRepGraph_Tool::Face::Usage(const BRepGraph&          theGraph,
                                                      const BRepGraph_FaceRefId theFaceRef)
{
  if (!theFaceRef.IsValid() || theGraph.Refs().Gen().IsRemoved(theFaceRef))
  {
    return FaceUsage();
  }
  const BRepGraphInc::FaceRef& aRef = theGraph.Refs().Faces().Entry(theFaceRef);
  if (!aRef.ChildFaceId.IsValid() || aRef.ChildFaceId.IsRemoved(theGraph))
  {
    return FaceUsage();
  }
  return FaceUsage{aRef.ChildFaceId, TopLoc_Location(), aRef.Orientation};
}

//=================================================================================================

double BRepGraph_Tool::Face::Tolerance(const BRepGraph& theGraph, const BRepGraph_FaceId theFace)
{
  return theGraph.Topo().Faces().Definition(theFace).Tolerance;
}

//=================================================================================================

double BRepGraph_Tool::Face::Tolerance(const BRepGraph&          theGraph,
                                       const BRepGraph_FaceRefId theFaceRef)
{
  const BRepGraph_FaceId aFace = faceOf(theGraph, theFaceRef);
  return aFace.IsValid() ? Tolerance(theGraph, aFace) : 0.0;
}

//=================================================================================================

bool BRepGraph_Tool::Face::HasSurface(const BRepGraph& theGraph, const BRepGraph_FaceId theFace)
{
  return !theGraph.Topo().Faces().Surface(theFace).IsNull();
}

//=================================================================================================

bool BRepGraph_Tool::Face::HasSurface(const BRepGraph&          theGraph,
                                      const BRepGraph_FaceRefId theFaceRef)
{
  const BRepGraph_FaceId aFace = faceOf(theGraph, theFaceRef);
  return aFace.IsValid() && HasSurface(theGraph, aFace);
}

//=================================================================================================

BRepGraph_WireId BRepGraph_Tool::Face::OuterWire(const BRepGraph&       theGraph,
                                                  const BRepGraph_FaceId theFace)
{
  const BRepGraph_WireRefId aWireRef = OuterWireRef(theGraph, theFace);
  if (!aWireRef.IsValid(theGraph.Refs().Wires().Nb()))
  {
    return BRepGraph_WireId();
  }
  return theGraph.Refs().Wires().Entry(aWireRef).ChildWireId;
}

//=================================================================================================

BRepGraph_WireId BRepGraph_Tool::Face::OuterWire(const BRepGraph&          theGraph,
                                                  const BRepGraph_FaceRefId theFaceRef)
{
  const BRepGraph_FaceId aFace = faceOf(theGraph, theFaceRef);
  return aFace.IsValid() ? OuterWire(theGraph, aFace) : BRepGraph_WireId();
}

//=================================================================================================

BRepGraph_WireRefId BRepGraph_Tool::Face::OuterWireRef(const BRepGraph&       theGraph,
                                                        const BRepGraph_FaceId theFace)
{
  if (!theFace.IsValid(theGraph.Topo().Faces().Nb()) || theFace.IsRemoved(theGraph))
  {
    return BRepGraph_WireRefId();
  }
  return derivedStateCache(theGraph)->OuterWireRef(theFace);
}

//=================================================================================================

BRepGraph_WireRefId BRepGraph_Tool::Face::OuterWireRef(const BRepGraph&          theGraph,
                                                        const BRepGraph_FaceRefId theFaceRef)
{
  const BRepGraph_FaceId aFace = faceOf(theGraph, theFaceRef);
  return aFace.IsValid() ? OuterWireRef(theGraph, aFace) : BRepGraph_WireRefId();
}

//=================================================================================================

static const occ::handle<Geom_Surface> THE_NULL_SURFACE;

const occ::handle<Geom_Surface>& BRepGraph_Tool::Face::Surface(const BRepGraph&       theGraph,
                                                               const BRepGraph_FaceId theFace)
{
  const BRepGraph_FaceSurfaceRepId aRepId =
    theGraph.Topo().Faces().Definition(theFace).SurfaceRepId;
  if (!aRepId.IsValid(theGraph.incStorage().NbFaceSurfaces())
      || theGraph.incStorage().IsRemoved(aRepId))
  {
    return THE_NULL_SURFACE;
  }
  return theGraph.incStorage().FaceSurfaceRep(aRepId).Surface;
}

//=================================================================================================

const occ::handle<Geom_Surface>& BRepGraph_Tool::Face::Surface(const BRepGraph&          theGraph,
                                                               const BRepGraph_FaceRefId theFaceRef)
{
  const BRepGraph_FaceId aFace = faceOf(theGraph, theFaceRef);
  return aFace.IsValid() ? Surface(theGraph, aFace) : THE_NULL_SURFACE;
}

//=================================================================================================

GeomAdaptor_TransformedSurface BRepGraph_Tool::Face::SurfaceAdaptor(const BRepGraph&       theGraph,
                                                                    const BRepGraph_FaceId theFace)
{
  const occ::handle<Geom_Surface> aSurf = Surface(theGraph, theFace);
  if (aSurf.IsNull())
  {
    return GeomAdaptor_TransformedSurface();
  }
  return GeomAdaptor_TransformedSurface(aSurf, gp_Trsf());
}

//=================================================================================================

GeomAdaptor_TransformedSurface BRepGraph_Tool::Face::SurfaceAdaptor(const BRepGraph& theGraph,
                                                                    const FaceUsage& theRef)
{
  const occ::handle<Geom_Surface> aSurf = Surface(theGraph, theRef.DefId);
  if (aSurf.IsNull())
  {
    return GeomAdaptor_TransformedSurface();
  }
  return GeomAdaptor_TransformedSurface(aSurf, theRef.Location.Transformation());
}

//=================================================================================================

GeomAdaptor_TransformedSurface BRepGraph_Tool::Face::SurfaceAdaptor(
  const BRepGraph&          theGraph,
  const BRepGraph_FaceRefId theFaceRef)
{
  const FaceUsage aUsage = Usage(theGraph, theFaceRef);
  return aUsage.IsValid() ? SurfaceAdaptor(theGraph, aUsage) : GeomAdaptor_TransformedSurface();
}

//=================================================================================================

GeomAdaptor_TransformedSurface BRepGraph_Tool::Face::SurfaceAdaptor(const BRepGraph&       theGraph,
                                                                    const BRepGraph_FaceId theFace,
                                                                    const double theUFirst,
                                                                    const double theULast,
                                                                    const double theVFirst,
                                                                    const double theVLast)
{
  const occ::handle<Geom_Surface> aSurf = Surface(theGraph, theFace);
  if (aSurf.IsNull())
  {
    return GeomAdaptor_TransformedSurface();
  }
  return GeomAdaptor_TransformedSurface(aSurf, theUFirst, theULast, theVFirst, theVLast, gp_Trsf());
}

//=================================================================================================

GeomAdaptor_TransformedSurface BRepGraph_Tool::Face::SurfaceAdaptor(const BRepGraph& theGraph,
                                                                    const FaceUsage& theRef,
                                                                    const double     theUFirst,
                                                                    const double     theULast,
                                                                    const double     theVFirst,
                                                                    const double     theVLast)
{
  const occ::handle<Geom_Surface> aSurf = Surface(theGraph, theRef.DefId);
  if (aSurf.IsNull())
  {
    return GeomAdaptor_TransformedSurface();
  }
  return GeomAdaptor_TransformedSurface(aSurf,
                                        theUFirst,
                                        theULast,
                                        theVFirst,
                                        theVLast,
                                        theRef.Location.Transformation());
}

//=================================================================================================

GeomAdaptor_TransformedSurface BRepGraph_Tool::Face::SurfaceAdaptor(
  const BRepGraph&          theGraph,
  const BRepGraph_FaceRefId theFaceRef,
  const double              theUFirst,
  const double              theULast,
  const double              theVFirst,
  const double              theVLast)
{
  const FaceUsage aUsage = Usage(theGraph, theFaceRef);
  return aUsage.IsValid()
           ? SurfaceAdaptor(theGraph, aUsage, theUFirst, theULast, theVFirst, theVLast)
           : GeomAdaptor_TransformedSurface();
}

//=================================================================================================

uint32_t BRepGraph_Tool::Face::NbWires(const BRepGraph& theGraph, const BRepGraph_FaceId theFace)
{
  return static_cast<uint32_t>(theGraph.Topo().Faces().Relations(theFace).WireRefIds.Size());
}

//=================================================================================================

uint32_t BRepGraph_Tool::Face::NbWires(const BRepGraph&          theGraph,
                                       const BRepGraph_FaceRefId theFaceRef)
{
  const BRepGraph_FaceId aFace = faceOf(theGraph, theFaceRef);
  return aFace.IsValid() ? NbWires(theGraph, aFace) : 0;
}

//=================================================================================================

void BRepGraph_Tool::Face::Bounds(const BRepGraph&       theGraph,
                                  const BRepGraph_FaceId theFace,
                                  double&                theUMin,
                                  double&                theUMax,
                                  double&                theVMin,
                                  double&                theVMax)
{
  theUMin = theUMax = theVMin = theVMax  = 0.0;
  const occ::handle<Geom_Surface>& aSurf = Surface(theGraph, theFace);
  if (!aSurf.IsNull())
  {
    aSurf->Bounds(theUMin, theUMax, theVMin, theVMax);
  }
}

//=================================================================================================

void BRepGraph_Tool::Face::Bounds(const BRepGraph&          theGraph,
                                  const BRepGraph_FaceRefId theFaceRef,
                                  double&                   theUMin,
                                  double&                   theUMax,
                                  double&                   theVMin,
                                  double&                   theVMax)
{
  const BRepGraph_FaceId aFace = faceOf(theGraph, theFaceRef);
  if (aFace.IsValid())
  {
    Bounds(theGraph, aFace, theUMin, theUMax, theVMin, theVMax);
    return;
  }
  theUMin = theUMax = theVMin = theVMax = 0.0;
}

//=================================================================================================

occ::handle<Adaptor3d_CurveOnSurface> BRepGraph_Tool::Edge::CurveOnSurface(
  const BRepGraph&                    theGraph,
  const BRepGraphInc::CoEdgeInstance& theRef,
  const BRepGraph_FaceId              theFace)
{
  const BRepGraphInc_Storage&    aStorage = theGraph.incStorage();

  if (!theRef.DefId.IsValid(aStorage.NbCoEdges()) || aStorage.IsRemoved(theRef.DefId)
      || !theFace.IsValid(aStorage.NbFaces()) || aStorage.IsRemoved(theFace))
  {
    return occ::handle<Adaptor3d_CurveOnSurface>();
  }

  const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdges().Definition(theRef.DefId);
  if (!aCoEdge.ChildEdgeId.IsValid(theGraph.Topo().Edges().Nb())
      || aCoEdge.ChildEdgeId.IsRemoved(theGraph)
      || !isCoEdgeInFaceContext(theGraph, aCoEdge, theFace))
  {
    return occ::handle<Adaptor3d_CurveOnSurface>();
  }
  const StoredPCurveResult aStored = storedCoEdgePCurveAdaptor(theGraph, aStorage, aCoEdge);
  const bool hasActiveStoredFace = aCoEdge.FaceId.IsValid(aStorage.NbFaces())
                                   && !aStorage.IsRemoved(aCoEdge.FaceId);
  const bool isStoredApplicable = hasActiveStoredFace
                                    ? aCoEdge.FaceId == theFace
                                    : CoEdge::FaceContextOf(theGraph, theRef.DefId) == theFace;
  if (isStoredApplicable && aStored.Status == StoredPCurveStatus::Malformed)
  {
    return occ::handle<Adaptor3d_CurveOnSurface>();
  }
  Geom2dAdaptor_Curve aPCurve = isStoredApplicable && aStored.Status == StoredPCurveStatus::Usable
                                  ? aStored.Adaptor
                                  : Geom2dAdaptor_Curve();
  if (!aPCurve.IsInitialized())
  {
    aPCurve = projectedPlanarPCurveAdaptor(theGraph, aCoEdge, theFace);
  }

  const occ::handle<Geom_Surface>& aSurf = Face::Surface(theGraph, theFace);
  if (!aPCurve.IsInitialized() || aPCurve.Curve().IsNull() || aSurf.IsNull())
  {
    return occ::handle<Adaptor3d_CurveOnSurface>();
  }

  occ::handle<Geom2dAdaptor_Curve> aHC2d =
    new Geom2dAdaptor_Curve(aPCurve.Curve(), aPCurve.FirstParameter(), aPCurve.LastParameter());
  occ::handle<GeomAdaptor_Surface> aHS = new GeomAdaptor_Surface(aSurf);
  return new Adaptor3d_CurveOnSurface(aHC2d, aHS);
}

//=================================================================================================

BRepGraph_Tool::WireUsage BRepGraph_Tool::Wire::Usage(const BRepGraph&          theGraph,
                                                      const BRepGraph_WireRefId theWireRef)
{
  if (!theWireRef.IsValid(theGraph.Refs().Wires().Nb())
      || theGraph.Refs().Gen().IsRemoved(theWireRef))
  {
    return WireUsage();
  }
  const BRepGraphInc::WireRef& aRef = theGraph.Refs().Wires().Entry(theWireRef);
  if (!aRef.ChildWireId.IsValid(theGraph.Topo().Wires().Nb())
      || aRef.ChildWireId.IsRemoved(theGraph))
  {
    return WireUsage();
  }
  return WireUsage{aRef.ChildWireId, TopLoc_Location(), aRef.Orientation};
}

//=================================================================================================

bool BRepGraph_Tool::Wire::IsClosed(const BRepGraph& theGraph, const BRepGraph_WireId theWire)
{
  bool aClosed = false;
  return derivedStateCache(theGraph)->GetWireIsClosed(theWire, aClosed) && aClosed;
}

//=================================================================================================

bool BRepGraph_Tool::Wire::IsClosed(const BRepGraph& theGraph, const BRepGraph_WireRefId theWireRef)
{
  const BRepGraph_WireId aWire = wireOf(theGraph, theWireRef);
  return aWire.IsValid() && IsClosed(theGraph, aWire);
}

//=================================================================================================

uint32_t BRepGraph_Tool::Wire::NbCoEdges(const BRepGraph& theGraph, const BRepGraph_WireId theWire)
{
  return static_cast<uint32_t>(theGraph.Topo().Wires().Relations(theWire).CoEdgeIds.Size());
}

//=================================================================================================

uint32_t BRepGraph_Tool::Wire::NbCoEdges(const BRepGraph&          theGraph,
                                         const BRepGraph_WireRefId theWireRef)
{
  const BRepGraph_WireId aWire = wireOf(theGraph, theWireRef);
  return aWire.IsValid() ? NbCoEdges(theGraph, aWire) : 0;
}

//=================================================================================================

uint32_t BRepGraph_Tool::Wire::NbDistinctEdges(const BRepGraph&       theGraph,
                                               const BRepGraph_WireId theWire)
{
  return derivedStateCache(theGraph)->NbDistinctEdges(theWire);
}

//=================================================================================================

uint32_t BRepGraph_Tool::Wire::NbDistinctEdges(const BRepGraph&          theGraph,
                                               const BRepGraph_WireRefId theWireRef)
{
  const BRepGraph_WireId aWire = wireOf(theGraph, theWireRef);
  return aWire.IsValid() ? NbDistinctEdges(theGraph, aWire) : 0;
}

//=================================================================================================

BRepGraph_FaceId BRepGraph_Tool::Wire::FaceOf(const BRepGraph&       theGraph,
                                               const BRepGraph_WireId theWire)
{
  if (!theWire.IsValid(theGraph.Topo().Wires().Nb()) || theWire.IsRemoved(theGraph))
  {
    return BRepGraph_FaceId();
  }

  const NCollection_LinearVector<BRepGraph_WireRefId>& aRefs =
    theGraph.Topo().Wires().Relations(theWire).ParentWireRefIds;
  for (const BRepGraph_WireRefId& aRefId : aRefs)
  {
    if (!aRefId.IsValid(theGraph.Refs().Wires().Nb()) || aRefId.IsRemoved(theGraph))
    {
      continue;
    }

    const BRepGraph_FaceId aFace = theGraph.Refs().Wires().Entry(aRefId).ParentFaceId;
    if (aFace.IsValid(theGraph.Topo().Faces().Nb()) && !aFace.IsRemoved(theGraph))
    {
      return aFace;
    }
  }
  return BRepGraph_FaceId();
}

//=================================================================================================

BRepGraph_FaceId BRepGraph_Tool::Wire::FaceOf(const BRepGraph&          theGraph,
                                               const BRepGraph_WireRefId theWireRef)
{
  if (!theWireRef.IsValid(theGraph.Refs().Wires().Nb())
      || theGraph.Refs().Gen().IsRemoved(theWireRef))
  {
    return BRepGraph_FaceId();
  }
  const BRepGraphInc::WireRef& aWireRef = theGraph.Refs().Wires().Entry(theWireRef);
  return aWireRef.ChildWireId.IsValid(theGraph.Topo().Wires().Nb())
             && !aWireRef.ChildWireId.IsRemoved(theGraph)
             && aWireRef.ParentFaceId.IsValid(theGraph.Topo().Faces().Nb())
             && !aWireRef.ParentFaceId.IsRemoved(theGraph)
           ? aWireRef.ParentFaceId
           : BRepGraph_FaceId();
}

//=================================================================================================

bool BRepGraph_Tool::Wire::IsOuter(const BRepGraph& theGraph, const BRepGraph_WireId theWire)
{
  if (!theWire.IsValid(theGraph.Topo().Wires().Nb()) || theWire.IsRemoved(theGraph))
  {
    return false;
  }

  const NCollection_LinearVector<BRepGraph_WireRefId>& aRefs =
    theGraph.Topo().Wires().Relations(theWire).ParentWireRefIds;
  for (const BRepGraph_WireRefId& aRefId : aRefs)
  {
    if (!aRefId.IsValid(theGraph.Refs().Wires().Nb()) || aRefId.IsRemoved(theGraph))
    {
      continue;
    }
    const BRepGraph_FaceId aFaceId = theGraph.Refs().Wires().Entry(aRefId).ParentFaceId;
    if (!aFaceId.IsValid(theGraph.Topo().Faces().Nb()) || aFaceId.IsRemoved(theGraph))
    {
      continue;
    }
    const BRepGraph::RefsView& aRefsView = theGraph.Refs();
    for (BRepGraph_RefsWireOfFace aWireIt(theGraph, aFaceId); aWireIt.More(); aWireIt.Next())
    {
      const BRepGraphInc::WireRef& aRef = aRefsView.Wires().Entry(aWireIt.CurrentId());
      if (!theGraph.Refs().Gen().IsRemoved(aWireIt.CurrentId()) && aRef.ChildWireId == theWire)
      {
        return BRepGraph_Tool::Face::OuterWire(theGraph, aFaceId) == theWire;
      }
    }
  }
  return false;
}

//=================================================================================================

bool BRepGraph_Tool::Wire::IsOuter(const BRepGraph& theGraph, const BRepGraph_WireRefId theWireRef)
{
  if (!theWireRef.IsValid(theGraph.Refs().Wires().Nb())
      || theGraph.Refs().Gen().IsRemoved(theWireRef))
  {
    return false;
  }

  const BRepGraphInc::WireRef& aWireRef = theGraph.Refs().Wires().Entry(theWireRef);
  return aWireRef.ChildWireId.IsValid(theGraph.Topo().Wires().Nb())
         && !aWireRef.ChildWireId.IsRemoved(theGraph)
         && aWireRef.ParentFaceId.IsValid(theGraph.Topo().Faces().Nb())
         && !aWireRef.ParentFaceId.IsRemoved(theGraph)
         && Face::OuterWireRef(theGraph, aWireRef.ParentFaceId) == theWireRef;
}

//=================================================================================================

BRepGraph_Tool::ShellUsage BRepGraph_Tool::Shell::Usage(const BRepGraph&           theGraph,
                                                        const BRepGraph_ShellRefId theShellRef)
{
  if (!theShellRef.IsValid() || theGraph.Refs().Gen().IsRemoved(theShellRef))
  {
    return ShellUsage();
  }
  const BRepGraphInc::ShellRef& aRef = theGraph.Refs().Shells().Entry(theShellRef);
  if (!aRef.ChildShellId.IsValid() || aRef.ChildShellId.IsRemoved(theGraph))
  {
    return ShellUsage();
  }
  return ShellUsage{aRef.ChildShellId, TopLoc_Location(), aRef.Orientation};
}

//=================================================================================================

bool BRepGraph_Tool::Shell::IsClosed(const BRepGraph& theGraph, const BRepGraph_ShellId theShell)
{
  return derivedStateCache(theGraph)->IsShellClosed(theShell);
}

//=================================================================================================

bool BRepGraph_Tool::Shell::IsClosed(const BRepGraph&           theGraph,
                                     const BRepGraph_ShellRefId theShellRef)
{
  const BRepGraph_ShellId aShell = shellOf(theGraph, theShellRef);
  return aShell.IsValid() && IsClosed(theGraph, aShell);
}

//=================================================================================================

uint32_t BRepGraph_Tool::Shell::NbFaces(const BRepGraph& theGraph, const BRepGraph_ShellId theShell)
{
  return static_cast<uint32_t>(theGraph.Topo().Shells().Relations(theShell).FaceRefIds.Size());
}

//=================================================================================================

uint32_t BRepGraph_Tool::Shell::NbFaces(const BRepGraph&           theGraph,
                                        const BRepGraph_ShellRefId theShellRef)
{
  const BRepGraph_ShellId aShell = shellOf(theGraph, theShellRef);
  return aShell.IsValid() ? NbFaces(theGraph, aShell) : 0;
}
