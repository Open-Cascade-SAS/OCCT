// Created on: 1995-11-03
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

#include <Adaptor3d_Curve.hxx>
#include <GeomFill_SimpleBound.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Law.hxx>
#include <Law_BSpFunc.hxx>
#include <Law_Function.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomFill_SimpleBound, GeomFill_Boundary)

//=================================================================================================

GeomFill_SimpleBound::GeomFill_SimpleBound(const occ::handle<Adaptor3d_Curve>& Curve,
                                           const double            Tol3d,
                                           const double            Tolang)
    : GeomFill_Boundary(Tol3d, Tolang),
      myC3d(Curve)
{
}

//=================================================================================================

gp_Pnt GeomFill_SimpleBound::Value(const double U) const
{
  double x = U;
  if (!myPar.IsNull())
    x = myPar->Value(U);
  return myC3d->Value(x);
}

//=================================================================================================

void GeomFill_SimpleBound::D1(const double U, gp_Pnt& P, gp_Vec& V) const
{
  double x = U, dx = 1.;
  if (!myPar.IsNull())
    myPar->D1(U, x, dx);
  myC3d->D1(x, P, V);
  V.Multiply(dx);
}

//=================================================================================================

void GeomFill_SimpleBound::Reparametrize(const double    First,
                                         const double    Last,
                                         const bool HasDF,
                                         const bool HasDL,
                                         const double    DF,
                                         const double    DL,
                                         const bool Rev)
{
  occ::handle<Law_BSpline> curve =
    Law::Reparametrize(*myC3d, First, Last, HasDF, HasDL, DF, DL, Rev, 30);
  myPar = new Law_BSpFunc();
  occ::down_cast<Law_BSpFunc>(myPar)->SetCurve(curve);
}

//=================================================================================================

void GeomFill_SimpleBound::Bounds(double& First, double& Last) const
{
  if (!myPar.IsNull())
    myPar->Bounds(First, Last);
  else
  {
    First = myC3d->FirstParameter();
    Last  = myC3d->LastParameter();
  }
}

//=================================================================================================

bool GeomFill_SimpleBound::IsDegenerated() const
{
  return false;
}
