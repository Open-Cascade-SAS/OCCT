// Created on: 1992-04-29
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



#include <Draw_Text2D.ixx>


//=======================================================================
//function : Draw_Text2D
//purpose  : 
//=======================================================================

Draw_Text2D::Draw_Text2D(const gp_Pnt2d& p, const Standard_CString T, 
			 const Draw_Color& col) :
       myPoint(p), myColor(col), myText(T), mymoveX(0), mymoveY(0)
{
}

//=======================================================================
//function : Draw_Text2D
//purpose  : 
//=======================================================================

Draw_Text2D::Draw_Text2D(const gp_Pnt2d& p, const Standard_CString T, 
			 const Draw_Color& col, const Standard_Integer moveX, const Standard_Integer moveY) :
       myPoint(p), myColor(col), myText(T), mymoveX(moveX), mymoveY(moveY)
{
}

//=======================================================================
//function : SetPnt2d
//purpose  : 
//=======================================================================

void Draw_Text2D::SetPnt2d(const gp_Pnt2d& p)
{
  myPoint = p;
}

//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void Draw_Text2D::DrawOn(Draw_Display& dis) const
{
  dis.SetColor(myColor);
  dis.DrawString(myPoint,myText.ToCString(),
		 (Standard_Real)mymoveX, (Standard_Real)mymoveY);
}
