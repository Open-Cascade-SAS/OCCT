// File:	Hatch_Parameter.cxx
// Created:	Wed Aug 19 11:10:07 1992
// Author:	Modelistation
//		<model@phylox>

#include <Hatch_Parameter.ixx>

//=======================================================================
//function : Hatch_Parameter
//purpose  : 
//=======================================================================

Hatch_Parameter::Hatch_Parameter()
{
}

//=======================================================================
//function : Hatch_Parameter
//purpose  : 
//=======================================================================

Hatch_Parameter::Hatch_Parameter
  (const Standard_Real    Par1,
   const Standard_Boolean Start,
   const Standard_Integer Index,
   const Standard_Real    Par2) :
       myPar1(Par1), myStart(Start), myIndex(Index), myPar2(Par2)
{
}
