// File:	PTopoDS_TFace.cxx
// Created:	Tue Mar  9 14:04:26 1993
// Author:	Remi LEQUETTE
//		<rle@phobox>

#include <PTopoDS_TFace.ixx>

//=======================================================================
//function : PTopoDS_TFace
//purpose  : 
//=======================================================================

PTopoDS_TFace::PTopoDS_TFace() 
{
}



//=======================================================================
//function : ShapeType 
//purpose  : 
//=======================================================================

TopAbs_ShapeEnum PTopoDS_TFace::ShapeType() const
{
  return TopAbs_FACE;
}
