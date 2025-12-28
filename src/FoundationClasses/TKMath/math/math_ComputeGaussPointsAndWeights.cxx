// Created on: 2005-12-19
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

#include <math_ValueAndWeight.hxx>
#include <NCollection_Array1.hxx>
#include <math_ComputeGaussPointsAndWeights.hxx>
#include <math_EigenValuesSearcher.hxx>
#include <Standard_ErrorHandler.hxx>

#include <algorithm>

math_ComputeGaussPointsAndWeights::math_ComputeGaussPointsAndWeights(const int Number)
{
  myIsDone = false;

  try
  {
    myPoints  = new NCollection_HArray1<double>(1, Number);
    myWeights = new NCollection_HArray1<double>(1, Number);

    int i;

    NCollection_Array1<double> aDiag(1, Number);
    NCollection_Array1<double> aSubDiag(1, Number);

    // Initialization of a real symmetric tridiagonal matrix for
    // computation of Gauss quadrature.

    for (i = 1; i <= Number; i++)
    {
      aDiag(i) = 0.;

      if (i == 1)
        aSubDiag(i) = 0.;
      else
      {
        int sqrIm1  = (i - 1) * (i - 1);
        aSubDiag(i) = sqrIm1 / (4. * sqrIm1 - 1);
        aSubDiag(i) = std::sqrt(aSubDiag(i));
      }
    }

    // Compute eigen values.
    math_EigenValuesSearcher EVsearch(aDiag, aSubDiag);

    if (EVsearch.IsDone())
    {
      NCollection_Array1<math_ValueAndWeight> VWarray(1, Number);
      for (i = 1; i <= Number; i++)
      {
        math_Vector anEigenVector = EVsearch.EigenVector(i);
        double      aWeight       = anEigenVector(1);
        aWeight                   = 2. * aWeight * aWeight;
        math_ValueAndWeight EVW(EVsearch.EigenValue(i), aWeight);
        VWarray(i) = EVW;
      }

      std::sort(VWarray.begin(), VWarray.end());

      for (i = 1; i <= Number; i++)
      {
        myPoints->ChangeValue(i)  = VWarray(i).Value();
        myWeights->ChangeValue(i) = VWarray(i).Weight();
      }
      myIsDone = true;
    }
  }
  catch (Standard_Failure const&)
  {
  }
}

bool math_ComputeGaussPointsAndWeights::IsDone() const
{
  return myIsDone;
}

math_Vector math_ComputeGaussPointsAndWeights::Points() const
{
  int         Number = myPoints->Length();
  math_Vector thePoints(1, Number);
  for (int i = 1; i <= Number; i++)
    thePoints(i) = myPoints->Value(i);

  return thePoints;
}

math_Vector math_ComputeGaussPointsAndWeights::Weights() const
{
  int         Number = myWeights->Length();
  math_Vector theWeights(1, Number);
  for (int i = 1; i <= Number; i++)
    theWeights(i) = myWeights->Value(i);

  return theWeights;
}
