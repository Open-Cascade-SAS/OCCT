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

#ifndef _GeomProp_OffsetSurface_HeaderFile
#define _GeomProp_OffsetSurface_HeaderFile

#include <GeomAdaptor_Surface.hxx>
#include <GeomProp.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <optional>

//! @brief Local differential properties for an offset surface.
//!
//! Uses numeric evaluation from surface derivatives.
//!
//! Can be constructed from either a GeomAdaptor_Surface pointer or a occ::handle<Geom_Surface>.
//! When constructed from a handle, no adaptor is created.
class GeomProp_OffsetSurface
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with adaptor pointer (non-owning).
  //! @param theAdaptor the surface adaptor (must not be null)
  GeomProp_OffsetSurface(
    const GeomAdaptor_Surface*  theAdaptor,
    GeomProp::SurfaceDerivOrder theOrder = GeomProp::SurfaceDerivOrder::Curvature)
      : myAdaptor(theAdaptor),
        myRequestedOrder(theOrder)
  {
  }

  //! Constructor from geometry handle.
  //! @param theSurface the 3D offset surface geometry
  //! @param theDomain optional parameter domain (for trimmed surfaces)
  GeomProp_OffsetSurface(const occ::handle<Geom_Surface>&              theSurface,
                         const std::optional<GeomProp::SurfaceDomain>& theDomain = std::nullopt)
      : myAdaptor(nullptr),
        myRequestedOrder(GeomProp::SurfaceDerivOrder::Curvature),
        mySurface(theSurface),
        myDomain(theDomain)
  {
  }

  //! Non-copyable and non-movable.
  GeomProp_OffsetSurface(const GeomProp_OffsetSurface&)            = delete;
  GeomProp_OffsetSurface& operator=(const GeomProp_OffsetSurface&) = delete;
  GeomProp_OffsetSurface(GeomProp_OffsetSurface&&)                 = delete;
  GeomProp_OffsetSurface& operator=(GeomProp_OffsetSurface&&)      = delete;

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
  GeomProp::SurfaceDerivOrder            myRequestedOrder;
  mutable GeomProp::SurfaceCache         myCache;
  occ::handle<Geom_Surface>              mySurface; //!< Geometry handle (handle path)
  std::optional<GeomProp::SurfaceDomain> myDomain;  //!< Optional parameter domain
};

#endif // _GeomProp_OffsetSurface_HeaderFile
