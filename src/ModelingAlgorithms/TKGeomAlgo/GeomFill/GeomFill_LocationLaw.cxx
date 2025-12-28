// Created on: 1997-11-21
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

#include <GeomFill_LocationLaw.hxx>

#include <Adaptor3d_Curve.hxx>
#include <gp_Mat.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomFill_LocationLaw, Standard_Transient)

bool GeomFill_LocationLaw::D1(const double,
                              gp_Mat&,
                              gp_Vec&,
                              gp_Mat&,
                              gp_Vec&,
                              NCollection_Array1<gp_Pnt2d>&,
                              NCollection_Array1<gp_Vec2d>&)
{
  throw Standard_NotImplemented("GeomFill_LocationLaw::D1");
}

bool GeomFill_LocationLaw::D2(const double,
                              gp_Mat&,
                              gp_Vec&,
                              gp_Mat&,
                              gp_Vec&,
                              gp_Mat&,
                              gp_Vec&,
                              NCollection_Array1<gp_Pnt2d>&,
                              NCollection_Array1<gp_Vec2d>&,
                              NCollection_Array1<gp_Vec2d>&)
{
  throw Standard_NotImplemented("GeomFill_LocationLaw::D2");
}

int GeomFill_LocationLaw::Nb2dCurves() const
{
  int N = TraceNumber();
  if (HasFirstRestriction())
    N++;
  if (HasLastRestriction())
    N++;

  return N;
}

bool GeomFill_LocationLaw::HasFirstRestriction() const
{
  return false;
}

bool GeomFill_LocationLaw::HasLastRestriction() const
{
  return false;
}

int GeomFill_LocationLaw::TraceNumber() const
{
  return 0;
}

//=================================================================================================

GeomFill_PipeError GeomFill_LocationLaw::ErrorStatus() const
{
  return GeomFill_PipeOk;
}

// void GeomFill_LocationLaw::Resolution(const int Index,const double
// Tol,double& TolU,double& TolV) const
void GeomFill_LocationLaw::Resolution(const int, const double, double&, double&) const
{
  throw Standard_NotImplemented("GeomFill_LocationLaw::Resolution");
}

void GeomFill_LocationLaw::SetTolerance(const double, const double)
{
  // Ne fait rien !!
}

bool GeomFill_LocationLaw::IsTranslation(double&) const
{
  return false;
}

bool GeomFill_LocationLaw::IsRotation(double&) const
{
  return false;
}

void GeomFill_LocationLaw::Rotation(gp_Pnt&) const
{
  throw Standard_NotImplemented("GeomFill_SectionLaw::Rotation");
}
