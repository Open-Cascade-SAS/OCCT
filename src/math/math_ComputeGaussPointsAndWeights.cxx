// Created on: 2005-12-19
// Created by: Julia GERASIMOVA
// Copyright (c) 2005-2012 OPEN CASCADE SAS
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



#include <math_ComputeGaussPointsAndWeights.ixx>

#include <math_EigenValuesSearcher.hxx>
#include <math_Array1OfValueAndWeight.hxx>
#include <math_CompareOfValueAndWeight.hxx>
#include <math_QuickSortOfValueAndWeight.hxx>
#include <Standard_ErrorHandler.hxx>

math_ComputeGaussPointsAndWeights::math_ComputeGaussPointsAndWeights(const Standard_Integer Number)
{
  myIsDone = Standard_False;

  try {
    myPoints  = new TColStd_HArray1OfReal(1, Number);
    myWeights = new TColStd_HArray1OfReal(1, Number);

    Standard_Integer i;

    TColStd_Array1OfReal aDiag(1, Number);
    TColStd_Array1OfReal aSubDiag(1, Number);

    //Initialization of a real symmetric tridiagonal matrix for
    //computation of Gauss quadrature.

    for (i = 1; i <= Number; i++) {
      aDiag(i) = 0.;

      if (i == 1)
	aSubDiag(i) = 0.;
      else {
	Standard_Integer sqrIm1 = (i-1)*(i-1);
	aSubDiag(i) = sqrIm1/(4.*sqrIm1 - 1);
	aSubDiag(i) = Sqrt(aSubDiag(i));
      }
    }

    // Compute eigen values.
    math_EigenValuesSearcher EVsearch(aDiag, aSubDiag); 

    if (EVsearch.IsDone()) {
      math_Array1OfValueAndWeight VWarray(1, Number);
      for (i = 1; i <= Number; i++) {
	math_Vector anEigenVector = EVsearch.EigenVector(i);
	Standard_Real aWeight = anEigenVector(1);
	aWeight = 2. * aWeight * aWeight;
	math_ValueAndWeight EVW( EVsearch.EigenValue(i), aWeight );
	VWarray(i) = EVW;
      }

      math_CompareOfValueAndWeight theComparator;
      math_QuickSortOfValueAndWeight::Sort(VWarray, theComparator);

      for (i = 1; i <= Number; i++) {
	myPoints->ChangeValue(i)  = VWarray(i).Value();
	myWeights->ChangeValue(i) = VWarray(i).Weight();
      }      
      myIsDone = Standard_True;
    }
  } catch (Standard_Failure) {
  }
}

Standard_Boolean math_ComputeGaussPointsAndWeights::IsDone() const
{
  return myIsDone;
}

math_Vector math_ComputeGaussPointsAndWeights::Points() const
{
  Standard_Integer Number = myPoints->Length();
  math_Vector thePoints(1, Number);
  for (Standard_Integer i = 1; i <= Number; i++)
    thePoints(i) = myPoints->Value(i);

  return thePoints;
}

math_Vector math_ComputeGaussPointsAndWeights::Weights() const
{
  Standard_Integer Number = myWeights->Length();
  math_Vector theWeights(1, Number);
  for (Standard_Integer i = 1; i <= Number; i++)
    theWeights(i) = myWeights->Value(i);

  return theWeights;
}
