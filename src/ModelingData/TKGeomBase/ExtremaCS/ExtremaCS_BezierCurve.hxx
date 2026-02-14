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

#ifndef _ExtremaCS_BezierCurve_HeaderFile
#define _ExtremaCS_BezierCurve_HeaderFile

#include <ExtremaCS.hxx>
#include <Geom_BezierCurve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_DefineAlloc.hxx>

#include <algorithm>

//! @brief Bezier curve evaluator wrapper for curve-surface extrema.
//!
//! Provides uniform interface for evaluating Bezier curve geometry,
//! compatible with ExtremaCS_Numerical template.
class ExtremaCS_BezierCurve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with Bezier curve (uses natural domain).
  explicit ExtremaCS_BezierCurve(const Handle(Geom_BezierCurve)& theCurve)
      : myCurve(theCurve)
  {
    myDomain.Min = theCurve->FirstParameter();
    myDomain.Max = theCurve->LastParameter();
    myNbSamples  = std::max(ExtremaCS::THE_BEZIER_MIN_SAMPLES,
                            theCurve->Degree() * ExtremaCS::THE_BEZIER_DEGREE_MULTIPLIER);
  }

  //! Constructor with Bezier curve and parameter domain.
  ExtremaCS_BezierCurve(const Handle(Geom_BezierCurve)& theCurve,
                        const ExtremaCS::Domain1D&     theDomain)
      : myCurve(theCurve),
        myDomain(theDomain)
  {
    myNbSamples = std::max(ExtremaCS::THE_BEZIER_MIN_SAMPLES,
                           theCurve->Degree() * ExtremaCS::THE_BEZIER_DEGREE_MULTIPLIER);
  }

  //! Evaluates point on curve at parameter.
  gp_Pnt Value(double theT) const { return myCurve->Value(theT); }

  //! Evaluates point and first derivative at parameter.
  void D1(double theT, gp_Pnt& thePt, gp_Vec& theD1) const { myCurve->D1(theT, thePt, theD1); }

  //! Evaluates point and first two derivatives at parameter.
  void D2(double theT, gp_Pnt& thePt, gp_Vec& theD1, gp_Vec& theD2) const
  {
    myCurve->D2(theT, thePt, theD1, theD2);
  }

  //! Returns true if domain is bounded.
  bool IsBounded() const { return myDomain.IsFinite(); }

  //! Returns the domain.
  const ExtremaCS::Domain1D& Domain() const { return myDomain; }

  //! Returns recommended number of samples for this curve.
  int NbSamples() const { return myNbSamples; }

  //! Returns the curve handle.
  const Handle(Geom_BezierCurve)& Curve() const { return myCurve; }

private:
  Handle(Geom_BezierCurve) myCurve;    //!< Bezier curve
  ExtremaCS::Domain1D      myDomain;   //!< Parameter domain
  int                      myNbSamples;//!< Recommended samples
};

#endif // _ExtremaCS_BezierCurve_HeaderFile
