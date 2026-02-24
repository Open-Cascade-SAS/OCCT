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

#ifndef _GeomProp_Plane_HeaderFile
#define _GeomProp_Plane_HeaderFile

#include <GeomAdaptor_Surface.hxx>
#include <GeomProp.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief Local differential properties for a plane surface.
//!
//! Trivial implementation: constant normal, zero curvatures everywhere.
//! All properties are computed analytically without numerical evaluation.
//!
//! @warning The caller must ensure that the adaptor pointer remains valid
//! for the entire lifetime of this object.
class GeomProp_Plane
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with adaptor pointer (non-owning).
  //! @param theAdaptor the surface adaptor (must not be null)
  GeomProp_Plane(const GeomAdaptor_Surface* theAdaptor)
      : myAdaptor(theAdaptor)
  {
  }

  //! Non-copyable and non-movable.
  GeomProp_Plane(const GeomProp_Plane&)            = delete;
  GeomProp_Plane& operator=(const GeomProp_Plane&) = delete;
  GeomProp_Plane(GeomProp_Plane&&)                 = delete;
  GeomProp_Plane& operator=(GeomProp_Plane&&)      = delete;

  //! Returns the adaptor pointer.
  const GeomAdaptor_Surface* Adaptor() const { return myAdaptor; }

  //! Compute surface normal. Constant for a plane.
  //! Uses D1U x D1V cross product to ensure correct sign for flipped planes.
  GeomProp::SurfaceNormalResult Normal(double theU, double theV, double theTol) const
  {
    if (myAdaptor == nullptr)
    {
      return {{}, false};
    }
    gp_Pnt aPnt;
    gp_Vec aD1U, aD1V;
    myAdaptor->D1(theU, theV, aPnt, aD1U, aD1V);
    return GeomProp::ComputeSurfaceNormal(aD1U, aD1V, theTol);
  }

  //! Compute principal curvatures. Both are zero for a plane.
  GeomProp::SurfaceCurvatureResult Curvatures(double /*theU*/,
                                              double /*theV*/,
                                              double /*theTol*/) const
  {
    if (myAdaptor == nullptr)
    {
      return {};
    }
    GeomProp::SurfaceCurvatureResult aResult;
    aResult.MinCurvature = 0.0;
    aResult.MaxCurvature = 0.0;
    aResult.MinDirection = myAdaptor->Plane().Position().XDirection();
    aResult.MaxDirection = myAdaptor->Plane().Position().YDirection();
    aResult.IsDefined    = true;
    aResult.IsUmbilic    = true;
    return aResult;
  }

  //! Compute mean and Gaussian curvatures. Both are zero for a plane.
  GeomProp::MeanGaussianResult MeanGaussian(double /*theU*/,
                                            double /*theV*/,
                                            double /*theTol*/) const
  {
    if (myAdaptor == nullptr)
    {
      return {};
    }
    return {0.0, 0.0, true};
  }

private:
  const GeomAdaptor_Surface* myAdaptor;
};

#endif // _GeomProp_Plane_HeaderFile
