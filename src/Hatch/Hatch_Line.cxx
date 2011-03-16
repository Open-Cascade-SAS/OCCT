// File:	Hatch_Line.cxx
// Created:	Wed Aug 19 15:43:22 1992
// Author:	Modelistation
//		<model@phylox>

#include <Hatch_Line.ixx>
#include <Hatch_Parameter.hxx>

//=======================================================================
//function : Hatch_Line
//purpose  : 
//=======================================================================

Hatch_Line::Hatch_Line()
{
}

//=======================================================================
//function : Hatch_Line
//purpose  : 
//=======================================================================

Hatch_Line::Hatch_Line(const gp_Lin2d& L, 
		       const Hatch_LineForm T) :
       myLin(L),
       myForm(T)
{
}

//=======================================================================
//function : AddIntersection
//purpose  : 
//=======================================================================

void  Hatch_Line::AddIntersection
  (const Standard_Real Par1, 
   const Standard_Boolean Start,
   const Standard_Integer Index,
   const Standard_Real Par2,
   const Standard_Real theToler)
{
  Hatch_Parameter P(Par1,Start,Index,Par2);
  Standard_Integer i;
  for (i = 1; i <= myInters.Length(); i++) {
    Standard_Real dfIntPar1 = myInters(i).myPar1;
    // akm OCC109 vvv : Two intersections too close
    if (Abs(Par1-dfIntPar1) < theToler)
    {
      myInters.Remove(i);
      return;
    }
    // akm OCC109 ^^^
    if (Par1 < dfIntPar1) {
      myInters.InsertBefore(i,P);
      return;
    }
  }
  myInters.Append(P);
}
