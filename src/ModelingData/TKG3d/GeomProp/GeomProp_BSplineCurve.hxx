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

#ifndef _GeomProp_BSplineCurve_HeaderFile
#define _GeomProp_BSplineCurve_HeaderFile

#include <Geom_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomProp.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <optional>

//! @brief Local differential properties for a 3D B-spline curve.
//!
//! Uses numeric root-finding for curvature extrema and inflection points.
//! For B-splines with continuity less than C3, the parameter range is subdivided
//! into C3 intervals for more robust root-finding.
//!
//! Can be constructed from either a GeomAdaptor_Curve pointer or a Handle(Geom_Curve).
//! When constructed from a handle, no adaptor is created; for complex methods
//! (FindCurvatureExtrema, FindInflections) a stack-local adaptor is created on demand.
class GeomProp_BSplineCurve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with adaptor pointer (non-owning).
  //! @param theAdaptor the 3D curve adaptor (must wrap a B-spline curve, must not be null)
  GeomProp_BSplineCurve(const GeomAdaptor_Curve*  theAdaptor,
                        GeomProp::CurveDerivOrder theOrder = GeomProp::CurveDerivOrder::Curvature)
      : myAdaptor(theAdaptor),
        myRequestedOrder(theOrder)
  {
  }

  //! Constructor from geometry handle.
  //! @param theCurve the 3D B-spline curve geometry
  //! @param theDomain optional parameter domain (for trimmed curves)
  GeomProp_BSplineCurve(const Handle(Geom_Curve)&                      theCurve,
                        const std::optional<GeomProp::CurveDomain>& theDomain = std::nullopt)
      : myAdaptor(nullptr),
        myRequestedOrder(GeomProp::CurveDerivOrder::Curvature),
        myCurve(theCurve),
        myDomain(theDomain)
  {
  }

  //! Non-copyable and non-movable.
  GeomProp_BSplineCurve(const GeomProp_BSplineCurve&)            = delete;
  GeomProp_BSplineCurve& operator=(const GeomProp_BSplineCurve&) = delete;
  GeomProp_BSplineCurve(GeomProp_BSplineCurve&&)                 = delete;
  GeomProp_BSplineCurve& operator=(GeomProp_BSplineCurve&&)      = delete;

  //! Returns the adaptor pointer (nullptr when constructed from handle).
  const GeomAdaptor_Curve* Adaptor() const { return myAdaptor; }

  //! Compute tangent at given parameter.
  Standard_EXPORT GeomProp::TangentResult Tangent(double theParam, double theTol) const;

  //! Compute curvature at given parameter.
  Standard_EXPORT GeomProp::CurvatureResult Curvature(double theParam, double theTol) const;

  //! Compute normal at given parameter.
  Standard_EXPORT GeomProp::NormalResult Normal(double theParam, double theTol) const;

  //! Compute centre of curvature at given parameter.
  Standard_EXPORT GeomProp::CentreResult CentreOfCurvature(double theParam, double theTol) const;

  //! Find curvature extrema using numeric root-finding.
  //! For non-C3 B-splines, subdivides into C3 intervals.
  Standard_EXPORT GeomProp::CurveAnalysis FindCurvatureExtrema() const;

  //! Find inflection points using numeric root-finding.
  //! For non-C3 B-splines, subdivides into C3 intervals.
  Standard_EXPORT GeomProp::CurveAnalysis FindInflections() const;

private:
  const GeomAdaptor_Curve*              myAdaptor;
  GeomProp::CurveDerivOrder             myRequestedOrder;
  mutable GeomProp::CurveCache          myCache;
  Handle(Geom_Curve) myCurve;                      //!< Geometry handle (handle path)
  std::optional<GeomProp::CurveDomain> myDomain;  //!< Optional parameter domain
};

#endif // _GeomProp_BSplineCurve_HeaderFile
