// File:	PTopoDS_TCompSolid.cxx
// Created:	Tue Mar  9 14:04:23 1993
// Author:	Remi LEQUETTE
//		<rle@phobox>

#include <PTopoDS_TCompSolid.ixx>

//=======================================================================
//function : PTopoDS_TCompSolid
//purpose  : 
//=======================================================================

PTopoDS_TCompSolid::PTopoDS_TCompSolid() 
{
}



//=======================================================================
//function : ShapeType 
//purpose  : 
//=======================================================================

TopAbs_ShapeEnum PTopoDS_TCompSolid::ShapeType() const
{
  return TopAbs_COMPSOLID;
}
