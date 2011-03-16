// File:	TestTopOpeDraw_DrawableP2D.cxx
// Created:	Thu Nov 17 16:42:42 1994
// Author:	Jean Yves LEBEY
//		<jyl@bravox>

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
