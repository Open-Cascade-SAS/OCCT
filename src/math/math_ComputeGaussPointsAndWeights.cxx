// File:	math_ComputeGaussPointsAndWeights.cxx
// Created:	Mon Dec 19 16:01:53 2005
// Author:	Julia GERASIMOVA
//		<jgv@clubox>


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
