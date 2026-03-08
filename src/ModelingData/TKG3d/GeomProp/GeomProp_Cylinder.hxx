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

#ifndef _GeomProp_Cylinder_HeaderFile
#define _GeomProp_Cylinder_HeaderFile

#include <GeomAdaptor_Surface.hxx>
#include <GeomProp.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief Local differential properties for a cylindrical surface.
//!
//! Analytical implementation with constant principal curvatures:
//! - Min curvature = 0 (along the axis direction)
//! - Max curvature = 1/R (along the circular cross-section)
//!
//! @warning The caller must ensure that the adaptor pointer remains valid
//! for the entire lifetime of this object.
class GeomProp_Cylinder
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with adaptor pointer (non-owning).
  //! @param theAdaptor the surface adaptor (must not be null)
  GeomProp_Cylinder(const GeomAdaptor_Surface*  theAdaptor,
                    GeomProp::SurfaceDerivOrder theOrder = GeomProp::SurfaceDerivOrder::Undefined)
      : myAdaptor(theAdaptor)
  {
    (void)theOrder;
  }

  //! Non-copyable and non-movable.
  GeomProp_Cylinder(const GeomProp_Cylinder&)            = delete;
  GeomProp_Cylinder& operator=(const GeomProp_Cylinder&) = delete;
  GeomProp_Cylinder(GeomProp_Cylinder&&)                 = delete;
  GeomProp_Cylinder& operator=(GeomProp_Cylinder&&)      = delete;

  //! Returns the adaptor pointer.
  const GeomAdaptor_Surface* Adaptor() const { return myAdaptor; }

  //! Compute surface normal at given (U, V) parameter.
  //! For a cylinder, the normal is radially outward from the axis.
  //! TODO: Could use analytical normal (radial direction from axis) for degenerate D1 cases,
  //!   though in practice cylinder D1 never degenerates.
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

#endif // _GeomProp_Cylinder_HeaderFile
