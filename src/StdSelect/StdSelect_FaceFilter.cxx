// Created on: 1996-03-08
// Created by: Robert COUBLANC
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


#include <BRepAdaptor_Surface.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <Standard_Type.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <StdSelect_FaceFilter.hxx>
#include <TopAbs.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StdSelect_FaceFilter,SelectMgr_Filter)

/*#include <.hxx>
#include <.hxx>
#include <.hxx>
#include <.hxx>
#include <.hxx>
#include <.hxx>
*/
StdSelect_FaceFilter::
StdSelect_FaceFilter(const StdSelect_TypeOfFace aTypeOfFace):
mytype(aTypeOfFace){}

void StdSelect_FaceFilter::
SetType(const StdSelect_TypeOfFace aTypeOfFace)
{mytype = aTypeOfFace;}


//==================================================
// Function: 
// Purpose :
//==================================================
StdSelect_TypeOfFace StdSelect_FaceFilter::Type() const 
{
  return mytype;
}

Standard_Boolean StdSelect_FaceFilter::
IsOk(const Handle(SelectMgr_EntityOwner)& EO) const
{
  Handle(StdSelect_BRepOwner) aBO (Handle(StdSelect_BRepOwner)::DownCast(EO));
  if (aBO.IsNull())
    return Standard_False;

  const TopoDS_Shape& anobj = aBO->Shape();
  if(anobj.ShapeType()!= TopAbs_FACE)
    return Standard_False;  

  switch(mytype) {
  case StdSelect_AnyFace:
    return Standard_True;
  case StdSelect_Plane:
    {
      BRepAdaptor_Surface surf(TopoDS::Face(anobj));
      return (surf.GetType() == GeomAbs_Plane);
    }
  case StdSelect_Cylinder:
    {
      BRepAdaptor_Surface surf(TopoDS::Face(anobj));
      return (surf.GetType() == GeomAbs_Cylinder);
    }
  case StdSelect_Sphere: 
    {
      BRepAdaptor_Surface surf(TopoDS::Face(anobj));
      return (surf.GetType() == GeomAbs_Sphere);      
    }
  case StdSelect_Torus: 
    {
      BRepAdaptor_Surface surf(TopoDS::Face(anobj));
      return ( surf.GetType() == GeomAbs_Torus);      
    }
  case StdSelect_Revol: 
    {
      BRepAdaptor_Surface surf(TopoDS::Face(anobj));
      return ( surf.GetType() == GeomAbs_Cylinder || 
	       surf.GetType() == GeomAbs_Cone     ||
	       surf.GetType() == GeomAbs_Torus    
	       ||
	       surf.GetType() == GeomAbs_Sphere	  || 
	       surf.GetType() == GeomAbs_SurfaceOfRevolution 
	     ); 
    }
  case StdSelect_Cone: // waiting for the free cdl, it is used for Cone
    {
      BRepAdaptor_Surface surf(TopoDS::Face(anobj));
      return (surf.GetType() == GeomAbs_Cone);      
    }
  }
  return Standard_False;
}
Standard_Boolean StdSelect_FaceFilter::ActsOn(const TopAbs_ShapeEnum aStandardMode) const 
{return aStandardMode==TopAbs_FACE;}
