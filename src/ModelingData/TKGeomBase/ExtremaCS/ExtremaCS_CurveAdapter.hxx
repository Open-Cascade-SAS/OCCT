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

#ifndef _ExtremaCS_CurveAdapter_HeaderFile
#define _ExtremaCS_CurveAdapter_HeaderFile

#include <ExtremaCS.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

//! @brief Lightweight adapter for using GeomAdaptor_Curve with extrema templates.
//!
//! This class provides the interface required by ExtremaCS_GridEvaluator3D
//! and ExtremaCS_Numerical templates, forwarding calls directly to
//! the underlying GeomAdaptor_Curve.
//!
//! Replaces the previous separate wrapper classes (ExtremaCS_BezierCurve,
//! ExtremaCS_BSplineCurve, ExtremaCS_OffsetCurve, ExtremaCS_OtherCurve)
//! with a single unified adapter.
//!
//! @note This class stores a non-owning pointer to the curve adaptor.
//!       The caller must ensure the adaptor outlives this adapter.
class ExtremaCS_CurveAdapter
{
public:
  //! Constructor with curve adaptor and domain.
  //! @param[in] theCurve curve adaptor (must outlive this object)
  //! @param[in] theDomain parameter domain
  ExtremaCS_CurveAdapter(const GeomAdaptor_Curve&   theCurve,
                         const ExtremaCS::Domain1D& theDomain)
      : myCurve(&theCurve),
        myDomain(theDomain)
  {
  }

  //! Constructor with curve adaptor using its natural bounds.
  //! @param[in] theCurve curve adaptor (must outlive this object)
  explicit ExtremaCS_CurveAdapter(const GeomAdaptor_Curve& theCurve)
      : myCurve(&theCurve),
        myDomain(theCurve.FirstParameter(), theCurve.LastParameter())
  {
  }

  //! Evaluates point on curve at parameter.
  //! @param[in] theT parameter value
  //! @return point on curve
  gp_Pnt Value(double theT) const { return myCurve->Value(theT); }

  //! Evaluates point and first derivative at parameter.
  //! @param[in] theT parameter value
  //! @param[out] thePt point on curve
  //! @param[out] theD1 first derivative
  void D1(double theT, gp_Pnt& thePt, gp_Vec& theD1) const { myCurve->D1(theT, thePt, theD1); }

  //! Evaluates point and first two derivatives at parameter.
  //! @param[in] theT parameter value
  //! @param[out] thePt point on curve
  //! @param[out] theD1 first derivative
  //! @param[out] theD2 second derivative
  void D2(double theT, gp_Pnt& thePt, gp_Vec& theD1, gp_Vec& theD2) const
  {
    myCurve->D2(theT, thePt, theD1, theD2);
  }

  //! Returns true if domain is bounded.
  bool IsBounded() const { return myDomain.IsFinite(); }

  //! Returns the parameter domain.
  const ExtremaCS::Domain1D& Domain() const { return myDomain; }

  //! Returns recommended number of samples for this curve.
  int NbSamples() const { return ExtremaCS::THE_DEFAULT_CURVE_SAMPLES; }

  //! Returns the underlying curve adaptor.
  const GeomAdaptor_Curve& Curve() const { return *myCurve; }

private:
  const GeomAdaptor_Curve* myCurve;  //!< Curve adaptor (non-owning)
  ExtremaCS::Domain1D      myDomain; //!< Parameter domain
};

#endif // _ExtremaCS_CurveAdapter_HeaderFile
