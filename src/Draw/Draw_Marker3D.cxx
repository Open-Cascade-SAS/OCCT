// File:	Draw_Marker3D.cxx
// Created:	Thu Apr 23 18:47:43 1992
// Author:	Remi LEQUETTE
//		<rle@sdsun1>



#include <Draw_Marker3D.ixx>

//=======================================================================
//function : Draw_Marker3D
//purpose  : 
//=======================================================================

Draw_Marker3D::Draw_Marker3D(const gp_Pnt& P, const Draw_MarkerShape T,
			     const Draw_Color& C, const Standard_Integer S) :
       myPos(P), myCol(C), myTyp(T), mySiz(S), myIsRSiz(Standard_False)
{
}

//=======================================================================
//function : Draw_Marker3D
//purpose  : 
//=======================================================================

Draw_Marker3D::Draw_Marker3D(const gp_Pnt& P, const Draw_MarkerShape T,
			     const Draw_Color& C, const Standard_Real RSize) :
       myPos(P), myCol(C), myTyp(T), myRSiz(RSize), myIsRSiz(Standard_True)
{
}

//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void Draw_Marker3D::DrawOn(Draw_Display& D) const
{
  D.SetColor(myCol);
  if(myIsRSiz) D.DrawMarker(myPos,myTyp,myRSiz);
  else D.DrawMarker(myPos,myTyp,mySiz);
}

//=======================================================================
//function : ChangePos
//purpose  : 
//=======================================================================

gp_Pnt& Draw_Marker3D::ChangePos()
{
  return myPos;
}

//=======================================================================
//function : PickReject
//purpose  : 
//=======================================================================

Standard_Boolean Draw_Marker3D::PickReject(const Standard_Real,
					   const Standard_Real,
					   const Standard_Real) const
{
  return Standard_False;
}
					   
