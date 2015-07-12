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


    void math_BracketMinimum::Perform(math_Function& F, 
                                      const Standard_Real A, 
                                      const Standard_Real B) {

     Standard_Boolean OK;
     Standard_Real ulim, u, r, q, f, fu, dum;

     Done = Standard_False; 
     Ax = A;
     Bx = B;
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
     Cx = Bx + Lambda * (Bx - Ax);
     OK = F.Value(Cx, FCx);
     if(!OK) return;
     while(FBx > FCx) {
       r = (Bx - Ax) * (FBx -FCx);
       q = (Bx - Cx) * (FBx -FAx);
       u = Bx - ((Bx - Cx) * q - (Bx - Ax) * r) / 
           (2.0 * SIGN(MAX(fabs(q - r), TINY), q - r));
       ulim = Bx + GLIMIT * (Cx - Bx);
       if((Bx - u) * (u - Cx) > 0.0) {
         OK = F.Value(u, fu);
         if(!OK) return;
         if(fu < FCx) {
           Ax = Bx;
           Bx = u;
           FAx = FBx;
           FBx = fu;
           Done = Standard_True;
           return;
         }
         else if(fu > FBx) {
           Cx = u;
           FCx = fu;
           Done = Standard_True;
           return;
         }
         u = Cx + Lambda * (Cx - Bx);
         OK = F.Value(u, fu);
         if(!OK) return;
       }
       else if((Cx - u) * (u - ulim) > 0.0) {
         OK = F.Value(u, fu);
         if(!OK) return;
         if(fu < FCx) {
           SHFT(Bx, Cx, u, Cx + GOLD * (Cx - Bx));
           OK = F.Value(u, f);
           if(!OK) return;
           SHFT(FBx, FCx, fu, f);
         }
       }
       else if ((u - ulim) * (ulim - Cx) >= 0.0) {
         u = ulim;
         OK = F.Value(u, fu);
         if(!OK) return;
       }
       else {
         u = Cx + GOLD * (Cx - Bx);
         OK = F.Value(u, fu);
         if(!OK) return;
       }
       SHFT(Ax, Bx, Cx, u);
       SHFT(FAx, FBx, FCx, fu);
     }
     Done = Standard_True;
   }




    math_BracketMinimum::math_BracketMinimum(math_Function& F, 
                                             const Standard_Real A, 
                                             const Standard_Real B) {

      myFA = Standard_False;
      myFB = Standard_False;
      Perform(F, A, B);
    }

    math_BracketMinimum::math_BracketMinimum(math_Function& F, 
                                             const Standard_Real A, 
                                             const Standard_Real B,
					     const Standard_Real FA) {
      FAx = FA;
      myFA = Standard_True;
      myFB = Standard_False;
      Perform(F, A, B);
    }

    math_BracketMinimum::math_BracketMinimum(math_Function& F, 
                                             const Standard_Real A, 
                                             const Standard_Real B,
					     const Standard_Real FA,
					     const Standard_Real FB) {
      FAx = FA;
      FBx = FB;
      myFA = Standard_True;
      myFB = Standard_True;
      Perform(F, A, B);
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

