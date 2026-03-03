// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <MathUtils_Gauss.hxx>

#include <MathLin_EigenSearch.hxx>

#include <math.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Failure.hxx>

#include <algorithm>
#include <cmath>

namespace
{
struct ValueAndWeight
{
  double Value  = 0.0;
  double Weight = 0.0;

  bool operator<(const ValueAndWeight& theOther) const { return Value < theOther.Value; }
};

bool ComputeGaussLegendre(const int theOrder, math_Vector& thePoints, math_Vector& theWeights)
{
  if (theOrder < 1 || thePoints.Length() != theOrder || theWeights.Length() != theOrder)
  {
    return false;
  }

  try
  {
    math_Vector aDiag(1, theOrder);
    math_Vector aSubDiag(1, theOrder);

    for (int i = 1; i <= theOrder; ++i)
    {
      aDiag(i)    = 0.0;
      aSubDiag(i) = 0.0;
      if (i > 1)
      {
        const int aSqrIm1 = (i - 1) * (i - 1);
        aSubDiag(i)       = std::sqrt(static_cast<double>(aSqrIm1) / (4.0 * aSqrIm1 - 1.0));
      }
    }

    const MathLin::EigenResult anEigen = MathLin::EigenTridiagonal(aDiag, aSubDiag);
    if (!anEigen.IsDone() || !anEigen.EigenValues.has_value() || !anEigen.EigenVectors.has_value())
    {
      return false;
    }

    const math_Vector& aEigenValues = *anEigen.EigenValues;
    const math_Matrix& aEigenVecs   = *anEigen.EigenVectors;

    NCollection_Array1<ValueAndWeight> aValuesAndWeights(1, theOrder);
    const int                          aVecLowerRow = aEigenVecs.LowerRow();
    const int                          aVecLowerCol = aEigenVecs.LowerCol();
    const int                          aValLower    = aEigenValues.Lower();
    for (int i = 1; i <= theOrder; ++i)
    {
      const double aWeight = 2.0 * aEigenVecs(aVecLowerRow, aVecLowerCol + i - 1)
                             * aEigenVecs(aVecLowerRow, aVecLowerCol + i - 1);
      aValuesAndWeights(i) = {aEigenValues(aValLower + i - 1), aWeight};
    }

    std::sort(aValuesAndWeights.begin(), aValuesAndWeights.end());

    const int aPointLower  = thePoints.Lower();
    const int aWeightLower = theWeights.Lower();
    for (int i = 1; i <= theOrder; ++i)
    {
      thePoints(aPointLower + i - 1)   = aValuesAndWeights(i).Value;
      theWeights(aWeightLower + i - 1) = aValuesAndWeights(i).Weight;
    }

    return true;
  }
  catch (Standard_Failure const&)
  {
    return false;
  }
}

} // namespace

//==================================================================================================

bool MathUtils::GetGaussPointsAndWeights(int          theOrder,
                                         math_Vector& thePoints,
                                         math_Vector& theWeights)
{
  if (theOrder < 1 || thePoints.Length() != theOrder || theWeights.Length() != theOrder)
  {
    return false;
  }

  if (theOrder <= math::GaussPointsMax())
  {
    return math::OrderedGaussPointsAndWeights(theOrder, thePoints, theWeights);
  }

  return ComputeGaussLegendre(theOrder, thePoints, theWeights);
}
