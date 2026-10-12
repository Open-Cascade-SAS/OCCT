// Created on: 1993-12-21
// Created by: Isabelle GRIGNON
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <ChFi3d.hxx>

#include <BRep_Tool.hxx>
#include <ChFi3d_Builder_0.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <BRepTools.hxx>
#include <IntTools_Tools.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepTopAdaptor_TopolTool.hxx>
#include <LocalAnalysis_SurfaceContinuity.hxx>
#include <TopOpeBRepTool_TOOL.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2d_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom_Surface.hxx>
#include <NCollection_LinearVector.hxx>

#include <algorithm>
#include <cmath>

namespace
{
// Pcurves and parameter ranges of the common edge.
struct FacePCurves
{
  occ::handle<Geom2d_Curve> Curve1;       //!< Pcurve on the first face.
  occ::handle<Geom2d_Curve> Curve2;       //!< Pcurve on the second face.
  double                    First1 = 0.0; //!< First parameter on the first face.
  double                    Last1  = 0.0; //!< Last parameter on the first face.
  double                    First2 = 0.0; //!< First parameter on the second face.
  double                    Last2  = 0.0; //!< Last parameter on the second face.
};

//==================================================================================================

// Select opposite pcurves for seams, using the first face occurrence for shared seams.
bool getFacePCurves(const TopoDS_Edge& theEdge,
                    const TopoDS_Face& theFace1,
                    const TopoDS_Face& theFace2,
                    FacePCurves&       theCurves)
{
  TopoDS_Edge anEdge1 = theEdge, anEdge2 = theEdge;
  TopoDS_Face aFace1 = theFace1, aFace2 = theFace2;
  if (theFace1.IsSame(theFace2))
  {
    anEdge2.Reverse();
  }
  else if (BRep_Tool::IsClosed(theEdge, theFace1) && BRep_Tool::IsClosed(theEdge, theFace2))
  {
    aFace1.Orientation(TopAbs_FORWARD);
    aFace2.Orientation(TopAbs_FORWARD);
    anEdge1.Nullify();
    for (TopExp_Explorer anEdges(aFace1, TopAbs_EDGE); anEdges.More(); anEdges.Next())
    {
      if (anEdges.Current().IsSame(theEdge))
      {
        anEdge1 = TopoDS::Edge(anEdges.Current());
        break;
      }
    }
    if (anEdge1.IsNull())
    {
      return false;
    }
    anEdge2 = anEdge1;
    anEdge2.Reverse();
  }
  theCurves.Curve1 = BRep_Tool::CurveOnSurface(anEdge1, aFace1, theCurves.First1, theCurves.Last1);
  theCurves.Curve2 = BRep_Tool::CurveOnSurface(anEdge2, aFace2, theCurves.First2, theCurves.Last2);
  return !theCurves.Curve1.IsNull() && !theCurves.Curve2.IsNull();
}

//==================================================================================================

// Append internal continuity breaks of a pcurve and its supporting surface.
void appendContinuityIntervals(const occ::handle<Geom2d_Curve>&  thePCurve,
                               const occ::handle<Geom_Surface>&  theSurface,
                               const double                      theFirst,
                               const double                      theLast,
                               NCollection_LinearVector<double>& theParameters)
{
  if (thePCurve->Continuity() == GeomAbs_CN && theSurface->Continuity() == GeomAbs_CN)
  {
    return;
  }
  Adaptor3d_CurveOnSurface   aCurve(new Geom2dAdaptor_Curve(thePCurve, theFirst, theLast),
                                  new GeomAdaptor_Surface(theSurface));
  NCollection_Array1<double> anIntervals(1, aCurve.NbIntervals(GeomAbs_CN) + 1);
  aCurve.Intervals(anIntervals, GeomAbs_CN);
  for (double aParameter : anIntervals)
  {
    if (aParameter > theFirst && aParameter < theLast)
    {
      theParameters.Append(aParameter);
    }
  }
}

//==================================================================================================

// Reuse a common rigid placement; otherwise evaluate both surfaces in global coordinates.
bool getFaceSurfaces(const TopoDS_Face&         theFace1,
                     const TopoDS_Face&         theFace2,
                     occ::handle<Geom_Surface>& theSurface1,
                     occ::handle<Geom_Surface>& theSurface2)
{
  TopLoc_Location aLocation1, aLocation2;
  theSurface1 = BRep_Tool::Surface(theFace1, aLocation1);
  theSurface2 = BRep_Tool::Surface(theFace2, aLocation2);
  if (theSurface1.IsNull() || theSurface2.IsNull())
  {
    return false;
  }

  // A common rigid frame preserves distances, angles and curvature.
  if (aLocation1 != aLocation2 || std::abs(aLocation1.Transformation().ScaleFactor()) != 1.0)
  {
    if (!aLocation1.IsIdentity())
    {
      theSurface1 =
        occ::down_cast<Geom_Surface>(theSurface1->Transformed(aLocation1.Transformation()));
    }
    if (!aLocation2.IsIdentity())
    {
      theSurface2 =
        occ::down_cast<Geom_Surface>(theSurface2->Transformed(aLocation2.Transformation()));
    }
  }

  return true;
}

//==================================================================================================

// Choose the sample count from surface complexity.
int surfaceSampleCount(const TopoDS_Face& theFace)
{
  BRepTopAdaptor_TopolTool aTool(new BRepAdaptor_Surface(theFace));
  return aTool.NbSamples();
}
} // namespace

static void Correct2dPoint(const TopoDS_Face& theF, gp_Pnt2d& theP2d);

//

//=================================================================================================

ChFiDS_TypeOfConcavity ChFi3d::DefineConnectType(const TopoDS_Edge& E,
                                                 const TopoDS_Face& F1,
                                                 const TopoDS_Face& F2,
                                                 const double       SinTol,
                                                 const bool         CorrectPoint)
{
  const occ::handle<Geom_Surface>& S1 = BRep_Tool::Surface(F1);
  const occ::handle<Geom_Surface>& S2 = BRep_Tool::Surface(F2);
  //
  double                    f, l;
  occ::handle<Geom2d_Curve> C1 = BRep_Tool::CurveOnSurface(E, F1, f, l);
  // For the case of seam edge
  TopoDS_Edge EE = E;
  if (F1.IsSame(F2))
  {
    EE.Reverse();
  }
  occ::handle<Geom2d_Curve> C2 = BRep_Tool::CurveOnSurface(EE, F2, f, l);
  if (C1.IsNull() || C2.IsNull())
  {
    return ChFiDS_Other;
  }

  BRepAdaptor_Curve C(E);
  f = C.FirstParameter();
  l = C.LastParameter();
  //
  double ParOnC = 0.5 * (f + l);
  gp_Vec T1     = C.DN(ParOnC, 1);
  if (T1.SquareMagnitude() <= gp::Resolution())
  {
    ParOnC = IntTools_Tools::IntermediatePoint(f, l);
    T1     = C.DN(ParOnC, 1);
  }
  if (T1.SquareMagnitude() > gp::Resolution())
  {
    T1.Normalize();
  }

  if (BRepTools::OriEdgeInFace(E, F1) == TopAbs_REVERSED)
  {
    T1.Reverse();
  }
  if (F1.Orientation() == TopAbs_REVERSED)
  {
    T1.Reverse();
  }

  gp_Pnt2d P = C1->Value(ParOnC);
  gp_Pnt   P3;
  gp_Vec   D1U, D1V;

  if (CorrectPoint)
  {
    Correct2dPoint(F1, P);
  }
  //
  S1->D1(P.X(), P.Y(), P3, D1U, D1V);
  gp_Vec DN1(D1U ^ D1V);
  if (F1.Orientation() == TopAbs_REVERSED)
  {
    DN1.Reverse();
  }

  P = C2->Value(ParOnC);
  if (CorrectPoint)
  {
    Correct2dPoint(F2, P);
  }
  S2->D1(P.X(), P.Y(), P3, D1U, D1V);
  gp_Vec DN2(D1U ^ D1V);
  if (F2.Orientation() == TopAbs_REVERSED)
  {
    DN2.Reverse();
  }

  DN1.Normalize();
  DN2.Normalize();

  gp_Vec ProVec     = DN1 ^ DN2;
  double NormProVec = ProVec.Magnitude();
  if (NormProVec < SinTol)
  {
    // plane
    if (DN1.Dot(DN2) > 0)
    {
      // Tangent
      return ChFiDS_Tangential;
    }
    else
    {
      // Mixed not finished!
#ifdef OCCT_DEBUG
      std::cout << " faces locally mixed" << std::endl;
#endif
      return ChFiDS_Convex;
    }
  }
  else
  {
    if (NormProVec > gp::Resolution())
    {
      ProVec /= NormProVec;
    }
    double Prod = T1.Dot(ProVec);
    if (Prod > 0.)
    {
      //
      return ChFiDS_Convex;
    }
    else
    {
      // reenters
      return ChFiDS_Concave;
    }
  }
}

//==================================================================================================

double ChFi3d::ConstructionContinuityTolerance(const TopoDS_Edge& theEdge)
{
  return std::max(0.001, 1.5 * BRep_Tool::Tolerance(theEdge));
}

//==================================================================================================

bool ChFi3d::IsTangentFaces(const TopoDS_Edge&  theEdge,
                            const TopoDS_Face&  theFace1,
                            const TopoDS_Face&  theFace2,
                            const GeomAbs_Shape theOrder)
{
  if (theOrder == GeomAbs_G1 && BRep_Tool::Continuity(theEdge, theFace1, theFace2) != GeomAbs_C0)
  {
    return true;
  }

  const double aTolC0 = ConstructionContinuityTolerance(theEdge);

  FacePCurves aCurves;
  if (!getFacePCurves(theEdge, theFace1, theFace2, aCurves))
  {
    return false;
  }
  const double aFirst = aCurves.First2;
  const double aLast  = aCurves.Last2;

  occ::handle<Geom_Surface> aSurf1, aSurf2;
  if (!getFaceSurfaces(theFace1, theFace2, aSurf1, aSurf2))
  {
    return false;
  }

  const int aNbSamples1 = surfaceSampleCount(theFace1);
  const int aNbSamples2 = theFace1.IsSame(theFace2) ? aNbSamples1 : surfaceSampleCount(theFace2);
  const int aNbSamples  = std::max(aNbSamples1, aNbSamples2);

  const double aDelta     = (aLast - aFirst) / (aNbSamples - 1);
  double       aPar       = aFirst;
  int          aNbNotDone = 0;

  for (int aSample = 1; aSample <= aNbSamples; ++aSample, aPar += aDelta)
  {
    if (aSample == aNbSamples)
    {
      aPar = aLast;
    }

    LocalAnalysis_SurfaceContinuity aCont(aCurves.Curve1,
                                          aCurves.Curve2,
                                          aPar,
                                          aSurf1,
                                          aSurf2,
                                          theOrder,
                                          0.001,
                                          aTolC0,
                                          0.1,
                                          0.1,
                                          0.1);
    if (!aCont.IsDone())
    {
      if (theOrder == GeomAbs_C2 && aCont.StatusError() == LocalAnalysis_NullSecondDerivative)
      {
        continue;
      }

      aNbNotDone++;
      continue;
    }

    if (!(theOrder == GeomAbs_G1 ? aCont.IsG1() : aCont.IsG2()))
    {
      return false;
    }
  }

  if (aNbNotDone == aNbSamples)
  {
    return false;
  }

  const double   aMiddle = (aFirst + aLast) / 2.0;
  const gp_Pnt2d aUV1    = aCurves.Curve1->EvalD0(aMiddle);
  const gp_Pnt2d aUV2    = aCurves.Curve2->EvalD0(aMiddle);
  gp_Dir         aNormal1, aNormal2;
  TopOpeBRepTool_TOOL::Nt(aUV1, theFace1, aNormal1);
  TopOpeBRepTool_TOOL::Nt(aUV2, theFace2, aNormal2);
  return aNormal1.Dot(aNormal2) >= 0.0;
}

//==================================================================================================

bool ChFi3d::IsTangentFaces(const TopoDS_Edge&  theEdge,
                            const TopoDS_Face&  theFace1,
                            const TopoDS_Face&  theFace2,
                            const GeomAbs_Shape theOrder,
                            const double        theAngularTolerance)
{
  if (!std::isfinite(theAngularTolerance) || theAngularTolerance < 0.0
      || theAngularTolerance >= M_PI / 2.0 || theEdge.IsNull() || theFace1.IsNull()
      || theFace2.IsNull() || (theOrder != GeomAbs_G1 && theOrder != GeomAbs_G2)
      || (theFace1.IsSame(theFace2) && !BRep_Tool::IsClosed(theEdge, theFace1)))
  {
    return false;
  }

  // Each surface representation can deviate from the common edge within its tolerance.
  const double aTolC0 = 2.0 * BRep_Tool::Tolerance(theEdge);
  FacePCurves  aCurves;
  if (!getFacePCurves(theEdge, theFace1, theFace2, aCurves))
  {
    return false;
  }

  // Both representations must cover the same parameter range.
  if (std::abs(aCurves.First1 - aCurves.First2) > Precision::PConfusion()
      || std::abs(aCurves.Last1 - aCurves.Last2) > Precision::PConfusion())
  {
    return false;
  }
  const double aFirst = std::max(aCurves.First1, aCurves.First2);
  const double aLast  = std::min(aCurves.Last1, aCurves.Last2);
  if (aFirst >= aLast)
  {
    return false;
  }

  occ::handle<Geom_Surface> aSurf1, aSurf2;
  if (!getFaceSurfaces(theFace1, theFace2, aSurf1, aSurf2))
  {
    return false;
  }

  // Start with D1; curvature evaluation requests D2 only for G2.
  GeomLProp_SLProps               aProps1(aSurf1, 1, Precision::Confusion());
  GeomLProp_SLProps               aProps2(aSurf2, 1, Precision::Confusion());
  LocalAnalysis_SurfaceContinuity aContinuity(Precision::Confusion(),
                                              aTolC0,
                                              Precision::Angular(),
                                              Precision::Angular(),
                                              theAngularTolerance);
  const double                    aSquareTolC0    = aTolC0 * aTolC0;
  const double                    aSinTolG1       = std::sin(theAngularTolerance);
  const double                    aSquareSinTolG1 = aSinTolG1 * aSinTolG1;
  const double                    anOrientation =
    (theFace1.Orientation() == TopAbs_REVERSED) == (theFace2.Orientation() == TopAbs_REVERSED)
                         ? 1.0
                         : -1.0;
  const auto isTangent = [&](const double theParameter) {
    const bool     isEndPoint = theParameter == aFirst || theParameter == aLast;
    const gp_Pnt2d aUV1       = aCurves.Curve1->EvalD0(theParameter);
    const gp_Pnt2d aUV2       = aCurves.Curve2->EvalD0(theParameter);
    aProps1.SetParameters(aUV1.X(), aUV1.Y());
    aProps2.SetParameters(aUV2.X(), aUV2.Y());
    // Positional continuity remains mandatory even at a singular endpoint.
    if (!(aProps1.Value().SquareDistance(aProps2.Value()) <= aSquareTolC0))
    {
      return false;
    }
    if (!aProps1.IsNormalDefined() || !aProps2.IsNormalDefined())
    {
      return isEndPoint;
    }
    // Cross products resolve tiny angles where cosine rounds to 1.
    // The dot product distinguishes aligned from opposed oriented normals.
    const gp_XYZ& aNormal1 = aProps1.Normal().XYZ();
    const gp_XYZ& aNormal2 = aProps2.Normal().XYZ();
    if (!(anOrientation * aNormal1.Dot(aNormal2) > 0.0)
        || !(aNormal1.Crossed(aNormal2).SquareModulus() <= aSquareSinTolG1))
    {
      return false;
    }
    if (theOrder == GeomAbs_G1)
    {
      return true;
    }
    aContinuity.ComputeAnalysis(aProps1, aProps2, GeomAbs_G2);
    return aContinuity.IsDone()
             ? aContinuity.IsG2()
             : isEndPoint && aContinuity.StatusError() == LocalAnalysis_CurvatureNotDefined;
  };

  // Reject the common non-tangent case before building interval and sampling
  // tools. This point is skipped when encountered again in the sample grid.
  const double aMiddle = 0.5 * aFirst + 0.5 * aLast;
  if (aMiddle == aFirst || aMiddle == aLast || !isTangent(aMiddle))
  {
    return false;
  }

  // Split at internal continuity breaks; sampling is not a certified bound.
  NCollection_LinearVector<double> aParameters;
  appendContinuityIntervals(aCurves.Curve1, aSurf1, aFirst, aLast, aParameters);
  appendContinuityIntervals(aCurves.Curve2, aSurf2, aFirst, aLast, aParameters);
  if (aParameters.Size() > 1)
  {
    std::sort(aParameters.begin(), aParameters.end());
  }

  int aNbSamples = surfaceSampleCount(theFace1);
  if (!theFace1.IsSame(theFace2))
  {
    aNbSamples = std::max(aNbSamples, surfaceSampleCount(theFace2));
  }
  // Preserve the sampling density, with a midpoint in every interval.
  aNbSamples = std::max(3, aNbSamples) | 1;
  for (size_t anIndex = 0; anIndex <= aParameters.Size(); ++anIndex)
  {
    const double aStart = anIndex == 0 ? aFirst : aParameters[anIndex - 1];
    const double anEnd  = anIndex == aParameters.Size() ? aLast : aParameters[anIndex];
    if (aStart == anEnd)
    {
      continue;
    }
    // A shared interval boundary has already been evaluated by its predecessor.
    for (int aSample = anIndex == 0 ? 0 : 1; aSample < aNbSamples; ++aSample)
    {
      const double aRatio     = double(aSample) / double(aNbSamples - 1);
      const double aParameter = (1.0 - aRatio) * aStart + aRatio * anEnd;
      if (aParameter != aMiddle && !isTangent(aParameter))
      {
        return false;
      }
    }
  }
  return true;
}

//=======================================================================
// function : ConcaveSide
// purpose  : calculate the concave face at the neighborhood of the border of
//           2 faces.
//=======================================================================
int ChFi3d::ConcaveSide(const BRepAdaptor_Surface& S1,
                        const BRepAdaptor_Surface& S2,
                        const TopoDS_Edge&         E,
                        TopAbs_Orientation&        Or1,
                        TopAbs_Orientation&        Or2)

{
  int ChoixConge;
  Or1 = Or2 = TopAbs_FORWARD;
  BRepAdaptor_Curve CE(E);
  double            first = CE.FirstParameter();
  double            last  = CE.LastParameter();
  double            par   = 0.691254 * first + 0.308746 * last;

  gp_Pnt             pt, pt1, pt2;
  gp_Vec             tgE, tgE1, tgE2, ns1, ns2, dint1, dint2;
  const TopoDS_Face& F1 = S1.Face();
  const TopoDS_Face& F2 = S2.Face();
  // F1.Orientation(TopAbs_FORWARD);
  // F2.Orientation(TopAbs_FORWARD);

  CE.D1(par, pt, tgE);
  tgE.Normalize();
  tgE2 = tgE1 = tgE;
  if (E.Orientation() == TopAbs_REVERSED)
  {
    tgE.Reverse();
  }

  TopoDS_Edge E1 = E, E2 = E;
  E1.Orientation(TopAbs_FORWARD);
  E2.Orientation(TopAbs_FORWARD);

  if (F1.IsSame(F2) && BRep_Tool::IsClosed(E, F1))
  {
    E2.Orientation(TopAbs_REVERSED);
    tgE2.Reverse();
  }
  else
  {
    TopExp_Explorer Exp;
    bool            found = false;
    for (Exp.Init(F1, TopAbs_EDGE); Exp.More() && !found; Exp.Next())
    {
      if (E.IsSame(TopoDS::Edge(Exp.Current())))
      {
        if (Exp.Current().Orientation() == TopAbs_REVERSED)
        {
          tgE1.Reverse();
        }
        found = true;
      }
    }
    if (!found)
    {
      return 0;
    }
    found = false;
    for (Exp.Init(F2, TopAbs_EDGE); Exp.More() && !found; Exp.Next())
    {
      if (E.IsSame(TopoDS::Edge(Exp.Current())))
      {
        if (Exp.Current().Orientation() == TopAbs_REVERSED)
        {
          tgE2.Reverse();
        }
        found = true;
      }
    }
    if (!found)
    {
      return 0;
    }
  }
  BRepAdaptor_Curve2d pc1(E1, F1);
  BRepAdaptor_Curve2d pc2(E2, F2);
  gp_Pnt2d            p2d1, p2d2;
  gp_Vec              DU1, DV1, DU2, DV2;
  p2d1 = pc1.Value(par);
  p2d2 = pc2.Value(par);
  S1.D1(p2d1.X(), p2d1.Y(), pt1, DU1, DV1);
  ns1 = DU1.Crossed(DV1);
  ns1.Normalize();
  if (F1.Orientation() == TopAbs_REVERSED)
  {
    ns1.Reverse();
  }
  S2.D1(p2d2.X(), p2d2.Y(), pt2, DU2, DV2);
  ns2 = DU2.Crossed(DV2);
  ns2.Normalize();
  if (F2.Orientation() == TopAbs_REVERSED)
  {
    ns2.Reverse();
  }

  dint1      = ns1.Crossed(tgE1);
  dint2      = ns2.Crossed(tgE2);
  double ang = ns1.CrossMagnitude(ns2);
  if (ang > 0.0001 * M_PI)
  {
    double scal = ns2.Dot(dint1);
    if (scal <= 0.)
    {
      ns2.Reverse();
      Or2 = TopAbs_REVERSED;
    }
    scal = ns1.Dot(dint2);
    if (scal <= 0.)
    {
      ns1.Reverse();
      Or1 = TopAbs_REVERSED;
    }
  }
  else
  {
    // the faces are locally tangent - this is fake!
    if (dint1.Dot(dint2) < 0.)
    {
      // This is a forgotten regularity
      gp_Vec DDU, DDV, DDUV;
      S1.D2(p2d1.X(), p2d1.Y(), pt1, DU1, DV1, DDU, DDV, DDUV);
      DU1 += (DU1 * dint1 < 0) ? -DDU : DDU;
      DV1 += (DV1 * dint1 < 0) ? -DDV : DDV;
      ns1 = DU1.Crossed(DV1);
      ns1.Normalize();
      if (F1.Orientation() == TopAbs_REVERSED)
      {
        ns1.Reverse();
      }
      S2.D2(p2d2.X(), p2d2.Y(), pt2, DU2, DV2, DDU, DDV, DDUV);
      DU2 += (DU2 * dint2 < 0) ? -DDU : DDU;
      DV2 += (DV2 * dint2 < 0) ? -DDV : DDV;
      ns2 = DU2.Crossed(DV2);
      ns2.Normalize();
      if (F2.Orientation() == TopAbs_REVERSED)
      {
        ns2.Reverse();
      }

      dint1 = ns1.Crossed(tgE1);
      dint2 = ns2.Crossed(tgE2);
      ang   = ns1.CrossMagnitude(ns2);
      if (ang > 0.0001 * M_PI)
      {
        double scal = ns2.Dot(dint1);
        if (scal <= 0.)
        {
          ns2.Reverse();
          Or2 = TopAbs_REVERSED;
        }
        scal = ns1.Dot(dint2);
        if (scal <= 0.)
        {
          ns1.Reverse();
          Or1 = TopAbs_REVERSED;
        }
      }
      else
      {
#ifdef OCCT_DEBUG
        std::cout << "ConcaveSide : no concave face" << std::endl;
#endif
        // This 10 shows that the face at end is in the extension of one of two base faces
        return 10;
      }
    }
    else
    {
      // here it turns back, the points are taken in faces
      // neither too close nor too far as much as possible.
      double u, v;
#ifdef OCCT_DEBUG
//      double deport = 1000*BRep_Tool::Tolerance(E);
#endif
      ChFi3d_Coefficient(dint1, DU1, DV1, u, v);
      p2d1.SetX(p2d1.X() + u);
      p2d1.SetY(p2d1.Y() + v);
      ChFi3d_Coefficient(dint1, DU2, DV2, u, v);
      p2d2.SetX(p2d2.X() + u);
      p2d2.SetY(p2d2.Y() + v);
      S1.D1(p2d1.X(), p2d1.Y(), pt1, DU1, DV1);
      ns1 = DU1.Crossed(DV1);
      if (F1.Orientation() == TopAbs_REVERSED)
      {
        ns1.Reverse();
      }
      S2.D1(p2d2.X(), p2d2.Y(), pt2, DU2, DV2);
      ns2 = DU2.Crossed(DV2);
      if (F2.Orientation() == TopAbs_REVERSED)
      {
        ns2.Reverse();
      }
      gp_Vec vref(pt1, pt2);
      if (ns1.Dot(vref) < 0.)
      {
        Or1 = TopAbs_REVERSED;
      }
      if (ns2.Dot(vref) > 0.)
      {
        Or2 = TopAbs_REVERSED;
      }
    }
  }

  if (Or1 == TopAbs_FORWARD)
  {
    if (Or2 == TopAbs_FORWARD)
    {
      ChoixConge = 1;
    }
    else
    {
      ChoixConge = 7;
    }
  }
  else
  {
    if (Or2 == TopAbs_FORWARD)
    {
      ChoixConge = 3;
    }
    else
    {
      ChoixConge = 5;
    }
  }
  if ((ns1.Crossed(ns2)).Dot(tgE) >= 0.)
  {
    ChoixConge++;
  }
  return ChoixConge;
}

//=================================================================================================

int ChFi3d::NextSide(TopAbs_Orientation&      Or1,
                     TopAbs_Orientation&      Or2,
                     const TopAbs_Orientation OrSave1,
                     const TopAbs_Orientation OrSave2,
                     const int                ChoixSave)
{
  if (Or1 == TopAbs_FORWARD)
  {
    Or1 = OrSave1;
  }
  else
  {
    Or1 = TopAbs::Reverse(OrSave1);
  }
  if (Or2 == TopAbs_FORWARD)
  {
    Or2 = OrSave2;
  }
  else
  {
    Or2 = TopAbs::Reverse(OrSave2);
  }

  int ChoixConge;
  if (Or1 == TopAbs_FORWARD)
  {
    if (Or2 == TopAbs_FORWARD)
    {
      ChoixConge = 1;
    }
    else
    {
      if (ChoixSave < 0)
      {
        ChoixConge = 3;
      }
      else
      {
        ChoixConge = 7;
      }
    }
  }
  else
  {
    if (Or2 == TopAbs_FORWARD)
    {
      if (ChoixSave < 0)
      {
        ChoixConge = 7;
      }
      else
      {
        ChoixConge = 3;
      }
    }
    else
    {
      ChoixConge = 5;
    }
  }
  if (std::abs(ChoixSave) % 2 == 0)
  {
    ChoixConge++;
  }
  return ChoixConge;
}

//=================================================================================================

void ChFi3d::NextSide(TopAbs_Orientation&      Or,
                      const TopAbs_Orientation OrSave,
                      const TopAbs_Orientation OrFace)
{
  if (Or == OrFace)
  {
    Or = OrSave;
  }
  else
  {
    Or = TopAbs::Reverse(OrSave);
  }
}

//=================================================================================================

bool ChFi3d::SameSide(const TopAbs_Orientation Or,
                      const TopAbs_Orientation OrSave1,
                      const TopAbs_Orientation OrSave2,
                      const TopAbs_Orientation OrFace1,
                      const TopAbs_Orientation OrFace2)
{
  TopAbs_Orientation o1, o2;
  if (Or == OrFace1)
  {
    o1 = OrSave1;
  }
  else
  {
    o1 = TopAbs::Reverse(OrSave1);
  }
  if (Or == OrFace2)
  {
    o2 = OrSave2;
  }
  else
  {
    o2 = TopAbs::Reverse(OrSave2);
  }
  return (o1 == o2);
}

//=================================================================================================

void Correct2dPoint(const TopoDS_Face& theF, gp_Pnt2d& theP2d)
{
  BRepAdaptor_Surface aBAS(theF, false);
  if (aBAS.GetType() < GeomAbs_BezierSurface)
  {
    return;
  }
  //
  const double coeff = 0.01;
  double       eps;
  double       u1, u2, v1, v2;
  //
  aBAS.Initialize(theF, true);
  u1 = aBAS.FirstUParameter();
  u2 = aBAS.LastUParameter();
  v1 = aBAS.FirstVParameter();
  v2 = aBAS.LastVParameter();
  if (!(Precision::IsInfinite(u1) || Precision::IsInfinite(u2)))
  {
    eps = std::max(coeff * (u2 - u1), Precision::PConfusion());
    if (std::abs(theP2d.X() - u1) < eps)
    {
      theP2d.SetX(u1 + eps);
    }
    if (std::abs(theP2d.X() - u2) < eps)
    {
      theP2d.SetX(u2 - eps);
    }
  }
  if (!(Precision::IsInfinite(v1) || Precision::IsInfinite(v2)))
  {
    eps = std::max(coeff * (v2 - v1), Precision::PConfusion());
    if (std::abs(theP2d.Y() - v1) < eps)
    {
      theP2d.SetY(v1 + eps);
    }
    if (std::abs(theP2d.Y() - v2) < eps)
    {
      theP2d.SetY(v2 - eps);
    }
  }
}
