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

#ifndef _ExtremaCC_Parabola_HeaderFile
#define _ExtremaCC_Parabola_HeaderFile

#include <ElCLib.hxx>
#include <ExtremaCC.hxx>
#include <gp_Parab.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_DefineAlloc.hxx>

#include <optional>

//! @brief Parabola curve evaluator wrapper for curve-curve extrema.
//!
//! Provides uniform interface for evaluating parabola geometry,
//! compatible with ExtremaCC_Numerical template.
class ExtremaCC_Parabola
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with parabola geometry (unbounded).
  explicit ExtremaCC_Parabola(const gp_Parab& theParabola)
      : myParabola(theParabola),
        myDomain(std::nullopt)
  {
  }

  //! Constructor with parabola geometry and parameter domain.
  ExtremaCC_Parabola(const gp_Parab& theParabola, const ExtremaCC::Domain1D& theDomain)
      : myParabola(theParabola),
        myDomain(theDomain)
  {
  }

  //! Evaluates point on parabola at parameter.
  gp_Pnt Value(double theU) const { return ElCLib::Value(theU, myParabola); }

  //! Evaluates point and first derivative at parameter.
  void D1(double theU, gp_Pnt& thePt, gp_Vec& theD1) const
  {
    ElCLib::D1(theU, myParabola, thePt, theD1);
  }

  //! Evaluates point and first two derivatives at parameter.
  void D2(double theU, gp_Pnt& thePt, gp_Vec& theD1, gp_Vec& theD2) const
  {
    ElCLib::D2(theU, myParabola, thePt, theD1, theD2);
  }

  //! Returns true if domain is bounded.
  bool IsBounded() const { return myDomain.has_value() && myDomain->IsFinite(); }

  //! Returns the domain.
  const ExtremaCC::Domain1D& Domain() const
  {
    static const ExtremaCC::Domain1D anUnbounded{-1.0e100, 1.0e100};
    return myDomain.has_value() ? *myDomain : anUnbounded;
  }

  //! Returns the parabola geometry.
  const gp_Parab& Parabola() const { return myParabola; }

private:
  gp_Parab                           myParabola; //!< Parabola geometry
  std::optional<ExtremaCC::Domain1D> myDomain;   //!< Parameter domain
};

#endif // _ExtremaCC_Parabola_HeaderFile
