// File:	PTopoDS_TWire.cxx
// Created:	Tue Mar  9 14:04:30 1993
// Author:	Remi LEQUETTE
//		<rle@phobox>

#include <PTopoDS_TWire.ixx>

//=======================================================================
//function : PTopoDS_TWire
//purpose  : 
//=======================================================================

PTopoDS_TWire::PTopoDS_TWire() 
{
}

//=======================================================================
//function : ShapeType 
//purpose  : 
//=======================================================================

TopAbs_ShapeEnum PTopoDS_TWire::ShapeType() const
{
  return TopAbs_WIRE;
}
