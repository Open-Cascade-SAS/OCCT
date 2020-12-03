// Created on: 1997-09-30
// Created by: Roman BORISOV
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _Approx_CurveOnSurface_HeaderFile
#define _Approx_CurveOnSurface_HeaderFile

#include <Adaptor2d_Curve2d.hxx>
#include <Adaptor3d_Surface.hxx>
#include <GeomAbs_Shape.hxx>

class Geom_BSplineCurve;
class Geom2d_BSplineCurve;

//! Approximation of   curve on surface
class Approx_CurveOnSurface 
{
public:

  DEFINE_STANDARD_ALLOC

  //! This constructor calls perform method. This constructor is deprecated.
  Standard_DEPRECATED("This constructor is deprecated. Use other constructor and perform method instead.")
  Standard_EXPORT Approx_CurveOnSurface(const Handle(Adaptor2d_Curve2d)& C2D, const Handle(Adaptor3d_Surface)& Surf, const Standard_Real First, const Standard_Real Last, const Standard_Real Tol, const GeomAbs_Shape Continuity, const Standard_Integer MaxDegree, const Standard_Integer MaxSegments, const Standard_Boolean Only3d = Standard_False, const Standard_Boolean Only2d = Standard_False);

  //! This constructor does not call perform method.
  //! @param theC2D   2D Curve to be approximated in 3D.
  //! @param theSurf  Surface where 2D curve is located.
  //! @param theFirst First parameter of resulting curve.
  //! @param theFirst Last parameter of resulting curve.
  //! @param theTol   Computation tolerance.
  Standard_EXPORT Approx_CurveOnSurface(const Handle(Adaptor2d_Curve2d)& theC2D,
                                        const Handle(Adaptor3d_Surface)& theSurf,
                                        const Standard_Real               theFirst,
                                        const Standard_Real               theLast,
                                        const Standard_Real               theTol);

  Standard_EXPORT Standard_Boolean IsDone() const;
  
  Standard_EXPORT Standard_Boolean HasResult() const;
  
  Standard_EXPORT Handle(Geom_BSplineCurve) Curve3d() const;
  
  Standard_EXPORT Standard_Real MaxError3d() const;
  
  Standard_EXPORT Handle(Geom2d_BSplineCurve) Curve2d() const;
  
  Standard_EXPORT Standard_Real MaxError2dU() const;
  
  //! returns the maximum errors relatively to the  U component or the V component of the
  //! 2d Curve
  Standard_EXPORT Standard_Real MaxError2dV() const;

  //! Constructs the 3d curve. Input parameters are ignored when the input curve is
  //! U-isoline or V-isoline.
  //! @param theMaxSegments Maximal number of segments in the resulting spline.
  //! @param theMaxDegree   Maximal degree of the result.
  //! @param theContinuity  Resulting continuity.
  //! @param theOnly3d      Determines building only 3D curve.
  //! @param theOnly2d      Determines building only 2D curve.
  Standard_EXPORT void Perform(const Standard_Integer theMaxSegments, 
                               const Standard_Integer theMaxDegree, 
                               const GeomAbs_Shape    theContinuity,
                               const Standard_Boolean theOnly3d = Standard_False,
                               const Standard_Boolean theOnly2d = Standard_False);

protected:

  //! Checks whether the 2d curve is a isoline. It can be represented by b-spline, bezier,
  //! or geometric line. This line should have natural parameterization.
  //! @param theC2D       Trimmed curve to be checked.
  //! @param theIsU       Flag indicating that line is u const.
  //! @param theParam     Line parameter.
  //! @param theIsForward Flag indicating forward parameterization on a isoline.
  //! @return Standard_True when 2d curve is a line and Standard_False otherwise.
  Standard_Boolean isIsoLine(const Handle(Adaptor2d_Curve2d) theC2D,
                             Standard_Boolean&                theIsU,
                             Standard_Real&                   theParam,
                             Standard_Boolean&                theIsForward) const;

  //! Builds 3D curve for a isoline. This method takes corresponding isoline from
  //! the input surface.
  //! @param theC2D   Trimmed curve to be approximated.
  //! @param theIsU   Flag indicating that line is u const.
  //! @param theParam Line parameter.
  //! @param theIsForward Flag indicating forward parameterization on a isoline.
  //! @return Standard_True when 3d curve is built and Standard_False otherwise.
  Standard_Boolean buildC3dOnIsoLine(const Handle(Adaptor2d_Curve2d) theC2D,
                                     const Standard_Boolean           theIsU,
                                     const Standard_Real              theParam,
                                     const Standard_Boolean           theIsForward);

private:
  Approx_CurveOnSurface& operator= (const Approx_CurveOnSurface&);

private:

  //! Input curve.
  const Handle(Adaptor2d_Curve2d) myC2D;

  //! Input surface.
  const Handle(Adaptor3d_Surface) mySurf;

  //! First parameter of the result.
  const Standard_Real myFirst;

  //! Last parameter of the result.
  const Standard_Real myLast;

  //! Tolerance.
  Standard_Real myTol;

  Handle(Geom2d_BSplineCurve) myCurve2d;
  Handle(Geom_BSplineCurve) myCurve3d;
  Standard_Boolean myIsDone;
  Standard_Boolean myHasResult;
  Standard_Real myError3d;
  Standard_Real myError2dU;
  Standard_Real myError2dV;

};

#endif // _Approx_CurveOnSurface_HeaderFile
