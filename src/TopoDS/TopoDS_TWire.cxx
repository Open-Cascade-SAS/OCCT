// File:	TopoDS_TWire.cxx
// Created:	Fri Apr 12 10:23:34 1991
// Author:	Remi LEQUETTE

#include <TopoDS_TWire.ixx>

#include <TopAbs.hxx>

//=======================================================================
//function : ShapeType
//purpose  : 
//=======================================================================

TopAbs_ShapeEnum TopoDS_TWire::ShapeType() const
{
  return TopAbs_WIRE;
}

//=======================================================================
//function : EmptyCopy
//purpose  : 
//=======================================================================

Handle(TopoDS_TShape) TopoDS_TWire::EmptyCopy() const
{
  return Handle(TopoDS_TWire)(new TopoDS_TWire());
}
