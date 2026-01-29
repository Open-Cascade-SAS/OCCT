// Created by: Modelization
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

#include <cstdio>
#include <IntPatch_Intersection.hxx>

#include <Adaptor3d_Surface.hxx>
#include <Adaptor3d_TopolTool.hxx>
#include <IntPatch_ALine.hxx>
#include <IntPatch_ALineToWLine.hxx>
#include <IntPatch_GLine.hxx>
#include <IntPatch_ImpImpIntersection.hxx>
#include <IntPatch_ImpPrmIntersection.hxx>
#include <IntPatch_PrmPrmIntersection.hxx>
#include <IntPatch_WLineTool.hxx>

#include <ProjLib_ProjectOnPlane.hxx>
#include <Geom_Plane.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <ProjLib_ProjectedCurve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <ProjLib.hxx>

//======================================================================
// function: SequenceOfLine
//======================================================================
const NCollection_Sequence<occ::handle<IntPatch_Line>>& IntPatch_Intersection::SequenceOfLine()
  const
{
  return (slin);
}

//======================================================================
// function: IntPatch_Intersection
//======================================================================
IntPatch_Intersection::IntPatch_Intersection()
    : done(false),
      empt(true),
      tgte(false),
      oppo(false),
      myTolArc(0.0),
      myTolTang(0.0),
      myUVMaxStep(0.0),
      myFleche(0.0),
      myIsStartPnt(false),
      myU1Start(0.0),
      myV1Start(0.0),
      myU2Start(0.0),
      myV2Start(0.0)
{
}

//======================================================================
// function: IntPatch_Intersection
//======================================================================
IntPatch_Intersection::IntPatch_Intersection(const occ::handle<Adaptor3d_Surface>&   S1,
                                             const occ::handle<Adaptor3d_TopolTool>& D1,
                                             const occ::handle<Adaptor3d_Surface>&   S2,
                                             const occ::handle<Adaptor3d_TopolTool>& D2,
                                             const double                            TolArc,
                                             const double                            TolTang)
    : done(false),
      empt(true),
      tgte(false),
      oppo(false),
      myTolArc(TolArc),
      myTolTang(TolTang),
      myUVMaxStep(0.0),
      myFleche(0.0),
      myIsStartPnt(false),
      myU1Start(0.0),
      myV1Start(0.0),
      myU2Start(0.0),
      myV2Start(0.0)
{
  if (myTolArc < 1e-8)
    myTolArc = 1e-8;
  if (myTolTang < 1e-8)
    myTolTang = 1e-8;
  if (myTolArc > 0.5)
    myTolArc = 0.5;
  if (myTolTang > 0.5)
    myTolTang = 0.5;
  Perform(S1, D1, S2, D2, TolArc, TolTang);
}

//======================================================================
// function: IntPatch_Intersection
//======================================================================
IntPatch_Intersection::IntPatch_Intersection(const occ::handle<Adaptor3d_Surface>&   S1,
                                             const occ::handle<Adaptor3d_TopolTool>& D1,
                                             const double                            TolArc,
                                             const double                            TolTang)
    : done(false),
      empt(true),
      tgte(false),
      oppo(false),
      myTolArc(TolArc),
      myTolTang(TolTang),
      myUVMaxStep(0.0),
      myFleche(0.0),
      myIsStartPnt(false),
      myU1Start(0.0),
      myV1Start(0.0),
      myU2Start(0.0),
      myV2Start(0.0)
{
  Perform(S1, D1, TolArc, TolTang);
}

//======================================================================
// function: SetTolerances
//======================================================================
void IntPatch_Intersection::SetTolerances(const double TolArc,
                                          const double TolTang,
                                          const double UVMaxStep,
                                          const double Fleche)
{
  myTolArc    = TolArc;
  myTolTang   = TolTang;
  myUVMaxStep = UVMaxStep;
  myFleche    = Fleche;
  if (myTolArc < 1e-8)
    myTolArc = 1e-8;
  if (myTolTang < 1e-8)
    myTolTang = 1e-8;
  if (myTolArc > 0.5)
    myTolArc = 0.5;
  if (myTolTang > 0.5)
    myTolTang = 0.5;
  if (myFleche < 1.0e-3)
    myFleche = 1e-3;
  // if(myUVMaxStep<1.0e-3) myUVMaxStep=1e-3;
  if (myFleche > 10)
    myFleche = 10;
  if (myUVMaxStep > 0.5)
    myUVMaxStep = 0.5;
}

//======================================================================
// function: Perform
//======================================================================
void IntPatch_Intersection::Perform(const occ::handle<Adaptor3d_Surface>&   S1,
                                    const occ::handle<Adaptor3d_TopolTool>& D1,
                                    const double                            TolArc,
                                    const double                            TolTang)
{
  myTolArc  = TolArc;
  myTolTang = TolTang;
  if (myFleche == 0.0)
    myFleche = 0.01;
  if (myUVMaxStep == 0.0)
    myUVMaxStep = 0.01;

  done = true;
  spnt.Clear();
  slin.Clear();

  empt = true;
  tgte = false;
  oppo = false;

  switch (S1->GetType())
  {
    case GeomAbs_Plane:
    case GeomAbs_Cylinder:
    case GeomAbs_Sphere:
    case GeomAbs_Cone:
    case GeomAbs_Torus:
      break;
    case GeomAbs_SurfaceOfExtrusion: {
      gp_Dir                       aDirection = S1->Direction();
      gp_Ax3                       anAxis(gp::Origin(), aDirection);
      occ::handle<Adaptor3d_Curve> aBasisCurve = S1->BasisCurve();
      ProjLib_ProjectOnPlane       Projector(anAxis);
      Projector.Load(aBasisCurve, Precision::Confusion());
      occ::handle<GeomAdaptor_Curve>   aProjCurve = Projector.GetResult();
      occ::handle<Geom_Plane>          aPlane     = new Geom_Plane(anAxis);
      occ::handle<GeomAdaptor_Surface> aGAHsurf   = new GeomAdaptor_Surface(aPlane);
      ProjLib_ProjectedCurve           aProjectedCurve(aGAHsurf, aProjCurve);
      occ::handle<Geom2d_Curve>        aPCurve;
      ProjLib::MakePCurveOfType(aProjectedCurve, aPCurve);
      Geom2dAdaptor_Curve AC(aPCurve,
                             aProjectedCurve.FirstParameter(),
                             aProjectedCurve.LastParameter());
      Geom2dInt_GInter    Intersector(AC, Precision::Confusion(), Precision::Confusion());
      if (Intersector.IsDone()
          && (Intersector.IsEmpty() || (AC.IsClosed() && Intersector.NbPoints() == 1)))
      {
        break;
      }
    }
      [[fallthrough]];
    default: {
      IntPatch_PrmPrmIntersection interpp;
      interpp.Perform(S1, D1, TolTang, TolArc, myFleche, myUVMaxStep);
      if (interpp.IsDone())
      {
        done           = true;
        tgte           = false;
        empt           = interpp.IsEmpty();
        const int nblm = interpp.NbLines();
        for (int i = 1; i <= nblm; i++)
          slin.Append(interpp.Line(i));
      }
    }
    break;
  }
}

/////////////////////////////////////////////////////////////////////////////
//  These several support functions provide methods which can help basic   //
//  algorithm to intersect infinite surfaces of the following types:       //
//                                                                         //
//  a.) SurfaceOfExtrusion;                                                //
//  b.) SurfaceOfRevolution;                                               //
//  c.) OffsetSurface.                                                     //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
#include <gp_XYZ.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Sequence.hxx>
#include <Extrema_ExtPS.hxx>
#include <Extrema_POnSurf.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dAPI_InterCurveCurve.hxx>
#include <GeomProjLib.hxx>
#include <ElCLib.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>

//===============================================================
// function: FUN_GetMinMaxXYZPnt
//===============================================================
static void FUN_GetMinMaxXYZPnt(const occ::handle<Adaptor3d_Surface>& S, gp_Pnt& pMin, gp_Pnt& pMax)
{
  const double DU      = 0.25 * std::abs(S->LastUParameter() - S->FirstUParameter());
  const double DV      = 0.25 * std::abs(S->LastVParameter() - S->FirstVParameter());
  double       tMinXYZ = RealLast();
  double       tMaxXYZ = -tMinXYZ;
  gp_Pnt       PUV, ptMax, ptMin;
  for (double U = S->FirstUParameter(); U <= S->LastUParameter(); U += DU)
  {
    for (double V = S->FirstVParameter(); V <= S->LastVParameter(); V += DV)
    {
      S->D0(U, V, PUV);
      const double cXYZ = PUV.XYZ().Modulus();
      if (cXYZ > tMaxXYZ)
      {
        tMaxXYZ = cXYZ;
        ptMax   = PUV;
      }
      if (cXYZ < tMinXYZ)
      {
        tMinXYZ = cXYZ;
        ptMin   = PUV;
      }
    }
  }
  pMin = ptMin;
  pMax = ptMax;
}

//==========================================================================
// function: FUN_TrimInfSurf
//==========================================================================
static void FUN_TrimInfSurf(const gp_Pnt&                         Pmin,
                            const gp_Pnt&                         Pmax,
                            const occ::handle<Adaptor3d_Surface>& InfSurf,
                            const double&                         AlternativeTrimPrm,
                            occ::handle<Adaptor3d_Surface>&       TrimS)
{
  double        TP = AlternativeTrimPrm;
  Extrema_ExtPS anExtPS;
  anExtPS.Initialize(*InfSurf,
                     InfSurf->FirstUParameter(),
                     InfSurf->LastUParameter(),
                     InfSurf->FirstVParameter(),
                     InfSurf->LastVParameter(),
                     1.e-7,
                     1.e-7);

  // Process Pmin
  anExtPS.Perform(Pmin);
  const bool isDone1 = anExtPS.IsDone();
  double     Umax = -1.e+100, Umin = 1.e+100, Vmax = -1.e+100, Vmin = 1.e+100, cU, cV;
  if (isDone1)
  {
    for (int i = 1; i <= anExtPS.NbExt(); i++)
    {
      const Extrema_POnSurf& pons = anExtPS.Point(i);
      pons.Parameter(cU, cV);
      if (cU > Umax)
        Umax = cU;
      if (cU < Umin)
        Umin = cU;
      if (cV > Vmax)
        Vmax = cV;
      if (cV < Vmin)
        Vmin = cV;
    }
  }

  // Process Pmax
  anExtPS.Perform(Pmax);
  const bool isDone2 = anExtPS.IsDone();
  if (isDone2)
  {
    for (int i = 1; i <= anExtPS.NbExt(); i++)
    {
      const Extrema_POnSurf& pons = anExtPS.Point(i);
      pons.Parameter(cU, cV);
      if (cU > Umax)
        Umax = cU;
      if (cU < Umin)
        Umin = cU;
      if (cV > Vmax)
        Vmax = cV;
      if (cV < Vmin)
        Vmin = cV;
    }
  }

  if (isDone1 || isDone2)
  {
    TP =
      std::max(std::abs(Umin), std::max(std::abs(Umax), std::max(std::abs(Vmin), std::abs(Vmax))));
  }
  if (TP == 0.)
  {
    TrimS = InfSurf;
    return;
  }
  else
  {
    const bool                     Uinf = Precision::IsNegativeInfinite(InfSurf->FirstUParameter());
    const bool                     Usup = Precision::IsPositiveInfinite(InfSurf->LastUParameter());
    const bool                     Vinf = Precision::IsNegativeInfinite(InfSurf->FirstVParameter());
    const bool                     Vsup = Precision::IsPositiveInfinite(InfSurf->LastVParameter());
    occ::handle<Adaptor3d_Surface> TmpSS;
    int                            IsTrimed = 0;
    const double                   tp       = 1000.0 * TP;
    if (Vinf && Vsup)
    {
      TrimS    = InfSurf->VTrim(-tp, tp, 1.0e-7);
      IsTrimed = 1;
    }
    if (Vinf && !Vsup)
    {
      TrimS    = InfSurf->VTrim(-tp, InfSurf->LastVParameter(), 1.0e-7);
      IsTrimed = 1;
    }
    if (!Vinf && Vsup)
    {
      TrimS    = InfSurf->VTrim(InfSurf->FirstVParameter(), tp, 1.0e-7);
      IsTrimed = 1;
    }
    if (IsTrimed)
    {
      TmpSS = TrimS;
      if (Uinf && Usup)
        TrimS = TmpSS->UTrim(-tp, tp, 1.0e-7);
      if (Uinf && !Usup)
        TrimS = TmpSS->UTrim(-tp, InfSurf->LastUParameter(), 1.0e-7);
      if (!Uinf && Usup)
        TrimS = TmpSS->UTrim(InfSurf->FirstUParameter(), tp, 1.0e-7);
    }
    else
    {
      if (Uinf && Usup)
        TrimS = InfSurf->UTrim(-tp, tp, 1.0e-7);
      if (Uinf && !Usup)
        TrimS = InfSurf->UTrim(-tp, InfSurf->LastUParameter(), 1.0e-7);
      if (!Uinf && Usup)
        TrimS = InfSurf->UTrim(InfSurf->FirstUParameter(), tp, 1.0e-7);
    }
  }
}

//================================================================================
// function: FUN_GetUiso
//================================================================================
static void FUN_GetUiso(const occ::handle<Geom_Surface>& GS,
                        const GeomAbs_SurfaceType&       T,
                        const double&                    FirstV,
                        const double&                    LastV,
                        const bool&                      IsVC,
                        const bool&                      IsVP,
                        const double&                    U,
                        occ::handle<Geom_Curve>&         I)
{
  if (T != GeomAbs_OffsetSurface)
  {
    occ::handle<Geom_Curve> gc = GS->UIso(U);
    if (IsVP && (FirstV == 0.0 && LastV == (2. * M_PI)))
      I = gc;
    else
    {
      occ::handle<Geom_TrimmedCurve> gtc = new Geom_TrimmedCurve(gc, FirstV, LastV);
      // szv:I = occ::down_cast<Geom_Curve>(gtc);
      I = gtc;
    }
  }
  else // OffsetSurface
  {
    const occ::handle<Geom_OffsetSurface> gos  = occ::down_cast<Geom_OffsetSurface>(GS);
    const occ::handle<Geom_Surface>       bs   = gos->BasisSurface();
    occ::handle<Geom_Curve>               gcbs = bs->UIso(U);
    GeomAdaptor_Curve                     gac(gcbs);
    const GeomAbs_CurveType               GACT = gac.GetType();
    if (IsVP || IsVC || GACT == GeomAbs_BSplineCurve || GACT == GeomAbs_BezierCurve
        || std::abs(LastV - FirstV) < 1.e+5)
    {
      occ::handle<Geom_Curve> gc = gos->UIso(U);
      if (IsVP && (FirstV == 0.0 && LastV == (2 * M_PI)))
        I = gc;
      else
      {
        occ::handle<Geom_TrimmedCurve> gtc = new Geom_TrimmedCurve(gc, FirstV, LastV);
        // szv:I = occ::down_cast<Geom_Curve>(gtc);
        I = gtc;
      }
    }
    else // Offset Line, Parab, Hyperb
    {
      double VmTr, VMTr;
      if (GACT != GeomAbs_Hyperbola)
      {
        if (FirstV >= 0. && LastV >= 0.)
        {
          VmTr = FirstV;
          VMTr = ((LastV - FirstV) > 1.e+4) ? (FirstV + 1.e+4) : LastV;
        }
        else if (FirstV < 0. && LastV < 0.)
        {
          VMTr = LastV;
          VmTr = ((FirstV - LastV) < -1.e+4) ? (LastV - 1.e+4) : FirstV;
        }
        else
        {
          VmTr = (FirstV < -1.e+4) ? -1.e+4 : FirstV;
          VMTr = (LastV > 1.e+4) ? 1.e+4 : LastV;
        }
      }
      else // Hyperbola
      {
        if (FirstV >= 0. && LastV >= 0.)
        {
          if (FirstV > 4.)
            return;
          VmTr = FirstV;
          VMTr = (LastV > 4.) ? 4. : LastV;
        }
        else if (FirstV < 0. && LastV < 0.)
        {
          if (LastV < -4.)
            return;
          VMTr = LastV;
          VmTr = (FirstV < -4.) ? -4. : FirstV;
        }
        else
        {
          VmTr = (FirstV < -4.) ? -4. : FirstV;
          VMTr = (LastV > 4.) ? 4. : LastV;
        }
      }
      // Make trimmed surface
      occ::handle<Geom_RectangularTrimmedSurface> rts =
        new Geom_RectangularTrimmedSurface(gos, VmTr, VMTr, true);
      I = rts->UIso(U);
    }
  }
}

//================================================================================
// function: FUN_GetViso
//================================================================================
static void FUN_GetViso(const occ::handle<Geom_Surface>& GS,
                        const GeomAbs_SurfaceType&       T,
                        const double&                    FirstU,
                        const double&                    LastU,
                        const bool&                      IsUC,
                        const bool&                      IsUP,
                        const double&                    V,
                        occ::handle<Geom_Curve>&         I)
{
  if (T != GeomAbs_OffsetSurface)
  {
    occ::handle<Geom_Curve> gc = GS->VIso(V);
    if (IsUP && (FirstU == 0.0 && LastU == (2 * M_PI)))
      I = gc;
    else
    {
      occ::handle<Geom_TrimmedCurve> gtc = new Geom_TrimmedCurve(gc, FirstU, LastU);
      // szv:I = occ::down_cast<Geom_Curve>(gtc);
      I = gtc;
    }
  }
  else // OffsetSurface
  {
    const occ::handle<Geom_OffsetSurface> gos  = occ::down_cast<Geom_OffsetSurface>(GS);
    const occ::handle<Geom_Surface>       bs   = gos->BasisSurface();
    occ::handle<Geom_Curve>               gcbs = bs->VIso(V);
    GeomAdaptor_Curve                     gac(gcbs);
    const GeomAbs_CurveType               GACT = gac.GetType();
    if (IsUP || IsUC || GACT == GeomAbs_BSplineCurve || GACT == GeomAbs_BezierCurve
        || std::abs(LastU - FirstU) < 1.e+5)
    {
      occ::handle<Geom_Curve> gc = gos->VIso(V);
      if (IsUP && (FirstU == 0.0 && LastU == (2 * M_PI)))
        I = gc;
      else
      {
        occ::handle<Geom_TrimmedCurve> gtc = new Geom_TrimmedCurve(gc, FirstU, LastU);
        // szv:I = occ::down_cast<Geom_Curve>(gtc);
        I = gtc;
      }
    }
    else // Offset Line, Parab, Hyperb
    {
      double UmTr, UMTr;
      if (GACT != GeomAbs_Hyperbola)
      {
        if (FirstU >= 0. && LastU >= 0.)
        {
          UmTr = FirstU;
          UMTr = ((LastU - FirstU) > 1.e+4) ? (FirstU + 1.e+4) : LastU;
        }
        else if (FirstU < 0. && LastU < 0.)
        {
          UMTr = LastU;
          UmTr = ((FirstU - LastU) < -1.e+4) ? (LastU - 1.e+4) : FirstU;
        }
        else
        {
          UmTr = (FirstU < -1.e+4) ? -1.e+4 : FirstU;
          UMTr = (LastU > 1.e+4) ? 1.e+4 : LastU;
        }
      }
      else // Hyperbola
      {
        if (FirstU >= 0. && LastU >= 0.)
        {
          if (FirstU > 4.)
            return;
          UmTr = FirstU;
          UMTr = (LastU > 4.) ? 4. : LastU;
        }
        else if (FirstU < 0. && LastU < 0.)
        {
          if (LastU < -4.)
            return;
          UMTr = LastU;
          UmTr = (FirstU < -4.) ? -4. : FirstU;
        }
        else
        {
          UmTr = (FirstU < -4.) ? -4. : FirstU;
          UMTr = (LastU > 4.) ? 4. : LastU;
        }
      }
      // Make trimmed surface
      occ::handle<Geom_RectangularTrimmedSurface> rts =
        new Geom_RectangularTrimmedSurface(gos, UmTr, UMTr, true);
      I = rts->VIso(V);
    }
  }
}

//================================================================================
// function: FUN_PL_Intersection
//================================================================================
static void FUN_PL_Intersection(const occ::handle<Adaptor3d_Surface>& S1,
                                const GeomAbs_SurfaceType&            T1,
                                const occ::handle<Adaptor3d_Surface>& S2,
                                const GeomAbs_SurfaceType&            T2,
                                bool&                                 IsOk,
                                NCollection_Sequence<gp_Pnt>&         SP,
                                gp_Vec&                               DV)
{
  IsOk = false;
  // 1. Check: both surfaces have U(V)isos - lines.
  DV                                             = gp_Vec(0., 0., 1.);
  bool                            isoS1isLine[2] = {false, false};
  bool                            isoS2isLine[2] = {false, false};
  occ::handle<Geom_Curve>         C1, C2;
  const GeomAdaptor_Surface&      gas1 = *(GeomAdaptor_Surface*)(S1.get());
  const GeomAdaptor_Surface&      gas2 = *(GeomAdaptor_Surface*)(S2.get());
  const occ::handle<Geom_Surface> gs1  = gas1.Surface();
  const occ::handle<Geom_Surface> gs2  = gas2.Surface();
  double                          MS1[2], MS2[2];
  MS1[0] = 0.5 * (S1->LastUParameter() + S1->FirstUParameter());
  MS1[1] = 0.5 * (S1->LastVParameter() + S1->FirstVParameter());
  MS2[0] = 0.5 * (S2->LastUParameter() + S2->FirstUParameter());
  MS2[1] = 0.5 * (S2->LastVParameter() + S2->FirstVParameter());
  if (T1 == GeomAbs_SurfaceOfExtrusion)
    isoS1isLine[0] = true;
  else if (!S1->IsVPeriodic() && !S1->IsVClosed())
  {
    if (T1 != GeomAbs_OffsetSurface)
      C1 = gs1->UIso(MS1[0]);
    else
    {
      const occ::handle<Geom_OffsetSurface> gos = occ::down_cast<Geom_OffsetSurface>(gs1);
      const occ::handle<Geom_Surface>       bs  = gos->BasisSurface();
      C1                                        = bs->UIso(MS1[0]);
    }
    GeomAdaptor_Curve gac(C1);
    if (gac.GetType() == GeomAbs_Line)
      isoS1isLine[0] = true;
  }
  if (!S1->IsUPeriodic() && !S1->IsUClosed())
  {
    if (T1 != GeomAbs_OffsetSurface)
      C1 = gs1->VIso(MS1[1]);
    else
    {
      const occ::handle<Geom_OffsetSurface> gos = occ::down_cast<Geom_OffsetSurface>(gs1);
      const occ::handle<Geom_Surface>       bs  = gos->BasisSurface();
      C1                                        = bs->VIso(MS1[1]);
    }
    GeomAdaptor_Curve gac(C1);
    if (gac.GetType() == GeomAbs_Line)
      isoS1isLine[1] = true;
  }
  if (T2 == GeomAbs_SurfaceOfExtrusion)
    isoS2isLine[0] = true;
  else if (!S2->IsVPeriodic() && !S2->IsVClosed())
  {
    if (T2 != GeomAbs_OffsetSurface)
      C2 = gs2->UIso(MS2[0]);
    else
    {
      const occ::handle<Geom_OffsetSurface> gos = occ::down_cast<Geom_OffsetSurface>(gs2);
      const occ::handle<Geom_Surface>       bs  = gos->BasisSurface();
      C2                                        = bs->UIso(MS2[0]);
    }
    GeomAdaptor_Curve gac(C2);
    if (gac.GetType() == GeomAbs_Line)
      isoS2isLine[0] = true;
  }
  if (!S2->IsUPeriodic() && !S2->IsUClosed())
  {
    if (T2 != GeomAbs_OffsetSurface)
      C2 = gs2->VIso(MS2[1]);
    else
    {
      const occ::handle<Geom_OffsetSurface> gos = occ::down_cast<Geom_OffsetSurface>(gs2);
      const occ::handle<Geom_Surface>       bs  = gos->BasisSurface();
      C2                                        = bs->VIso(MS2[1]);
    }
    GeomAdaptor_Curve gac(C2);
    if (gac.GetType() == GeomAbs_Line)
      isoS2isLine[1] = true;
  }
  bool IsBothLines = ((isoS1isLine[0] || isoS1isLine[1]) && (isoS2isLine[0] || isoS2isLine[1]));
  if (!IsBothLines)
  {
    return;
  }
  // 2. Check: Uiso lines of both surfaces are collinear.
  gp_Pnt puvS1, puvS2;
  gp_Vec derS1[2], derS2[2];
  S1->D1(MS1[0], MS1[1], puvS1, derS1[0], derS1[1]);
  S2->D1(MS2[0], MS2[1], puvS2, derS2[0], derS2[1]);
  C1.Nullify();
  C2.Nullify();
  int iso = 0;
  if (isoS1isLine[0] && isoS2isLine[0] && derS1[1].IsParallel(derS2[1], Precision::Angular()))
  {
    iso = 1;
    FUN_GetViso(gs1,
                T1,
                S1->FirstUParameter(),
                S1->LastUParameter(),
                S1->IsUClosed(),
                S1->IsUPeriodic(),
                MS1[1],
                C1);
    FUN_GetViso(gs2,
                T2,
                S2->FirstUParameter(),
                S2->LastUParameter(),
                S2->IsUClosed(),
                S2->IsUPeriodic(),
                MS2[1],
                C2);
  }
  else if (isoS1isLine[0] && isoS2isLine[1] && derS1[1].IsParallel(derS2[0], Precision::Angular()))
  {
    iso = 1;
    FUN_GetViso(gs1,
                T1,
                S1->FirstUParameter(),
                S1->LastUParameter(),
                S1->IsUClosed(),
                S1->IsUPeriodic(),
                MS1[1],
                C1);
    FUN_GetUiso(gs2,
                T2,
                S2->FirstVParameter(),
                S2->LastVParameter(),
                S2->IsVClosed(),
                S2->IsVPeriodic(),
                MS2[0],
                C2);
  }
  else if (isoS1isLine[1] && isoS2isLine[0] && derS1[0].IsParallel(derS2[1], Precision::Angular()))
  {
    iso = 0;
    FUN_GetUiso(gs1,
                T1,
                S1->FirstVParameter(),
                S1->LastVParameter(),
                S1->IsVClosed(),
                S1->IsVPeriodic(),
                MS1[0],
                C1);
    FUN_GetViso(gs2,
                T2,
                S2->FirstUParameter(),
                S2->LastUParameter(),
                S2->IsUClosed(),
                S2->IsUPeriodic(),
                MS2[1],
                C2);
  }
  else if (isoS1isLine[1] && isoS2isLine[1] && derS1[0].IsParallel(derS2[0], Precision::Angular()))
  {
    iso = 0;
    FUN_GetUiso(gs1,
                T1,
                S1->FirstVParameter(),
                S1->LastVParameter(),
                S1->IsVClosed(),
                S1->IsVPeriodic(),
                MS1[0],
                C1);
    FUN_GetUiso(gs2,
                T2,
                S2->FirstVParameter(),
                S2->LastVParameter(),
                S2->IsVClosed(),
                S2->IsVPeriodic(),
                MS2[0],
                C2);
  }
  else
  {
    IsOk = false;
    return;
  }
  IsOk = true;
  // 3. Make intersections of V(U)isos
  if (C1.IsNull() || C2.IsNull())
    return;
  DV                            = derS1[iso];
  occ::handle<Geom_Plane> GPln  = new Geom_Plane(gp_Pln(puvS1, gp_Dir(DV)));
  occ::handle<Geom_Curve> C1Prj = GeomProjLib::ProjectOnPlane(C1, GPln, gp_Dir(DV), true);
  occ::handle<Geom_Curve> C2Prj = GeomProjLib::ProjectOnPlane(C2, GPln, gp_Dir(DV), true);
  if (C1Prj.IsNull() || C2Prj.IsNull())
    return;
  occ::handle<Geom2d_Curve> C1Prj2d = GeomProjLib::Curve2d(C1Prj, GPln);
  occ::handle<Geom2d_Curve> C2Prj2d = GeomProjLib::Curve2d(C2Prj, GPln);
  Geom2dAPI_InterCurveCurve ICC(C1Prj2d, C2Prj2d, 1.0e-7);
  if (ICC.NbPoints() > 0)
  {
    for (int ip = 1; ip <= ICC.NbPoints(); ip++)
    {
      gp_Pnt2d P   = ICC.Point(ip);
      gp_Pnt   P3d = ElCLib::To3d(gp_Ax2(puvS1, gp_Dir(DV)), P);
      SP.Append(P3d);
    }
  }
}

//================================================================================
// function: FUN_NewFirstLast
//================================================================================
static void FUN_NewFirstLast(const GeomAbs_CurveType& ga_ct,
                             const double&            Fst,
                             const double&            Lst,
                             const double&            TrVal,
                             double&                  NewFst,
                             double&                  NewLst,
                             bool&                    NeedTr)
{
  NewFst = Fst;
  NewLst = Lst;
  NeedTr = false;
  switch (ga_ct)
  {
    case GeomAbs_Line:
    case GeomAbs_Parabola: {
      if (std::abs(Lst - Fst) > TrVal)
      {
        if (Fst >= 0. && Lst >= 0.)
        {
          NewFst = Fst;
          NewLst = ((Fst + TrVal) < Lst) ? (Fst + TrVal) : Lst;
        }
        if (Fst < 0. && Lst < 0.)
        {
          NewLst = Lst;
          NewFst = ((Lst - TrVal) > Fst) ? (Lst - TrVal) : Fst;
        }
        else
        {
          NewFst = (Fst < -TrVal) ? -TrVal : Fst;
          NewLst = (Lst > TrVal) ? TrVal : Lst;
        }
        NeedTr = true;
      }
      break;
    }
    case GeomAbs_Hyperbola: {
      if (std::abs(Lst - Fst) > 10.)
      {
        if (Fst >= 0. && Lst >= 0.)
        {
          if (Fst > 4.)
            return;
          NewFst = Fst;
          NewLst = (Lst > 4.) ? 4. : Lst;
        }
        if (Fst < 0. && Lst < 0.)
        {
          if (Lst < -4.)
            return;
          NewLst = Lst;
          NewFst = (Fst < -4.) ? -4. : Fst;
        }
        else
        {
          NewFst = (Fst < -4.) ? -4. : Fst;
          NewLst = (Lst > 4.) ? 4. : Lst;
        }
        NeedTr = true;
      }
      break;
    }
    default:
      break;
  }
}

//================================================================================
// function: FUN_TrimBothSurf
//================================================================================
static void FUN_TrimBothSurf(const occ::handle<Adaptor3d_Surface>& S1,
                             const GeomAbs_SurfaceType&            T1,
                             const occ::handle<Adaptor3d_Surface>& S2,
                             const GeomAbs_SurfaceType&            T2,
                             const double&                         TV,
                             occ::handle<Adaptor3d_Surface>&       NS1,
                             occ::handle<Adaptor3d_Surface>&       NS2)
{
  const GeomAdaptor_Surface&      gas1 = *(GeomAdaptor_Surface*)(S1.get());
  const GeomAdaptor_Surface&      gas2 = *(GeomAdaptor_Surface*)(S2.get());
  const occ::handle<Geom_Surface> gs1  = gas1.Surface();
  const occ::handle<Geom_Surface> gs2  = gas2.Surface();
  const double                    UM1  = 0.5 * (S1->LastUParameter() + S1->FirstUParameter());
  const double                    UM2  = 0.5 * (S2->LastUParameter() + S2->FirstUParameter());
  const double                    VM1  = 0.5 * (S1->LastVParameter() + S1->FirstVParameter());
  const double                    VM2  = 0.5 * (S2->LastVParameter() + S2->FirstVParameter());
  occ::handle<Geom_Curve>         visoS1, visoS2, uisoS1, uisoS2;
  if (T1 != GeomAbs_OffsetSurface)
  {
    visoS1 = gs1->VIso(VM1);
    uisoS1 = gs1->UIso(UM1);
  }
  else
  {
    const occ::handle<Geom_OffsetSurface> gos = occ::down_cast<Geom_OffsetSurface>(gs1);
    const occ::handle<Geom_Surface>       bs  = gos->BasisSurface();
    visoS1                                    = bs->VIso(VM1);
    uisoS1                                    = bs->UIso(UM1);
  }
  if (T2 != GeomAbs_OffsetSurface)
  {
    visoS2 = gs2->VIso(VM2);
    uisoS2 = gs2->UIso(UM2);
  }
  else
  {
    const occ::handle<Geom_OffsetSurface> gos = occ::down_cast<Geom_OffsetSurface>(gs2);
    const occ::handle<Geom_Surface>       bs  = gos->BasisSurface();
    visoS2                                    = bs->VIso(VM2);
    uisoS2                                    = bs->UIso(UM2);
  }
  if (uisoS1.IsNull() || uisoS2.IsNull() || visoS1.IsNull() || visoS2.IsNull())
  {
    NS1 = S1;
    NS2 = S2;
    return;
  }
  GeomAdaptor_Curve gau1(uisoS1);
  GeomAdaptor_Curve gav1(visoS1);
  GeomAdaptor_Curve gau2(uisoS2);
  GeomAdaptor_Curve gav2(visoS2);
  GeomAbs_CurveType GA_U1 = gau1.GetType();
  GeomAbs_CurveType GA_V1 = gav1.GetType();
  GeomAbs_CurveType GA_U2 = gau2.GetType();
  GeomAbs_CurveType GA_V2 = gav2.GetType();
  bool              TrmU1 = false;
  bool              TrmV1 = false;
  bool              TrmU2 = false;
  bool              TrmV2 = false;
  double            V1S1, V2S1, U1S1, U2S1, V1S2, V2S2, U1S2, U2S2;
  FUN_NewFirstLast(GA_U1, S1->FirstVParameter(), S1->LastVParameter(), TV, V1S1, V2S1, TrmV1);
  FUN_NewFirstLast(GA_V1, S1->FirstUParameter(), S1->LastUParameter(), TV, U1S1, U2S1, TrmU1);
  FUN_NewFirstLast(GA_U2, S2->FirstVParameter(), S2->LastVParameter(), TV, V1S2, V2S2, TrmV2);
  FUN_NewFirstLast(GA_V2, S2->FirstUParameter(), S2->LastUParameter(), TV, U1S2, U2S2, TrmU2);
  if (TrmV1)
    NS1 = S1->VTrim(V1S1, V2S1, 1.0e-7);
  if (TrmV2)
    NS2 = S2->VTrim(V1S2, V2S2, 1.0e-7);
  if (TrmU1)
  {
    if (TrmV1)
    {
      occ::handle<Adaptor3d_Surface> TS = NS1;
      NS1                               = TS->UTrim(U1S1, U2S1, 1.0e-7);
    }
    else
      NS1 = S1->UTrim(U1S1, U2S1, 1.0e-7);
  }
  if (TrmU2)
  {
    if (TrmV2)
    {
      occ::handle<Adaptor3d_Surface> TS = NS2;
      NS2                               = TS->UTrim(U1S2, U2S2, 1.0e-7);
    }
    else
      NS2 = S2->UTrim(U1S2, U2S2, 1.0e-7);
  }
}

//=================================================================================================

void IntPatch_Intersection::Perform(const occ::handle<Adaptor3d_Surface>&   theS1,
                                    const occ::handle<Adaptor3d_TopolTool>& theD1,
                                    const occ::handle<Adaptor3d_Surface>&   theS2,
                                    const occ::handle<Adaptor3d_TopolTool>& theD2,
                                    const double                            TolArc,
                                    const double                            TolTang,
                                    const bool                              isGeomInt,
                                    const bool                              theIsReqToKeepRLine,
                                    const bool                              theIsReqToPostWLProc)
{
  myTolArc  = TolArc;
  myTolTang = TolTang;
  if (myFleche <= Precision::PConfusion())
    myFleche = 0.01;
  if (myUVMaxStep <= Precision::PConfusion())
    myUVMaxStep = 0.01;

  done = false;
  spnt.Clear();
  slin.Clear();
  empt = true;
  tgte = false;
  oppo = false;

  GeomAbs_SurfaceType typs1 = theS1->GetType();
  GeomAbs_SurfaceType typs2 = theS2->GetType();

  // treatment of the cases with cone or torus
  bool TreatAsBiParametric = false;
  int  bGeomGeom           = 0;
  //
  if (typs1 == GeomAbs_Cone || typs2 == GeomAbs_Cone || typs1 == GeomAbs_Torus
      || typs2 == GeomAbs_Torus)
  {
    gp_Ax1              aCTAx, aGeomAx;
    GeomAbs_SurfaceType aCTType;
    bool                bToCheck;
    //
    const occ::handle<Adaptor3d_Surface>& aCTSurf =
      (typs1 == GeomAbs_Cone || typs1 == GeomAbs_Torus) ? theS1 : theS2;
    const occ::handle<Adaptor3d_Surface>& aGeomSurf =
      (typs1 == GeomAbs_Cone || typs1 == GeomAbs_Torus) ? theS2 : theS1;
    //
    aCTType  = aCTSurf->GetType();
    bToCheck = false;
    //
    if (typs1 == GeomAbs_Cone || typs2 == GeomAbs_Cone)
    {
      const gp_Cone aCon1 = (aCTType == GeomAbs_Cone) ? aCTSurf->Cone() : aGeomSurf->Cone();
      double        a1    = std::abs(aCon1.SemiAngle());
      bToCheck            = (a1 < 0.02) || (a1 > 1.55);
      //
      if (typs1 == typs2)
      {
        const gp_Cone aCon2 = aGeomSurf->Cone();
        double        a2    = std::abs(aCon2.SemiAngle());
        bToCheck            = bToCheck || (a2 < 0.02) || (a2 > 1.55);
        //
        if (a1 > 1.55 && a2 > 1.55)
        { // quasi-planes: if same domain, treat as canonic
          const gp_Ax1 A1 = aCon1.Axis(), A2 = aCon2.Axis();
          if (A1.IsParallel(A2, Precision::Angular()))
          {
            const gp_Pnt Apex1 = aCon1.Apex(), Apex2 = aCon2.Apex();
            const gp_Pln Plan1(Apex1, A1.Direction());
            if (Plan1.Distance(Apex2) <= Precision::Confusion())
            {
              bToCheck = false;
            }
          }
        }
      }
      //
      TreatAsBiParametric = bToCheck;
      if (aCTType == GeomAbs_Cone)
      {
        aCTAx = aCon1.Axis();
      }
    }
    //
    if (typs1 == GeomAbs_Torus || typs2 == GeomAbs_Torus)
    {
      const gp_Torus aTor1 = (aCTType == GeomAbs_Torus) ? aCTSurf->Torus() : aGeomSurf->Torus();
      bToCheck             = aTor1.MajorRadius() > aTor1.MinorRadius();
      if (typs1 == typs2)
      {
        const gp_Torus aTor2 = aGeomSurf->Torus();
        bToCheck             = aTor2.MajorRadius() > aTor2.MinorRadius();
      }
      //
      if (aCTType == GeomAbs_Torus)
      {
        aCTAx = aTor1.Axis();
      }
    }
    //
    if (bToCheck)
    {
      const gp_Lin aL1(aCTAx);
      //
      switch (aGeomSurf->GetType())
      {
        case GeomAbs_Plane: {
          aGeomAx = aGeomSurf->Plane().Axis();
          if (aCTType == GeomAbs_Cone)
          {
            bGeomGeom = 1;
            if (std::abs(aCTSurf->Cone().SemiAngle()) < 0.02)
            {
              double ps = std::abs(aCTAx.Direction().Dot(aGeomAx.Direction()));
              if (ps < 0.015)
              {
                bGeomGeom = 0;
              }
            }
          }
          else
          {
            if (aCTAx.IsParallel(aGeomAx, Precision::Angular())
                || (aCTAx.IsNormal(aGeomAx, Precision::Angular())
                    && (aGeomSurf->Plane().Distance(aCTAx.Location()) < Precision::Confusion())))
            {
              bGeomGeom = 1;
            }
          }
          bToCheck = false;
          break;
        }
        case GeomAbs_Sphere: {
          if (aL1.Distance(aGeomSurf->Sphere().Location()) < Precision::Confusion())
          {
            bGeomGeom = 1;
          }
          bToCheck = false;
          break;
        }
        case GeomAbs_Cylinder:
          aGeomAx = aGeomSurf->Cylinder().Axis();
          break;
        case GeomAbs_Cone:
          aGeomAx = aGeomSurf->Cone().Axis();
          break;
        case GeomAbs_Torus:
          aGeomAx = aGeomSurf->Torus().Axis();
          break;
        default:
          bToCheck = false;
          break;
      }
      //
      if (bToCheck)
      {
        if (aCTAx.IsParallel(aGeomAx, Precision::Angular())
            && (aL1.Distance(aGeomAx.Location()) <= Precision::Confusion()))
        {
          bGeomGeom = 1;
        }
      }
      //
      if (bGeomGeom == 1)
      {
        TreatAsBiParametric = false;
      }
    }
  }
  //

  if (theD1->DomainIsInfinite() || theD2->DomainIsInfinite())
  {
    TreatAsBiParametric = false;
  }

  //  Modified by skv - Mon Sep 26 14:58:30 2005 Begin
  //   if(TreatAsBiParametric) { typs1 = typs2 = GeomAbs_BezierSurface; }
  if (TreatAsBiParametric)
  {
    if (typs1 == GeomAbs_Cone && typs2 == GeomAbs_Plane)
      typs1 = GeomAbs_BezierSurface; // Using Imp-Prm Intersector
    else if (typs1 == GeomAbs_Plane && typs2 == GeomAbs_Cone)
      typs2 = GeomAbs_BezierSurface; // Using Imp-Prm Intersector
    else
    {
      // Using Prm-Prm Intersector
      typs1 = GeomAbs_BezierSurface;
      typs2 = GeomAbs_BezierSurface;
    }
  }
  //  Modified by skv - Mon Sep 26 14:58:30 2005 End

  // Surface type definition
  int ts1 = 0;
  switch (typs1)
  {
    case GeomAbs_Plane:
    case GeomAbs_Cylinder:
    case GeomAbs_Sphere:
    case GeomAbs_Cone:
      ts1 = 1;
      break;
    case GeomAbs_Torus:
      ts1 = bGeomGeom;
      break;
    default:
      break;
  }

  int ts2 = 0;
  switch (typs2)
  {
    case GeomAbs_Plane:
    case GeomAbs_Cylinder:
    case GeomAbs_Sphere:
    case GeomAbs_Cone:
      ts2 = 1;
      break;
    case GeomAbs_Torus:
      ts2 = bGeomGeom;
      break;
    default:
      break;
  }
  //
  // treatment of the cases with torus and any other geom surface
  //
  // Possible intersection types: 1. ts1 == ts2 == 1 <Geom-Geom>
  //                              2. ts1 != ts2      <Geom-Param>
  //                              3. ts1 == ts2 == 0 <Param-Param>

  // Geom - Geom
  if (ts1 == ts2 && ts1 == 1)
  {
    NCollection_List<IntSurf_PntOn2S> ListOfPnts;
    ListOfPnts.Clear();
    if (isGeomInt)
    {
      GeomGeomPerfom(theS1,
                     theD1,
                     theS2,
                     theD2,
                     TolArc,
                     TolTang,
                     ListOfPnts,
                     typs1,
                     typs2,
                     theIsReqToKeepRLine);
    }
    else
    {
      ParamParamPerfom(theS1, theD1, theS2, theD2, TolArc, TolTang, ListOfPnts, typs1, typs2);
    }
  }

  // Geom - Param
  if (ts1 != ts2)
  {
    GeomParamPerfom(theS1, theD1, theS2, theD2, ts1 == 0, typs1, typs2);
  }

  // Param - Param
  if (ts1 == ts2 && ts1 == 0)
  {
    NCollection_List<IntSurf_PntOn2S> ListOfPnts;
    ListOfPnts.Clear();

    ParamParamPerfom(theS1, theD1, theS2, theD2, TolArc, TolTang, ListOfPnts, typs1, typs2);
  }

  if (!theIsReqToPostWLProc)
    return;

  for (int i = NCollection_Sequence<opencascade::handle<IntPatch_Line>>::Lower(); i <= slin.Upper();
       i++)
  {
    occ::handle<IntPatch_WLine> aWL = occ::down_cast<IntPatch_WLine>(slin.Value(i));

    if (aWL.IsNull())
      continue;

    if (!aWL->IsPurgingAllowed())
      continue;

    occ::handle<IntPatch_WLine> aRW =
      IntPatch_WLineTool::ComputePurgedWLine(aWL, theS1, theS2, theD1, theD2);

    if (aRW.IsNull())
      continue;

    slin.InsertAfter(i, aRW);
    slin.Remove(i);
  }
}

//=================================================================================================

void IntPatch_Intersection::Perform(const occ::handle<Adaptor3d_Surface>&   theS1,
                                    const occ::handle<Adaptor3d_TopolTool>& theD1,
                                    const occ::handle<Adaptor3d_Surface>&   theS2,
                                    const occ::handle<Adaptor3d_TopolTool>& theD2,
                                    const double                            TolArc,
                                    const double                            TolTang,
                                    NCollection_List<IntSurf_PntOn2S>&      ListOfPnts,
                                    const bool                              isGeomInt,
                                    const bool                              theIsReqToKeepRLine,
                                    const bool                              theIsReqToPostWLProc)
{
  myTolArc  = TolArc;
  myTolTang = TolTang;
  if (myFleche <= Precision::PConfusion())
    myFleche = 0.01;
  if (myUVMaxStep <= Precision::PConfusion())
    myUVMaxStep = 0.01;
  done = false;
  spnt.Clear();
  slin.Clear();
  empt = true;
  tgte = false;
  oppo = false;

  GeomAbs_SurfaceType typs1 = theS1->GetType();
  GeomAbs_SurfaceType typs2 = theS2->GetType();
  //
  // treatment of the cases with cone or torus
  bool TreatAsBiParametric = false;
  int  bGeomGeom           = 0;
  //
  if (typs1 == GeomAbs_Cone || typs2 == GeomAbs_Cone || typs1 == GeomAbs_Torus
      || typs2 == GeomAbs_Torus)
  {
    gp_Ax1              aCTAx, aGeomAx;
    GeomAbs_SurfaceType aCTType;
    bool                bToCheck;
    //
    const occ::handle<Adaptor3d_Surface>& aCTSurf =
      (typs1 == GeomAbs_Cone || typs1 == GeomAbs_Torus) ? theS1 : theS2;
    const occ::handle<Adaptor3d_Surface>& aGeomSurf =
      (typs1 == GeomAbs_Cone || typs1 == GeomAbs_Torus) ? theS2 : theS1;
    //
    aCTType  = aCTSurf->GetType();
    bToCheck = false;
    //
    if (typs1 == GeomAbs_Cone || typs2 == GeomAbs_Cone)
    {
      const gp_Cone aCon1 = (aCTType == GeomAbs_Cone) ? aCTSurf->Cone() : aGeomSurf->Cone();
      double        a1    = std::abs(aCon1.SemiAngle());
      bToCheck            = (a1 < 0.02) || (a1 > 1.55);
      //
      if (typs1 == typs2)
      {
        const gp_Cone aCon2 = aGeomSurf->Cone();
        double        a2    = std::abs(aCon2.SemiAngle());
        bToCheck            = bToCheck || (a2 < 0.02) || (a2 > 1.55);
        //
        if (a1 > 1.55 && a2 > 1.55)
        { // quasi-planes: if same domain, treat as canonic
          const gp_Ax1 A1 = aCon1.Axis(), A2 = aCon2.Axis();
          if (A1.IsParallel(A2, Precision::Angular()))
          {
            const gp_Pnt Apex1 = aCon1.Apex(), Apex2 = aCon2.Apex();
            const gp_Pln Plan1(Apex1, A1.Direction());
            if (Plan1.Distance(Apex2) <= Precision::Confusion())
            {
              bToCheck = false;
            }
          }
        }
      }
      //
      TreatAsBiParametric = bToCheck;
      if (aCTType == GeomAbs_Cone)
      {
        aCTAx = aCon1.Axis();
      }
    }
    //
    if (typs1 == GeomAbs_Torus || typs2 == GeomAbs_Torus)
    {
      const gp_Torus aTor1 = (aCTType == GeomAbs_Torus) ? aCTSurf->Torus() : aGeomSurf->Torus();
      bToCheck             = aTor1.MajorRadius() > aTor1.MinorRadius();
      if (typs1 == typs2)
      {
        const gp_Torus aTor2 = aGeomSurf->Torus();
        bToCheck             = (bToCheck && (aTor2.MajorRadius() > aTor2.MinorRadius()))
                   || (std::abs(aTor1.MajorRadius() - aTor2.MajorRadius()) < TolTang
                       && std::abs(aTor1.MinorRadius() - aTor2.MinorRadius()) < TolTang);
      }
      //
      if (aCTType == GeomAbs_Torus)
      {
        aCTAx = aTor1.Axis();
      }
    }
    //
    if (bToCheck)
    {
      const gp_Lin aL1(aCTAx);
      //
      switch (aGeomSurf->GetType())
      {
        case GeomAbs_Plane: {
          aGeomAx = aGeomSurf->Plane().Axis();
          if (aCTType == GeomAbs_Cone)
          {
            bGeomGeom = 1;
            if (std::abs(aCTSurf->Cone().SemiAngle()) < 0.02)
            {
              double ps = std::abs(aCTAx.Direction().Dot(aGeomAx.Direction()));
              if (ps < 0.015)
              {
                bGeomGeom = 0;
              }
            }
          }
          else
          {
            if (aCTAx.IsParallel(aGeomAx, Precision::Angular())
                || (aCTAx.IsNormal(aGeomAx, Precision::Angular())
                    && (aGeomSurf->Plane().Distance(aCTAx.Location()) < Precision::Confusion())))
            {
              bGeomGeom = 1;
            }
          }
          bToCheck = false;
          break;
        }
        case GeomAbs_Sphere: {
          if (aL1.Distance(aGeomSurf->Sphere().Location()) < Precision::Confusion())
          {
            bGeomGeom = 1;
          }
          bToCheck = false;
          break;
        }
        case GeomAbs_Cylinder:
          aGeomAx = aGeomSurf->Cylinder().Axis();
          break;
        case GeomAbs_Cone:
          aGeomAx = aGeomSurf->Cone().Axis();
          break;
        case GeomAbs_Torus:
          aGeomAx = aGeomSurf->Torus().Axis();
          break;
        default:
          bToCheck = false;
          break;
      }
      //
      if (bToCheck)
      {
        if (aCTAx.IsParallel(aGeomAx, Precision::Angular())
            && (aL1.Distance(aGeomAx.Location()) <= Precision::Confusion()))
        {
          bGeomGeom = 1;
        }
      }
      //
      if (bGeomGeom == 1)
      {
        TreatAsBiParametric = false;
      }
    }
  }
  //

  if (theD1->DomainIsInfinite() || theD2->DomainIsInfinite())
  {
    TreatAsBiParametric = false;
  }

  if (TreatAsBiParametric)
  {
    // Using Prm-Prm Intersector
    typs1 = GeomAbs_BezierSurface;
    typs2 = GeomAbs_BezierSurface;
  }

  // Surface type definition
  int ts1 = 0;
  switch (typs1)
  {
    case GeomAbs_Plane:
    case GeomAbs_Cylinder:
    case GeomAbs_Sphere:
    case GeomAbs_Cone:
      ts1 = 1;
      break;
    case GeomAbs_Torus:
      ts1 = bGeomGeom;
      break;
    default:
      break;
  }

  int ts2 = 0;
  switch (typs2)
  {
    case GeomAbs_Plane:
    case GeomAbs_Cylinder:
    case GeomAbs_Sphere:
    case GeomAbs_Cone:
      ts2 = 1;
      break;
    case GeomAbs_Torus:
      ts2 = bGeomGeom;
      break;
    default:
      break;
  }
  //
  // Possible intersection types: 1. ts1 == ts2 == 1 <Geom-Geom>
  //                              2. ts1 != ts2      <Geom-Param>
  //                              3. ts1 == ts2 == 0 <Param-Param>

  if (!isGeomInt)
  {
    ParamParamPerfom(theS1, theD1, theS2, theD2, TolArc, TolTang, ListOfPnts, typs1, typs2);
  }
  else if (ts1 != ts2)
  {
    GeomParamPerfom(theS1, theD1, theS2, theD2, ts1 == 0, typs1, typs2);
  }
  else if (ts1 == 0)
  {
    ParamParamPerfom(theS1, theD1, theS2, theD2, TolArc, TolTang, ListOfPnts, typs1, typs2);
  }
  else if (ts1 == 1)
  {
    GeomGeomPerfom(theS1,
                   theD1,
                   theS2,
                   theD2,
                   TolArc,
                   TolTang,
                   ListOfPnts,
                   typs1,
                   typs2,
                   theIsReqToKeepRLine);
  }

  if (!theIsReqToPostWLProc)
    return;

  for (int i = NCollection_Sequence<opencascade::handle<IntPatch_Line>>::Lower(); i <= slin.Upper();
       i++)
  {
    occ::handle<IntPatch_WLine> aWL = occ::down_cast<IntPatch_WLine>(slin.Value(i));

    if (aWL.IsNull())
      continue;

    if (!aWL->IsPurgingAllowed())
      continue;

    occ::handle<IntPatch_WLine> aRW =
      IntPatch_WLineTool::ComputePurgedWLine(aWL, theS1, theS2, theD1, theD2);

    if (aRW.IsNull())
      continue;

    slin.InsertAfter(i, aRW);
    slin.Remove(i);
  }
}

//=================================================================================================

void IntPatch_Intersection::ParamParamPerfom(const occ::handle<Adaptor3d_Surface>&   theS1,
                                             const occ::handle<Adaptor3d_TopolTool>& theD1,
                                             const occ::handle<Adaptor3d_Surface>&   theS2,
                                             const occ::handle<Adaptor3d_TopolTool>& theD2,
                                             const double                            TolArc,
                                             const double                            TolTang,
                                             NCollection_List<IntSurf_PntOn2S>&      ListOfPnts,
                                             const GeomAbs_SurfaceType               typs1,
                                             const GeomAbs_SurfaceType               typs2)
{
  IntPatch_PrmPrmIntersection interpp;
  //
  if (!theD1->DomainIsInfinite() && !theD2->DomainIsInfinite())
  {
    bool ClearFlag = true;
    if (!ListOfPnts.IsEmpty())
    {
      interpp
        .Perform(theS1, theD1, theS2, theD2, TolTang, TolArc, myFleche, myUVMaxStep, ListOfPnts);
      ClearFlag = false;
    }
    interpp.Perform(theS1, theD1, theS2, theD2, TolTang, TolArc, myFleche, myUVMaxStep, ClearFlag);
  }
  else if ((theD1->DomainIsInfinite()) ^ (theD2->DomainIsInfinite()))
  {
    gp_Pnt pMaxXYZ, pMinXYZ;
    if (theD1->DomainIsInfinite())
    {
      FUN_GetMinMaxXYZPnt(theS2, pMinXYZ, pMaxXYZ);
      const double MU =
        std::max(std::abs(theS2->FirstUParameter()), std::abs(theS2->LastUParameter()));
      const double MV =
        std::max(std::abs(theS2->FirstVParameter()), std::abs(theS2->LastVParameter()));
      const double                   AP = std::max(MU, MV);
      occ::handle<Adaptor3d_Surface> SS;
      FUN_TrimInfSurf(pMinXYZ, pMaxXYZ, theS1, AP, SS);
      interpp.Perform(SS, theD1, theS2, theD2, TolTang, TolArc, myFleche, myUVMaxStep);
    }
    else
    {
      FUN_GetMinMaxXYZPnt(theS1, pMinXYZ, pMaxXYZ);
      const double MU =
        std::max(std::abs(theS1->FirstUParameter()), std::abs(theS1->LastUParameter()));
      const double MV =
        std::max(std::abs(theS1->FirstVParameter()), std::abs(theS1->LastVParameter()));
      const double                   AP = std::max(MU, MV);
      occ::handle<Adaptor3d_Surface> SS;
      FUN_TrimInfSurf(pMinXYZ, pMaxXYZ, theS2, AP, SS);
      interpp.Perform(theS1, theD1, SS, theD2, TolTang, TolArc, myFleche, myUVMaxStep);
    }
  } //(theD1->DomainIsInfinite()) ^ (theD2->DomainIsInfinite())
  else
  {
    if (typs1 == GeomAbs_OtherSurface || typs2 == GeomAbs_OtherSurface)
    {
      done = false;
      return;
    }

    bool                         IsPLInt = false;
    NCollection_Sequence<gp_Pnt> sop;
    gp_Vec                       v;
    FUN_PL_Intersection(theS1, typs1, theS2, typs2, IsPLInt, sop, v);

    if (IsPLInt)
    {
      if (sop.Length() > 0)
      {
        for (int ip = 1; ip <= sop.Length(); ip++)
        {
          gp_Lin                     lin(sop.Value(ip), gp_Dir(v));
          occ::handle<IntPatch_Line> gl = new IntPatch_GLine(lin, false);
          slin.Append(gl);
        }

        done = true;
      }
      else
        done = false;

      return;
    } // 'COLLINEAR LINES'
    else
    {
      occ::handle<Adaptor3d_Surface> nS1 = theS1;
      occ::handle<Adaptor3d_Surface> nS2 = theS2;
      FUN_TrimBothSurf(theS1, typs1, theS2, typs2, 1.e+8, nS1, nS2);
      interpp.Perform(nS1, theD1, nS2, theD2, TolTang, TolArc, myFleche, myUVMaxStep);
    } // 'NON - COLLINEAR LINES'
  } // both domains are infinite

  if (interpp.IsDone())
  {
    done = true;
    tgte = false;
    empt = interpp.IsEmpty();

    for (int i = 1; i <= interpp.NbLines(); i++)
    {
      if (interpp.Line(i)->ArcType() != IntPatch_Walking)
        slin.Append(interpp.Line(i));
    }

    for (int i = 1; i <= interpp.NbLines(); i++)
    {
      if (interpp.Line(i)->ArcType() == IntPatch_Walking)
        slin.Append(interpp.Line(i));
    }
  }
}

//=======================================================================
////function : GeomGeomPerfom
// purpose  :
//=======================================================================
void IntPatch_Intersection::GeomGeomPerfom(const occ::handle<Adaptor3d_Surface>&   theS1,
                                           const occ::handle<Adaptor3d_TopolTool>& theD1,
                                           const occ::handle<Adaptor3d_Surface>&   theS2,
                                           const occ::handle<Adaptor3d_TopolTool>& theD2,
                                           const double                            TolArc,
                                           const double                            TolTang,
                                           NCollection_List<IntSurf_PntOn2S>&      ListOfPnts,
                                           const GeomAbs_SurfaceType               theTyps1,
                                           const GeomAbs_SurfaceType               theTyps2,
                                           const bool theIsReqToKeepRLine)
{
  IntPatch_ImpImpIntersection
    interii(theS1, theD1, theS2, theD2, myTolArc, myTolTang, theIsReqToKeepRLine);

  if (!interii.IsDone())
  {
    done = false;
    ParamParamPerfom(theS1, theD1, theS2, theD2, TolArc, TolTang, ListOfPnts, theTyps1, theTyps2);
    return;
  }

  done = (interii.GetStatus() == IntPatch_ImpImpIntersection::IntStatus_OK);
  empt = interii.IsEmpty();

  if (empt)
  {
    return;
  }

  const int aNbPointsInALine = 200;

  tgte = interii.TangentFaces();
  if (tgte)
    oppo = interii.OppositeFaces();

  bool                  isWLExist = false;
  IntPatch_ALineToWLine AToW(theS1, theS2, aNbPointsInALine);

  for (int i = 1; i <= interii.NbLines(); i++)
  {
    const occ::handle<IntPatch_Line>& line = interii.Line(i);
    if (line->ArcType() == IntPatch_Analytic)
    {
      isWLExist = true;
      AToW.MakeWLine(occ::down_cast<IntPatch_ALine>(line), slin);
    }
    else
    {
      if (line->ArcType() == IntPatch_Walking)
      {
        occ::down_cast<IntPatch_WLine>(line)->EnablePurging(false);
      }

      if ((line->ArcType() != IntPatch_Restriction) || theIsReqToKeepRLine)
        slin.Append(line);
    }
  }

  for (int i = 1; i <= interii.NbPnts(); i++)
  {
    spnt.Append(interii.Point(i));
  }

  if ((theTyps1 == GeomAbs_Cylinder) && (theTyps2 == GeomAbs_Cylinder))
  {
    IntPatch_WLineTool::JoinWLines(slin, spnt, theS1, theS2, TolTang);
  }

  if (isWLExist)
  {
    Bnd_Box2d    aBx1, aBx2;
    const double aU1F = theS1->FirstUParameter(), aU1L = theS1->LastUParameter(),
                 aV1F = theS1->FirstVParameter(), aV1L = theS1->LastVParameter(),
                 aU2F = theS2->FirstUParameter(), aU2L = theS2->LastUParameter(),
                 aV2F = theS2->FirstVParameter(), aV2L = theS2->LastVParameter();
    aBx1.Add(gp_Pnt2d(aU1F, aV1F));
    aBx1.Add(gp_Pnt2d(aU1L, aV1F));
    aBx1.Add(gp_Pnt2d(aU1L, aV1L));
    aBx1.Add(gp_Pnt2d(aU1F, aV1L));
    aBx2.Add(gp_Pnt2d(aU2F, aV2F));
    aBx2.Add(gp_Pnt2d(aU2L, aV2F));
    aBx2.Add(gp_Pnt2d(aU2L, aV2L));
    aBx2.Add(gp_Pnt2d(aU2F, aV2L));

    aBx1.Enlarge(Precision::PConfusion());
    aBx2.Enlarge(Precision::PConfusion());

    const double anArrOfPeriod[4] = {theS1->IsUPeriodic() ? theS1->UPeriod() : 0.0,
                                     theS1->IsVPeriodic() ? theS1->VPeriod() : 0.0,
                                     theS2->IsUPeriodic() ? theS2->UPeriod() : 0.0,
                                     theS2->IsVPeriodic() ? theS2->VPeriod() : 0.0};

    NCollection_List<gp_Pnt> aListOfCriticalPoints;

    if (theS1->GetType() == GeomAbs_Cone)
    {
      aListOfCriticalPoints.Append(theS1->Cone().Apex());
    }
    else if (theS1->GetType() == GeomAbs_Sphere)
    {
      aListOfCriticalPoints.Append(theS1->Value(0.0, M_PI_2));
      aListOfCriticalPoints.Append(theS1->Value(0.0, -M_PI_2));
    }

    if (theS2->GetType() == GeomAbs_Cone)
    {
      aListOfCriticalPoints.Append(theS2->Cone().Apex());
    }
    else if (theS2->GetType() == GeomAbs_Sphere)
    {
      aListOfCriticalPoints.Append(theS2->Value(0.0, M_PI_2));
      aListOfCriticalPoints.Append(theS2->Value(0.0, -M_PI_2));
    }

    IntPatch_WLineTool::ExtendTwoWLines(slin,
                                        theS1,
                                        theS2,
                                        TolTang,
                                        anArrOfPeriod,
                                        aBx1,
                                        aBx2,
                                        aListOfCriticalPoints);
  }
}

//=================================================================================================

void IntPatch_Intersection::GeomParamPerfom(const occ::handle<Adaptor3d_Surface>&   theS1,
                                            const occ::handle<Adaptor3d_TopolTool>& theD1,
                                            const occ::handle<Adaptor3d_Surface>&   theS2,
                                            const occ::handle<Adaptor3d_TopolTool>& theD2,
                                            const bool                              isNotAnalitical,
                                            const GeomAbs_SurfaceType               typs1,
                                            const GeomAbs_SurfaceType               typs2)
{
  IntPatch_ImpPrmIntersection interip;
  if (myIsStartPnt)
  {
    if (isNotAnalitical /*ts1 == 0*/)
      interip.SetStartPoint(myU1Start, myV1Start);
    else
      interip.SetStartPoint(myU2Start, myV2Start);
  }

  if (theD1->DomainIsInfinite() && theD2->DomainIsInfinite())
  {
    bool                         IsPLInt = false;
    NCollection_Sequence<gp_Pnt> sop;
    gp_Vec                       v;
    FUN_PL_Intersection(theS1, typs1, theS2, typs2, IsPLInt, sop, v);

    if (IsPLInt)
    {
      if (sop.Length() > 0)
      {
        for (int ip = 1; ip <= sop.Length(); ip++)
        {
          gp_Lin                     lin(sop.Value(ip), gp_Dir(v));
          occ::handle<IntPatch_Line> gl = new IntPatch_GLine(lin, false);
          slin.Append(gl);
        }

        done = true;
      }
      else
        done = false;

      return;
    }
    else
    {
      occ::handle<Adaptor3d_Surface> nS1 = theS1;
      occ::handle<Adaptor3d_Surface> nS2 = theS2;
      FUN_TrimBothSurf(theS1, typs1, theS2, typs2, 1.e+5, nS1, nS2);
      interip.Perform(nS1, theD1, nS2, theD2, myTolArc, myTolTang, myFleche, myUVMaxStep);
    }
  }
  else
    interip.Perform(theS1, theD1, theS2, theD2, myTolArc, myTolTang, myFleche, myUVMaxStep);

  if (interip.IsDone())
  {
    done = true;
    empt = interip.IsEmpty();

    if (!empt)
    {
      const int aNbLines = interip.NbLines();
      for (int i = 1; i <= aNbLines; i++)
      {
        if (interip.Line(i)->ArcType() != IntPatch_Walking)
          slin.Append(interip.Line(i));
      }

      for (int i = 1; i <= aNbLines; i++)
      {
        if (interip.Line(i)->ArcType() == IntPatch_Walking)
          slin.Append(interip.Line(i));
      }

      for (int i = 1; i <= interip.NbPnts(); i++)
        spnt.Append(interip.Point(i));
    }
  }
}

void IntPatch_Intersection::Perform(const occ::handle<Adaptor3d_Surface>&   S1,
                                    const occ::handle<Adaptor3d_TopolTool>& D1,
                                    const occ::handle<Adaptor3d_Surface>&   S2,
                                    const occ::handle<Adaptor3d_TopolTool>& D2,
                                    const double                            U1,
                                    const double                            V1,
                                    const double                            U2,
                                    const double                            V2,
                                    const double                            TolArc,
                                    const double                            TolTang)
{
  myTolArc  = TolArc;
  myTolTang = TolTang;
  if (myFleche == 0.0)
  {
#ifdef OCCT_DEBUG
    // std::cout<<" -- IntPatch_Intersection::myFleche fixe par defaut a 0.01 --"<<std::endl;
    // std::cout<<" -- Utiliser la Methode SetTolerances( ... ) "<<std::endl;
#endif
    myFleche = 0.01;
  }
  if (myUVMaxStep == 0.0)
  {
#ifdef OCCT_DEBUG
    // std::cout<<" -- IntPatch_Intersection::myUVMaxStep fixe par defaut a 0.01 --"<<std::endl;
    // std::cout<<" -- Utiliser la Methode SetTolerances( ... ) "<<std::endl;
#endif
    myUVMaxStep = 0.01;
  }

  done = false;
  spnt.Clear();
  slin.Clear();

  empt = true;
  tgte = false;
  oppo = false;

  const GeomAbs_SurfaceType typs1 = S1->GetType();
  const GeomAbs_SurfaceType typs2 = S2->GetType();

  if (typs1 == GeomAbs_Plane || typs1 == GeomAbs_Cylinder || typs1 == GeomAbs_Sphere
      || typs1 == GeomAbs_Cone || typs2 == GeomAbs_Plane || typs2 == GeomAbs_Cylinder
      || typs2 == GeomAbs_Sphere || typs2 == GeomAbs_Cone)
  {
    myIsStartPnt = true;
    myU1Start    = U1;
    myV1Start    = V1;
    myU2Start    = U2;
    myV2Start    = V2;
    Perform(S1, D1, S2, D2, TolArc, TolTang);
    myIsStartPnt = false;
  }
  else
  {
    IntPatch_PrmPrmIntersection interpp;
    interpp.Perform(S1, D1, S2, D2, U1, V1, U2, V2, TolTang, TolArc, myFleche, myUVMaxStep);
    if (interpp.IsDone())
    {
      done           = true;
      tgte           = false;
      empt           = interpp.IsEmpty();
      const int nblm = interpp.NbLines();
      int       i    = 1;
      for (; i <= nblm; i++)
        slin.Append(interpp.Line(i));
    }
  }

  for (int i = NCollection_Sequence<opencascade::handle<IntPatch_Line>>::Lower(); i <= slin.Upper();
       i++)
  {
    occ::handle<IntPatch_WLine> aWL = occ::down_cast<IntPatch_WLine>(slin.Value(i));

    if (aWL.IsNull())
      continue;

    if (!aWL->IsPurgingAllowed())
      continue;

    occ::handle<IntPatch_WLine> aRW = IntPatch_WLineTool::ComputePurgedWLine(aWL, S1, S2, D1, D2);

    if (aRW.IsNull())
      continue;

    slin.InsertAfter(i, aRW);
    slin.Remove(i);
  }
}

#ifdef DUMPOFIntPatch_Intersection

void IntPatch_Intersection__MAJ_R(occ::handle<Adaptor2d_Curve2d>* R1,
                                  occ::handle<Adaptor2d_Curve2d>*,
                                  int* NR1,
                                  int*,
                                  int nbR1,
                                  int,
                                  const IntPatch_Point& VTX)
{

  if (VTX.IsOnDomS1())
  {

    //-- long unsigned ptr= *((long unsigned *)(((occ::handle<Standard_Transient>
    //*)(&(VTX.ArcOnS1())))));
    for (int i = 0; i < nbR1; i++)
    {
      if (VTX.ArcOnS1() == R1[i])
      {
        NR1[i]++;
        printf("\n ******************************");
        return;
      }
    }
    printf("\n R Pas trouvee  (IntPatch)\n");
  }
}
#endif

void IntPatch_Intersection::Dump(const int /*Mode*/,
                                 const occ::handle<Adaptor3d_Surface>& /*S1*/,
                                 const occ::handle<Adaptor3d_TopolTool>& /*D1*/,
                                 const occ::handle<Adaptor3d_Surface>& /*S2*/,
                                 const occ::handle<Adaptor3d_TopolTool>& /*D2*/) const
{
#ifdef DUMPOFIntPatch_Intersection
  const int MAXR = 200;
  //-- ----------------------------------------------------------------------
  //--  construction de la liste des restrictions & vertex
  //--
  int                            NR1[MAXR], NR2[MAXR];
  occ::handle<Adaptor2d_Curve2d> R1[MAXR], R2[MAXR];
  int                            nbR1 = 0, nbR2 = 0;
  for (D1->Init(); D1->More() && nbR1 < MAXR; D1->Next())
  {
    R1[nbR1]  = D1->Value();
    NR1[nbR1] = 0;
    nbR1++;
  }
  for (D2->Init(); D2->More() && nbR2 < MAXR; D2->Next())
  {
    R2[nbR2]  = D2->Value();
    NR2[nbR2] = 0;
    nbR2++;
  }

  printf("\nDUMP_INT:  ----empt:%2ud  tgte:%2ud  oppo:%2ud ---------------------------------",
         empt,
         tgte,
         empt);
  int i, nbr1, nbr2, nbgl, nbgc, nbge, nbgp, nbgh, nbl, nbr, nbg, nbw, nba;
  nbl = nbr = nbg = nbw = nba = nbgl = nbge = nbr1 = nbr2 = nbgc = nbgp = nbgh = 0;
  nbl                                                                          = NbLines();
  for (i = 1; i <= nbl; i++)
  {
    const occ::handle<IntPatch_Line>& line  = Line(i);
    const IntPatch_IType              IType = line->ArcType();
    if (IType == IntPatch_Walking)
      nbw++;
    else if (IType == IntPatch_Restriction)
    {
      nbr++;
      occ::handle<IntPatch_RLine> rlin(occ::down_cast<IntPatch_RLine>(line));
      if (rlin->IsArcOnS1())
        nbr1++;
      if (rlin->IsArcOnS2())
        nbr2++;
    }
    else if (IType == IntPatch_Analytic)
      nba++;
    else
    {
      nbg++;
      if (IType == IntPatch_Lin)
        nbgl++;
      else if (IType == IntPatch_Circle)
        nbgc++;
      else if (IType == IntPatch_Parabola)
        nbgp++;
      else if (IType == IntPatch_Hyperbola)
        nbgh++;
      else if (IType == IntPatch_Ellipse)
        nbge++;
    }
  }

  printf("\nDUMP_INT:Lines:%2d Wlin:%2d Restr:%2d(On1:%2d On2:%2d) Ana:%2d Geom:%2d(L:%2d C:%2d "
         "E:%2d H:%2d P:%2d)",
         nbl,
         nbw,
         nbr,
         nbr1,
         nbr2,
         nba,
         nbg,
         nbgl,
         nbgc,
         nbge,
         nbgh,
         nbgp);

  IntPatch_LineConstructor LineConstructor(2);

  int nbllc = 0;
  nbw = nbr = nbg = nba = 0;
  int nbva, nbvw, nbvr, nbvg;
  nbva = nbvr = nbvw = nbvg = 0;
  for (j = 1; j <= nbl; j++)
  {
    int                               v, nbvtx;
    const occ::handle<IntPatch_Line>& intersLinej = Line(j);
    int                               NbLines;
    LineConstructor.Perform(SequenceOfLine(), intersLinej, S1, D1, S2, D2, 1e-7);
    NbLines = LineConstructor.NbLines();

    for (int k = 1; k <= NbLines; k++)
    {
      nbllc++;
      const occ::handle<IntPatch_Line>& LineK = LineConstructor.Line(k);
      if (LineK->ArcType() == IntPatch_Analytic)
      {
        occ::handle<IntPatch_ALine> alin(occ::down_cast<IntPatch_ALine>(LineK));
        nbvtx = alin->NbVertex();
        nbva += nbvtx;
        nba++;
        for (v = 1; v <= nbvtx; v++)
        {
          IntPatch_Intersection__MAJ_R(R1, R2, NR1, NR2, nbR1, nbR2, alin->Vertex(v));
        }
      }
      else if (LineK->ArcType() == IntPatch_Restriction)
      {
        occ::handle<IntPatch_RLine> rlin(occ::down_cast<IntPatch_RLine>(LineK));
        nbvtx = rlin->NbVertex();
        nbvr += nbvtx;
        nbr++;
        for (v = 1; v <= nbvtx; v++)
        {
          IntPatch_Intersection__MAJ_R(R1, R2, NR1, NR2, nbR1, nbR2, rlin->Vertex(v));
        }
      }
      else if (LineK->ArcType() == IntPatch_Walking)
      {
        occ::handle<IntPatch_WLine> wlin(occ::down_cast<IntPatch_WLine>(LineK));
        nbvtx = wlin->NbVertex();
        nbvw += nbvtx;
        nbw++;
        for (v = 1; v <= nbvtx; v++)
        {
          IntPatch_Intersection__MAJ_R(R1, R2, NR1, NR2, nbR1, nbR2, wlin->Vertex(v));
        }
      }
      else
      {
        occ::handle<IntPatch_GLine> glin(occ::down_cast<IntPatch_GLine>(LineK));
        nbvtx = glin->NbVertex();
        nbvg += nbvtx;
        nbg++;
        for (v = 1; v <= nbvtx; v++)
        {
          IntPatch_Intersection__MAJ_R(R1, R2, NR1, NR2, nbR1, nbR2, glin->Vertex(v));
        }
      }
    }
  }
  printf("\nDUMP_LC :Lines:%2d WLin:%2d Restr:%2d Ana:%2d Geom:%2d", nbllc, nbw, nbr, nba, nbg);
  printf("\nDUMP_LC :vtx          :%2d     r:%2d    :%2d     :%2d", nbvw, nbvr, nbva, nbvg);

  printf("\n");

#endif
}

//=================================================================================================

bool IntPatch_Intersection::CheckSingularPoints(const occ::handle<Adaptor3d_Surface>&   theS1,
                                                const occ::handle<Adaptor3d_TopolTool>& theD1,
                                                const occ::handle<Adaptor3d_Surface>&   theS2,
                                                double&                                 theDist)
{
  theDist         = Precision::Infinite();
  bool isSingular = false;
  if (theS1 == theS2)
  {
    return isSingular;
  }
  //
  const int        aNbBndPnts = 5;
  constexpr double aTol       = Precision::Confusion();
  int              i;
  theD1->Init();
  bool isU = true;
  for (; theD1->More(); theD1->Next())
  {
    occ::handle<Adaptor2d_Curve2d> aBnd = theD1->Value();
    double                         pinf = aBnd->FirstParameter(), psup = aBnd->LastParameter();
    if (Precision::IsNegativeInfinite(pinf) || Precision::IsPositiveInfinite(psup)
        || (std::abs(pinf) < Precision::PConfusion() && std::abs(psup) < Precision::PConfusion()))
    {
      continue;
    }
    double   t, dt = (psup - pinf) / (aNbBndPnts - 1);
    gp_Pnt2d aP1;
    gp_Vec2d aDir;
    aBnd->D1((pinf + psup) / 2., aP1, aDir);
    isU = std::abs(aDir.X()) > std::abs(aDir.Y());
    gp_Pnt aPP1;
    gp_Vec aDU, aDV;
    double aD1NormMax = 0.;
    gp_XYZ aPmid(0., 0., 0.);
    int    aNb = 0;
    for (t = pinf; t <= psup; t += dt)
    {
      aP1 = aBnd->Value(t);
      theS1->D1(aP1.X(), aP1.Y(), aPP1, aDU, aDV);
      if (isU)
        aD1NormMax = std::max(aD1NormMax, aDU.Magnitude());
      else
        aD1NormMax = std::max(aD1NormMax, aDV.Magnitude());

      aPmid += aPP1.XYZ();
      aNb++;

      if (aD1NormMax > aTol)
        break;
    }

    if (aD1NormMax <= aTol)
    {
      // Singular point aPP1;
      aPmid /= aNb;
      aPP1.SetXYZ(aPmid);
      constexpr double aTolU = Precision::PConfusion(), aTolV = Precision::PConfusion();
      Extrema_ExtPS    aProj(aPP1, *theS2.get(), aTolU, aTolV, Extrema_ExtFlag_MIN);

      if (aProj.IsDone())
      {
        int aNbExt = aProj.NbExt();
        for (i = 1; i <= aNbExt; ++i)
        {
          theDist = std::min(theDist, aProj.SquareDistance(i));
        }
      }
    }
  }
  if (!Precision::IsInfinite(theDist))
  {
    theDist    = std::sqrt(theDist);
    isSingular = true;
  }

  return isSingular;
}

//=================================================================================================

double IntPatch_Intersection::DefineUVMaxStep(const occ::handle<Adaptor3d_Surface>&   theS1,
                                              const occ::handle<Adaptor3d_TopolTool>& theD1,
                                              const occ::handle<Adaptor3d_Surface>&   theS2,
                                              const occ::handle<Adaptor3d_TopolTool>& theD2)
{
  double           anUVMaxStep  = 0.001;
  double           aDistToSing1 = Precision::Infinite();
  double           aDistToSing2 = Precision::Infinite();
  constexpr double aTolMin = Precision::Confusion(), aTolMax = 1.e-5;
  if (theS1 != theS2)
  {
    bool isSing1 = CheckSingularPoints(theS1, theD1, theS2, aDistToSing1);
    if (isSing1)
    {
      if (aDistToSing1 > aTolMin && aDistToSing1 < aTolMax)
      {
        anUVMaxStep = 0.0001;
      }
      else
      {
        isSing1 = false;
      }
    }
    if (!isSing1)
    {
      bool isSing2 = CheckSingularPoints(theS2, theD2, theS1, aDistToSing2);
      if (isSing2)
      {
        if (aDistToSing2 > aTolMin && aDistToSing2 < aTolMax)
        {
          anUVMaxStep = 0.0001;
        }
      }
    }
  }
  return anUVMaxStep;
}

//=======================================================================
// function : splitCone
// purpose  : Splits cone by the apex
//=======================================================================
static void splitCone(const occ::handle<Adaptor3d_Surface>&               theS,
                      const occ::handle<Adaptor3d_TopolTool>&             theD,
                      const double                                        theTol,
                      NCollection_Vector<occ::handle<Adaptor3d_Surface>>& theVecHS)
{
  if (theS->GetType() != GeomAbs_Cone)
  {
    throw Standard_NoSuchObject("IntPatch_Intersection : Surface is not Cone");
  }

  gp_Cone aCone = theS->Cone();

  double aU0, aV0;
  Adaptor3d_TopolTool::GetConeApexParam(aCone, aU0, aV0);

  TopAbs_State aState = theD->Classify(gp_Pnt2d(aU0, aV0), theTol);

  if (aState == TopAbs_IN || aState == TopAbs_ON)
  {
    const occ::handle<Adaptor3d_Surface> aHSDn =
      theS->VTrim(theS->FirstVParameter(), aV0, Precision::PConfusion());
    const occ::handle<Adaptor3d_Surface> aHSUp =
      theS->VTrim(aV0, theS->LastVParameter(), Precision::PConfusion());

    theVecHS.Append(aHSDn);
    theVecHS.Append(aHSUp);
  }
  else
  {
    theVecHS.Append(theS);
  }
}

//=======================================================================
// function : PrepareSurfaces
// purpose  : Prepares surfaces for intersection
//=======================================================================
void IntPatch_Intersection::PrepareSurfaces(
  const occ::handle<Adaptor3d_Surface>&               theS1,
  const occ::handle<Adaptor3d_TopolTool>&             theD1,
  const occ::handle<Adaptor3d_Surface>&               theS2,
  const occ::handle<Adaptor3d_TopolTool>&             theD2,
  const double                                        theTol,
  NCollection_Vector<occ::handle<Adaptor3d_Surface>>& theVecHS1,
  NCollection_Vector<occ::handle<Adaptor3d_Surface>>& theVecHS2)
{
  if ((theS1->GetType() == GeomAbs_Cone)
      && (std::abs(M_PI / 2. - std::abs(theS1->Cone().SemiAngle())) < theTol))
  {
    splitCone(theS1, theD1, theTol, theVecHS1);
  }
  else
  {
    theVecHS1.Append(theS1);
  }

  if ((theS2->GetType() == GeomAbs_Cone)
      && (std::abs(M_PI / 2. - std::abs(theS2->Cone().SemiAngle())) < theTol))
  {
    splitCone(theS2, theD2, theTol, theVecHS2);
  }
  else
  {
    theVecHS2.Append(theS2);
  }
}
