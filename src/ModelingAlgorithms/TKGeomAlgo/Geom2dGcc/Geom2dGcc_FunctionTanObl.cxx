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

#include <Geom2dGcc_CurveTool.hxx>
#include <Geom2dGcc_FunctionTanObl.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>

Geom2dGcc_FunctionTanObl::Geom2dGcc_FunctionTanObl(const Geom2dAdaptor_Curve& C,
                                                   const gp_Dir2d&            Dir)
{
  TheCurv      = C;
  TheDirection = Dir;
}

bool Geom2dGcc_FunctionTanObl::Value(const double X, double& Fval)
{
  gp_Pnt2d Point;
  gp_Vec2d Vect;
  Geom2dGcc_CurveTool::D1(TheCurv, X, Point, Vect);
  double NormeD1 = Vect.Magnitude();
  Fval           = TheDirection.XY().Crossed(Vect.XY()) / NormeD1;
  return true;
}

bool Geom2dGcc_FunctionTanObl::Derivative(const double X, double& Deriv)
{
  gp_Pnt2d Point;
  gp_Vec2d Vec1;
  gp_Vec2d Vec2;
  Geom2dGcc_CurveTool::D2(TheCurv, X, Point, Vec1, Vec2);
  double NormeD1 = Vec1.Magnitude();
  Deriv          = TheDirection.XY().Crossed(Vec2.XY()) / NormeD1
          - Vec1.XY().Dot(Vec2.XY()) * TheDirection.XY().Crossed(Vec1.XY()) / NormeD1;
  return true;
}

bool Geom2dGcc_FunctionTanObl::Values(const double X, double& Fval, double& Deriv)
{
  gp_Pnt2d Point;
  gp_Vec2d Vec1;
  gp_Vec2d Vec2;
  Geom2dGcc_CurveTool::D2(TheCurv, X, Point, Vec1, Vec2);
  double NormeD1 = Vec1.Magnitude();
  Fval           = TheDirection.XY().Crossed(Vec1.XY()) / NormeD1;
  Deriv          = TheDirection.XY().Crossed(Vec2.XY()) / NormeD1
          - Vec1.XY().Dot(Vec2.XY()) * TheDirection.XY().Crossed(Vec1.XY()) / NormeD1;
  return true;
}
