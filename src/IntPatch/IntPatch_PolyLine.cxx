// Created on: 1993-01-29
// Created by: Isabelle GRIGNON
// Copyright (c) 1993-1999 Matra Datavision
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


//-- lbr le 12 juin : Ajout des fleches sur les Lines
//-- msv 13.03.2002 : compute deflection for WLine; Error() returns deflection

#include <IntPatch_PolyLine.ixx>

#include <Precision.hxx>

#define INITDEFLE Precision::PConfusion()*100.

//=======================================================================
//function : IntPatch_PolyLine
//purpose  : 
//=======================================================================

IntPatch_PolyLine::IntPatch_PolyLine ()
     : IntPatch_Polygo(INITDEFLE)
{}

//=======================================================================
//function : IntPatch_PolyLine
//purpose  : 
//=======================================================================

IntPatch_PolyLine::IntPatch_PolyLine (const Standard_Real InitDefle)
     : IntPatch_Polygo(InitDefle)
{}

//=======================================================================
//function : SetWLine
//purpose  : 
//=======================================================================

void IntPatch_PolyLine::SetWLine(const Standard_Boolean OnFirst, const Handle(IntPatch_WLine)& Line)
{
  typ = IntPatch_Walking;
  wpoly = Line;
  onfirst = OnFirst;
  Prepare();
}

//=======================================================================
//function : SetRLine
//purpose  : 
//=======================================================================

void IntPatch_PolyLine::SetRLine(const Standard_Boolean OnFirst, const Handle(IntPatch_RLine)& Line)
{
  typ = IntPatch_Restriction;
  rpoly = Line;
  onfirst = OnFirst;
  Prepare();
}

//=======================================================================
//function : Prepare
//purpose  : 
//=======================================================================

void IntPatch_PolyLine::Prepare()
{
  Standard_Integer i;
  myBox.SetVoid();
  Standard_Integer n=NbPoints();
  Standard_Real eps = myError;

  gp_Pnt2d P1, P2;
  if (n >= 3) {
    P1 = Point(1); P2 = Point(2);
  }
  for (i=1; i<=n ;i++) {
    const gp_Pnt2d& P3 = Point(i);
    if (i >= 3) {
      gp_XY V13 = P3.XY() - P1.XY();
      gp_XY V12 = P2.XY() - P1.XY();
      Standard_Real d13 = V13.Modulus(), d;
      if (d13 > eps)
	d = V13.CrossMagnitude(V12) / d13;
      else
	d = eps;
      if (d > myError) {
	// try to compute deflection more precisely using parabola interpolation
	gp_XY V23 = P3.XY() - P2.XY();
	Standard_Real d12 = V12.Modulus(), d23 = V23.Modulus();
	// compute parameter of P2 (assume parameters of P1,P3 are 0,1)
	Standard_Real tm = d12 / (d12+d23);
	if (tm > 0.1 && tm < 0.9) {
	  tm -= (tm-0.5) * 0.6;
	  Standard_Real tm1mtm = tm*(1-tm);
	  // coefficients of parabola
	  Standard_Real Ax = (tm*V13.X() - V12.X()) / tm1mtm;
	  Standard_Real Bx = (V12.X() - tm*tm*V13.X()) / tm1mtm;
	  Standard_Real Cx = P1.X();
	  Standard_Real Ay = (tm*V13.Y() - V12.Y()) / tm1mtm;
	  Standard_Real By = (V12.Y() - tm*tm*V13.Y()) / tm1mtm;
	  Standard_Real Cy = P1.Y();
	  // equations of lines P1-P2 and P2-P3
	  Standard_Real A1 = V12.Y() / d12;
	  Standard_Real B1 = -V12.X() / d12;
	  Standard_Real C1 = (P2.X()*P1.Y() - P1.X()*P2.Y()) / d12;
	  Standard_Real A2 = V23.Y() / d23;
	  Standard_Real B2 = -V23.X() / d23;
	  Standard_Real C2 = (P3.X()*P2.Y() - P2.X()*P3.Y()) / d23;
	  // points on parabola with max deflection
	  Standard_Real t1 = -0.5 * (A1*Bx + B1*By) / (A1*Ax + B1*Ay);
	  Standard_Real t2 = -0.5 * (A2*Bx + B2*By) / (A2*Ax + B2*Ay);
	  Standard_Real xt1 = Ax*t1*t1 + Bx*t1 + Cx;
	  Standard_Real yt1 = Ay*t1*t1 + By*t1 + Cy;
	  Standard_Real xt2 = Ax*t2*t2 + Bx*t2 + Cx;
	  Standard_Real yt2 = Ay*t2*t2 + By*t2 + Cy;
	  // max deflection on segments P1-P2 and P2-P3
	  Standard_Real d1 = Abs (A1*xt1 + B1*yt1 + C1);
	  Standard_Real d2 = Abs (A2*xt2 + B2*yt2 + C2);
	  if (d2 > d1) d1 = d2;
	  // select min deflection from linear and parabolic ones
	  if (d1 < d) d = d1;
	}
	if (d > myError) myError=d;
      }
      P1 = P2; P2 = P3;
    }
    myBox.Add(P3);
  }
  myBox.Enlarge(myError);  
}

//=======================================================================
//function : ResetError
//purpose  : 
//=======================================================================

void IntPatch_PolyLine::ResetError()
{
  myError = INITDEFLE;
}

//=======================================================================
//function : NbPoints
//purpose  : 
//=======================================================================

Standard_Integer IntPatch_PolyLine::NbPoints() const
{
  return (typ == IntPatch_Walking ? wpoly->NbPnts() : rpoly->NbPnts());
}

//=======================================================================
//function : Point
//purpose  : 
//=======================================================================

gp_Pnt2d IntPatch_PolyLine::Point(const Standard_Integer Index )  const 
{ 
  Standard_Real X,Y,X1,Y1,DX,DY;
  DX=DY=0;
  if (onfirst) {
    if (typ == IntPatch_Walking) {
      wpoly->Point(Index).ParametersOnS1(X,Y);
      if(Index==1) { 
	wpoly->Point(2).ParametersOnS1(X1,Y1);
	DX=0.0000001*(X-X1);
	DY=0.0000001*(Y-Y1);	
      }
      else if(Index==wpoly->NbPnts()) { 
	wpoly->Point(Index-1).ParametersOnS1(X1,Y1);
	DX=0.0000001*(X-X1);
	DY=0.0000001*(Y-Y1);	
      }
    }
    else {
      rpoly->Point(Index).ParametersOnS1(X,Y);
    }
  }
  else {
    if (typ == IntPatch_Walking) {
      wpoly->Point(Index).ParametersOnS2(X,Y);
      if(Index==1) { 
	wpoly->Point(2).ParametersOnS2(X1,Y1);
	DX=0.0000001*(X-X1);
	DY=0.0000001*(Y-Y1);	
      }
      else if(Index==wpoly->NbPnts()) { 
	wpoly->Point(Index-1).ParametersOnS2(X1,Y1);
	DX=0.0000001*(X-X1);
	DY=0.0000001*(Y-Y1);	
      }
    }
    else {
      rpoly->Point(Index).ParametersOnS2(X,Y);
    }
  }

  return(gp_Pnt2d(X+DX,Y+DY));
}
