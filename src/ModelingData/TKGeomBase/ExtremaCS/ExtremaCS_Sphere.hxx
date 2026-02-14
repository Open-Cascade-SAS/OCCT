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

#ifndef _ExtremaCS_Sphere_HeaderFile
#define _ExtremaCS_Sphere_HeaderFile

#include <ElSLib.hxx>
#include <ExtremaCS.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <gp_Vec.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief Sphere surface evaluator wrapper for curve-surface extrema.
//!
//! Provides uniform interface for evaluating sphere geometry,
//! compatible with ExtremaCS_Numerical template.
class ExtremaCS_Sphere
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with sphere geometry (default domain: U in [0, 2*PI], V in [-PI/2, PI/2]).
  explicit ExtremaCS_Sphere(const gp_Sphere& theSphere)
      : mySphere(theSphere),
        myDomain(ExtremaCS::Domain2D{0.0,
                                     ExtremaCS::THE_TWO_PI,
                                     -ExtremaCS::THE_HALF_PI,
                                     ExtremaCS::THE_HALF_PI})
  {
  }

  //! Constructor with sphere geometry and parameter domain.
  ExtremaCS_Sphere(const gp_Sphere& theSphere, const ExtremaCS::Domain2D& theDomain)
      : mySphere(theSphere),
        myDomain(theDomain)
  {
  }

  //! Evaluates point on surface at parameters.
  gp_Pnt Value(double theU, double theV) const { return ElSLib::Value(theU, theV, mySphere); }

  //! Evaluates point and first derivatives at parameters.
  void D1(double theU, double theV, gp_Pnt& thePt, gp_Vec& theD1U, gp_Vec& theD1V) const
  {
    ElSLib::D1(theU, theV, mySphere, thePt, theD1U, theD1V);
  }

  //! Evaluates point and first two derivatives at parameters.
  void D2(double  theU,
          double  theV,
          gp_Pnt& thePt,
          gp_Vec& theD1U,
          gp_Vec& theD1V,
          gp_Vec& theD2UU,
          gp_Vec& theD2VV,
          gp_Vec& theD2UV) const
  {
    ElSLib::D2(theU, theV, mySphere, thePt, theD1U, theD1V, theD2UU, theD2VV, theD2UV);
  }

  //! Returns true if domain is bounded.
  bool IsBounded() const { return myDomain.IsFinite(); }

  //! Returns the domain.
  const ExtremaCS::Domain2D& Domain() const { return myDomain; }

  //! Returns the sphere geometry.
  const gp_Sphere& Sphere() const { return mySphere; }

private:
  gp_Sphere           mySphere; //!< Sphere geometry
  ExtremaCS::Domain2D myDomain; //!< Parameter domain
};

#endif // _ExtremaCS_Sphere_HeaderFile
