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

#ifndef _Geom2dProp_Hyperbola_HeaderFile
#define _Geom2dProp_Hyperbola_HeaderFile

#include <Geom2d_Curve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dProp.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <optional>

//! @brief Local differential properties for a 2D hyperbola.
//!
//! A hyperbola has a single curvature extremum (maximum |curvature|) at parameter 0
//! (the vertex). No inflection points exist.
//!
//! Can be constructed from either a Geom2dAdaptor_Curve pointer or a occ::handle<Geom2d_Curve>.
//! When constructed from a handle, no adaptor is created.
class Geom2dProp_Hyperbola
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with adaptor pointer (non-owning).
  //! @param theAdaptor the 2D curve adaptor (must wrap a hyperbola, must not be null)
  Geom2dProp_Hyperbola(
    const Geom2dAdaptor_Curve*  theAdaptor,
    Geom2dProp::CurveDerivOrder theOrder = Geom2dProp::CurveDerivOrder::Undefined)
      : myAdaptor(theAdaptor)
  {
    (void)theOrder;
  }

  //! Constructor from geometry handle.
  //! @param theCurve the 2D hyperbola geometry
  //! @param theDomain optional parameter domain (for trimmed curves)
  Geom2dProp_Hyperbola(const occ::handle<Geom2d_Curve>&              theCurve,
                       const std::optional<Geom2dProp::CurveDomain>& theDomain = std::nullopt)
      : myAdaptor(nullptr),
        myCurve(theCurve),
        myDomain(theDomain)
  {
  }

  //! Non-copyable and non-movable.
  Geom2dProp_Hyperbola(const Geom2dProp_Hyperbola&)            = delete;
  Geom2dProp_Hyperbola& operator=(const Geom2dProp_Hyperbola&) = delete;
  Geom2dProp_Hyperbola(Geom2dProp_Hyperbola&&)                 = delete;
  Geom2dProp_Hyperbola& operator=(Geom2dProp_Hyperbola&&)      = delete;

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

  //! Find curvature extrema on the hyperbola.
  //! Single extremum at parameter 0 (the vertex), if within parameter range.
  Standard_EXPORT Geom2dProp::CurveAnalysis FindCurvatureExtrema() const;

  //! Find inflection points on the hyperbola.
  //! A hyperbola has no inflection points.
  Geom2dProp::CurveAnalysis FindInflections() const { return {{}, true}; }

private:
  const Geom2dAdaptor_Curve*             myAdaptor; //!< Non-owning adaptor pointer (adaptor path)
  occ::handle<Geom2d_Curve>              myCurve;   //!< Geometry handle (handle path)
  std::optional<Geom2dProp::CurveDomain> myDomain;  //!< Optional parameter domain
};

#endif // _Geom2dProp_Hyperbola_HeaderFile
