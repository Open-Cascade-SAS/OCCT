// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <XSControl_Vars.ixx>
#include <Geom_CartesianPoint.hxx>
#include <Geom2d_CartesianPoint.hxx>
#include <TopoDS_HShape.hxx>

#include <Interface_Macros.hxx>


XSControl_Vars::XSControl_Vars  ()
    {  thevars = new Dico_DictionaryOfTransient;  }

void  XSControl_Vars::Set
  (const Standard_CString name,
   const Handle(Standard_Transient)& val)
{
  thevars->SetItem (name,val);
}

Handle(Standard_Transient)  XSControl_Vars::Get ( Standard_CString& name) const
{
  Handle(Standard_Transient) val;
  if (!thevars->GetItem (name,val)) val.Nullify();
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
