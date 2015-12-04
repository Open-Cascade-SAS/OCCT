// Created on: 1991-04-25
// Created by: Arnaud BOUZY
// Copyright (c) 1991-1999 Matra Datavision
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


#include <Draw_Color.hxx>
#include <Draw_Display.hxx>
#include <Draw_Segment2D.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Draw_Segment2D,Draw_Drawable2D)

//=======================================================================
//function : Draw_Segment2D
//purpose  : 
//=======================================================================
Draw_Segment2D::Draw_Segment2D(const gp_Pnt2d& p1, 
			       const gp_Pnt2d& p2, 
			       const Draw_Color& col) :
       myFirst(p1),
       myLast(p2),
       myColor(col)
{
}


//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void  Draw_Segment2D::DrawOn(Draw_Display& dis)const 
{
  dis.SetColor(myColor);
  dis.Draw(myFirst,myLast);
}


//=======================================================================
//function : First
//purpose  : 
//=======================================================================

const gp_Pnt2d&  Draw_Segment2D::First() const
{
  return myFirst;
}


//=======================================================================
//function : First
//purpose  : 
//=======================================================================

void  Draw_Segment2D::First(const gp_Pnt2d& P)
{
  myFirst = P;
}


//=======================================================================
//function : Last
//purpose  : 
//=======================================================================

const gp_Pnt2d&  Draw_Segment2D::Last() const
{
  return myLast;
}


//=======================================================================
//function : Last
//purpose  : 
//=======================================================================

void  Draw_Segment2D::Last(const gp_Pnt2d& P)
{
  myLast = P;
}

//=======================================================================
//function : Whatis
//purpose  : 
//=======================================================================

void  Draw_Segment2D::Whatis(Draw_Interpretor& S) const
{
  S << "segment 2d";
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void  Draw_Segment2D::Dump(Standard_OStream& S) const
{
  S << setw(17) << myFirst.X() << " " << setw(17) << myFirst.Y() << " - "
    << setw(17) << myLast.X() << " " << setw(17) << myLast.Y() << "\n";
}
