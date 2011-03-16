// File:	BRepPrimAPI_MakeOneAxis.cxx
// Created:	Fri Jul 23 15:51:49 1993
// Author:	Remi LEQUETTE
//		<rle@nonox>


#include <BRepPrimAPI_MakeOneAxis.ixx>
#include <BRepBuilderAPI.hxx>
#include <BRepPrim_OneAxis.hxx>
#include <TopoDS.hxx>

//=======================================================================
//function : Face
//purpose  : 
//=======================================================================

const TopoDS_Face&  BRepPrimAPI_MakeOneAxis::Face()
{
  Build();
  return ((BRepPrim_OneAxis*) OneAxis())->LateralFace();
}


//=======================================================================
//function : Shell
//purpose  : 
//=======================================================================

const TopoDS_Shell&  BRepPrimAPI_MakeOneAxis::Shell()
{
  Build();
  return ((BRepPrim_OneAxis*) OneAxis())->Shell();
}

//=======================================================================
//function : Build
//purpose  : 
//=======================================================================

void BRepPrimAPI_MakeOneAxis::Build()
{
  BRep_Builder B;
  B.MakeSolid(TopoDS::Solid(myShape));
  B.Add(myShape,((BRepPrim_OneAxis*) OneAxis())->Shell());
  myShape.Closed(Standard_True);
  Done();
}

//=======================================================================
//function : Solid
//purpose  : 
//=======================================================================

const TopoDS_Solid&  BRepPrimAPI_MakeOneAxis::Solid()
{
  Build();
  return TopoDS::Solid(myShape);
}



//=======================================================================
//function : operator
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeOneAxis::operator TopoDS_Face()
{
  return Face();
}

//=======================================================================
//function : operator
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeOneAxis::operator TopoDS_Shell()
{
  return Shell();
}


//=======================================================================
//function : operator
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeOneAxis::operator TopoDS_Solid()
{
  return Solid();
}

