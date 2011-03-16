// File:	TopoDS_TFace.cxx
// Created:	Fri Apr 12 11:41:10 1991
// Author:	Remi LEQUETTE

#include <TopoDS_TFace.ixx>

#include <TopAbs.hxx>

//=======================================================================
//function : ShapeType
//purpose  : 
//=======================================================================

TopAbs_ShapeEnum TopoDS_TFace::ShapeType() const
{
  return TopAbs_FACE;
}

//=======================================================================
//function : EmptyCopy
//purpose  : 
//=======================================================================

Handle(TopoDS_TShape) TopoDS_TFace::EmptyCopy() const
{
  return Handle(TopoDS_TFace)(new TopoDS_TFace());
}
