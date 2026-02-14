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

#ifndef _ExtremaCC_BezierCurve_HeaderFile
#define _ExtremaCC_BezierCurve_HeaderFile

#include <ExtremaCC.hxx>
#include <Geom_BezierCurve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <optional>

//! @brief Bezier curve evaluator wrapper for curve-curve extrema.
//!
//! Provides uniform interface for evaluating Bezier curve geometry,
//! compatible with ExtremaCC_Numerical and ExtremaCC_GridEvaluator2D templates.
class ExtremaCC_BezierCurve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with Bezier curve (full domain).
  Standard_EXPORT explicit ExtremaCC_BezierCurve(
    const occ::handle<Geom_BezierCurve>& theCurve);

  //! Constructor with Bezier curve and parameter domain.
  Standard_EXPORT ExtremaCC_BezierCurve(const occ::handle<Geom_BezierCurve>& theCurve,
                                        const ExtremaCC::Domain1D&           theDomain);

  //! Evaluates point on curve at parameter.
  Standard_EXPORT gp_Pnt Value(double theU) const;

  //! Evaluates point and first derivative at parameter.
  Standard_EXPORT void D1(double theU, gp_Pnt& thePt, gp_Vec& theD1) const;

  //! Evaluates point and first two derivatives at parameter.
  Standard_EXPORT void D2(double theU, gp_Pnt& thePt, gp_Vec& theD1, gp_Vec& theD2) const;

  //! Returns true if domain is bounded.
  bool IsBounded() const { return true; } // Bezier curves are always bounded

  //! Returns the domain.
  const ExtremaCC::Domain1D& Domain() const { return myDomain; }

  //! Returns the Bezier curve.
  const occ::handle<Geom_BezierCurve>& Curve() const { return myCurve; }

private:
  occ::handle<Geom_BezierCurve> myCurve;   //!< Bezier curve
  ExtremaCC::Domain1D           myDomain;  //!< Parameter domain
};

#endif // _ExtremaCC_BezierCurve_HeaderFile
