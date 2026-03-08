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

#ifndef _Geom2dProp_BezierCurve_HeaderFile
#define _Geom2dProp_BezierCurve_HeaderFile

#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dProp.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief Local differential properties for a 2D Bezier curve.
//!
//! Uses numeric root-finding for curvature extrema and inflection points.
//!
//! @warning The caller must ensure that the adaptor pointer remains valid
//! for the entire lifetime of this object. This class does not manage
//! the adaptor's lifetime.
class Geom2dProp_BezierCurve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with adaptor pointer (non-owning).
  //! @param theAdaptor the 2D curve adaptor (must wrap a Bezier curve, must not be null)
  Geom2dProp_BezierCurve(const Geom2dAdaptor_Curve*  theAdaptor,
                         Geom2dProp::CurveDerivOrder theOrder = Geom2dProp::CurveDerivOrder::Curvature)
      : myAdaptor(theAdaptor),
        myRequestedOrder(theOrder)
  {
  }

  //! Non-copyable and non-movable.
  Geom2dProp_BezierCurve(const Geom2dProp_BezierCurve&)            = delete;
  Geom2dProp_BezierCurve& operator=(const Geom2dProp_BezierCurve&) = delete;
  Geom2dProp_BezierCurve(Geom2dProp_BezierCurve&&)                 = delete;
  Geom2dProp_BezierCurve& operator=(Geom2dProp_BezierCurve&&)      = delete;

  //! Returns the adaptor pointer.
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
  Standard_EXPORT Geom2dProp::CurveAnalysis FindCurvatureExtrema() const;

  //! Find inflection points using numeric root-finding.
  Standard_EXPORT Geom2dProp::CurveAnalysis FindInflections() const;

private:
  const Geom2dAdaptor_Curve*     myAdaptor;
  Geom2dProp::CurveDerivOrder    myRequestedOrder;
  mutable Geom2dProp::CurveCache myCache;
};

#endif // _Geom2dProp_BezierCurve_HeaderFile
