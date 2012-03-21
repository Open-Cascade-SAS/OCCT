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


#include <Draw_Axis3D.ixx>
#include <gp_Vec.hxx>
#include <gp.hxx>

extern Standard_Boolean Draw_Bounds;


//=======================================================================
//function : Draw_Axis3D
//purpose  : 
//=======================================================================

Draw_Axis3D::Draw_Axis3D (const Draw_Color& col, 
			  const Standard_Integer Size) :
       myAxes(gp::XOY()),myColor(col), mySize(Size)
{
}

//=======================================================================
//function : Draw_Axis3D
//purpose  : 
//=======================================================================

Draw_Axis3D::Draw_Axis3D (const gp_Pnt& p, 
			  const Draw_Color& col, 
			  const Standard_Integer Size) :
       myAxes(p,gp::DZ(),gp::DX()), myColor(col), mySize(Size)
{
}

//=======================================================================
//function : Draw_Axis3D
//purpose  : 
//=======================================================================

Draw_Axis3D::Draw_Axis3D (const gp_Ax3& a, 
			  const Draw_Color& col, 
			  const Standard_Integer Size) :
       myAxes(a), myColor(col), mySize(Size)
{
}

//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void Draw_Axis3D::DrawOn (Draw_Display& dis) const
{
  Draw_Bounds = Standard_False;
  dis.SetColor(myColor);
  Standard_Real z = dis.Zoom();
  z = (Standard_Real)mySize / z;
  gp_Pnt P,P0 = myAxes.Location();
  P = P0.Translated(gp_Vec(myAxes.XDirection()) * z);
  dis.Draw(P0,P);
  dis.DrawString(P,"X");
  P = P0.Translated(gp_Vec(myAxes.YDirection()) * z);
  dis.Draw(P0,P);
  dis.DrawString(P,"Y");
  P = P0.Translated(gp_Vec(myAxes.Direction()) * z);
  dis.Draw(P0,P);
  dis.DrawString(P,"Z");
  Draw_Bounds = Standard_True;
}

