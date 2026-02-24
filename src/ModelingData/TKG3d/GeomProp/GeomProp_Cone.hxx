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

#ifndef _GeomProp_Cone_HeaderFile
#define _GeomProp_Cone_HeaderFile

#include <GeomAdaptor_Surface.hxx>
#include <GeomProp.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief Local differential properties for a conical surface.
//!
//! Uses analytical formulas where possible; the curvature
//! varies along the meridian (V direction).
//! Min principal curvature = 0 (along the ruling direction).
//! Max principal curvature = cos(alpha) / R(V), where alpha is the half-angle
//! and R(V) is the radius at parameter V.
//!
//! @warning The caller must ensure that the adaptor pointer remains valid
//! for the entire lifetime of this object.
class GeomProp_Cone
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with adaptor pointer (non-owning).
  //! @param theAdaptor the surface adaptor (must not be null)
  GeomProp_Cone(const GeomAdaptor_Surface* theAdaptor)
      : myAdaptor(theAdaptor)
  {
  }

  //! Non-copyable and non-movable.
  GeomProp_Cone(const GeomProp_Cone&)            = delete;
  GeomProp_Cone& operator=(const GeomProp_Cone&) = delete;
  GeomProp_Cone(GeomProp_Cone&&)                 = delete;
  GeomProp_Cone& operator=(GeomProp_Cone&&)      = delete;

  //! Returns the adaptor pointer.
  const GeomAdaptor_Surface* Adaptor() const { return myAdaptor; }

  //! Compute surface normal at given parameter.
  //! TODO: At the apex (V = -R/sin(alpha)), D1U degenerates and Normal returns IsDefined=false.
  //!   Could use analytical normal for this special case.
  Standard_EXPORT GeomProp::SurfaceNormalResult Normal(double theU,
                                                       double theV,
                                                       double theTol) const;

  //! Compute principal curvatures at given parameter.
  Standard_EXPORT GeomProp::SurfaceCurvatureResult Curvatures(double theU,
                                                              double theV,
                                                              double theTol) const;

  //! Compute mean and Gaussian curvatures at given parameter.
  Standard_EXPORT GeomProp::MeanGaussianResult MeanGaussian(double theU,
                                                            double theV,
                                                            double theTol) const;

private:
  const GeomAdaptor_Surface* myAdaptor;
};

#endif // _GeomProp_Cone_HeaderFile
