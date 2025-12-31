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

#ifndef _ExtremaCS_BSplineSurface_HeaderFile
#define _ExtremaCS_BSplineSurface_HeaderFile

#include <ExtremaCS.hxx>
#include <Geom_BSplineSurface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief BSpline surface evaluator wrapper for curve-surface extrema.
//!
//! Provides uniform interface for evaluating BSpline surface geometry,
//! compatible with ExtremaCS_Numerical template.
class ExtremaCS_BSplineSurface
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with BSpline surface (uses natural domain).
  explicit ExtremaCS_BSplineSurface(const Handle(Geom_BSplineSurface)& theSurface)
      : mySurface(theSurface)
  {
    myDomain.UMin = theSurface->UKnot(1);
    myDomain.UMax = theSurface->UKnot(theSurface->NbUKnots());
    myDomain.VMin = theSurface->VKnot(1);
    myDomain.VMax = theSurface->VKnot(theSurface->NbVKnots());
  }

  //! Constructor with BSpline surface and parameter domain.
  ExtremaCS_BSplineSurface(const Handle(Geom_BSplineSurface)& theSurface,
                           const ExtremaCS::Domain2D&        theDomain)
      : mySurface(theSurface),
        myDomain(theDomain)
  {
  }

  //! Evaluates point on surface at parameters.
  gp_Pnt Value(double theU, double theV) const { return mySurface->Value(theU, theV); }

  //! Evaluates point and first derivatives at parameters.
  void D1(double theU, double theV, gp_Pnt& thePt, gp_Vec& theD1U, gp_Vec& theD1V) const
  {
    mySurface->D1(theU, theV, thePt, theD1U, theD1V);
  }

  //! Evaluates point and first two derivatives at parameters.
  void D2(double theU, double theV, gp_Pnt& thePt,
          gp_Vec& theD1U, gp_Vec& theD1V,
          gp_Vec& theD2UU, gp_Vec& theD2VV, gp_Vec& theD2UV) const
  {
    mySurface->D2(theU, theV, thePt, theD1U, theD1V, theD2UU, theD2VV, theD2UV);
  }

  //! Returns true if domain is bounded.
  bool IsBounded() const { return myDomain.IsFinite(); }

  //! Returns the domain.
  const ExtremaCS::Domain2D& Domain() const { return myDomain; }

  //! Returns the surface handle.
  const Handle(Geom_BSplineSurface)& Surface() const { return mySurface; }

private:
  Handle(Geom_BSplineSurface) mySurface; //!< BSpline surface
  ExtremaCS::Domain2D         myDomain;  //!< Parameter domain
};

#endif // _ExtremaCS_BSplineSurface_HeaderFile
