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

#ifndef _ExtremaCS_Ellipse_HeaderFile
#define _ExtremaCS_Ellipse_HeaderFile

#include <ElCLib.hxx>
#include <ExtremaCS.hxx>
#include <gp_Elips.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_DefineAlloc.hxx>

#include <cmath>
#include <optional>

//! @brief Ellipse curve evaluator wrapper for curve-surface extrema.
//!
//! Provides uniform interface for evaluating ellipse geometry,
//! compatible with ExtremaCS_Numerical template.
class ExtremaCS_Ellipse
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with ellipse geometry (full ellipse domain [0, 2*PI]).
  explicit ExtremaCS_Ellipse(const gp_Elips& theEllipse)
      : myEllipse(theEllipse),
        myDomain(ExtremaCS::Domain1D{0.0, ExtremaCS::THE_TWO_PI})
  {
  }

  //! Constructor with ellipse geometry and parameter domain.
  ExtremaCS_Ellipse(const gp_Elips& theEllipse, const ExtremaCS::Domain1D& theDomain)
      : myEllipse(theEllipse),
        myDomain(theDomain)
  {
  }

  //! Evaluates point on ellipse at parameter.
  gp_Pnt Value(double theT) const { return ElCLib::Value(theT, myEllipse); }

  //! Evaluates point and first derivative at parameter.
  void D1(double theT, gp_Pnt& thePt, gp_Vec& theD1) const
  {
    ElCLib::D1(theT, myEllipse, thePt, theD1);
  }

  //! Evaluates point and first two derivatives at parameter.
  void D2(double theT, gp_Pnt& thePt, gp_Vec& theD1, gp_Vec& theD2) const
  {
    ElCLib::D2(theT, myEllipse, thePt, theD1, theD2);
  }

  //! Returns true if domain is bounded.
  bool IsBounded() const { return myDomain.IsFinite(); }

  //! Returns the domain.
  const ExtremaCS::Domain1D& Domain() const { return myDomain; }

  //! Returns the ellipse geometry.
  const gp_Elips& Ellipse() const { return myEllipse; }

private:
  gp_Elips            myEllipse; //!< Ellipse geometry
  ExtremaCS::Domain1D myDomain;  //!< Parameter domain
};

#endif // _ExtremaCS_Ellipse_HeaderFile
