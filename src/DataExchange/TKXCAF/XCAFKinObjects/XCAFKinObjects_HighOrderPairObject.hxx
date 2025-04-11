// Created on: 2020-03-16
// Created by: Irina KRYLOVA
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

#ifndef _XCAFKinObjects_HighOrderPairObject_HeaderFile
#define _XCAFKinObjects_HighOrderPairObject_HeaderFile

#include <Geom_Geometry.hxx>
#include <XCAFKinObjects_PairObject.hxx>
#include <TColStd_HArray1OfReal.hxx>

class Geom_Curve;
class Geom_Surface;
class Geom_RectangularTrimmedSurface;
class Geom_TrimmedCurve;

DEFINE_STANDARD_HANDLE(XCAFKinObjects_HighOrderPairObject, XCAFKinObjects_PairObject)

//! Object for high order kinematic pairs, which are characterized by higher complexity
//! and typically involve interactions between curves and surfaces.
//!
//! Supported high order pair types include:
//! - XCAFKinObjects_PairType_PointOnSurface: A point constrained to move on a surface
//! - XCAFKinObjects_PairType_SlidingSurface: Two surfaces that slide against each other
//! - XCAFKinObjects_PairType_RollingSurface: Two surfaces that roll against each other
//! - XCAFKinObjects_PairType_PointOnPlanarCurve: A point constrained to move on a planar curve
//! - XCAFKinObjects_PairType_SlidingCurve: Two curves that slide against each other
//! - XCAFKinObjects_PairType_RollingCurve: Two curves that roll against each other
//! - XCAFKinObjects_PairType_LinearFlexibleAndPlanarCurve: A flexible connection that can move
//! along a curve
//!
//! Each pair type has specific geometric definitions and constraints that can be stored
//! using this object. The object can store curves, surfaces, and limit values depending
//! on the specific pair type.
class XCAFKinObjects_HighOrderPairObject : public XCAFKinObjects_PairObject
{

public:
  //! Default constructor.
  //! Creates an empty high order pair object with undefined type.
  Standard_EXPORT XCAFKinObjects_HighOrderPairObject();

  //! Copy constructor.
  //! @param theObj Source high order pair object to copy from
  Standard_EXPORT XCAFKinObjects_HighOrderPairObject(
    const Handle(XCAFKinObjects_HighOrderPairObject)& theObj);

  //! Sets the type of the high order pair and initializes appropriate data structures.
  //! Different pair types require different geometric representations and limit values.
  //! @param theType Type of kinematic pair to set
  Standard_EXPORT void SetType(const XCAFKinObjects_PairType theType) Standard_OVERRIDE;

  //! Sets the orientation flag for the pair.
  //! @param theOrientation Orientation value to set (true/false)
  void SetOrientation(const Standard_Boolean theOrientation) { myOrientation = theOrientation; }

  //! Returns the orientation flag value.
  //! @return Current orientation flag
  Standard_Boolean Orientation() const { return myOrientation; }

  //! Sets all limit values for the pair from an array.
  //! The array size must match the expected number of limits for the pair type.
  //! @param theLimits Array containing limit values
  Standard_EXPORT void SetAllLimits(const Handle(TColStd_HArray1OfReal)& theLimits)
    Standard_OVERRIDE;

  //! Checks if the pair has defined limit values.
  //! @return true if limits are defined, false otherwise
  Standard_EXPORT Standard_Boolean HasLimits() const Standard_OVERRIDE;

  //! Returns all limit values for the pair.
  //! @return Array containing all limit values, or NULL if no limits are defined
  Handle(TColStd_HArray1OfReal) GetAllLimits() const Standard_OVERRIDE
  {

    if (HasLimits())
      return myLimits;
    return nullptr;
  }

  //! Sets low limit of rotation attribute (only for SlidingSurface and RollingSurface pairs).
  //! @param theLimit Lower rotation limit value
  Standard_EXPORT void SetRotationLowLimit(const Standard_Real theLimit);

  //! Gets low rotation limit (only valid for SlidingSurface and RollingSurface pairs).
  //! @return Lower rotation limit value, or 0 if not applicable
  Standard_EXPORT Standard_Real RotationLowLimit();

  //! Sets upper limit of rotation attribute (only for SlidingSurface and RollingSurface pairs).
  //! @param theLimit Upper rotation limit value
  Standard_EXPORT void SetRotationUpperLimit(const Standard_Real theLimit);

  //! Gets upper rotation limit (only valid for SlidingSurface and RollingSurface pairs).
  //! @return Upper rotation limit value, or 0 if not applicable
  Standard_EXPORT Standard_Real RotationUpperLimit();

  //! Sets low limit of yaw angle (only for PointOnPlanarCurve and PointOnSurface pairs).
  //! @param theLimit Lower yaw angle limit value
  Standard_EXPORT void SetLowLimitYaw(const Standard_Real theLimit);

  //! Gets low limit of yaw angle (only valid for PointOnPlanarCurve and PointOnSurface pairs).
  //! @return Lower yaw angle limit value, or 0 if not applicable
  Standard_EXPORT Standard_Real LowLimitYaw();

  //! Sets upper limit of yaw angle (only for PointOnPlanarCurve and PointOnSurface pairs).
  //! @param theLimit Upper yaw angle limit value
  Standard_EXPORT void SetUpperLimitYaw(const Standard_Real theLimit);

  //! Gets upper limit of yaw angle (only valid for PointOnPlanarCurve and PointOnSurface pairs).
  //! @return Upper yaw angle limit value, or 0 if not applicable
  Standard_EXPORT Standard_Real UpperLimitYaw();

  //! Sets low limit of roll angle (only for PointOnPlanarCurve and PointOnSurface pairs).
  //! @param theLimit Lower roll angle limit value
  Standard_EXPORT void SetLowLimitRoll(const Standard_Real theLimit);

  //! Gets low limit of roll angle (only valid for PointOnPlanarCurve and PointOnSurface pairs).
  //! @return Lower roll angle limit value, or 0 if not applicable
  Standard_EXPORT Standard_Real LowLimitRoll();

  //! Sets upper limit of roll angle (only for PointOnPlanarCurve and PointOnSurface pairs).
  //! @param theLimit Upper roll angle limit value
  Standard_EXPORT void SetUpperLimitRoll(const Standard_Real theLimit);

  //! Gets upper limit of roll angle (only valid for PointOnPlanarCurve and PointOnSurface pairs).
  //! @return Upper roll angle limit value, or 0 if not applicable
  Standard_EXPORT Standard_Real UpperLimitRoll();

  //! Sets low limit of pitch angle (only for PointOnPlanarCurve and PointOnSurface pairs).
  //! @param theLimit Lower pitch angle limit value
  Standard_EXPORT void SetLowLimitPitch(const Standard_Real theLimit);

  //! Gets low limit of pitch angle (only valid for PointOnPlanarCurve and PointOnSurface pairs).
  //! @return Lower pitch angle limit value, or 0 if not applicable
  Standard_EXPORT Standard_Real LowLimitPitch();

  //! Sets upper limit of pitch angle (only for PointOnPlanarCurve and PointOnSurface pairs).
  //! @param theLimit Upper pitch angle limit value
  Standard_EXPORT void SetUpperLimitPitch(const Standard_Real theLimit);

  //! Gets upper limit of pitch angle (only valid for PointOnPlanarCurve and PointOnSurface pairs).
  //! @return Upper pitch angle limit value, or 0 if not applicable
  Standard_EXPORT Standard_Real UpperLimitPitch();

  //! Sets curve attribute (only for PointOnPlanarCurve and LinearFlexibleAndPlanarCurve pairs).
  //! @param theCurve Curve geometry to set
  Standard_EXPORT void SetCurve(const Handle(Geom_Curve)& theCurve);

  //! Gets curve attribute (only valid for PointOnPlanarCurve and LinearFlexibleAndPlanarCurve
  //! pairs).
  //! @return Handle to the curve, or NULL if not applicable
  Standard_EXPORT Handle(Geom_Curve) Curve() const;

  //! Sets first curve attribute (only for SlidingCurve and RollingCurve pairs).
  //! @param theCurve First curve geometry to set
  Standard_EXPORT void SetFirstCurve(const Handle(Geom_Curve)& theCurve);

  //! Gets first curve attribute (only valid for SlidingCurve and RollingCurve pairs).
  //! @return Handle to the first curve, or NULL if not applicable
  Standard_EXPORT Handle(Geom_Curve) FirstCurve() const;

  //! Sets second curve attribute (only for SlidingCurve and RollingCurve pairs).
  //! @param theCurve Second curve geometry to set
  Standard_EXPORT void SetSecondCurve(const Handle(Geom_Curve)& theCurve);

  //! Gets second curve attribute (only valid for SlidingCurve and RollingCurve pairs).
  //! @return Handle to the second curve, or NULL if not applicable
  Standard_EXPORT Handle(Geom_Curve) SecondCurve() const;

  //! Sets surface attribute (only for PointOnSurface pairs).
  //! @param theSurface Surface geometry to set
  Standard_EXPORT void SetSurface(const Handle(Geom_Surface)& theSurface);

  //! Gets surface attribute (only valid for PointOnSurface pairs).
  //! @return Handle to the surface, or NULL if not applicable
  Standard_EXPORT Handle(Geom_Surface) Surface() const;

  //! Sets first surface attribute (only for SlidingSurface and RollingSurface pairs).
  //! @param theSurface First surface geometry to set
  Standard_EXPORT void SetFirstSurface(const Handle(Geom_Surface)& theSurface);

  //! Gets first surface attribute (only valid for SlidingSurface and RollingSurface pairs).
  //! @return Handle to the first surface, or NULL if not applicable
  Standard_EXPORT Handle(Geom_Surface) FirstSurface() const;

  //! Sets second surface attribute (only for SlidingSurface and RollingSurface pairs).
  //! @param theSurface Second surface geometry to set
  Standard_EXPORT void SetSecondSurface(const Handle(Geom_Surface)& theSurface);

  //! Gets second surface attribute (only valid for SlidingSurface and RollingSurface pairs).
  //! @return Handle to the second surface, or NULL if not applicable
  Standard_EXPORT Handle(Geom_Surface) SecondSurface() const;

  DEFINE_STANDARD_RTTIEXT(XCAFKinObjects_HighOrderPairObject, XCAFKinObjects_PairObject)

private:
  //! Orientation flag for the kinematic pair
  Standard_Boolean myOrientation;
  //! Array of limit values, size depends on pair type
  Handle(TColStd_HArray1OfReal) myLimits;
  //! Geometric elements (curves or surfaces) for the pair
  NCollection_Array1<Handle(Geom_Geometry)> myGeom;
  //! Flag indicating if limits are defined
  Standard_Boolean myIsRanged;
};

#endif // _XCAFKinObjects_HighOrderPairObject_HeaderFile
