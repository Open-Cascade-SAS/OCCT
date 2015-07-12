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


#include <math_BissecNewton.hxx>
#include <math_FunctionWithDerivative.hxx>
#include <StdFail_NotDone.hxx>

//=======================================================================
//function : math_BissecNewton
//purpose  : Constructor
//=======================================================================
math_BissecNewton::math_BissecNewton(const Standard_Real theXTolerance)
: TheStatus(math_NotBracketed),
  XTol     (theXTolerance),
  x        (0.0),
  dx       (0.0),
  f        (0.0),
  df       (0.0),
  Done     (Standard_False)
{
}

//=======================================================================
//function : ~math_BissecNewton
//purpose  : Destructor
//=======================================================================
math_BissecNewton::~math_BissecNewton()
{
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void math_BissecNewton::Perform(math_FunctionWithDerivative& F,
                                const Standard_Real    Bound1,
                                const Standard_Real    Bound2,
                                const Standard_Integer NbIterations)
{
  Standard_Boolean GOOD;
  Standard_Integer j;
  Standard_Real dxold, fh, fl;
  Standard_Real swap, temp, xh, xl;
  
  GOOD = F.Values(Bound1, fl, df);
  if(!GOOD) {
    Done = Standard_False;
    TheStatus = math_FunctionError;
    return;
  }
  GOOD = F.Values(Bound2, fh, df);
  if(!GOOD) {
    Done = Standard_False;
    TheStatus = math_FunctionError;
    return;
  }
//  Modified by Sergey KHROMOV - Wed Jan 22 12:06:45 2003 Begin
  Standard_Real aFTol = RealEpsilon();

//   if(fl * fh >= 0.0) {
  if(fl * fh > aFTol*aFTol) {
    Done = Standard_False;
    TheStatus = math_NotBracketed;
    return;
  }
//   if(fl < 0.0) {
  if(fl < -aFTol || (fl < aFTol && fh < -aFTol)) {
    xl = Bound1;
    xh = Bound2;
  }
  else {
    xl = Bound2;
    xh = Bound1;
    swap = fl;
    fl = fh;
    fh = swap;
  }
//  Modified by Sergey KHROMOV - Wed Jan 22 12:06:49 2003 End
  x = 0.5 * (Bound1 + Bound2);
  dxold = fabs(Bound2 - Bound1);
  dx = dxold;
  GOOD = F.Values(x, f, df);
  if(!GOOD) {
    Done = Standard_False;
    TheStatus = math_FunctionError;
    return;
  }
  for(j = 1; j <= NbIterations; j++) {
    if((((x - xh) * df - f) * ((x - xl) * df - f) >= 0.0)
       || (fabs(2.0 * f) > fabs(dxold * df))) {
      dxold = dx;
      dx = 0.5 * (xh - xl);
      x = xl + dx;
      if(Abs(dx) < XTol) {
	TheStatus = math_OK;
	Done = Standard_True;
	return;
      }
    }
    else {
      dxold = dx;
      dx = f / df;
      temp = x;
      x -= dx;
      if(temp == x) {
	TheStatus = math_OK;
	Done = Standard_True;
	return;
      }
    }
    if(IsSolutionReached(F)) {
      TheStatus = math_OK;
      Done = Standard_True;
      return;
    }
    GOOD = F.Values(x, f, df);
    if(!GOOD) {
      Done = Standard_False;
      TheStatus = math_FunctionError;
      return;
    }
    if(f < 0.0) {
      xl = x;
      fl = f;
    }
    else if(f > 0.0) {
      xh = x;
      fh = f;
    }
    else {
      TheStatus = math_OK;
      Done = Standard_True;
      return;
    }
  }
  TheStatus = math_TooManyIterations;
  Done = Standard_False;
  return;
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================
void math_BissecNewton::Dump(Standard_OStream& o) const {
  
  o << "math_BissecNewton ";
  if(Done) {
    o << " Status = Done \n";
    o << " The Root  is: " << x << endl;
    o << " The value at this Root is: " << f << endl;
  }
  else {
    o << " Status = not Done \n";
  }
}

