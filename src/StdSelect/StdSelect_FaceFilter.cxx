// Created on: 1996-03-08
// Created by: Robert COUBLANC
// Copyright (c) 1996-1999 Matra Datavision
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



#define BUC60576	//GG_5/10/99 Adds Cone to enum TypeOfFace


#include <StdSelect_FaceFilter.ixx>
#include <BRepAdaptor_Surface.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>
#include <TopAbs.hxx>
#include <StdSelect_BRepOwner.hxx>
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
  if (Handle(StdSelect_BRepOwner)::DownCast(EO).IsNull()) return Standard_False;
  const TopoDS_Shape& anobj= ((Handle(StdSelect_BRepOwner)&)EO)->Shape();
  if(anobj.ShapeType()!= TopAbs_FACE)return Standard_False;  
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
#ifdef BUC60576
      return (surf.GetType() == GeomAbs_Sphere);      
#else
      return (surf.GetType() == GeomAbs_Cone);      
#endif
    }
  case StdSelect_Torus: 
    {
      BRepAdaptor_Surface surf(TopoDS::Face(anobj));
#ifdef BUC60576
      return ( surf.GetType() == GeomAbs_Torus);      
#else
      return ( surf.GetType() == GeomAbs_Plane || surf.GetType() == GeomAbs_Cone);      
#endif
    }
  case StdSelect_Revol: 
    {
      BRepAdaptor_Surface surf(TopoDS::Face(anobj));
      return ( surf.GetType() == GeomAbs_Cylinder || 
	       surf.GetType() == GeomAbs_Cone     ||
	       surf.GetType() == GeomAbs_Torus    
#ifdef BUC60576
	       ||
	       surf.GetType() == GeomAbs_Sphere	  || 
	       surf.GetType() == GeomAbs_SurfaceOfRevolution 
#endif
	     ); 
    }
#ifdef BUC60576
  case StdSelect_Cone: // waiting for the free cdl, it is used for Cone
    {
      BRepAdaptor_Surface surf(TopoDS::Face(anobj));
      return (surf.GetType() == GeomAbs_Cone);      
    }
#endif
  }
  return Standard_False;
}
Standard_Boolean StdSelect_FaceFilter::ActsOn(const TopAbs_ShapeEnum aStandardMode) const 
{return aStandardMode==TopAbs_FACE;}
