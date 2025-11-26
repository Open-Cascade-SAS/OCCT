// Copyright (c) 2018 OPEN CASCADE SAS
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

#ifndef _BSplCLib_CacheParams_Headerfile
#define _BSplCLib_CacheParams_Headerfile

#include <BSplCLib.hxx>

#include <algorithm>
#include <cmath>

//! Simple structure containing parameters describing parameterization
//! of a B-spline curve or a surface in one direction (U or V),
//! and data of the current span for its caching
struct BSplCLib_CacheParams
{
  const int    Degree;         ///< degree of Bezier/B-spline
  const bool   IsPeriodic;     ///< true of the B-spline is periodic
  const double FirstParameter; ///< first valid parameter
  const double LastParameter;  ///< last valid parameter

  const int SpanIndexMin; ///< minimal index of span
  const int SpanIndexMax; ///< maximal index of span

  double SpanStart;  ///< parameter for the frst point of the span
  double SpanLength; ///< length of the span
  int    SpanIndex;  ///< index of the span

  //! Constructor, prepares data structures for caching.
  //! \param theDegree     degree of the B-spline (or Bezier)
  //! \param thePeriodic   identify whether the B-spline is periodic
  //! \param theFlatKnots  knots of Bezier / B-spline parameterization
  BSplCLib_CacheParams(int theDegree, bool thePeriodic, const TColStd_Array1OfReal& theFlatKnots)
      : Degree(theDegree),
        IsPeriodic(thePeriodic),
        FirstParameter(theFlatKnots.Value(theFlatKnots.Lower() + theDegree)),
        LastParameter(theFlatKnots.Value(theFlatKnots.Upper() - theDegree)),
        SpanIndexMin(theFlatKnots.Lower() + theDegree),
        SpanIndexMax(theFlatKnots.Upper() - theDegree - 1),
        SpanStart(0.0),
        SpanLength(0.0),
        SpanIndex(0)
  {
  }

  //! Normalizes the parameter for periodic B-splines
  //! \param theParameter the value to be normalized into the knots array
  double PeriodicNormalization(double theParameter) const noexcept
  {
    if (IsPeriodic)
    {
      const double aPeriod = LastParameter - FirstParameter;
      if (theParameter < FirstParameter)
      {
        const double aScale = std::trunc((FirstParameter - theParameter) / aPeriod);
        return theParameter + aPeriod * (aScale + 1.0);
      }
      if (theParameter > LastParameter)
      {
        const double aScale = std::trunc((theParameter - LastParameter) / aPeriod);
        return theParameter - aPeriod * (aScale + 1.0);
      }
    }
    return theParameter;
  }

  //! Verifies validity of the cache using flat parameter of the point
  //! \param theParameter parameter of the point placed in the span
  bool IsCacheValid(double theParameter) const noexcept
  {
    const double aNewParam = PeriodicNormalization(theParameter);
    const double aDelta    = aNewParam - SpanStart;
    if (!((aDelta >= 0.0 || SpanIndex == SpanIndexMin)
          && (aDelta < SpanLength || SpanIndex == SpanIndexMax)))
    {
      return false;
    }

    if (SpanIndex == SpanIndexMax)
      return true;

    // from BSplCLib::LocateParameter() check hitting of the next knot
    // within double floating point precision
    const double anEps        = Epsilon((std::min)(std::fabs(LastParameter), std::fabs(aNewParam)));
    const double aDeltaToNext = std::fabs(aDelta - SpanLength);
    if (aDeltaToNext <= anEps)
      return false; // next knot should be used instead

    return true;
  }

  //! Computes span for the specified parameter
  //! \param theParameter parameter of the point placed in the span
  //! \param theFlatKnots  knots of Bezier / B-spline parameterization
  void LocateParameter(double& theParameter, const TColStd_Array1OfReal& theFlatKnots)
  {
    SpanIndex = 0;
    BSplCLib::LocateParameter(Degree,
                              theFlatKnots,
                              BSplCLib::NoMults(),
                              theParameter,
                              IsPeriodic,
                              SpanIndex,
                              theParameter);
    SpanStart  = theFlatKnots.Value(SpanIndex);
    SpanLength = theFlatKnots.Value(SpanIndex + 1) - SpanStart;
  }

  // copying is prohibited
  BSplCLib_CacheParams(const BSplCLib_CacheParams&)            = delete;
  BSplCLib_CacheParams& operator=(const BSplCLib_CacheParams&) = delete;
};

#endif
