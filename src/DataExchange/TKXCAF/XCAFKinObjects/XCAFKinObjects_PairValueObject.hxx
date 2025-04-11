// Created on: 2020-03-27
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

#ifndef _XCAFKinObjects_PairValueObject_HeaderFile
#define _XCAFKinObjects_PairValueObject_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <XCAFKinObjects_PairType.hxx>

DEFINE_STANDARD_HANDLE(XCAFKinObjects_PairValueObject, Standard_Transient)

class gp_Ax2;

//! Object to store and process current position or state of a kinematic pair.
//!
//! Each kinematic pair type requires different value parameters to define its state:
//!
//! Basic low order pairs:
//! - XCAFKinObjects_PairType_NoType - no values
//! - XCAFKinObjects_PairType_FullyConstrained - no values
//! - XCAFKinObjects_PairType_Revolute - angle (rotation)
//! - XCAFKinObjects_PairType_Prismatic - length (translation)
//! - XCAFKinObjects_PairType_Cylindrical - angle + length (rotation and translation)
//! - XCAFKinObjects_PairType_Universal - angle + angle (two rotations)
//! - XCAFKinObjects_PairType_Homokinetic - angle + angle (two rotations)
//! - XCAFKinObjects_PairType_SphericalWithPin - yaw, pitch, roll (three rotation angles)
//! - XCAFKinObjects_PairType_Spherical - yaw, pitch, roll (three rotation angles)
//! - XCAFKinObjects_PairType_Planar - angle + length + length (rotation and two translations)
//! - XCAFKinObjects_PairType_Unconstrained - ax2 (complete transformation)
//!
//! Low order pairs with motion coupling:
//! - XCAFKinObjects_PairType_Screw - angle (rotation that determines translation via pitch)
//! - XCAFKinObjects_PairType_RackAndPinion - length (translation that determines rotation)
//! - XCAFKinObjects_PairType_Gear - angle (rotation of first gear that determines rotation of
//! second)
//!
//! High order pairs:
//! - XCAFKinObjects_PairType_PointOnSurface - (u,v) + yaw, pitch, roll (point on surface +
//! orientation)
//! - XCAFKinObjects_PairType_SlidingSurface - angle + (u1,v1) + (u2,v2) (rotation + two points on
//! surfaces)
//! - XCAFKinObjects_PairType_RollingSurface - angle + (u,v) (rotation + point on surface)
//! - XCAFKinObjects_PairType_PointOnPlanarCurve - t + yaw, pitch, roll (point on curve +
//! orientation)
//! - XCAFKinObjects_PairType_SlidingCurve - t1 + t2 (two points on curves)
//! - XCAFKinObjects_PairType_RollingCurve - t (point on curve)
class XCAFKinObjects_PairValueObject : public Standard_Transient
{

public:
  //! Default constructor.
  //! Creates an empty pair value object with undefined type and no values.
  Standard_EXPORT XCAFKinObjects_PairValueObject();

  //! Copy constructor.
  //! @param theObj Source pair value object to copy from
  Standard_EXPORT XCAFKinObjects_PairValueObject(
    const Handle(XCAFKinObjects_PairValueObject)& theObj);

  //! Sets the type of the kinematic pair and initializes appropriate value array.
  //! Different pair types require different numbers of values to define their state.
  //! @param theType Type of kinematic pair to set
  Standard_EXPORT void SetType(const XCAFKinObjects_PairType theType);

  //! Returns the type of the kinematic pair.
  //! @return Type of kinematic pair as an enumeration value
  XCAFKinObjects_PairType Type() const { return myType; }

  //! Sets all values for the pair state from an array.
  //! The array size must match the size required by the pair type.
  //! @param theValues Array containing all value parameters
  void SetAllValues(const Handle(TColStd_HArray1OfReal)& theValues)
  {
    if (myValues->Length() == theValues->Length())
      myValues = theValues;
  };

  //! Returns all values for the pair state.
  //! @return Array containing all value parameters
  Handle(TColStd_HArray1OfReal) GetAllValues() const { return myValues; }

  //! Sets the rotation value for pairs with single rotation parameter.
  //! Convenience method that calls SetFirstRotation.
  //! @param theRotation Rotation angle in radians
  void SetRotation(const Standard_Real theRotation) { SetFirstRotation(theRotation); }

  //! Gets the rotation value for pairs with single rotation parameter.
  //! Convenience method that calls GetFirstRotation.
  //! @return Rotation angle in radians
  Standard_Real GetRotation() const { return GetFirstRotation(); }

  //! Sets the first rotation value for pairs with rotation parameters.
  //! Applicable for Revolute, Cylindrical, Universal, Homokinetic, Planar,
  //! Screw, Gear, SlidingSurface, and RollingSurface pairs.
  //! @param theRotation Rotation angle in radians
  Standard_EXPORT void SetFirstRotation(const Standard_Real theRotation);

  //! Gets the first rotation value for pairs with rotation parameters.
  //! @return First rotation angle in radians, or 0 if not applicable
  Standard_EXPORT Standard_Real GetFirstRotation() const;

  //! Sets the second rotation value for pairs with multiple rotation parameters.
  //! Currently only applicable for Universal and Homokinetic pairs.
  //! @param theRotation Second rotation angle in radians
  Standard_EXPORT void SetSecondRotation(const Standard_Real theRotation);

  //! Gets the second rotation value for pairs with multiple rotation parameters.
  //! @return Second rotation angle in radians, or 0 if not applicable
  Standard_EXPORT Standard_Real GetSecondRotation() const;

  //! Sets the translation value for pairs with single translation parameter.
  //! Convenience method that calls SetFirstTranslation.
  //! @param theTranslation Translation distance
  void SetTranslation(const Standard_Real theTranslation) { SetFirstTranslation(theTranslation); }

  //! Gets the translation value for pairs with single translation parameter.
  //! Convenience method that calls GetFirstTranslation.
  //! @return Translation distance
  Standard_Real GetTranslation() const { return GetFirstTranslation(); }

  //! Sets the first translation value for pairs with translation parameters.
  //! Applicable for Prismatic, Cylindrical, Planar, and RackAndPinion pairs.
  //! @param theTranslation Translation distance
  Standard_EXPORT void SetFirstTranslation(const Standard_Real theTranslation);

  //! Gets the first translation value for pairs with translation parameters.
  //! @return First translation distance, or 0 if not applicable
  Standard_EXPORT Standard_Real GetFirstTranslation() const;

  //! Sets the second translation value for pairs with multiple translation parameters.
  //! Currently only applicable for Planar pairs.
  //! @param theTranslation Second translation distance
  Standard_EXPORT void SetSecondTranslation(const Standard_Real theTranslation);

  //! Gets the second translation value for pairs with multiple translation parameters.
  //! @return Second translation distance, or 0 if not applicable
  Standard_EXPORT Standard_Real GetSecondTranslation() const;

  //! Sets the point on surface parameters for pairs with single point on surface.
  //! Convenience method that calls SetFirstPointOnSurface.
  //! @param theU U parameter on surface
  //! @param theV V parameter on surface
  void SetPointOnSurface(const Standard_Real theU, const Standard_Real theV)
  {
    SetFirstPointOnSurface(theU, theV);
  }

  //! Gets the point on surface parameters for pairs with single point on surface.
  //! Convenience method that calls GetFirstPointOnSurface.
  //! @param theU [out] U parameter on surface
  //! @param theV [out] V parameter on surface
  //! @return true if values were retrieved successfully, false otherwise
  Standard_Boolean GetPointOnSurface(Standard_Real& theU, Standard_Real& theV) const
  {
    return GetFirstPointOnSurface(theU, theV);
  }

  //! Sets the first point on surface parameters.
  //! Applicable for PointOnSurface, SlidingSurface, and RollingSurface pairs.
  //! @param theU U parameter on first surface
  //! @param theV V parameter on first surface
  Standard_EXPORT void SetFirstPointOnSurface(const Standard_Real theU, const Standard_Real theV);

  //! Gets the first point on surface parameters.
  //! @param theU [out] U parameter on first surface
  //! @param theV [out] V parameter on first surface
  //! @return true if values were retrieved successfully, false otherwise
  Standard_EXPORT Standard_Boolean GetFirstPointOnSurface(Standard_Real& theU,
                                                          Standard_Real& theV) const;

  //! Sets the second point on surface parameters.
  //! Applicable only for SlidingSurface pairs.
  //! @param theU U parameter on second surface
  //! @param theV V parameter on second surface
  Standard_EXPORT void SetSecondPointOnSurface(const Standard_Real theU, const Standard_Real theV);

  //! Gets the second point on surface parameters.
  //! @param theU [out] U parameter on second surface
  //! @param theV [out] V parameter on second surface
  //! @return true if values were retrieved successfully, false otherwise
  Standard_EXPORT Standard_Boolean GetSecondPointOnSurface(Standard_Real& theU,
                                                           Standard_Real& theV) const;

  //! Sets the point on curve parameter for pairs with single point on curve.
  //! Convenience method that calls SetFirstPointOnCurve.
  //! @param theT Parameter on curve
  void SetPointOnCurve(const Standard_Real theT) { SetFirstPointOnCurve(theT); }

  //! Gets the point on curve parameter for pairs with single point on curve.
  //! Convenience method that calls GetFirstPointOnCurve.
  //! @return Parameter on curve
  Standard_Real GetPointOnCurve() const { return GetFirstPointOnCurve(); }

  //! Sets the first point on curve parameter.
  //! Applicable for PointOnPlanarCurve, SlidingCurve, and RollingCurve pairs.
  //! @param theT Parameter on first curve
  Standard_EXPORT void SetFirstPointOnCurve(const Standard_Real theT);

  //! Gets the first point on curve parameter.
  //! @return Parameter on first curve, or 0 if not applicable
  Standard_EXPORT Standard_Real GetFirstPointOnCurve() const;

  //! Sets the second point on curve parameter.
  //! Applicable only for SlidingCurve pairs.
  //! @param theT Parameter on second curve
  Standard_EXPORT void SetSecondPointOnCurve(const Standard_Real theT);

  //! Gets the second point on curve parameter.
  //! @return Parameter on second curve, or 0 if not applicable
  Standard_EXPORT Standard_Real GetSecondPointOnCurve() const;

  //! Sets yaw, pitch, roll orientation angles.
  //! Applicable for SphericalWithPin, Spherical, PointOnSurface, and PointOnPlanarCurve pairs.
  //! @param theYaw Yaw angle in radians
  //! @param thePitch Pitch angle in radians
  //! @param theRoll Roll angle in radians
  Standard_EXPORT void SetYPR(const Standard_Real theYaw,
                              const Standard_Real thePitch,
                              const Standard_Real theRoll);

  //! Gets yaw, pitch, roll orientation angles.
  //! @param theYaw [out] Yaw angle in radians
  //! @param thePitch [out] Pitch angle in radians
  //! @param theRoll [out] Roll angle in radians
  //! @return true if values were retrieved successfully, false otherwise
  Standard_EXPORT Standard_Boolean GetYPR(Standard_Real& theYaw,
                                          Standard_Real& thePitch,
                                          Standard_Real& theRoll) const;

  //! Sets the complete transformation for Unconstrained pairs.
  //! @param theTrsf Coordinate system defining the transformation
  Standard_EXPORT void SetTransformation(const gp_Ax2& theTrsf);

  //! Gets the complete transformation for Unconstrained pairs.
  //! @return Coordinate system representing the transformation
  Standard_EXPORT gp_Ax2 GetTransformation();

  DEFINE_STANDARD_RTTIEXT(XCAFKinObjects_PairValueObject, Standard_Transient)

private:
  XCAFKinObjects_PairType myType; //!< Type of kinematic pair defining the required state parameters
  Handle(TColStd_HArray1OfReal) myValues; //!< Array containing all state parameter values
};

#endif // _XCAFKinObjects_PairValueObject_HeaderFile
