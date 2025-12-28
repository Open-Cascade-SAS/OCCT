// Created on: 1996-03-29
// Created by: Laurent BOURESCHE
// Copyright (c) 1996-1999 Matra Datavision
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

// pmn -> 17/01/1996 added : Continuity, (Nb)Interals, D2, Trim

#include <Law_Constant.hxx>
#include <Law_Function.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Law_Constant, Law_Function)

//=================================================================================================

Law_Constant::Law_Constant()
    : radius(0.0),
      first(0.0),
      last(0.0)
{
}

//=================================================================================================

void Law_Constant::Set(const double Radius, const double PFirst, const double PLast)
{
  radius = Radius;
  first  = PFirst;
  last   = PLast;
}

//=================================================================================================

GeomAbs_Shape Law_Constant::Continuity() const
{
  return GeomAbs_CN;
}

//=================================================================================================

// int Law_Constant::NbIntervals(const GeomAbs_Shape S) const
int Law_Constant::NbIntervals(const GeomAbs_Shape) const
{
  return 1;
}

//=================================================================================================

void Law_Constant::Intervals(NCollection_Array1<double>& T,
                             //                             const GeomAbs_Shape S) const
                             const GeomAbs_Shape) const
{
  T.SetValue(T.Lower(), first);
  T.SetValue(T.Upper(), last);
}

//=================================================================================================

double Law_Constant::Value(const double)
{
  return radius;
}

//=================================================================================================

void Law_Constant::D1(const double, double& F, double& D)
{
  F = radius;
  D = 0.;
}

//=================================================================================================

void Law_Constant::D2(const double, double& F, double& D, double& D2)
{
  F = radius;
  D = D2 = 0.;
}

//=================================================================================================

occ::handle<Law_Function> Law_Constant::Trim(const double PFirst,
                                             const double PLast,
                                             //				      const double Tol) const
                                             const double) const
{
  occ::handle<Law_Constant> l = new (Law_Constant)();
  l->Set(radius, PFirst, PLast);
  return l;
}

//=================================================================================================

void Law_Constant::Bounds(double& PFirst, double& PLast)
{
  PFirst = first;
  PLast  = last;
}
