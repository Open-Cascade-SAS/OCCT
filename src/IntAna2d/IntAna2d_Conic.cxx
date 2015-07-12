// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <gp_Ax2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab2d.hxx>
#include <gp_XY.hxx>
#include <IntAna2d_Conic.hxx>

IntAna2d_Conic::IntAna2d_Conic (const gp_Lin2d& L) {

  a = 0.0;
  b = 0.0;
  c = 0.0;
  L.Coefficients(d,e,f);
  f = 2*f;
}

IntAna2d_Conic::IntAna2d_Conic (const gp_Circ2d& C) {

  C.Coefficients(a,b,c,d,e,f);
}


IntAna2d_Conic::IntAna2d_Conic (const gp_Elips2d& E) {

  E.Coefficients(a,b,c,d,e,f);
}


IntAna2d_Conic::IntAna2d_Conic (const gp_Parab2d& P) {
  P.Coefficients(a,b,c,d,e,f);
}


IntAna2d_Conic::IntAna2d_Conic (const gp_Hypr2d& H) {
  H.Coefficients(a,b,c,d,e,f);
}


void IntAna2d_Conic::NewCoefficients(Standard_Real& A,Standard_Real& B,Standard_Real& C
			  ,Standard_Real& D,Standard_Real& E,Standard_Real& F
			  ,const gp_Ax2d& Dir1)  const {
  Standard_Real t11,t12,t13;                  // x = t11 X + t12 Y + t13
  Standard_Real t21,t22,t23;                  // y = t21 X + t22 Y + t23
  Standard_Real A1,B1,C1,D1,E1,F1;            

  //      P0(x,y)=A x x + B y y + ... + F =0  (x,y "absolute" coordinates)
  // and  P1(X(x,y),Y(x,y))=P0(x,y)
  // with P1(X,Y)= A1 X X + B1 Y Y + 2 C1 X Y + 2 D1 X + 2 E1 Y + F1
  //             = A  x x + B  y y + 2 C  x y + 2 D  x + 2 E  y + f

  Dir1.Direction().Coord(t11,t21);
  Dir1.Location().Coord(t13,t23);

  t22=t11;
  t12=-t21;

  A1=(t11*(A*t11 + 2*C*t21) + B*t21*t21);
  B1=(t12*(A*t12 + 2*C*t22) + B*t22*t22);
  C1=(t12*(A*t11 + C*t21) + t22*(C*t11 + B*t21));
  D1=(t11*(D + A*t13) + t21*(E + C*t13) + t23*(C*t11 + B*t21));
  E1=(t12*(D + A*t13) + t22*(E + C*t13) + t23*(C*t12 + B*t22));
  F1=F + t13*(2.0*D + A*t13) + t23*(2.0*E + 2.0*C*t13 + B*t23);
  
  A=A1; B=B1; C=C1; D=D1; E=E1; F=F1;
}


Standard_Real IntAna2d_Conic::Value (const Standard_Real X, const Standard_Real Y) const {
  Standard_Real _a,_b,_c,_d,_e,_f;
  this->Coefficients(_a,_b,_c,_d,_e,_f);
  return (_a*X*X + _b*Y*Y + 2.*_c*X*Y + 2.*_d*X + 2.*_e*Y +_f);
}

gp_XY IntAna2d_Conic::Grad (const Standard_Real X, const Standard_Real Y) const {
  Standard_Real _a,_b,_c,_d,_e,_f;
  this->Coefficients(_a,_b,_c,_d,_e,_f);
  return gp_XY(2.*_a*X + 2.*_c*Y + 2.*_d, 2.*_b*Y + 2.*_c*X + 2.*_e);
}

void IntAna2d_Conic::ValAndGrad (const Standard_Real X, const Standard_Real Y, 
				  Standard_Real& Val, gp_XY& Grd) const {
  Standard_Real la,lb,lc,ld,le,lf;
  this->Coefficients(la,lb,lc,ld,le,lf);
  Grd.SetCoord(2.*la*X + 2.*lc*Y + 2.*ld, 2.*lb*Y + 2.*lc*X + 2.*le);
  Val = la*X*X + lb*Y*Y + 2.*lc*X*Y + 2.*ld*X + 2.*le*Y +lf;
}


void IntAna2d_Conic::Coefficients(Standard_Real& A,Standard_Real& B,Standard_Real& C,
				  Standard_Real& D,Standard_Real& E,Standard_Real& F) const 
{
  A=a; B=b; C=c; D=d; E=e; F=f; 
}



