// Created on: 1997-12-05
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

#include <GeomFill_TrihedronLaw.hxx>
#include <gp_Vec.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomFill_TrihedronLaw, Standard_Transient)

bool GeomFill_TrihedronLaw::SetCurve(const occ::handle<Adaptor3d_Curve>& C)
{
  myCurve   = C;
  myTrimmed = myCurve;
  return true;
}

//=================================================================================================

GeomFill_PipeError GeomFill_TrihedronLaw::ErrorStatus() const
{
  return GeomFill_PipeOk;
}

bool GeomFill_TrihedronLaw::D1(const double,
                                           gp_Vec&,
                                           gp_Vec&,
                                           gp_Vec&,
                                           gp_Vec&,
                                           gp_Vec&,
                                           gp_Vec&)
{
  throw Standard_NotImplemented(" GeomFill_TrihedronLaw::D2");
}

bool GeomFill_TrihedronLaw::D2(const double,
                                           gp_Vec&,
                                           gp_Vec&,
                                           gp_Vec&,
                                           gp_Vec&,
                                           gp_Vec&,
                                           gp_Vec&,
                                           gp_Vec&,
                                           gp_Vec&,
                                           gp_Vec&)
{
  throw Standard_NotImplemented(" GeomFill_TrihedronLaw::D2");
}

void GeomFill_TrihedronLaw::SetInterval(const double First, const double Last)
{
  myTrimmed = myCurve->Trim(First, Last, 0);
}

void GeomFill_TrihedronLaw::GetInterval(double& First, double& Last)
{
  First = myTrimmed->FirstParameter();
  Last  = myTrimmed->LastParameter();
}

bool GeomFill_TrihedronLaw::IsConstant() const
{
  return false;
}

bool GeomFill_TrihedronLaw::IsOnlyBy3dCurve() const
{
  return false;
}
