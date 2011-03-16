// File:	BRepBuilderAPI_NurbsConvert.cxx
// Created:	Fri Dec  9 09:14:55 1994
// Author:	Jacques GOUSSARD
//		<jag@topsn2>


#include <BRepBuilderAPI_NurbsConvert.ixx>
#include <BRepLib.hxx>
#include <TopAbs.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools_NurbsConvertModification.hxx>
//#include <gp.hxx>


//=======================================================================
//function : BRepBuilderAPI_NurbsConvert
//purpose  : 
//=======================================================================

BRepBuilderAPI_NurbsConvert::BRepBuilderAPI_NurbsConvert () 
     
{
  myModification = new BRepTools_NurbsConvertModification();
}

//=======================================================================
//function : BRepBuilderAPI_NurbsConvert
//purpose  : 
//=======================================================================

BRepBuilderAPI_NurbsConvert::BRepBuilderAPI_NurbsConvert (const TopoDS_Shape& S,
					    const Standard_Boolean Copy) 
     
{
  myModification = new BRepTools_NurbsConvertModification();
  Perform(S,Copy);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepBuilderAPI_NurbsConvert::Perform(const TopoDS_Shape& S,
				   const Standard_Boolean Copy)
{
  Handle(BRepTools_NurbsConvertModification) theModif = 
    Handle(BRepTools_NurbsConvertModification)::DownCast(myModification);
  DoModif(S,myModification);
}



