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

#include <GeomAdaptor_Curve.hxx>
#include <GeomProp.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief Local differential properties for a 3D offset curve.
//!
//! Uses numeric root-finding for curvature extrema and inflection points.
//! Local properties are computed from the offset curve's own D1/D2/D3.
//!
//! @warning The caller must ensure that the adaptor pointer remains valid
//! for the entire lifetime of this object. This class does not manage
//! the adaptor's lifetime.
class GeomProp_OffsetCurve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with adaptor pointer (non-owning).
  //! @param theAdaptor the 3D curve adaptor (must wrap an offset curve, must not be null)
  GeomProp_OffsetCurve(const GeomAdaptor_Curve* theAdaptor)
      : myAdaptor(theAdaptor)
  {
  }

  //! Non-copyable and non-movable.
  GeomProp_OffsetCurve(const GeomProp_OffsetCurve&)            = delete;
  GeomProp_OffsetCurve& operator=(const GeomProp_OffsetCurve&) = delete;
  GeomProp_OffsetCurve(GeomProp_OffsetCurve&&)                 = delete;
  GeomProp_OffsetCurve& operator=(GeomProp_OffsetCurve&&)      = delete;

  //! Returns the adaptor pointer.
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
  Standard_EXPORT GeomProp::CurveAnalysis FindCurvatureExtrema() const;

  //! Find inflection points using numeric root-finding.
  Standard_EXPORT GeomProp::CurveAnalysis FindInflections() const;

private:
  const GeomAdaptor_Curve* myAdaptor;
};

#endif // _GeomProp_OffsetCurve_HeaderFile
