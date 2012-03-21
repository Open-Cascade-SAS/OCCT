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

//#ifndef DEB
#define No_Standard_RangeError
#define No_Standard_OutOfRange
#define No_Standard_DimensionError
//#endif

#include <math_FunctionSample.ixx>


#include <Standard_OutOfRange.hxx>


math_FunctionSample::math_FunctionSample (const Standard_Real A,
					    const Standard_Real B,
					    const Standard_Integer N):
  a(A),b(B),n(N)
{
}

void math_FunctionSample::Bounds (Standard_Real& A, Standard_Real& B) const {

  A=a;
  B=b;
}


Standard_Integer math_FunctionSample::NbPoints () const {
  return n;
}


Standard_Real math_FunctionSample::GetParameter (const Standard_Integer Index) const {
  Standard_OutOfRange_Raise_if((Index <= 0)||(Index > n), " ");
  return ((n-Index)*a+(Index-1)*b)/(n-1);
}

