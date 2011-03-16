// File:	BRepBuilderAPI_MakeShape.cxx
// Created:	Fri Jul 23 15:51:51 1993
// Author:	Remi LEQUETTE
//		<rle@nonox>


#include <BRepBuilderAPI_MakeShape.ixx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>


//=======================================================================
//function : BRepBuilderAPI_MakeShape
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeShape::BRepBuilderAPI_MakeShape()
{
}

void BRepBuilderAPI_MakeShape::Delete()
{}


//=======================================================================
//function : Build
//purpose  : 
//=======================================================================

void BRepBuilderAPI_MakeShape::Build()
{
}

//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

const TopoDS_Shape&  BRepBuilderAPI_MakeShape::Shape() const
{
  if (!IsDone()) {
    // the following is const cast away
    ((BRepBuilderAPI_MakeShape*) (void*) this)->Build();
    Check();
  }
  return myShape;
}


//=======================================================================
//function : operator
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeShape::operator TopoDS_Shape() const
{
  return Shape();
}


//=======================================================================
//function : Generated
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepBuilderAPI_MakeShape::Generated (const TopoDS_Shape& S) 

{
  myGenerated.Clear();
  return myGenerated;
}


//=======================================================================
//function : Modified
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepBuilderAPI_MakeShape::Modified (const TopoDS_Shape& S) 

{
  myGenerated.Clear();
  return myGenerated;
}


//=======================================================================
//function : IsDeleted
//purpose  : 
//=======================================================================

Standard_Boolean BRepBuilderAPI_MakeShape::IsDeleted (const TopoDS_Shape& S) 

{
  return Standard_False;
}




