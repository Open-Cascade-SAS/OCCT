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

#ifndef _GeomProp_BezierSurface_HeaderFile
#define _GeomProp_BezierSurface_HeaderFile

#include <GeomAdaptor_Surface.hxx>
#include <GeomProp.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief Local differential properties for a Bezier surface.
//!
//! Uses numeric evaluation from adaptor derivatives.
//!
//! @warning The caller must ensure that the adaptor pointer remains valid
//! for the entire lifetime of this object.
class GeomProp_BezierSurface
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with adaptor pointer (non-owning).
  //! @param theAdaptor the surface adaptor (must not be null)
  GeomProp_BezierSurface(const GeomAdaptor_Surface*  theAdaptor,
                         GeomProp::SurfaceDerivOrder theOrder = GeomProp::SurfaceDerivOrder::Curvature)
      : myAdaptor(theAdaptor),
        myRequestedOrder(theOrder)
  {
  }

  //! Non-copyable and non-movable.
  GeomProp_BezierSurface(const GeomProp_BezierSurface&)            = delete;
  GeomProp_BezierSurface& operator=(const GeomProp_BezierSurface&) = delete;
  GeomProp_BezierSurface(GeomProp_BezierSurface&&)                 = delete;
  GeomProp_BezierSurface& operator=(GeomProp_BezierSurface&&)      = delete;

  //! Returns the adaptor pointer.
  const GeomAdaptor_Surface* Adaptor() const { return myAdaptor; }

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
  const GeomAdaptor_Surface*     myAdaptor;
  GeomProp::SurfaceDerivOrder    myRequestedOrder;
  mutable GeomProp::SurfaceCache myCache;
};

#endif // _GeomProp_BezierSurface_HeaderFile
