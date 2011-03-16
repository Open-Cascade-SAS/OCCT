// File:	PTopoDS_TShell.cxx
// Created:	Tue Mar  9 14:04:27 1993
// Author:	Remi LEQUETTE
//		<rle@phobox>

#include <PTopoDS_TShell.ixx>

//=======================================================================
//function : PTopoDS_TShell
//purpose  : 
//=======================================================================

PTopoDS_TShell::PTopoDS_TShell() 
{
}


//=======================================================================
//function : ShapeType 
//purpose  : 
//=======================================================================

TopAbs_ShapeEnum PTopoDS_TShell::ShapeType() const
{
  return TopAbs_SHELL;
}




