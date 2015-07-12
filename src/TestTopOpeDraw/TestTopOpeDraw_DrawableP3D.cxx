// Created on: 1994-11-17
// Created by: Jean Yves LEBEY
// Copyright (c) 1994-1999 Matra Datavision
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
#include <Draw_Text3D.hxx>
#include <gp_Pnt.hxx>
#include <Standard_Type.hxx>
#include <TestTopOpeDraw_DrawableP3D.hxx>

//=======================================================================
//function : TestTopOpeDraw_DrawableP3D
//purpose  : 
//=======================================================================
TestTopOpeDraw_DrawableP3D::TestTopOpeDraw_DrawableP3D 
(const gp_Pnt& P,
 const Draw_Color& PColor,
 const Standard_Real MoveX,
 const Standard_Real MoveY) :
 Draw_Marker3D(P,Draw_Square,PColor,2), // Size
 myPnt(P),
 myText(" "),
 myTextColor(PColor),
 myMoveX(MoveX),
 myMoveY(MoveY)
{
  myText3D = new Draw_Text3D(myPnt,myText,myTextColor, myMoveX, myMoveY);
}

//=======================================================================
//function : TestTopOpeDraw_DrawableP3D
//purpose  : 
//=======================================================================

TestTopOpeDraw_DrawableP3D::TestTopOpeDraw_DrawableP3D 
(const gp_Pnt& P, const Draw_Color& PColor,
 const Standard_CString Text, const Draw_Color& TextColor,
 const Standard_Real MoveX,
 const Standard_Real MoveY) :
 Draw_Marker3D(P,Draw_Square,PColor,2), // Size
 myPnt(P),
 myText(Text),
 myTextColor(TextColor),
 myMoveX(MoveX),
 myMoveY(MoveY)
{
  myText3D = new Draw_Text3D(myPnt,myText,myTextColor, myMoveX, myMoveY);
}

//=======================================================================
//function : TestTopOpeDraw_DrawableP3D
//purpose  : 
//=======================================================================

TestTopOpeDraw_DrawableP3D::TestTopOpeDraw_DrawableP3D 
(const gp_Pnt& P, const Draw_MarkerShape T, const Draw_Color& PColor,
 const Standard_CString Text, const Draw_Color& TextColor,
 const Standard_Integer Size,
 const Standard_Real MoveX,
 const Standard_Real MoveY) :
 Draw_Marker3D(P,T,PColor,Size),
 myPnt(P),
 myText(Text),
 myTextColor(TextColor),
 myMoveX(MoveX),
 myMoveY(MoveY)
{
  myText3D = new Draw_Text3D(myPnt,myText,myTextColor, myMoveX, myMoveY);
}

//=======================================================================
//function : TestTopOpeDraw_DrawableP3D
//purpose  : 
//=======================================================================

TestTopOpeDraw_DrawableP3D::TestTopOpeDraw_DrawableP3D 
(const gp_Pnt& P, const Draw_MarkerShape T,
 const Draw_Color& PColor, const Standard_CString Text,
 const Draw_Color& TextColor, const Standard_Real Tol,
 const Standard_Real MoveX, const Standard_Real MoveY) :
 Draw_Marker3D(P, T, PColor, Tol),
 myPnt(P),
 myText(Text),
 myTextColor(TextColor),
 myMoveX(MoveX),
 myMoveY(MoveY)
{
  myText3D = new Draw_Text3D(myPnt,myText,myTextColor, myMoveX, myMoveY);
}

//=======================================================================
//function : ChangePnt
//purpose  : 
//=======================================================================

void TestTopOpeDraw_DrawableP3D::ChangePnt(const gp_Pnt& P)
{
  myPnt = P;
  myText3D = new Draw_Text3D(myPnt, myText, myTextColor, myMoveX, myMoveY);
  ChangePos() = P;
  
}

//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void  TestTopOpeDraw_DrawableP3D::DrawOn(Draw_Display& dis) const
{
  Draw_Marker3D::DrawOn(dis);
  myText3D->DrawOn(dis);
}
