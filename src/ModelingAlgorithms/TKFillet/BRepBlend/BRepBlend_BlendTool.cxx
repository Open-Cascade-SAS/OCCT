// Created on: 1993-12-06
// Created by: Jacques GOUSSARD
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

#include <BRepBlend_BlendTool.hxx>

#include <Adaptor2d_Curve2d.hxx>
#include <Adaptor3d_Surface.hxx>
#include <BRep_Tool.hxx>
#include <BRepBlend_HCurve2dTool.hxx>
#include <Extrema_EPCOfExtPC2d.hxx>
#include <Extrema_POnCurv2d.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <IntRes2d_IntersectionPoint.hxx>

//=======================================================================
// function : Project
// purpose  : Projection orthogonal d'un point sur une courbe
// pmn 8/10/98 : On retourne toujours une distance. (BUC60360)
//=======================================================================
bool BRepBlend_BlendTool::Project(const gp_Pnt2d& P,
                                              const occ::handle<Adaptor3d_Surface>&,
                                              const occ::handle<Adaptor2d_Curve2d>& C,
                                              double&                   Paramproj,
                                              double&                   Dist)
{
  Paramproj = BRepBlend_HCurve2dTool::FirstParameter(C);
  gp_Pnt2d P2d;
  BRepBlend_HCurve2dTool::D0(C, Paramproj, P2d);
  Dist = P2d.Distance(P);

  const double t = BRepBlend_HCurve2dTool::LastParameter(C);
  BRepBlend_HCurve2dTool::D0(C, t, P2d);
  if (P2d.Distance(P) < Dist)
  {
    Paramproj = t;
    Dist      = P2d.Distance(P);
  }

  const double    epsX = 1.e-8;
  const int Nbu  = 20;
  const double    Tol  = 1.e-5;
  Extrema_EPCOfExtPC2d   extrema(P, *C, Nbu, epsX, Tol);
  if (!extrema.IsDone())
    return true;

  const int Nbext  = extrema.NbExt();
  double          aDist2 = Dist * Dist;
  for (int i = 1; i <= Nbext; i++)
  {
    if (extrema.SquareDistance(i) < aDist2)
    {
      aDist2    = extrema.SquareDistance(i);
      Paramproj = extrema.Point(i).Parameter();
    }
  }
  Dist = sqrt(aDist2);

  return true;
}

//=======================================================================
// function : Inters
// purpose  : Intersection d'un segment avec une courbe
//=======================================================================
bool BRepBlend_BlendTool::Inters(const gp_Pnt2d& P1,
                                             const gp_Pnt2d& P2,
                                             const occ::handle<Adaptor3d_Surface>&,
                                             const occ::handle<Adaptor2d_Curve2d>& C,
                                             double&                   Param,
                                             double&                   Dist)
{
  const double Tol = 1.e-8;
  const gp_Vec2d      v(P1, P2);
  const double mag = v.Magnitude();
  if (mag < Tol)
    return false;

  gp_Dir2d            d(v);
  occ::handle<Geom2d_Line> bid = new Geom2d_Line(P1, d);
  Geom2dAdaptor_Curve seg(bid, -0.01 * mag, 1.01 * mag);

  Geom2dInt_GInter inter(seg, *C, Tol, Tol);
  if (!inter.IsDone())
    return false;

  const int Nbint = inter.NbPoints();
  if (Nbint == 0)
    return false;

  IntRes2d_IntersectionPoint ip = inter.Point(1);
  Param                         = ip.ParamOnSecond();
  Dist                          = P1.Distance(ip.Value());
  return true;
}

int BRepBlend_BlendTool::NbSamplesV(const occ::handle<Adaptor3d_Surface>&,
                                                 const double,
                                                 const double)
{
  return 10;
}

int BRepBlend_BlendTool::NbSamplesU(const occ::handle<Adaptor3d_Surface>&,
                                                 const double,
                                                 const double)
{
  return 10;
}

void BRepBlend_BlendTool::Bounds(const occ::handle<Adaptor2d_Curve2d>& A,
                                 double&                   Ufirst,
                                 double&                   Ulast)
{
  Ufirst = BRepBlend_HCurve2dTool::FirstParameter(A);
  Ulast  = BRepBlend_HCurve2dTool::LastParameter(A);
}
