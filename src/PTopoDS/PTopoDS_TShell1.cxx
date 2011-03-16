// File:	PTopoDS_TShell1.cxx
// Created:	Tue Mar  9 14:04:27 1993
// Author:	Remi LEQUETTE
//		<rle@phobox>

#include <PTopoDS_TShell1.ixx>

//=======================================================================
//function : PTopoDS_TShell1
//purpose  : 
//=======================================================================

PTopoDS_TShell1::PTopoDS_TShell1() 
{
}


//=======================================================================
//function : ShapeType 
//purpose  : 
//=======================================================================

TopAbs_ShapeEnum PTopoDS_TShell1::ShapeType() const
{
  return TopAbs_SHELL;
}




