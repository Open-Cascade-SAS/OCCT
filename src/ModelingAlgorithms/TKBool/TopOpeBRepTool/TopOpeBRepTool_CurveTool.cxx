// Created on: 1993-06-24
// Created by: Jean Yves LEBEY
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

#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepApprox_Approx.hxx>
#include <BRepApprox_ApproxLine.hxx>
#include <BRepTools.hxx>
#include <ElSLib.hxx>
#include <gce_MakeCirc.hxx>
#include <gce_MakeLin.hxx>
#include <gce_MakeLin2d.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomLib_Check2dBSplineCurve.hxx>
#include <GeomLib_CheckBSplineCurve.hxx>
#include <GeomTools_Curve2dSet.hxx>
#include <gp_Lin.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <ProjLib_ProjectedCurve.hxx>
#include <Standard_NotImplemented.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepTool_CurveTool.hxx>
#include <TopOpeBRepTool_GeomTool.hxx>

// #include <Approx.hxx>
#ifdef OCCT_DEBUG
  #include <TopOpeBRepTool_KRO.hxx>
TOPKRO      KRO_CURVETOOL_APPRO("approximation");
extern bool TopOpeBRepTool_GettraceKRO();
extern bool TopOpeBRepTool_GettracePCURV();
extern bool TopOpeBRepTool_GettraceCHKBSPL();
#endif
// #define IFV
#define CurveImprovement
//=================================================================================================

TopOpeBRepTool_CurveTool::TopOpeBRepTool_CurveTool() = default;

//=================================================================================================

TopOpeBRepTool_CurveTool::TopOpeBRepTool_CurveTool(const TopOpeBRepTool_OutCurveType O)
{
  TopOpeBRepTool_GeomTool GT(O);
  SetGeomTool(GT);
}

//=================================================================================================

TopOpeBRepTool_CurveTool::TopOpeBRepTool_CurveTool(const TopOpeBRepTool_GeomTool& GT)
{
  SetGeomTool(GT);
}

//=================================================================================================

TopOpeBRepTool_GeomTool& TopOpeBRepTool_CurveTool::ChangeGeomTool()
{
  return myGeomTool;
}

//=================================================================================================

const TopOpeBRepTool_GeomTool& TopOpeBRepTool_CurveTool::GetGeomTool() const
{
  return myGeomTool;
}

//=================================================================================================

void TopOpeBRepTool_CurveTool::SetGeomTool(const TopOpeBRepTool_GeomTool& GT)
{
  myGeomTool.Define(GT);
}

//=================================================================================================

Standard_EXPORT occ::handle<Geom2d_Curve> MakePCurve(const ProjLib_ProjectedCurve& PC)
{
  occ::handle<Geom2d_Curve> C2D;
  switch (PC.GetType())
  {
    case GeomAbs_Line:
      C2D = new Geom2d_Line(PC.Line());
      break;
    case GeomAbs_Circle:
      C2D = new Geom2d_Circle(PC.Circle());
      break;
    case GeomAbs_Ellipse:
      C2D = new Geom2d_Ellipse(PC.Ellipse());
      break;
    case GeomAbs_Parabola:
      C2D = new Geom2d_Parabola(PC.Parabola());
      break;
    case GeomAbs_Hyperbola:
      C2D = new Geom2d_Hyperbola(PC.Hyperbola());
      break;
    case GeomAbs_BSplineCurve:
      C2D = PC.BSpline();
      break;
    default:
      throw Standard_NotImplemented("CurveTool::MakePCurve");
      break;
  }
  return C2D;
}

//------------------------------------------------------------------
static bool CheckApproxResults(const BRepApprox_Approx& Approx)
//------------------------------------------------------------------
{
  const AppParCurves_MultiBSpCurve& amc = Approx.Value(1);
  int                               np  = amc.NbPoles();
  int                               nc  = amc.NbCurves();
  if (np < 2 || nc < 1)
    return false;

  // check the knots for coincidence
  const NCollection_Array1<double>& knots = amc.Knots();
  for (int i = knots.Lower(); i < knots.Upper(); i++)
  {
    if (knots(i + 1) - knots(i) <= Epsilon(knots(i)))
    {
      return false;
    }
  }
  return true;
}

//------------------------------------------------------------------
static bool CheckPCurve(const occ::handle<Geom2d_Curve>& aPC, const TopoDS_Face& aFace)
//------------------------------------------------------------------
// check if points of the pcurve are out of the face bounds
{
  const int NPoints = 23;
  double    umin, umax, vmin, vmax;

  BRepTools::UVBounds(aFace, umin, umax, vmin, vmax);
  double tolU = std::max((umax - umin) * 0.01, Precision::Confusion());
  double tolV = std::max((vmax - vmin) * 0.01, Precision::Confusion());
  double fp   = aPC->FirstParameter();
  double lp   = aPC->LastParameter();
  double step = (lp - fp) / (NPoints + 1);

  // adjust domain for periodic surfaces
  TopLoc_Location           aLoc;
  occ::handle<Geom_Surface> aSurf = BRep_Tool::Surface(aFace, aLoc);
  if (aSurf->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
    aSurf = (occ::down_cast<Geom_RectangularTrimmedSurface>(aSurf))->BasisSurface();

  gp_Pnt2d pnt = aPC->Value((fp + lp) / 2);
  double   u, v;
  pnt.Coord(u, v);

  if (aSurf->IsUPeriodic())
  {
    double aPer   = aSurf->UPeriod();
    int    nshift = (int)((u - umin) / aPer);
    if (u < umin + aPer * nshift)
      nshift--;
    umin += aPer * nshift;
    umax += aPer * nshift;
  }
  if (aSurf->IsVPeriodic())
  {
    double aPer   = aSurf->VPeriod();
    int    nshift = (int)((v - vmin) / aPer);
    if (v < vmin + aPer * nshift)
      nshift--;
    vmin += aPer * nshift;
    vmax += aPer * nshift;
  }

  int i;
  for (i = 1; i <= NPoints; i++)
  {
    double p = fp + i * step;
    pnt      = aPC->Value(p);
    pnt.Coord(u, v);
    if (umin - u > tolU || u - umax > tolU || vmin - v > tolV || v - vmax > tolV)
      return false;
  }
  return true;
}

//------------------------------------------------------------------
static occ::handle<Geom_Curve> MakeCurve3DfromWLineApprox(const BRepApprox_Approx& Approx,
                                                          const int)
//------------------------------------------------------------------
{
  const AppParCurves_MultiBSpCurve& amc = Approx.Value(1);
  int                               np  = amc.NbPoles();
  // int nc = amc.NbCurves();
  NCollection_Array1<gp_Pnt> poles3d(1, np);
  int                        ic = 1;
  // for (ic=1; ic<=nc; ic++) {
  // if (ic == CI) {
  amc.Curve(ic, poles3d);
  //}
  //}

  const NCollection_Array1<double>& knots  = amc.Knots();
  const NCollection_Array1<int>&    mults  = amc.Multiplicities();
  int                               degree = amc.Degree();
  occ::handle<Geom_Curve>           C3D    = new Geom_BSplineCurve(poles3d, knots, mults, degree);
  return C3D;
}

//------------------------------------------------------------------
static occ::handle<Geom2d_Curve> MakeCurve2DfromWLineApproxAndPlane(const BRepApprox_Approx& Approx,
                                                                    const gp_Pln&            Pl)
//------------------------------------------------------------------
{
  const AppParCurves_MultiBSpCurve& amc = Approx.Value(1);
  int                               np  = amc.NbPoles();
  NCollection_Array1<gp_Pnt2d>      poles2d(1, np);
  NCollection_Array1<gp_Pnt>        poles3d(1, np);
  amc.Curve(1, poles3d);
  for (int i = 1; i <= np; i++)
  {
    double U, V;
    ElSLib::Parameters(Pl, poles3d.Value(i), U, V);
    poles2d.SetValue(i, gp_Pnt2d(U, V));
  }
  const NCollection_Array1<double>& knots  = amc.Knots();
  const NCollection_Array1<int>&    mults  = amc.Multiplicities();
  int                               degree = amc.Degree();
  occ::handle<Geom2d_Curve>         C2D    = new Geom2d_BSplineCurve(poles2d, knots, mults, degree);
  return C2D;
}

//------------------------------------------------------------------
static occ::handle<Geom2d_Curve> MakeCurve2DfromWLineApprox(const BRepApprox_Approx& Approx,
                                                            const int                CI)
//------------------------------------------------------------------
{
  const AppParCurves_MultiBSpCurve& amc = Approx.Value(1);
  int                               np  = amc.NbPoles();
  NCollection_Array1<gp_Pnt2d>      poles2d(1, np);
  int                               nc = amc.NbCurves();
  for (int ic = 1; ic <= nc; ic++)
    if (ic == CI)
      amc.Curve(ic, poles2d);
  const NCollection_Array1<double>& knots  = amc.Knots();
  const NCollection_Array1<int>&    mults  = amc.Multiplicities();
  int                               degree = amc.Degree();
  occ::handle<Geom2d_Curve>         C2D    = new Geom2d_BSplineCurve(poles2d, knots, mults, degree);
  return C2D;
}

//=================================================================================================

bool TopOpeBRepTool_CurveTool::MakeCurves(const double                     parmin,
                                          const double                     parmax,
                                          const occ::handle<Geom_Curve>&   C3D,
                                          const occ::handle<Geom2d_Curve>& PC1,
                                          const occ::handle<Geom2d_Curve>& PC2,
                                          const TopoDS_Shape&              S1,
                                          const TopoDS_Shape&              S2,
                                          occ::handle<Geom_Curve>&         C3Dnew,
                                          occ::handle<Geom2d_Curve>&       PC1new,
                                          occ::handle<Geom2d_Curve>&       PC2new,
                                          double&                          TolReached3d,
                                          double&                          TolReached2d) const
{
  const double TOLCHECK    = 1.e-7;
  const double TOLANGCHECK = 1.e-6;

  bool CompC3D = myGeomTool.CompC3D();

  // std::cout << "MakeCurves begin" << std::endl;
  if (!CompC3D)
    return false;

  bool   CompPC1 = myGeomTool.CompPC1();
  bool   CompPC2 = myGeomTool.CompPC2();
  double tol3d, tol2d;
  myGeomTool.GetTolerances(tol3d, tol2d);
  int NbPntMax = myGeomTool.NbPntMax();

#ifdef OCCT_DEBUG
  if (TopOpeBRepTool_GettraceKRO())
    KRO_CURVETOOL_APPRO.Start();
#endif

//----------------------------------
///*
#ifdef IFV
  char  name[16];
  char* nm = &name[0];
  Sprintf(name, "C3D_%d", ++NbCalls);
  DrawTrSurf::Set(nm, C3D);
  Sprintf(name, "PC1_%d", NbCalls);
  DrawTrSurf::Set(nm, PC1);
  Sprintf(name, "PC2_%d", NbCalls);
  DrawTrSurf::Set(nm, PC2);
#endif
  //*/
  //---------------------------------------------

  int iparmin = (int)parmin;
  int iparmax = (int)parmax;

  occ::handle<Geom_BSplineCurve>   HC3D(occ::down_cast<Geom_BSplineCurve>(C3D));
  occ::handle<Geom2d_BSplineCurve> HPC1(occ::down_cast<Geom2d_BSplineCurve>(PC1));
  occ::handle<Geom2d_BSplineCurve> HPC2(occ::down_cast<Geom2d_BSplineCurve>(PC2));

//--------------------- IFV - "improving" initial curves
#ifdef CurveImprovement
  int nbpol = HC3D->NbPoles();
  // std::cout <<"nbpol = " << nbpol << std::endl;
  if (nbpol > 100)
  {
    const NCollection_Array1<gp_Pnt>&   PolC3D = HC3D->Poles();
    NCollection_Array1<gp_Pnt2d>        aDummyPnt2d;
    const NCollection_Array1<gp_Pnt2d>& PolPC1 = CompPC1 ? HPC1->Poles() : aDummyPnt2d;
    const NCollection_Array1<gp_Pnt2d>& PolPC2 = CompPC2 ? HPC2->Poles() : aDummyPnt2d;
    NCollection_Array1<bool>            IsValid(1, nbpol);
    IsValid.Init(true);
    double tol   = std::max(1.e-10, 100. * tol3d * tol3d); // tol *= tol; - square distance
    double tl2d  = tol * (tol2d * tol2d) / (tol3d * tol3d);
    double def   = tol;
    double def2d = tol2d;

    int      ip = 1, NbPol = 1;
    double   d, d1, d2;
    gp_Pnt   P = PolC3D(ip);
    gp_Pnt2d P1, P2;
    if (CompPC1)
      P1 = PolPC1(ip);
    if (CompPC2)
      P2 = PolPC2(ip);

    for (ip = 2; ip <= nbpol; ip++)
    {
      if (IsValid(ip))
      {
        d = P.SquareDistance(PolC3D(ip));
        if (CompPC1)
        {
          d1 = P1.SquareDistance(PolPC1(ip));
        }
        else
        {
          d1 = 0.;
        }
        if (CompPC2)
        {
          d2 = P2.SquareDistance(PolPC2(ip));
        }
        else
        {
          d2 = 0.;
        }
        if (d > tol || d1 > tl2d || d2 > tl2d)
        {
          double dd = 0.;
          if (ip < nbpol)
            dd = P.Distance(PolC3D(ip + 1));
          if (ip < nbpol && dd < 10. * tol)
          {
            gce_MakeLin mkL(P, PolC3D(ip + 1));
            if (mkL.IsDone())
            {
              gp_Lin L = mkL.Value();
              d        = L.SquareDistance(PolC3D(ip));
              if (CompPC1)
              {
                gp_Lin2d L1 = gce_MakeLin2d(P1, PolPC1(ip + 1));
                d1          = L1.SquareDistance(PolPC1(ip));
              }
              else
                d1 = 0.;
              if (CompPC2)
              {
                gp_Lin2d L2 = gce_MakeLin2d(P2, PolPC2(ip + 1));
                d2          = L2.SquareDistance(PolPC2(ip));
              }
              else
                d2 = 0.;

              if (d > def || d1 > def2d || d2 > def2d)
              {
                NbPol++;
                P = PolC3D(ip);
                if (CompPC1)
                  P1 = PolPC1(ip);
                if (CompPC2)
                  P2 = PolPC2(ip);
              }
              else
              {
                IsValid(ip) = false;
              }
            }
            else
            {
              IsValid(ip + 1) = false;
              NbPol++;
              P = PolC3D(ip);
              if (CompPC1)
                P1 = PolPC1(ip);
              if (CompPC2)
                P2 = PolPC2(ip);
            }
          }
          else
          {
            NbPol++;
            P = PolC3D(ip);
            if (CompPC1)
              P1 = PolPC1(ip);
            if (CompPC2)
              P2 = PolPC2(ip);
          }
        }
        else
        {
          IsValid(ip) = false;
        }
      }
    }

    if (NbPol < 2)
    {
      IsValid(nbpol) = true;
      NbPol++;
    }

    if (NbPol < nbpol)
    {
  #ifdef IFV
      std::cout << "NbPol < nbpol : " << NbPol << " " << nbpol << std::endl;
  #endif
      NCollection_Array1<gp_Pnt>   Polc3d(1, NbPol);
      NCollection_Array1<gp_Pnt2d> Polpc1(1, NbPol);
      NCollection_Array1<gp_Pnt2d> Polpc2(1, NbPol);
      NCollection_Array1<double>   knots(1, NbPol);
      NCollection_Array1<int>      mults(1, NbPol);
      mults.Init(1);
      mults(1)     = 2;
      mults(NbPol) = 2;
      int count    = 0;
      for (ip = 1; ip <= nbpol; ip++)
      {
        if (IsValid(ip))
        {
          count++;
          Polc3d(count) = PolC3D(ip);
          if (CompPC1)
            Polpc1(count) = PolPC1(ip);
          if (CompPC2)
            Polpc2(count) = PolPC2(ip);
          knots(count) = count;
        }
      }

      Polc3d(NbPol) = PolC3D(nbpol);
      if (CompPC1)
        Polpc1(NbPol) = PolPC1(nbpol);
      if (CompPC2)
        Polpc2(NbPol) = PolPC2(nbpol);

      const_cast<occ::handle<Geom_Curve>&>(C3D) = new Geom_BSplineCurve(Polc3d, knots, mults, 1);
      if (CompPC1)
        const_cast<occ::handle<Geom2d_Curve>&>(PC1) =
          new Geom2d_BSplineCurve(Polpc1, knots, mults, 1);
      if (CompPC2)
        const_cast<occ::handle<Geom2d_Curve>&>(PC2) =
          new Geom2d_BSplineCurve(Polpc2, knots, mults, 1);
      iparmax = NbPol;

  #ifdef IFV
      Sprintf(name, "C3Dmod_%d", NbCalls);
      nm = &name[0];
      DrawTrSurf::Set(nm, C3D);
      Sprintf(name, "PC1mod_%d", NbCalls);
      nm = &name[0];
      DrawTrSurf::Set(nm, PC1);
      Sprintf(name, "PC2mod_%d", NbCalls);
      nm = &name[0];
      DrawTrSurf::Set(nm, PC2);
  #endif
    }
  }
//--------------- IFV - end "improving"
#endif

  BRepApprox_Approx Approx;

  int                        degmin          = 4;
  int                        degmax          = 8;
  Approx_ParametrizationType parametrization = Approx_ChordLength;

  int                               npol   = HC3D->NbPoles();
  const NCollection_Array1<gp_Pnt>& Polc3d = HC3D->Poles();
  NCollection_Array1<double>        par(1, npol);
  gp_Pnt                            P = Polc3d(1);

  bool   IsBad = false;
  int    ip;
  double ksi = 0., kc, kcprev = 0.;
  double dist;
  par(1) = 0.;
  for (ip = 2; ip <= npol; ip++)
  {
    dist = P.Distance(Polc3d(ip));

    if (dist < Precision::Confusion())
    {
      IsBad = true;
      break;
    }

    par(ip) = par(ip - 1) + dist;
    P       = Polc3d(ip);
  }

  if (!IsBad)
  {

    NCollection_Array1<double> Curvature(1, npol);

    if (npol > 3)
    {
      int ic = 1;
      for (ip = 2; ip <= npol - 1; ip += npol - 3)
      {
        gp_Vec v1(Polc3d(ip - 1), Polc3d(ip));
        gp_Vec v2(Polc3d(ip), Polc3d(ip + 1));
        if (!v1.IsParallel(v2, 1.e-4))
        {
          gce_MakeCirc mc(Polc3d(ip - 1), Polc3d(ip), Polc3d(ip + 1));
          gp_Circ      cir = mc.Value();
          kc               = 1. / cir.Radius();
        }
        else
          kc = 0.;

        Curvature(ic) = kc;
        ic            = npol;
      }
    }
    else if (npol == 3)
    {
      ip = 2;
      gp_Vec v1(Polc3d(ip - 1), Polc3d(ip));
      gp_Vec v2(Polc3d(ip), Polc3d(ip + 1));
      if (!v1.IsParallel(v2, 1.e-4))
      {
        gce_MakeCirc mc(Polc3d(ip - 1), Polc3d(ip), Polc3d(ip + 1));
        gp_Circ      cir = mc.Value();
        kc               = 1. / cir.Radius();
      }
      else
        kc = 0.;
      Curvature(1) = Curvature(npol) = kc;
    }
    else
    {
      Curvature(1) = Curvature(npol) = 0.;
    }

    ip        = 1;
    double dt = par(ip + 1) - par(ip);
    double dx = (Polc3d(ip + 1).X() - Polc3d(ip).X()) / dt,
           dy = (Polc3d(ip + 1).Y() - Polc3d(ip).Y()) / dt,
           dz = (Polc3d(ip + 1).Z() - Polc3d(ip).Z()) / dt;
    double dx1, dy1, dz1, d2x, d2y, d2z, d2t;

    for (ip = 2; ip <= npol - 1; ip++)
    {
      dt            = par(ip + 1) - par(ip);
      dx1           = (Polc3d(ip + 1).X() - Polc3d(ip).X()) / dt;
      dy1           = (Polc3d(ip + 1).Y() - Polc3d(ip).Y()) / dt,
      dz1           = (Polc3d(ip + 1).Z() - Polc3d(ip).Z()) / dt;
      d2t           = 2. / (par(ip + 1) - par(ip - 1));
      d2x           = d2t * (dx1 - dx);
      d2y           = d2t * (dy1 - dy);
      d2z           = d2t * (dz1 - dz);
      Curvature(ip) = std::sqrt(d2x * d2x + d2y * d2y + d2z * d2z);
      dx            = dx1;
      dy            = dy1;
      dz            = dz1;
    }

    double crit = 1000.; // empirical criterion !!!

    dt     = par(2) - par(1);
    kcprev = (Curvature(2) - Curvature(1)) / dt;
    for (ip = 2; ip <= npol - 1; ip++)
    {
      dt  = par(ip + 1) - par(ip);
      kc  = (Curvature(ip + 1) - Curvature(ip)) / dt;
      ksi = ksi + std::abs(kc - kcprev);
      if (ksi > crit)
      {
        IsBad = true;
        break;
      }
      kc = kcprev;
    }
  }
  // std::cout << NbCalls << " ksi = " << ksi << std::endl;
  // std::cout << "IsBad = " << IsBad << std::endl;

  if (IsBad)
  {
    double tt       = std::min(10. * tol3d, Precision::Approximation());
    tol2d           = tt * tol2d / tol3d;
    tol3d           = tt;
    NbPntMax        = 40;
    degmax          = 4;
    parametrization = Approx_Centripetal;
  }

  int  nitmax       = 0; // use projection only
  bool withtangency = true;

  bool                compminmaxUV = true;
  BRepAdaptor_Surface BAS1(TopoDS::Face(S1), compminmaxUV);
  BRepAdaptor_Surface BAS2(TopoDS::Face(S2), compminmaxUV);

  occ::handle<BRepApprox_ApproxLine> AL;
  AL = new BRepApprox_ApproxLine(HC3D, HPC1, HPC2);

  Approx
    .SetParameters(tol3d, tol2d, degmin, degmax, nitmax, NbPntMax, withtangency, parametrization);

  if (CompC3D && CompPC1 && BAS1.GetType() == GeomAbs_Plane)
  {
    //-- The curve X,Y,Z and U2,V2 is approximated
    Approx.Perform(BAS1, BAS2, AL, CompC3D, false, CompPC2, iparmin, iparmax);
  }

  else if (CompC3D && CompPC2 && BAS2.GetType() == GeomAbs_Plane)
  {
    //-- The curve X,Y,Z and U1,V1 is approximated
    Approx.Perform(BAS1, BAS2, AL, CompC3D, CompPC1, false, iparmin, iparmax);
  }

  else
  {
    Approx.Perform(BAS1, BAS2, AL, CompC3D, CompPC1, CompPC2, iparmin, iparmax);
  }

  // MSV Nov 9, 2001: do not raise exception in the case of failure,
  //                  but return status

  bool done = Approx.IsDone();
  done      = done && CheckApproxResults(Approx);

  if (done)
  {
    if (CompC3D && CompPC1 && BAS1.GetType() == GeomAbs_Plane)
    {
      C3Dnew = ::MakeCurve3DfromWLineApprox(Approx, 1);
      PC1new = ::MakeCurve2DfromWLineApproxAndPlane(Approx, BAS1.Plane());
      if (CompPC2)
        PC2new = ::MakeCurve2DfromWLineApprox(Approx, 2);
    }
    else if (CompC3D && CompPC2 && BAS2.GetType() == GeomAbs_Plane)
    {
      C3Dnew = ::MakeCurve3DfromWLineApprox(Approx, 1);
      if (CompPC1)
        PC1new = ::MakeCurve2DfromWLineApprox(Approx, 2);
      PC2new = ::MakeCurve2DfromWLineApproxAndPlane(Approx, BAS2.Plane());
    }
    else
    {
      if (CompC3D)
        C3Dnew = ::MakeCurve3DfromWLineApprox(Approx, 1);
      if (CompPC1)
        PC1new = ::MakeCurve2DfromWLineApprox(Approx, 2);
      if (CompPC2)
      {
        int i32 = (CompPC1) ? 3 : 2;
        PC2new  = ::MakeCurve2DfromWLineApprox(Approx, i32);
      }
    }

    // check the pcurves relatively the faces bounds
    if (CompPC1)
      done = done && CheckPCurve(PC1new, TopoDS::Face(S1));
    if (CompPC2)
      done = done && CheckPCurve(PC2new, TopoDS::Face(S2));
  }

  if (!done)
  {
    if (CompC3D)
      C3Dnew.Nullify();
    if (CompPC1)
      PC1new.Nullify();
    if (CompPC2)
      PC2new.Nullify();
    return false;
  }

#ifdef IFV
  Sprintf(name, "C3Dnew_%d", NbCalls);
  nm = &name[0];
  DrawTrSurf::Set(nm, C3Dnew);
  if (CompPC1)
  {
    Sprintf(name, "PC1new_%d", NbCalls);
    nm = &name[0];
    DrawTrSurf::Set(nm, PC1new);
  }
  if (CompPC2)
  {
    Sprintf(name, "PC2new_%d", NbCalls);
    nm = &name[0];
    DrawTrSurf::Set(nm, PC2new);
  }

#endif

  TolReached3d = Approx.TolReached3d();
  TolReached2d = Approx.TolReached2d();
#ifdef IFV
  std::cout << NbCalls << " : Tol = " << TolReached3d << " " << TolReached2d << std::endl;
#endif

  bool bf, bl;

  occ::handle<Geom_BSplineCurve> Curve(occ::down_cast<Geom_BSplineCurve>(C3Dnew));
  if (!Curve.IsNull())
  {
    GeomLib_CheckBSplineCurve cbsc(Curve, TOLCHECK, TOLANGCHECK);
    cbsc.NeedTangentFix(bf, bl);

#ifdef OCCT_DEBUG
    if (TopOpeBRepTool_GettraceCHKBSPL())
    {
      if (bf || bl)
      {
        std::cout << "Problem orientation GeomLib_CheckBSplineCurve : First = " << bf;
        std::cout << " Last = " << bl << std::endl;
      }
    }
#endif
    cbsc.FixTangent(bf, bl);
  }

  occ::handle<Geom2d_BSplineCurve> Curve2df(occ::down_cast<Geom2d_BSplineCurve>(PC1new));
  if (!Curve2df.IsNull())
  {
    GeomLib_Check2dBSplineCurve cbsc2df(Curve2df, TOLCHECK, TOLANGCHECK);
    cbsc2df.NeedTangentFix(bf, bl);
#ifdef OCCT_DEBUG
    if (TopOpeBRepTool_GettraceCHKBSPL())
    {
      if (bf || bl)
      {
        std::cout << "Problem orientation GeomLib_CheckBSplineCurve : First = " << bf;
        std::cout << " Last = " << bl << std::endl;
      }
    }
#endif
    cbsc2df.FixTangent(bf, bl);
  }

  occ::handle<Geom2d_BSplineCurve> Curve2ds(occ::down_cast<Geom2d_BSplineCurve>(PC2new));
  if (!Curve2ds.IsNull())
  {
    GeomLib_Check2dBSplineCurve cbsc2ds(Curve2ds, TOLCHECK, TOLANGCHECK);
    cbsc2ds.NeedTangentFix(bf, bl);
#ifdef OCCT_DEBUG
    if (TopOpeBRepTool_GettraceCHKBSPL())
    {
      if (bf || bl)
      {
        std::cout << "Problem orientation GeomLib_CheckBSplineCurve : First = " << bf;
        std::cout << " Last = " << bl << std::endl;
      }
    }
#endif
    cbsc2ds.FixTangent(bf, bl);
  }

#ifdef OCCT_DEBUG
  if (TopOpeBRepTool_GettraceKRO())
    KRO_CURVETOOL_APPRO.Stop();
#endif
  //  std::cout << "MakeCurves end" << std::endl;

  return true;
}

//=================================================================================================

occ::handle<Geom_Curve> TopOpeBRepTool_CurveTool::MakeBSpline1fromPnt(
  const NCollection_Array1<gp_Pnt>& Points)
{
  int Degree = 1;

  int i, nbpoints = Points.Length();
  int nbknots = nbpoints - Degree + 1;

  //  First compute the parameters
  //  double length = 0.;
  //  NCollection_Array1<double> parameters(1,nbpoints);
  //  for (i = 1; i < nbpoints; i++) {
  //    parameters(i) = length;
  //    double dist = Points(i).Distance(Points(i+1));
  //    length += dist;
  //  }
  //  parameters(nbpoints) = length;

  // knots and multiplicities
  NCollection_Array1<double> knots(1, nbknots);
  NCollection_Array1<int>    mults(1, nbknots);
  mults.Init(1);
  mults(1) = mults(nbknots) = Degree + 1;

  //  knots(1) = 0;
  //  for (i=2;i<nbknots;i++) knots(i) = (parameters(i) + parameters(i+1)) /2.;
  //  knots(nbknots) = length;

  // take point index as parameter : JYL 01/AUG/94
  for (i = 1; i <= nbknots; i++)
    knots(i) = (double)i;
  occ::handle<Geom_Curve> C = new Geom_BSplineCurve(Points, knots, mults, Degree);
  return C;
}

//=================================================================================================

occ::handle<Geom2d_Curve> TopOpeBRepTool_CurveTool::MakeBSpline1fromPnt2d(
  const NCollection_Array1<gp_Pnt2d>& Points)
{
  int Degree = 1;

  int i, nbpoints = Points.Length();
  int nbknots = nbpoints - Degree + 1;

  //  First compute the parameters
  //  double length = 0;
  //  NCollection_Array1<double> parameters(1,nbpoints);
  //  for (i = 1; i < nbpoints; i++) {
  //    parameters(i) = length;
  //    double dist = Points(i).Distance(Points(i+1));
  //    length += dist;
  //  }
  //  parameters(nbpoints) = length;

  // knots and multiplicities
  NCollection_Array1<double> knots(1, nbknots);
  NCollection_Array1<int>    mults(1, nbknots);
  mults.Init(1);
  mults(1) = mults(nbknots) = Degree + 1;

  //  knots(1) = 0;
  //  for (i=2;i<nbknots;i++) knots(i) = (parameters(i) + parameters(i+1)) /2.;
  //  knots(nbknots) = length;

  // take point index as parameter : JYL 01/AUG/94
  for (i = 1; i <= nbknots; i++)
    knots(i) = (double)i;
  occ::handle<Geom2d_Curve> C = new Geom2d_BSplineCurve(Points, knots, mults, Degree);
  return C;
}

//=================================================================================================

bool TopOpeBRepTool_CurveTool::IsProjectable(const TopoDS_Shape&            S,
                                             const occ::handle<Geom_Curve>& C3D)
{
  const TopoDS_Face&  F            = TopoDS::Face(S);
  bool                compminmaxUV = false;
  BRepAdaptor_Surface BAS(F, compminmaxUV);
  GeomAbs_SurfaceType suty = BAS.GetType();
  GeomAdaptor_Curve   GAC(C3D);
  GeomAbs_CurveType   cuty = GAC.GetType();

  // --------
  // avoid projection of 3d curve on surface in case
  // of a quadric (ellipse,hyperbola,parabola) on a cone.
  // Projection fails when the curve in not fully inside the UV domain
  // of the cone : only part of 2d curve is built.
  // NYI : projection of quadric on cone (crossing cone domain)
  // --------

  bool projectable = true;
  if (suty == GeomAbs_Cone)
  {
    if ((cuty == GeomAbs_Ellipse) || (cuty == GeomAbs_Hyperbola) || (cuty == GeomAbs_Parabola))
    {
      projectable = false;
    }
  }
  else if (suty == GeomAbs_Cylinder)
  {
    if (cuty == GeomAbs_Ellipse)
    {
      projectable = false;
    }
  }
  else if (suty == GeomAbs_Sphere)
  {
    if (cuty == GeomAbs_Circle)
    {
      projectable = false;
    }
  }
  else if (suty == GeomAbs_Torus)
  {
    if (cuty == GeomAbs_Circle)
    {
      projectable = false;
    }
  }

#ifdef OCCT_DEBUG
  if (TopOpeBRepTool_GettracePCURV())
  {
    std::cout << "--- IsProjectable : ";
    if (projectable)
      std::cout << "projectable" << std::endl;
    else
      std::cout << "NOT projectable" << std::endl;
  }
#endif

  return projectable;
}

//=================================================================================================

occ::handle<Geom2d_Curve> TopOpeBRepTool_CurveTool::MakePCurveOnFace(
  const TopoDS_Shape&            S,
  const occ::handle<Geom_Curve>& C3D,
  double&                        TolReached2d,
  const double                   first,
  const double                   last)

{
  bool trim = false;
  if (first < last)
    trim = true;

  const TopoDS_Face&  F            = TopoDS::Face(S);
  bool                compminmaxUV = false;
  BRepAdaptor_Surface BAS(F, compminmaxUV);
  GeomAdaptor_Curve   GAC;
  if (trim)
    GAC.Load(C3D, first, last);
  else
    GAC.Load(C3D);
  occ::handle<BRepAdaptor_Surface> BAHS = new BRepAdaptor_Surface(BAS);
  occ::handle<GeomAdaptor_Curve>   BAHC = new GeomAdaptor_Curve(GAC);
  ProjLib_ProjectedCurve           projcurv(BAHS, BAHC);
  occ::handle<Geom2d_Curve>        C2D = ::MakePCurve(projcurv);
  TolReached2d                         = projcurv.GetTolerance();

  double UMin, UMax, VMin, VMax;
  BRepTools::UVBounds(F, UMin, UMax, VMin, VMax);

  double   f = GAC.FirstParameter();
  double   l = GAC.LastParameter();
  double   t = (f + l) * .5;
  gp_Pnt2d pC2D;
  C2D->D0(t, pC2D);
  double u2 = pC2D.X();
  double v2 = pC2D.Y();

  if (BAS.GetType() == GeomAbs_Sphere)
  {
    // MSV: consider quasiperiodic shift of pcurve
    double VFirst  = BAS.FirstVParameter();
    double VLast   = BAS.LastVParameter();
    bool   mincond = v2 < VFirst;
    bool   maxcond = v2 > VLast;
    if (mincond || maxcond)
    {
      occ::handle<Geom2d_Curve> PCT = occ::down_cast<Geom2d_Curve>(C2D->Copy());
      // make mirror relative to the isoline of apex -PI/2 or PI/2
      gp_Trsf2d aTrsf;
      gp_Pnt2d  po(0, -M_PI / 2);
      if (maxcond)
        po.SetY(M_PI / 2);
      aTrsf.SetMirror(gp_Ax2d(po, gp_Dir2d(gp_Dir2d::D::X)));
      PCT->Transform(aTrsf);
      // add translation along U direction on PI
      gp_Vec2d vec(M_PI, 0);
      double   UFirst = BAS.FirstUParameter();
      if (u2 - UFirst - M_PI > -1e-7)
        vec.Reverse();
      PCT->Translate(vec);
      C2D = PCT;
      // recompute the test point
      C2D->D0(t, pC2D);
      u2 = pC2D.X();
      v2 = pC2D.Y();
    }
  }

  double du = 0.;
  if (BAHS->IsUPeriodic())
  {
    // modified by NIZHNY-MZV  Thu Mar 30 10:03:15 2000
    bool mincond = UMin - u2 > 1e-7;
    bool maxcond = u2 - UMax > 1e-7;
    bool decalu  = mincond || maxcond;
    if (decalu)
      du = (mincond) ? BAHS->UPeriod() : -BAHS->UPeriod();
    // bool decalu = ( u2 < UMin || u2 > UMax);
    // if (decalu) du = ( u2 < UMin ) ? BAHS->UPeriod() : -BAHS->UPeriod();
  }
  double dv = 0.;
  if (BAHS->IsVPeriodic())
  {
    // modified by NIZHNY-MZV  Thu Mar 30 10:06:24 2000
    bool mincond = VMin - v2 > 1e-7;
    bool maxcond = v2 - VMax > 1e-7;
    bool decalv  = mincond || maxcond;
    if (decalv)
      dv = (mincond) ? BAHS->VPeriod() : -BAHS->VPeriod();
    // bool decalv = ( v2 < VMin || v2 > VMax);
    // if (decalv) dv = ( v2 < VMin ) ? BAHS->VPeriod() : -BAHS->VPeriod();
  }

  if (du != 0. || dv != 0.)
  {
    occ::handle<Geom2d_Curve> PCT = occ::down_cast<Geom2d_Curve>(C2D->Copy());
    PCT->Translate(gp_Vec2d(du, dv));
    C2D = PCT;
  }

  return C2D;
}
