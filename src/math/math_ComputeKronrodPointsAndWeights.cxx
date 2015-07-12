// Created on: 2005-12-21
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


#include <math_Array1OfValueAndWeight.hxx>
#include <math_ComputeKronrodPointsAndWeights.hxx>
#include <math_EigenValuesSearcher.hxx>
#include <Standard_ErrorHandler.hxx>

#include <algorithm>
math_ComputeKronrodPointsAndWeights::math_ComputeKronrodPointsAndWeights(const Standard_Integer Number)
{
  myIsDone = Standard_False;

  try {
    Standard_Integer i, j;
    Standard_Integer a2NP1 = 2*Number + 1;
  
    myPoints  = new TColStd_HArray1OfReal(1, a2NP1);
    myWeights = new TColStd_HArray1OfReal(1, a2NP1);

    TColStd_Array1OfReal aDiag(1, a2NP1);
    TColStd_Array1OfReal aSubDiag(1, a2NP1);
  
    // Initialize symmetric tridiagonal matrix.
    Standard_Integer n         = Number;
    Standard_Integer aKronrodN = 2*Number + 1;
    Standard_Integer a3KN2p1   = Min(3*(Number + 1)/2 + 1, aKronrodN);
    for (i = 1; i <= a3KN2p1; i++) {
      aDiag(i) = 0.;
      
      if (i == 1)
	aSubDiag(i) = 0.;
      else {
	Standard_Integer sqrIm1 = (i-1)*(i-1);
	aSubDiag(i) = sqrIm1/(4.*sqrIm1 - 1);
      }
    }

    for (i = a3KN2p1 + 1; i <= aKronrodN; i++) {
      aDiag(i)    = 0.;
      aSubDiag(i) = 0.;
    }
  
    // Initialization of temporary data structures.
    Standard_Integer  aNd2 =     Number/2;
    Standard_Real    *s    = new Standard_Real[aNd2 + 2];
    Standard_Real    *t    = new Standard_Real[aNd2 + 2];
    Standard_Real    *ss   =     s++;
    Standard_Real    *tt   =     t++;
  
    for (i = -1; i <= aNd2; i++) {
      s[i] = 0.;
      t[i] = 0.;
    }
  
    // Generation of Jacobi-Kronrod matrix.
    Standard_Real    *aa = new Standard_Real [a2NP1+1];
    Standard_Real    *bb = new Standard_Real [a2NP1+1];
    for (i = 1; i <= a2NP1; i++) {
      aa[i] = aDiag(i);
      bb[i] = aSubDiag(i);
    }
    Standard_Real    *ptrtmp;
    Standard_Real     u;
    Standard_Integer  m;
    Standard_Integer  k;
    Standard_Integer  l;

    Standard_Real *a = aa+1;
    Standard_Real *b = bb+1;

    // Eastward phase.
    t[0] = b[Number + 1];

    for (m = 0; m <= n - 2; m++) {
      u = 0;

      for (k = (m + 1)/2; k >= 0; k--) {
	l     = m - k;
	u    += (a[k + n + 1] - a[l])*t[k] + b[k + n + 1]*s[k - 1] - b[l]*s[k];
	s[k]  = u;
      }

      ptrtmp = t;
      t      = s;
      s      = ptrtmp;
    }

    for (j = aNd2; j >= 0; j--)
      s[j] = s[j - 1];

    // Southward phase.
    for (m = n - 1; m <= 2*n - 3; m++) {
      u = 0;

      for (k = m + 1 - n; k <= (m - 1)/2; k++) {
	l     =  m - k;
	j     =  n - 1 - l;
	u    += -(a[k + n + 1] - a[l])*t[j] - b[k + n + 1]*s[j] + b[l]*s[j + 1];
	s[j]  =  u;
      }

      if (m % 2 == 0) {
	k = m/2;
	a[k + n + 1] = a[k] + (s[j] - b[k + n + 1]*s[j + 1])/ t[j + 1];
      } else {
	k = (m + 1)/2;
	b[k + n + 1] = s[j]/s[j + 1];
      }

      ptrtmp = t;
      t      = s;
      s      = ptrtmp;
    }

    // Termination phase.
    a[2*Number] = a[n - 1] - b[2*Number]*s[0]/t[0];

    delete [] ss;
    delete [] tt;
    for (i = 1; i <= a2NP1; i++) {
      aDiag(i)    = aa[i];
      aSubDiag(i) = bb[i];
    }
    delete [] aa;
    delete [] bb;
  
    for (i = 1; i <= a2NP1; i++)
      aSubDiag(i)  = Sqrt(aSubDiag(i));
  
    // Compute eigen values.
    math_EigenValuesSearcher EVsearch(aDiag, aSubDiag); 
  
    if (EVsearch.IsDone()) {
      math_Array1OfValueAndWeight VWarray(1, a2NP1);
      for (i = 1; i <= a2NP1; i++) {
	math_Vector anEigenVector = EVsearch.EigenVector(i);
	Standard_Real aWeight = anEigenVector(1);
	aWeight = 2. * aWeight * aWeight;
	math_ValueAndWeight EVW( EVsearch.EigenValue(i), aWeight );
	VWarray(i) = EVW;
      }

      std::sort (VWarray.begin(), VWarray.end());
      
      for (i = 1; i <= a2NP1; i++) {
	myPoints->ChangeValue(i)  = VWarray(i).Value();
	myWeights->ChangeValue(i) = VWarray(i).Weight();
      }      
      myIsDone = Standard_True;
    }
  } catch (Standard_Failure) {
  }
}

Standard_Boolean math_ComputeKronrodPointsAndWeights::IsDone() const
{
  return myIsDone;
}

math_Vector math_ComputeKronrodPointsAndWeights::Points() const
{
  Standard_Integer Number = myPoints->Length();
  math_Vector thePoints(1, Number);
  for (Standard_Integer i = 1; i <= Number; i++)
    thePoints(i) = myPoints->Value(i);

  return thePoints;
}

math_Vector math_ComputeKronrodPointsAndWeights::Weights() const
{
  Standard_Integer Number = myWeights->Length();
  math_Vector theWeights(1, Number);
  for (Standard_Integer i = 1; i <= Number; i++)
    theWeights(i) = myWeights->Value(i);

  return theWeights;
}
