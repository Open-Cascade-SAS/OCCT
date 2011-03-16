// File:	TopoDS_TCompound.cxx
// Created:	Fri Apr 12 15:11:28 1991
// Author:	Remi LEQUETTE

#include <TopoDS_TCompound.ixx>

#include <TopAbs.hxx>

//=======================================================================
//function : ShapeType
//purpose  : 
//=======================================================================

TopAbs_ShapeEnum TopoDS_TCompound::ShapeType() const
{
  return TopAbs_COMPOUND;
}

//=======================================================================
//function : EmptyCopy
//purpose  : 
//=======================================================================

Handle(TopoDS_TShape) TopoDS_TCompound::EmptyCopy() const
{
  return Handle(TopoDS_TCompound)(new TopoDS_TCompound());
}
