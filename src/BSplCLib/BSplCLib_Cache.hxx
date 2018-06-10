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

#ifndef _BSplCLib_Cache_Headerfile
#define _BSplCLib_Cache_Headerfile

#include <Standard.hxx>
#include <Standard_Macro.hxx>
#include <Standard_Type.hxx>
#include <Standard_Transient.hxx>

#include <gp_Pnt2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec2d.hxx>
#include <gp_Vec.hxx>

#include <TColStd_HArray2OfReal.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>

#include <BSplCLib_CacheParams.hxx>

//! \brief A cache class for Bezier and B-spline curves.
//!
//! Defines all data, that can be cached on a span of a curve.
//! The data should be recalculated in going from span to span.
class BSplCLib_Cache : public Standard_Transient
{
public:

  //! Constructor, prepares data structures for caching values on a 2d curve.
  //! \param theDegree     degree of the curve
  //! \param thePeriodic   identify whether the curve is periodic
  //! \param theFlatKnots  knots of Bezier/B-spline curve (with repetitions)
  //! \param thePoles2d    array of poles of 2D curve
  //! \param theWeights    array of weights of corresponding poles
  Standard_EXPORT BSplCLib_Cache(const Standard_Integer&        theDegree,
                                 const Standard_Boolean&        thePeriodic,
                                 const TColStd_Array1OfReal&    theFlatKnots,
                                 const TColgp_Array1OfPnt2d&    thePoles2d,
                                 const TColStd_Array1OfReal*    theWeights = NULL);

  //! Constructor, prepares data structures for caching values on a 3d curve.
  //! \param theDegree     degree of the curve
  //! \param thePeriodic   identify whether the curve is periodic
  //! \param theFlatKnots  knots of Bezier/B-spline curve (with repetitions)
  //! \param thePoles      array of poles of 3D curve
  //! \param theWeights    array of weights of corresponding poles
  Standard_EXPORT BSplCLib_Cache(const Standard_Integer&        theDegree,
                                 const Standard_Boolean&        thePeriodic,
                                 const TColStd_Array1OfReal&    theFlatKnots,
                                 const TColgp_Array1OfPnt&      thePoles,
                                 const TColStd_Array1OfReal*    theWeights = NULL);

  //! Verifies validity of the cache using flat parameter of the point
  //! \param theParameter parameter of the point placed in the span
  Standard_EXPORT Standard_Boolean IsCacheValid(Standard_Real theParameter) const;

  //! Recomputes the cache data for 2D curves. Does not verify validity of the cache
  //! \param theParameter  the value on the knot's axis to identify the span
  //! \param theFlatKnots  knots of Bezier/B-spline curve (with repetitions)
  //! \param thePoles2d    array of poles of 2D curve
  //! \param theWeights    array of weights of corresponding poles
  Standard_EXPORT void BuildCache(const Standard_Real&           theParameter,
                                  const TColStd_Array1OfReal&    theFlatKnots,
                                  const TColgp_Array1OfPnt2d&    thePoles2d,
                                  const TColStd_Array1OfReal*    theWeights);

  //! Recomputes the cache data for 3D curves. Does not verify validity of the cache
  //! \param theParameter  the value on the knot's axis to identify the span
  //! \param theFlatKnots  knots of Bezier/B-spline curve (with repetitions)
  //! \param thePoles      array of poles of 3D curve
  //! \param theWeights    array of weights of corresponding poles
  Standard_EXPORT void BuildCache(const Standard_Real&           theParameter,
                                  const TColStd_Array1OfReal&    theFlatKnots,
                                  const TColgp_Array1OfPnt&      thePoles,
                                  const TColStd_Array1OfReal*    theWeights = NULL);

  //! Calculates the point on the curve in the specified parameter
  //! \param[in]  theParameter parameter of calculation of the value
  //! \param[out] thePoint     the result of calculation (the point on the curve)
  Standard_EXPORT void D0(const Standard_Real& theParameter, gp_Pnt2d& thePoint) const;
  Standard_EXPORT void D0(const Standard_Real& theParameter, gp_Pnt&   thePoint) const;

  //! Calculates the point on the curve and its first derivative in the specified parameter
  //! \param[in]  theParameter parameter of calculation of the value
  //! \param[out] thePoint     the result of calculation (the point on the curve)
  //! \param[out] theTangent   tangent vector (first derivatives) for the curve in the calculated point
  Standard_EXPORT void D1(const Standard_Real& theParameter, gp_Pnt2d& thePoint, gp_Vec2d& theTangent) const;
  Standard_EXPORT void D1(const Standard_Real& theParameter, gp_Pnt&   thePoint, gp_Vec&   theTangent) const;

  //! Calculates the point on the curve and two derivatives in the specified parameter
  //! \param[in]  theParameter parameter of calculation of the value
  //! \param[out] thePoint     the result of calculation (the point on the curve)
  //! \param[out] theTangent   tangent vector (1st derivatives) for the curve in the calculated point
  //! \param[out] theCurvature curvature vector (2nd derivatives) for the curve in the calculated point
  Standard_EXPORT void D2(const Standard_Real& theParameter, 
                                gp_Pnt2d&      thePoint, 
                                gp_Vec2d&      theTangent, 
                                gp_Vec2d&      theCurvature) const;
  Standard_EXPORT void D2(const Standard_Real& theParameter, 
                                gp_Pnt&        thePoint, 
                                gp_Vec&        theTangent, 
                                gp_Vec&        theCurvature) const;

  //! Calculates the point on the curve and three derivatives in the specified parameter
  //! \param[in]  theParameter parameter of calculation of the value
  //! \param[out] thePoint     the result of calculation (the point on the curve)
  //! \param[out] theTangent   tangent vector (1st derivatives) for the curve in the calculated point
  //! \param[out] theCurvature curvature vector (2nd derivatives) for the curve in the calculated point
  //! \param[out] theTorsion   second curvature vector (3rd derivatives) for the curve in the calculated point
  Standard_EXPORT void D3(const Standard_Real& theParameter, 
                                gp_Pnt2d&      thePoint, 
                                gp_Vec2d&      theTangent, 
                                gp_Vec2d&      theCurvature,
                                gp_Vec2d&      theTorsion) const;
  Standard_EXPORT void D3(const Standard_Real& theParameter, 
                                gp_Pnt&        thePoint, 
                                gp_Vec&        theTangent, 
                                gp_Vec&        theCurvature,
                                gp_Vec&        theTorsion) const;


  DEFINE_STANDARD_RTTIEXT(BSplCLib_Cache,Standard_Transient)

protected:

  //! Fills array of derivatives in the selected point of the curve
  //! \param[in]  theParameter  parameter of the calculation
  //! \param[in]  theDerivative maximal derivative to be calculated (computes all derivatives lesser than specified)
  //! \param[out] theDerivArray result array of derivatives (with size (theDerivative+1)*(PntDim+1), 
  //!                           where PntDim = 2 or 3 is a dimension of the curve)
  void CalculateDerivative(const Standard_Real&    theParameter, 
                           const Standard_Integer& theDerivative, 
                                 Standard_Real&    theDerivArray) const;

  // copying is prohibited
  BSplCLib_Cache (const BSplCLib_Cache&);
  void operator = (const BSplCLib_Cache&);

private:
  Standard_Boolean myIsRational;                //!< identifies the rationality of Bezier/B-spline curve
  BSplCLib_CacheParams myParams;                //!< cache parameters
  Handle(TColStd_HArray2OfReal) myPolesWeights; //!< array of poles and weights of calculated cache
                                                // the array has following structure:
                                                //       x1 y1 [z1] [w1]
                                                //       x2 y2 [z2] [w2] etc
                                                // for 2D-curves there is no z conponent, for non-rational curves there is no weight
};

DEFINE_STANDARD_HANDLE(BSplCLib_Cache, Standard_Transient)

#endif
