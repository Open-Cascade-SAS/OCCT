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

#ifndef _ExtremaCS_Cone_HeaderFile
#define _ExtremaCS_Cone_HeaderFile

#include <ElSLib.hxx>
#include <ExtremaCS.hxx>
#include <gp_Cone.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_DefineAlloc.hxx>

#include <optional>

//! @brief Cone surface evaluator wrapper for curve-surface extrema.
//!
//! Provides uniform interface for evaluating cone geometry,
//! compatible with ExtremaCS_Numerical template.
class ExtremaCS_Cone
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with cone geometry (default domain: U in [0, 2*PI], V unbounded).
  explicit ExtremaCS_Cone(const gp_Cone& theCone)
      : myCone(theCone),
        myDomain(ExtremaCS::Domain2D{0.0, ExtremaCS::THE_TWO_PI, -1.0e100, 1.0e100})
  {
  }

  //! Constructor with cone geometry and parameter domain.
  ExtremaCS_Cone(const gp_Cone& theCone, const ExtremaCS::Domain2D& theDomain)
      : myCone(theCone),
        myDomain(theDomain)
  {
  }

  //! Evaluates point on surface at parameters.
  gp_Pnt Value(double theU, double theV) const { return ElSLib::Value(theU, theV, myCone); }

  //! Evaluates point and first derivatives at parameters.
  void D1(double theU, double theV, gp_Pnt& thePt, gp_Vec& theD1U, gp_Vec& theD1V) const
  {
    ElSLib::D1(theU, theV, myCone, thePt, theD1U, theD1V);
  }

  //! Evaluates point and first two derivatives at parameters.
  void D2(double theU, double theV, gp_Pnt& thePt,
          gp_Vec& theD1U, gp_Vec& theD1V,
          gp_Vec& theD2UU, gp_Vec& theD2VV, gp_Vec& theD2UV) const
  {
    ElSLib::D2(theU, theV, myCone, thePt, theD1U, theD1V, theD2UU, theD2VV, theD2UV);
  }

  //! Returns true if domain is bounded.
  bool IsBounded() const { return myDomain.IsFinite(); }

  //! Returns the domain.
  const ExtremaCS::Domain2D& Domain() const { return myDomain; }

  //! Returns the cone geometry.
  const gp_Cone& Cone() const { return myCone; }

private:
  gp_Cone             myCone;   //!< Cone geometry
  ExtremaCS::Domain2D myDomain; //!< Parameter domain
};

#endif // _ExtremaCS_Cone_HeaderFile
