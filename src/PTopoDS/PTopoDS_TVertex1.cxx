// File:	PTopoDS_TVertex1.cxx
// Created:	Tue Mar  9 14:04:29 1993
// Author:	Remi LEQUETTE
//		<rle@phobox>

#include <PTopoDS_TVertex1.ixx>


//=======================================================================
//function : ShapeType 
//purpose  : 
//=======================================================================

TopAbs_ShapeEnum PTopoDS_TVertex1::ShapeType() const
{
  return TopAbs_VERTEX;
}
