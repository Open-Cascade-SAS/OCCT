// Created on: 1992-08-19
// Created by: Modelistation
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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
