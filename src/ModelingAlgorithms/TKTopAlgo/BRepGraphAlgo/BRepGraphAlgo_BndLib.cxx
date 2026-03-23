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

#include <BRepGraph.hxx>
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_TopoNode.hxx>
#include <BRepGraph_UsagesView.hxx>
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
static void findExactUVBounds(const BRepGraph& theGraph,
                              int              theFaceIdx,
                              double&          theUmin,
                              double&          theUmax,
                              double&          theVmin,
                              double&          theVmax,
                              double           theTol,
                              bool&            theIsNaturalRestriction);

//! Get the face usage's GlobalLocation for triangulation transform.
//! Returns identity if no usages exist.
static TopLoc_Location faceGlobalLocation(const BRepGraph& theGraph, int theFaceIdx)
{
  const BRepGraph_TopoNode::FaceDef& aFaceDef = theGraph.Defs().Face(theFaceIdx);
  if (aFaceDef.Usages.IsEmpty())
  {
    return TopLoc_Location();
  }
  return theGraph.Usages().Face(aFaceDef.Usages.First().Index).GlobalLocation;
}

//! Add vertex point + tolerance to the bounding box.
static void addVertexBox(const BRepGraph& theGraph, int theVertIdx, Bnd_Box& theBox)
{
  const BRepGraph_TopoNode::VertexDef& aVtxDef = theGraph.Defs().Vertex(theVertIdx);
  theBox.Add(aVtxDef.Point);
  theBox.Enlarge(aVtxDef.Tolerance);
}

//! Add edge curve bbox using the graph's CurveAdaptor.
static void addEdgeBox(const BRepGraph& theGraph, int theEdgeIdx, Bnd_Box& theBox)
{
  const BRepGraph_TopoNode::EdgeDef& anEdgeDef = theGraph.Defs().Edge(theEdgeIdx);
  if (anEdgeDef.IsDegenerate || anEdgeDef.Curve3d.IsNull())
  {
    return;
  }

  const GeomAdaptor_TransformedCurve aCurveAdaptor = theGraph.Defs().CurveAdaptor(anEdgeDef.Id);
  BndLib_Add3dCurve::Add(aCurveAdaptor, anEdgeDef.Tolerance, theBox);
}

//! Add face bbox using triangulation or geometry.
static void addFaceBox(const BRepGraph& theGraph, int theFaceIdx, Bnd_Box& theBox, bool theUseTri)
{
  const BRepGraph_TopoNode::FaceDef& aFaceDef = theGraph.Defs().Face(theFaceIdx);

  // Triangulation path (fast, common).
  const Handle(Poly_Triangulation) aTri = aFaceDef.ActiveTriangulation();
  if ((theUseTri || aFaceDef.Surface.IsNull()) && !aTri.IsNull())
  {
    const TopLoc_Location aLoc = faceGlobalLocation(theGraph, theFaceIdx);
    if (aTri->MinMax(theBox, aLoc))
    {
      theBox.Enlarge(aTri->Deflection() + aFaceDef.Tolerance);
      return;
    }
  }

  // Geometry path.
  if (aFaceDef.Surface.IsNull())
  {
    return;
  }

  // Check surface type (location-invariant).
  const BRepGraph_NodeId aFaceDefId(BRepGraph_NodeId::Kind::Face, theFaceIdx);
  GeomAdaptor_Surface    aGAS(aFaceDef.Surface);
  if (aGAS.GetType() != GeomAbs_Plane)
  {
    // Non-plane: use SurfaceAdaptor with UV bounds from PCurves (avoids face reconstruction).
    double aUMin, aUMax, aVMin, aVMax;
    bool   anIsNatural = false;
    findExactUVBounds(theGraph, theFaceIdx, aUMin, aUMax, aVMin, aVMax, aFaceDef.Tolerance, anIsNatural);
    const GeomAdaptor_TransformedSurface aSurfAdaptor =
      theGraph.Defs().SurfaceAdaptor(aFaceDefId, aUMin, aUMax, aVMin, aVMax);
    BndLib_AddSurface::Add(aSurfAdaptor, aFaceDef.Tolerance, theBox);
  }
  else
  {
    // Plane: use edge 3D curves directly from the graph.
    bool hasEdges = false;
    if (!aFaceDef.Usages.IsEmpty())
    {
      const BRepGraph_TopoNode::FaceUsage& aFaceUsage =
        theGraph.Usages().Face(aFaceDef.Usages.First().Index);

      auto addWireEdges = [&](BRepGraph_UsageId theWireUsageId) {
        const BRepGraph_TopoNode::WireUsage& aWireUsage =
          theGraph.Usages().Wire(theWireUsageId.Index);
        for (int anIdx = 0; anIdx < aWireUsage.EdgeUsages.Length(); ++anIdx)
        {
          const BRepGraph_TopoNode::EdgeUsage& anEdgeUsage =
            theGraph.Usages().Edge(aWireUsage.EdgeUsages.Value(anIdx).Index);
          const BRepGraph_TopoNode::EdgeDef& anEdgeDef =
            theGraph.Defs().Edge(anEdgeUsage.DefId.Index);
          if (anEdgeDef.IsDegenerate || anEdgeDef.Curve3d.IsNull())
          {
            continue;
          }
          const GeomAdaptor_TransformedCurve aCurveAdaptor =
            theGraph.Defs().CurveAdaptor(anEdgeDef.Id);
          BndLib_Add3dCurve::Add(aCurveAdaptor, anEdgeDef.Tolerance, theBox);
          hasEdges = true;
        }
      };

      if (aFaceUsage.OuterWireUsage.IsValid())
      {
        addWireEdges(aFaceUsage.OuterWireUsage);
      }
      for (int i = 0; i < aFaceUsage.InnerWireUsages.Length(); ++i)
      {
        addWireEdges(aFaceUsage.InnerWireUsages.Value(i));
      }
    }

    if (hasEdges)
    {
      theBox.Enlarge(aFaceDef.Tolerance);
    }
    else
    {
      // No edges: fall back to surface with UV bounds from PCurves.
      double aUMin2, aUMax2, aVMin2, aVMax2;
      bool   anIsNatural2 = false;
      findExactUVBounds(theGraph,
                        theFaceIdx,
                        aUMin2,
                        aUMax2,
                        aVMin2,
                        aVMax2,
                        aFaceDef.Tolerance,
                        anIsNatural2);
      const GeomAdaptor_TransformedSurface aSurfAdaptor2 =
        theGraph.Defs().SurfaceAdaptor(aFaceDefId, aUMin2, aUMax2, aVMin2, aVMax2);
      BndLib_AddSurface::Add(aSurfAdaptor2, aFaceDef.Tolerance, theBox);
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
static void findExactUVBounds(const BRepGraph& theGraph,
                              int              theFaceIdx,
                              double&          theUmin,
                              double&          theUmax,
                              double&          theVmin,
                              double&          theVmax,
                              double           /*theTol*/,
                              bool&            theIsNaturalRestriction)
{
  BRepGraphAlgo_UVBounds::CachedData aData;
  BRepGraphAlgo_UVBounds::Compute(theGraph, theFaceIdx, aData);
  if (aData.IsValid)
  {
    theUmin = aData.UMin;
    theUmax = aData.UMax;
    theVmin = aData.VMin;
    theVmax = aData.VMax;
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
                          int                      theFaceIdx,
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
  const BRepGraphAlgo_FClass2d aFClass(theGraph, theFaceIdx, std::max(aTolU, aTolV));

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
static void addEdgeBoxOptimal(const BRepGraph& theGraph,
                              int              theEdgeIdx,
                              Bnd_Box&         theBox,
                              bool             theUseShapeTol)
{
  const BRepGraph_TopoNode::EdgeDef& anEdgeDef = theGraph.Defs().Edge(theEdgeIdx);
  if (anEdgeDef.IsDegenerate || anEdgeDef.Curve3d.IsNull())
  {
    return;
  }
  const GeomAdaptor_TransformedCurve aCurveAdaptor = theGraph.Defs().CurveAdaptor(anEdgeDef.Id);
  const double                       aTol          = theUseShapeTol ? anEdgeDef.Tolerance : 0.;
  BndLib_Add3dCurve::AddOptimal(aCurveAdaptor, aTol, theBox);
}

//! Add precise face bbox (optimal path).
static void addFaceBoxOptimal(const BRepGraph& theGraph,
                              int              theFaceIdx,
                              Bnd_Box&         theBox,
                              bool             theUseTri,
                              bool             theUseShapeTol)
{
  const BRepGraph_TopoNode::FaceDef& aFaceDef = theGraph.Defs().Face(theFaceIdx);

  // Triangulation path.
  const Handle(Poly_Triangulation) aTri = aFaceDef.ActiveTriangulation();
  if (theUseTri && !aTri.IsNull())
  {
    Bnd_Box               aLocBox;
    const TopLoc_Location aLoc = faceGlobalLocation(theGraph, theFaceIdx);
    if (aTri->MinMax(aLocBox, aLoc))
    {
      aLocBox.Enlarge(aTri->Deflection() + aFaceDef.Tolerance);
      double xmin, ymin, zmin, xmax, ymax, zmax;
      aLocBox.Get(xmin, ymin, zmin, xmax, ymax, zmax);
      theBox.Update(xmin, ymin, zmin, xmax, ymax, zmax);
      return;
    }
  }

  // Geometry path.
  if (aFaceDef.Surface.IsNull())
  {
    return;
  }

  // Use SurfaceAdaptor directly (avoids face reconstruction).
  const BRepGraph_NodeId               aFaceDefId(BRepGraph_NodeId::Kind::Face, theFaceIdx);
  const GeomAdaptor_TransformedSurface aBS      = theGraph.Defs().SurfaceAdaptor(aFaceDefId);
  const double                         aFaceTol = theUseShapeTol ? aFaceDef.Tolerance : 0.;

  Bnd_Box aLocBox;
  if (canUseEdges(aBS))
  {
    // Edge-based path: iterate wire edges.
    bool hasEdges = false;
    if (!aFaceDef.Usages.IsEmpty())
    {
      const BRepGraph_TopoNode::FaceUsage& aFaceUsage =
        theGraph.Usages().Face(aFaceDef.Usages.First().Index);

      auto addWireEdgesOptimal = [&](BRepGraph_UsageId theWireUsageId) {
        const BRepGraph_TopoNode::WireUsage& aWireUsage =
          theGraph.Usages().Wire(theWireUsageId.Index);
        for (int anIdx = 0; anIdx < aWireUsage.EdgeUsages.Length(); ++anIdx)
        {
          const BRepGraph_TopoNode::EdgeUsage& anEdgeUsage =
            theGraph.Usages().Edge(aWireUsage.EdgeUsages.Value(anIdx).Index);
          const BRepGraph_TopoNode::EdgeDef& anEdgeDef =
            theGraph.Defs().Edge(anEdgeUsage.DefId.Index);
          if (anEdgeDef.IsDegenerate || anEdgeDef.Curve3d.IsNull())
          {
            continue;
          }
          Bnd_Box                            anEBox;
          const GeomAdaptor_TransformedCurve aCurveAdaptor =
            theGraph.Defs().CurveAdaptor(anEdgeDef.Id);
          const double aTol = theUseShapeTol ? anEdgeDef.Tolerance : 0.;
          BndLib_Add3dCurve::AddOptimal(aCurveAdaptor, aTol, anEBox);
          aLocBox.Add(anEBox);
          hasEdges = true;
        }
      };

      if (aFaceUsage.OuterWireUsage.IsValid())
      {
        addWireEdgesOptimal(aFaceUsage.OuterWireUsage);
      }
      for (int i = 0; i < aFaceUsage.InnerWireUsages.Length(); ++i)
      {
        addWireEdgesOptimal(aFaceUsage.InnerWireUsages.Value(i));
      }
    }

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
    findExactUVBounds(theGraph, theFaceIdx, umin, umax, vmin, vmax, aFaceTol, isNaturalRestriction);

    const GeomAdaptor_TransformedSurface aBSTrimmed =
      theGraph.Defs().SurfaceAdaptor(aFaceDefId, umin, umax, vmin, vmax);
    BndLib_AddSurface::AddOptimal(aBSTrimmed, umin, umax, vmin, vmax, aFaceTol, aLocBox);

    if (!isNaturalRestriction)
    {
      // Build edge box for adjustment.
      Bnd_Box aEBox;
      if (!aFaceDef.Usages.IsEmpty())
      {
        const BRepGraph_TopoNode::FaceUsage& aFaceUsage =
          theGraph.Usages().Face(aFaceDef.Usages.First().Index);

        auto addWireEdgesForAdjust = [&](BRepGraph_UsageId theWireUsageId) {
          const BRepGraph_TopoNode::WireUsage& aWireUsage =
            theGraph.Usages().Wire(theWireUsageId.Index);
          for (int anIdx = 0; anIdx < aWireUsage.EdgeUsages.Length(); ++anIdx)
          {
            const BRepGraph_TopoNode::EdgeUsage& anEdgeUsage =
              theGraph.Usages().Edge(aWireUsage.EdgeUsages.Value(anIdx).Index);
            const BRepGraph_TopoNode::EdgeDef& anEdgeDef =
              theGraph.Defs().Edge(anEdgeUsage.DefId.Index);
            if (anEdgeDef.IsDegenerate || anEdgeDef.Curve3d.IsNull())
            {
              continue;
            }
            Bnd_Box                            anEBox;
            const GeomAdaptor_TransformedCurve aCurveAdaptor =
              theGraph.Defs().CurveAdaptor(anEdgeDef.Id);
            const double aTol = theUseShapeTol ? anEdgeDef.Tolerance : 0.;
            BndLib_Add3dCurve::AddOptimal(aCurveAdaptor, aTol, anEBox);
            aEBox.Add(anEBox);
          }
        };

        if (aFaceUsage.OuterWireUsage.IsValid())
        {
          addWireEdgesForAdjust(aFaceUsage.OuterWireUsage);
        }
        for (int i = 0; i < aFaceUsage.InnerWireUsages.Length(); ++i)
        {
          addWireEdgesForAdjust(aFaceUsage.InnerWireUsages.Value(i));
        }
      }
      adjustFaceBox(theGraph,
                    theFaceIdx,
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
static void addNodeBox(const BRepGraph& theGraph,
                       BRepGraph_NodeId theNode,
                       Bnd_Box&         theBox,
                       bool             theUseTri)
{
  if (!theNode.IsValid())
  {
    return;
  }

  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Vertex: {
      addVertexBox(theGraph, theNode.Index, theBox);
      break;
    }
    case BRepGraph_NodeId::Kind::Edge: {
      addEdgeBox(theGraph, theNode.Index, theBox);
      break;
    }
    case BRepGraph_NodeId::Kind::Wire: {
      const BRepGraph_TopoNode::WireDef& aWireDef = theGraph.Defs().Wire(theNode.Index);
      if (!aWireDef.Usages.IsEmpty())
      {
        const BRepGraph_TopoNode::WireUsage& aWireUsage =
          theGraph.Usages().Wire(aWireDef.Usages.Value(0).Index);
        for (int anIdx = 0; anIdx < aWireUsage.EdgeUsages.Length(); ++anIdx)
        {
          const BRepGraph_TopoNode::EdgeUsage& anEdgeUsage =
            theGraph.Usages().Edge(aWireUsage.EdgeUsages.Value(anIdx).Index);
          addNodeBox(theGraph, anEdgeUsage.DefId, theBox, theUseTri);
        }
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Face: {
      addFaceBox(theGraph, theNode.Index, theBox, theUseTri);
      break;
    }
    case BRepGraph_NodeId::Kind::Shell: {
      const int aNbFaces = theGraph.Defs().NbShellFaces(theNode.Index);
      for (int i = 0; i < aNbFaces; ++i)
      {
        const BRepGraph_NodeId aFaceDefId = theGraph.Defs().ShellFaceDef(theNode.Index, i);
        addFaceBox(theGraph, aFaceDefId.Index, theBox, theUseTri);
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Solid: {
      const BRepGraph_TopoNode::SolidDef& aSolidDef = theGraph.Defs().Solid(theNode.Index);
      if (aSolidDef.Usages.IsEmpty())
      {
        break;
      }
      const BRepGraph_TopoNode::SolidUsage& aSolidUsage =
        theGraph.Usages().Solid(aSolidDef.Usages.First().Index);
      for (int anIdx = 0; anIdx < aSolidUsage.ShellUsages.Length(); ++anIdx)
      {
        const BRepGraph_NodeId aShellDefId = theGraph.DefOf(aSolidUsage.ShellUsages.Value(anIdx));
        addNodeBox(theGraph, aShellDefId, theBox, theUseTri);
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Compound: {
      const BRepGraph_TopoNode::CompoundDef& aCompDef = theGraph.Defs().Compound(theNode.Index);
      for (int anIdx = 0; anIdx < aCompDef.ChildDefIds.Length(); ++anIdx)
      {
        addNodeBox(theGraph, aCompDef.ChildDefIds.Value(anIdx), theBox, theUseTri);
      }
      break;
    }
    case BRepGraph_NodeId::Kind::CompSolid: {
      const BRepGraph_TopoNode::CompSolidDef& aCSolDef = theGraph.Defs().CompSolid(theNode.Index);
      for (int anIdx = 0; anIdx < aCSolDef.SolidDefIds.Length(); ++anIdx)
      {
        addNodeBox(theGraph, aCSolDef.SolidDefIds.Value(anIdx), theBox, theUseTri);
      }
      break;
    }
    default:
      break;
  }
}

//! Recursive per-node optimal bounding box computation.
static void addNodeBoxOptimal(const BRepGraph& theGraph,
                              BRepGraph_NodeId theNode,
                              Bnd_Box&         theBox,
                              bool             theUseTri,
                              bool             theUseShapeTol)
{
  if (!theNode.IsValid())
  {
    return;
  }

  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Vertex: {
      const BRepGraph_TopoNode::VertexDef& aVtxDef = theGraph.Defs().Vertex(theNode.Index);
      Bnd_Box                              aLocBox;
      aLocBox.Add(aVtxDef.Point);
      const double aTol = theUseShapeTol ? aVtxDef.Tolerance : 0.;
      aLocBox.Enlarge(aTol);
      double xmin, ymin, zmin, xmax, ymax, zmax;
      aLocBox.Get(xmin, ymin, zmin, xmax, ymax, zmax);
      theBox.Update(xmin, ymin, zmin, xmax, ymax, zmax);
      break;
    }
    case BRepGraph_NodeId::Kind::Edge: {
      Bnd_Box aLocBox;
      addEdgeBoxOptimal(theGraph, theNode.Index, aLocBox, theUseShapeTol);
      if (!aLocBox.IsVoid())
      {
        double xmin, ymin, zmin, xmax, ymax, zmax;
        aLocBox.Get(xmin, ymin, zmin, xmax, ymax, zmax);
        theBox.Update(xmin, ymin, zmin, xmax, ymax, zmax);
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Wire: {
      const BRepGraph_TopoNode::WireDef& aWireDef = theGraph.Defs().Wire(theNode.Index);
      if (!aWireDef.Usages.IsEmpty())
      {
        const BRepGraph_TopoNode::WireUsage& aWireUsage =
          theGraph.Usages().Wire(aWireDef.Usages.Value(0).Index);
        for (int anIdx = 0; anIdx < aWireUsage.EdgeUsages.Length(); ++anIdx)
        {
          const BRepGraph_TopoNode::EdgeUsage& anEdgeUsage =
            theGraph.Usages().Edge(aWireUsage.EdgeUsages.Value(anIdx).Index);
          addNodeBoxOptimal(theGraph,
                            anEdgeUsage.DefId,
                            theBox,
                            theUseTri,
                            theUseShapeTol);
        }
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Face: {
      addFaceBoxOptimal(theGraph, theNode.Index, theBox, theUseTri, theUseShapeTol);
      break;
    }
    case BRepGraph_NodeId::Kind::Shell: {
      const int aNbFaces = theGraph.Defs().NbShellFaces(theNode.Index);
      for (int i = 0; i < aNbFaces; ++i)
      {
        const BRepGraph_NodeId aFaceDefId = theGraph.Defs().ShellFaceDef(theNode.Index, i);
        addFaceBoxOptimal(theGraph, aFaceDefId.Index, theBox, theUseTri, theUseShapeTol);
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Solid: {
      const BRepGraph_TopoNode::SolidDef& aSolidDef = theGraph.Defs().Solid(theNode.Index);
      if (aSolidDef.Usages.IsEmpty())
      {
        break;
      }
      const BRepGraph_TopoNode::SolidUsage& aSolidUsage =
        theGraph.Usages().Solid(aSolidDef.Usages.First().Index);
      for (int anIdx = 0; anIdx < aSolidUsage.ShellUsages.Length(); ++anIdx)
      {
        const BRepGraph_NodeId aShellDefId = theGraph.DefOf(aSolidUsage.ShellUsages.Value(anIdx));
        addNodeBoxOptimal(theGraph, aShellDefId, theBox, theUseTri, theUseShapeTol);
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Compound: {
      const BRepGraph_TopoNode::CompoundDef& aCompDef = theGraph.Defs().Compound(theNode.Index);
      for (int anIdx = 0; anIdx < aCompDef.ChildDefIds.Length(); ++anIdx)
      {
        addNodeBoxOptimal(theGraph,
                          aCompDef.ChildDefIds.Value(anIdx),
                          theBox,
                          theUseTri,
                          theUseShapeTol);
      }
      break;
    }
    case BRepGraph_NodeId::Kind::CompSolid: {
      const BRepGraph_TopoNode::CompSolidDef& aCSolDef = theGraph.Defs().CompSolid(theNode.Index);
      for (int anIdx = 0; anIdx < aCSolDef.SolidDefIds.Length(); ++anIdx)
      {
        addNodeBoxOptimal(theGraph,
                          aCSolDef.SolidDefIds.Value(anIdx),
                          theBox,
                          theUseTri,
                          theUseShapeTol);
      }
      break;
    }
    default:
      break;
  }
}

} // namespace

//=================================================================================================

void BRepGraphAlgo_BndLib::Add(const BRepGraph& theGraph, Bnd_Box& theBox, bool theUseTriangulation)
{
  // Add all faces.
  const int aNbFaces = theGraph.Defs().NbFaces();
  for (int i = 0; i < aNbFaces; ++i)
  {
    addFaceBox(theGraph, i, theBox, theUseTriangulation);
  }

  // Add free edges (edges not in any face, identified by having no PCurves).
  const int aNbEdges = theGraph.Defs().NbEdges();
  for (int i = 0; i < aNbEdges; ++i)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdgeDef = theGraph.Defs().Edge(i);
    if (anEdgeDef.PCurves.IsEmpty())
    {
      addEdgeBox(theGraph, i, theBox);
    }
  }

  // Add free vertices (vertices not in edges).
  // For simplicity, add all vertices — redundant additions are harmless.
  const int aNbVerts = theGraph.Defs().NbVertices();
  for (int i = 0; i < aNbVerts; ++i)
  {
    addVertexBox(theGraph, i, theBox);
  }
}

//=================================================================================================

void BRepGraphAlgo_BndLib::Add(const BRepGraph& theGraph,
                               BRepGraph_NodeId theNode,
                               Bnd_Box&         theBox,
                               bool             theUseTriangulation)
{
  addNodeBox(theGraph, theNode, theBox, theUseTriangulation);
}

//=================================================================================================

void BRepGraphAlgo_BndLib::AddOptimal(const BRepGraph& theGraph,
                                      Bnd_Box&         theBox,
                                      bool             theUseTriangulation,
                                      bool             theUseShapeTolerance)
{
  // Add all faces (optimal).
  const int aNbFaces = theGraph.Defs().NbFaces();
  for (int i = 0; i < aNbFaces; ++i)
  {
    addFaceBoxOptimal(theGraph, i, theBox, theUseTriangulation, theUseShapeTolerance);
  }

  // Add free edges.
  const int aNbEdges = theGraph.Defs().NbEdges();
  for (int i = 0; i < aNbEdges; ++i)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdgeDef = theGraph.Defs().Edge(i);
    if (anEdgeDef.PCurves.IsEmpty())
    {
      Bnd_Box aLocBox;
      addEdgeBoxOptimal(theGraph, i, aLocBox, theUseShapeTolerance);
      if (!aLocBox.IsVoid())
      {
        double xmin, ymin, zmin, xmax, ymax, zmax;
        aLocBox.Get(xmin, ymin, zmin, xmax, ymax, zmax);
        theBox.Update(xmin, ymin, zmin, xmax, ymax, zmax);
      }
    }
  }

  // Add free vertices.
  const int aNbVerts = theGraph.Defs().NbVertices();
  for (int i = 0; i < aNbVerts; ++i)
  {
    const BRepGraph_TopoNode::VertexDef& aVtxDef = theGraph.Defs().Vertex(i);
    Bnd_Box                              aLocBox;
    aLocBox.Add(aVtxDef.Point);
    const double aTol = theUseShapeTolerance ? aVtxDef.Tolerance : 0.;
    aLocBox.Enlarge(aTol);
    double xmin, ymin, zmin, xmax, ymax, zmax;
    aLocBox.Get(xmin, ymin, zmin, xmax, ymax, zmax);
    theBox.Update(xmin, ymin, zmin, xmax, ymax, zmax);
  }
}

//=================================================================================================

void BRepGraphAlgo_BndLib::AddOptimal(const BRepGraph& theGraph,
                                      BRepGraph_NodeId theNode,
                                      Bnd_Box&         theBox,
                                      bool             theUseTriangulation,
                                      bool             theUseShapeTolerance)
{
  addNodeBoxOptimal(theGraph, theNode, theBox, theUseTriangulation, theUseShapeTolerance);
}

//=================================================================================================
// Cached API -- bounding box attribute stored as user attribute on graph nodes
//=================================================================================================

namespace
{

//! GUID for bounding box cache attribute.
static const Standard_GUID THE_BNDBOX_GUID("b7e3a1f0-4c82-4d5e-9a1b-3f8e2d7c6a05");

//! Internal attribute class storing precision-aware bbox data.
class BRepGraphAlgo_BndBoxAttribute : public BRepGraph_UserAttribute
{
public:
  BRepGraphAlgo_BndBoxAttribute() = default;

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

int BRepGraphAlgo_BndLib::CacheKey()
{
  static const int THE_KEY = BRepGraph_UserAttribute::AllocateKey(THE_BNDBOX_GUID);
  return THE_KEY;
}

//=================================================================================================

bool BRepGraphAlgo_BndLib::GetCached(const BRepGraph&                  theGraph,
                                     BRepGraph_NodeId                  theNode,
                                     BRepGraphAlgo_BndLib::CachedData& theData)
{
  const int                          aKey = CacheKey();
  const BRepGraph_TopoNode::BaseDef* aDef = theGraph.TopoDef(theNode);
  if (aDef == nullptr)
  {
    return false;
  }

  BRepGraph_UserAttrPtr anAttr = aDef->Cache.GetUserAttribute(aKey);
  if (!anAttr)
  {
    return false;
  }

  auto* aBndAttr = dynamic_cast<BRepGraphAlgo_BndBoxAttribute*>(anAttr.get());
  if (aBndAttr == nullptr)
  {
    return false;
  }

  return aBndAttr->GetAny(theData);
}

//=================================================================================================

Bnd_Box BRepGraphAlgo_BndLib::AddCached(BRepGraph&                      theGraph,
                                        BRepGraph_NodeId                theNode,
                                        BRepGraphAlgo_BndLib::Precision thePrecision,
                                        bool                            theUseTriangulation)
{
  const int aKey = CacheKey();

  // Try to read existing cached value.
  BRepGraph_NodeCache* aCache = theGraph.mutableCache(theNode);
  if (aCache != nullptr)
  {
    BRepGraph_UserAttrPtr anExisting = aCache->GetUserAttribute(aKey);
    if (anExisting)
    {
      auto* aBndAttr = dynamic_cast<BRepGraphAlgo_BndBoxAttribute*>(anExisting.get());
      if (aBndAttr != nullptr)
      {
        BRepGraphAlgo_BndLib::CachedData aData;
        if (aBndAttr->GetIfSufficient(thePrecision, aData))
        {
          return aData.Box;
        }
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
  }

  // Cache the result.
  BRepGraphAlgo_BndLib::CachedData aData;
  aData.Box                = aBox;
  aData.BoxPrecision       = thePrecision;
  aData.UsedTriangulation  = theUseTriangulation;
  aData.UsedShapeTolerance = aUsedShapeTol;

  if (aCache != nullptr)
  {
    BRepGraph_UserAttrPtr anExisting = aCache->GetUserAttribute(aKey);
    if (anExisting)
    {
      auto* aBndAttr = dynamic_cast<BRepGraphAlgo_BndBoxAttribute*>(anExisting.get());
      if (aBndAttr != nullptr)
      {
        aBndAttr->SetData(aData);
        return aBox;
      }
    }

    auto aNewAttr = std::make_shared<BRepGraphAlgo_BndBoxAttribute>();
    aNewAttr->SetData(aData);
    aCache->SetUserAttribute(aKey, aNewAttr);
  }

  return aBox;
}

//=================================================================================================

void BRepGraphAlgo_BndLib::SetCached(BRepGraph&       theGraph,
                                     BRepGraph_NodeId theNode,
                                     const Bnd_Box&   theBox,
                                     Precision        thePrecision,
                                     bool             theUsedTriangulation,
                                     bool             theUsedShapeTolerance)
{
  const int            aKey   = CacheKey();
  BRepGraph_NodeCache* aCache = theGraph.mutableCache(theNode);
  if (aCache == nullptr)
  {
    return;
  }

  CachedData aData;
  aData.Box                = theBox;
  aData.BoxPrecision       = thePrecision;
  aData.UsedTriangulation  = theUsedTriangulation;
  aData.UsedShapeTolerance = theUsedShapeTolerance;

  BRepGraph_UserAttrPtr anExisting = aCache->GetUserAttribute(aKey);
  if (anExisting)
  {
    auto* aBndAttr = dynamic_cast<BRepGraphAlgo_BndBoxAttribute*>(anExisting.get());
    if (aBndAttr != nullptr)
    {
      aBndAttr->SetData(aData);
      return;
    }
  }

  auto aNewAttr = std::make_shared<BRepGraphAlgo_BndBoxAttribute>();
  aNewAttr->SetData(aData);
  aCache->SetUserAttribute(aKey, aNewAttr);
}

//=================================================================================================

void BRepGraphAlgo_BndLib::InvalidateCached(BRepGraph& theGraph, BRepGraph_NodeId theNode)
{
  const int            aKey   = CacheKey();
  BRepGraph_NodeCache* aCache = theGraph.mutableCache(theNode);
  if (aCache == nullptr)
  {
    return;
  }
  aCache->InvalidateUserAttribute(aKey);
}
