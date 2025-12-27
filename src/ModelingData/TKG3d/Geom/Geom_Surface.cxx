// Created on: 1993-03-10
// Created by: JCV
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

#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_UndefinedDerivative.hxx>
#include <Geom_UndefinedValue.hxx>
#include <gp_GTrsf2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_Surface, Geom_Geometry)

typedef Geom_Surface Surface;

//=================================================================================================

occ::handle<Geom_Surface> Geom_Surface::UReversed() const
{
  occ::handle<Geom_Surface> S = occ::down_cast<Geom_Surface>(Copy());
  S->UReverse();
  return S;
}

//=================================================================================================

occ::handle<Geom_Surface> Geom_Surface::VReversed() const
{
  occ::handle<Geom_Surface> S = occ::down_cast<Geom_Surface>(Copy());
  S->VReverse();
  return S;
}

//=================================================================================================

void Geom_Surface::TransformParameters(double&, double&, const gp_Trsf&) const {}

//=================================================================================================

gp_GTrsf2d Geom_Surface::ParametricTransformation(const gp_Trsf&) const
{
  gp_GTrsf2d dummy;
  return dummy;
}

//=================================================================================================

double Geom_Surface::UPeriod() const
{
  Standard_NoSuchObject_Raise_if(!IsUPeriodic(), "Geom_Surface::UPeriod");

  double U1, U2, V1, V2;
  Bounds(U1, U2, V1, V2);
  return (U2 - U1);
}

//=================================================================================================

double Geom_Surface::VPeriod() const
{
  Standard_NoSuchObject_Raise_if(!IsVPeriodic(), "Geom_Surface::VPeriod");

  double U1, U2, V1, V2;
  Bounds(U1, U2, V1, V2);
  return (V2 - V1);
}

//=================================================================================================

gp_Pnt Geom_Surface::Value(const double U, const double V) const
{
  gp_Pnt P;
  D0(U, V, P);
  return P;
}

//=================================================================================================

void Geom_Surface::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_Geometry)
}
