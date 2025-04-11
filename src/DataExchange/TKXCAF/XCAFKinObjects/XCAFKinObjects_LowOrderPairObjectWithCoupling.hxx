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

#ifndef _XCAFKinObjects_LowOrderPairObjectWithCoupling_HeaderFile
#define _XCAFKinObjects_LowOrderPairObjectWithCoupling_HeaderFile

#include <XCAFKinObjects_PairObject.hxx>
#include <TColStd_HArray1OfReal.hxx>

DEFINE_STANDARD_HANDLE(XCAFKinObjects_LowOrderPairObjectWithCoupling, XCAFKinObjects_PairObject)

//! Object for low order kinematic pairs with motion coupling that represent
//! mechanical joints where the movement of one component is directly linked to
//! the movement of another component through a specific mechanical relationship.
//!
//! These pairs have additional parameters that define the coupling relationship
//! between the movements. Supported pair types include:
//! - XCAFKinObjects_PairType_Screw: Rotation is coupled with translation along the
//!   same axis according to the pitch parameter (like a screw thread)
//! - XCAFKinObjects_PairType_RackAndPinion: Rotation of the pinion is coupled with
//!   linear translation of the rack
//! - XCAFKinObjects_PairType_Gear: Rotation of one gear is coupled with rotation
//!   of another gear according to their radii
//! - XCAFKinObjects_PairType_LinearFlexibleAndPinion: Linear motion is coupled with
//!   rotational motion through a flexible connection
class XCAFKinObjects_LowOrderPairObjectWithCoupling : public XCAFKinObjects_PairObject
{

public:
  //! Default constructor.
  //! Creates an empty low order pair object with motion coupling and initializes
  //! limits to infinity (no constraints).
  Standard_EXPORT XCAFKinObjects_LowOrderPairObjectWithCoupling();

  //! Copy constructor.
  //! @param theObj Source low order pair object with motion coupling to copy from
  Standard_EXPORT XCAFKinObjects_LowOrderPairObjectWithCoupling(
    const Handle(XCAFKinObjects_LowOrderPairObjectWithCoupling)& theObj);

  //! Sets the type of the low order pair with motion coupling and initializes
  //! appropriate parameter arrays based on the type.
  //! For Gear pairs, creates a parameter array with 5 elements.
  //! For other types, creates a parameter array with 1 element.
  //! @param theType Type of kinematic pair to set
  Standard_EXPORT void SetType(const XCAFKinObjects_PairType theType) Standard_OVERRIDE;

  //! Sets the lower limit of the motion range.
  //! @param theLimit Lower limit value
  void SetLowLimit(const Standard_Real theLimit)
  {
    myIsRanged = Standard_True;
    myLowLimit = theLimit;
  }

  //! Returns the lower limit of the motion range.
  //! @return Lower limit value
  Standard_Real LowLimit() const { return myLowLimit; }

  //! Sets the upper limit of the motion range.
  //! @param theLimit Upper limit value
  void SetUpperLimit(const Standard_Real theLimit)
  {
    myIsRanged   = Standard_True;
    myUpperLimit = theLimit;
  }

  //! Returns the upper limit of the motion range.
  //! @return Upper limit value
  Standard_Real UpperLimit() const { return myUpperLimit; }

  //! Creates and returns an array containing both limit values.
  //! The array contains 2 values: [lowLimit, upperLimit]
  //! @return Array containing limit values or NULL if no limits defined
  Standard_EXPORT Handle(TColStd_HArray1OfReal) GetAllLimits() const Standard_OVERRIDE;

  //! Sets the pitch parameter for Screw pairs.
  //! The pitch defines the linear displacement per revolution.
  //! Only applicable for XCAFKinObjects_PairType_Screw.
  //! @param thePitch Pitch value (linear displacement per revolution)
  Standard_EXPORT void SetPitch(const Standard_Real thePitch);

  //! Returns the pitch parameter for Screw pairs.
  //! @return Pitch value, or 0 if not applicable for this pair type
  Standard_EXPORT Standard_Real Pitch() const;

  //! Sets the pinion radius parameter for RackAndPinion or LinearFlexibleAndPinion pairs.
  //! Only applicable for XCAFKinObjects_PairType_RackAndPinion and
  //! XCAFKinObjects_PairType_LinearFlexibleAndPinion.
  //! @param theRadius Radius value of the pinion
  Standard_EXPORT void SetPinionRadius(const Standard_Real theRadius);

  //! Returns the pinion radius parameter for RackAndPinion or LinearFlexibleAndPinion pairs.
  //! @return Pinion radius value, or 0 if not applicable for this pair type
  Standard_EXPORT Standard_Real PinionRadius() const;

  //! Sets the first link radius parameter for Gear pairs.
  //! Only applicable for XCAFKinObjects_PairType_Gear.
  //! @param theRadius Radius value of the first gear
  Standard_EXPORT void SetRadiusFirstLink(const Standard_Real theRadius);

  //! Returns the first link radius parameter for Gear pairs.
  //! @return First gear radius value, or 0 if not applicable for this pair type
  Standard_EXPORT Standard_Real RadiusFirstLink() const;

  //! Sets the second link radius parameter for Gear pairs.
  //! Only applicable for XCAFKinObjects_PairType_Gear.
  //! @param theRadius Radius value of the second gear
  Standard_EXPORT void SetRadiusSecondLink(const Standard_Real theRadius);

  //! Returns the second link radius parameter for Gear pairs.
  //! @return Second gear radius value, or 0 if not applicable for this pair type
  Standard_EXPORT Standard_Real RadiusSecondLink() const;

  //! Sets the bevel parameter for Gear pairs.
  //! The bevel parameter defines the angle between gear axes for bevel gears.
  //! Only applicable for XCAFKinObjects_PairType_Gear.
  //! @param theBevel Bevel angle value in radians
  Standard_EXPORT void SetBevel(const Standard_Real theBevel);

  //! Returns the bevel parameter for Gear pairs.
  //! @return Bevel angle value in radians, or 0 if not applicable for this pair type
  Standard_EXPORT Standard_Real Bevel() const;

  //! Sets the helical angle parameter for Gear pairs.
  //! The helical angle defines the angle of the gear teeth relative to the axis of rotation.
  //! Only applicable for XCAFKinObjects_PairType_Gear.
  //! @param theAngle Helical angle value in radians
  Standard_EXPORT void SetHelicalAngle(const Standard_Real theAngle);

  //! Returns the helical angle parameter for Gear pairs.
  //! @return Helical angle value in radians, or 0 if not applicable for this pair type
  Standard_EXPORT Standard_Real HelicalAngle() const;

  //! Sets the gear ratio parameter for Gear pairs.
  //! Only applicable for XCAFKinObjects_PairType_Gear.
  //! @param theGearRatio Gear ratio value
  Standard_EXPORT void SetGearRatio(const Standard_Real theGearRatio);

  //! Returns the gear ratio parameter for Gear pairs.
  //! @return Gear ratio value, or 0 if not applicable for this pair type
  Standard_EXPORT Standard_Real GearRatio() const;

  //! Sets all parameters from an array.
  //! The array size must match the expected number of parameters for the pair type:
  //! - 1 parameter for Screw, RackAndPinion, and LinearFlexibleAndPinion pairs
  //! - 5 parameters for Gear pairs
  //! @param theParams Array containing parameter values
  Standard_EXPORT void SetAllParams(const Handle(TColStd_HArray1OfReal)& theParams);

  //! Returns the array containing all parameter values for the pair.
  //! @return Array of parameter values
  Handle(TColStd_HArray1OfReal) GetAllParams() const { return myParams; }

  //! Checks if any limits are defined for this kinematic pair.
  //! @return true if at least one limit value has been defined, false otherwise
  Standard_EXPORT Standard_Boolean HasLimits() const Standard_OVERRIDE;

  //! Sets both limit values from an array.
  //! The array must contain exactly 2 values: [lowLimit, upperLimit]
  //! @param theLimits Array containing limit values to set
  Standard_EXPORT void SetAllLimits(const Handle(TColStd_HArray1OfReal)& theLimits)
    Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(XCAFKinObjects_LowOrderPairObjectWithCoupling, XCAFKinObjects_PairObject)

private:
  //! Lower limit of motion range
  Standard_Real myLowLimit;
  //! Upper limit of motion range
  Standard_Real myUpperLimit;
  //! Array of coupling parameters specific to the pair type
  Handle(TColStd_HArray1OfReal) myParams;
  //! Flag indicating if any limits are defined
  Standard_Boolean myIsRanged;
};

#endif // _XCAFKinObjects_LowOrderPairObjectWithCoupling_HeaderFile
