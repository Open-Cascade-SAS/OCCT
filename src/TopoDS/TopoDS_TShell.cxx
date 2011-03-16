// File:	TopoDS_TShell.cxx
// Created:	Fri Apr 12 14:21:38 1991
// Author:	Remi LEQUETTE

#include <TopoDS_TShell.ixx>

#include <TopAbs.hxx>

//=======================================================================
//function : ShapeType
//purpose  : 
//=======================================================================

TopAbs_ShapeEnum TopoDS_TShell::ShapeType() const
{
  return TopAbs_SHELL;
}

//=======================================================================
//function : EmptyCopy
//purpose  : 
//=======================================================================

Handle(TopoDS_TShape) TopoDS_TShell::EmptyCopy() const
{
  return Handle(TopoDS_TShell)(new TopoDS_TShell());
}
