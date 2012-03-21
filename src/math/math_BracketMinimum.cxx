// Copyright (c) 1997-1999 Matra Datavision
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

#include <math_BracketMinimum.ixx>

#include <StdFail_NotDone.hxx>   // waiting for NotDone Exception
#include <math_Function.hxx>

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

