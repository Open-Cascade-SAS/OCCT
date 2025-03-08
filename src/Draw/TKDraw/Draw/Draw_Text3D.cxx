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

#include <Draw_Display.hxx>
#include <Draw_Text3D.hxx>
#include <gp_Pnt.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Draw_Text3D, Draw_Drawable3D)

//=================================================================================================

Draw_Text3D::Draw_Text3D(const gp_Pnt& p, const Standard_CString T, const Draw_Color& col)
    : myPoint(p),
      myColor(col),
      myText(T),
      mymoveX(0.0),
      mymoveY(0.0)
{
}

//=================================================================================================

Draw_Text3D::Draw_Text3D(const gp_Pnt&          p,
                         const Standard_CString T,
                         const Draw_Color&      col,
                         const Standard_Real    moveX,
                         const Standard_Real    moveY)
    : myPoint(p),
      myColor(col),
      myText(T),
      mymoveX(moveX),
      mymoveY(moveY)
{
}

//=================================================================================================

void Draw_Text3D::SetPnt(const gp_Pnt& p)
{
  myPoint = p;
}

//=================================================================================================

void Draw_Text3D::DrawOn(Draw_Display& dis) const
{
  dis.SetColor(myColor);
  dis.DrawString(myPoint, myText.ToCString(), mymoveX, mymoveY);
}
