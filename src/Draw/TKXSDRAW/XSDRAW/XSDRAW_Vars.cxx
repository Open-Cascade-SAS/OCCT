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

#include <DBRep.hxx>
#include <DrawTrSurf.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_Surface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <MoniTool_Macros.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TopoDS_Shape.hxx>
#include <XSDRAW_Vars.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XSDRAW_Vars, XSControl_Vars)

XSDRAW_Vars::XSDRAW_Vars() {}

void XSDRAW_Vars::Set(const char* name, const occ::handle<Standard_Transient>& val)
{
  // char* nam = name;
  // selon type
  DeclareAndCast(Geom_Geometry, geom, val);
  if (!geom.IsNull())
  {
    DrawTrSurf::Set(name, geom);
    return;
  }
  DeclareAndCast(Geom2d_Curve, g2d, val);
  if (!g2d.IsNull())
  {
    DrawTrSurf::Set(name, geom);
    return;
  }
  //  ??
}

/*
occ::handle<Standard_Transient>  XSDRAW_Vars::Get (const char* name) const
{
  occ::handle<Standard_Transient> val;
  if (!thevars->GetItem (name,val)) val.Nullify();
  return val;
}
*/

occ::handle<Geom_Geometry> XSDRAW_Vars::GetGeom(const char*& name) const
{ // char* nam = name;
  return DrawTrSurf::Get(name);
}

occ::handle<Geom2d_Curve> XSDRAW_Vars::GetCurve2d(const char*& name) const
{ // char* nam = name;
  return DrawTrSurf::GetCurve2d(name);
}

occ::handle<Geom_Curve> XSDRAW_Vars::GetCurve(const char*& name) const
{ // char* nam = name;
  return DrawTrSurf::GetCurve(name);
}

occ::handle<Geom_Surface> XSDRAW_Vars::GetSurface(const char*& name) const
{ // char* nam = name;
  return DrawTrSurf::GetSurface(name);
}

void XSDRAW_Vars::SetPoint(const char* name, const gp_Pnt& val)
{
  // char* nam = name;
  DrawTrSurf::Set(name, val);
}

bool XSDRAW_Vars::GetPoint(const char*& name, gp_Pnt& pnt) const
{ // char* nam = name;
  return DrawTrSurf::GetPoint(name, pnt);
}

void XSDRAW_Vars::SetPoint2d(const char* name, const gp_Pnt2d& val)
{
  // char* nam = name;
  DrawTrSurf::Set(name, val);
}

bool XSDRAW_Vars::GetPoint2d(const char*& name, gp_Pnt2d& pnt) const
{
  // char* nam = name;
  return DrawTrSurf::GetPoint2d(name, pnt);
}

void XSDRAW_Vars::SetShape(const char* name, const TopoDS_Shape& val)
{
  DBRep::Set(name, val);
}

TopoDS_Shape XSDRAW_Vars::GetShape(const char*& name) const
{
  // char* nam = name;
  return DBRep::Get(name);
}
