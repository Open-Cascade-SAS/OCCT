// File:	TopoDS_TSolid.cxx
// Created:	Fri Apr 12 14:50:32 1991
// Author:	Remi LEQUETTE

#include <TopoDS_TSolid.ixx>

#include <TopAbs.hxx>

//=======================================================================
//function : ShapeType
//purpose  : 
//=======================================================================

TopAbs_ShapeEnum TopoDS_TSolid::ShapeType() const
{
  return TopAbs_SOLID;
}

//=======================================================================
//function : EmptyCopy
//purpose  : 
//=======================================================================

Handle(TopoDS_TShape) TopoDS_TSolid::EmptyCopy() const
{
  return Handle(TopoDS_TSolid)(new TopoDS_TSolid());
}
