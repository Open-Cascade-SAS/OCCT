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

#ifndef _BRepProp_Surface_HeaderFile
#define _BRepProp_Surface_HeaderFile

#include <BRepAdaptor_Surface.hxx>
#include <GeomProp.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

class TopoDS_Face;

//! @brief Local differential property evaluator for BRep faces.
//!
//! Thin wrapper over GeomProp:: free functions.
//! Delegates derivative computation to BRepAdaptor_Surface and passes
//! results to GeomProp::ComputeSurfaceNormal, ComputeSurfaceCurvatures, etc.
//!
//! Can be initialized from a TopoDS_Face or a BRepAdaptor_Surface.
//! When initialized from an existing BRepAdaptor_Surface, the adaptor is
//! referenced without copying (non-owning); when initialized from a
//! TopoDS_Face, an internal adaptor is created and owned.
//!
//! Usage:
//! @code
//!   BRepProp_Surface aProp(myFace);
//!   GeomProp::SurfaceNormalResult aNorm = aProp.Normal(0.5, 0.5, Precision::Confusion());
//!   if (aNorm.IsDefined)
//!   {
//!     gp_Dir aDir = aNorm.Direction;
//!   }
//! @endcode
class BRepProp_Surface
{
public:
  DEFINE_STANDARD_ALLOC

  //! Construct from a TopoDS_Face.
  //! Creates an internal BRepAdaptor_Surface (owning).
  //! @param[in] theFace the face to evaluate
  Standard_EXPORT BRepProp_Surface(const TopoDS_Face& theFace);

  //! Construct from an existing BRepAdaptor_Surface.
  //! The adaptor is referenced without copying (non-owning);
  //! the caller must ensure the adaptor outlives this object.
  //! @param[in] theSurface the adaptor to reference
  Standard_EXPORT BRepProp_Surface(const BRepAdaptor_Surface& theSurface);

  //! Construct from a handle to BRepAdaptor_Surface.
  //! Shares ownership of the adaptor (no copy).
  //! @param[in] theSurface handle to the adaptor
  Standard_EXPORT BRepProp_Surface(const occ::handle<BRepAdaptor_Surface>& theSurface);

  //! Non-copyable and non-movable.
  BRepProp_Surface(const BRepProp_Surface&)            = delete;
  BRepProp_Surface& operator=(const BRepProp_Surface&) = delete;
  BRepProp_Surface(BRepProp_Surface&&)                 = delete;
  BRepProp_Surface& operator=(BRepProp_Surface&&)      = delete;

  //! Returns the underlying adaptor.
  const BRepAdaptor_Surface& Adaptor() const { return *myPtr; }

  //! Compute surface normal at given (U, V) parameter.
  //! @param[in] theU U parameter
  //! @param[in] theV V parameter
  //! @param[in] theTol linear tolerance
  //! @return normal result with validity flag
  Standard_EXPORT GeomProp::SurfaceNormalResult Normal(double theU,
                                                       double theV,
                                                       double theTol) const;

  //! Compute principal curvatures at given (U, V) parameter.
  //! @param[in] theU U parameter
  //! @param[in] theV V parameter
  //! @param[in] theTol linear tolerance
  //! @return curvature result with min/max values, directions, and validity flag
  Standard_EXPORT GeomProp::SurfaceCurvatureResult Curvatures(double theU,
                                                              double theV,
                                                              double theTol) const;

  //! Compute mean and Gaussian curvatures at given (U, V) parameter.
  //! @param[in] theU U parameter
  //! @param[in] theV V parameter
  //! @param[in] theTol linear tolerance
  //! @return mean/Gaussian curvature result with validity flag
  Standard_EXPORT GeomProp::MeanGaussianResult MeanGaussian(double theU,
                                                            double theV,
                                                            double theTol) const;

protected:
  //! Initialize from a TopoDS_Face.
  //! @param[in] theFace the face to evaluate
  Standard_EXPORT void initialization(const TopoDS_Face& theFace);

  //! Initialize from an existing BRepAdaptor_Surface (non-owning).
  //! @param[in] theSurface the adaptor to reference
  Standard_EXPORT void initialization(const BRepAdaptor_Surface& theSurface);

  //! Initialize from a handle to BRepAdaptor_Surface.
  //! @param[in] theSurface handle to the adaptor
  Standard_EXPORT void initialization(const occ::handle<BRepAdaptor_Surface>& theSurface);

private:
  occ::handle<BRepAdaptor_Surface> myOwned; //!< Owns the adaptor when created from TopoDS_Face.
  const BRepAdaptor_Surface*       myPtr = nullptr; //!< Non-owning pointer to the active adaptor.
};

#endif // _BRepProp_Surface_HeaderFile
