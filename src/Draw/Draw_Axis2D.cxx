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


#include <Draw_Axis2D.ixx>
#include <gp_Vec2d.hxx>
#include <gp.hxx>

extern Standard_Boolean Draw_Bounds;

//=======================================================================
//function : Draw_Axis2D
//purpose  : 
//=======================================================================

Draw_Axis2D::Draw_Axis2D (const Draw_Color& col, 
			  const Standard_Integer Size) :
       myAxes(gp_Pnt2d(0,0),gp_Dir2d(1,0)),myColor(col), mySize(Size)
{
}

//=======================================================================
//function : Draw_Axis2D
//purpose  : 
//=======================================================================

Draw_Axis2D::Draw_Axis2D (const gp_Pnt2d& p, 
			  const Draw_Color& col, 
			  const Standard_Integer Size) :
       myAxes(p,gp_Dir2d(1,0)), myColor(col), mySize(Size)
{
}

//=======================================================================
//function : Draw_Axis2D
//purpose  : 
//=======================================================================

Draw_Axis2D::Draw_Axis2D (const gp_Ax22d& a, 
			  const Draw_Color& col, 
			  const Standard_Integer Size) :
       myAxes(a), myColor(col), mySize(Size)
{
}

//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void Draw_Axis2D::DrawOn (Draw_Display& dis) const
{
  Draw_Bounds = Standard_False;
  dis.SetColor(myColor);
  Standard_Real z = dis.Zoom();
  z = (Standard_Real)mySize / z;
  gp_Pnt2d P,P0 = myAxes.Location();
  P = P0.Translated(gp_Vec2d(myAxes.XDirection()) * z);
  dis.Draw(P0,P);
  dis.DrawString(P,"X");
  P = P0.Translated(gp_Vec2d(myAxes.YDirection()) * z);
  dis.Draw(P0,P);
  dis.DrawString(P,"Y");
  Draw_Bounds = Standard_True;
}

