// Created on: 1995-10-19
// Created by: Laurent BOURESCHE
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

#include <GeomFill_BoundWithSurf.hxx>

#include <Adaptor2d_Curve2d.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <Adaptor3d_Surface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <Law.hxx>
#include <Law_BSpFunc.hxx>
#include <Law_Function.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomFill_BoundWithSurf, GeomFill_Boundary)

//=================================================================================================

GeomFill_BoundWithSurf::GeomFill_BoundWithSurf(const Adaptor3d_CurveOnSurface& CurveOnSurf,
                                               const double             Tol3d,
                                               const double             Tolang)
    : GeomFill_Boundary(Tol3d, Tolang),
      myConS(CurveOnSurf)
{
}

//=================================================================================================

gp_Pnt GeomFill_BoundWithSurf::Value(const double U) const
{
  double x = U;
  if (!myPar.IsNull())
    x = myPar->Value(U);
  return myConS.Value(x);
}

//=================================================================================================

void GeomFill_BoundWithSurf::D1(const double U, gp_Pnt& P, gp_Vec& V) const
{
  double x = U, dx = 1.;
  if (!myPar.IsNull())
    myPar->D1(U, x, dx);
  myConS.D1(x, P, V);
  V.Multiply(dx);
}

//=================================================================================================

bool GeomFill_BoundWithSurf::HasNormals() const
{
  return true;
}

//=================================================================================================

gp_Vec GeomFill_BoundWithSurf::Norm(const double U) const
{
  // voir s il ne faudrait pas utiliser LProp ou autre.
  if (!HasNormals())
    throw Standard_Failure("BoundWithSurf Norm : pas de contrainte");

  //  occ::handle<Adaptor3d_Surface>& S = myConS.GetSurface();
  //  occ::handle<Adaptor2d_Curve2d>& C2d = myConS.GetCurve();
  double x, y;
  double w = U;
  if (!myPar.IsNull())
    w = myPar->Value(U);
  myConS.GetCurve()->Value(w).Coord(x, y);
  gp_Pnt P;
  gp_Vec Su, Sv;
  myConS.GetSurface()->D1(x, y, P, Su, Sv);
  Su.Cross(Sv);
  Su.Normalize();
  return Su;
}

//=================================================================================================

void GeomFill_BoundWithSurf::D1Norm(const double U, gp_Vec& N, gp_Vec& DN) const
{
  if (!HasNormals())
    throw Standard_Failure("BoundWithSurf Norm : pas de contrainte");
  //  occ::handle<Adaptor3d_Surface>& S = myConS.GetSurface();
  //  occ::handle<Adaptor2d_Curve2d>& C2d = myConS.GetCurve();
  gp_Pnt2d      P2d;
  gp_Vec2d      V2d;
  double x, y, dx, dy;
  double w = U, dw = 1.;
  if (!myPar.IsNull())
    myPar->D1(U, w, dw);
  myConS.GetCurve()->D1(w, P2d, V2d);
  P2d.Coord(x, y);
  V2d.Multiply(dw);
  V2d.Coord(dx, dy);
  gp_Pnt P;
  gp_Vec Su, Sv, Suu, Suv, Svv;
  myConS.GetSurface()->D2(x, y, P, Su, Sv, Suu, Svv, Suv);
  N = Su.Crossed(Sv);
  N.Normalize();
  double nsuu = N.Dot(Suu), nsuv = N.Dot(Suv), nsvv = N.Dot(Svv);
  double susu = Su.Dot(Su), susv = Su.Dot(Sv), svsv = Sv.Dot(Sv);
  double deno = (susu * svsv) - (susv * susv);
  if (std::abs(deno) < 1.e-16)
  {
    // on embraye sur un calcul approche, c est mieux que rien!?!
    gp_Vec temp = Norm(U + 1.e-12);
    DN          = N.Multiplied(-1.);
    DN.Add(temp);
    DN.Multiply(1.e-12);
  }
  else
  {
    double a = (-nsuu * svsv + nsuv * susv) / deno;
    double b = (nsuu * susv - nsuv * susu) / deno;
    double c = (-nsuv * svsv + nsvv * susv) / deno;
    double d = (nsuv * susv - nsvv * susu) / deno;

    gp_Vec temp1 = Su.Multiplied(a);
    gp_Vec temp2 = Sv.Multiplied(b);
    temp1.Add(temp2);
    temp2        = Su.Multiplied(c);
    gp_Vec temp3 = Sv.Multiplied(d);
    temp2.Add(temp3);
    temp1.Multiply(dx);
    temp2.Multiply(dy);
    DN = temp1.Added(temp2);
  }
}

//=================================================================================================

void GeomFill_BoundWithSurf::Reparametrize(const double    First,
                                           const double    Last,
                                           const bool HasDF,
                                           const bool HasDL,
                                           const double    DF,
                                           const double    DL,
                                           const bool Rev)
{
  occ::handle<Law_BSpline> curve =
    Law::Reparametrize(myConS, First, Last, HasDF, HasDL, DF, DL, Rev, 30);
  myPar = new Law_BSpFunc();
  occ::down_cast<Law_BSpFunc>(myPar)->SetCurve(curve);
}

//=================================================================================================

void GeomFill_BoundWithSurf::Bounds(double& First, double& Last) const
{
  if (!myPar.IsNull())
    myPar->Bounds(First, Last);
  else
  {
    First = myConS.FirstParameter();
    Last  = myConS.LastParameter();
  }
}

//=================================================================================================

bool GeomFill_BoundWithSurf::IsDegenerated() const
{
  return false;
}
