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

#ifndef _ExtremaCS_OffsetSurface_HeaderFile
#define _ExtremaCS_OffsetSurface_HeaderFile

#include <ExtremaCS.hxx>
#include <Geom_OffsetSurface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief Offset surface evaluator wrapper for curve-surface extrema.
//!
//! Provides uniform interface for evaluating offset surface geometry,
//! compatible with ExtremaCS_Numerical template.
class ExtremaCS_OffsetSurface
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with offset surface (uses natural domain).
  explicit ExtremaCS_OffsetSurface(const Handle(Geom_OffsetSurface)& theSurface)
      : mySurface(theSurface)
  {
    theSurface->Bounds(myDomain.UMin, myDomain.UMax, myDomain.VMin, myDomain.VMax);
  }

  //! Constructor with offset surface and parameter domain.
  ExtremaCS_OffsetSurface(const Handle(Geom_OffsetSurface)& theSurface,
                          const ExtremaCS::Domain2D&       theDomain)
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
  const Handle(Geom_OffsetSurface)& Surface() const { return mySurface; }

private:
  Handle(Geom_OffsetSurface) mySurface; //!< Offset surface
  ExtremaCS::Domain2D        myDomain;  //!< Parameter domain
};

#endif // _ExtremaCS_OffsetSurface_HeaderFile
