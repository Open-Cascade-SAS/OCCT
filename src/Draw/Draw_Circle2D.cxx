// File:	Draw_Circle2D.cxx
// Created:	Thu Apr 30 12:05:20 1992
// Author:	Remi LEQUETTE
//		<rle@sdsun1>


#include <Draw_Circle2D.ixx>

//=======================================================================
//function : Draw_Circle2D
//purpose  : 
//=======================================================================

Draw_Circle2D::Draw_Circle2D(const gp_Circ2d& C,
			     const Standard_Real A1, const Standard_Real A2,
			     const Draw_Color& col) :
       myCirc(C), myA1(A1), myA2(A2), myColor(col)
{
}

//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void Draw_Circle2D::DrawOn(Draw_Display& d) const
{
  d.SetColor(myColor);
  d.Draw(myCirc,myA1,myA2);
}
