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

#ifndef _GeomProp_OffsetCurve_HeaderFile
#define _GeomProp_OffsetCurve_HeaderFile

#include <Geom_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomProp.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <optional>

//! @brief Local differential properties for a 3D offset curve.
//!
//! Uses numeric root-finding for curvature extrema and inflection points.
//! Local properties are computed from the offset curve's own D1/D2/D3.
//!
//! Can be constructed from either a GeomAdaptor_Curve pointer or a occ::handle<Geom_Curve>.
//! When constructed from a handle, no adaptor is created; for complex methods
//! (FindCurvatureExtrema, FindInflections) a stack-local adaptor is created on demand.
class GeomProp_OffsetCurve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with adaptor pointer (non-owning).
  //! @param theAdaptor the 3D curve adaptor (must wrap an offset curve, must not be null)
  GeomProp_OffsetCurve(const GeomAdaptor_Curve*  theAdaptor,
                       GeomProp::CurveDerivOrder theOrder = GeomProp::CurveDerivOrder::Curvature)
      : myAdaptor(theAdaptor),
        myRequestedOrder(theOrder)
  {
  }

  //! Constructor from geometry handle.
  //! @param theCurve the 3D offset curve geometry
  //! @param theDomain optional parameter domain (for trimmed curves)
  GeomProp_OffsetCurve(const occ::handle<Geom_Curve>&              theCurve,
                       const std::optional<GeomProp::CurveDomain>& theDomain = std::nullopt,
                       GeomProp::CurveDerivOrder theOrder = GeomProp::CurveDerivOrder::Curvature)
      : myAdaptor(nullptr),
        myRequestedOrder(theOrder),
        myCurve(theCurve),
        myDomain(theDomain)
  {
  }

  //! Non-copyable and non-movable.
  GeomProp_OffsetCurve(const GeomProp_OffsetCurve&)            = delete;
  GeomProp_OffsetCurve& operator=(const GeomProp_OffsetCurve&) = delete;
  GeomProp_OffsetCurve(GeomProp_OffsetCurve&&)                 = delete;
  GeomProp_OffsetCurve& operator=(GeomProp_OffsetCurve&&)      = delete;

  //! Sets the derivative caching order.
  void SetDerivOrder(GeomProp::CurveDerivOrder theOrder) { myRequestedOrder = theOrder; }

  //! Returns the derivative caching order.
  GeomProp::CurveDerivOrder DerivOrder() const { return myRequestedOrder; }

  //! Returns the adaptor pointer (nullptr when constructed from handle).
  const GeomAdaptor_Curve* Adaptor() const { return myAdaptor; }

  //! Returns pointer to underlying geometry, or nullptr if constructed from adaptor.
  const Geom_Curve* Geometry() const { return myCurve.get(); }

  //! Compute tangent at given parameter.
  Standard_EXPORT GeomProp::TangentResult Tangent(double theParam, double theTol) const;

  //! Compute curvature at given parameter.
  Standard_EXPORT GeomProp::CurvatureResult Curvature(double theParam, double theTol) const;

  //! Compute normal at given parameter.
  Standard_EXPORT GeomProp::NormalResult Normal(double theParam, double theTol) const;

  //! Compute centre of curvature at given parameter.
  Standard_EXPORT GeomProp::CentreResult CentreOfCurvature(double theParam, double theTol) const;

  //! Find curvature extrema using numeric root-finding.
  Standard_EXPORT GeomProp::CurveAnalysis FindCurvatureExtrema() const;

  //! Find inflection points using numeric root-finding.
  Standard_EXPORT GeomProp::CurveAnalysis FindInflections() const;

private:
  const GeomAdaptor_Curve*             myAdaptor;
  GeomProp::CurveDerivOrder            myRequestedOrder;
  mutable GeomProp::CurveCache         myCache;
  occ::handle<Geom_Curve>              myCurve;  //!< Geometry handle (handle path)
  std::optional<GeomProp::CurveDomain> myDomain; //!< Optional parameter domain
};

#endif // _GeomProp_OffsetCurve_HeaderFile
