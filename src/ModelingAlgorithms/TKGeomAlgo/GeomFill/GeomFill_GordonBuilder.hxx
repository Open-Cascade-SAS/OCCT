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

#ifndef _GeomFill_GordonBuilder_HeaderFile
#define _GeomFill_GordonBuilder_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <gp_Pnt.hxx>

//! Core mathematical kernel for Gordon surface construction via the Boolean sum method.
//!
//! Accepts pre-compatible BSpline curves (same degree and knot vector within each
//! direction) and intersection parameters. Builds three intermediate surfaces
//! (skin profiles, skin guides, tensor product) then computes the Boolean sum:
//!   S_gordon = S_profiles + S_guides - S_tensor
//!
//! This class can be used independently when curves are already compatible,
//! or via GeomFill_Gordon which handles curve preparation.
//!
//! Supports closed (periodic) curve networks when the first and last curves
//! in a direction are geometrically identical. The closedness flags enable
//! C2-continuous periodic interpolation during skinning.
//!
//! Limitations:
//! - Non-rational curves only
class GeomFill_GordonBuilder
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an empty Gordon builder.
  Standard_EXPORT GeomFill_GordonBuilder();

  //! Initializes the builder with compatible BSpline curves and intersection parameters.
  //! @param[in] theProfiles    array of profile curves (V-direction sections), must share
  //!                           the same degree and knot vector
  //! @param[in] theGuides      array of guide curves (U-direction sections), must share
  //!                           the same degree and knot vector
  //! @param[in] theProfileParams  parameter values at which profiles are positioned
  //!                              (in V-direction), size must equal theProfiles.Length()
  //! @param[in] theGuideParams    parameter values at which guides are positioned
  //!                              (in U-direction), size must equal theGuides.Length()
  //! @param[in] theTolerance   geometric tolerance for validation
  //! @param[in] theIsUClosed   if true, the U-direction (guides) forms a closed loop
  //! @param[in] theIsVClosed   if true, the V-direction (profiles) forms a closed loop
  Standard_EXPORT void Init(const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theProfiles,
                            const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theGuides,
                            const NCollection_Array1<double>& theProfileParams,
                            const NCollection_Array1<double>& theGuideParams,
                            double                            theTolerance,
                            bool                              theIsUClosed = false,
                            bool                              theIsVClosed = false);

  //! Performs the Gordon surface construction.
  Standard_EXPORT void Perform();

  //! Returns true if the surface was successfully constructed.
  [[nodiscard]] bool IsDone() const { return myIsDone; }

  //! Returns the resulting Gordon surface.
  //! @return handle to the constructed B-spline surface
  [[nodiscard]] Standard_EXPORT const occ::handle<Geom_BSplineSurface>& Surface() const;

  //! Returns the intermediate surface skinned through profiles.
  [[nodiscard]] Standard_EXPORT const occ::handle<Geom_BSplineSurface>& ProfileSurface() const;

  //! Returns the intermediate surface skinned through guides.
  [[nodiscard]] Standard_EXPORT const occ::handle<Geom_BSplineSurface>& GuideSurface() const;

  //! Returns the intermediate tensor product surface.
  [[nodiscard]] Standard_EXPORT const occ::handle<Geom_BSplineSurface>& TensorSurface() const;

private:
  //! Builds a skinned surface by interpolating pole columns of compatible curves.
  //! @param[in] theSections       array of compatible BSpline curves
  //! @param[in] theSectionParams  parameter values for each section
  //! @param[in] theIsClosed       if true, use periodic interpolation in V-direction
  //! @return the skinned B-spline surface, or null handle on failure
  occ::handle<Geom_BSplineSurface> buildSkinSurface(
    const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theSections,
    const NCollection_Array1<double>&                         theSectionParams,
    bool                                                      theIsClosed = false) const;

  //! Builds a tensor product surface interpolating a grid of intersection points.
  //! @param[in] thePoints     2D array of intersection points (NGuides x NProfiles)
  //! @param[in] theUParams    parameter values in U-direction (guide params)
  //! @param[in] theVParams    parameter values in V-direction (profile params)
  //! @param[in] theIsUClosed  if true, use periodic interpolation in U-direction
  //! @param[in] theIsVClosed  if true, use periodic interpolation in V-direction
  //! @return the tensor product B-spline surface, or null handle on failure
  occ::handle<Geom_BSplineSurface> buildTensorSurface(const NCollection_Array2<gp_Pnt>& thePoints,
                                                      const NCollection_Array1<double>& theUParams,
                                                      const NCollection_Array1<double>& theVParams,
                                                      bool theIsUClosed = false,
                                                      bool theIsVClosed = false) const;

  //! Unifies knot vectors of two surfaces so they share the same degrees and knots
  //! in both U and V directions.
  //! @param[in,out] theSurf1  first surface to unify
  //! @param[in,out] theSurf2  second surface to unify
  static void unifySurfaces(occ::handle<Geom_BSplineSurface>& theSurf1,
                            occ::handle<Geom_BSplineSurface>& theSurf2);

  //! Computes the Boolean sum: S_gordon = S_profiles + S_guides - S_tensor.
  //! All three surfaces must have identical degree and knot vectors.
  //! @param[in] theProfileSurf  surface skinned through profiles
  //! @param[in] theGuideSurf    surface skinned through guides
  //! @param[in] theTensorSurf   tensor product surface
  //! @return the Gordon surface
  occ::handle<Geom_BSplineSurface> computeBooleanSum(
    const occ::handle<Geom_BSplineSurface>& theProfileSurf,
    const occ::handle<Geom_BSplineSurface>& theGuideSurf,
    const occ::handle<Geom_BSplineSurface>& theTensorSurf) const;

  NCollection_Array1<occ::handle<Geom_BSplineCurve>> myProfiles;
  NCollection_Array1<occ::handle<Geom_BSplineCurve>> myGuides;
  NCollection_Array1<double>                         myProfileParams;
  NCollection_Array1<double>                         myGuideParams;
  occ::handle<Geom_BSplineSurface>                   mySurface;
  occ::handle<Geom_BSplineSurface>                   myProfileSurface;
  occ::handle<Geom_BSplineSurface>                   myGuideSurface;
  occ::handle<Geom_BSplineSurface>                   myTensorSurface;
  double                                             myTolerance = 0.0;
  bool                                               myIsUClosed = false;
  bool                                               myIsVClosed = false;
  bool                                               myIsDone    = false;
};

#endif // _GeomFill_GordonBuilder_HeaderFile
