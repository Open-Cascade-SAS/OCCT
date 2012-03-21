// Created on: 1992-04-23
// Created by: Remi LEQUETTE
// Copyright (c) 1992-1999 Matra Datavision
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
					   
