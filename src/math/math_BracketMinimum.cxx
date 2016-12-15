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


#include <math_BracketMinimum.hxx>
#include <math_Function.hxx>
#include <StdFail_NotDone.hxx>

// waiting for NotDone Exception
#define GOLD           1.618034
#define CGOLD          0.3819660
#define GLIMIT         100.0
#define TINY           1.0e-20
#ifdef MAX
#undef MAX
#endif
#define MAX(a,b)       ((a) > (b) ? (a) : (b))
#define SIGN(a,b)      ((b) > 0.0 ? fabs(a) : -fabs(a))
#define SHFT(a,b,c,d)  (a)=(b);(b)=(c);(c)=(d)

Standard_Boolean math_BracketMinimum::LimitAndMayBeSwap
                   (math_Function& F,
                    const Standard_Real theA,
                    Standard_Real& theB,
                    Standard_Real& theFB,
                    Standard_Real& theC,
                    Standard_Real& theFC) const
{
  theC = Limited(theC);
  if (Abs(theB - theC) < Precision::PConfusion())
    return Standard_False;
  Standard_Boolean OK = F.Value(theC, theFC);
  if (!OK)
    return Standard_False;
  // check that B is between A and C
  if ((theA - theB) * (theB - theC) < 0)
  {
    // swap B and C
    Standard_Real dum;
    SHFT(dum, theB, theC, dum);
    SHFT(dum, theFB, theFC, dum);
  }
  return Standard_True;
}

    void math_BracketMinimum::Perform(math_Function& F)
    {

     Standard_Boolean OK;
     Standard_Real ulim, u, r, q, fu, dum;

     Done = Standard_False; 
     Standard_Real Lambda = GOLD;
     if (!myFA) {
       OK = F.Value(Ax, FAx);
       if(!OK) return;
     }
     if (!myFB) {
       OK = F.Value(Bx, FBx);
       if(!OK) return;
     }
     if(FBx > FAx) {
       SHFT(dum, Ax, Bx, dum);
       SHFT(dum, FBx, FAx, dum);
     }

     // get next prob after (A, B)
     Cx = Bx + Lambda * (Bx - Ax);
     if (myIsLimited)
     {
       OK = LimitAndMayBeSwap(F, Ax, Bx, FBx, Cx, FCx);
       if (!OK)
         return;
     }
     else
     {
       OK = F.Value(Cx, FCx);
       if (!OK)
         return;
     }

     while(FBx > FCx) {
       r = (Bx - Ax) * (FBx -FCx);
       q = (Bx - Cx) * (FBx -FAx);
       u = Bx - ((Bx - Cx) * q - (Bx - Ax) * r) / 
           (2.0 * SIGN(MAX(fabs(q - r), TINY), q - r));
       ulim = Bx + GLIMIT * (Cx - Bx);
       if (myIsLimited)
         ulim = Limited(ulim);
       if ((Bx - u) * (u - Cx) > 0.0) {
         // u is between B and C
         OK = F.Value(u, fu);
         if(!OK) return;
         if(fu < FCx) {
           // solution is found (B, u, c)
           Ax = Bx;
           Bx = u;
           FAx = FBx;
           FBx = fu;
           Done = Standard_True;
           return;
         }
         else if(fu > FBx) {
           // solution is found (A, B, u)
           Cx = u;
           FCx = fu;
           Done = Standard_True;
           return;
         }
         // get next prob after (B, C)
         u = Cx + Lambda * (Cx - Bx);
         if (myIsLimited)
         {
           OK = LimitAndMayBeSwap(F, Bx, Cx, FCx, u, fu);
           if (!OK)
             return;
         }
         else
         {
           OK = F.Value(u, fu);
           if (!OK)
             return;
         }
       }
       else if((Cx - u) * (u - ulim) > 0.0) {
         // u is beyond C but between C and limit
         OK = F.Value(u, fu);
         if(!OK) return;
       }
       else if ((u - ulim) * (ulim - Cx) >= 0.0) {
         // u is beyond limit
         u = ulim;
         OK = F.Value(u, fu);
         if(!OK) return;
       }
       else {
         // u tends to approach to the side of A,
         // so reset it to the next prob after (B, C)
         u = Cx + GOLD * (Cx - Bx);
         if (myIsLimited)
         {
           OK = LimitAndMayBeSwap(F, Bx, Cx, FCx, u, fu);
           if (!OK)
             return;
         }
         else
         {
           OK = F.Value(u, fu);
           if (!OK)
             return;
         }
       }
       SHFT(Ax, Bx, Cx, u);
       SHFT(FAx, FBx, FCx, fu);
     }
     Done = Standard_True;
   }




    math_BracketMinimum::math_BracketMinimum(math_Function& F, 
                                             const Standard_Real A, 
                                             const Standard_Real B)
    : Done(Standard_False),
      Ax(A), Bx(B), Cx(0.),
      FAx(0.), FBx(0.), FCx(0.),
      myLeft(-Precision::Infinite()),
      myRight(Precision::Infinite()),
      myIsLimited(Standard_False),
      myFA(Standard_False),
      myFB (Standard_False)
    {
      Perform(F);
    }

    math_BracketMinimum::math_BracketMinimum(math_Function& F, 
                                             const Standard_Real A, 
                                             const Standard_Real B,
					     const Standard_Real FA)
    : Done(Standard_False),
      Ax(A), Bx(B), Cx(0.),
      FAx(FA), FBx(0.), FCx(0.),
      myLeft(-Precision::Infinite()),
      myRight(Precision::Infinite()),
      myIsLimited(Standard_False),
      myFA(Standard_True),
      myFB (Standard_False)
    {
      Perform(F);
    }

    math_BracketMinimum::math_BracketMinimum(math_Function& F, 
                                             const Standard_Real A, 
                                             const Standard_Real B,
					     const Standard_Real FA,
					     const Standard_Real FB)
    : Done(Standard_False),
      Ax(A), Bx(B), Cx(0.),
      FAx(FA), FBx(FB), FCx(0.),
      myLeft(-Precision::Infinite()),
      myRight(Precision::Infinite()),
      myIsLimited(Standard_False),
      myFA(Standard_True),
      myFB(Standard_True)
    {
      Perform(F);
    }


    void math_BracketMinimum::Values(Standard_Real& A, Standard_Real& B, Standard_Real& C) const{

      StdFail_NotDone_Raise_if(!Done, " ");
      A = Ax;
      B = Bx;
      C = Cx;
    }

    void math_BracketMinimum::FunctionValues(Standard_Real& FA, Standard_Real& FB, Standard_Real& FC) const{

      StdFail_NotDone_Raise_if(!Done, " ");
      FA = FAx;
      FB = FBx;
      FC = FCx;
    }

    void math_BracketMinimum::Dump(Standard_OStream& o) const {

       o << "math_BracketMinimum ";
       if(Done) {
         o << " Status = Done \n";
	 o << " The bracketed triplet is: " << endl;
	 o << Ax << ", " << Bx << ", " << Cx << endl;
	 o << " The corresponding function values are: "<< endl;
	 o << FAx << ", " << FBx << ", " << FCx << endl;
       }
       else {
         o << " Status = not Done \n";
       }
}

