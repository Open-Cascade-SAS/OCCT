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

namespace MathUtils
{
namespace Utils
{
struct ValueAndWeight
{
  double Value  = 0.0;
  double Weight = 0.0;

  bool operator<(const ValueAndWeight& theOther) const { return Value < theOther.Value; }
};

bool ComputeGaussLegendre(const int theOrder, math_Vector& thePoints, math_Vector& theWeights)
{
  if (theOrder < 1 || thePoints.Size() != static_cast<size_t>(theOrder)
      || theWeights.Size() != static_cast<size_t>(theOrder))
  {
    return false;
  }

  try
  {
    const size_t anOrder = static_cast<size_t>(theOrder);
    math_Vector  aDiag(anOrder);
    math_Vector  aSubDiag(anOrder);

    for (size_t i = 0; i < anOrder; ++i)
    {
      aDiag.ChangeAt(i)    = 0.0;
      aSubDiag.ChangeAt(i) = 0.0;
      if (i > 0)
      {
        const size_t aSqrI = i * i;
        aSubDiag.ChangeAt(i) = std::sqrt(static_cast<double>(aSqrI) / (4.0 * aSqrI - 1.0));
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
    for (size_t i = 0; i < anOrder; ++i)
    {
      const double aWeight = 2.0 * aEigenVecs.At(0, i) * aEigenVecs.At(0, i);
      aValuesAndWeights(static_cast<int>(i) + 1) = {aEigenValues.At(i), aWeight};
    }

    std::sort(aValuesAndWeights.begin(), aValuesAndWeights.end());

    for (size_t i = 0; i < anOrder; ++i)
    {
      const ValueAndWeight& aValueAndWeight = aValuesAndWeights(static_cast<int>(i) + 1);
      thePoints.ChangeAt(i)                 = aValueAndWeight.Value;
      theWeights.ChangeAt(i)                = aValueAndWeight.Weight;
    }

    return true;
  }
  catch (Standard_Failure const&)
  {
    return false;
  }
}

} // namespace Utils
} // namespace MathUtils

//==================================================================================================

bool MathUtils::GetGaussPointsAndWeights(int          theOrder,
                                         math_Vector& thePoints,
                                         math_Vector& theWeights)
{
  if (theOrder < 1 || thePoints.Size() != static_cast<size_t>(theOrder)
      || theWeights.Size() != static_cast<size_t>(theOrder))
  {
    return false;
  }
  if (theOrder <= math::GaussPointsMax())
  {
    const size_t anOrder = static_cast<size_t>(theOrder);
    math_Vector  aPoints(anOrder);
    math_Vector  aWeights(anOrder);
    if (!math::OrderedGaussPointsAndWeights(theOrder, aPoints, aWeights))
    {
      return false;
    }
    for (size_t i = 0; i < anOrder; ++i)
    {
      thePoints.ChangeAt(i)  = aPoints.At(i);
      theWeights.ChangeAt(i) = aWeights.At(i);
    }
    return true;
  }

  return Utils::ComputeGaussLegendre(theOrder, thePoints, theWeights);
}
