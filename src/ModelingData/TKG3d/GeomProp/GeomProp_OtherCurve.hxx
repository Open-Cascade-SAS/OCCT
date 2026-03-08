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

#ifndef _GeomProp_OtherCurve_HeaderFile
#define _GeomProp_OtherCurve_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomProp.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief Fallback local differential properties for any 3D curve type.
//!
//! Uses adaptor D1/D2/D3 methods for property computation
//! and numeric root-finding for curvature extrema and inflection points.
//!
//! @warning The caller must ensure that the adaptor pointer remains valid
//! for the entire lifetime of this object. This class does not manage
//! the adaptor's lifetime.
class GeomProp_OtherCurve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with adaptor pointer (non-owning).
  //! @param theAdaptor the 3D curve adaptor (must not be null)
  GeomProp_OtherCurve(const GeomAdaptor_Curve*  theAdaptor,
                      GeomProp::CurveDerivOrder theOrder = GeomProp::CurveDerivOrder::Curvature)
      : myAdaptor(theAdaptor),
        myRequestedOrder(theOrder)
  {
  }

  GeomProp_OtherCurve(const Adaptor3d_Curve*    theAdaptor,
                      GeomProp::CurveDerivOrder theOrder = GeomProp::CurveDerivOrder::Curvature)
      : myAdaptor(theAdaptor),
        myRequestedOrder(theOrder)
  {
  }

  //! Non-copyable and non-movable.
  GeomProp_OtherCurve(const GeomProp_OtherCurve&)            = delete;
  GeomProp_OtherCurve& operator=(const GeomProp_OtherCurve&) = delete;
  GeomProp_OtherCurve(GeomProp_OtherCurve&&)                 = delete;
  GeomProp_OtherCurve& operator=(GeomProp_OtherCurve&&)      = delete;

  //! Returns the adaptor pointer.
  const GeomAdaptor_Curve* Adaptor() const
  {
    return dynamic_cast<const GeomAdaptor_Curve*>(myAdaptor);
  }

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
  const Adaptor3d_Curve*       myAdaptor;
  GeomProp::CurveDerivOrder    myRequestedOrder;
  mutable GeomProp::CurveCache myCache;
};

#endif // _GeomProp_OtherCurve_HeaderFile
