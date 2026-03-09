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

#ifndef _GeomProp_Torus_HeaderFile
#define _GeomProp_Torus_HeaderFile

#include <GeomAdaptor_Surface.hxx>
#include <GeomProp.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <optional>

//! @brief Local differential properties for a toroidal surface.
//!
//! Uses analytical formulas. Curvature varies along the meridian (V direction):
//! - k1 = 1/r (constant, along the minor circle direction)
//! - k2 = cos(V) / (R + r*cos(V)) (varies, along the major circle direction)
//! where R is the major radius and r is the minor radius.
//!
//! Can be constructed from either a GeomAdaptor_Surface pointer or a occ::handle<Geom_Surface>.
//! When constructed from a handle, no adaptor is created.
class GeomProp_Torus
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with adaptor pointer (non-owning).
  //! @param theAdaptor the surface adaptor (must not be null)
  GeomProp_Torus(const GeomAdaptor_Surface*  theAdaptor,
                 GeomProp::SurfaceDerivOrder theOrder = GeomProp::SurfaceDerivOrder::Undefined)
      : myAdaptor(theAdaptor)
  {
    (void)theOrder;
  }

  //! Constructor from geometry handle.
  //! @param theSurface the 3D toroidal surface geometry
  //! @param theDomain optional parameter domain (for trimmed surfaces)
  GeomProp_Torus(const occ::handle<Geom_Surface>&              theSurface,
                 const std::optional<GeomProp::SurfaceDomain>& theDomain = std::nullopt,
                 GeomProp::SurfaceDerivOrder theOrder = GeomProp::SurfaceDerivOrder::Undefined)
      : myAdaptor(nullptr),
        mySurface(theSurface),
        myDomain(theDomain)
  {
    (void)theOrder;
  }

  //! Non-copyable and non-movable.
  GeomProp_Torus(const GeomProp_Torus&)            = delete;
  GeomProp_Torus& operator=(const GeomProp_Torus&) = delete;
  GeomProp_Torus(GeomProp_Torus&&)                 = delete;
  GeomProp_Torus& operator=(GeomProp_Torus&&)      = delete;

  //! Sets the derivative caching order (no-op for analytical surfaces).
  void SetDerivOrder(GeomProp::SurfaceDerivOrder) {}

  //! Returns the derivative caching order (always Undefined for analytical surfaces).
  GeomProp::SurfaceDerivOrder DerivOrder() const { return GeomProp::SurfaceDerivOrder::Undefined; }

  //! Returns the adaptor pointer (nullptr when constructed from handle).
  const GeomAdaptor_Surface* Adaptor() const { return myAdaptor; }

  //! Returns pointer to underlying geometry, or nullptr if constructed from adaptor.
  const Geom_Surface* Geometry() const { return mySurface.get(); }

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
  const GeomAdaptor_Surface*             myAdaptor;
  occ::handle<Geom_Surface>              mySurface; //!< Geometry handle (handle path)
  std::optional<GeomProp::SurfaceDomain> myDomain;  //!< Optional parameter domain
};

#endif // _GeomProp_Torus_HeaderFile
