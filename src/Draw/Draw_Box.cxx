// Created on: 1995-03-10
// Created by: Remi LEQUETTE
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <Draw_Box.hxx>
#include <Draw_Color.hxx>
#include <Draw_Display.hxx>
#include <gp_Pnt.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Draw_Box,Draw_Drawable3D)

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

