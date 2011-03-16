// File:	PTopoDS_TCompound.cxx
// Created:	Tue Mar  9 14:04:24 1993
// Author:	Remi LEQUETTE
//		<rle@phobox>

#include <PTopoDS_TCompound.ixx>

//=======================================================================
//function : PTopoDS_TCompound
//purpose  : 
//=======================================================================

PTopoDS_TCompound::PTopoDS_TCompound() 
{
}



//=======================================================================
//function : ShapeType 
//purpose  : 
//=======================================================================

TopAbs_ShapeEnum PTopoDS_TCompound::ShapeType() const
{
  return TopAbs_COMPOUND;
}
