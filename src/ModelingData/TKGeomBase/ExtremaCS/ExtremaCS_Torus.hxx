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

#ifndef _ExtremaCS_Torus_HeaderFile
#define _ExtremaCS_Torus_HeaderFile

#include <ElSLib.hxx>
#include <ExtremaCS.hxx>
#include <gp_Pnt.hxx>
#include <gp_Torus.hxx>
#include <gp_Vec.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief Torus surface evaluator wrapper for curve-surface extrema.
//!
//! Provides uniform interface for evaluating torus geometry,
//! compatible with ExtremaCS_Numerical template.
class ExtremaCS_Torus
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with torus geometry (default domain: U,V in [0, 2*PI]).
  explicit ExtremaCS_Torus(const gp_Torus& theTorus)
      : myTorus(theTorus),
        myDomain(ExtremaCS::Domain2D{0.0, ExtremaCS::THE_TWO_PI, 0.0, ExtremaCS::THE_TWO_PI})
  {
  }

  //! Constructor with torus geometry and parameter domain.
  ExtremaCS_Torus(const gp_Torus& theTorus, const ExtremaCS::Domain2D& theDomain)
      : myTorus(theTorus),
        myDomain(theDomain)
  {
  }

  //! Evaluates point on surface at parameters.
  gp_Pnt Value(double theU, double theV) const { return ElSLib::Value(theU, theV, myTorus); }

  //! Evaluates point and first derivatives at parameters.
  void D1(double theU, double theV, gp_Pnt& thePt, gp_Vec& theD1U, gp_Vec& theD1V) const
  {
    ElSLib::D1(theU, theV, myTorus, thePt, theD1U, theD1V);
  }

  //! Evaluates point and first two derivatives at parameters.
  void D2(double theU, double theV, gp_Pnt& thePt,
          gp_Vec& theD1U, gp_Vec& theD1V,
          gp_Vec& theD2UU, gp_Vec& theD2VV, gp_Vec& theD2UV) const
  {
    ElSLib::D2(theU, theV, myTorus, thePt, theD1U, theD1V, theD2UU, theD2VV, theD2UV);
  }

  //! Returns true if domain is bounded.
  bool IsBounded() const { return myDomain.IsFinite(); }

  //! Returns the domain.
  const ExtremaCS::Domain2D& Domain() const { return myDomain; }

  //! Returns the torus geometry.
  const gp_Torus& Torus() const { return myTorus; }

private:
  gp_Torus            myTorus;  //!< Torus geometry
  ExtremaCS::Domain2D myDomain; //!< Parameter domain
};

#endif // _ExtremaCS_Torus_HeaderFile
