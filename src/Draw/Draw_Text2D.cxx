// File:	Draw_Text2D.cxx
// Created:	Wed Apr 29 15:37:27 1992
// Author:	Remi LEQUETTE
//		<rle@sdsun1>


#include <Draw_Text2D.ixx>


//=======================================================================
//function : Draw_Text2D
//purpose  : 
//=======================================================================

Draw_Text2D::Draw_Text2D(const gp_Pnt2d& p, const Standard_CString T, 
			 const Draw_Color& col) :
       myPoint(p), myColor(col), myText(T), mymoveX(0), mymoveY(0)
{
}

//=======================================================================
//function : Draw_Text2D
//purpose  : 
//=======================================================================

Draw_Text2D::Draw_Text2D(const gp_Pnt2d& p, const Standard_CString T, 
			 const Draw_Color& col, const Standard_Integer moveX, const Standard_Integer moveY) :
       myPoint(p), myColor(col), myText(T), mymoveX(moveX), mymoveY(moveY)
{
}

//=======================================================================
//function : SetPnt2d
//purpose  : 
//=======================================================================

void Draw_Text2D::SetPnt2d(const gp_Pnt2d& p)
{
  myPoint = p;
}

//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void Draw_Text2D::DrawOn(Draw_Display& dis) const
{
  dis.SetColor(myColor);
  dis.DrawString(myPoint,myText.ToCString(),
		 (Standard_Real)mymoveX, (Standard_Real)mymoveY);
}
