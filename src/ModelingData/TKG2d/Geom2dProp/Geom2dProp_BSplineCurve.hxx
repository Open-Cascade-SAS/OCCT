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

#ifndef _Geom2dProp_BSplineCurve_HeaderFile
#define _Geom2dProp_BSplineCurve_HeaderFile

#include <Geom2d_Curve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dProp.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <optional>

//! @brief Local differential properties for a 2D B-spline curve.
//!
//! Uses numeric root-finding for curvature extrema and inflection points.
//! For B-splines with continuity less than C3, the parameter range is subdivided
//! into C3 intervals for more robust root-finding.
//!
//! Can be constructed from either a Geom2dAdaptor_Curve pointer or a occ::handle<Geom2d_Curve>.
//! When constructed from a handle, no adaptor is created; for complex methods
//! (FindCurvatureExtrema, FindInflections) a stack-local adaptor is created on demand.
class Geom2dProp_BSplineCurve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with adaptor pointer (non-owning).
  //! @param theAdaptor the 2D curve adaptor (must wrap a B-spline curve, must not be null)
  Geom2dProp_BSplineCurve(
    const Geom2dAdaptor_Curve*  theAdaptor,
    Geom2dProp::CurveDerivOrder theOrder = Geom2dProp::CurveDerivOrder::Curvature)
      : myAdaptor(theAdaptor),
        myRequestedOrder(theOrder)
  {
  }

  //! Constructor from geometry handle.
  //! @param theCurve the 2D B-spline curve geometry
  //! @param theDomain optional parameter domain (for trimmed curves)
  Geom2dProp_BSplineCurve(
    const occ::handle<Geom2d_Curve>&              theCurve,
    const std::optional<Geom2dProp::CurveDomain>& theDomain = std::nullopt,
    Geom2dProp::CurveDerivOrder                   theOrder = Geom2dProp::CurveDerivOrder::Curvature)
      : myAdaptor(nullptr),
        myRequestedOrder(theOrder),
        myCurve(theCurve),
        myDomain(theDomain)
  {
  }

  //! Non-copyable and non-movable.
  Geom2dProp_BSplineCurve(const Geom2dProp_BSplineCurve&)            = delete;
  Geom2dProp_BSplineCurve& operator=(const Geom2dProp_BSplineCurve&) = delete;
  Geom2dProp_BSplineCurve(Geom2dProp_BSplineCurve&&)                 = delete;
  Geom2dProp_BSplineCurve& operator=(Geom2dProp_BSplineCurve&&)      = delete;

  //! Sets the derivative caching order.
  void SetDerivOrder(Geom2dProp::CurveDerivOrder theOrder) { myRequestedOrder = theOrder; }

  //! Returns the derivative caching order.
  Geom2dProp::CurveDerivOrder DerivOrder() const { return myRequestedOrder; }

  //! Returns the adaptor pointer (nullptr when constructed from handle).
  const Geom2dAdaptor_Curve* Adaptor() const { return myAdaptor; }

  //! Compute tangent at given parameter.
  Standard_EXPORT Geom2dProp::TangentResult Tangent(double theParam, double theTol) const;

  //! Compute curvature at given parameter.
  Standard_EXPORT Geom2dProp::CurvatureResult Curvature(double theParam, double theTol) const;

  //! Compute normal at given parameter.
  Standard_EXPORT Geom2dProp::NormalResult Normal(double theParam, double theTol) const;

  //! Compute centre of curvature at given parameter.
  Standard_EXPORT Geom2dProp::CentreResult CentreOfCurvature(double theParam, double theTol) const;

  //! Find curvature extrema using numeric root-finding.
  //! For non-C3 B-splines, subdivides into C3 intervals.
  Standard_EXPORT Geom2dProp::CurveAnalysis FindCurvatureExtrema() const;

  //! Find inflection points using numeric root-finding.
  //! For non-C3 B-splines, subdivides into C3 intervals.
  Standard_EXPORT Geom2dProp::CurveAnalysis FindInflections() const;

private:
  const Geom2dAdaptor_Curve*             myAdaptor;
  Geom2dProp::CurveDerivOrder            myRequestedOrder;
  mutable Geom2dProp::CurveCache         myCache;
  occ::handle<Geom2d_Curve>              myCurve;  //!< Geometry handle (handle path)
  std::optional<Geom2dProp::CurveDomain> myDomain; //!< Optional parameter domain
};

#endif // _Geom2dProp_BSplineCurve_HeaderFile
