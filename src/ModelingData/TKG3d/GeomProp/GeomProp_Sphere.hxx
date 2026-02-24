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

#ifndef _GeomProp_Sphere_HeaderFile
#define _GeomProp_Sphere_HeaderFile

#include <GeomAdaptor_Surface.hxx>
#include <GeomProp.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief Local differential properties for a spherical surface.
//!
//! Analytical implementation: constant curvature 1/R, umbilic everywhere.
//! Both principal curvatures equal 1/R.
//!
//! @warning The caller must ensure that the adaptor pointer remains valid
//! for the entire lifetime of this object.
class GeomProp_Sphere
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with adaptor pointer (non-owning).
  //! @param theAdaptor the surface adaptor (must not be null)
  GeomProp_Sphere(const GeomAdaptor_Surface* theAdaptor)
      : myAdaptor(theAdaptor)
  {
  }

  //! Non-copyable and non-movable.
  GeomProp_Sphere(const GeomProp_Sphere&)            = delete;
  GeomProp_Sphere& operator=(const GeomProp_Sphere&) = delete;
  GeomProp_Sphere(GeomProp_Sphere&&)                 = delete;
  GeomProp_Sphere& operator=(GeomProp_Sphere&&)      = delete;

  //! Returns the adaptor pointer.
  const GeomAdaptor_Surface* Adaptor() const { return myAdaptor; }

  //! Compute surface normal at given (U, V) parameter.
  //! For a sphere, the normal is radially outward from the center.
  //! TODO: At poles (V = +/-PI/2), D1U degenerates and Normal returns IsDefined=false.
  //!   Could use analytical normal (radial direction from center) for these special cases.
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

  //! Compute principal curvatures using fundamental forms for correct sign convention.
  GeomProp::SurfaceCurvatureResult Curvatures(double theU, double theV, double theTol) const
  {
    if (myAdaptor == nullptr)
    {
      return {};
    }
    gp_Pnt aPnt;
    gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV;
    myAdaptor->D2(theU, theV, aPnt, aD1U, aD1V, aD2U, aD2V, aD2UV);
    return GeomProp::ComputeSurfaceCurvatures(aD1U, aD1V, aD2U, aD2V, aD2UV, theTol);
  }

  //! Compute mean and Gaussian curvatures using fundamental forms for correct sign convention.
  GeomProp::MeanGaussianResult MeanGaussian(double theU, double theV, double theTol) const
  {
    if (myAdaptor == nullptr)
    {
      return {};
    }
    gp_Pnt aPnt;
    gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV;
    myAdaptor->D2(theU, theV, aPnt, aD1U, aD1V, aD2U, aD2V, aD2UV);
    return GeomProp::ComputeMeanGaussian(aD1U, aD1V, aD2U, aD2V, aD2UV, theTol);
  }

private:
  const GeomAdaptor_Surface* myAdaptor;
};

#endif // _GeomProp_Sphere_HeaderFile
