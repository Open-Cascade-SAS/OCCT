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

#include <optional>

//! @brief Local differential properties for a conical surface.
//!
//! Uses analytical formulas where possible; the curvature
//! varies along the meridian (V direction).
//! Min principal curvature = 0 (along the ruling direction).
//! Max principal curvature = cos(alpha) / R(V), where alpha is the half-angle
//! and R(V) is the radius at parameter V.
//!
//! Can be constructed from either a GeomAdaptor_Surface pointer or a occ::handle<Geom_Surface>.
//! When constructed from a handle, no adaptor is created.
class GeomProp_Cone
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with adaptor pointer (non-owning).
  //! @param theAdaptor the surface adaptor (must not be null)
  GeomProp_Cone(const GeomAdaptor_Surface*  theAdaptor,
                GeomProp::SurfaceDerivOrder theOrder = GeomProp::SurfaceDerivOrder::Undefined)
      : myAdaptor(theAdaptor)
  {
    (void)theOrder;
  }

  //! Constructor from geometry handle.
  //! @param theSurface the 3D conical surface geometry
  //! @param theDomain optional parameter domain (for trimmed surfaces)
  GeomProp_Cone(const occ::handle<Geom_Surface>&              theSurface,
                const std::optional<GeomProp::SurfaceDomain>& theDomain = std::nullopt,
                GeomProp::SurfaceDerivOrder theOrder = GeomProp::SurfaceDerivOrder::Undefined)
      : myAdaptor(nullptr),
        mySurface(theSurface),
        myDomain(theDomain)
  {
    (void)theOrder;
  }

  //! Non-copyable and non-movable.
  GeomProp_Cone(const GeomProp_Cone&)            = delete;
  GeomProp_Cone& operator=(const GeomProp_Cone&) = delete;
  GeomProp_Cone(GeomProp_Cone&&)                 = delete;
  GeomProp_Cone& operator=(GeomProp_Cone&&)      = delete;

  //! Sets the derivative caching order (no-op for analytical surfaces).
  void SetDerivOrder(GeomProp::SurfaceDerivOrder) {}

  //! Returns the derivative caching order (always Undefined for analytical surfaces).
  GeomProp::SurfaceDerivOrder DerivOrder() const { return GeomProp::SurfaceDerivOrder::Undefined; }

  //! Returns the adaptor pointer (nullptr when constructed from handle).
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
  const GeomAdaptor_Surface*             myAdaptor;
  occ::handle<Geom_Surface>              mySurface; //!< Geometry handle (handle path)
  std::optional<GeomProp::SurfaceDomain> myDomain;  //!< Optional parameter domain
};

#endif // _GeomProp_Cone_HeaderFile
