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

#ifndef _ExtremaCS_Hyperbola_HeaderFile
#define _ExtremaCS_Hyperbola_HeaderFile

#include <ElCLib.hxx>
#include <ExtremaCS.hxx>
#include <gp_Hypr.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_DefineAlloc.hxx>

#include <cmath>
#include <optional>

//! @brief Hyperbola curve evaluator wrapper for curve-surface extrema.
//!
//! Provides uniform interface for evaluating hyperbola geometry,
//! compatible with ExtremaCS_Numerical template.
class ExtremaCS_Hyperbola
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with hyperbola geometry (unbounded).
  explicit ExtremaCS_Hyperbola(const gp_Hypr& theHyperbola)
      : myHyperbola(theHyperbola),
        myDomain(std::nullopt)
  {
  }

  //! Constructor with hyperbola geometry and parameter domain.
  ExtremaCS_Hyperbola(const gp_Hypr& theHyperbola, const ExtremaCS::Domain1D& theDomain)
      : myHyperbola(theHyperbola),
        myDomain(theDomain)
  {
  }

  //! Evaluates point on hyperbola at parameter.
  gp_Pnt Value(double theT) const { return ElCLib::Value(theT, myHyperbola); }

  //! Evaluates point and first derivative at parameter.
  void D1(double theT, gp_Pnt& thePt, gp_Vec& theD1) const
  {
    ElCLib::D1(theT, myHyperbola, thePt, theD1);
  }

  //! Evaluates point and first two derivatives at parameter.
  void D2(double theT, gp_Pnt& thePt, gp_Vec& theD1, gp_Vec& theD2) const
  {
    ElCLib::D2(theT, myHyperbola, thePt, theD1, theD2);
  }

  //! Returns true if domain is bounded.
  bool IsBounded() const { return myDomain.has_value() && myDomain->IsFinite(); }

  //! Returns the domain.
  const ExtremaCS::Domain1D& Domain() const
  {
    static const ExtremaCS::Domain1D anUnbounded{-1.0e100, 1.0e100};
    return myDomain.has_value() ? *myDomain : anUnbounded;
  }

  //! Returns the hyperbola geometry.
  const gp_Hypr& Hyperbola() const { return myHyperbola; }

private:
  gp_Hypr                            myHyperbola; //!< Hyperbola geometry
  std::optional<ExtremaCS::Domain1D> myDomain;    //!< Parameter domain
};

#endif // _ExtremaCS_Hyperbola_HeaderFile
