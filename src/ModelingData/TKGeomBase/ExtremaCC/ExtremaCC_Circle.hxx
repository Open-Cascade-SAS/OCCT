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

#ifndef _ExtremaCC_Circle_HeaderFile
#define _ExtremaCC_Circle_HeaderFile

#include <ElCLib.hxx>
#include <ExtremaCC.hxx>
#include <gp_Circ.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_DefineAlloc.hxx>

#include <cmath>
#include <optional>

//! @brief Circle curve evaluator wrapper for curve-curve extrema.
//!
//! Provides uniform interface for evaluating circle geometry,
//! compatible with ExtremaCC_Numerical template.
class ExtremaCC_Circle
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with circle geometry (full circle).
  explicit ExtremaCC_Circle(const gp_Circ& theCircle)
      : myCircle(theCircle),
        myDomain(std::nullopt)
  {
  }

  //! Constructor with circle geometry and parameter domain.
  ExtremaCC_Circle(const gp_Circ& theCircle, const ExtremaCC::Domain1D& theDomain)
      : myCircle(theCircle),
        myDomain(theDomain)
  {
  }

  //! Evaluates point on circle at parameter (radians).
  gp_Pnt Value(double theU) const { return ElCLib::Value(theU, myCircle); }

  //! Evaluates point and first derivative at parameter.
  void D1(double theU, gp_Pnt& thePt, gp_Vec& theD1) const
  {
    ElCLib::D1(theU, myCircle, thePt, theD1);
  }

  //! Evaluates point and first two derivatives at parameter.
  void D2(double theU, gp_Pnt& thePt, gp_Vec& theD1, gp_Vec& theD2) const
  {
    ElCLib::D2(theU, myCircle, thePt, theD1, theD2);
  }

  //! Returns true if domain is bounded (arc, not full circle).
  bool IsBounded() const
  {
    if (!myDomain.has_value())
      return false;
    return !myDomain->IsFullPeriod(2.0 * M_PI);
  }

  //! Returns the domain.
  const ExtremaCC::Domain1D& Domain() const
  {
    static const ExtremaCC::Domain1D aFullCircle{0.0, 2.0 * M_PI};
    return myDomain.has_value() ? *myDomain : aFullCircle;
  }

  //! Returns the circle geometry.
  const gp_Circ& Circle() const { return myCircle; }

private:
  gp_Circ                            myCircle; //!< Circle geometry
  std::optional<ExtremaCC::Domain1D> myDomain; //!< Parameter domain
};

#endif // _ExtremaCC_Circle_HeaderFile
