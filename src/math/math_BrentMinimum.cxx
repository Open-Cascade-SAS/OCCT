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

#include <math_BrentMinimum.ixx>
#include <math_Function.hxx>

#define CGOLD         0.3819660
#ifdef MAX
#undef MAX
#endif
#define MAX(a,b)      ((a) > (b) ? (a) : (b))
#define SIGN(a,b)     ((b) > 0.0 ? fabs(a) : -fabs(a))
#define SHFT(a,b,c,d) (a)=(b);(b)=(c);(c)=(d)

void math_BrentMinimum::Perform(math_Function& F,
				const Standard_Real    ax,
				const Standard_Real    bx,
				const Standard_Real    cx) {

  Standard_Boolean OK;  
  Standard_Real etemp, fu, p, q, r;  
  Standard_Real tol1, tol2, u, v, w, xm;
  Standard_Real e = 0.0;
  Standard_Real d = RealLast();
  
  a = ((ax < cx) ? ax : cx);
  b = ((ax > cx) ? ax : cx);
  x = w = v = bx;
  if (!myF) {
    OK = F.Value(x, fx);
    if(!OK) return;
  }
  fw = fv = fx;
  for(iter = 1; iter <= Itermax; iter++) {
    xm = 0.5 * (a + b);
    tol1 = XTol * fabs(x) + EPSZ;
    tol2 = 2.0 * tol1;
    if(IsSolutionReached(F)) {
      Done = Standard_True;
      return;
    }
    if(fabs(e) > tol1) {
      r = (x - w) * (fx - fv);
      q = (x - v) * (fx - fw);
      p = (x - v) * q - (x - w) * r;
      q = 2.0 * (q - r);
      if(q > 0.0) p = -p;
      q = fabs(q);
      etemp = e;
      e = d;
      if(fabs(p) >= fabs(0.5 * q * etemp) 
	 || p <= q * ( a - x) || p >= q * (b - x)) {
	e = (x >= xm ? a - x : b - x);
	d = CGOLD * e;
      }
      else {
	d = p / q;
	u = x + d;
	if(u - a < tol2 || b - u < tol2) d = SIGN(tol1, xm - x);
      }
    }
    else {
      e = (x >= xm ? a - x : b - x);
      d = CGOLD * e;
    }
    u = (fabs(d) >= tol1 ? x + d : x + SIGN(tol1, d));
    OK = F.Value(u, fu);
    if(!OK) return;
    if(fu <= fx) {
      if(u >= x) a = x; else b = x;
      SHFT(v, w, x, u);
      SHFT(fv, fw, fx, fu);
    }
    else {
      if(u < x) a = u; else b = u;
      if(fu <= fw || w == x) {
	v = w;
	w = u;
	fv = fw;
	fw = fu;
      }
      else if(fu <= fv || v == x || v == w) {
	v = u;
	fv = fu;
      }
    }
  }
  Done = Standard_False;
  return;
}


math_BrentMinimum::math_BrentMinimum(math_Function& F,
				     const Standard_Real    Ax,
				     const Standard_Real    Bx,
				     const Standard_Real    Cx,
				     const Standard_Real    TolX,
				     const Standard_Integer NbIterations,
				     const Standard_Real    ZEPS) {

  XTol = TolX;
  EPSZ = ZEPS;
  Itermax = NbIterations;
  myF = Standard_False;
  Perform(F, Ax, Bx, Cx);
}


// Constructeur d'initialisation des champs.

math_BrentMinimum::math_BrentMinimum(const Standard_Real    TolX,
				     const Standard_Integer NbIterations,
				     const Standard_Real    ZEPS) {
  myF = Standard_False;
  XTol = TolX;
  EPSZ = ZEPS;
  Itermax = NbIterations;
}

math_BrentMinimum::math_BrentMinimum(const Standard_Real    TolX,
                                     const Standard_Real    Fbx,
				     const Standard_Integer NbIterations,
				     const Standard_Real    ZEPS) {

  fx = Fbx;
  myF = Standard_True;
  XTol = TolX;
  EPSZ = ZEPS;
  Itermax = NbIterations;
}


//    Standard_Boolean math_BrentMinimum::IsSolutionReached(math_Function& F) {
    Standard_Boolean math_BrentMinimum::IsSolutionReached(math_Function& ) {

//       Standard_Real xm = 0.5 * (a + b);
       // modified by NIZHNY-MKK  Mon Oct  3 17:45:57 2005.BEGIN
//        Standard_Real tol = XTol * fabs(x) + EPSZ;
//        return fabs(x - xm) <= 2.0 * tol - 0.5 * (b - a);
       Standard_Real TwoTol = 2.0 *(XTol * fabs(x) + EPSZ);
       return ((x <= (TwoTol + a)) && (x >= (b - TwoTol)));
       // modified by NIZHNY-MKK  Mon Oct  3 17:46:00 2005.END
  }



    void math_BrentMinimum::Dump(Standard_OStream& o) const {
       o << "math_BrentMinimum ";
       if(Done) {
         o << " Status = Done \n";
         o << " Location value = " << x <<"\n";
         o << " Minimum value = " << fx << "\n";
         o << " Number of iterations = " << iter <<"\n";
       }
       else {
         o << " Status = not Done \n";
       }
    }
