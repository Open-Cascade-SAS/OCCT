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


#include <Geom2d_CartesianPoint.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_Surface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Interface_Macros.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TopoDS_HShape.hxx>
#include <TopoDS_Shape.hxx>
#include <XSControl_Vars.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XSControl_Vars,Standard_Transient)

XSControl_Vars::XSControl_Vars() { }

void  XSControl_Vars::Set
  (const Standard_CString name,
   const Handle(Standard_Transient)& val)
{
  thevars.Bind(name,val);
}

Handle(Standard_Transient)  XSControl_Vars::Get ( Standard_CString& name) const
{
  Handle(Standard_Transient) val;
  if (!thevars.Find(name, val))
    val.Nullify();
  return val;
}

Handle(Geom_Geometry)  XSControl_Vars::GetGeom ( Standard_CString& name) const
{  return GetCasted(Geom_Geometry,Get(name));  }

Handle(Geom2d_Curve)  XSControl_Vars::GetCurve2d ( Standard_CString& name) const
{  return GetCasted(Geom2d_Curve,Get(name));  }

Handle(Geom_Curve)  XSControl_Vars::GetCurve ( Standard_CString& name) const
{  return GetCasted(Geom_Curve,Get(name));  }

Handle(Geom_Surface)  XSControl_Vars::GetSurface ( Standard_CString& name) const
{  return GetCasted(Geom_Surface,Get(name));  }


void  XSControl_Vars::SetPoint (const Standard_CString name, const gp_Pnt& val)
{  Set (name, new Geom_CartesianPoint(val));  }

Standard_Boolean  XSControl_Vars::GetPoint (Standard_CString& name, gp_Pnt& pnt) const
{
  DeclareAndCast(Geom_CartesianPoint,val,Get(name));
  if (val.IsNull()) return Standard_False;
  pnt = val->Pnt();
  return Standard_True;
}


void  XSControl_Vars::SetPoint2d (const Standard_CString name, const gp_Pnt2d& val)
{  Set (name, new Geom2d_CartesianPoint(val));  }

Standard_Boolean  XSControl_Vars::GetPoint2d (Standard_CString& name, gp_Pnt2d& pnt) const
{
  DeclareAndCast(Geom2d_CartesianPoint,val,Get(name));
  if (val.IsNull()) return Standard_False;
  pnt = val->Pnt2d();
  return Standard_True;
}


void  XSControl_Vars::SetShape (const Standard_CString name, const TopoDS_Shape& val)
{  Set (name, new TopoDS_HShape(val));  }

TopoDS_Shape  XSControl_Vars::GetShape (Standard_CString& name) const
{
  TopoDS_Shape sh;
  DeclareAndCast(TopoDS_HShape,val,Get(name));
  if (!val.IsNull()) sh = val->Shape();
  return sh;
}
