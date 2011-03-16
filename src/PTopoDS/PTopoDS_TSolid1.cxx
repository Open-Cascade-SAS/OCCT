// File:	PTopoDS_TSolid1.cxx
// Created:	Tue Mar  9 14:04:28 1993
// Author:	Remi LEQUETTE
//		<rle@phobox>

#include <PTopoDS_TSolid1.ixx>

//=======================================================================
//function : PTopoDS_TSolid1
//purpose  : 
//=======================================================================

PTopoDS_TSolid1::PTopoDS_TSolid1() 
{
}



//=======================================================================
//function : ShapeType 
//purpose  : 
//=======================================================================

TopAbs_ShapeEnum PTopoDS_TSolid1::ShapeType() const
{
  return TopAbs_SOLID;
}
