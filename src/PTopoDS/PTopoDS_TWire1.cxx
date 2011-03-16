// File:	PTopoDS_TWire1.cxx
// Created:	Tue Mar  9 14:04:30 1993
// Author:	Remi LEQUETTE
//		<rle@phobox>

#include <PTopoDS_TWire1.ixx>

//=======================================================================
//function : PTopoDS_TWire1
//purpose  : 
//=======================================================================

PTopoDS_TWire1::PTopoDS_TWire1() 
{
}

//=======================================================================
//function : ShapeType 
//purpose  : 
//=======================================================================

TopAbs_ShapeEnum PTopoDS_TWire1::ShapeType() const
{
  return TopAbs_WIRE;
}
