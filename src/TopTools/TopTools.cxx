// File:	TopTools.cxx
// Created:	Wed Jan 20 20:04:06 1993
// Author:	Remi LEQUETTE
//		<rle@phylox>


#include <TopTools.ixx>
#include <TopTools_ShapeSet.hxx>

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void  TopTools::Dump(const TopoDS_Shape& Sh, Standard_OStream& S)
{
  TopTools_ShapeSet SSet;
  SSet.Add(Sh);
  SSet.Dump(Sh,S);
  SSet.Dump(S);
}


void TopTools::Dummy(const Standard_Integer)
{
}
