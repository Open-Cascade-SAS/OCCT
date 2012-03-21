// Created on: 1992-04-30
// Created by: Remi LEQUETTE
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



#include <Draw_Circle2D.ixx>

//=======================================================================
//function : Draw_Circle2D
//purpose  : 
//=======================================================================

Draw_Circle2D::Draw_Circle2D(const gp_Circ2d& C,
			     const Standard_Real A1, const Standard_Real A2,
			     const Draw_Color& col) :
       myCirc(C), myA1(A1), myA2(A2), myColor(col)
{
}

//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void Draw_Circle2D::DrawOn(Draw_Display& d) const
{
  d.SetColor(myColor);
  d.Draw(myCirc,myA1,myA2);
}
