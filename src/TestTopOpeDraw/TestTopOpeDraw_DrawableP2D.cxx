// Created on: 1994-11-17
// Created by: Jean Yves LEBEY
// Copyright (c) 1994-1999 Matra Datavision
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


#include <TestTopOpeDraw_DrawableP2D.ixx>

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
