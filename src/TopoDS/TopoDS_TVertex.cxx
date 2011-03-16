// File:	TopoDS_TVertex.cxx
// Created:	Wed Apr 10 09:42:42 1991
// Author:	Remi LEQUETTE

#include <TopoDS_TVertex.ixx>

#include <TopAbs.hxx>

//=======================================================================
//function : ShapeType
//purpose  : 
//=======================================================================

TopAbs_ShapeEnum TopoDS_TVertex::ShapeType() const
{
  return TopAbs_VERTEX;
}
