// Copyright: 	Matra-Datavision 1991
// File:	Draw_Segment3D.cxx
// Created:	Thu Apr 25 11:25:22 1991
// Author:	Arnaud BOUZY
//		<adn>

#include <Draw_Segment3D.ixx>

//=======================================================================
//function : Draw_Segment3D
//purpose  : 
//=======================================================================

Draw_Segment3D::Draw_Segment3D(const gp_Pnt& p1, 
			       const gp_Pnt& p2, 
			       const Draw_Color& col) :
       myFirst(p1),
       myLast(p2),
       myColor(col)
{
}


//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void  Draw_Segment3D::DrawOn(Draw_Display& dis)const 
{
  dis.SetColor(myColor);
  dis.Draw(myFirst,myLast);
}


//=======================================================================
//function : First
//purpose  : 
//=======================================================================

const gp_Pnt&  Draw_Segment3D::First() const
{
  return myFirst;
}


//=======================================================================
//function : First
//purpose  : 
//=======================================================================

void  Draw_Segment3D::First(const gp_Pnt& P)
{
  myFirst = P;
}


//=======================================================================
//function : Last
//purpose  : 
//=======================================================================

const gp_Pnt&  Draw_Segment3D::Last() const
{
  return myLast;
}


//=======================================================================
//function : Last
//purpose  : 
//=======================================================================

void  Draw_Segment3D::Last(const gp_Pnt& P)
{
  myLast = P;
}


