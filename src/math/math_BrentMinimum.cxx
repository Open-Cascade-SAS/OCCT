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


#include <math_BrentMinimum.hxx>
#include <math_Function.hxx>
#include <StdFail_NotDone.hxx>

static const Standard_Real CGOLD = 0.3819660; //0.5*(3 - sqrt(5));


//=======================================================================
//function : SHFT
//purpose  : Shifts arguments
//=======================================================================
inline void SHFT(Standard_Real &theA, Standard_Real &theB,
                 Standard_Real &theC, Standard_Real &theD)
{
  theA = theB;
  theB = theC;
  theC = theD;
}

//=======================================================================
//function : math_BrentMinimum
//purpose  : Constructor
//=======================================================================
math_BrentMinimum::math_BrentMinimum(const Standard_Real    theTolX,
                                     const Standard_Integer theNbIterations,
                                     const Standard_Real    theZEPS)
: a   (0.0),
  b   (0.0),
  x   (0.0),
  fx  (0.0),
  fv  (0.0),
  fw  (0.0),
  XTol(theTolX),
  EPSZ(theZEPS),
  Done   (Standard_False),
  iter   (0),
  Itermax(theNbIterations),
  myF    (Standard_False)
{
}

//=======================================================================
//function : math_BrentMinimum
//purpose  : Constructor
//=======================================================================
math_BrentMinimum::math_BrentMinimum(const Standard_Real    theTolX,
                                     const Standard_Real    theFbx,
                                     const Standard_Integer theNbIterations,
                                     const Standard_Real    theZEPS)
: a   (0.0),
  b   (0.0),
  x   (0.0),
  fx  (theFbx),
  fv  (0.0),
  fw  (0.0),
  XTol(theTolX),
  EPSZ(theZEPS),
  Done   (Standard_False),
  iter   (0),
  Itermax(theNbIterations),
  myF    (Standard_True)
{
}

//=======================================================================
//function : ~math_BrentMinimum
//purpose  : Destructor
//=======================================================================
math_BrentMinimum::~math_BrentMinimum()
{
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void math_BrentMinimum::Perform(math_Function& F,
                                const Standard_Real    ax,
                                const Standard_Real    bx,
                                const Standard_Real    cx)
{
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
    if (!OK) return;
  }
  fw = fv = fx;
  for (iter = 1; iter <= Itermax; iter++) {
    xm = 0.5 * (a + b);
    tol1 = XTol * fabs(x) + EPSZ;
    tol2 = 2.0 * tol1;
    if (IsSolutionReached(F)) {
      Done = Standard_True;
      return;
    }
    if (fabs(e) > tol1) {
      r = (x - w) * (fx - fv);
      q = (x - v) * (fx - fw);
      p = (x - v) * q - (x - w) * r;
      q = 2.0 * (q - r);
      if (q > 0.0) p = -p;
      q = fabs(q);
      etemp = e;
      e = d;
      if (fabs(p) >= fabs(0.5 * q * etemp)
        || p <= q * (a - x) || p >= q * (b - x)) {
        e = (x >= xm ? a - x : b - x);
        d = CGOLD * e;
      }
      else {
        d = p / q;
        u = x + d;
        if (u - a < tol2 || b - u < tol2) d = Sign(tol1, xm - x);
      }
    }
    else {
      e = (x >= xm ? a - x : b - x);
      d = CGOLD * e;
    }
    u = (fabs(d) >= tol1 ? x + d : x + Sign(tol1, d));
    OK = F.Value(u, fu);
    if (!OK) return;
    if (fu <= fx) {
      if (u >= x) a = x; else b = x;
      SHFT(v, w, x, u);
      SHFT(fv, fw, fx, fu);
    }
    else {
      if (u < x) a = u; else b = u;
      if (fu <= fw || w == x) {
        v = w;
        w = u;
        fv = fw;
        fw = fu;
      }
      else if (fu <= fv || v == x || v == w) {
        v = u;
        fv = fu;
      }
    }
  }
  Done = Standard_False;
  return;
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================
void math_BrentMinimum::Dump(Standard_OStream& o) const
{
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
