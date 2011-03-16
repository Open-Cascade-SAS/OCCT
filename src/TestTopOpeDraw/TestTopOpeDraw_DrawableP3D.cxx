// File:	TestTopOpeDraw_DrawableP3D.cxx
// Created:	Thu Nov 17 16:42:42 1994
// Author:	Jean Yves LEBEY
//		<jyl@bravox>

#include <TestTopOpeDraw_DrawableP3D.ixx>

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
