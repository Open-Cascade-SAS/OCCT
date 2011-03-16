// File:	PTopoDS_TVertex.cxx
// Created:	Tue Mar  9 14:04:29 1993
// Author:	Remi LEQUETTE
//		<rle@phobox>

#include <PTopoDS_TVertex.ixx>


//=======================================================================
//function : ShapeType 
//purpose  : 
//=======================================================================

TopAbs_ShapeEnum PTopoDS_TVertex::ShapeType() const
{
  return TopAbs_VERTEX;
}
