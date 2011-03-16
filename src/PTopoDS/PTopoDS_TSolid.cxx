// File:	PTopoDS_TSolid.cxx
// Created:	Tue Mar  9 14:04:28 1993
// Author:	Remi LEQUETTE
//		<rle@phobox>

#include <PTopoDS_TSolid.ixx>

//=======================================================================
//function : PTopoDS_TSolid
//purpose  : 
//=======================================================================

PTopoDS_TSolid::PTopoDS_TSolid() 
{
}



//=======================================================================
//function : ShapeType 
//purpose  : 
//=======================================================================

TopAbs_ShapeEnum PTopoDS_TSolid::ShapeType() const
{
  return TopAbs_SOLID;
}
