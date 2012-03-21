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


#include <gce_MakePln.ixx>
#include <StdFail_NotDone.hxx>
#include <gp.hxx>
#include <gp_Ax3.hxx>

gce_MakePln::gce_MakePln(const gp_Ax2& A2)
{
  ThePln = gp_Pln(gp_Ax3(A2));
  TheError = gce_Done;
}

gce_MakePln::gce_MakePln(const gp_Pnt& P,
			 const gp_Dir& V)
{
  ThePln = gp_Pln(P,V);
  TheError = gce_Done;
}

gce_MakePln::gce_MakePln(const gp_Pnt& P1,
			 const gp_Pnt& P2)
{
  if (P1.Distance(P2) <= gp::Resolution()) { TheError = gce_ConfusedPoints; }
  else {
    gp_Dir dir(P2.XYZ()-P1.XYZ());
    ThePln = gp_Pln(P1,dir);
    TheError = gce_Done;
  }
}

gce_MakePln::gce_MakePln(const Standard_Real A,
			 const Standard_Real B,
			 const Standard_Real C,
			 const Standard_Real D)
{
  if (A*A + B*B + C*C <= gp::Resolution()) {
    TheError = gce_BadEquation;
  }
  else {
    ThePln = gp_Pln(A,B,C,D);
    TheError = gce_Done;
  }
}

//=========================================================================
//   Creation d un gp_pln passant par trois points.                       +
//=========================================================================

gce_MakePln::gce_MakePln(const gp_Pnt& P1 ,
			 const gp_Pnt& P2 ,
			 const gp_Pnt& P3 ) 
{
  gp_XYZ V1(P2.XYZ()-P1.XYZ());
  gp_XYZ V2(P3.XYZ()-P1.XYZ());
  gp_XYZ Norm(V1.Crossed(V2));
  if (Norm.Modulus() < gp::Resolution()) { TheError = gce_ColinearPoints; }
  else {
    gp_Dir DNorm(Norm);
    gp_Dir Dx(V1);
    ThePln = gp_Pln(gp_Ax3(P1,DNorm,Dx));
    TheError = gce_Done;
  }
}

//=========================================================================
//   Creation d un gp_pln parallele a un autre pln a une distance donnee. +
//=========================================================================

gce_MakePln::gce_MakePln(const gp_Pln&       Pl   ,
			 const Standard_Real Dist ) 
{
  gp_Pnt Center(Pl.Location().XYZ()+Dist*gp_XYZ(Pl.Axis().Direction().XYZ()));
  ThePln=gp_Pln(gp_Ax3(Center,Pl.Axis().Direction(),Pl.XAxis().Direction()));
  TheError = gce_Done;
}

//=========================================================================
//   Creation d un gp_pln parallele a un autre pln passant par un point   +
//   <Point1>.                                                            +
//=========================================================================

gce_MakePln::gce_MakePln(const gp_Pln& Pl    ,
			 const gp_Pnt& Point ) 
{
  ThePln = gp_Pln(gp_Ax3(Point,Pl.Axis().Direction(),Pl.XAxis().Direction()));
  TheError = gce_Done;
}

//=========================================================================
//  Creation d un gp_pln a partir d un Ax1 (Point + Normale).             +
//=========================================================================

gce_MakePln::gce_MakePln(const gp_Ax1& Axis ) 
{
  ThePln = gp_Pln(Axis.Location(),Axis.Direction());
  TheError = gce_Done;
}

//=========================================================================
//  Creation d un gp_pln par un tableau de points.                        +
//=========================================================================

/*gce_MakePln::gce_MakePln(const gp_Array1OfPnt& Pts     ,
			       Standard_Real   ErrMax  ,
			       Standard_Real   ErrMean ) 
{
  TheError = gce_ConfusedPoints;
}
*/
const gp_Pln& gce_MakePln::Value () const
{
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return ThePln;
}

const gp_Pln& gce_MakePln::Operator() const 
{
  return Value();
}

gce_MakePln::operator gp_Pln() const
{
  return Value();
}





