// Created on: 1995-09-22
// Created by: Bruno DUMORTIER
// Copyright (c) 1995-1999 Matra Datavision
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

#include <AppDef_Compute.hxx>
#include <AppDef_MultiLine.hxx>
#include <AppDef_MultiPointConstraint.hxx>
#include <AppParCurves_MultiCurve.hxx>
#include <BRepFill_ApproxSeewing.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <StdFail_NotDone.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_Integer.hxx>

//=================================================================================================

BRepFill_ApproxSeewing::BRepFill_ApproxSeewing()
    : myIsDone(false)
{
}

//=================================================================================================

BRepFill_ApproxSeewing::BRepFill_ApproxSeewing(const BRepFill_MultiLine& ML)
    : myIsDone(false)
{
  Perform(ML);
}

//=================================================================================================

void BRepFill_ApproxSeewing::Perform(const BRepFill_MultiLine& ML)
{
  myML = ML;

  // evaluate the approximative length of the 3dCurve
  int    i;
  double Length   = 0.;
  double U1       = myML.FirstParameter();
  double U2       = myML.LastParameter();
  int    NbPoints = 50;
  double Dist, dU = (U2 - U1) / (2 * NbPoints - 1);

  NCollection_Array1<gp_Pnt2d> LP(1, 2 * NbPoints); // tableau Longueur <-> Param
  gp_Pnt                       aPnt1, aPnt2;
  aPnt1 = myML.Value(U1);

  for (i = 0; i < 2 * NbPoints; i++)
  {
    aPnt2 = myML.Value(U1 + i * dU);
    Dist  = aPnt1.Distance(aPnt2);
    Length += Dist;
    LP(i + 1) = gp_Pnt2d(Length, U1 + (i * dU));
    aPnt1     = aPnt2;
  }

  // On cherche a mettre NbPoints dans la curve.
  // on met les points environ a Length/NbPoints.

  AppDef_MultiLine            MLS(NbPoints);
  AppDef_MultiPointConstraint MP(1, 2);
  gp_Pnt                      P3d;
  gp_Pnt2d                    PF1, PF2;

  ML.Value3dOnF1OnF2(U1, P3d, PF1, PF2);
  MP.SetPoint(1, P3d);
  MP.SetPoint2d(2, PF1);
  MP.SetPoint2d(3, PF2);
  MLS.SetValue(1, MP);

#ifdef DUMP_ML
  i = 1;
  std::cout << "--Point " << i << std::endl;
  std::cout << "P3d:    " << P3d.X() << " " << P3d.Y() << " " << P3d.Z() << std::endl;
  std::cout << "P2d1;2: " << PF1.X() << " " << PF1.Y() << " ; " << PF2.X() << " " << PF2.Y()
            << std::endl;
#endif

  double DCorde = Length / (NbPoints - 1);
  double Corde  = DCorde;
  int    Index  = 1;
  double U, Alpha;
  for (i = 2; i < NbPoints; i++)
  {
    while (LP(Index).X() < Corde)
      Index++;
    Alpha = (Corde - LP(Index - 1).X()) / (LP(Index).X() - LP(Index - 1).X());
    U     = LP(Index - 1).Y() + Alpha * (LP(Index).Y() - LP(Index - 1).Y());
    AppDef_MultiPointConstraint MPC(1, 2);
    ML.Value3dOnF1OnF2(U, P3d, PF1, PF2);
#ifdef DUMP_ML
    std::cout << "--Point " << i << std::endl;
    std::cout << "P3d:    " << P3d.X() << " " << P3d.Y() << " " << P3d.Z() << std::endl;
    std::cout << "P2d1;2: " << PF1.X() << " " << PF1.Y() << " ; " << PF2.X() << " " << PF2.Y()
              << std::endl;
#endif
    MPC.SetPoint(1, P3d);
    MPC.SetPoint2d(2, PF1);
    MPC.SetPoint2d(3, PF2);
    MLS.SetValue(i, MPC);
    Corde = i * DCorde;
  }
  AppDef_MultiPointConstraint MPE(1, 2);
  ML.Value3dOnF1OnF2(U2, P3d, PF1, PF2);
#ifdef DUMP_ML
  i = NbPoints;
  std::cout << "--Point " << i << std::endl;
  std::cout << "P3d:    " << P3d.X() << " " << P3d.Y() << " " << P3d.Z() << std::endl;
  std::cout << "P2d1;2: " << PF1.X() << " " << PF1.Y() << " ; " << PF2.X() << " " << PF2.Y()
            << std::endl;
#endif
  MPE.SetPoint(1, P3d);
  MPE.SetPoint2d(2, PF1);
  MPE.SetPoint2d(3, PF2);
  MLS.SetValue(NbPoints, MPE);

  AppDef_Compute Fit(MLS);

  int NbCurves = Fit.NbMultiCurves();
  //  int MaxDeg = 0;

  if (NbCurves == 0)
  {
#ifdef OCCT_DEBUG
    std::cout << " TrimSurfaceTool : Approx echoue, on met les polygones" << std::endl;
#endif

    NCollection_Array1<double> Knots(1, NbPoints);
    NCollection_Array1<int>    Mults(1, NbPoints);
    Mults.Init(1);
    Mults(1) = Mults(NbPoints) = 2;
    NCollection_Array1<gp_Pnt>   P(1, NbPoints);
    NCollection_Array1<gp_Pnt2d> P1(1, NbPoints);
    NCollection_Array1<gp_Pnt2d> P2(1, NbPoints);

    double                      Uf   = ML.FirstParameter();
    double                      Ul   = ML.LastParameter();
    double                      dUlf = (Ul - Uf) / (NbPoints - 1);
    AppDef_MultiPointConstraint MPC;
    for (i = 1; i <= NbPoints - 1; i++)
    {
      MPC      = MLS.Value(i);
      U        = Uf + (i - 1) * dUlf;
      P(i)     = MPC.Point(1);
      P1(i)    = MPC.Point2d(2);
      P2(i)    = MPC.Point2d(3);
      Knots(i) = U;
    }
    // eval the last point on Ul
    MPC             = MLS.Value(NbPoints);
    P(NbPoints)     = MPC.Point(1);
    P1(NbPoints)    = MPC.Point2d(2);
    P2(NbPoints)    = MPC.Point2d(3);
    Knots(NbPoints) = Ul;

    myCurve   = new Geom_BSplineCurve(P, Knots, Mults, 1);
    myPCurve1 = new Geom2d_BSplineCurve(P1, Knots, Mults, 1);
    myPCurve2 = new Geom2d_BSplineCurve(P2, Knots, Mults, 1);

    myIsDone = true;

    return;
  }

  // Les approx sont a priori OK.

  const AppParCurves_MultiBSpCurve& MBSp    = Fit.SplineValue();
  int                               NbPoles = MBSp.NbPoles();
  NCollection_Array1<gp_Pnt>        Poles(1, NbPoles);
  NCollection_Array1<gp_Pnt2d>      Poles2d1(1, NbPoles);
  NCollection_Array1<gp_Pnt2d>      Poles2d2(1, NbPoles);

  MBSp.Curve(1, Poles);
  MBSp.Curve(2, Poles2d1);
  MBSp.Curve(3, Poles2d2);

  const NCollection_Array1<double>& Knots  = MBSp.Knots();
  const NCollection_Array1<int>&    Mults  = MBSp.Multiplicities();
  int                               Degree = MBSp.Degree();

  myCurve   = new Geom_BSplineCurve(Poles, Knots, Mults, Degree);
  myPCurve1 = new Geom2d_BSplineCurve(Poles2d1, Knots, Mults, Degree);
  myPCurve2 = new Geom2d_BSplineCurve(Poles2d2, Knots, Mults, Degree);

  myIsDone = true;
}

//=================================================================================================

bool BRepFill_ApproxSeewing::IsDone() const
{
  return myIsDone;
}

//=======================================================================
// function : occ::handle<Geom_Curve>&
// purpose  :
//=======================================================================

const occ::handle<Geom_Curve>& BRepFill_ApproxSeewing::Curve() const
{
  StdFail_NotDone_Raise_if(!myIsDone, "BRepFill_ApproxSeewing::Curve");
  return myCurve;
}

//=======================================================================
// function : occ::handle<Geom2d_Curve>&
// purpose  :
//=======================================================================

const occ::handle<Geom2d_Curve>& BRepFill_ApproxSeewing::CurveOnF1() const
{
  StdFail_NotDone_Raise_if(!myIsDone, "BRepFill_ApproxSeewing::CurveOnF1");
  return myPCurve1;
}

//=======================================================================
// function : occ::handle<Geom2d_Curve>&
// purpose  :
//=======================================================================

const occ::handle<Geom2d_Curve>& BRepFill_ApproxSeewing::CurveOnF2() const
{
  StdFail_NotDone_Raise_if(!myIsDone, "BRepFill_ApproxSeewing::CurveOnF2");
  return myPCurve2;
}
