// File:	PTopoDS_TEdge1.cxx
// Created:	Tue Mar  9 14:04:25 1993
// Author:	Remi LEQUETTE
//		<rle@phobox>

#include <PTopoDS_TEdge1.ixx>

//=======================================================================
//function : ShapeType 
//purpose  : 
//=======================================================================

TopAbs_ShapeEnum PTopoDS_TEdge1::ShapeType() const
{
  return TopAbs_EDGE;
}
