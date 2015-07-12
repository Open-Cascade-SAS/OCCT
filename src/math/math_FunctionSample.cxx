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

#include <math_FunctionSample.hxx>
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

