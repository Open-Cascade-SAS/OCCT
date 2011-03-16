// File:	TopoDS_TCompSolid.cxx
// Created:	Fri Apr 12 15:00:10 1991
// Author:	Remi LEQUETTE

#include <TopoDS_TCompSolid.ixx>

#include <TopAbs.hxx>

//=======================================================================
//function : ShapeType
//purpose  : 
//=======================================================================

TopAbs_ShapeEnum TopoDS_TCompSolid::ShapeType() const
{
  return TopAbs_COMPSOLID;
}

//=======================================================================
//function : EmptyCopy
//purpose  : 
//=======================================================================

Handle(TopoDS_TShape) TopoDS_TCompSolid::EmptyCopy() const
{
  return Handle(TopoDS_TCompSolid)(new TopoDS_TCompSolid());
}
