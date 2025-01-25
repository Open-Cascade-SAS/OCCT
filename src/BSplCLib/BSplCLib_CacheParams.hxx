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

//! Simple structure containing parameters describing parameterization
//! of a B-spline curve or a surface in one direction (U or V),
//! and data of the current span for its caching
struct BSplCLib_CacheParams
{
  const Standard_Integer Degree;         ///< degree of Bezier/B-spline
  const Standard_Boolean IsPeriodic;     ///< true of the B-spline is periodic
  const Standard_Real    FirstParameter; ///< first valid parameter
  const Standard_Real    LastParameter;  ///< last valid parameter

  const Standard_Integer SpanIndexMin; ///< minimal index of span
  const Standard_Integer SpanIndexMax; ///< maximal index of span

  Standard_Real    SpanStart;  ///< parameter for the frst point of the span
  Standard_Real    SpanLength; ///< length of the span
  Standard_Integer SpanIndex;  ///< index of the span

  //! Constructor, prepares data structures for caching.
  //! \param theDegree     degree of the B-spline (or Bezier)
  //! \param thePeriodic   identify whether the B-spline is periodic
  //! \param theFlatKnots  knots of Bezier / B-spline parameterization
  BSplCLib_CacheParams(Standard_Integer            theDegree,
                       Standard_Boolean            thePeriodic,
                       const TColStd_Array1OfReal& theFlatKnots)
      : Degree(theDegree),
        IsPeriodic(thePeriodic),
        FirstParameter(theFlatKnots.Value(theFlatKnots.Lower() + theDegree)),
        LastParameter(theFlatKnots.Value(theFlatKnots.Upper() - theDegree)),
        SpanIndexMin(theFlatKnots.Lower() + theDegree),
        SpanIndexMax(theFlatKnots.Upper() - theDegree - 1),
        SpanStart(0.),
        SpanLength(0.),
        SpanIndex(0)
  {
  }

  //! Normalizes the parameter for periodic B-splines
  //! \param theParameter the value to be normalized into the knots array
  Standard_Real PeriodicNormalization(Standard_Real theParameter) const
  {
    if (IsPeriodic)
    {
      if (theParameter < FirstParameter)
      {
        Standard_Real aPeriod = LastParameter - FirstParameter;
        Standard_Real aScale  = IntegerPart((FirstParameter - theParameter) / aPeriod);
        return theParameter + aPeriod * (aScale + 1.0);
      }
      if (theParameter > LastParameter)
      {
        Standard_Real aPeriod = LastParameter - FirstParameter;
        Standard_Real aScale  = IntegerPart((theParameter - LastParameter) / aPeriod);
        return theParameter - aPeriod * (aScale + 1.0);
      }
    }
    return theParameter;
  }

  //! Verifies validity of the cache using flat parameter of the point
  //! \param theParameter parameter of the point placed in the span
  Standard_Boolean IsCacheValid(Standard_Real theParameter) const
  {
    Standard_Real aNewParam = PeriodicNormalization(theParameter);
    Standard_Real aDelta    = aNewParam - SpanStart;
    return ((aDelta >= 0.0 || SpanIndex == SpanIndexMin)
            && (aDelta < SpanLength || SpanIndex == SpanIndexMax));
  }

  //! Computes span for the specified parameter
  //! \param theParameter parameter of the point placed in the span
  //! \param theFlatKnots  knots of Bezier / B-spline parameterization
  void LocateParameter(Standard_Real& theParameter, const TColStd_Array1OfReal& theFlatKnots)
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

private:
  // copying is prohibited
  BSplCLib_CacheParams(const BSplCLib_CacheParams&);
  void operator=(const BSplCLib_CacheParams&);
};

#endif
