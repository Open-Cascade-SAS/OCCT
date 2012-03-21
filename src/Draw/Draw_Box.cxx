// Created on: 1995-03-10
// Created by: Remi LEQUETTE
// Copyright (c) 1995-1999 Matra Datavision
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



#include <Draw_Box.ixx>

//=======================================================================
//function : Draw_Box
//purpose  : 
//=======================================================================

Draw_Box::Draw_Box(const gp_Pnt& p1, const gp_Pnt& p2, const Draw_Color& col) :
   myFirst(p1), myLast(p2),myColor(col)
{
  Standard_Real t;
  if (myLast.X() < myFirst.X()) {
    t = myFirst.X();
    myFirst.SetX(myLast.X());
    myLast.SetX(t);
  }
  if (myLast.Y() < myFirst.Y()) {
    t = myFirst.Y();
    myFirst.SetY(myLast.Y());
    myLast.SetY(t);
  }
  if (myLast.Z() < myFirst.Z()) {
    t = myFirst.Z();
    myFirst.SetZ(myLast.Z());
    myLast.SetZ(t);
  }
}

//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void Draw_Box::DrawOn(Draw_Display& dis) const 
{
  dis.SetColor(myColor);
  gp_Pnt P = myFirst;

  dis.MoveTo(P);
  P.SetX(myLast.X());
  dis.DrawTo(P);
  P.SetY(myLast.Y());
  dis.DrawTo(P);
  P.SetZ(myLast.Z());
  dis.DrawTo(P);
  P.SetX(myFirst.X());
  dis.DrawTo(P);
  P.SetY(myFirst.Y());
  dis.DrawTo(P);
  P.SetZ(myFirst.Z());
  dis.DrawTo(P);

  P.SetX(myLast.X());
  dis.MoveTo(P);
  P.SetZ(myLast.Z());
  dis.DrawTo(P);
  P.SetX(myFirst.X());
  dis.DrawTo(P);
  
  P.SetX(myLast.X());
  dis.MoveTo(P);
  P.SetY(myLast.Y());
  dis.DrawTo(P);
  
  P.SetX(myFirst.X());
  dis.MoveTo(P);
  P.SetZ(myFirst.Z());
  dis.DrawTo(P);
  P.SetY(myFirst.Y());
  dis.DrawTo(P);
  
  P.SetY(myLast.Y());
  dis.MoveTo(P);
  P.SetX(myLast.X());
  dis.DrawTo(P);
}

//=======================================================================
//function : First
//purpose  : 
//=======================================================================

const gp_Pnt& Draw_Box::First() const 
{
  return myFirst;
}

//=======================================================================
//function : First
//purpose  : 
//=======================================================================

void Draw_Box::First(const gp_Pnt& P)
{
  myFirst = P;
}

//=======================================================================
//function : Last
//purpose  : 
//=======================================================================

const gp_Pnt& Draw_Box::Last() const 
{
  return myLast;
}

//=======================================================================
//function : Last
//purpose  : 
//=======================================================================

void Draw_Box::Last(const gp_Pnt& P)
{
  myLast = P;
}

