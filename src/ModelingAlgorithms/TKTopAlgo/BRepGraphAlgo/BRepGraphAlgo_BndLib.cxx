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

#include <BRepGraphAlgo_BndLib.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>

#include <BRepGraph.hxx>
#include <BRepGraph_CacheView.hxx>
#include <BRepGraph_TransientCache.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <Standard_Assert.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraphAlgo_FClass2d.hxx>
#include <BRepGraphAlgo_UVBounds.hxx>

#include <Bnd_Box.hxx>
#include <Bnd_Box2d.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <BndLib_AddSurface.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <Extrema_ExtSS.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_Plane.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAdaptor_TransformedSurface.hxx>
#include <Poly_Triangulation.hxx>
#include <Precision.hxx>

namespace
{

// Forward declaration: defined below addFaceBox.
static void findExactUVBounds(const BRepGraph&       theGraph,
                              const BRepGraph_FaceId theFaceId,
                              double&                theUmin,
                              double&                theUmax,
                              double&                theVmin,
                              double&                theVmax,
                              double                 theTol,
                              bool&                  theIsNaturalRestriction);

//! Iterate active wire reference entries for a face in append order.
template <typename FuncT>
static void forFaceWireRefEntries(const BRepGraph&       theGraph,
                                  const BRepGraph_FaceId theFaceId,
                                  FuncT&&                theFunc)
{
  const BRepGraph::RefsView&   aRefs    = theGraph.Refs();
  const BRepGraphInc::FaceDef& aFaceEnt = theGraph.Topo().Faces().Definition(theFaceId);
  for (int i = 0; i < aFaceEnt.WireRefIds.Length(); ++i)
  {
    const BRepGraph_WireRefId    aRefId = aFaceEnt.WireRefIds.Value(i);
    const BRepGraphInc::WireRef& aWR    = aRefs.Wire(aRefId);
    if (aWR.IsRemoved || !aWR.WireDefId.IsValid(theGraph.Topo().NbWires()))
    {
      continue;
    }
    theFunc(aWR);
  }
}

//! Iterate active coedge reference entries for a wire in append order.
template <typename FuncT>
static void forWireCoEdgeRefEntries(const BRepGraph&       theGraph,
                                    const BRepGraph_WireId theWireId,
                                    FuncT&&                theFunc)
{
  const BRepGraph::RefsView&   aRefs    = theGraph.Refs();
  const BRepGraphInc::WireDef& aWireEnt = theGraph.Topo().Wires().Definition(theWireId);
  for (int i = 0; i < aWireEnt.CoEdgeRefIds.Length(); ++i)
  {
    const BRepGraph_CoEdgeRefId    aRefId = aWireEnt.CoEdgeRefIds.Value(i);
    const BRepGraphInc::CoEdgeRef& aCR    = aRefs.CoEdge(aRefId);
    if (aCR.IsRemoved || !aCR.CoEdgeDefId.IsValid(theGraph.Topo().NbCoEdges()))
    {
      continue;
    }
    theFunc(aCR);
  }
}

//! Iterate active shell reference entries for a solid in append order.
template <typename FuncT>
static void forSolidShellRefEntries(const BRepGraph&        theGraph,
                                    const BRepGraph_SolidId theSolidId,
                                    FuncT&&                 theFunc)
{
  const BRepGraph::RefsView&    aRefs     = theGraph.Refs();
  const BRepGraphInc::SolidDef& aSolidEnt = theGraph.Topo().Solids().Definition(theSolidId);
  for (int i = 0; i < aSolidEnt.ShellRefIds.Length(); ++i)
  {
    const BRepGraph_ShellRefId    aRefId = aSolidEnt.ShellRefIds.Value(i);
    const BRepGraphInc::ShellRef& aSR    = aRefs.Shell(aRefId);
    if (aSR.IsRemoved || !aSR.ShellDefId.IsValid(theGraph.Topo().NbShells()))
    {
      continue;
    }
    theFunc(aSR);
  }
}

//! Iterate active child reference entries for a compound in append order.
template <typename FuncT>
static void forCompoundChildRefEntries(const BRepGraph&       theGraph,
                                       const BRepGraph_NodeId theCompNode,
                                       FuncT&&                theFunc)
{
  const BRepGraph::RefsView&       aRefs = theGraph.Refs();
  const BRepGraphInc::CompoundDef& aCompEnt =
    theGraph.Topo().Compounds().Definition(BRepGraph_CompoundId(theCompNode.Index));
  for (int i = 0; i < aCompEnt.ChildRefIds.Length(); ++i)
  {
    const BRepGraph_ChildRefId    aRefId = aCompEnt.ChildRefIds.Value(i);
    const BRepGraphInc::ChildRef& aCR    = aRefs.Child(aRefId);
    if (aCR.IsRemoved || !aCR.ChildDefId.IsValid())
    {
      continue;
    }
    theFunc(aCR);
  }
}

//! Iterate active solid reference entries for a compsolid in append order.
template <typename FuncT>
static void forCompSolidSolidRefEntries(const BRepGraph&            theGraph,
                                        const BRepGraph_CompSolidId theCompSolidId,
                                        FuncT&&                     theFunc)
{
  const BRepGraph::RefsView&        aRefs  = theGraph.Refs();
  const BRepGraphInc::CompSolidDef& aCSEnt = theGraph.Topo().CompSolids().Definition(theCompSolidId);
  for (int i = 0; i < aCSEnt.SolidRefIds.Length(); ++i)
  {
    const BRepGraph_SolidRefId    aRefId = aCSEnt.SolidRefIds.Value(i);
    const BRepGraphInc::SolidRef& aSR    = aRefs.Solid(aRefId);
    if (aSR.IsRemoved || !aSR.SolidDefId.IsValid(theGraph.Topo().NbSolids()))
    {
      continue;
    }
    theFunc(aSR);
  }
}

//! Add vertex point + tolerance to the bounding box.
static void addVertexBox(const BRepGraph&         theGraph,
                         const BRepGraph_VertexId theVertId,
                         Bnd_Box&                 theBox)
{
  theBox.Add(BRepGraph_Tool::Vertex::Pnt(theGraph, theVertId));
  theBox.Enlarge(BRepGraph_Tool::Vertex::Tolerance(theGraph, theVertId));
}

//! Add edge curve bbox using the graph's CurveAdaptor.
static void addEdgeBox(const BRepGraph& theGraph, const BRepGraph_EdgeId theEdgeId, Bnd_Box& theBox)
{
  if (BRepGraph_Tool::Edge::Degenerated(theGraph, theEdgeId)
      || !BRepGraph_Tool::Edge::HasCurve(theGraph, theEdgeId))
  {
    return;
  }

  const GeomAdaptor_TransformedCurve aCurveAdaptor =
    BRepGraph_Tool::Edge::CurveAdaptor(theGraph, theEdgeId);
  BndLib_Add3dCurve::Add(aCurveAdaptor,
                         BRepGraph_Tool::Edge::Tolerance(theGraph, theEdgeId),
                         theBox);
}

//! Add face bbox using triangulation or geometry.
static void addFaceBox(const BRepGraph&       theGraph,
                       const BRepGraph_FaceId theFaceId,
                       Bnd_Box&               theBox,
                       const bool             theUseTri)
{
  const double aFaceTol = BRepGraph_Tool::Face::Tolerance(theGraph, theFaceId);

  // Triangulation path (fast, common).
  const bool aHasTri = BRepGraph_Tool::Face::HasTriangulation(theGraph, theFaceId);
  if ((theUseTri || !BRepGraph_Tool::Face::HasSurface(theGraph, theFaceId)) && aHasTri)
  {
    const occ::handle<Poly_Triangulation>& aTri =
      BRepGraph_Tool::Face::Triangulation(theGraph, theFaceId);
    if (!aTri.IsNull())
    {
      const TopLoc_Location aLoc;
      if (aTri->MinMax(theBox, aLoc))
      {
        theBox.Enlarge(aTri->Deflection() + aFaceTol);
        return;
      }
    }
  }

  // Geometry path.
  if (!BRepGraph_Tool::Face::HasSurface(theGraph, theFaceId))
  {
    return;
  }

  // Check surface type (location-invariant).
  const GeomAdaptor_TransformedSurface aGAS =
    BRepGraph_Tool::Face::SurfaceAdaptor(theGraph, theFaceId);
  if (aGAS.GetType() != GeomAbs_Plane)
  {
    // Non-plane: use SurfaceAdaptor with UV bounds from PCurves (avoids face reconstruction).
    double aUMin, aUMax, aVMin, aVMax;
    bool   anIsNatural = false;
    findExactUVBounds(theGraph, theFaceId, aUMin, aUMax, aVMin, aVMax, aFaceTol, anIsNatural);
    const GeomAdaptor_TransformedSurface aSurfAdaptor =
      BRepGraph_Tool::Face::SurfaceAdaptor(theGraph, theFaceId, aUMin, aUMax, aVMin, aVMax);
    BndLib_AddSurface::Add(aSurfAdaptor, aFaceTol, theBox);
  }
  else
  {
    // Plane: use edge 3D curves directly from the graph.
    bool hasEdges = false;
    forFaceWireRefEntries(theGraph, theFaceId, [&](const BRepGraphInc::WireRef& aWR) {
      forWireCoEdgeRefEntries(theGraph, aWR.WireDefId, [&](const BRepGraphInc::CoEdgeRef& aCR) {
        const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdges().Definition(aCR.CoEdgeDefId);
        const BRepGraph_EdgeId         anEdgeId(aCoEdge.EdgeDefId);
        if (BRepGraph_Tool::Edge::Degenerated(theGraph, anEdgeId)
            || !BRepGraph_Tool::Edge::HasCurve(theGraph, anEdgeId))
        {
          return;
        }
        const GeomAdaptor_TransformedCurve aCurveAdaptor =
          BRepGraph_Tool::Edge::CurveAdaptor(theGraph, anEdgeId);
        BndLib_Add3dCurve::Add(aCurveAdaptor,
                               BRepGraph_Tool::Edge::Tolerance(theGraph, anEdgeId),
                               theBox);
        hasEdges = true;
      });
    });

    if (hasEdges)
    {
      theBox.Enlarge(aFaceTol);
    }
    else
    {
      // No edges: fall back to surface with UV bounds from PCurves.
      double aUMin2, aUMax2, aVMin2, aVMax2;
      bool   anIsNatural2 = false;
      findExactUVBounds(theGraph,
                        theFaceId,
                        aUMin2,
                        aUMax2,
                        aVMin2,
                        aVMax2,
                        aFaceTol,
                        anIsNatural2);
      const GeomAdaptor_TransformedSurface aSurfAdaptor2 =
        BRepGraph_Tool::Face::SurfaceAdaptor(theGraph, theFaceId, aUMin2, aUMax2, aVMin2, aVMax2);
      BndLib_AddSurface::Add(aSurfAdaptor2, aFaceTol, theBox);
    }
  }
}

//! Returns true if edge bounding boxes define the face bounding box.
static bool canUseEdges(const Adaptor3d_Surface& theBS)
{
  const GeomAbs_SurfaceType aST = theBS.GetType();
  if (aST == GeomAbs_Plane || aST == GeomAbs_Cylinder || aST == GeomAbs_Cone
      || aST == GeomAbs_SurfaceOfExtrusion)
  {
    return true;
  }
  else if (aST == GeomAbs_SurfaceOfRevolution)
  {
    const occ::handle<Adaptor3d_Curve>& aBC = theBS.BasisCurve();
    return aBC->GetType() == GeomAbs_Line;
  }
  else if (aST == GeomAbs_OffsetSurface)
  {
    const occ::handle<Adaptor3d_Surface>& aS = theBS.BasisSurface();
    return canUseEdges(*aS);
  }
  else if (aST == GeomAbs_BSplineSurface)
  {
    occ::handle<Geom_BSplineSurface> aBSpl = theBS.BSpline();
    return (aBSpl->UDegree() == 1 && aBSpl->NbUKnots() == 2)
           || (aBSpl->VDegree() == 1 && aBSpl->NbVKnots() == 2);
  }
  else if (aST == GeomAbs_BezierSurface)
  {
    occ::handle<Geom_BezierSurface> aBz = theBS.Bezier();
    return (aBz->UDegree() == 1) || (aBz->VDegree() == 1);
  }
  return false;
}

//! Compute precise UV bounds from face PCurves.
//! Delegates to BRepGraphAlgo_UVBounds::Compute().
static void findExactUVBounds(const BRepGraph&       theGraph,
                              const BRepGraph_FaceId theFaceId,
                              double&                theUmin,
                              double&                theUmax,
                              double&                theVmin,
                              double&                theVmax,
                              double /*theTol*/,
                              bool& theIsNaturalRestriction)
{
  BRepGraphAlgo_UVBounds::CachedData aData;
  BRepGraphAlgo_UVBounds::Compute(theGraph, theFaceId, aData);
  if (aData.IsValid)
  {
    theUmin                 = aData.UMin;
    theUmax                 = aData.UMax;
    theVmin                 = aData.VMin;
    theVmax                 = aData.VMax;
    theIsNaturalRestriction = aData.IsNaturalRestriction;
  }
}

//! Reorder two values so a <= b.
inline void reorder(double& a, double& b)
{
  if (a > b)
  {
    std::swap(a, b);
  }
}

//! Check if the face box can be tightened along a specific axis.
static bool isModifySize(const Adaptor3d_Surface&      theBS,
                         const gp_Pln&                 thePln,
                         const gp_Pnt&                 theP,
                         double                        theUmin,
                         double                        theUmax,
                         double                        theVmin,
                         double                        theVmax,
                         const BRepGraphAlgo_FClass2d& theFClass,
                         double                        theTolU,
                         double                        theTolV)
{
  double pu1 = 0, pu2, pv1 = 0, pv2;
  ElSLib::PlaneParameters(thePln.Position(), theP, pu2, pv2);
  reorder(pu1, pu2);
  reorder(pv1, pv2);
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(thePln);
  GeomAdaptor_Surface     aGAPln(aPlane, pu1, pu2, pv1, pv2);
  Extrema_ExtSS
    anExtr(aGAPln, theBS, pu1, pu2, pv1, pv2, theUmin, theUmax, theVmin, theVmax, theTolU, theTolV);
  if (anExtr.IsDone())
  {
    if (anExtr.NbExt() > 0)
    {
      int             imin  = 0;
      double          dmin  = RealLast();
      double          uextr = 0., vextr = 0.;
      Extrema_POnSurf P1, P2;
      for (int i = 1; i <= anExtr.NbExt(); ++i)
      {
        const double d = anExtr.SquareDistance(i);
        if (d < dmin)
        {
          imin = i;
          dmin = d;
        }
      }
      if (imin > 0)
      {
        anExtr.Points(imin, P1, P2);
        P2.Parameter(uextr, vextr);
      }
      else
      {
        return false;
      }
      const gp_Pnt2d     aP2d(uextr, vextr);
      const TopAbs_State aSt = theFClass.Perform(aP2d);
      if (aSt != TopAbs_IN)
      {
        return true;
      }
    }
    else
    {
      return true;
    }
  }
  return false;
}

//! Tighten face box using edge box comparison and extrema checks.
static void adjustFaceBox(const BRepGraph&         theGraph,
                          const BRepGraph_FaceId   theFaceId,
                          const Adaptor3d_Surface& theBS,
                          double                   theUmin,
                          double                   theUmax,
                          double                   theVmin,
                          double                   theVmax,
                          Bnd_Box&                 theFaceBox,
                          const Bnd_Box&           theEdgeBox,
                          double                   theTol)
{
  if (theEdgeBox.IsVoid())
  {
    return;
  }
  if (theFaceBox.IsVoid())
  {
    theFaceBox = theEdgeBox;
    return;
  }

  double fxmin, fymin, fzmin, fxmax, fymax, fzmax;
  double exmin, eymin, ezmin, exmax, eymax, ezmax;
  theFaceBox.Get(fxmin, fymin, fzmin, fxmax, fymax, fzmax);
  theEdgeBox.Get(exmin, eymin, ezmin, exmax, eymax, ezmax);

  const double                 aTolU = std::max(theBS.UResolution(theTol), Precision::PConfusion());
  const double                 aTolV = std::max(theBS.VResolution(theTol), Precision::PConfusion());
  const BRepGraphAlgo_FClass2d aFClass(theGraph, theFaceId, std::max(aTolU, aTolV));

  bool isModified = false;
  if (exmin > fxmin)
  {
    gp_Pln pl(gp_Ax3(gp_Pnt(fxmin, fymin, fzmin), gp::DX()));
    gp_Pnt aP(fxmin, fymax, fzmax);
    if (isModifySize(theBS, pl, aP, theUmin, theUmax, theVmin, theVmax, aFClass, aTolU, aTolV))
    {
      fxmin      = exmin;
      isModified = true;
    }
  }
  if (exmax < fxmax)
  {
    gp_Pln pl(gp_Ax3(gp_Pnt(fxmax, fymax, fzmax), gp::DX()));
    gp_Pnt aP(fxmax, fymin, fzmin);
    if (isModifySize(theBS, pl, aP, theUmin, theUmax, theVmin, theVmax, aFClass, aTolU, aTolV))
    {
      fxmax      = exmax;
      isModified = true;
    }
  }
  if (eymin > fymin)
  {
    gp_Pln pl(gp_Ax3(gp_Pnt(fxmin, fymin, fzmin), gp::DY()));
    gp_Pnt aP(fxmax, fymin, fzmax);
    if (isModifySize(theBS, pl, aP, theUmin, theUmax, theVmin, theVmax, aFClass, aTolU, aTolV))
    {
      fymin      = eymin;
      isModified = true;
    }
  }
  if (eymax < fymax)
  {
    gp_Pln pl(gp_Ax3(gp_Pnt(fxmax, fymax, fzmax), gp::DY()));
    gp_Pnt aP(fxmin, fymax, fzmin);
    if (isModifySize(theBS, pl, aP, theUmin, theUmax, theVmin, theVmax, aFClass, aTolU, aTolV))
    {
      fymax      = eymax;
      isModified = true;
    }
  }
  if (ezmin > fzmin)
  {
    gp_Pln pl(gp_Ax3(gp_Pnt(fxmin, fymin, fzmin), gp::DZ()));
    gp_Pnt aP(fxmax, fymax, fzmin);
    if (isModifySize(theBS, pl, aP, theUmin, theUmax, theVmin, theVmax, aFClass, aTolU, aTolV))
    {
      fzmin      = ezmin;
      isModified = true;
    }
  }
  if (ezmax < fzmax)
  {
    gp_Pln pl(gp_Ax3(gp_Pnt(fxmax, fymax, fzmax), gp::DZ()));
    gp_Pnt aP(fxmin, fymin, fzmax);
    if (isModifySize(theBS, pl, aP, theUmin, theUmax, theVmin, theVmax, aFClass, aTolU, aTolV))
    {
      fzmax      = ezmax;
      isModified = true;
    }
  }

  if (isModified)
  {
    theFaceBox.SetVoid();
    theFaceBox.Update(fxmin, fymin, fzmin, fxmax, fymax, fzmax);
  }
}

//! Add precise edge curve bbox.
static void addEdgeBoxOptimal(const BRepGraph&       theGraph,
                              const BRepGraph_EdgeId theEdgeId,
                              Bnd_Box&               theBox,
                              const bool             theUseShapeTol)
{
  if (BRepGraph_Tool::Edge::Degenerated(theGraph, theEdgeId)
      || !BRepGraph_Tool::Edge::HasCurve(theGraph, theEdgeId))
  {
    return;
  }
  const GeomAdaptor_TransformedCurve aCurveAdaptor =
    BRepGraph_Tool::Edge::CurveAdaptor(theGraph, theEdgeId);
  const double aTol = theUseShapeTol ? BRepGraph_Tool::Edge::Tolerance(theGraph, theEdgeId) : 0.;
  BndLib_Add3dCurve::AddOptimal(aCurveAdaptor, aTol, theBox);
}

//! Add precise face bbox (optimal path).
static void addFaceBoxOptimal(const BRepGraph&       theGraph,
                              const BRepGraph_FaceId theFaceId,
                              Bnd_Box&               theBox,
                              const bool             theUseTri,
                              const bool             theUseShapeTol)
{
  // Triangulation path.
  if (theUseTri && BRepGraph_Tool::Face::HasTriangulation(theGraph, theFaceId))
  {
    const occ::handle<Poly_Triangulation>& aTri =
      BRepGraph_Tool::Face::Triangulation(theGraph, theFaceId);
    if (!aTri.IsNull())
    {
      Bnd_Box               aLocBox;
      const TopLoc_Location aLoc;
      if (aTri->MinMax(aLocBox, aLoc))
      {
        aLocBox.Enlarge(aTri->Deflection() + BRepGraph_Tool::Face::Tolerance(theGraph, theFaceId));
        double xmin, ymin, zmin, xmax, ymax, zmax;
        aLocBox.Get(xmin, ymin, zmin, xmax, ymax, zmax);
        theBox.Update(xmin, ymin, zmin, xmax, ymax, zmax);
        return;
      }
    }
  }

  // Geometry path.
  if (!BRepGraph_Tool::Face::HasSurface(theGraph, theFaceId))
  {
    return;
  }

  // Use SurfaceAdaptor directly (avoids face reconstruction).
  const GeomAdaptor_TransformedSurface aBS =
    BRepGraph_Tool::Face::SurfaceAdaptor(theGraph, theFaceId);
  const double aFaceTol =
    theUseShapeTol ? BRepGraph_Tool::Face::Tolerance(theGraph, theFaceId) : 0.;

  Bnd_Box aLocBox;
  if (canUseEdges(aBS))
  {
    // Edge-based path: iterate wire coedges.
    bool hasEdges = false;
    forFaceWireRefEntries(theGraph, theFaceId, [&](const BRepGraphInc::WireRef& aWR) {
      forWireCoEdgeRefEntries(theGraph, aWR.WireDefId, [&](const BRepGraphInc::CoEdgeRef& aCR) {
        const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdges().Definition(aCR.CoEdgeDefId);
        const BRepGraph_EdgeId         anEdgeId(aCoEdge.EdgeDefId);
        if (BRepGraph_Tool::Edge::Degenerated(theGraph, anEdgeId)
            || !BRepGraph_Tool::Edge::HasCurve(theGraph, anEdgeId))
        {
          return;
        }
        Bnd_Box                            anEBox;
        const GeomAdaptor_TransformedCurve aCurveAdaptor =
          BRepGraph_Tool::Edge::CurveAdaptor(theGraph, anEdgeId);
        const double aTol =
          theUseShapeTol ? BRepGraph_Tool::Edge::Tolerance(theGraph, anEdgeId) : 0.;
        BndLib_Add3dCurve::AddOptimal(aCurveAdaptor, aTol, anEBox);
        aLocBox.Add(anEBox);
        hasEdges = true;
      });
    });

    if (!hasEdges)
    {
      BndLib_AddSurface::AddOptimal(aBS, aFaceTol, aLocBox);
    }
  }
  else
  {
    // Surface-based path with UV bounds from PCurves.
    double umin, umax, vmin, vmax;
    bool   isNaturalRestriction = false;
    findExactUVBounds(theGraph, theFaceId, umin, umax, vmin, vmax, aFaceTol, isNaturalRestriction);

    const GeomAdaptor_TransformedSurface aBSTrimmed =
      BRepGraph_Tool::Face::SurfaceAdaptor(theGraph, theFaceId, umin, umax, vmin, vmax);
    BndLib_AddSurface::AddOptimal(aBSTrimmed, umin, umax, vmin, vmax, aFaceTol, aLocBox);

    if (!isNaturalRestriction)
    {
      // Build edge box for adjustment.
      Bnd_Box aEBox;
      forFaceWireRefEntries(theGraph, theFaceId, [&](const BRepGraphInc::WireRef& aWR) {
        forWireCoEdgeRefEntries(theGraph, aWR.WireDefId, [&](const BRepGraphInc::CoEdgeRef& aCR) {
          const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdges().Definition(aCR.CoEdgeDefId);
          const BRepGraph_EdgeId         anEdgeId(aCoEdge.EdgeDefId);
          if (BRepGraph_Tool::Edge::Degenerated(theGraph, anEdgeId)
              || !BRepGraph_Tool::Edge::HasCurve(theGraph, anEdgeId))
          {
            return;
          }
          Bnd_Box                            anEBox;
          const GeomAdaptor_TransformedCurve aCurveAdaptor =
            BRepGraph_Tool::Edge::CurveAdaptor(theGraph, anEdgeId);
          const double aTol =
            theUseShapeTol ? BRepGraph_Tool::Edge::Tolerance(theGraph, anEdgeId) : 0.;
          BndLib_Add3dCurve::AddOptimal(aCurveAdaptor, aTol, anEBox);
          aEBox.Add(anEBox);
        });
      });
      adjustFaceBox(theGraph,
                    theFaceId,
                    aBSTrimmed,
                    umin,
                    umax,
                    vmin,
                    vmax,
                    aLocBox,
                    aEBox,
                    aFaceTol);
    }
  }

  if (!aLocBox.IsVoid())
  {
    double xmin, ymin, zmin, xmax, ymax, zmax;
    aLocBox.Get(xmin, ymin, zmin, xmax, ymax, zmax);
    theBox.Update(xmin, ymin, zmin, xmax, ymax, zmax);
  }
}

//! Recursive per-node bounding box computation.
static void addNodeBox(const BRepGraph&       theGraph,
                       const BRepGraph_NodeId theNode,
                       Bnd_Box&               theBox,
                       const bool             theUseTri)
{
  if (!theNode.IsValid())
  {
    return;
  }

  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Vertex: {
      addVertexBox(theGraph, BRepGraph_VertexId(theNode.Index), theBox);
      break;
    }
    case BRepGraph_NodeId::Kind::Edge: {
      addEdgeBox(theGraph, BRepGraph_EdgeId(theNode.Index), theBox);
      break;
    }
    case BRepGraph_NodeId::Kind::Wire: {
      forWireCoEdgeRefEntries(theGraph,
                              BRepGraph_WireId(theNode.Index),
                              [&](const BRepGraphInc::CoEdgeRef& aCR) {
                                const BRepGraphInc::CoEdgeDef& aCoEdge =
                                  theGraph.Topo().CoEdges().Definition(aCR.CoEdgeDefId);
                                addNodeBox(theGraph, aCoEdge.EdgeDefId, theBox, theUseTri);
                              });
      break;
    }
    case BRepGraph_NodeId::Kind::Face: {
      addFaceBox(theGraph, BRepGraph_FaceId(theNode.Index), theBox, theUseTri);
      break;
    }
    case BRepGraph_NodeId::Kind::Shell: {
      const BRepGraph_ShellId aShellId(theNode.Index);
      const NCollection_Vector<BRepGraph_FaceRefId>& aFaceRefIds = theGraph.Refs().FaceRefIdsOf(aShellId);
      for (int i = 0; i < aFaceRefIds.Length(); ++i)
      {
        const BRepGraphInc::FaceRef& aFaceRef = theGraph.Refs().Face(aFaceRefIds.Value(i));
        if (aFaceRef.IsRemoved)
          continue;
        addFaceBox(theGraph, aFaceRef.FaceDefId, theBox, theUseTri);
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Solid: {
      forSolidShellRefEntries(theGraph,
                              BRepGraph_SolidId(theNode.Index),
                              [&](const BRepGraphInc::ShellRef& aSR) {
                                addNodeBox(theGraph, aSR.ShellDefId, theBox, theUseTri);
                              });
      break;
    }
    case BRepGraph_NodeId::Kind::Compound: {
      forCompoundChildRefEntries(theGraph,
                                 BRepGraph_CompoundId(theNode.Index),
                                 [&](const BRepGraphInc::ChildRef& aCR) {
                                   addNodeBox(theGraph, aCR.ChildDefId, theBox, theUseTri);
                                 });
      break;
    }
    case BRepGraph_NodeId::Kind::CompSolid: {
      forCompSolidSolidRefEntries(theGraph,
                                  BRepGraph_CompSolidId(theNode.Index),
                                  [&](const BRepGraphInc::SolidRef& aSR) {
                                    addNodeBox(theGraph, aSR.SolidDefId, theBox, theUseTri);
                                  });
      break;
    }
    default:
      break;
  }
}

//! Recursive per-node optimal bounding box computation.
static void addNodeBoxOptimal(const BRepGraph&       theGraph,
                              const BRepGraph_NodeId theNode,
                              Bnd_Box&               theBox,
                              const bool             theUseTri,
                              const bool             theUseShapeTol)
{
  if (!theNode.IsValid())
  {
    return;
  }

  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Vertex: {
      Bnd_Box                  aLocBox;
      const BRepGraph_VertexId aVertId(theNode.Index);
      aLocBox.Add(BRepGraph_Tool::Vertex::Pnt(theGraph, aVertId));
      const double aTol =
        theUseShapeTol ? BRepGraph_Tool::Vertex::Tolerance(theGraph, aVertId) : 0.;
      aLocBox.Enlarge(aTol);
      double xmin, ymin, zmin, xmax, ymax, zmax;
      aLocBox.Get(xmin, ymin, zmin, xmax, ymax, zmax);
      theBox.Update(xmin, ymin, zmin, xmax, ymax, zmax);
      break;
    }
    case BRepGraph_NodeId::Kind::Edge: {
      Bnd_Box aLocBox;
      addEdgeBoxOptimal(theGraph, BRepGraph_EdgeId(theNode.Index), aLocBox, theUseShapeTol);
      if (!aLocBox.IsVoid())
      {
        double xmin, ymin, zmin, xmax, ymax, zmax;
        aLocBox.Get(xmin, ymin, zmin, xmax, ymax, zmax);
        theBox.Update(xmin, ymin, zmin, xmax, ymax, zmax);
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Wire: {
      forWireCoEdgeRefEntries(
        theGraph,
        BRepGraph_WireId(theNode.Index),
        [&](const BRepGraphInc::CoEdgeRef& aCR) {
          const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdges().Definition(aCR.CoEdgeDefId);
          addNodeBoxOptimal(theGraph, aCoEdge.EdgeDefId, theBox, theUseTri, theUseShapeTol);
        });
      break;
    }
    case BRepGraph_NodeId::Kind::Face: {
      addFaceBoxOptimal(theGraph,
                        BRepGraph_FaceId(theNode.Index),
                        theBox,
                        theUseTri,
                        theUseShapeTol);
      break;
    }
    case BRepGraph_NodeId::Kind::Shell: {
      const BRepGraph_ShellId aShellId(theNode.Index);
      const NCollection_Vector<BRepGraph_FaceRefId>& aFaceRefIds = theGraph.Refs().FaceRefIdsOf(aShellId);
      for (int i = 0; i < aFaceRefIds.Length(); ++i)
      {
        const BRepGraphInc::FaceRef& aFaceRef = theGraph.Refs().Face(aFaceRefIds.Value(i));
        if (aFaceRef.IsRemoved)
          continue;
        addFaceBoxOptimal(theGraph, aFaceRef.FaceDefId, theBox, theUseTri, theUseShapeTol);
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Solid: {
      forSolidShellRefEntries(
        theGraph,
        BRepGraph_SolidId(theNode.Index),
        [&](const BRepGraphInc::ShellRef& aSR) {
          addNodeBoxOptimal(theGraph, aSR.ShellDefId, theBox, theUseTri, theUseShapeTol);
        });
      break;
    }
    case BRepGraph_NodeId::Kind::Compound: {
      forCompoundChildRefEntries(
        theGraph,
        BRepGraph_CompoundId(theNode.Index),
        [&](const BRepGraphInc::ChildRef& aCR) {
          addNodeBoxOptimal(theGraph, aCR.ChildDefId, theBox, theUseTri, theUseShapeTol);
        });
      break;
    }
    case BRepGraph_NodeId::Kind::CompSolid: {
      forCompSolidSolidRefEntries(
        theGraph,
        BRepGraph_CompSolidId(theNode.Index),
        [&](const BRepGraphInc::SolidRef& aSR) {
          addNodeBoxOptimal(theGraph, aSR.SolidDefId, theBox, theUseTri, theUseShapeTol);
        });
      break;
    }
    default:
      break;
  }
}

} // namespace

//=================================================================================================

void BRepGraphAlgo_BndLib::Add(const BRepGraph& theGraph,
                               Bnd_Box&         theBox,
                               const bool       theUseTriangulation)
{
  // Add all faces.
  const int aNbFaces = theGraph.Topo().NbFaces();
  for (int i = 0; i < aNbFaces; ++i)
  {
    addFaceBox(theGraph, BRepGraph_FaceId(i), theBox, theUseTriangulation);
  }

  // Add free edges (edges not in any face, identified by having no CoEdges with a face).
  const int aNbEdges = theGraph.Topo().NbEdges();
  for (int i = 0; i < aNbEdges; ++i)
  {
    if (theGraph.Topo().Edges().NbFaces(BRepGraph_EdgeId(i)) == 0)
    {
      addEdgeBox(theGraph, BRepGraph_EdgeId(i), theBox);
    }
  }

  // Add free vertices (vertices not in edges).
  // For simplicity, add all vertices - redundant additions are harmless.
  const int aNbVerts = theGraph.Topo().NbVertices();
  for (int i = 0; i < aNbVerts; ++i)
  {
    addVertexBox(theGraph, BRepGraph_VertexId(i), theBox);
  }
}

//=================================================================================================

void BRepGraphAlgo_BndLib::Add(const BRepGraph&       theGraph,
                               const BRepGraph_NodeId theNode,
                               Bnd_Box&               theBox,
                               const bool             theUseTriangulation)
{
  addNodeBox(theGraph, theNode, theBox, theUseTriangulation);
}

//=================================================================================================

void BRepGraphAlgo_BndLib::AddOptimal(const BRepGraph& theGraph,
                                      Bnd_Box&         theBox,
                                      const bool       theUseTriangulation,
                                      const bool       theUseShapeTolerance)
{
  // Add all faces (optimal).
  const int aNbFaces = theGraph.Topo().NbFaces();
  for (int i = 0; i < aNbFaces; ++i)
  {
    addFaceBoxOptimal(theGraph,
                      BRepGraph_FaceId(i),
                      theBox,
                      theUseTriangulation,
                      theUseShapeTolerance);
  }

  // Add free edges (edges not in any face).
  const int aNbEdges = theGraph.Topo().NbEdges();
  for (int i = 0; i < aNbEdges; ++i)
  {
    if (theGraph.Topo().Edges().NbFaces(BRepGraph_EdgeId(i)) == 0)
    {
      Bnd_Box aLocBox;
      addEdgeBoxOptimal(theGraph, BRepGraph_EdgeId(i), aLocBox, theUseShapeTolerance);
      if (!aLocBox.IsVoid())
      {
        double xmin, ymin, zmin, xmax, ymax, zmax;
        aLocBox.Get(xmin, ymin, zmin, xmax, ymax, zmax);
        theBox.Update(xmin, ymin, zmin, xmax, ymax, zmax);
      }
    }
  }

  // Add free vertices.
  const int aNbVerts = theGraph.Topo().NbVertices();
  for (int i = 0; i < aNbVerts; ++i)
  {
    Bnd_Box                  aLocBox;
    const BRepGraph_VertexId aVertId(i);
    aLocBox.Add(BRepGraph_Tool::Vertex::Pnt(theGraph, aVertId));
    const double aTol =
      theUseShapeTolerance ? BRepGraph_Tool::Vertex::Tolerance(theGraph, aVertId) : 0.;
    aLocBox.Enlarge(aTol);
    double xmin, ymin, zmin, xmax, ymax, zmax;
    aLocBox.Get(xmin, ymin, zmin, xmax, ymax, zmax);
    theBox.Update(xmin, ymin, zmin, xmax, ymax, zmax);
  }
}

//=================================================================================================

void BRepGraphAlgo_BndLib::AddOptimal(const BRepGraph&       theGraph,
                                      const BRepGraph_NodeId theNode,
                                      Bnd_Box&               theBox,
                                      const bool             theUseTriangulation,
                                      const bool             theUseShapeTolerance)
{
  addNodeBoxOptimal(theGraph, theNode, theBox, theUseTriangulation, theUseShapeTolerance);
}

//=================================================================================================
// Cached API - bounding box data stored as transient cache value on graph nodes.
//=================================================================================================

namespace
{

//! GUID for bounding box cache kind.
static const Standard_GUID THE_BNDBOX_GUID("b7e3a1f0-4c82-4d5e-9a1b-3f8e2d7c6a05");

//! Internal cache-value class storing precision-aware bbox data.
class BRepGraphAlgo_BndBoxCacheValue : public BRepGraph_CacheValue
{
public:
  DEFINE_STANDARD_RTTI_INLINE(BRepGraphAlgo_BndBoxCacheValue, BRepGraph_CacheValue)
  BRepGraphAlgo_BndBoxCacheValue() = default;

  //! Return cached data if its precision is sufficient for the request.
  //! @param[in]  thePrecision required minimum precision
  //! @param[out] theData      cached data (unchanged if insufficient)
  //! @return true if cached data is sufficient
  bool GetIfSufficient(BRepGraphAlgo_BndLib::Precision   thePrecision,
                       BRepGraphAlgo_BndLib::CachedData& theData) const
  {
    std::shared_lock<std::shared_mutex> aReadLock(myMutex);
    if (IsDirty() || static_cast<int>(myData.BoxPrecision) < static_cast<int>(thePrecision))
    {
      return false;
    }
    theData = myData;
    return true;
  }

  //! Return cached data at any precision.
  //! @param[out] theData cached data
  //! @return true if cached data exists (not dirty)
  bool GetAny(BRepGraphAlgo_BndLib::CachedData& theData) const
  {
    std::shared_lock<std::shared_mutex> aReadLock(myMutex);
    if (IsDirty())
    {
      return false;
    }
    theData = myData;
    return true;
  }

  //! Store new data and mark clean.
  void SetData(const BRepGraphAlgo_BndLib::CachedData& theData)
  {
    std::unique_lock<std::shared_mutex> aWriteLock(myMutex);
    myData = theData;
    MarkClean();
  }

private:
  BRepGraphAlgo_BndLib::CachedData myData;
};

} // namespace

//=================================================================================================

const occ::handle<BRepGraph_CacheKind>& BRepGraphAlgo_BndLib::CacheKind()
{
  static const occ::handle<BRepGraph_CacheKind> THE_KIND =
    new BRepGraph_CacheKind(THE_BNDBOX_GUID, "BndBox");
  return THE_KIND;
}

//! Pre-resolved slot index for the BndBox cache kind.
static int cacheKindSlot()
{
  static const int THE_SLOT = BRepGraph_CacheKindRegistry::Register(BRepGraphAlgo_BndLib::CacheKind());
  return THE_SLOT;
}

//=================================================================================================

bool BRepGraphAlgo_BndLib::GetCached(const BRepGraph&                  theGraph,
                                     const BRepGraph_NodeId            theNode,
                                     BRepGraphAlgo_BndLib::CachedData& theData)
{
  occ::handle<BRepGraph_CacheValue> aValue = theGraph.Cache().Get(theNode, cacheKindSlot());
  if (aValue.IsNull())
  {
    return false;
  }

  occ::handle<BRepGraphAlgo_BndBoxCacheValue> aBndValue =
    occ::down_cast<BRepGraphAlgo_BndBoxCacheValue>(aValue);
  if (aBndValue.IsNull())
  {
    return false;
  }

  return aBndValue->GetAny(theData);
}

//=================================================================================================

Bnd_Box BRepGraphAlgo_BndLib::AddCached(BRepGraph&                            theGraph,
                                        const BRepGraph_NodeId                theNode,
                                        const BRepGraphAlgo_BndLib::Precision thePrecision,
                                        const bool                            theUseTriangulation)
{
  const int aKindSlot = cacheKindSlot();
  occ::handle<BRepGraph_CacheValue> anExistingValue = theGraph.Cache().Get(theNode, aKindSlot);
  if (anExistingValue)
  {
    occ::handle<BRepGraphAlgo_BndBoxCacheValue> aBndValue =
      occ::down_cast<BRepGraphAlgo_BndBoxCacheValue>(anExistingValue);
    if (!aBndValue.IsNull())
    {
      BRepGraphAlgo_BndLib::CachedData aData;
      if (aBndValue->GetIfSufficient(thePrecision, aData))
      {
        return aData.Box;
      }
    }
  }

  // Compute the bbox.
  Bnd_Box aBox;
  bool    aUsedShapeTol = false;
  switch (thePrecision)
  {
    case Precision::Standard:
      Add(theGraph, theNode, aBox, theUseTriangulation);
      break;
    case Precision::Optimal:
      aUsedShapeTol = true;
      AddOptimal(theGraph, theNode, aBox, theUseTriangulation, true);
      break;
    default:
      Standard_ASSERT_VOID(false, "AddCached: unhandled Precision value");
      break;
  }

  // Cache the result.
  BRepGraphAlgo_BndLib::CachedData aCachedData;
  aCachedData.Box                = aBox;
  aCachedData.BoxPrecision       = thePrecision;
  aCachedData.UsedTriangulation  = theUseTriangulation;
  aCachedData.UsedShapeTolerance = aUsedShapeTol;

  if (anExistingValue)
  {
    occ::handle<BRepGraphAlgo_BndBoxCacheValue> aBndValue =
      occ::down_cast<BRepGraphAlgo_BndBoxCacheValue>(anExistingValue);
    if (!aBndValue.IsNull())
    {
      aBndValue->SetData(aCachedData);
      return aBox;
    }
  }

  occ::handle<BRepGraphAlgo_BndBoxCacheValue> aNewValue = new BRepGraphAlgo_BndBoxCacheValue();
  aNewValue->SetData(aCachedData);
  theGraph.Cache().Set(theNode, aKindSlot, aNewValue);

  return aBox;
}

//=================================================================================================

void BRepGraphAlgo_BndLib::SetCached(BRepGraph&             theGraph,
                                     const BRepGraph_NodeId theNode,
                                     const Bnd_Box&         theBox,
                                     const Precision        thePrecision,
                                     const bool             theUsedTriangulation,
                                     const bool             theUsedShapeTolerance)
{
  const int aKindSlot = cacheKindSlot();
  CachedData aData;
  aData.Box                = theBox;
  aData.BoxPrecision       = thePrecision;
  aData.UsedTriangulation  = theUsedTriangulation;
  aData.UsedShapeTolerance = theUsedShapeTolerance;

  occ::handle<BRepGraph_CacheValue> anExistingValue = theGraph.Cache().Get(theNode, aKindSlot);
  if (anExistingValue)
  {
    occ::handle<BRepGraphAlgo_BndBoxCacheValue> aBndValue =
      occ::down_cast<BRepGraphAlgo_BndBoxCacheValue>(anExistingValue);
    if (!aBndValue.IsNull())
    {
      aBndValue->SetData(aData);
      return;
    }
  }

  occ::handle<BRepGraphAlgo_BndBoxCacheValue> aNewValue = new BRepGraphAlgo_BndBoxCacheValue();
  aNewValue->SetData(aData);
  theGraph.Cache().Set(theNode, aKindSlot, aNewValue);
}

//=================================================================================================

void BRepGraphAlgo_BndLib::InvalidateCached(BRepGraph& theGraph, const BRepGraph_NodeId theNode)
{
  (void)theGraph.Cache().Remove(theNode, cacheKindSlot());
}
