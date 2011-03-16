// File:	PTopoDS_TCompSolid1.cxx
// Created:	Tue Mar  9 14:04:23 1993
// Author:	Remi LEQUETTE
//		<rle@phobox>

#include <PTopoDS_TCompSolid1.ixx>

//=======================================================================
//function : PTopoDS_TCompSolid1
//purpose  : 
//=======================================================================

PTopoDS_TCompSolid1::PTopoDS_TCompSolid1() 
{
}



//=======================================================================
//function : ShapeType 
//purpose  : 
//=======================================================================

TopAbs_ShapeEnum PTopoDS_TCompSolid1::ShapeType() const
{
  return TopAbs_COMPSOLID;
}
