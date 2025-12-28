// Created on: 1994-09-06
// Created by: Yves FRICAUD
// Copyright (c) 1994-1999 Matra Datavision
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

#include <Geom2d_Curve.hxx>
#include <Geom2dLProp_Curve2dTool.hxx>
#include <Geom2dLProp_FuncCurExt.hxx>
#include <gp_Pnt2d.hxx>

//=================================================================================================

Geom2dLProp_FuncCurExt::Geom2dLProp_FuncCurExt(const occ::handle<Geom2d_Curve>& C,
                                               const double         Tol)
    : theCurve(C)
{
  epsX = Tol;
}

//=============================================================================
// function : Value
// purpose : KC = (V1^V2.Z) / ||V1||^3  avec V1 tangente etV2 derivee seconde.
//           F  = d KC/ dU.
//=============================================================================
bool Geom2dLProp_FuncCurExt::Value(const double X, double& F)
{
  gp_Pnt2d P1;
  gp_Vec2d V1, V2, V3;

  Geom2dLProp_Curve2dTool::D3(theCurve, X, P1, V1, V2, V3);
  double CPV1V2 = V1.Crossed(V2);
  double CPV1V3 = V1.Crossed(V3);
  double V1V2   = V1.Dot(V2);
  double V1V1   = V1.SquareMagnitude();
  double NV1    = std::sqrt(V1V1);
  double V13    = V1V1 * NV1;
  double V15    = V13 * V1V1;

  if (V15 < gp::Resolution())
  {
    return false;
  }
  F = CPV1V3 / V13 - 3 * CPV1V2 * V1V2 / V15;

  return true;
}

//=================================================================================================

bool Geom2dLProp_FuncCurExt::Derivative(const double X, double& D)
{
  double F;
  return Values(X, F, D);
}

//=================================================================================================

bool Geom2dLProp_FuncCurExt::Values(const double X,
                                                double&      F,
                                                double&      D)
{
  double F2;
  double Dx = epsX / 100.;

  if (X + Dx > Geom2dLProp_Curve2dTool::LastParameter(theCurve))
  {
    Dx = -Dx;
  }

  Value(X, F);
  Value(X + Dx, F2);
  D = (F2 - F) / Dx;

  return true;
}

//=============================================================================
// function : IsMinKC
// purpose : Teste si le parametere coorespond a un minimum du rayon de courbure
//           par comparaison avec un point voisin.
//=============================================================================
bool Geom2dLProp_FuncCurExt::IsMinKC(const double X) const
{
  gp_Pnt2d      P1;
  gp_Vec2d      V1, V2, V3;
  double Dx = epsX;
  double KC, KP;

  Geom2dLProp_Curve2dTool::D3(theCurve, X, P1, V1, V2, V3);
  double CPV1V2 = V1.Crossed(V2);
  double V1V1   = V1.SquareMagnitude();
  double NV1    = std::sqrt(V1V1);
  double V13    = V1V1 * NV1;

  if (V13 < gp::Resolution())
  {
    return false;
  }

  KC = CPV1V2 / V13;

  if (X + Dx > Geom2dLProp_Curve2dTool::LastParameter(theCurve))
  {
    Dx = -Dx;
  }

  Geom2dLProp_Curve2dTool::D3(theCurve, X + Dx, P1, V1, V2, V3);
  CPV1V2 = V1.Crossed(V2);
  V1V1   = V1.SquareMagnitude();
  NV1    = std::sqrt(V1V1);
  V13    = V1V1 * NV1;

  if (V13 < gp::Resolution())
  {
    return false;
  }
  KP = CPV1V2 / V13;

  if (std::abs(KC) > std::abs(KP))
  {
    return true;
  }
  else
  {
    return false;
  }
}
