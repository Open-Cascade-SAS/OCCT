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

#include <Geom_Plane.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomProp.hxx>
#include <Standard_DefineAlloc.hxx>

#include <optional>

//! @brief Local differential properties for a plane surface.
//!
//! Trivial implementation: constant normal, zero curvatures everywhere.
//! All properties are computed analytically without numerical evaluation.
//!
//! Can be constructed from either a GeomAdaptor_Surface pointer or a occ::handle<Geom_Surface>.
//! When constructed from a handle, no adaptor is created.
class GeomProp_Plane
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with adaptor pointer (non-owning).
  //! @param theAdaptor the surface adaptor (must not be null)
  GeomProp_Plane(const GeomAdaptor_Surface*  theAdaptor,
                 GeomProp::SurfaceDerivOrder theOrder = GeomProp::SurfaceDerivOrder::Undefined)
      : myAdaptor(theAdaptor),
        myPosition(theAdaptor->Plane().Position())
  {
    (void)theOrder;
  }

  //! Constructor from geometry handle.
  //! @param theSurface the 3D plane geometry
  //! @param theDomain optional parameter domain (unused for plane)
  GeomProp_Plane(const occ::handle<Geom_Surface>&              theSurface,
                 const std::optional<GeomProp::SurfaceDomain>& theDomain = std::nullopt,
                 GeomProp::SurfaceDerivOrder theOrder = GeomProp::SurfaceDerivOrder::Undefined)
      : myAdaptor(nullptr),
        mySurface(theSurface),
        myPosition(occ::down_cast<Geom_Plane>(theSurface)->Pln().Position()),
        myDomain(theDomain)
  {
    (void)theOrder;
  }

  //! Non-copyable and non-movable.
  GeomProp_Plane(const GeomProp_Plane&)            = delete;
  GeomProp_Plane& operator=(const GeomProp_Plane&) = delete;
  GeomProp_Plane(GeomProp_Plane&&)                 = delete;
  GeomProp_Plane& operator=(GeomProp_Plane&&)      = delete;

  //! Sets the derivative caching order (no-op for analytical surfaces).
  void SetDerivOrder(GeomProp::SurfaceDerivOrder) {}

  //! Returns the derivative caching order (always Undefined for analytical surfaces).
  GeomProp::SurfaceDerivOrder DerivOrder() const { return GeomProp::SurfaceDerivOrder::Undefined; }

  //! Returns the adaptor pointer (nullptr when constructed from handle).
  const GeomAdaptor_Surface* Adaptor() const { return myAdaptor; }

  //! Returns pointer to underlying geometry, or nullptr if constructed from adaptor.
  const Geom_Surface* Geometry() const { return mySurface.get(); }

  //! Compute surface normal. Constant for a plane.
  //! Uses D1U x D1V cross product to ensure correct sign for flipped planes.
  GeomProp::SurfaceNormalResult Normal(double theU, double theV, double theTol) const
  {
    gp_Pnt aPnt;
    gp_Vec aD1U, aD1V;
    if (!mySurface.IsNull())
    {
      mySurface->D1(theU, theV, aPnt, aD1U, aD1V);
    }
    else if (myAdaptor != nullptr)
    {
      myAdaptor->D1(theU, theV, aPnt, aD1U, aD1V);
    }
    else
    {
      return {{}, false};
    }
    return GeomProp::ComputeSurfaceNormal(aD1U, aD1V, theTol);
  }

  //! Compute principal curvatures. Both are zero for a plane.
  GeomProp::SurfaceCurvatureResult Curvatures(double /*theU*/,
                                              double /*theV*/,
                                              double /*theTol*/) const
  {
    if (mySurface.IsNull() && myAdaptor == nullptr)
    {
      return {};
    }
    GeomProp::SurfaceCurvatureResult aResult;
    aResult.MinCurvature = 0.0;
    aResult.MaxCurvature = 0.0;
    aResult.MinDirection = myPosition.XDirection();
    aResult.MaxDirection = myPosition.YDirection();
    aResult.IsDefined    = true;
    aResult.IsUmbilic    = true;
    return aResult;
  }

  //! Compute mean and Gaussian curvatures. Both are zero for a plane.
  GeomProp::MeanGaussianResult MeanGaussian(double /*theU*/,
                                            double /*theV*/,
                                            double /*theTol*/) const
  {
    if (mySurface.IsNull() && myAdaptor == nullptr)
    {
      return {};
    }
    return {0.0, 0.0, true};
  }

private:
  const GeomAdaptor_Surface*             myAdaptor;
  occ::handle<Geom_Surface>              mySurface;  //!< Geometry handle (handle path)
  gp_Ax3                                 myPosition; //!< Cached plane position
  std::optional<GeomProp::SurfaceDomain> myDomain;   //!< Optional parameter domain
};

#endif // _GeomProp_Plane_HeaderFile
