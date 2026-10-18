// Created by: Nikolai BUKHALOV
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _GeomLib_CheckCurveOnSurface_HeaderFile
#define _GeomLib_CheckCurveOnSurface_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <Precision.hxx>
#include <Standard.hxx>

#include <optional>

class Adaptor3d_CurveOnSurface;

//! Computes the max distance between 3D-curve and 2D-curve
//! in some surface.
class GeomLib_CheckCurveOnSurface
{
public:
  DEFINE_STANDARD_ALLOC

  //! Default constructor
  Standard_EXPORT GeomLib_CheckCurveOnSurface();

  //! Constructor
  Standard_EXPORT GeomLib_CheckCurveOnSurface(const occ::handle<Adaptor3d_Curve>& theCurve,
                                              const double theTolRange = Precision::PConfusion());

  //! Sets the data for the algorithm
  Standard_EXPORT void Init(const occ::handle<Adaptor3d_Curve>& theCurve,
                            const double theTolRange = Precision::PConfusion());

  //! Initializes all members by default values
  Standard_EXPORT void Init();

  //! Computes the max distance for the 3d curve <myCurve>
  //! and 2d curve <theCurveOnSurface>
  //! If isMultiThread == true then computation will be performed in parallel.
  Standard_EXPORT void Perform(const occ::handle<Adaptor3d_CurveOnSurface>& theCurveOnSurface);

  //! Computes the maximum distance, or an evaluated distance above the limit.
  //! @param[in] theCurveOnSurface curve on surface to check
  //! @param[in] theDistanceLimit non-negative finite distance limit
  Standard_EXPORT void Perform(const occ::handle<Adaptor3d_CurveOnSurface>& theCurveOnSurface,
                               double                                       theDistanceLimit);

  //! Sets parallel flag
  void SetParallel(const bool theIsParallel) { myIsParallel = theIsParallel; }

  //! Returns true if parallel flag is set
  bool IsParallel() { return myIsParallel; }

  //! Returns true if the requested distance computation succeeded.
  bool IsDone() const { return (myErrorStatus == 0); }

  //! Returns error status
  //! The possible values are:
  //! 0 - OK;
  //! 1 - null curve or surface or 2d curve;
  //! 2 - invalid parametric range or distance limit;
  //! 3 - error in calculations.
  int ErrorStatus() const { return myErrorStatus; }

  //! Returns the maximum distance, or an evaluated distance exceeding the supplied limit.
  double MaxDistance() const { return myMaxDistance; }

  //! Returns the parameter of the reported distance.
  double MaxParameter() const { return myMaxParameter; }

private:
  //! Computes a full maximum or a distance exceeding the supplied limit.
  void perform(const occ::handle<Adaptor3d_CurveOnSurface>& theCurveOnSurface,
               std::optional<double>                        theDistanceLimit);

  occ::handle<Adaptor3d_Curve> myCurve;
  int                          myErrorStatus;
  double                       myMaxDistance;
  double                       myMaxParameter;
  double                       myTolRange;
  bool                         myIsParallel;
};

#endif // _BRepLib_CheckCurveOnSurface_HeaderFile
