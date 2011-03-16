// File:	BRepBuilderAPI.cxx
// Created:	Wed Oct 13 08:38:28 1999
// Author:	Atelier CAS2000
//		<cas@brunox.paris1.matra-dtv.fr>


#include <BRepBuilderAPI.ixx>

#include <BRepLib.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Compound.hxx>
#include <gp.hxx>


//=======================================================================
//function : Plane
//purpose  : 
//=======================================================================

void  BRepBuilderAPI::Plane(const Handle(Geom_Plane)& P)
{
  BRepLib::Plane(P);
}


//=======================================================================
//function : Plane
//purpose  : 
//=======================================================================

const Handle(Geom_Plane)&  BRepBuilderAPI::Plane()
{
  return BRepLib::Plane();
}


//=======================================================================
//function : Precision
//purpose  : 
//=======================================================================

void  BRepBuilderAPI::Precision(const Standard_Real P)
{
  BRepLib::Precision(P);
}


//=======================================================================
//function : Precision
//purpose  : 
//=======================================================================

Standard_Real  BRepBuilderAPI::Precision()
{
  return BRepLib::Precision();
}
