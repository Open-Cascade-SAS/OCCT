// Created on: 2003-03-18
// Created by: Oleg FEDYAEV
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#ifndef _GeomLib_Tool_HeaderFile
#define _GeomLib_Tool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>

class Geom_Curve;
class Geom_Surface;
class Geom2d_Curve;
class Geom2dAdaptor_Curve;
class gp_Lin2d;
class gp_Pnt;
class gp_Pnt2d;
class gp_Vec2d;

//! Provides various methods with Geom2d and Geom curves and surfaces.
//! The methods of this class compute the parameter(s) of a given point on a
//! curve or a surface. To get the valid result the point must be located rather close
//! to the curve (surface) or at least to allow getting unambiguous result
//! (do not put point at center of circle...),
//! but choice of "trust" distance between curve/surface and point is
//! responsibility of user (parameter MaxDist).
//! Return FALSE if the point is beyond the MaxDist
//! limit or if computation fails.
class GeomLib_Tool
{
public:

  DEFINE_STANDARD_ALLOC

    //! Extracts the parameter of a 3D point lying on a 3D curve
    //! or at a distance less than the MaxDist value.
    Standard_EXPORT static Standard_Boolean Parameter(const Handle(Geom_Curve)& Curve, const gp_Pnt& Point, const Standard_Real MaxDist, Standard_Real& U);

  //! Extracts the parameter of a 3D point lying on a surface
  //! or at a distance less than the MaxDist value.
  Standard_EXPORT static Standard_Boolean Parameters(const Handle(Geom_Surface)& Surface, const gp_Pnt& Point, const Standard_Real MaxDist, Standard_Real& U, Standard_Real& V);

  //! Extracts the parameter of a 2D point lying on a 2D curve
  //! or at a distance less than the MaxDist value.
  Standard_EXPORT static Standard_Boolean Parameter(const Handle(Geom2d_Curve)& Curve, const gp_Pnt2d& Point, const Standard_Real MaxDist, Standard_Real& U);

  //! Computes parameter in theCurve (*thePrmOnCurve) where maximal deviation
  //! between theCurve and the linear segment joining its points with 
  //! the parameters theFPar and theLPar is obtained.
  //! Returns the (positive) value of deviation. Returns negative value if
  //! the deviation cannot be computed.
  //! The returned parameter (in case of successful) will always be in 
  //! the range [theFPar, theLPar].
  //! Iterative method is used for computation. So, theStartParameter is
  //! needed to be set. Recommend value of theStartParameter can be found with
  //! the overloaded method.
  //! Additionally, following values can be returned (optionally):
  //! @param thePtOnCurve - the point on curve where maximal deviation is achieved;
  //! @param thePrmOnCurve - the parameter of thePtOnCurve;
  //! @param theVecCurvLine - the vector along which is computed (this vector is always
  //!                         perpendicular theLine);
  //! @param theLine - the linear segment joining the point of theCurve having parameters
  //!                  theFPar and theLPar.
  Standard_EXPORT static
    Standard_Real ComputeDeviation(const Geom2dAdaptor_Curve& theCurve,
                                  const Standard_Real theFPar,
                                  const Standard_Real theLPar,
                                  const Standard_Real theStartParameter,
                                  const Standard_Integer theNbIters = 100,
                                  Standard_Real* const thePrmOnCurve = NULL,
                                  gp_Pnt2d* const thePtOnCurve = NULL,
                                  gp_Vec2d* const theVecCurvLine = NULL,
                                  gp_Lin2d* const theLine = NULL);

  //! Computes parameter in theCurve (*thePrmOnCurve) where maximal deviation
  //! between theCurve and the linear segment joining its points with 
  //! the parameters theFPar and theLPar is obtained.
  //! Returns the (positive) value of deviation. Returns negative value if
  //! the deviation cannot be computed.
  //! The returned parameter (in case of successful) will always be in 
  //! the range [theFPar, theLPar].
  //! theNbSubIntervals defines discretization of the given interval [theFPar, theLPar]
  //! to provide better search condition. This value should be chosen taking into
  //! account complexity of the curve in considered interval. E.g. if there are many
  //! oscillations of the curve in the interval then theNbSubIntervals mus be 
  //! great number. However, the greater value of theNbSubIntervals the slower the
  //! algorithm will compute.
  //! theNbIters sets number of iterations.
  //!   ATTENTION!!!
  //! This algorithm cannot compute deviation precisely (so, there is no point in
  //! setting big value of theNbIters). But it can give some start point for
  //! the overloaded method.
  Standard_EXPORT static
    Standard_Real ComputeDeviation(const Geom2dAdaptor_Curve& theCurve,
                                   const Standard_Real theFPar,
                                   const Standard_Real theLPar,
                                   const Standard_Integer theNbSubIntervals,
                                   const Standard_Integer theNbIters = 10,
                                   Standard_Real * const thePrmOnCurve = NULL);

};

#endif // _GeomLib_Tool_HeaderFile