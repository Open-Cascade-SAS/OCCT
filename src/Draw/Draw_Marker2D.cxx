// File:	Draw_Marker2D.cxx
// Created:	Thu Apr 23 18:47:43 1992
// Author:	Remi LEQUETTE
//		<rle@sdsun1>



#include <Draw_Marker2D.ixx>

//=======================================================================
//function : Draw_Marker2D
//purpose  : 
//=======================================================================

Draw_Marker2D::Draw_Marker2D(const gp_Pnt2d& P, const Draw_MarkerShape T,
			     const Draw_Color& C, const Standard_Integer S) :
       myPos(P),  myCol(C), myTyp(T),mySiz(S)
{
}

//=======================================================================
//function : Draw_Marker2D
//purpose  : 
//=======================================================================

Draw_Marker2D::Draw_Marker2D(const gp_Pnt2d& P, const Draw_MarkerShape T,
			     const Draw_Color& C, const Standard_Real RSize) :
       myPos(P), myCol(C), myTyp(T), myRSiz(RSize), myIsRSiz(Standard_True)
{
}

//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void Draw_Marker2D::DrawOn(Draw_Display& D) const
{
  D.SetColor(myCol);
  D.DrawMarker(myPos,myTyp,mySiz);
}

//=======================================================================
//function : ChangePos
//purpose  : 
//=======================================================================

gp_Pnt2d& Draw_Marker2D::ChangePos()
{
  return myPos;
}

//=======================================================================
//function : PickReject
//purpose  : 
//=======================================================================

Standard_Boolean Draw_Marker2D::PickReject(const Standard_Real,
					   const Standard_Real,
					   const Standard_Real) const
{
  return Standard_False;
}
					   
