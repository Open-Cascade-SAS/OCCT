// File:	StdSelect_FaceFilter.cxx
// Created:	Fri Mar  8 17:16:47 1996
// Author:	Robert COUBLANC
//		<rob@fidox>


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
