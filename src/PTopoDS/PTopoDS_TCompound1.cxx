// File:	PTopoDS_TCompound1.cxx
// Created:	Tue Mar  9 14:04:24 1993
// Author:	Remi LEQUETTE
//		<rle@phobox>

#include <PTopoDS_TCompound1.ixx>

//=======================================================================
//function : PTopoDS_TCompound1
//purpose  : 
//=======================================================================

PTopoDS_TCompound1::PTopoDS_TCompound1() 
{
}



//=======================================================================
//function : ShapeType 
//purpose  : 
//=======================================================================

TopAbs_ShapeEnum PTopoDS_TCompound1::ShapeType() const
{
  return TopAbs_COMPOUND;
}
