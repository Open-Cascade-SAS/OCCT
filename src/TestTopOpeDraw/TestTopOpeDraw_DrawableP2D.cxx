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
#include <Draw_Text2D.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_Type.hxx>
#include <TestTopOpeDraw_DrawableP2D.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TestTopOpeDraw_DrawableP2D,Draw_Marker2D)

//=======================================================================
//function : TestTopOpeDraw_DrawableP2D
//purpose  : 
//=======================================================================
TestTopOpeDraw_DrawableP2D::TestTopOpeDraw_DrawableP2D 
(const gp_Pnt2d& P,
 const Draw_Color& PColor,
 const Standard_Real MoveX,
 const Standard_Real MoveY) :
 Draw_Marker2D(P,Draw_Square,PColor,2), // Size
 myPnt2d(P),
 myText(" "),
 myTextColor(PColor),
 myMoveX(MoveX),
 myMoveY(MoveY)
{
  myText2D = new Draw_Text2D(myPnt2d,myText,myTextColor,(Standard_Integer)myMoveX,(Standard_Integer)myMoveY);
}

//=======================================================================
//function : TestTopOpeDraw_DrawableP2D
//purpose  : 
//=======================================================================

TestTopOpeDraw_DrawableP2D::TestTopOpeDraw_DrawableP2D 
(const gp_Pnt2d& P,const Draw_Color& PColor,
 const Standard_CString Text,const Draw_Color& TextColor,
 const Standard_Real MoveX,
 const Standard_Real MoveY) :
 Draw_Marker2D(P,Draw_Square,PColor,2), // Size
 myPnt2d(P),
 myText(Text),
 myTextColor(TextColor),
 myMoveX(MoveX),
 myMoveY(MoveY)
{
  myText2D = new Draw_Text2D(myPnt2d,myText,myTextColor,(Standard_Integer)myMoveX,(Standard_Integer)myMoveY);
}

//=======================================================================
//function : TestTopOpeDraw_DrawableP2D
//purpose  : 
//=======================================================================

TestTopOpeDraw_DrawableP2D::TestTopOpeDraw_DrawableP2D 
(const gp_Pnt2d& P,const Draw_MarkerShape T,const Draw_Color& PColor,
 const Standard_CString Text,const Draw_Color& TextColor,
 const Standard_Integer Size,
 const Standard_Real MoveX,
 const Standard_Real MoveY) :
 Draw_Marker2D(P,T,PColor,Size),
 myPnt2d(P),
 myText(Text),
 myTextColor(TextColor),
 myMoveX(MoveX),
 myMoveY(MoveY)
{
  myText2D = new Draw_Text2D(myPnt2d,myText,myTextColor,(Standard_Integer)myMoveX,(Standard_Integer)myMoveY);
}

//=======================================================================
//function : TestTopOpeDraw_DrawableP2D
//purpose  : 
//=======================================================================

TestTopOpeDraw_DrawableP2D::TestTopOpeDraw_DrawableP2D 
(const gp_Pnt2d& P,const Draw_MarkerShape T,
 const Draw_Color& PColor,const Standard_CString Text,
 const Draw_Color& TextColor,const Standard_Real Tol,
 const Standard_Real MoveX,const Standard_Real MoveY) :
 Draw_Marker2D(P,T,PColor,Tol),
 myPnt2d(P),
 myText(Text),
 myTextColor(TextColor),
 myMoveX(MoveX),
 myMoveY(MoveY)
{
  myText2D = new Draw_Text2D(myPnt2d,myText,myTextColor,(Standard_Integer)myMoveX,(Standard_Integer)myMoveY);
}

//=======================================================================
//function : ChangePnt2d
//purpose  : 
//=======================================================================

void TestTopOpeDraw_DrawableP2D::ChangePnt2d(const gp_Pnt2d& P)
{
  myPnt2d = P;
  myText2D = new Draw_Text2D(myPnt2d,myText,myTextColor,(Standard_Integer)myMoveX,(Standard_Integer)myMoveY);
  ChangePos() = P;
}

//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void TestTopOpeDraw_DrawableP2D::DrawOn(Draw_Display& dis) const
{
  Draw_Marker2D::DrawOn(dis);
  myText2D->DrawOn(dis);
}
