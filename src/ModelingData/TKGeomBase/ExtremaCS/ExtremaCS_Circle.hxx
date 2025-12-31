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

#ifndef _ExtremaCS_Circle_HeaderFile
#define _ExtremaCS_Circle_HeaderFile

#include <ElCLib.hxx>
#include <ExtremaCS.hxx>
#include <gp_Circ.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_DefineAlloc.hxx>

#include <cmath>
#include <optional>

//! @brief Circle curve evaluator wrapper for curve-surface extrema.
//!
//! Provides uniform interface for evaluating circle geometry,
//! compatible with ExtremaCS_Numerical template.
class ExtremaCS_Circle
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with circle geometry (full circle domain [0, 2*PI]).
  explicit ExtremaCS_Circle(const gp_Circ& theCircle)
      : myCircle(theCircle),
        myDomain(ExtremaCS::Domain1D{0.0, ExtremaCS::THE_TWO_PI})
  {
  }

  //! Constructor with circle geometry and parameter domain.
  ExtremaCS_Circle(const gp_Circ& theCircle, const ExtremaCS::Domain1D& theDomain)
      : myCircle(theCircle),
        myDomain(theDomain)
  {
  }

  //! Evaluates point on circle at parameter.
  gp_Pnt Value(double theT) const { return ElCLib::Value(theT, myCircle); }

  //! Evaluates point and first derivative at parameter.
  void D1(double theT, gp_Pnt& thePt, gp_Vec& theD1) const
  {
    ElCLib::D1(theT, myCircle, thePt, theD1);
  }

  //! Evaluates point and first two derivatives at parameter.
  void D2(double theT, gp_Pnt& thePt, gp_Vec& theD1, gp_Vec& theD2) const
  {
    ElCLib::D2(theT, myCircle, thePt, theD1, theD2);
  }

  //! Returns true if domain is bounded.
  bool IsBounded() const { return myDomain.IsFinite(); }

  //! Returns the domain.
  const ExtremaCS::Domain1D& Domain() const { return myDomain; }

  //! Returns the circle geometry.
  const gp_Circ& Circle() const { return myCircle; }

private:
  gp_Circ             myCircle; //!< Circle geometry
  ExtremaCS::Domain1D myDomain; //!< Parameter domain
};

#endif // _ExtremaCS_Circle_HeaderFile
