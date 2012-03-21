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

#include <math_BracketedRoot.ixx>
#include <math_Function.hxx>

// reference algorithme:  
//                   Brent method 
//                   numerical recipes in C  p 269

math_BracketedRoot::math_BracketedRoot (math_Function& F, 
                                           const Standard_Real Bound1, 
                                           const Standard_Real Bound2, 
                                           const Standard_Real Tolerance, 
                                           const Standard_Integer NbIterations,
                                           const Standard_Real ZEPS ) {

    Standard_Real Fa,Fc,a,c=0,d=0,e=0;
    Standard_Real min1,min2,p,q,r,s,tol1,xm;
    Standard_Boolean Ok;
  
    a = Bound1;
    TheRoot = Bound2;
    Ok = F.Value(a,Fa);
    Ok = F.Value(TheRoot,TheError);
    if (Fa*TheError > 0.) { Done = Standard_False;}
    else {
      Fc = TheError ;
      for (NbIter = 1; NbIter <= NbIterations; NbIter++) {
          if (TheError*Fc > 0.) {
             c = a;      // rename a TheRoot c and adjust bounding interval d
             Fc = Fa;
             d = TheRoot - a;
             e = d;
          } 
          if ( Abs(Fc) < Abs(Fa) ) {
             a = TheRoot;
             TheRoot = c;
             c = a;
             Fa = TheError;
             TheError = Fc;
             Fc = Fa;
          }
          tol1 = 2.*ZEPS * Abs(TheRoot) + 0.5 * Tolerance; // convergence check
          xm = 0.5 * ( c - TheRoot );
          if (Abs(xm) <= tol1 || TheError == 0. ) {
               Done = Standard_True;
               return;
	  }
          if (Abs(e) >= tol1 && Abs(Fa) > Abs(TheError) ) {
             s = TheError / Fa; // attempt inverse quadratic interpolation
             if (a == c) {
                p = 2.*xm*s;
                q = 1. - s;
             }
             else {
                q = Fa / Fc;
                r = TheError / Fc;
                p = s * (2.*xm *q * (q - r) - (TheRoot - a)*(r - 1.)); 
                q = (q -1.) * (r - 1.) * (s - 1.);
             }
            if ( p > 0. ) { q = -q;} // check whether in bounds
            p = Abs(p);
            min1 = 3.* xm* q - Abs(tol1 *q);
            min2 = Abs(e * q);
            if (2.* p < (min1 < min2 ? min1 : min2) ) {
               e = d ;  // accept interpolation
               d = p / q;
            }
            else {
               d = xm;  // interpolation failed,use bissection
               e = d;
            }
          }
          else {   // bounds decreasing too slowly ,use bissection
              d = xm;
              e =d;            
          }
          a = TheRoot ;   // move last best guess to a
          Fa = TheError;
          if (Abs(d) > tol1) {  // evaluate new trial root
             TheRoot += d;
          }
          else {
             TheRoot += (xm > 0. ? Abs(tol1) : -Abs(tol1));
          }
          Ok = F.Value(TheRoot,TheError);
      }  
     Done = Standard_False;
    }  
  }


    void math_BracketedRoot::Dump(Standard_OStream& o) const {

       o << "math_BracketedRoot ";
       if(Done) {
         o << " Status = Done \n";
	 o << " Number of iterations = " << NbIter << endl;
	 o << " The Root is: " << TheRoot << endl;
	 o << " The value at the root is: " << TheError << endl;
       }
       else {
         o << " Status = not Done \n";
       }
}
