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

#ifndef _MathRoot_Multiple_HeaderFile
#define _MathRoot_Multiple_HeaderFile

#include <MathRoot_MultipleUtils.hxx>

//! @file MathRoot_Multiple.hxx
//! @brief Algorithms for finding all roots of a function in a given range.
//!
//! These algorithms sample the function to find sign changes, then refine
//! each root using Brent's method. They can find multiple roots in a single call.

namespace MathRoot
{

//! Finds all real roots of a function within the range [theLower, theUpper].
//! Uses uniform sampling to detect sign changes, then refines each root using Brent's method.
//!
//! Algorithm:
//! 1. Sample function at NbSamples uniform points
//! 2. Detect all sign changes and zero crossings
//! 3. For each bracket, apply Brent's method to find precise root
//! 4. Check for extrema that might touch zero
//!
//! @tparam Function type with Value(double theX, double& theF) method
//! @param theFunc function object to find roots of
//! @param theLower lower bound of search interval
//! @param theUpper upper bound of search interval
//! @param theConfig configuration parameters
//! @return result containing all found roots
template <typename Function>
MultipleResult FindAllRoots(Function&             theFunc,
                            double                theLower,
                            double                theUpper,
                            const MultipleConfig& theConfig = MultipleConfig())
{
  const int   aNbSamples = std::max(theConfig.NbSamples, 10);
  math_Vector aSamples(0, aNbSamples);

  MultipleSampleValueFn<Function>     aSampleFn{theFunc, aSamples, theConfig.Offset};
  MultipleGetValueFn                  aGetValue{aSamples};
  MultipleBrentValueWrapper<Function> aWrapper{theFunc, theConfig.Offset};
  MultipleGetRootValueFn<Function>    aGetRootValue{theFunc};

  return FindAllRootsImpl(theLower,
                          theUpper,
                          theConfig,
                          aSampleFn,
                          aGetValue,
                          aWrapper,
                          aGetRootValue,
                          MultipleNoExtraHandler());
}

//! Finds all real roots of a function with derivative within range [theLower, theUpper].
//! Can also detect extrema where function touches zero without crossing.
//!
//! This version uses derivative information for better extrema detection.
//!
//! @tparam Function type with Values(double theX, double& theF, double& theDF) method
//! @param theFunc function object with derivative
//! @param theLower lower bound of search interval
//! @param theUpper upper bound of search interval
//! @param theConfig configuration parameters
//! @return result containing all found roots
template <typename Function>
MultipleResult FindAllRootsWithDerivative(Function&             theFunc,
                                          double                theLower,
                                          double                theUpper,
                                          const MultipleConfig& theConfig = MultipleConfig())
{
  const int   aNbSamples = std::max(theConfig.NbSamples, 10);
  math_Vector aFValues(0, aNbSamples);
  math_Vector aDFValues(0, aNbSamples);

  MultipleSampleDerivFn<Function>     aSampleFn{theFunc, aFValues, aDFValues, theConfig.Offset};
  MultipleGetValueFn                  aGetValue{aFValues};
  MultipleBrentDerivWrapper<Function> aWrapper{theFunc, theConfig.Offset};
  MultipleGetRootDerivFn<Function>    aGetRootValue{theFunc};
  MultipleTangentialHandler<Function> aTangentialExtra{theFunc,
                                                       aDFValues,
                                                       theConfig.Offset,
                                                       theConfig.FTolerance};

  return FindAllRootsImpl(theLower,
                          theUpper,
                          theConfig,
                          aSampleFn,
                          aGetValue,
                          aWrapper,
                          aGetRootValue,
                          aTangentialExtra);
}

//! Convenience alias using default configuration.
//! @tparam Function type with Value(double, double&) method
//! @param theFunc function to find roots of
//! @param theLower lower bound
//! @param theUpper upper bound
//! @param theNbSamples number of sample points
//! @return result with all found roots
template <typename Function>
MultipleResult FindAllRoots(Function& theFunc, double theLower, double theUpper, int theNbSamples)
{
  MultipleConfig aConfig;
  aConfig.NbSamples = theNbSamples;
  return FindAllRoots(theFunc, theLower, theUpper, aConfig);
}

} // namespace MathRoot

#endif // _MathRoot_Multiple_HeaderFile
