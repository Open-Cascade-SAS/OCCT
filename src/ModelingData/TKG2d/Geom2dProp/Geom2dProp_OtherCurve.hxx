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

#ifndef _Geom2dProp_OtherCurve_HeaderFile
#define _Geom2dProp_OtherCurve_HeaderFile

#include <Adaptor2d_Curve2d.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dProp.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <optional>

//! @brief Fallback local differential properties for any 2D curve type.
//!
//! Uses adaptor D1/D2/D3 methods for property computation
//! and numeric root-finding for curvature extrema and inflection points.
//!
//! Can be constructed from an Adaptor2d_Curve2d pointer or a occ::handle<Geom2d_Curve>.
//! When constructed from a handle, no adaptor is created;
//! for complex methods a stack-local adaptor is created on demand.
class Geom2dProp_OtherCurve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with adaptor pointer (non-owning).
  //! @param theAdaptor the 2D curve adaptor (must not be null)
  Geom2dProp_OtherCurve(
    const Adaptor2d_Curve2d*    theAdaptor,
    Geom2dProp::CurveDerivOrder theOrder = Geom2dProp::CurveDerivOrder::Curvature)
      : myAdaptor(theAdaptor),
        myRequestedOrder(theOrder)
  {
  }

  //! Constructor from geometry handle.
  //! @param theCurve the 2D curve geometry
  //! @param theDomain optional parameter domain (for trimmed curves)
  Geom2dProp_OtherCurve(const occ::handle<Geom2d_Curve>&              theCurve,
                        const std::optional<Geom2dProp::CurveDomain>& theDomain = std::nullopt)
      : myAdaptor(nullptr),
        myRequestedOrder(Geom2dProp::CurveDerivOrder::Curvature),
        myCurve(theCurve),
        myDomain(theDomain)
  {
  }

  //! Non-copyable and non-movable.
  Geom2dProp_OtherCurve(const Geom2dProp_OtherCurve&)            = delete;
  Geom2dProp_OtherCurve& operator=(const Geom2dProp_OtherCurve&) = delete;
  Geom2dProp_OtherCurve(Geom2dProp_OtherCurve&&)                 = delete;
  Geom2dProp_OtherCurve& operator=(Geom2dProp_OtherCurve&&)      = delete;

  //! Returns the adaptor pointer (nullptr when constructed from handle).
  const Adaptor2d_Curve2d* Adaptor() const { return myAdaptor; }

  //! Compute tangent at given parameter.
  Standard_EXPORT Geom2dProp::TangentResult Tangent(double theParam, double theTol) const;

  //! Compute curvature at given parameter.
  Standard_EXPORT Geom2dProp::CurvatureResult Curvature(double theParam, double theTol) const;

  //! Compute normal at given parameter.
  Standard_EXPORT Geom2dProp::NormalResult Normal(double theParam, double theTol) const;

  //! Compute centre of curvature at given parameter.
  Standard_EXPORT Geom2dProp::CentreResult CentreOfCurvature(double theParam, double theTol) const;

  //! Find curvature extrema using numeric root-finding.
  Standard_EXPORT Geom2dProp::CurveAnalysis FindCurvatureExtrema() const;

  //! Find inflection points using numeric root-finding.
  Standard_EXPORT Geom2dProp::CurveAnalysis FindInflections() const;

private:
  const Adaptor2d_Curve2d*               myAdaptor;
  Geom2dProp::CurveDerivOrder            myRequestedOrder;
  mutable Geom2dProp::CurveCache         myCache;
  occ::handle<Geom2d_Curve>              myCurve;  //!< Geometry handle (handle path)
  std::optional<Geom2dProp::CurveDomain> myDomain; //!< Optional parameter domain
};

#endif // _Geom2dProp_OtherCurve_HeaderFile
