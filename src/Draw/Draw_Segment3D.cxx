// Created on: 1991-04-25
// Created by: Arnaud BOUZY
// Copyright (c) 1991-1999 Matra Datavision
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


#include <Draw_Segment3D.ixx>

//=======================================================================
//function : Draw_Segment3D
//purpose  : 
//=======================================================================

Draw_Segment3D::Draw_Segment3D(const gp_Pnt& p1, 
			       const gp_Pnt& p2, 
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

void  Draw_Segment3D::DrawOn(Draw_Display& dis)const 
{
  dis.SetColor(myColor);
  dis.Draw(myFirst,myLast);
}


//=======================================================================
//function : First
//purpose  : 
//=======================================================================

const gp_Pnt&  Draw_Segment3D::First() const
{
  return myFirst;
}


//=======================================================================
//function : First
//purpose  : 
//=======================================================================

void  Draw_Segment3D::First(const gp_Pnt& P)
{
  myFirst = P;
}


//=======================================================================
//function : Last
//purpose  : 
//=======================================================================

const gp_Pnt&  Draw_Segment3D::Last() const
{
  return myLast;
}


//=======================================================================
//function : Last
//purpose  : 
//=======================================================================

void  Draw_Segment3D::Last(const gp_Pnt& P)
{
  myLast = P;
}


