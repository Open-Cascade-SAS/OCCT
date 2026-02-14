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

#ifndef _ExtremaCS_Plane_HeaderFile
#define _ExtremaCS_Plane_HeaderFile

#include <ElSLib.hxx>
#include <ExtremaCS.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_DefineAlloc.hxx>

#include <optional>

//! @brief Plane surface evaluator wrapper for curve-surface extrema.
//!
//! Provides uniform interface for evaluating plane geometry,
//! compatible with ExtremaCS_Numerical template.
class ExtremaCS_Plane
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with plane geometry (unbounded).
  explicit ExtremaCS_Plane(const gp_Pln& thePlane)
      : myPlane(thePlane),
        myDomain(std::nullopt)
  {
  }

  //! Constructor with plane geometry and parameter domain.
  ExtremaCS_Plane(const gp_Pln& thePlane, const ExtremaCS::Domain2D& theDomain)
      : myPlane(thePlane),
        myDomain(theDomain)
  {
  }

  //! Evaluates point on surface at parameters.
  gp_Pnt Value(double theU, double theV) const { return ElSLib::Value(theU, theV, myPlane); }

  //! Evaluates point and first derivatives at parameters.
  void D1(double theU, double theV, gp_Pnt& thePt, gp_Vec& theD1U, gp_Vec& theD1V) const
  {
    ElSLib::D1(theU, theV, myPlane, thePt, theD1U, theD1V);
  }

  //! Evaluates point and first two derivatives at parameters.
  //! For a plane, all second derivatives are zero (zero curvature).
  void D2(double theU, double theV, gp_Pnt& thePt,
          gp_Vec& theD1U, gp_Vec& theD1V,
          gp_Vec& theD2UU, gp_Vec& theD2VV, gp_Vec& theD2UV) const
  {
    ElSLib::D1(theU, theV, myPlane, thePt, theD1U, theD1V);
    // Plane has zero curvature - all second derivatives are zero
    theD2UU = gp_Vec(0.0, 0.0, 0.0);
    theD2VV = gp_Vec(0.0, 0.0, 0.0);
    theD2UV = gp_Vec(0.0, 0.0, 0.0);
  }

  //! Returns true if domain is bounded.
  bool IsBounded() const { return myDomain.has_value() && myDomain->IsFinite(); }

  //! Returns the domain.
  const ExtremaCS::Domain2D& Domain() const
  {
    static const ExtremaCS::Domain2D anUnbounded{-1.0e100, 1.0e100, -1.0e100, 1.0e100};
    return myDomain.has_value() ? *myDomain : anUnbounded;
  }

  //! Returns the plane geometry.
  const gp_Pln& Plane() const { return myPlane; }

private:
  gp_Pln                             myPlane;  //!< Plane geometry
  std::optional<ExtremaCS::Domain2D> myDomain; //!< Parameter domain
};

#endif // _ExtremaCS_Plane_HeaderFile
