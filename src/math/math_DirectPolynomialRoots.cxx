// Copyright (c) 1997-1999 Matra Datavision
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

//#ifndef OCCT_DEBUG
#define No_Standard_RangeError
#define No_Standard_OutOfRange
#define No_Standard_DimensionError

//#endif

#include <math_DirectPolynomialRoots.hxx>
#include <Standard_RangeError.hxx>
#include <StdFail_InfiniteSolutions.hxx>

// Reference pour solution equation 3ieme degre et 2ieme degre :
//     ALGORITHMES NUMERIQUES ANALYSE ET MISE EN OEUVRE, tome 2
//          (equations et systemes non lineaires)
// J. VIGNES editions TECHNIP.
const Standard_Real ZERO = 1.0e-30;
    const Standard_Real EPSILON = RealEpsilon();
    const Standard_Real RADIX = 2;
    const Standard_Real Un_Sur_Log_RADIX = 1.0/log(2.0);

    static Standard_Real Value(const Standard_Integer N, Standard_Real *Poly, const Standard_Real X) {
 
        Standard_Real Result = Poly[0];
        for(Standard_Integer Index = 1; Index < N; Index++) {
          Result = Result * X + Poly[Index];
        }
        return Result;
    }


    static void Values(const Standard_Integer N, Standard_Real *Poly, const Standard_Real X,
                       Standard_Real& Val, Standard_Real& Der) {
 
        Val = Poly[0] * X + Poly[1];
        Der = Poly[0];
        for(Standard_Integer Index = 2; Index < N; Index++) {
          Der = Der * X + Val;
          Val = Val * X + Poly[Index];
        }
    }

    static Standard_Real Improve(const Standard_Integer N, Standard_Real *Poly, const Standard_Real IniSol) {

        Standard_Real Val = 0., Der, Delta;
        Standard_Real Sol = IniSol;
        Standard_Real IniVal = Value(N, Poly, IniSol);
        Standard_Integer Index;

//      cout << "Improve\n";
        for(Index = 1; Index < 10; Index++) {
          Values(N, Poly, Sol, Val, Der);
          if(Abs(Der) <= ZERO) break;
          Delta = - Val / Der;
          if(Abs(Delta) <= EPSILON * Abs(Sol)) break;
          Sol = Sol + Delta;
//        cout << " Iter = " << Index << " Delta = " << Delta 
//             << " Val  = " << Val   << " Der   = " << Der << "\n";
        }
        if(Abs(Val) <= Abs(IniVal)) {
          return Sol;
        }
        else {
          return IniSol;
        }
    }

    Standard_Real Improve(const Standard_Real A, const Standard_Real B, const Standard_Real C,
                 const Standard_Real D, const Standard_Real E, const Standard_Real IniSol) {
      
        Standard_Real Poly[5];
        Poly[0] = A;
        Poly[1] = B;
        Poly[2] = C;
        Poly[3] = D;
        Poly[4] = E;
         return Improve(5, Poly, IniSol);
    }

    Standard_Real Improve(const Standard_Real A, const Standard_Real B, 
                 const Standard_Real C, const Standard_Real D, const Standard_Real IniSol) {
      
        Standard_Real Poly[4];
        Poly[0] = A;
        Poly[1] = B;
        Poly[2] = C;
        Poly[3] = D;
        return Improve(4, Poly, IniSol);
    }

    Standard_Real Improve(const Standard_Real A, const Standard_Real B, 
                 const Standard_Real C, const Standard_Real IniSol) {
      
        Standard_Real Poly[3];
        Poly[0] = A;
        Poly[1] = B;
        Poly[2] = C;
        return Improve(3, Poly, IniSol);
    }

    Standard_Integer BaseExponent(const Standard_Real X) {

        if(X > 1.0) {
          return (Standard_Integer)(log(X) * Un_Sur_Log_RADIX);
        }
        else if(X < -1.0) {
          return (Standard_Integer)(-log(-X) * Un_Sur_Log_RADIX);
        }
        else {
          return 0;
        }
    }


    math_DirectPolynomialRoots::math_DirectPolynomialRoots(const Standard_Real A,
			       const Standard_Real B,
			       const Standard_Real C,
			       const Standard_Real D,
			       const Standard_Real E) {
      InfiniteStatus = Standard_False;
      Done = Standard_True;
      Solve(A, B, C, D, E);
    }

math_DirectPolynomialRoots::math_DirectPolynomialRoots(const Standard_Real A,
						       const Standard_Real B,
						       const Standard_Real C,
						       const Standard_Real D) {
  Done = Standard_True;
  InfiniteStatus = Standard_False;
  Solve(A, B, C, D);
}

math_DirectPolynomialRoots::math_DirectPolynomialRoots(const Standard_Real A,
						       const Standard_Real B,
						       const Standard_Real C) {
  Done = Standard_True;
  InfiniteStatus = Standard_False;
  Solve(A, B, C);
}

math_DirectPolynomialRoots::math_DirectPolynomialRoots(const Standard_Real A,
						       const Standard_Real B) {
  Done = Standard_True;
  InfiniteStatus = Standard_False;
  Solve(A, B);
}


void math_DirectPolynomialRoots::Solve(const Standard_Real a,
				       const Standard_Real b,
				       const Standard_Real c,
				       const Standard_Real d,
				       const Standard_Real e) {
  if(Abs(a) <= ZERO) {
    Solve(b, c, d, e);
    return;
  }        

  //// modified by jgv, 22.01.09 ////
  Standard_Real aZero = ZERO;
  Standard_Real Abs_b = Abs(b), Abs_c = Abs(c), Abs_d = Abs(d), Abs_e = Abs(e);

  if (Abs_b > aZero)
    aZero = Abs_b;
  if (Abs_c > aZero)
    aZero = Abs_c;
  if (Abs_d > aZero)
    aZero = Abs_d;
  if (Abs_e > aZero)
    aZero = Abs_e;
  if (aZero > ZERO)
    aZero = Epsilon(100.*aZero);

  if(Abs(a) <= aZero) {
    Standard_Real aZero1000 = 1000.*aZero;
    Standard_Boolean with_a = Standard_False;
    if (Abs_b > ZERO && Abs_b <= aZero1000)
      with_a = Standard_True;
    if (Abs_c > ZERO && Abs_c <= aZero1000)
      with_a = Standard_True;
    if (Abs_d > ZERO && Abs_d <= aZero1000)
      with_a = Standard_True;
    if (Abs_e > ZERO && Abs_e <= aZero1000)
      with_a = Standard_True;

    if (!with_a)
      {
	Solve(b, c, d, e);
	return;
      }
  }        
  ///////////////////////////////////

  Standard_Real A, B, C, D, R3, S3, T3, Q3, Y0, P0, Q0, P, Q, P1, Q1;
  Standard_Real Discr, Sdiscr;
  Standard_Integer Index;
  Standard_Integer Exp;
  Standard_Real PowRadix1,PowRadix2;
  
  A = b / a;
  B = c / a;
  C = d / a;
  D = e / a;
  Exp = BaseExponent(D) / 4;
  //-- 
  //-- A = A / pow(RADIX, Exp);
  //-- B = B / pow(RADIX, 2 * Exp);
  //-- C = C / pow(RADIX, 3 * Exp);
  //-- D = D / pow(RADIX, 4 * Exp);
  PowRadix1 = pow(RADIX,Exp);
  A/= PowRadix1;  PowRadix2 = PowRadix1 * PowRadix1;
  B/= PowRadix2;
  C/= PowRadix2 * PowRadix1;
  D/= PowRadix2 * PowRadix2;
  //-- 
  R3 = -B;
  S3 = A * C - 4.0 * D;
  T3 = D * (4.0 * B - A * A) - C * C;
  Q3 = 1.0;                
  math_DirectPolynomialRoots Sol3(Q3, R3, S3, T3);
  //-- ################################################################################
  if(Sol3.IsDone() == Standard_False) { Done = Standard_False; return; } 
  //-- ################################################################################


  Y0 = Sol3.Value(1);
  for(Index = 2; Index <= Sol3.NbSolutions(); Index++) {
    if(Sol3.Value(Index) > Y0) Y0 = Sol3.Value(Index);
  }
  Discr = A * Y0 * 0.5 - C;
  if(Discr >= 0.0) { 
    Sdiscr = 1.0;
  }
  else {
    Sdiscr = -1.0;
  }
  P0 = A * A * 0.25 - B + Y0;
  if(P0 < 0.0) P0 = 0.0; 
  P0 = sqrt(P0);
  Q0 = Y0 * Y0 * 0.25 - D;
  if(Q0 < 0.0) Q0 = 0.0; 
  Q0 = sqrt(Q0);

  Standard_Real Ademi    = A  * 0.5;
  Standard_Real Ydemi    = Y0 * 0.5;
  Standard_Real SdiscrQ0 = Sdiscr * Q0;

  P = Ademi + P0;
  Q = Ydemi + SdiscrQ0;
  P1 = Ademi - P0;
  Q1 = Ydemi - SdiscrQ0;
//  Modified by skv - Wed Apr 14 16:05:24 2004 IDEM(Airbus) Begin
  Standard_Real eps;

  eps = Epsilon(100.*Max(Ademi, P0));
  if (Abs(P) <= eps)
    P = 0.;
  if (Abs(P1) <= eps)
    P1 = 0.;

  eps = Epsilon(100.*Max(Ydemi, SdiscrQ0));
  if (Abs(Q) <= eps)
    Q = 0.;
  if (Abs(Q1) <= eps)
    Q1 = 0.;
//  Modified by skv - Wed Apr 14 16:05:24 2004 IDEM(Airbus) End
  Ademi = 1.0;

  math_DirectPolynomialRoots ASol2(Ademi, P,  Q);
  //-- ################################################################################
  if(ASol2.IsDone() == Standard_False) { Done = Standard_False; return; } 
  //-- ################################################################################
  math_DirectPolynomialRoots BSol2(Ademi, P1,  Q1);
  //-- ################################################################################
  if(BSol2.IsDone() == Standard_False) { Done = Standard_False; return; } 
  //-- ################################################################################

  NbSol = ASol2.NbSolutions() + BSol2.NbSolutions();
  for(Index = 0; Index < ASol2.NbSolutions(); Index++) {
    TheRoots[Index] = ASol2.TheRoots[Index];
  }
  for(Index = 0; Index < BSol2.NbSolutions(); Index++) {
    TheRoots[ASol2.NbSolutions() + Index] = BSol2.TheRoots[Index];
  }
  for(Index = 0; Index < NbSol; Index++) {
    TheRoots[Index] = TheRoots[Index] * PowRadix1;
    TheRoots[Index] = Improve(a, b, c, d, e, TheRoots[Index]);
  }
}

    void math_DirectPolynomialRoots::Solve(const Standard_Real A,
                                           const Standard_Real B,
                                           const Standard_Real C,
                                           const Standard_Real D) { 

      if(Abs(A) <= ZERO) {
        Solve(B, C, D);
        return;
      }

      Standard_Real Beta, Gamma, Del, P1, P2, P, Ep, Q1, Q2, Q3, Q, Eq, A1, A2, Discr;
      Standard_Real Sigma, Psi, D1, D2, Sb, Omega, Sp3, Y1, Dbg, Sdbg, Den1, Den2;
      Standard_Real U, H, Sq;
      Standard_Integer Exp;

      Beta  = B / A;
      Gamma = C / A;
      Del   = D / A;

      Exp = BaseExponent(Del) / 3;

      Standard_Real PowRadix1 = pow(RADIX,Exp);
      Standard_Real PowRadix2 = PowRadix1*PowRadix1;
      Beta/=   PowRadix1;                   
      Gamma/=  PowRadix2;
      Del/=    PowRadix2 * PowRadix1;
      //-- Beta  = Beta  / pow(RADIX, Exp);
      //-- Gamma = Gamma / pow(RADIX, 2 * Exp);
      //-- Del   = Del   / pow(RADIX, 3 * Exp);

      P1 = Gamma;
      P2 = - (Beta * Beta) / 3.0;
      P = P1 + P2;
      Ep = 5.0 * EPSILON * (Abs(P1) + Abs(P2));
      if(Abs(P) <= Ep) P = 0.0;
      Q1 = Del;
      Q2 = - Beta * Gamma / 3.0;
      Q3 = 2.0  * (Beta * Beta * Beta) / 27.0;
      Q = Q1 + Q2 + Q3;
      Eq = 10.0 * EPSILON * (Abs(Q1) + Abs(Q2) + Abs(Q3));
      if(Abs(Q) <= Eq) Q = 0.0;
      //-- ############################################################
      Standard_Real AbsP = P; if(P<0.0) AbsP = -P;
      if(AbsP>1e+80) { Done = Standard_False; return; } 
      //-- ############################################################      
      A1 = (P * P * P) / 27.0;  
      A2 = (Q * Q) / 4.0;
      Discr = A1 + A2;
      if(P < 0.0) {
        Sigma = - Q2 - Q3;
        Psi = Gamma * Gamma * (4.0 * Gamma - Beta * Beta) / 27.0;
        if(Sigma >= 0.0) {
          D1 = Sigma + 2.0 * sqrt(-A1);
        }
        else {
          D1 = Sigma - 2.0 * sqrt(-A1);
        }
        D2 = Psi / D1;
        Discr = 0.0;
        if(Abs(Del - D1) >= 18.0 * EPSILON * (Abs(Del) + Abs(D1)) &&
           Abs(Del - D2) >= 24.0 * EPSILON * (Abs(Del) + Abs(D2))) {
          Discr = (Del - D1) * (Del - D2) / 4.0;
        }
      }
      if(Beta >= 0.0) {
        Sb = 1.0;
      }
      else {
        Sb = -1.0;
      }
      if(Discr < 0.0) {
        NbSol = 3;
        if(Beta == 0.0 && Q == 0.0) {
          TheRoots[0] = sqrt(-P);
          TheRoots[1] = -TheRoots[0];
          TheRoots[2] = 0.0;
        }
        else {
          Omega = atan(0.5 * Q / sqrt(- Discr));
          Sp3 = sqrt(-P / 3.0);
          Y1 = -2.0 * Sb * Sp3 * cos(M_PI / 6.0 - Sb * Omega / 3.0);          
          TheRoots[0] = - Beta / 3.0 + Y1;
          if(Beta * Q <= 0.0) {
            TheRoots[1] = - Beta / 3.0 + 2.0 * Sp3 * sin(Omega / 3.0);
          }
          else {
            Dbg = Del - Beta * Gamma;
            if(Dbg >= 0.0) {
              Sdbg = 1.0;
            }
            else {
              Sdbg = -1.0;
            }
            Den1 = 8.0 * Beta * Beta / 9.0 - 4.0 * Beta * Y1 / 3.0 
                                           - 2.0 * Q / Y1;
            Den2 = 2.0 * Y1 * Y1 - Q / Y1;
            TheRoots[1] = Dbg / Den1 + Sdbg * sqrt(-27.0 * Discr) / Den2;
          }
          TheRoots[2] = - Del / (TheRoots[0] * TheRoots[1]);
        }
      }
      else if(Discr > 0.0) {
        NbSol = 1;
        U = sqrt(Discr) + Abs(Q / 2.0);
        if(U >= 0.0) {
          U = pow(U, 1.0 / 3.0);   
        }
        else {
          U = - pow(Abs(U), 1.0 / 3.0);
        }
        if(P >= 0.0) {
          H = U * U + P / 3.0 + (P / U) * (P / U) / 9.0;          
        } 
        else {
          H = U * Abs(Q) / (U * U - P / 3.0);
        }
        if(Beta * Q >= 0.0) {
          if(Abs(H) <= RealSmall() && Abs(Q) <= RealSmall()) {
            TheRoots[0] = - Beta / 3.0 - U + P / (3.0 * U);
          }
          else {
            TheRoots[0] = - Beta / 3.0 - Q / H;
          }
        }
        else {
          TheRoots[0] = - Del / (Beta * Beta / 9.0 + H - Beta * Q / (3.0 * H));
        }
      }
      else {
        NbSol = 3;
        if(Q >= 0.0) {
          Sq = 1.0;
        }
        else {
          Sq = -1.0;
        }
        Sp3 = sqrt(-P / 3.0);
        if(Beta * Q <= 0.0) {
          TheRoots[0] = -Beta / 3.0 + Sq * Sp3;
          TheRoots[1] = TheRoots[0];
          if(Beta * Q == 0.0) {
            TheRoots[2] = -Beta / 3.0 - 2.0 * Sq * Sp3;
          }
          else {
            TheRoots[2] = - Del / (TheRoots[0] * TheRoots[1]);
          }
        }
        else {
          TheRoots[0] = -Gamma / (Beta + 3.0 * Sq * Sp3);
          TheRoots[1] = TheRoots[0];
          TheRoots[2] = -Beta / 3.0 - 2.0 * Sq * Sp3;
        }
      }
      for(Standard_Integer Index = 0; Index < NbSol; Index++) {
        TheRoots[Index] = TheRoots[Index] * pow(RADIX, Exp);
        TheRoots[Index] = Improve(A, B, C, D, TheRoots[Index]);
      }
    }

    void math_DirectPolynomialRoots::Solve(const Standard_Real A,
                                           const Standard_Real B,
                                           const Standard_Real C) {

        if(Abs(A) <= ZERO) {
          Solve(B, C);
          return;
        }

        Standard_Real EpsD = 3.0 * EPSILON * (B * B + Abs(4.0 * A * C));
        Standard_Real Discrim = B * B - 4.0 * A * C;

        if(Abs(Discrim) <= EpsD) Discrim = 0.0;
        if(Discrim < 0.0) {
          NbSol = 0;
        }
        else if(Discrim == 0.0) {
          NbSol = 2;
          TheRoots[0] = -0.5 * B / A;
          TheRoots[0] = Improve(A, B, C, TheRoots[0]);
          TheRoots[1] = TheRoots[0];
        } 
        else {
          NbSol = 2;
          if(B > 0.0) {
            TheRoots[0] = - (B + sqrt(Discrim)) / (2.0 * A);
          }
          else {
            TheRoots[0] = - (B - sqrt(Discrim)) / (2.0 * A);
          }
          TheRoots[0] = Improve(A, B, C, TheRoots[0]);
          TheRoots[1] = C / (A * TheRoots[0]);
          TheRoots[1] = Improve(A, B, C, TheRoots[1]);
        } 
    }

    void math_DirectPolynomialRoots::Solve(const Standard_Real A,
                                           const Standard_Real B) { 
        
        if(Abs(A) <= ZERO) {
	  if (Abs(B) <= ZERO) {
	    InfiniteStatus = Standard_True;
	    return;
	  }
	  NbSol = 0;
	  return;
        }
        NbSol = 1;
        TheRoots[0] = -B / A;
    }

void math_DirectPolynomialRoots::Dump(Standard_OStream& o) const {
       o << "math_DirectPolynomialRoots ";
       if (!Done) {
	 o <<" Not Done \n";
       }
       else if(InfiniteStatus) {
         o << " Status = Infinity Roots \n";
       }
       else if (!InfiniteStatus) {
         o << " Status = Not Infinity Roots \n";
	 o << " Number of solutions = " << NbSol <<"\n";
	 for (Standard_Integer i = 1; i <= NbSol; i++) {
	   o << " Solution number " << i << " = " << TheRoots[i-1] <<"\n";
	 }
       }
    }



