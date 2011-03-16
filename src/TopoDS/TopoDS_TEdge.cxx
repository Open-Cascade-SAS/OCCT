// File:	TopoDS_TEdge.cxx
// Created:	Thu Apr 11 16:21:48 1991
// Author:	Remi LEQUETTE

#include <TopoDS_TEdge.ixx>

#include <TopAbs.hxx>

//=======================================================================
//function : ShapeType
//purpose  : 
//=======================================================================

TopAbs_ShapeEnum TopoDS_TEdge::ShapeType() const
{
  return TopAbs_EDGE;
}
