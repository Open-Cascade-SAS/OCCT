// Copyright (c) 2014 OPEN CASCADE SAS
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

#ifndef _BSplSLib_Cache_Headerfile
#define _BSplSLib_Cache_Headerfile

#include <TColStd_HArray2OfReal.hxx>
#include <TColStd_Array2OfReal.hxx>

#include <BSplCLib_CacheParams.hxx>

//! \brief A cache class for Bezier and B-spline surfaces.
//!
//! Defines all data, that can be cached on a span of the surface.
//! The data should be recalculated in going from span to span.
class BSplSLib_Cache : public Standard_Transient
{
public:
  //! Constructor for caching of the span for the surface
  //! \param theDegreeU    degree along the first parameter (U) of the surface
  //! \param thePeriodicU  identify the surface is periodical along U axis
  //! \param theFlatKnotsU knots of the surface (with repetition) along U axis
  //! \param theDegreeV    degree along the second parameter (V) of the surface
  //! \param thePeriodicV  identify the surface is periodical along V axis
  //! \param theFlatKnotsV knots of the surface (with repetition) along V axis
  //! \param theWeights    array of weights of corresponding poles
  Standard_EXPORT BSplSLib_Cache(const Standard_Integer&     theDegreeU,
                                 const Standard_Boolean&     thePeriodicU,
                                 const TColStd_Array1OfReal& theFlatKnotsU,
                                 const Standard_Integer&     theDegreeV,
                                 const Standard_Boolean&     thePeriodicV,
                                 const TColStd_Array1OfReal& theFlatKnotsV,
                                 const TColStd_Array2OfReal* theWeights = NULL);

  //! Verifies validity of the cache using parameters of the point
  //! \param theParameterU  first parameter of the point placed in the span
  //! \param theParameterV  second parameter of the point placed in the span
  Standard_EXPORT Standard_Boolean IsCacheValid(Standard_Real theParameterU,
                                                Standard_Real theParameterV) const;

  //! Recomputes the cache data. Does not verify validity of the cache
  //! \param theParameterU  the parametric value on the U axis to identify the span
  //! \param theParameterV  the parametric value on the V axis to identify the span
  //! \param theDegreeU     degree along U axis
  //! \param thePeriodicU   identify whether the surface is periodic along U axis
  //! \param theFlatKnotsU  flat knots of the surface along U axis
  //! \param theDegreeV     degree along V axis
  //! \param thePeriodicV   identify whether the surface is periodic along V axis
  //! \param theFlatKnotsV  flat knots of the surface along V axis
  //! \param thePoles       array of poles of the surface
  //! \param theWeights     array of weights of corresponding poles
  Standard_EXPORT void BuildCache(const Standard_Real&        theParameterU,
                                  const Standard_Real&        theParameterV,
                                  const TColStd_Array1OfReal& theFlatKnotsU,
                                  const TColStd_Array1OfReal& theFlatKnotsV,
                                  const TColgp_Array2OfPnt&   thePoles,
                                  const TColStd_Array2OfReal* theWeights = NULL);

  //! Calculates the point on the surface for specified parameters
  //! \param[in]  theU      first parameter for calculation of the value
  //! \param[in]  theV      second parameter for calculation of the value
  //! \param[out] thePoint  the result of calculation (the point on the surface)
  Standard_EXPORT void D0(const Standard_Real& theU,
                          const Standard_Real& theV,
                          gp_Pnt&              thePoint) const;

  //! Calculates the point on the surface and its first derivative
  //! \param[in]  theU         first parameter of calculation of the value
  //! \param[in]  theV         second parameter of calculation of the value
  //! \param[out] thePoint     the result of calculation (the point on the surface)
  //! \param[out] theTangentU  tangent vector along U axis in the calculated point
  //! \param[out] theTangentV  tangent vector along V axis in the calculated point
  Standard_EXPORT void D1(const Standard_Real& theU,
                          const Standard_Real& theV,
                          gp_Pnt&              thePoint,
                          gp_Vec&              theTangentU,
                          gp_Vec&              theTangentV) const;

  //! Calculates the point on the surface and derivatives till second order
  //! \param[in]  theU            first parameter of calculation of the value
  //! \param[in]  theV            second parameter of calculation of the value
  //! \param[out] thePoint        the result of calculation (the point on the surface)
  //! \param[out] theTangentU     tangent vector along U axis in the calculated point
  //! \param[out] theTangentV     tangent vector along V axis in the calculated point
  //! \param[out] theCurvatureU   curvature vector (2nd derivative on U) along U axis
  //! \param[out] theCurvatureV   curvature vector (2nd derivative on V) along V axis
  //! \param[out] theCurvatureUV  2nd mixed derivative on U anv V
  Standard_EXPORT void D2(const Standard_Real& theU,
                          const Standard_Real& theV,
                          gp_Pnt&              thePoint,
                          gp_Vec&              theTangentU,
                          gp_Vec&              theTangentV,
                          gp_Vec&              theCurvatureU,
                          gp_Vec&              theCurvatureV,
                          gp_Vec&              theCurvatureUV) const;

  DEFINE_STANDARD_RTTIEXT(BSplSLib_Cache, Standard_Transient)

private:
  // copying is prohibited
  BSplSLib_Cache(const BSplSLib_Cache&);
  void operator=(const BSplSLib_Cache&);

private:
  // clang-format off
  Standard_Boolean myIsRational;                //!< identifies the rationality of Bezier/B-spline surface
  BSplCLib_CacheParams myParamsU, myParamsV;    //!< cache parameters by U and V directions
  Handle(TColStd_HArray2OfReal) myPolesWeights; //!< array of poles and weights of calculated cache
                                                // the array has following structure:
                                                //       x11 y11 z11 [w11] x12 y12 z12 [w12] ...
                                                //       x21 y21 z21 [w21] x22 y22 z22 [w22] etc
                                                // for non-rational surfaces there is no weight;
                                                // size of array: (max(myDegree)+1) * A*(min(myDegree)+1), where A = 4 or 3
  // clang-format on
};

DEFINE_STANDARD_HANDLE(BSplSLib_Cache, Standard_Transient)

#endif
