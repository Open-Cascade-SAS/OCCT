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

#ifndef _ExtremaCS_SurfaceAdapter_HeaderFile
#define _ExtremaCS_SurfaceAdapter_HeaderFile

#include <ExtremaCS.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

//! @brief Lightweight adapter for using GeomAdaptor_Surface with extrema templates.
//!
//! This class provides the interface required by ExtremaCS_GridEvaluator3D
//! and ExtremaCS_Numerical templates, forwarding calls directly to
//! the underlying GeomAdaptor_Surface.
//!
//! Replaces the previous separate wrapper classes (ExtremaCS_BezierSurface,
//! ExtremaCS_BSplineSurface, ExtremaCS_OffsetSurface, ExtremaCS_SurfaceOfRevolution,
//! ExtremaCS_SurfaceOfExtrusion, ExtremaCS_OtherSurface) with a single unified adapter.
//!
//! @note This class stores a non-owning pointer to the surface adaptor.
//!       The caller must ensure the adaptor outlives this adapter.
class ExtremaCS_SurfaceAdapter
{
public:
  //! Constructor with surface adaptor and domain.
  //! @param[in] theSurface surface adaptor (must outlive this object)
  //! @param[in] theDomain parameter domain
  ExtremaCS_SurfaceAdapter(const GeomAdaptor_Surface& theSurface,
                           const ExtremaCS::Domain2D& theDomain)
      : mySurface(&theSurface),
        myDomain(theDomain)
  {
  }

  //! Constructor with surface adaptor using its natural bounds.
  //! @param[in] theSurface surface adaptor (must outlive this object)
  explicit ExtremaCS_SurfaceAdapter(const GeomAdaptor_Surface& theSurface)
      : mySurface(&theSurface),
        myDomain(theSurface.FirstUParameter(),
                 theSurface.LastUParameter(),
                 theSurface.FirstVParameter(),
                 theSurface.LastVParameter())
  {
  }

  //! Evaluates point on surface at parameters.
  //! @param[in] theU U parameter value
  //! @param[in] theV V parameter value
  //! @return point on surface
  gp_Pnt Value(double theU, double theV) const { return mySurface->Value(theU, theV); }

  //! Evaluates point and first derivatives at parameters.
  //! @param[in] theU U parameter value
  //! @param[in] theV V parameter value
  //! @param[out] thePt point on surface
  //! @param[out] theD1U first derivative in U
  //! @param[out] theD1V first derivative in V
  void D1(double theU, double theV, gp_Pnt& thePt, gp_Vec& theD1U, gp_Vec& theD1V) const
  {
    mySurface->D1(theU, theV, thePt, theD1U, theD1V);
  }

  //! Evaluates point and first two derivatives at parameters.
  //! @param[in] theU U parameter value
  //! @param[in] theV V parameter value
  //! @param[out] thePt point on surface
  //! @param[out] theD1U first derivative in U
  //! @param[out] theD1V first derivative in V
  //! @param[out] theD2UU second derivative in U
  //! @param[out] theD2VV second derivative in V
  //! @param[out] theD2UV mixed second derivative
  void D2(double  theU,
          double  theV,
          gp_Pnt& thePt,
          gp_Vec& theD1U,
          gp_Vec& theD1V,
          gp_Vec& theD2UU,
          gp_Vec& theD2VV,
          gp_Vec& theD2UV) const
  {
    mySurface->D2(theU, theV, thePt, theD1U, theD1V, theD2UU, theD2VV, theD2UV);
  }

  //! Returns true if domain is bounded.
  bool IsBounded() const { return myDomain.IsFinite(); }

  //! Returns the parameter domain.
  const ExtremaCS::Domain2D& Domain() const { return myDomain; }

  //! Returns the underlying surface adaptor.
  const GeomAdaptor_Surface& Surface() const { return *mySurface; }

private:
  const GeomAdaptor_Surface* mySurface; //!< Surface adaptor (non-owning)
  ExtremaCS::Domain2D        myDomain;  //!< Parameter domain
};

#endif // _ExtremaCS_SurfaceAdapter_HeaderFile
