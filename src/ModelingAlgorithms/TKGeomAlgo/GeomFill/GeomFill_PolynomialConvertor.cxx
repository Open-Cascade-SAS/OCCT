// Created on: 1997-07-18
// Created by: Philippe MANGIN
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

#include <Convert_CompPolynomialToPoles.hxx>
#include <GeomFill_PolynomialConvertor.hxx>
#include <gp_Mat.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <PLib.hxx>
#include <Standard_Integer.hxx>
#include <StdFail_NotDone.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <math_Vector.hxx>

GeomFill_PolynomialConvertor::GeomFill_PolynomialConvertor()
    : Ordre(8),
      myinit(false),
      BH(1, Ordre, 1, Ordre)
{
}

bool GeomFill_PolynomialConvertor::Initialized() const
{
  return myinit;
}

void GeomFill_PolynomialConvertor::Init()
{
  if (myinit)
    return;

  // BH = B * H where B is the monomial-to-BSpline conversion matrix on [-1,1], degree 7,
  // and H is the Hermite coefficients matrix. Both are mathematical constants computed once.
  static const math_Matrix THE_BH_MATRIX = []() {
    constexpr int              anOrdre = 8;
    NCollection_Array1<double> aCoeffs(1, anOrdre * anOrdre);
    NCollection_Array1<double> anInter(1, 2), aTrueInter(1, 2);
    anInter.SetValue(1, -1);
    anInter.SetValue(2, 1);
    aTrueInter.SetValue(1, -1);
    aTrueInter.SetValue(2, 1);
    aCoeffs.Init(0);
    for (int ii = 1; ii <= anOrdre; ii++)
      aCoeffs.SetValue(ii + (ii - 1) * anOrdre, 1);

    Convert_CompPolynomialToPoles     aConverter(anOrdre,
                                             anOrdre - 1,
                                             anOrdre - 1,
                                             aCoeffs,
                                             anInter,
                                             aTrueInter);
    const NCollection_Array2<double>& aPoles = aConverter.Poles();
    math_Matrix                       aB(1, anOrdre, 1, anOrdre);
    for (int jj = 1; jj <= anOrdre; jj++)
      for (int ii = 1; ii <= anOrdre; ii++)
      {
        double aTerm = aPoles.Value(ii, jj);
        if (std::abs(aTerm - 1) < 1.e-9)
          aTerm = 1;
        if (std::abs(aTerm + 1) < 1.e-9)
          aTerm = -1;
        aB(ii, jj) = aTerm;
      }

    math_Matrix aH(1, anOrdre, 1, anOrdre);
    PLib::HermiteCoefficients(-1, 1, anOrdre / 2 - 1, anOrdre / 2 - 1, aH);
    aH.Transpose();
    return math_Matrix(aB * aH);
  }();

  BH     = THE_BH_MATRIX;
  myinit = true;
}

void GeomFill_PolynomialConvertor::Section(const gp_Pnt&               FirstPnt,
                                           const gp_Pnt&               Center,
                                           const gp_Vec&               Dir,
                                           const double                Angle,
                                           NCollection_Array1<gp_Pnt>& Poles) const
{
  math_Vector Vx(1, Ordre), Vy(1, Ordre);
  math_Vector Px(1, Ordre), Py(1, Ordre);
  int         ii;
  double      Cos_b = std::cos(Angle), Sin_b = std::sin(Angle);
  double      beta, beta2, beta3;
  gp_Vec      V1(Center, FirstPnt), V2;
  V2    = Dir ^ V1;
  beta  = Angle / 2;
  beta2 = beta * beta;
  beta3 = beta * beta2;

  // Calcul de la transformation
  gp_Mat M(V1.X(), V2.X(), 0, V1.Y(), V2.Y(), 0, V1.Z(), V2.Z(), 0);

  // Calcul des contraintes  -----------
  Vx(1) = 1;
  Vy(1) = 0;
  Vx(2) = 0;
  Vy(2) = beta;
  Vx(3) = -beta2;
  Vy(3) = 0;
  Vx(4) = 0;
  Vy(4) = -beta3;
  Vx(5) = Cos_b;
  Vy(5) = Sin_b;
  Vx(6) = -beta * Sin_b;
  Vy(6) = beta * Cos_b;
  Vx(7) = -beta2 * Cos_b;
  Vy(7) = -beta2 * Sin_b;
  Vx(8) = beta3 * Sin_b;
  Vy(8) = -beta3 * Cos_b;

  // Calcul des poles
  Px = BH * Vx;
  Py = BH * Vy;
  gp_XYZ pnt;
  for (ii = 1; ii <= Ordre; ii++)
  {
    pnt.SetCoord(Px(ii), Py(ii), 0);
    pnt *= M;
    pnt += Center.XYZ();
    Poles(ii).ChangeCoord() = pnt;
  }
}

void GeomFill_PolynomialConvertor::Section(const gp_Pnt&               FirstPnt,
                                           const gp_Vec&               DFirstPnt,
                                           const gp_Pnt&               Center,
                                           const gp_Vec&               DCenter,
                                           const gp_Vec&               Dir,
                                           const gp_Vec&               DDir,
                                           const double                Angle,
                                           const double                DAngle,
                                           NCollection_Array1<gp_Pnt>& Poles,
                                           NCollection_Array1<gp_Vec>& DPoles) const
{
  math_Vector Vx(1, Ordre), Vy(1, Ordre), DVx(1, Ordre), DVy(1, Ordre);
  math_Vector Px(1, Ordre), Py(1, Ordre), DPx(1, Ordre), DPy(1, Ordre);
  int         ii;
  double      Cos_b = std::cos(Angle), Sin_b = std::sin(Angle);
  double      beta, beta2, beta3, bprim;
  gp_Vec      V1(Center, FirstPnt), V1Prim, V2;
  V2    = Dir ^ V1;
  beta  = Angle / 2;
  bprim = DAngle / 2;
  beta2 = beta * beta;
  beta3 = beta * beta2;

  // Calcul des  transformations
  gp_Mat M(V1.X(), V2.X(), 0, V1.Y(), V2.Y(), 0, V1.Z(), V2.Z(), 0);
  V1Prim = DFirstPnt - DCenter;
  V2     = (DDir ^ V1) + (Dir ^ V1Prim);
  gp_Mat MPrim(V1Prim.X(), V2.X(), 0, V1Prim.Y(), V2.Y(), 0, V1Prim.Z(), V2.Z(), 0);

  // Calcul des contraintes  -----------
  Vx(1) = 1;
  Vy(1) = 0;
  Vx(2) = 0;
  Vy(2) = beta;
  Vx(3) = -beta2;
  Vy(3) = 0;
  Vx(4) = 0;
  Vy(4) = -beta3;
  Vx(5) = Cos_b;
  Vy(5) = Sin_b;
  Vx(6) = -beta * Sin_b;
  Vy(6) = beta * Cos_b;
  Vx(7) = -beta2 * Cos_b;
  Vy(7) = -beta2 * Sin_b;
  Vx(8) = beta3 * Sin_b;
  Vy(8) = -beta3 * Cos_b;

  double b_bprim = bprim * beta, b2_bprim = bprim * beta2;
  DVx(1) = 0;
  DVy(1) = 0;
  DVx(2) = 0;
  DVy(2) = bprim;
  DVx(3) = -2 * b_bprim;
  DVy(3) = 0;
  DVx(4) = 0;
  DVy(4) = -3 * b2_bprim;
  DVx(5) = -2 * bprim * Sin_b;
  DVy(5) = 2 * bprim * Cos_b;
  DVx(6) = -bprim * Sin_b - 2 * b_bprim * Cos_b;
  DVy(6) = bprim * Cos_b - 2 * b_bprim * Sin_b;
  DVx(7) = 2 * b_bprim * (-Cos_b + beta * Sin_b);
  DVy(7) = -2 * b_bprim * (Sin_b + beta * Cos_b);
  DVx(8) = b2_bprim * (3 * Sin_b + 2 * beta * Cos_b);
  DVy(8) = b2_bprim * (2 * beta * Sin_b - 3 * Cos_b);

  // Calcul des poles
  Px  = BH * Vx;
  Py  = BH * Vy;
  DPx = BH * DVx;
  DPy = BH * DVy;
  gp_XYZ P, DP, aux;

  for (ii = 1; ii <= Ordre; ii++)
  {
    P.SetCoord(Px(ii), Py(ii), 0);
    Poles(ii).ChangeCoord() = M * P + Center.XYZ();
    P *= MPrim;
    DP.SetCoord(DPx(ii), DPy(ii), 0);
    DP *= M;
    aux.SetLinearForm(1, P, 1, DP, DCenter.XYZ());
    DPoles(ii).SetXYZ(aux);
  }
}

void GeomFill_PolynomialConvertor::Section(const gp_Pnt&               FirstPnt,
                                           const gp_Vec&               DFirstPnt,
                                           const gp_Vec&               D2FirstPnt,
                                           const gp_Pnt&               Center,
                                           const gp_Vec&               DCenter,
                                           const gp_Vec&               D2Center,
                                           const gp_Vec&               Dir,
                                           const gp_Vec&               DDir,
                                           const gp_Vec&               D2Dir,
                                           const double                Angle,
                                           const double                DAngle,
                                           const double                D2Angle,
                                           NCollection_Array1<gp_Pnt>& Poles,
                                           NCollection_Array1<gp_Vec>& DPoles,
                                           NCollection_Array1<gp_Vec>& D2Poles) const
{
  math_Vector Vx(1, Ordre), Vy(1, Ordre), DVx(1, Ordre), DVy(1, Ordre), D2Vx(1, Ordre),
    D2Vy(1, Ordre);
  math_Vector Px(1, Ordre), Py(1, Ordre), DPx(1, Ordre), DPy(1, Ordre), D2Px(1, Ordre),
    D2Py(1, Ordre);

  int    ii;
  double aux, Cos_b = std::cos(Angle), Sin_b = std::sin(Angle);
  double beta, beta2, beta3, bprim, bprim2, bsecn;
  gp_Vec V1(Center, FirstPnt), V1Prim, V1Secn, V2;
  V2     = Dir ^ V1;
  beta   = Angle / 2;
  bprim  = DAngle / 2;
  bsecn  = D2Angle / 2;
  bsecn  = D2Angle / 2;
  beta2  = beta * beta;
  beta3  = beta * beta2;
  bprim2 = bprim * bprim;

  // Calcul des  transformations
  gp_Mat M(V1.X(), V2.X(), 0, V1.Y(), V2.Y(), 0, V1.Z(), V2.Z(), 0);
  V1Prim = DFirstPnt - DCenter;
  V2     = (DDir ^ V1) + (Dir ^ V1Prim);

  gp_Mat MPrim(V1Prim.X(), V2.X(), 0, V1Prim.Y(), V2.Y(), 0, V1Prim.Z(), V2.Z(), 0);
  V1Secn = D2FirstPnt - D2Center;
  V2     = DDir ^ V1Prim;
  V2 *= 2;
  V2 += (D2Dir ^ V1) + (Dir ^ V1Secn);

  gp_Mat MSecn(V1Secn.X(), V2.X(), 0, V1Secn.Y(), V2.Y(), 0, V1Secn.Z(), V2.Z(), 0);

  // Calcul des contraintes  -----------
  Vx(1) = 1;
  Vy(1) = 0;
  Vx(2) = 0;
  Vy(2) = beta;
  Vx(3) = -beta2;
  Vy(3) = 0;
  Vx(4) = 0;
  Vy(4) = -beta3;
  Vx(5) = Cos_b;
  Vy(5) = Sin_b;
  Vx(6) = -beta * Sin_b;
  Vy(6) = beta * Cos_b;
  Vx(7) = -beta2 * Cos_b;
  Vy(7) = -beta2 * Sin_b;
  Vx(8) = beta3 * Sin_b;
  Vy(8) = -beta3 * Cos_b;

  double b_bprim = bprim * beta, b2_bprim = bprim * beta2, b_bsecn = bsecn * beta;
  DVx(1) = 0;
  DVy(1) = 0;
  DVx(2) = 0;
  DVy(2) = bprim;
  DVx(3) = -2 * b_bprim;
  DVy(3) = 0;
  DVx(4) = 0;
  DVy(4) = -3 * b2_bprim;
  DVx(5) = -2 * bprim * Sin_b;
  DVy(5) = 2 * bprim * Cos_b;
  DVx(6) = -bprim * Sin_b - 2 * b_bprim * Cos_b;
  DVy(6) = bprim * Cos_b - 2 * b_bprim * Sin_b;
  DVx(7) = 2 * b_bprim * (-Cos_b + beta * Sin_b);
  DVy(7) = -2 * b_bprim * (Sin_b + beta * Cos_b);
  DVx(8) = b2_bprim * (3 * Sin_b + 2 * beta * Cos_b);
  DVy(8) = b2_bprim * (2 * beta * Sin_b - 3 * Cos_b);

  D2Vx(1) = 0;
  D2Vy(1) = 0;
  D2Vx(2) = 0;
  D2Vy(2) = bsecn;
  D2Vx(3) = -2 * (bprim2 + b_bsecn);
  D2Vy(3) = 0;
  D2Vx(4) = 0;
  D2Vy(4) = -3 * beta * (2 * bprim2 + b_bsecn);
  D2Vx(5) = -2 * (bsecn * Sin_b + 2 * bprim2 * Cos_b);
  D2Vy(5) = 2 * (bsecn * Cos_b - 2 * bprim2 * Sin_b);
  D2Vx(6) = (4 * beta * bprim2 - bsecn) * Sin_b - 2 * (2 * bprim2 + b_bsecn) * Cos_b;

  D2Vy(6) = (bsecn - 4 * beta * bprim2) * Cos_b - 2 * (b_bsecn + 2 * bprim2) * Sin_b;

  aux     = 2 * (bprim2 + b_bsecn);
  D2Vx(7) = aux * (-Cos_b + beta * Sin_b) + 2 * beta * bprim2 * (2 * beta * Cos_b + 3 * Sin_b);

  D2Vy(7) = -aux * (Sin_b + beta * Cos_b) - 2 * beta * bprim2 * (3 * Cos_b - 2 * beta * Sin_b);

  aux     = beta * (2 * bprim2 + b_bsecn);
  D2Vx(8) = aux * (3 * Sin_b + 2 * beta * Cos_b) + 4 * beta2 * bprim2 * (2 * Cos_b - beta * Sin_b);

  D2Vy(8) = aux * (2 * beta * Sin_b - 3 * Cos_b) + 4 * beta2 * bprim2 * (2 * Sin_b + beta * Cos_b);

  // Calcul des poles
  Px   = BH * Vx;
  Py   = BH * Vy;
  DPx  = BH * DVx;
  DPy  = BH * DVy;
  D2Px = BH * D2Vx;
  D2Py = BH * D2Vy;

  gp_XYZ P, DP, D2P, auxyz;
  for (ii = 1; ii <= Ordre; ii++)
  {
    P.SetCoord(Px(ii), Py(ii), 0);
    DP.SetCoord(DPx(ii), DPy(ii), 0);
    D2P.SetCoord(D2Px(ii), D2Py(ii), 0);

    Poles(ii).ChangeCoord() = M * P + Center.XYZ();
    auxyz.SetLinearForm(1, MPrim * P, 1, M * DP, DCenter.XYZ());
    DPoles(ii).SetXYZ(auxyz);
    P *= MSecn;
    DP *= MPrim;
    D2P *= M;
    auxyz.SetLinearForm(1, P, 2, DP, 1, D2P, D2Center.XYZ());
    D2Poles(ii).SetXYZ(auxyz);
  }
}
