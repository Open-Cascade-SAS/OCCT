// File:	TopoDS_Shape.cxx
// Created:	Wed Mar 20 11:33:26 1991
// Author:	Remi Lequette
 
#include <TopoDS_Shape.ixx>

//=======================================================================
//function : HashCode
//purpose  : 
//=======================================================================

Standard_Integer TopoDS_Shape::HashCode(const Standard_Integer Upper) const
{
  //PKV
  const Standard_Integer aI = (Standard_Integer) ptrdiff_t(myTShape.operator->());
  const Standard_Integer aHS = ::HashCode(aI,Upper);
  const Standard_Integer aHL = myLocation.HashCode(Upper);
  return (aHS^aHL)%Upper;
} 
