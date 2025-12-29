// Created on: 1992-01-20
// Created by: Remi GILET
// Copyright (c) 1992-1999 Matra Datavision
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

#include <ElCLib.hxx>
#include <Geom2dGcc_CurveTool.hxx>
#include <Geom2dGcc_FunctionTanCuCuCu.hxx>
#include <gp.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <math_Matrix.hxx>
#include <Standard_ConstructionError.hxx>

void Geom2dGcc_FunctionTanCuCuCu::InitDerivative(const math_Vector& X,
                                                 gp_Pnt2d&          Point1,
                                                 gp_Pnt2d&          Point2,
                                                 gp_Pnt2d&          Point3,
                                                 gp_Vec2d&          Tan1,
                                                 gp_Vec2d&          Tan2,
                                                 gp_Vec2d&          Tan3,
                                                 gp_Vec2d&          D21,
                                                 gp_Vec2d&          D22,
                                                 gp_Vec2d&          D23)
{
  switch (TheType)
  {
    case Geom2dGcc_CiCuCu: {
      ElCLib::D2(X(1), Circ1, Point1, Tan1, D21);
      Geom2dGcc_CurveTool::D2(Curv2, X(2), Point2, Tan2, D22);
      Geom2dGcc_CurveTool::D2(Curv3, X(3), Point3, Tan3, D23);
    }
    break;
    case Geom2dGcc_CiCiCu: {
      ElCLib::D2(X(1), Circ1, Point1, Tan1, D21);
      ElCLib::D2(X(2), Circ2, Point2, Tan2, D22);
      Geom2dGcc_CurveTool::D2(Curv3, X(3), Point3, Tan3, D23);
    }
    break;
    case Geom2dGcc_CiLiCu: {
      ElCLib::D2(X(1), Circ1, Point1, Tan1, D21);
      ElCLib::D1(X(2), Lin2, Point2, Tan2);
      D22 = gp_Vec2d(0., 0.);
      Geom2dGcc_CurveTool::D2(Curv3, X(3), Point3, Tan3, D23);
    }
    break;
    case Geom2dGcc_LiCuCu: {
      ElCLib::D1(X(1), Lin1, Point1, Tan1);
      D21 = gp_Vec2d(0., 0.);
      Geom2dGcc_CurveTool::D2(Curv2, X(2), Point2, Tan2, D22);
      Geom2dGcc_CurveTool::D2(Curv3, X(3), Point3, Tan3, D23);
    }
    break;
    case Geom2dGcc_LiLiCu: {
      ElCLib::D1(X(1), Lin1, Point1, Tan1);
      D21 = gp_Vec2d(0., 0.);
      ElCLib::D1(X(2), Lin2, Point2, Tan2);
      D22 = gp_Vec2d(0., 0.);
      Geom2dGcc_CurveTool::D2(Curv3, X(3), Point3, Tan3, D23);
    }
    break;
    case Geom2dGcc_CuCuCu: {
      Geom2dGcc_CurveTool::D2(Curv1, X(1), Point1, Tan1, D21);
      Geom2dGcc_CurveTool::D2(Curv2, X(2), Point2, Tan2, D22);
      Geom2dGcc_CurveTool::D2(Curv3, X(3), Point3, Tan3, D23);
    }
    break;
    default: {
      throw Standard_ConstructionError();
    }
  }
}

Geom2dGcc_FunctionTanCuCuCu::Geom2dGcc_FunctionTanCuCuCu(const Geom2dAdaptor_Curve& C1,
                                                         const Geom2dAdaptor_Curve& C2,
                                                         const Geom2dAdaptor_Curve& C3)
{
  Curv1   = C1;
  Curv2   = C2;
  Curv3   = C3;
  TheType = Geom2dGcc_CuCuCu;
}

Geom2dGcc_FunctionTanCuCuCu::Geom2dGcc_FunctionTanCuCuCu(const gp_Circ2d&           C1,
                                                         const Geom2dAdaptor_Curve& C2,
                                                         const Geom2dAdaptor_Curve& C3)
{
  Circ1   = C1;
  Curv2   = C2;
  Curv3   = C3;
  TheType = Geom2dGcc_CiCuCu;
}

Geom2dGcc_FunctionTanCuCuCu::Geom2dGcc_FunctionTanCuCuCu(const gp_Circ2d&           C1,
                                                         const gp_Circ2d&           C2,
                                                         const Geom2dAdaptor_Curve& C3)
{
  Circ1   = C1;
  Circ2   = C2;
  Curv3   = C3;
  TheType = Geom2dGcc_CiCiCu;
}

Geom2dGcc_FunctionTanCuCuCu::Geom2dGcc_FunctionTanCuCuCu(const gp_Circ2d&           C1,
                                                         const gp_Lin2d&            L2,
                                                         const Geom2dAdaptor_Curve& C3)
{
  Circ1   = C1;
  Lin2    = L2;
  Curv3   = C3;
  TheType = Geom2dGcc_CiLiCu;
}

Geom2dGcc_FunctionTanCuCuCu::Geom2dGcc_FunctionTanCuCuCu(const gp_Lin2d&            L1,
                                                         const gp_Lin2d&            L2,
                                                         const Geom2dAdaptor_Curve& C3)
{
  Lin1    = L1;
  Lin2    = L2;
  Curv3   = C3;
  TheType = Geom2dGcc_LiLiCu;
}

Geom2dGcc_FunctionTanCuCuCu::Geom2dGcc_FunctionTanCuCuCu(const gp_Lin2d&            L1,
                                                         const Geom2dAdaptor_Curve& C2,
                                                         const Geom2dAdaptor_Curve& C3)
{
  Lin1    = L1;
  Curv2   = C2;
  Curv3   = C3;
  TheType = Geom2dGcc_LiCuCu;
}

//==========================================================================

int Geom2dGcc_FunctionTanCuCuCu::NbVariables() const
{
  return 3;
}

int Geom2dGcc_FunctionTanCuCuCu::NbEquations() const
{
  return 3;
}

bool Geom2dGcc_FunctionTanCuCuCu::Value(const math_Vector& X, math_Vector& Fval)
{
  gp_Pnt2d Point1;
  gp_Pnt2d Point2;
  gp_Pnt2d Point3;
  gp_Vec2d Tan1;
  gp_Vec2d Tan2;
  gp_Vec2d Tan3;
  gp_Vec2d D21;
  gp_Vec2d D22;
  gp_Vec2d D23;
  InitDerivative(X, Point1, Point2, Point3, Tan1, Tan2, Tan3, D21, D22, D23);
  // pipj (normes) et PiPj (non Normes).
  gp_XY  P1P2(gp_Vec2d(Point1, Point2).XY());
  gp_XY  P2P3(gp_Vec2d(Point2, Point3).XY());
  gp_XY  P3P1(gp_Vec2d(Point3, Point1).XY());
  double NorP1P2 = P1P2.Modulus();
  double NorP2P3 = P2P3.Modulus();
  double NorP3P1 = P3P1.Modulus();
  gp_XY  p1p2, p2p3, p3p1;
  if (NorP1P2 >= gp::Resolution())
  {
    p1p2 = P1P2 / NorP1P2;
  }
  else
  {
    p1p2 = gp_XY(0., 0.);
  }
  if (NorP2P3 >= gp::Resolution())
  {
    p2p3 = P2P3 / NorP2P3;
  }
  else
  {
    p2p3 = gp_XY(0., 0.);
  }
  if (NorP3P1 >= gp::Resolution())
  {
    p3p1 = P3P1 / NorP3P1;
  }
  else
  {
    p3p1 = gp_XY(0., 0.);
  }
  // derivees premieres non normees Deriv1ui.
  gp_XY Deriv1u1(Tan1.XY());
  gp_XY Deriv1u2(Tan2.XY());
  gp_XY Deriv1u3(Tan3.XY());
  // normales aux courbes.
  double nnor1 = Deriv1u1.Modulus();
  double nnor2 = Deriv1u2.Modulus();
  double nnor3 = Deriv1u3.Modulus();
  gp_XY  Nor1(-Deriv1u1.Y(), Deriv1u1.X());
  gp_XY  Nor2(-Deriv1u2.Y(), Deriv1u2.X());
  gp_XY  Nor3(-Deriv1u3.Y(), Deriv1u3.X());
  gp_XY  nor1, nor2, nor3;
  if (nnor1 >= gp::Resolution())
  {
    nor1 = Nor1 / nnor1;
  }
  else
  {
    nor1 = gp_XY(0., 0.);
  }
  if (nnor2 >= gp::Resolution())
  {
    nor2 = Nor2 / nnor2;
  }
  else
  {
    nor2 = gp_XY(0., 0.);
  }
  if (nnor3 >= gp::Resolution())
  {
    nor3 = Nor3 / nnor3;
  }
  else
  {
    nor3 = gp_XY(0., 0.);
  }
  // determination des signes pour les produits scalaires.
  double   signe1 = 1.;
  double   signe2 = 1.;
  double   signe3 = 1.;
  gp_Pnt2d Pcenter(gp_XY(Point1.XY() + Point2.XY() + Point3.XY()) / 3.);
  gp_XY    fic1(Pcenter.XY() - Point1.XY());
  gp_XY    fic2(Pcenter.XY() - Point2.XY());
  gp_XY    fic3(Pcenter.XY() - Point3.XY());
  double   pscal11 = nor1.Dot(fic1);
  double   pscal22 = nor2.Dot(fic2);
  double   pscal33 = nor3.Dot(fic3);
  if (pscal11 <= 0.)
  {
    signe1 = -1;
  }
  if (pscal22 <= 0.)
  {
    signe2 = -1;
  }
  if (pscal33 <= 0.)
  {
    signe3 = -1;
  }
  // Fonctions Fui.
  // ==============
  Fval(1) = signe1 * nor1.Dot(p1p2) + signe2 * nor2.Dot(p1p2);
  Fval(2) = signe2 * nor2.Dot(p2p3) + signe3 * nor3.Dot(p2p3);
  Fval(3) = signe3 * nor3.Dot(p3p1) + signe1 * nor1.Dot(p3p1);
  return true;
}

bool Geom2dGcc_FunctionTanCuCuCu::Derivatives(const math_Vector&, math_Matrix&)
{
  return true;
}

bool Geom2dGcc_FunctionTanCuCuCu::Values(const math_Vector&, math_Vector&, math_Matrix&)
{
  return true;
}
