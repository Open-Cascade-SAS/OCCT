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

#ifndef _ExtremaCC_CurveAdapter_HeaderFile
#define _ExtremaCC_CurveAdapter_HeaderFile

#include <ExtremaCC.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

//! @brief Lightweight adapter for using GeomAdaptor_Curve with extrema templates.
//!
//! This class provides the interface required by ExtremaCC_GridEvaluator2D
//! and ExtremaCC_DistanceFunction templates, forwarding calls directly to
//! the underlying GeomAdaptor_Curve.
//!
//! Replaces the previous separate wrapper classes (ExtremaCC_BezierCurve,
//! ExtremaCC_BSplineCurve, ExtremaCC_OffsetCurve, ExtremaCC_OtherCurve)
//! with a single unified adapter.
//!
//! @note This class stores a non-owning pointer to the curve adaptor.
//!       The caller must ensure the adaptor outlives this adapter.
class ExtremaCC_CurveAdapter
{
public:
  //! Constructor with curve adaptor and domain.
  //! @param[in] theCurve curve adaptor (must outlive this object)
  //! @param[in] theDomain parameter domain
  ExtremaCC_CurveAdapter(const GeomAdaptor_Curve& theCurve, const ExtremaCC::Domain1D& theDomain)
      : myCurve(&theCurve),
        myDomain(theDomain)
  {
  }

  //! Constructor with curve adaptor using its natural bounds.
  //! @param[in] theCurve curve adaptor (must outlive this object)
  explicit ExtremaCC_CurveAdapter(const GeomAdaptor_Curve& theCurve)
      : myCurve(&theCurve),
        myDomain(theCurve.FirstParameter(), theCurve.LastParameter())
  {
  }

  //! Evaluates point on curve at parameter.
  //! @param[in] theU parameter value
  //! @return point on curve
  gp_Pnt Value(double theU) const { return myCurve->Value(theU); }

  //! Evaluates point and first derivative at parameter.
  //! @param[in] theU parameter value
  //! @param[out] thePt point on curve
  //! @param[out] theD1 first derivative
  void D1(double theU, gp_Pnt& thePt, gp_Vec& theD1) const { myCurve->D1(theU, thePt, theD1); }

  //! Evaluates point and first two derivatives at parameter.
  //! @param[in] theU parameter value
  //! @param[out] thePt point on curve
  //! @param[out] theD1 first derivative
  //! @param[out] theD2 second derivative
  void D2(double theU, gp_Pnt& thePt, gp_Vec& theD1, gp_Vec& theD2) const
  {
    myCurve->D2(theU, thePt, theD1, theD2);
  }

  //! Returns true if domain is bounded.
  bool IsBounded() const { return myDomain.IsFinite(); }

  //! Returns the parameter domain.
  const ExtremaCC::Domain1D& Domain() const { return myDomain; }

  //! Returns the underlying curve adaptor.
  const GeomAdaptor_Curve& Curve() const { return *myCurve; }

private:
  const GeomAdaptor_Curve* myCurve;  //!< Curve adaptor (non-owning)
  ExtremaCC::Domain1D      myDomain; //!< Parameter domain
};

#endif // _ExtremaCC_CurveAdapter_HeaderFile
