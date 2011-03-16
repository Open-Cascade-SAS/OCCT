// File:	HLRBRep_BiPnt2D.cxx
// Created:	Tue Aug 25 10:43:05 1992
// Author:	Christophe MARION
//		<cma@sdsun2>

#include <HLRBRep_BiPnt2D.ixx>

//=======================================================================
//function : HLRBRep_BiPnt2D
//purpose  : 
//=======================================================================

HLRBRep_BiPnt2D::HLRBRep_BiPnt2D ()
{
}

//=======================================================================
//function : HLRBRep_BiPnt2D
//purpose  : 
//=======================================================================

HLRBRep_BiPnt2D::HLRBRep_BiPnt2D (const Standard_Real x1,
				  const Standard_Real y1,
				  const Standard_Real x2,
				  const Standard_Real y2,
				  const TopoDS_Shape& S,
				  const Standard_Boolean reg1,
				  const Standard_Boolean regn,
				  const Standard_Boolean outl,
				  const Standard_Boolean intl) :
				  myP1(x1,y1),
				  myP2(x2,y2),
				  myShape(S),
				  myFlags(0)
{
  Rg1Line(reg1);
  RgNLine(regn);
  OutLine(outl);
  IntLine(intl);
}

