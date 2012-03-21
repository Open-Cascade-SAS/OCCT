// Created on: 1993-06-09
// Created by: Laurent BOURESCHE
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



#include <BRepSweep_Tool.ixx>
#include <TopExp.hxx>

//=======================================================================
//function : BRepSweep_Tool
//purpose  : 
//=======================================================================

BRepSweep_Tool::BRepSweep_Tool(const TopoDS_Shape& aShape)
{
 TopExp::MapShapes(aShape,myMap);
}


//=======================================================================
//function : NbShapes
//purpose  : 
//=======================================================================

Standard_Integer  BRepSweep_Tool::NbShapes()const 
{
  return myMap.Extent();
}


//=======================================================================
//function : Index
//purpose  : 
//=======================================================================

Standard_Integer  BRepSweep_Tool::Index(const TopoDS_Shape& aShape)const 
{
  if(!myMap.Contains(aShape)) return 0;
  return myMap.FindIndex(aShape);
}


//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

TopoDS_Shape BRepSweep_Tool::Shape(const Standard_Integer anIndex)const 
{
  return myMap.FindKey(anIndex);
}


//=======================================================================
//function : Type
//purpose  : 
//=======================================================================

TopAbs_ShapeEnum  BRepSweep_Tool::Type(const TopoDS_Shape& aShape)const 
{
  return aShape.ShapeType();
}


//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================

TopAbs_Orientation  BRepSweep_Tool::Orientation
                                     (const TopoDS_Shape& aShape)const 
{
  return aShape.Orientation();
}


//=======================================================================
//function : SetOrientation
//purpose  : 
//=======================================================================

void BRepSweep_Tool::SetOrientation (TopoDS_Shape& aShape, 
				     const TopAbs_Orientation Or)const 
{
  aShape.Orientation(Or);
}
