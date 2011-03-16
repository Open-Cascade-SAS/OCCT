// File:	PTopoDS_TFace1.cxx
// Created:	Tue Mar  9 14:04:26 1993
// Author:	Remi LEQUETTE
//		<rle@phobox>

#include <PTopoDS_TFace1.ixx>

//=======================================================================
//function : PTopoDS_TFace1
//purpose  : 
//=======================================================================

PTopoDS_TFace1::PTopoDS_TFace1() 
{
}



//=======================================================================
//function : ShapeType 
//purpose  : 
//=======================================================================

TopAbs_ShapeEnum PTopoDS_TFace1::ShapeType() const
{
  return TopAbs_FACE;
}
