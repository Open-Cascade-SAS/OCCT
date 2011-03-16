// File:	Draw_Text3D.cxx
// Created:	Wed Apr 29 15:37:27 1992
// Author:	Remi LEQUETTE
//		<rle@sdsun1>


#include <Draw_Text3D.ixx>

//=======================================================================
//function : Draw_Text3D
//purpose  : 
//=======================================================================

Draw_Text3D::Draw_Text3D(const gp_Pnt& p, const Standard_CString T, 
			 const Draw_Color& col) :
     myPoint(p), myColor(col), myText(T), mymoveX(0.0), mymoveY(0.0)
{
}

//=======================================================================
//function : Draw_Text3D
//purpose  : 
//=======================================================================

Draw_Text3D::Draw_Text3D(const gp_Pnt& p, const Standard_CString T, 
			 const Draw_Color& col,
			 const Standard_Real moveX, const Standard_Real moveY) :
       myPoint(p), myColor(col), myText(T), mymoveX(moveX), mymoveY(moveY)
{
}

//=======================================================================
//function : SetPnt
//purpose  : 
//=======================================================================

void Draw_Text3D::SetPnt(const gp_Pnt& p)
{
  myPoint = p;
}

//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void Draw_Text3D::DrawOn(Draw_Display& dis) const
{
  dis.SetColor(myColor);
  dis.DrawString(myPoint,myText.ToCString(),
		 mymoveX, mymoveY);
}

