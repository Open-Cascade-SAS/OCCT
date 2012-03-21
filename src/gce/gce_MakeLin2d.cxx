// Created on: 1992-09-02
// Created by: Remi GILET
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


#include <gce_MakeLin2d.ixx>
#include <gp.hxx>
#include <StdFail_NotDone.hxx>

//=========================================================================
//   Creation d une ligne 2d de gp a partir d un Ax2d de gp.              +
//=========================================================================

gce_MakeLin2d::gce_MakeLin2d(const gp_Ax2d& A)
{
  TheLin2d = gp_Lin2d(A);
  TheError = gce_Done;
}

//=========================================================================
//   Creation d une ligne 2d de gp a partir de son origine P (Pnt2d de gp)+
//   et d une direction V (Dir2d de gp).                                  +
//=========================================================================

gce_MakeLin2d::gce_MakeLin2d(const gp_Pnt2d& P,
			     const gp_Dir2d& V)
{
  TheLin2d = gp_Lin2d(P,V);
  TheError = gce_Done;
}

//=========================================================================
//   Creation d une ligne 2d de gp a partir des parametres de son         +
//    equation.                                                           +
//=========================================================================

gce_MakeLin2d::gce_MakeLin2d(const Standard_Real A,
			     const Standard_Real B,
			     const Standard_Real C)
{
  if (A*A + B*B <= gp::Resolution()) {
    TheError = gce_NullAxis;
  }
  else {
    TheLin2d = gp_Lin2d(A,B,C);
    TheError = gce_Done;
  }
}

//=========================================================================
//   Creation d une ligne 2d de gp passant par les deux points <P1> et    +
//   <P2>.                                                                +
//=========================================================================

gce_MakeLin2d::gce_MakeLin2d(const gp_Pnt2d& P1,
			     const gp_Pnt2d& P2)
{
  if (P1.Distance(P2) >= gp::Resolution()) {
    TheLin2d = gp_Lin2d(P1,gp_Dir2d(P2.XY()-P1.XY()));
    TheError = gce_Done;
  }
  else { 
    TheError = gce_ConfusedPoints;
  }
}

//=========================================================================
//   Creation d une ligne 2d de gp <TheLine> parallele a une autre ligne  +
//   <Line1> passant par le point <Point1>.                               +
//=========================================================================

gce_MakeLin2d::gce_MakeLin2d(const gp_Lin2d& Line,
			     const gp_Pnt2d& Point)
{
  TheLin2d = gp_Lin2d(Point,Line.Direction());
  TheError = gce_Done;
}

//=========================================================================
//   Creation d une ligne 2d de gp <TheLine> parallele a une autre ligne  +
//   <Line1> a une distance <Dist1>.                                      +
//=========================================================================

gce_MakeLin2d::gce_MakeLin2d(const gp_Lin2d&     Line,
			     const Standard_Real Dist)
{
  gp_Pnt2d Point(Line.Location().XY()+
		 Dist*gp_XY(-Line.Direction().Y(),Line.Direction().X()));
  TheLin2d = gp_Lin2d(Point,Line.Direction());
  TheError = gce_Done;
}

gp_Lin2d gce_MakeLin2d::Value() const
{ 
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return TheLin2d;
}

gp_Lin2d gce_MakeLin2d::Operator() const 
{
  return Value();
}

gce_MakeLin2d::operator gp_Lin2d () const
{
  return Value();
}

