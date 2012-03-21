// Created on: 1993-07-23
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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

