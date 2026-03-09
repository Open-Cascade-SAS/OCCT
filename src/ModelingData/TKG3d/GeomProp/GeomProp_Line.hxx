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

#ifndef _GeomProp_Line_HeaderFile
#define _GeomProp_Line_HeaderFile

#include <Geom_Line.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomProp.hxx>
#include <Standard_DefineAlloc.hxx>

#include <optional>

//! @brief Local differential properties for a 3D line.
//!
//! A line has constant tangent, zero curvature, undefined normal and centre.
//! No curvature extrema or inflection points exist.
//!
//! Can be constructed from either a GeomAdaptor_Curve pointer or a occ::handle<Geom_Curve>.
//! When constructed from a handle, no adaptor is created.
class GeomProp_Line
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with adaptor pointer (non-owning).
  //! @param theAdaptor the 3D curve adaptor (must wrap a line, must not be null)
  GeomProp_Line(const GeomAdaptor_Curve*  theAdaptor,
                GeomProp::CurveDerivOrder theOrder = GeomProp::CurveDerivOrder::Undefined)
      : myDirection(theAdaptor->Line().Direction())
  {
    (void)theOrder;
  }

  //! Constructor from geometry handle.
  //! @param theCurve the 3D line geometry (must be a Geom_Line or downcastable to it)
  //! @param theDomain optional parameter domain (unused for line)
  GeomProp_Line(const occ::handle<Geom_Curve>&              theCurve,
                const std::optional<GeomProp::CurveDomain>& theDomain = std::nullopt,
                GeomProp::CurveDerivOrder theOrder = GeomProp::CurveDerivOrder::Undefined)
      : myDirection(occ::down_cast<Geom_Line>(theCurve)->Position().Direction())
  {
    (void)theDomain;
    (void)theOrder;
  }

  //! Non-copyable and non-movable.
  GeomProp_Line(const GeomProp_Line&)            = delete;
  GeomProp_Line& operator=(const GeomProp_Line&) = delete;
  GeomProp_Line(GeomProp_Line&&)                 = delete;
  GeomProp_Line& operator=(GeomProp_Line&&)      = delete;

  //! Sets the derivative caching order (no-op for analytical curves).
  void SetDerivOrder(GeomProp::CurveDerivOrder) {}

  //! Returns the derivative caching order (always Undefined for analytical curves).
  GeomProp::CurveDerivOrder DerivOrder() const { return GeomProp::CurveDerivOrder::Undefined; }

  //! Returns nullptr (no adaptor is stored).
  const GeomAdaptor_Curve* Adaptor() const { return nullptr; }

  //! Compute tangent at given parameter.
  //! For a line, the tangent is always the line direction.
  //! @param[in] theParam curve parameter (unused)
  //! @param[in] theTol tolerance (unused)
  //! @return tangent result (always defined)
  GeomProp::TangentResult Tangent(double theParam, double theTol) const
  {
    (void)theParam;
    (void)theTol;
    return {myDirection, true};
  }

  //! Compute curvature at given parameter.
  //! For a line, curvature is always zero.
  //! @param[in] theParam curve parameter (unused)
  //! @param[in] theTol tolerance (unused)
  //! @return curvature result (always zero)
  GeomProp::CurvatureResult Curvature(double theParam, double theTol) const
  {
    (void)theParam;
    (void)theTol;
    return {0.0, true, false};
  }

  //! Compute normal at given parameter.
  //! For a line, the normal is undefined (zero curvature).
  //! @param[in] theParam curve parameter (unused)
  //! @param[in] theTol tolerance (unused)
  //! @return normal result (always undefined)
  GeomProp::NormalResult Normal(double theParam, double theTol) const
  {
    (void)theParam;
    (void)theTol;
    return {{}, false};
  }

  //! Compute centre of curvature at given parameter.
  //! For a line, the centre is undefined (zero curvature).
  //! @param[in] theParam curve parameter (unused)
  //! @param[in] theTol tolerance (unused)
  //! @return centre result (always undefined)
  GeomProp::CentreResult CentreOfCurvature(double theParam, double theTol) const
  {
    (void)theParam;
    (void)theTol;
    return {{}, false};
  }

  //! Find curvature extrema on the line.
  //! A line has no curvature extrema.
  //! @return empty analysis (always done)
  GeomProp::CurveAnalysis FindCurvatureExtrema() const { return {{}, true}; }

  //! Find inflection points on the line.
  //! A line has no inflection points.
  //! @return empty analysis (always done)
  GeomProp::CurveAnalysis FindInflections() const { return {{}, true}; }

private:
  gp_Dir myDirection; //!< Cached line direction
};

#endif // _GeomProp_Line_HeaderFile
