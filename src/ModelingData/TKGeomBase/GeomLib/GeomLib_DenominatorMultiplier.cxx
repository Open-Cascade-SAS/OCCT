// Created on: 1997-05-13
// Created by: Stagiaire Francois DUMONT
// Copyright (c) 1997-1999 Matra Datavision
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

#include <BSplSLib.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomLib_DenominatorMultiplier.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <math_Matrix.hxx>
#include <NCollection_Array2.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>

//=================================================================================================

GeomLib_DenominatorMultiplier::GeomLib_DenominatorMultiplier(
  const occ::handle<Geom_BSplineSurface>& Surface,
  const NCollection_Array1<double>&       KnotVector)
    : mySurface(Surface),
      myKnotFlatVector(1, KnotVector.Length())
{
  int i;
  for (i = 1; i <= KnotVector.Length(); i++)
    myKnotFlatVector.SetValue(i, KnotVector(i));
}

//=======================================================================
// function : value
// purpose  : give the value of a(UParameter,VParameter)
//=======================================================================

double GeomLib_DenominatorMultiplier::Value(const double UParameter, const double VParameter) const

{
  double Dumaxv, Duminv, dDduumaxv, dDduuminv, Dv, Buv = 0.0;
  // gp_Pnt         HermPnt;
  gp_Pnt N;
  gp_Vec Nu, Nv;

  const NCollection_Array2<gp_Pnt>& surface_poles   = mySurface->Poles();
  const NCollection_Array2<double>* surface_weights = mySurface->Weights();
  const NCollection_Array1<double>& surface_u_knots = mySurface->UKnots();
  const NCollection_Array1<int>&    surface_u_mults = mySurface->UMultiplicities();
  const NCollection_Array1<double>& surface_v_knots = mySurface->VKnots();
  const NCollection_Array1<int>&    surface_v_mults = mySurface->VMultiplicities();
  int                               udegree         = mySurface->UDegree();
  int                               vdegree         = mySurface->VDegree();

  BSplSLib::HomogeneousD1(mySurface->UKnot(mySurface->LastUKnotIndex()),
                          VParameter,
                          0,
                          0,
                          surface_poles,
                          surface_weights,
                          surface_u_knots,
                          surface_v_knots,
                          &surface_u_mults,
                          &surface_v_mults,
                          udegree,
                          vdegree,
                          mySurface->IsURational(),
                          mySurface->IsVRational(),
                          mySurface->IsUPeriodic(),
                          mySurface->IsVPeriodic(),
                          N,
                          Nu,
                          Nv,
                          Dumaxv,
                          dDduumaxv,
                          Dv);
  BSplSLib::HomogeneousD1(mySurface->UKnot(1),
                          VParameter,
                          0,
                          0,
                          surface_poles,
                          surface_weights,
                          surface_u_knots,
                          surface_v_knots,
                          &surface_u_mults,
                          &surface_v_mults,
                          udegree,
                          vdegree,
                          mySurface->IsURational(),
                          mySurface->IsVRational(),
                          mySurface->IsUPeriodic(),
                          mySurface->IsVPeriodic(),
                          N,
                          Nu,
                          Nv,
                          Duminv,
                          dDduuminv,
                          Dv);

  math_Matrix BSplineBasisDeriv(1, 2, 1, 4, 0.0);
  double      B1prim0, Bprelastprim1,
    lambda = (mySurface->Weight(1, 1) / mySurface->Weight(mySurface->NbUPoles(), 1));
  int index, i;

  BSplCLib::EvalBsplineBasis(1, 4, myKnotFlatVector, 0.0, index, BSplineBasisDeriv);
  B1prim0 = BSplineBasisDeriv(2, 2);

  BSplCLib::EvalBsplineBasis(1, 4, myKnotFlatVector, 1.0, index, BSplineBasisDeriv);
  Bprelastprim1 = BSplineBasisDeriv(2, 3);

  math_Matrix BSplineBasisValue(1, 1, 1, 4, 0.0);
  BSplCLib::EvalBsplineBasis(0, 4, myKnotFlatVector, UParameter, index, BSplineBasisValue);

  NCollection_Array1<double> value(0, 5);
  NCollection_Array1<double> Polesenv(0, 5); // poles of a(u,v)

  for (i = 0; i <= 5; i++)
    Polesenv(i) = 0.0;
  Polesenv(0) = Duminv;
  Polesenv(1) = Duminv - dDduuminv / B1prim0;
  Polesenv(4) = lambda * lambda * (Dumaxv - dDduumaxv / Bprelastprim1);
  Polesenv(5) = lambda * lambda * Dumaxv;

  if (myKnotFlatVector.Length() == 8)
  {
    value(0) = BSplineBasisValue(1, 1); // values of the basic functions
    value(1) = BSplineBasisValue(1, 2);
    value(2) = 0.0;
    value(3) = 0.0;
    value(4) = BSplineBasisValue(1, 3);
    value(5) = BSplineBasisValue(1, 4);
  }
  if (myKnotFlatVector.Length() == 9)
  {
    if (index == 1)
    {
      value(0) = BSplineBasisValue(1, 1);
      value(1) = BSplineBasisValue(1, 2);
      value(2) = BSplineBasisValue(1, 3);
      value(3) = 0.0;
      value(4) = BSplineBasisValue(1, 4);
      value(5) = 0.0;
    }
    else
    {
      value(0) = 0.0;
      value(1) = BSplineBasisValue(1, 1);
      value(2) = BSplineBasisValue(1, 2);
      value(3) = 0.0;
      value(4) = BSplineBasisValue(1, 3);
      value(5) = BSplineBasisValue(1, 4);
    }
    Polesenv(2) = (0.5 * (Polesenv(0) + Polesenv(5)));
  }
  if (myKnotFlatVector.Length() == 10)
  {
    if (index == 1)
    {
      value(0) = BSplineBasisValue(1, 1);
      value(1) = BSplineBasisValue(1, 2);
      value(2) = BSplineBasisValue(1, 3);
      value(3) = BSplineBasisValue(1, 4);
      value(4) = 0.0;
      value(5) = 0.0;
    }
    if (index == 2)
    {
      value(0) = 0.0;
      value(1) = BSplineBasisValue(1, 1);
      value(2) = BSplineBasisValue(1, 2);
      value(3) = BSplineBasisValue(1, 3);
      value(4) = BSplineBasisValue(1, 4);
      value(5) = 0.0;
    }
    if (index == 3)
    {
      value(0) = 0.0;
      value(1) = 0.0;
      value(2) = BSplineBasisValue(1, 1);
      value(3) = BSplineBasisValue(1, 2);
      value(4) = BSplineBasisValue(1, 3);
      value(5) = BSplineBasisValue(1, 4);
    }
    Polesenv(2) = (0.5 * (Polesenv(0) + Polesenv(5)));
    Polesenv(3) = Polesenv(2);
  }
  for (i = 0; i <= 5; i++)
    Buv += Polesenv(i) * value(i);
  return Buv;
}
