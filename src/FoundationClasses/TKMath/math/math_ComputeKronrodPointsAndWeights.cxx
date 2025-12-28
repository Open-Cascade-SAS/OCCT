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

#include <math_ValueAndWeight.hxx>
#include <NCollection_Array1.hxx>
#include <math_ComputeKronrodPointsAndWeights.hxx>
#include <math_EigenValuesSearcher.hxx>
#include <Standard_ErrorHandler.hxx>

#include <algorithm>

math_ComputeKronrodPointsAndWeights::math_ComputeKronrodPointsAndWeights(
  const int Number)
{
  myIsDone = false;

  try
  {
    int i, j;
    int a2NP1 = 2 * Number + 1;

    myPoints  = new NCollection_HArray1<double>(1, a2NP1);
    myWeights = new NCollection_HArray1<double>(1, a2NP1);

    NCollection_Array1<double> aDiag(1, a2NP1);
    NCollection_Array1<double> aSubDiag(1, a2NP1);

    // Initialize symmetric tridiagonal matrix.
    int n         = Number;
    int aKronrodN = 2 * Number + 1;
    int a3KN2p1   = std::min(3 * (Number + 1) / 2 + 1, aKronrodN);
    for (i = 1; i <= a3KN2p1; i++)
    {
      aDiag(i) = 0.;

      if (i == 1)
        aSubDiag(i) = 0.;
      else
      {
        int sqrIm1 = (i - 1) * (i - 1);
        aSubDiag(i)             = sqrIm1 / (4. * sqrIm1 - 1);
      }
    }

    for (i = a3KN2p1 + 1; i <= aKronrodN; i++)
    {
      aDiag(i)    = 0.;
      aSubDiag(i) = 0.;
    }

    // Algorithm calculates weights and points symmetrically and uses -1 index
    // by design. Memory corruption is avoided by moving pointer `s` to the
    // next element and saving original pointer into `ss` for the proper memory
    // releasing. Similarly, `t` and `tt` are addressed.
    int aNd2 = Number / 2;
    double*   s    = new double[aNd2 + 2];
    double*   t    = new double[aNd2 + 2];
    double*   ss   = s++;
    double*   tt   = t++;

    for (i = -1; i <= aNd2; i++)
    {
      s[i] = 0.;
      t[i] = 0.;
    }

    // Generation of Jacobi-Kronrod matrix.
    double* aa = new double[a2NP1 + 1];
    double* bb = new double[a2NP1 + 1];
    for (i = 1; i <= a2NP1; i++)
    {
      aa[i] = aDiag(i);
      bb[i] = aSubDiag(i);
    }
    double*   ptrtmp;
    double    u;
    int m;
    int k;
    int l;

    double* a = aa + 1;
    double* b = bb + 1;

    // Eastward phase.
    t[0] = b[Number + 1];

    for (m = 0; m <= n - 2; m++)
    {
      u = 0;

      for (k = (m + 1) / 2; k >= 0; k--)
      {
        l = m - k;
        u += (a[k + n + 1] - a[l]) * t[k] + b[k + n + 1] * s[k - 1] - b[l] * s[k];
        s[k] = u;
      }

      ptrtmp = t;
      t      = s;
      s      = ptrtmp;
    }

    for (j = aNd2; j >= 0; j--)
      s[j] = s[j - 1];

    // Southward phase.
    for (m = n - 1; m <= 2 * n - 3; m++)
    {
      u = 0;

      for (k = m + 1 - n; k <= (m - 1) / 2; k++)
      {
        l = m - k;
        j = n - 1 - l;
        u += -(a[k + n + 1] - a[l]) * t[j] - b[k + n + 1] * s[j] + b[l] * s[j + 1];
        s[j] = u;
      }

      if (m % 2 == 0)
      {
        k            = m / 2;
        a[k + n + 1] = a[k] + (s[j] - b[k + n + 1] * s[j + 1]) / t[j + 1];
      }
      else
      {
        k            = (m + 1) / 2;
        b[k + n + 1] = s[j] / s[j + 1];
      }

      ptrtmp = t;
      t      = s;
      s      = ptrtmp;
    }

    // Termination phase.
    a[2 * Number] = a[n - 1] - b[2 * Number] * s[0] / t[0];

    delete[] ss;
    delete[] tt;
    for (i = 1; i <= a2NP1; i++)
    {
      aDiag(i)    = aa[i];
      aSubDiag(i) = bb[i];
    }
    delete[] aa;
    delete[] bb;

    for (i = 1; i <= a2NP1; i++)
      aSubDiag(i) = std::sqrt(aSubDiag(i));

    // Compute eigen values.
    math_EigenValuesSearcher EVsearch(aDiag, aSubDiag);

    if (EVsearch.IsDone())
    {
      NCollection_Array1<math_ValueAndWeight> VWarray(1, a2NP1);
      for (i = 1; i <= a2NP1; i++)
      {
        math_Vector   anEigenVector = EVsearch.EigenVector(i);
        double aWeight       = anEigenVector(1);
        aWeight                     = 2. * aWeight * aWeight;
        math_ValueAndWeight EVW(EVsearch.EigenValue(i), aWeight);
        VWarray(i) = EVW;
      }

      std::sort(VWarray.begin(), VWarray.end());

      for (i = 1; i <= a2NP1; i++)
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

bool math_ComputeKronrodPointsAndWeights::IsDone() const
{
  return myIsDone;
}

math_Vector math_ComputeKronrodPointsAndWeights::Points() const
{
  int Number = myPoints->Length();
  math_Vector      thePoints(1, Number);
  for (int i = 1; i <= Number; i++)
    thePoints(i) = myPoints->Value(i);

  return thePoints;
}

math_Vector math_ComputeKronrodPointsAndWeights::Weights() const
{
  int Number = myWeights->Length();
  math_Vector      theWeights(1, Number);
  for (int i = 1; i <= Number; i++)
    theWeights(i) = myWeights->Value(i);

  return theWeights;
}
