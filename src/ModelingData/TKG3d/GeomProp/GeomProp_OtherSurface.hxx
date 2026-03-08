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

#ifndef _GeomProp_OtherSurface_HeaderFile
#define _GeomProp_OtherSurface_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomProp.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief Fallback local differential properties for any surface type.
//!
//! Uses adaptor D1/D2 methods for property computation.
//!
//! @warning The caller must ensure that the adaptor pointer remains valid
//! for the entire lifetime of this object.
class GeomProp_OtherSurface
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with adaptor pointer (non-owning).
  //! @param theAdaptor the surface adaptor (must not be null)
  GeomProp_OtherSurface(const GeomAdaptor_Surface* theAdaptor)
      : myAdaptor(theAdaptor)
  {
  }

  GeomProp_OtherSurface(const Adaptor3d_Surface* theAdaptor)
      : myAdaptor(theAdaptor)
  {
  }

  //! Non-copyable and non-movable.
  GeomProp_OtherSurface(const GeomProp_OtherSurface&)            = delete;
  GeomProp_OtherSurface& operator=(const GeomProp_OtherSurface&) = delete;
  GeomProp_OtherSurface(GeomProp_OtherSurface&&)                 = delete;
  GeomProp_OtherSurface& operator=(GeomProp_OtherSurface&&)      = delete;

  //! Returns the adaptor pointer.
  const GeomAdaptor_Surface* Adaptor() const
  {
    return dynamic_cast<const GeomAdaptor_Surface*>(myAdaptor);
  }

  //! Compute surface normal at given parameter.
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
  const Adaptor3d_Surface* myAdaptor;
};

#endif // _GeomProp_OtherSurface_HeaderFile
