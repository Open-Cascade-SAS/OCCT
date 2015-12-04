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


#include <Draw_Axis3D.hxx>
#include <Draw_Color.hxx>
#include <Draw_Display.hxx>
#include <gp.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Draw_Axis3D,Draw_Drawable3D)

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

