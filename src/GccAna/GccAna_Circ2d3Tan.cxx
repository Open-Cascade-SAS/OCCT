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


#include <ElCLib.hxx>
#include <GccAna_Circ2d3Tan.hxx>
#include <GccAna_Circ2dBisec.hxx>
#include <GccEnt_BadQualifier.hxx>
#include <GccEnt_QualifiedCirc.hxx>
#include <GccEnt_QualifiedLin.hxx>
#include <GccInt_BCirc.hxx>
#include <GccInt_BElips.hxx>
#include <GccInt_BHyper.hxx>
#include <GccInt_BLine.hxx>
#include <GccInt_IType.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_Conic.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <math_DirectPolynomialRoots.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <TColStd_Array1OfReal.hxx>

//=========================================================================
//   Creation of a circle tangent to three circles.                        +
//=========================================================================
GccAna_Circ2d3Tan::
  GccAna_Circ2d3Tan (const GccEnt_QualifiedCirc& Qualified1,
		     const GccEnt_QualifiedCirc& Qualified2,
		     const GccEnt_QualifiedCirc& Qualified3,
		     const Standard_Real         Tolerance ):

//=========================================================================
//   Initialization of fields.                                           +
//=========================================================================

  cirsol(1,16)   ,
  qualifier1(1,16),
  qualifier2(1,16),
  qualifier3(1,16),
  TheSame1(1,16) ,
  TheSame2(1,16) ,
  TheSame3(1,16) ,
  pnttg1sol(1,16),
  pnttg2sol(1,16),
  pnttg3sol(1,16),
  par1sol(1,16)  ,
  par2sol(1,16)  ,
  par3sol(1,16)  ,
  pararg1(1,16)  ,
  pararg2(1,16)  ,
  pararg3(1,16)  
{

  gp_Dir2d dirx(1.0,0.0);
  Standard_Real Tol = Abs(Tolerance);
  WellDone = Standard_False;
  NbrSol = 0;
  if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
	Qualified1.IsOutside() || Qualified1.IsUnqualified()) ||
      !(Qualified2.IsEnclosed() || Qualified2.IsEnclosing() || 
	Qualified2.IsOutside() || Qualified2.IsUnqualified()) ||
      !(Qualified3.IsEnclosed() || Qualified3.IsEnclosing() || 
	Qualified3.IsOutside() || Qualified3.IsUnqualified())) {
    throw GccEnt_BadQualifier();
      return;
    }
  
//=========================================================================
//   Processing.                                                          +
//=========================================================================

  gp_Circ2d Cir1 = Qualified1.Qualified();
  gp_Circ2d Cir2 = Qualified2.Qualified();
  gp_Circ2d Cir3 = Qualified3.Qualified();
  Standard_Real R1 = Cir1.Radius();
  Standard_Real R2 = Cir2.Radius();
  Standard_Real R3 = Cir3.Radius();
  gp_Pnt2d center1(Cir1.Location());
  gp_Pnt2d center2(Cir2.Location());
  gp_Pnt2d center3(Cir3.Location());
  
  Standard_Real X1 = center1.X();
  Standard_Real X2 = center2.X();
  Standard_Real X3 = center3.X();
  
  Standard_Real Y1 = center1.Y();
  Standard_Real Y2 = center2.Y();
  Standard_Real Y3 = center3.Y();

  gp_XY dir2 = center1.XY() - center2.XY();
  gp_XY dir3 = center1.XY() - center3.XY();

//////////
  if ((Abs(R1 - R2) <= Tolerance && center1.IsEqual(center2, Tolerance)) ||
      (Abs(R1 - R3) <= Tolerance && center1.IsEqual(center3, Tolerance)) ||
      (Abs(R2 - R3) <= Tolerance && center2.IsEqual(center3, Tolerance)))
    return;
  else {
    if (Abs(dir2^dir3) <= Tolerance) {
      Standard_Real Dist1 = center1.Distance(center2);
      Standard_Real Dist2 = center1.Distance(center3);
      Standard_Real Dist3 = center2.Distance(center3);
      if (Abs(Abs(R1 - R2) - Dist1) <= Tolerance) {
	if (Abs(Abs(R1 - R3) - Dist2) <= Tolerance) {
	  if (Abs(Abs(R2 - R3) - Dist3) <= Tolerance)
	    return;
	} else if (Abs(R1 + R3 - Dist2) <= Tolerance) {
	  if (Abs(R2 + R3 - Dist3) <= Tolerance)
	    return;
	}
      } else if (Abs(R1 + R2 - Dist1) <= Tolerance) {
	if (Abs(Abs(R1 - R3) - Dist2) <= Tolerance &&
	    Abs(R2 + R3 - Dist3) <= Tolerance) {
	} else {
	  if (Abs(Abs(R2 - R3) - Dist3) <= Tolerance &&
	      Abs(R1 + R3 - Dist2) <= Tolerance)
	    return;
	}
      }
    }
  }
/////////
  TColStd_Array1OfReal A2(1, 8), B2(1, 8), C2(1, 8), D2(1, 8), E2(1, 8), F2(1, 8); 
  TColStd_Array1OfReal A3(1, 8), B3(1, 8), C3(1, 8), D3(1, 8), E3(1, 8), F3(1, 8); 
  TColStd_Array1OfReal Beta2(1, 8), Gamma2(1, 8), Delta2(1, 8);
  TColStd_Array1OfReal Beta3(1, 8), Gamma3(1, 8), Delta3(1, 8);
  Standard_Real a2, b2, c2, d2, e2, f2;
  Standard_Real a3, b3, c3, d3, e3, f3;
  Standard_Real A, B, C, D, E;
  Standard_Boolean IsSame;
  Standard_Boolean IsTouch;
  Standard_Integer FirstIndex;

  Standard_Integer i, j, k, l;
  TColStd_Array1OfReal xSol(1, 64);
  TColStd_Array1OfReal ySol(1, 64);
  TColStd_Array1OfReal rSol(1, 16);
  TColStd_Array1OfInteger FirstSol(1, 9);
  TColStd_Array1OfReal xSol1(1, 32);
  TColStd_Array1OfReal ySol1(1, 32);
  TColStd_Array1OfReal rSol1(1, 32);
  TColStd_Array1OfInteger FirstSol1(1, 9);
  Standard_Real x, y, r;
  Standard_Real m, n, t, s, v;
  Standard_Real p, q;
  Standard_Real Epsilon;

  Standard_Integer CurSol;

//*********************************************************************************************
//*********************************************************************************************

//   Actually we have to find solutions of eight systems of equations:
//         _                                          _
//        | (X - X1)2 + (Y - Y1)2 = (R - R1)2        | (X - X1)2 + (Y - Y1)2 = (R + R1)2
//   1)  <  (X - X2)2 + (Y - Y2)2 = (R - R2)2   2)  <  (X - X2)2 + (Y - Y2)2 = (R - R2)2
//        \_(X - X3)2 + (Y - Y3)2 = (R - R3)2        \_(X - X3)2 + (Y - Y3)2 = (R - R3)2
//         _                                          _
//        | (X - X1)2 + (Y - Y1)2 = (R - R1)2        | (X - X1)2 + (Y - Y1)2 = (R - R1)2
//   3)  <  (X - X2)2 + (Y - Y2)2 = (R + R2)2   4)  <  (X - X2)2 + (Y - Y2)2 = (R - R2)2
//        \_(X - X3)2 + (Y - Y3)2 = (R - R3)2        \_(X - X3)2 + (Y - Y3)2 = (R + R3)2
//         _                                          _
//        | (X - X1)2 + (Y - Y1)2 = (R + R1)2        | (X - X1)2 + (Y - Y1)2 = (R + R1)2
//   5)  <  (X - X2)2 + (Y - Y2)2 = (R + R2)2   6)  <  (X - X2)2 + (Y - Y2)2 = (R - R2)2
//        \_(X - X3)2 + (Y - Y3)2 = (R - R3)2        \_(X - X3)2 + (Y - Y3)2 = (R + R3)2
//         _                                          _
//        | (X - X1)2 + (Y - Y1)2 = (R - R1)2        | (X - X1)2 + (Y - Y1)2 = (R + R1)2
//   7)  <  (X - X2)2 + (Y - Y2)2 = (R + R2)2   8)  <  (X - X2)2 + (Y - Y2)2 = (R + R2)2
//        \_(X - X3)2 + (Y - Y3)2 = (R + R3)2        \_(X - X3)2 + (Y - Y3)2 = (R + R3)2

//   each equation (X - Xi)2 + (Y - Yi)2 = (R +- Ri)2 means that the circle (X,Y,R) is tangent 
//   to the circle (Xi,Yi,Ri).

//   The number of each system is very important. Further index i shows the numer of the system

//   Further Beta, Gamma and Delta are coefficients of the equation:
//                R +- Ri = Beta*X + Gamma*Y + Delta  where i=2 or i=3

//*********************************************************************************************
//*********************************************************************************************

//   Verification do two circles touch each other or not
//   if at least one circle touches other one IsTouch become Standard_Standard_True

  if (Abs((X1 - X2)*(X1 - X2) + (Y1 - Y2)*(Y1 - Y2) - (R1 - R2)*(R1 - R2)) <= Tolerance || 
      Abs((X1 - X2)*(X1 - X2) + (Y1 - Y2)*(Y1 - Y2) - (R1 + R2)*(R1 + R2)) <= Tolerance || 
      Abs((X1 - X3)*(X1 - X3) + (Y1 - Y3)*(Y1 - Y3) - (R1 - R3)*(R1 - R3)) <= Tolerance || 
      Abs((X1 - X3)*(X1 - X3) + (Y1 - Y3)*(Y1 - Y3) - (R1 + R3)*(R1 + R3)) <= Tolerance || 
      Abs((X2 - X3)*(X2 - X3) + (Y2 - Y3)*(Y2 - Y3) - (R2 - R3)*(R2 - R3)) <= Tolerance || 
      Abs((X2 - X3)*(X2 - X3) + (Y2 - Y3)*(Y2 - Y3) - (R2 + R3)*(R2 + R3)) <= Tolerance)
    IsTouch = Standard_True;
  else
    IsTouch = Standard_False;

//   First step:
//     We are searching for Beta, Gamma and Delta coefficients
//     and also coefficients of the system of second order equations:
//     _
//    |  a2*x*x +2*b2*x*y + c2*y*y +2*d2*x + 2*e2*y + f2 = 0
//   <
//    \_ a3*x*x +2*b3*x*y + c3*y*y +2*d3*x + 2*e3*y + f3 = 0   ,

//     obtained by exclusion of R from source systems.

  for (i = 1; i <= 8; i++) {

//    _
//   | (X - X1)2 + (Y - Y1)2 = (R +- R1)2
//  <
//   \_(X - X2)2 + (Y - Y2)2 = (R +- R2)2

    if (i == 1 || i == 4 || i == 5 || i == 8) {
      if (Abs(R1 - R2) > Tolerance) {
	Beta2(i) = (X1 - X2)/(R1 - R2);
	Gamma2(i) = (Y1 - Y2)/(R1 - R2);
	Delta2(i) = (X2*X2 - X1*X1 + Y2*Y2 - Y1*Y1 + (R1 - R2)*(R1 - R2))/(2*(R1 - R2));
      }
    } else {
      Beta2(i) = (X1 - X2)/(R1 + R2);
      Gamma2(i) = (Y1 - Y2)/(R1 + R2);
      Delta2(i) = (X2*X2 - X1*X1 + Y2*Y2 - Y1*Y1 + (R1 + R2)*(R1 + R2))/(2*(R1 + R2));
    }
    if ((i == 1 || i == 4 || i == 5 || i == 8) &&
	(Abs(R1 - R2) <= Tolerance)) {
//  If R1 = R2
      A2(i) = 0.;
      B2(i) = 0.;
      C2(i) = 0.;
      D2(i) = X2 - X1;
      E2(i) = Y2 - Y1;
      F2(i) = X1*X1 - X2*X2 + Y1*Y1 - Y2*Y2;
    } else {
      A2(i) = Beta2(i)*Beta2(i) - 1.;
      B2(i) = Beta2(i)*Gamma2(i);
      C2(i) = Gamma2(i)*Gamma2(i) - 1.;
      D2(i) = Beta2(i)*Delta2(i) + X2;
      E2(i) = Gamma2(i)*Delta2(i) + Y2;
      F2(i) = Delta2(i)*Delta2(i) - X2*X2 - Y2*Y2;
    }
    
//    _
//   | (X - X1)2 + (Y - Y1)2 = (R +- R1)2
//  <
//   \_(X - X3)2 + (Y - Y3)2 = (R +- R3)2
    
    if (i == 1 || i == 3 || i == 6 || i == 8) {
      if (Abs(R1 - R3) > Tolerance) {
	Beta3(i) = (X1 - X3)/(R1 - R3);
	Gamma3(i) = (Y1 - Y3)/(R1 - R3);
	Delta3(i) = (X3*X3 - X1*X1 + Y3*Y3 - Y1*Y1 + (R1 - R3)*(R1 - R3))/(2*(R1 - R3));
      }
    } else {
      Beta3(i) = (X1 - X3)/(R1 + R3);
      Gamma3(i) = (Y1 - Y3)/(R1 + R3);
      Delta3(i) = (X3*X3 - X1*X1 + Y3*Y3 - Y1*Y1 + (R1 + R3)*(R1 + R3))/(2*(R1 + R3));
    }
    if ((i == 1 || i == 3 || i == 6 || i == 8) &&
	(Abs(R1 - R3) <= Tolerance)) {
      A3(i) = 0.;
      B3(i) = 0.;
      C3(i) = 0.;
      D3(i) = X3 - X1;
      E3(i) = Y3 - Y1;
      F3(i) = X1*X1 - X3*X3 + Y1*Y1 - Y3*Y3;
    } else {
      A3(i) = Beta3(i)*Beta3(i) - 1.;
      B3(i) = Beta3(i)*Gamma3(i);
      C3(i) = Gamma3(i)*Gamma3(i) - 1.;
      D3(i) = Beta3(i)*Delta3(i) + X3;
      E3(i) = Gamma3(i)*Delta3(i) + Y3;
      F3(i) = Delta3(i)*Delta3(i) - X3*X3 - Y3*Y3;
    }
  }

//   Second step:
//     We are searching for the couple (X,Y) as a solution  of the system:
//     _
//    |  a2*x*x +2*b2*x*y + c2*y*y +2*d2*x + 2*e2*y + f2 = 0
//   <
//    \_ a3*x*x +2*b3*x*y + c3*y*y +2*d3*x + 2*e3*y + f3 = 0

  CurSol = 1;
  for (i = 1; i <= 8; i++) {
    a2 = A2(i);    a3 = A3(i);
    b2 = B2(i);    b3 = B3(i);
    c2 = C2(i);    c3 = C3(i);
    d2 = D2(i);    d3 = D3(i);
    e2 = E2(i);    e3 = E3(i);
    f2 = F2(i);    f3 = F3(i);

    FirstSol(i) = CurSol;

//  In some cases we know that some systems have no solution in any case due to qualifiers
    if (((i == 2 || i == 5 || i == 6 || i == 8) && 
	 (Qualified1.IsEnclosed() || Qualified1.IsEnclosing())) ||
	((i == 1 || i == 3 || i == 4 || i == 7) && Qualified1.IsOutside()))
      continue;

    if (((i == 3 || i == 5 || i == 7 || i == 8) && 
	 (Qualified2.IsEnclosed() || Qualified2.IsEnclosing())) ||
	((i == 1 || i == 2 || i == 4 || i == 6) && Qualified2.IsOutside()))
      continue;

    if (((i == 4 || i == 6 || i == 7 || i == 8) && 
	 (Qualified3.IsEnclosed() || Qualified3.IsEnclosing())) ||
	((i == 1 || i == 2 || i == 3 || i == 5) && Qualified3.IsOutside()))
      continue;

// Check is Cir1 a solution of this system or not
// In that case equations are equal to each other
    if (Abs(a2 - a3) <= Tolerance && Abs(b2 - b3) <= Tolerance && Abs(c2 - c3) <= Tolerance &&
	Abs(d2 - d3) <= Tolerance && Abs(e2 - e3) <= Tolerance && Abs(f2 - f3) <= Tolerance) {
      xSol(CurSol) = X1;
      ySol(CurSol) = Y1;
      CurSol++;
      continue;
    }
// 1) a2 = 0
    if (Abs(a2) <= Tolerance) {

// 1.1) b2y + d2 = 0
//   Searching for solution of the equation Ay2 + By + C = 0
      A = c2; B = 2.*e2; C = f2;
      math_DirectPolynomialRoots yRoots(A, B, C);
      if (yRoots.IsDone() && !yRoots.InfiniteRoots())
	for (k = 1; k <= yRoots.NbSolutions(); k++) {
//   for each y solution:
	  y = yRoots.Value(k);
//   Searching for solution of the equation Ax2 + Bx + C = 0
	  if (!(k == 2 && Abs(y - yRoots.Value(1)) <= 10*Tolerance) && 
	      Abs(b2*y + d2) <= b2*Tolerance) {
	    A = a3; B = 2*(b3*y + d3); C = c3*(y*y) + 2*e3*y + f3;
	    math_DirectPolynomialRoots xRoots(A, B, C);
	    if (xRoots.IsDone() && !xRoots.InfiniteRoots())
	      for (j = 1; j <= xRoots.NbSolutions(); j++) {
		x = xRoots.Value(j);
		if (!(j == 2 && Abs(x - xRoots.Value(1)) <= 10*Tolerance)) {
		  xSol(CurSol) = x;
		  ySol(CurSol) = y;
		  CurSol++;
		}
	      }
	  }
	}

// 1.2) b2y + d2 != 0
      A = a3*c2*c2 - 4*b2*(b3*c2 - b2*c3);
      B = 4*a3*c2*e2 - 4*b3*(c2*d2 + 2*b2*e2) + 4*b2*(2*c3*d2 - c2*d3 + 2*b2*e3);
      C = 2*a3*(c2*f2 + 2*e2*e2) - 4*b3*(b2*f2 + 2*e2*d2) + 4*c3*d2*d2 - 4*d3*(c2*d2 + 2*b2*e2) 
	+ 16*b2*e3*d2 + 4*b2*b2*f3;
      D = 4*a3*e2*f2 - 4*b3*d2*f2 - 4*d3*(b2*f2 + 2*d2*e2) + 8*d2*d2*e3 + 8*b2*d2*f3;
      E = a3*f2*f2 - 4*d2*d3*f2 + 4*d2*d2*f3;

//   Searching for solution of the equation Ay4 + By3 + Cy2 + Dy + E = 0
// Special case: one circle touches other 
      if (IsTouch) {
// Derivation of the equation Ay4 + By3 + Cy2 + Dy + E
	math_DirectPolynomialRoots yRoots1(4*A, 3*B, 2*C, D);
	if (yRoots1.IsDone() && !yRoots1.InfiniteRoots())
	  for (k = 1; k <= yRoots1.NbSolutions(); k++) {
	    y = yRoots1.Value(k);
// Check if this value is already catched
	    IsSame = Standard_False;
	    for (l = 1; l < k; l++)
	      if (Abs(y - yRoots1.Value(l)) <= 10*Tolerance) IsSame = Standard_True;
	    
	    Epsilon = (Abs((Abs((Abs(4*A*y) + Abs(3*B))*y) + Abs(2*C))*y) + Abs(D));
	    if (Abs((((A*y + B)*y + C)*y + D)*y + E) <= Epsilon*Tolerance) {
	      if (!IsSame && Abs(b2*y + d2) > b2*Tolerance) {
		x = -(c2*(y*y) + 2*e2*y + f2)/(2*(b2*y + d2));
		xSol(CurSol) = x;
		ySol(CurSol) = y;
		CurSol++;
	      }
	    }
	  }
      }

      math_DirectPolynomialRoots yRoots1(A, B, C, D, E);
      if (yRoots1.IsDone() && !yRoots1.InfiniteRoots())
	for (k = 1; k <= yRoots1.NbSolutions(); k++) {
	  y = yRoots1.Value(k);
// Check if this value is already catched
	  IsSame = Standard_False;
	  FirstIndex = (i == 1) ? 1 : FirstSol(i);
	  for (l = FirstIndex; l < CurSol; l++)
	    if (Abs(y - ySol(l)) <= 10*Tolerance) IsSame = Standard_True;

	  if (!IsSame && Abs(b2*y + d2) > b2*Tolerance) {
	    x = -(c2*(y*y) + 2*e2*y + f2)/(2*(b2*y + d2));
	    xSol(CurSol) = x;
	    ySol(CurSol) = y;
	    CurSol++;
	  }
	}
    } else {
// 2) a2 != 0
// Coefficients of the equation     (sy + v)Sqrt(p2 - q) = (my2 + ny + t)
      m = 2*a3*b2*b2/(a2*a2) - 2*b2*b3/a2 - a3*c2/a2 + c3;
      n = 4*a3*b2*d2/(a2*a2) - 2*b3*d2/a2 - 2*b2*d3/a2 - 2*a3*e2/a2 + 2*e3;
      t = 2*a3*d2*d2/(a2*a2) - 2*d2*d3/a2 - a3*f2/a2 + f3;
      s = 2*b3 - 2*a3*b2/a2;
      v = 2*d3 - 2*d2*a3/a2;

//------------------------------------------
// If s = v = 0
      if (Abs(s) <= Tolerance && Abs(v) <= Tolerance) {
	math_DirectPolynomialRoots yRoots(m, n, t);
	if (yRoots.IsDone() && !yRoots.InfiniteRoots())
	  for (k = 1; k <= yRoots.NbSolutions(); k++) {
//   for each y solution:
	    y = yRoots.Value(k);

	    p = -(b2*y + d2)/a2;
	    q = (c2*(y*y) + 2*e2*y + f2)/a2;
	    Epsilon = 2.*(Abs((b2*b2 + Abs(a2*c2))*y) + Abs(b2*d2) + Abs(a2*e2))/(a2*a2);
	    if (!(k == 2 && Abs(y - yRoots.Value(1)) <= 10*Tolerance) &&
		p*p - q >= -Epsilon*Tolerance) {
	      A = a2; 
	      B = 2*(b2*y + d2);
	      C = c2*y*y + 2*e2*y + f2;
	      math_DirectPolynomialRoots xRoots(A, B, C);
	      if (xRoots.IsDone() && !xRoots.InfiniteRoots())
		for (l = 1; l <= xRoots.NbSolutions(); l++) {
//   for each x solution:
		  x = xRoots.Value(l);

		  if (!(l == 2 && Abs(x - xRoots.Value(1)) <= 10*Tolerance)) {
		    xSol(CurSol) = x;
		    ySol(CurSol) = y;
		    CurSol++;
		  }
		}
	    }
	  }
      } else {
//------------------------------------------
// If (s*y + v) != 0

	A = s*s*(b2*b2 - a2*c2) - m*m*a2*a2;
	B = 2*s*v*(b2*b2 - a2*c2) + 2*s*s*(b2*d2 - a2*e2) - 2*m*n*a2*a2;
	C = v*v*(b2*b2 - a2*c2) + 4*s*v*(b2*d2 - a2*e2) + s*s*(d2*d2 - a2*f2) - a2*a2*(2*m*t + n*n);
	D = 2*v*v*(b2*d2 - a2*e2) + 2*s*v*(d2*d2 - a2*f2) - 2*n*t*a2*a2;
	E = v*v*(d2*d2 - a2*f2) - t*t*a2*a2;

//   Searching for solution of the equation Ay4 + By3 + Cy2 + Dy + E = 0
// Special case: one circle touches other 
	if (IsTouch) {
// Derivation of the equation Ay4 + By3 + Cy2 + Dy + E
	  math_DirectPolynomialRoots yRoots1(4*A, 3*B, 2*C, D);
	  if (yRoots1.IsDone() && !yRoots1.InfiniteRoots())
	    for (k = 1; k <= yRoots1.NbSolutions(); k++) {
	      y = yRoots1.Value(k);

	      p = -(b2*y + d2)/a2;
	      q = (c2*(y*y) + 2*e2*y + f2)/a2;

// Check if this value is already catched
	      IsSame = Standard_False;
	      FirstIndex = (i == 1) ? 1 : FirstSol(i);
	      for (l = FirstIndex; l < CurSol; l++)
		if (Abs(y - ySol(l)) <= 10*Tolerance) IsSame = Standard_True;

	      Epsilon = (Abs((Abs((Abs(4*A*y) + Abs(3*B))*y) + Abs(2*C))*y) + Abs(D));
	      if (Abs((((A*y + B)*y + C)*y + D)*y + E) <= Epsilon*Tolerance) {

		Epsilon = 2.*(Abs((b2*b2 + Abs(a2*c2))*y) + Abs(b2*d2) + Abs(a2*e2))/(a2*a2);
		if (!IsSame && p*p - q >= -Epsilon*Tolerance) {
		  A = a2; 
		  B = 2*(b2*y + d2);
		  C = c2*y*y + 2*e2*y + f2;
		  math_DirectPolynomialRoots xRoots(A, B, C);
		  if (xRoots.IsDone() && !xRoots.InfiniteRoots())
		    for (l = 1; l <= xRoots.NbSolutions(); l++) {
//   for each x solution:
		      x = xRoots.Value(l);

		      if (!(l == 2 && Abs(x - xRoots.Value(1)) <= 10*Tolerance)) {
			xSol(CurSol) = x;
			ySol(CurSol) = y;
			CurSol++;
		      }
		    }
		}
	      }
	    }
	}

	math_DirectPolynomialRoots yRoots(A, B, C, D, E);
	if (yRoots.IsDone() && !yRoots.InfiniteRoots())
	  for (k = 1; k <= yRoots.NbSolutions(); k++) {
//   for each y solution:
	    y = yRoots.Value(k);

	    p = -(b2*y + d2)/a2;
	    q = (c2*(y*y) + 2*e2*y + f2)/a2;

// Check if this value is already catched
	    IsSame = Standard_False;
	    for (l = 1; l < k; l++)
	      if (Abs(y - yRoots.Value(l)) <= 10*Tolerance) IsSame = Standard_True;

	    Epsilon = 2.*(Abs((b2*b2 + Abs(a2*c2))*y) + Abs(b2*d2) + Abs(a2*e2))/(a2*a2);
	    if (!IsSame && p*p - q >= -Epsilon*Tolerance) {
	      A = a2; 
	      B = 2*(b2*y + d2);
	      C = c2*y*y + 2*e2*y + f2;
	      math_DirectPolynomialRoots xRoots(A, B, C);
	      if (xRoots.IsDone() && !xRoots.InfiniteRoots())
		for (l = 1; l <= xRoots.NbSolutions(); l++) {
//   for each x solution:
		  x = xRoots.Value(l);

		  if (!(l == 2 && Abs(x - xRoots.Value(1)) <= 10*Tolerance)) {
		    xSol(CurSol) = x;
		    ySol(CurSol) = y;
		    CurSol++;
		  }
		}
	    }
	  }
      }
    }
  }
  FirstSol(9) = CurSol;

//  Third step:
//    Check of couples (X,Y) and searching for R. R must be great than 0
  CurSol = 1;
  for (i = 1; i <= 8; i++) {
    FirstSol1(i) = CurSol;
    for (j = FirstSol(i); j < FirstSol(i + 1); j++) {
      x = xSol(j);
      y = ySol(j);
// in some cases when R1 = R2 :
      if ((i == 1 || i == 4 || i == 5 || i == 8) && (Abs(R1 - R2) <= Tolerance)) {
	if (i == 1 || i == 4) {
	  r = R1 + Sqrt((x - X1)*(x - X1) + (y - Y1)*(y - Y1));
	  Epsilon = 10*(2*Abs(r - R2) + Abs(x - X2) + Abs(y - Y2));
	  if (Abs((r - R2)*(r - R2) - (x - X2)*(x - X2) - (y - Y2)*(y - Y2)) <= 
	      Epsilon*Tolerance) {
	    xSol1(CurSol) = x;
	    ySol1(CurSol) = y;
	    rSol1(CurSol) = r;
	    CurSol++;
	  }
	  r = R1 - Sqrt((x - X1)*(x - X1) + (y - Y1)*(y - Y1));
	  Epsilon = 10*(2*Abs(r - R2) + Abs(x - X2) + Abs(y - Y2));
	  if ((r > Tolerance) && 
	      (Abs((r - R2)*(r - R2) - (x - X2)*(x - X2) - (y - Y2)*(y - Y2)) <= 
	       Epsilon*Tolerance)) {
	    xSol1(CurSol) = x;
	    ySol1(CurSol) = y;
	    rSol1(CurSol) = r;
	    CurSol++;
	  }
	} else {
//	i == 5 || i == 8
	  r =  - R1 + Sqrt((x - X1)*(x - X1) + (y - Y1)*(y - Y1));
	  if (r > Tolerance) {
	    xSol1(CurSol) = x;
	    ySol1(CurSol) = y;
	    rSol1(CurSol) = r;
	    CurSol++;
	  }
	}
      } else {
// Other cases
	if (i == 1 || i == 4) {
	  r = R2 + Beta2(i)*x + Gamma2(i)*y + Delta2(i);
	  if (r > Tolerance) {
	    xSol1(CurSol) = x;
	    ySol1(CurSol) = y;
	    rSol1(CurSol) = r;
	    CurSol++;
	  }
	}
	if (i == 5 || i == 8) {
	  r = -R2 - Beta2(i)*x - Gamma2(i)*y - Delta2(i);
	  if (r > Tolerance) {
	    xSol1(CurSol) = x;
	    ySol1(CurSol) = y;
	    rSol1(CurSol) = r;
	    CurSol++;
	  }
	}
	if (i == 3 || i == 7) {
	  r = - R2 + Beta2(i)*x + Gamma2(i)*y + Delta2(i);
	  if (r > Tolerance) {
	    xSol1(CurSol) = x;
	    ySol1(CurSol) = y;
	    rSol1(CurSol) = r;
	    CurSol++;
	  }
	}
	if (i == 2 || i == 6) {
	  r = R2 - Beta2(i)*x - Gamma2(i)*y - Delta2(i);
	  if (r > Tolerance) {
	    xSol1(CurSol) = x;
	    ySol1(CurSol) = y;
	    rSol1(CurSol) = r;
	    CurSol++;
	  }
	}
      }
    }
  }
  FirstSol1(9) = CurSol;
//   Fourth step
//     Check of triplets (X,Y,R).
  CurSol = 1;
  for (i = 1; i <= 8; i++) {
    FirstSol(i) = CurSol;
    for (j = FirstSol1(i); j < FirstSol1(i + 1); j++) {
      x = xSol1(j);
      y = ySol1(j);
      r = rSol1(j);
// in some cases when R1 = R3 :
      if ((i == 1 || i == 3 || i == 6 || i == 8) && Abs(R1 - R3) <= Tolerance) {
	if (i == 1 || i == 3) {
	  Epsilon = 10*(2*Abs(r - R3) + Abs(x - X3) + Abs(y - Y3));
	  if (Abs((r - R3)*(r - R3) - (x - X3)*(x - X3) - (y - Y3)*(y - Y3)) <= 
	      Epsilon*Tolerance) {
	    xSol(CurSol) = x;
	    ySol(CurSol) = y;
	    rSol(CurSol) = r;
	    CurSol++;
	  }
	} else {
//	i == 6 || i == 8
	  Epsilon = 10*(2*(r + R3) + Abs(x - X3) + Abs(y - Y3));
	  if (Abs((r + R3)*(r + R3) - (x - X3)*(x - X3) - (y - Y3)*(y - Y3)) <= 
	      Epsilon*Tolerance) {
 	    xSol(CurSol) = x;
	    ySol(CurSol) = y;
	    rSol(CurSol) = r;
	    CurSol++;
	  }
	}
      } else {
// Other cases
	Epsilon = 10*(Abs(Beta3(i)) + Abs(Gamma3(i)) + 1.);
	if (i == 1 || i == 3)
	  if (Abs(R3 + Beta3(i)*x + Gamma3(i)*y + Delta3(i) - r) <= Epsilon*Tolerance) {
	    xSol(CurSol) = x;
	    ySol(CurSol) = y;
	    rSol(CurSol) = r;
	    CurSol++;
	  }
	if (i == 6 || i == 8)
	  if (Abs(R3 + Beta3(i)*x + Gamma3(i)*y + Delta3(i) + r) <= Epsilon*Tolerance) {
	    xSol(CurSol) = x;
	    ySol(CurSol) = y;
	    rSol(CurSol) = r;
	    CurSol++;
	  }
	if (i == 4 || i == 7)
	  if (Abs(Beta3(i)*x + Gamma3(i)*y + Delta3(i) - r - R3) <= Epsilon*Tolerance) {
	    xSol(CurSol) = x;
	    ySol(CurSol) = y;
	    rSol(CurSol) = r;
	    CurSol++;
	  }
	if (i == 2 || i == 5)
	  if (Abs(r - R3 + Beta3(i)*x + Gamma3(i)*y + Delta3(i)) <= Epsilon*Tolerance) {
	    xSol(CurSol) = x;
	    ySol(CurSol) = y;
	    rSol(CurSol) = r;
	    CurSol++;
	  }
      }
    }
  }
  FirstSol(9) = CurSol;

//  Fifth step:
//    We have found all solutions. We have to calculate some parameters for each one.
  for (i = 1 ; i <= 8; i++) {
    for (j = FirstSol(i); j < FirstSol(i + 1); j++) {

      if ((Qualified1.IsEnclosed() && rSol(j) > R1) ||
	  (Qualified1.IsEnclosing() && rSol(j) < R1))
	continue;
      if ((Qualified2.IsEnclosed() && rSol(j) > R2) ||
	  (Qualified2.IsEnclosing() && rSol(j) < R2))
 	continue;
      if ((Qualified3.IsEnclosed() && rSol(j) > R3) ||
	  (Qualified3.IsEnclosing() && rSol(j) < R3))
 	continue;

      NbrSol++;

// RLE, avoid out of range
      if (NbrSol > cirsol.Upper()) NbrSol = cirsol.Upper();
      
      gp_Pnt2d Center = gp_Pnt2d(xSol(j), ySol(j));

      cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center,dirx),rSol(j));

//   ==========================================================
      Standard_Real distcc1 = Center.Distance(center1);
      if (!Qualified1.IsUnqualified())
	qualifier1(NbrSol) = Qualified1.Qualifier();
      else if (Abs(distcc1 + rSol(j) - R1) <= Tol)
	qualifier1(NbrSol) = GccEnt_enclosed;
      else if (Abs(distcc1 - R1 - rSol(j)) <= Tol)
	qualifier1(NbrSol) = GccEnt_outside;
      else qualifier1(NbrSol) = GccEnt_enclosing;

      Standard_Real distcc2 = Center.Distance(center1);
      if (!Qualified2.IsUnqualified())
	qualifier2(NbrSol) = Qualified2.Qualifier();
      else if (Abs(distcc2 + rSol(j) - R2) <= Tol)
	qualifier2(NbrSol) = GccEnt_enclosed;
      else if (Abs(distcc2 - R2 - rSol(j)) <= Tol)
	qualifier2(NbrSol) = GccEnt_outside;
      else qualifier2(NbrSol) = GccEnt_enclosing;

      Standard_Real distcc3 = Center.Distance(center1);
      if (!Qualified3.IsUnqualified())
	qualifier3(NbrSol) = Qualified3.Qualifier();
      else if (Abs(distcc3 + rSol(j) - R3) <= Tol)
	qualifier3(NbrSol) = GccEnt_enclosed;
      else if (Abs(distcc3 - R3 - rSol(j)) <= Tol)
	qualifier3(NbrSol) = GccEnt_outside;
      else qualifier3(NbrSol) = GccEnt_enclosing;

//   ==========================================================

      if (Center.Distance(Cir1.Location()) <= Tolerance)
	TheSame1(NbrSol) = 1;
      else {
	TheSame1(NbrSol) = 0;
	gp_Dir2d dc;
	if ((i == 2 || i == 5 || i == 6 || i == 8) || rSol(j) > R1)
	  dc.SetXY(Cir1.Location().XY() - Center.XY());
	else
	  dc.SetXY(Center.XY() - Cir1.Location().XY());
	pnttg1sol(NbrSol)=gp_Pnt2d(Center.XY() + rSol(j)*dc.XY());
	par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
					  pnttg1sol(NbrSol));
	pararg1(NbrSol)=ElCLib::Parameter(Cir1,pnttg1sol(NbrSol));
      }

      if (Center.Distance(Cir2.Location()) <= Tolerance)
	TheSame2(NbrSol) = 1;
      else {
	TheSame2(NbrSol) = 0;
	gp_Dir2d dc;
	if ((i == 3 || i == 5 || i == 7 || i == 8) || rSol(j) > R2)
	  dc.SetXY(Cir2.Location().XY() - Center.XY());
	else
	  dc.SetXY(Center.XY() - Cir2.Location().XY());
	pnttg2sol(NbrSol)=gp_Pnt2d(Center.XY() + rSol(j)*dc.XY());
	par2sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
					  pnttg2sol(NbrSol));
	pararg2(NbrSol)=ElCLib::Parameter(Cir2,pnttg2sol(NbrSol));
      }

      if (Center.Distance(Cir3.Location()) <= Tolerance)
	TheSame3(NbrSol) = 1;
      else {
	TheSame3(NbrSol) = 0;
	gp_Dir2d dc;
	if ((i == 4 || i == 6 || i == 7 || i == 8) || rSol(j) > R3)
	  dc.SetXY(Cir3.Location().XY() - Center.XY());
 	else
	  dc.SetXY(Center.XY() - Cir3.Location().XY());
	pnttg3sol(NbrSol)=gp_Pnt2d(Center.XY() + rSol(j)*dc.XY());
	par3sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
					  pnttg3sol(NbrSol));
	pararg3(NbrSol)=ElCLib::Parameter(Cir3,pnttg3sol(NbrSol));
      }
    }
  }
  WellDone = Standard_True;
}

//=========================================================================

Standard_Boolean GccAna_Circ2d3Tan::
   IsDone () const {
   return WellDone;
 }

Standard_Integer GccAna_Circ2d3Tan::
   NbSolutions () const {
   return NbrSol;
 }

gp_Circ2d GccAna_Circ2d3Tan::
   ThisSolution (const Standard_Integer Index) const 
{
  if (!WellDone)
    throw StdFail_NotDone();
  
  if (Index <= 0 ||Index > NbrSol)
    throw Standard_OutOfRange();
  
  return cirsol(Index); 
}

void GccAna_Circ2d3Tan::
  WhichQualifier(const Standard_Integer Index   ,
		       GccEnt_Position& Qualif1 ,
		       GccEnt_Position& Qualif2 ,
		       GccEnt_Position& Qualif3 ) const
{
  if (!WellDone) { throw StdFail_NotDone(); }
   else if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
   else {
     Qualif1 = qualifier1(Index);
     Qualif2 = qualifier2(Index);
     Qualif3 = qualifier3(Index);
   }
}

void GccAna_Circ2d3Tan::
   Tangency1 (const Standard_Integer Index,
              Standard_Real& ParSol,
              Standard_Real& ParArg,
              gp_Pnt2d& PntSol) const {
   if (!WellDone) {
     throw StdFail_NotDone();
   }
   else if (Index <= 0 ||Index > NbrSol) {
     throw Standard_OutOfRange();
   }
   else {
     if (TheSame1(Index) == 0) {
       ParSol = par1sol(Index);
       ParArg = pararg1(Index);
       PntSol = gp_Pnt2d(pnttg1sol(Index));
     }
     else { throw StdFail_NotDone(); }
   }
 }

void GccAna_Circ2d3Tan::
   Tangency2 (const Standard_Integer Index,
              Standard_Real& ParSol,
              Standard_Real& ParArg,
              gp_Pnt2d& PntSol) const{
   if (!WellDone) {
     throw StdFail_NotDone();
   }
   else if (Index <= 0 ||Index > NbrSol) {
     throw Standard_OutOfRange();
   }
   else {
     if (TheSame2(Index) == 0) {
       ParSol = par2sol(Index);
       ParArg = pararg2(Index);
       PntSol = gp_Pnt2d(pnttg2sol(Index));
     }
     else { throw StdFail_NotDone(); }
   }
 }

void GccAna_Circ2d3Tan::
   Tangency3 (const Standard_Integer Index,
              Standard_Real& ParSol,
              Standard_Real& ParArg,
              gp_Pnt2d& PntSol) const{
   if (!WellDone) {
     throw StdFail_NotDone();
   }
   else if (Index <= 0 ||Index > NbrSol) {
     throw Standard_OutOfRange();
   }
   else {
     if (TheSame3(Index) == 0) {
       ParSol = par3sol(Index);
       ParArg = pararg3(Index);
       PntSol = gp_Pnt2d(pnttg3sol(Index));
     }
     else { throw StdFail_NotDone(); }
   }
 }

Standard_Boolean GccAna_Circ2d3Tan::
   IsTheSame1 (const Standard_Integer Index) const
{
  if (!WellDone)
    throw StdFail_NotDone();
  
  if (Index <= 0 ||Index > NbrSol)
    throw Standard_OutOfRange();
  
  if (TheSame1(Index) == 0)
    return Standard_False;
  
  return Standard_True;
}

Standard_Boolean GccAna_Circ2d3Tan::
   IsTheSame2 (const Standard_Integer Index) const
{
  if (!WellDone)
    throw StdFail_NotDone();

  if (Index <= 0 ||Index > NbrSol)
    throw Standard_OutOfRange();

  if (TheSame2(Index) == 0)
    return Standard_False;

  return Standard_True;
}

Standard_Boolean GccAna_Circ2d3Tan::
   IsTheSame3 (const Standard_Integer Index) const
{
  if (!WellDone)
    throw StdFail_NotDone();
  
  if (Index <= 0 ||Index > NbrSol)
    throw Standard_OutOfRange();

  if (TheSame3(Index) == 0) 
    return Standard_False;
  
  return Standard_True;
}


