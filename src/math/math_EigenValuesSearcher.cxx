// Created on: 2005-12-15
// Created by: Julia GERASIMOVA
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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


#include <math_EigenValuesSearcher.hxx>
#include <StdFail_NotDone.hxx>

//==========================================================================
//function : pythag
//           Computation of sqrt(x*x + y*y).
//==========================================================================
static inline Standard_Real pythag(const Standard_Real x,
				   const Standard_Real y)
{
  return Sqrt(x*x + y*y);
}

math_EigenValuesSearcher::math_EigenValuesSearcher(const TColStd_Array1OfReal& Diagonal,
						   const TColStd_Array1OfReal& Subdiagonal)
{
  myIsDone = Standard_False;

  Standard_Integer n = Diagonal.Length();
  if (Subdiagonal.Length() != n)
    throw Standard_Failure("math_EigenValuesSearcher : dimension mismatch");

  myDiagonal    = new TColStd_HArray1OfReal(1, n);
  myDiagonal->ChangeArray1() = Diagonal;
  mySubdiagonal = new TColStd_HArray1OfReal(1, n);
  mySubdiagonal->ChangeArray1() = Subdiagonal;
  myN = n;
  myEigenValues  = new TColStd_HArray1OfReal(1, n);
  myEigenVectors = new TColStd_HArray2OfReal(1, n, 1, n);

  Standard_Real* d  = new Standard_Real [n+1];
  Standard_Real* e  = new Standard_Real [n+1];
  Standard_Real** z = new Standard_Real* [n+1];
  Standard_Integer i, j;
  for (i = 1; i <= n; i++)
    z[i] = new Standard_Real [n+1];

  for (i = 1; i <= n; i++)
    d[i] = myDiagonal->Value(i);
  for (i = 2; i <= n; i++)
    e[i] = mySubdiagonal->Value(i);
  for (i = 1; i <= n; i++)
    for (j = 1; j <= n; j++)
      z[i][j] = (i == j)? 1. : 0.;

  Standard_Boolean result;
  Standard_Integer m;
  Standard_Integer l;
  Standard_Integer iter;
  //Standard_Integer i;
  Standard_Integer k;
  Standard_Real    s;
  Standard_Real    r;
  Standard_Real    p;
  Standard_Real    g;
  Standard_Real    f;
  Standard_Real    dd;
  Standard_Real    c;
  Standard_Real    b;

  result = Standard_True;

  if (n != 1)
    {
      // Shift e.
      for (i = 2; i <= n; i++)
	e[i - 1] = e[i];
      
      e[n] = 0.0;
      
      for (l = 1; l <= n; l++) {
	iter = 0;
	
	do {
	  for (m = l; m <= n-1; m++) {
	    dd = Abs(d[m]) + Abs(d[m + 1]);
	    
	    if (Abs(e[m]) + dd == dd)
	      break;
	  }
	  
	  if (m != l) {
	    if (iter++ == 30) {
	      result = Standard_False;
	      break; //return result;
	    }
	    
	    g = (d[l + 1] - d[l])/(2.*e[l]);
	    r = pythag(1., g);
	    
	    if (g < 0)
	      g = d[m] - d[l] + e[l]/(g - r);
	    else
	      g = d[m] - d[l] + e[l]/(g + r);
	    
	    s = 1.;
	    c = 1.;
	    p = 0.;
	    
	    for (i = m - 1; i >= l; i--) {
	      f        = s*e[i];
	      b        = c*e[i];
	      r        = pythag (f, g);
	      e[i + 1] = r;
	      
	      if (r == 0.) {
		d[i + 1] -= p;
		e[m]      = 0.;
		break;
	      }
	      
	      s        = f/r;
	      c        = g/r;
	      g        = d[i + 1] - p;
	      r        = (d[i] - g)*s + 2.0*c*b;
	      p        = s*r;
	      d[i + 1] = g + p;
	      g        = c*r - b;
	      
	      for (k = 1; k <= n; k++) {
		f           = z[k][i + 1];
		z[k][i + 1] = s*z[k][i] + c*f;
		z[k][i]     = c*z[k][i] - s*f;
	      }
	    }
	    
	    if (r == 0 && i >= 1)
	      continue;
	    
	    d[l] -= p;
	    e[l]  = g;
	    e[m]  = 0.;
	  }
	}
	while (m != l);
	if (result == Standard_False)
	  break;
      } //end of for (l = 1; l <= n; l++)
    } //end of if (n != 1)

  if (result)
    {
      for (i = 1; i <= n; i++)
	myEigenValues->ChangeValue(i) = d[i];
      for (i = 1; i <= n; i++)
	for (j = 1; j <= n; j++)
	  myEigenVectors->ChangeValue(i, j) = z[i][j];
    }

  myIsDone = result;

  delete [] d;
  delete [] e;
  for (i = 1; i <= n; i++)
    delete [] z[i];
  delete [] z;
}

Standard_Boolean math_EigenValuesSearcher::IsDone() const
{
  return myIsDone;
}

Standard_Integer math_EigenValuesSearcher::Dimension() const
{
  return myN;
}

Standard_Real math_EigenValuesSearcher::EigenValue(const Standard_Integer Index) const
{
  return myEigenValues->Value(Index);
}

math_Vector math_EigenValuesSearcher::EigenVector(const Standard_Integer Index) const
{
  math_Vector theVector(1, myN);

  Standard_Integer i;
  for (i = 1; i <= myN; i++)
    theVector(i) = myEigenVectors->Value(i, Index);

  return theVector;
}
