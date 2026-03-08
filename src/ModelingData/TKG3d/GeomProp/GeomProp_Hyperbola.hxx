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

#ifndef _GeomProp_Hyperbola_HeaderFile
#define _GeomProp_Hyperbola_HeaderFile

#include <Geom_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomProp.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <optional>

//! @brief Local differential properties for a 3D hyperbola.
//!
//! A hyperbola has a single curvature extremum (maximum |curvature|) at parameter 0
//! (the vertex). No inflection points exist.
//!
//! Can be constructed from either a GeomAdaptor_Curve pointer or a Handle(Geom_Curve).
//! When constructed from a handle, no adaptor is created.
class GeomProp_Hyperbola
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with adaptor pointer (non-owning).
  //! @param theAdaptor the 3D curve adaptor (must wrap a hyperbola, must not be null)
  GeomProp_Hyperbola(const GeomAdaptor_Curve*  theAdaptor,
                     GeomProp::CurveDerivOrder theOrder = GeomProp::CurveDerivOrder::Undefined)
      : myAdaptor(theAdaptor)
  {
    (void)theOrder;
  }

  //! Constructor from geometry handle.
  //! @param theCurve the 3D hyperbola geometry
  //! @param theDomain optional parameter domain (for trimmed curves)
  GeomProp_Hyperbola(const Handle(Geom_Curve)&                      theCurve,
                     const std::optional<GeomProp::CurveDomain>& theDomain = std::nullopt)
      : myAdaptor(nullptr),
        myCurve(theCurve),
        myDomain(theDomain)
  {
  }

  //! Non-copyable and non-movable.
  GeomProp_Hyperbola(const GeomProp_Hyperbola&)            = delete;
  GeomProp_Hyperbola& operator=(const GeomProp_Hyperbola&) = delete;
  GeomProp_Hyperbola(GeomProp_Hyperbola&&)                 = delete;
  GeomProp_Hyperbola& operator=(GeomProp_Hyperbola&&)      = delete;

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

  //! Find curvature extrema on the hyperbola.
  //! Single extremum at parameter 0 (the vertex), if within parameter range.
  Standard_EXPORT GeomProp::CurveAnalysis FindCurvatureExtrema() const;

  //! Find inflection points on the hyperbola.
  //! A hyperbola has no inflection points.
  GeomProp::CurveAnalysis FindInflections() const { return {{}, true}; }

private:
  const GeomAdaptor_Curve*              myAdaptor; //!< Non-owning adaptor pointer (adaptor path)
  Handle(Geom_Curve) myCurve;                      //!< Geometry handle (handle path)
  std::optional<GeomProp::CurveDomain> myDomain;  //!< Optional parameter domain
};

#endif // _GeomProp_Hyperbola_HeaderFile
