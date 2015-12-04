// Created on: 1992-04-29
// Created by: Remi LEQUETTE
// Copyright (c) 1992-1999 Matra Datavision
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


#include <Draw_Axis2D.hxx>
#include <Draw_Color.hxx>
#include <Draw_Display.hxx>
#include <gp.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Draw_Axis2D,Draw_Drawable2D)

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

