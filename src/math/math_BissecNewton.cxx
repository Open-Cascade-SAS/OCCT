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

#include <math_BissecNewton.ixx>
#include <math_FunctionWithDerivative.hxx>


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
      if(xl == x) {
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

Standard_Boolean math_BissecNewton::IsSolutionReached
//(math_FunctionWithDerivative& F)
(math_FunctionWithDerivative& )
{
  return Abs(dx) <= XTol;
}


math_BissecNewton::math_BissecNewton(math_FunctionWithDerivative& F,
				     const Standard_Real    Bound1,
				     const Standard_Real    Bound2,
				     const Standard_Real    TolX, 
				     const Standard_Integer NbIterations) {
  
  XTol = TolX;
  Perform(F, Bound1, Bound2, NbIterations);
}


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

