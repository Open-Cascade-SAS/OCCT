// File:	PTopoDS_TEdge.cxx
// Created:	Tue Mar  9 14:04:25 1993
// Author:	Remi LEQUETTE
//		<rle@phobox>

#include <PTopoDS_TEdge.ixx>

//=======================================================================
//function : ShapeType 
//purpose  : 
//=======================================================================

TopAbs_ShapeEnum PTopoDS_TEdge::ShapeType() const
{
  return TopAbs_EDGE;
}
