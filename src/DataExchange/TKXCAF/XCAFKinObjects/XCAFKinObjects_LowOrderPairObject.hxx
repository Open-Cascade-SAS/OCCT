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

#ifndef _XCAFKinObjects_LowOrderPairObject_HeaderFile
#define _XCAFKinObjects_LowOrderPairObject_HeaderFile

#include <XCAFKinObjects_PairObject.hxx>

class TColStd_HArray1OfReal;

DEFINE_STANDARD_HANDLE(XCAFKinObjects_LowOrderPairObject, XCAFKinObjects_PairObject)

// clang-format off
//! Object for low order kinematic pairs that represent standard mechanical joints
//! with common degrees of freedom and constraints.
//!
//! Low order pairs constrain the relative movement between two bodies in a well-defined manner.
//! Each pair has a specific combination of allowed and restricted degrees of freedom (DOF),
//! represented as a tuple of 6 boolean values <rX, rY, rZ, tX, tY, tZ> where:
//! - rX, rY, rZ: rotation around X, Y, and Z axes respectively
//! - tX, tY, tZ: translation along X, Y, and Z axes respectively
//! - T: allowed movement (true)
//! - F: restricted movement (false)
//! Supported low order pair types include:
//! - XCAFKinObjects_PairType_FullyConstrained <F, F, F, F, F, F> - No DOF, rigid connection
//! - XCAFKinObjects_PairType_Revolute         <F, F, T, F, F, F> - 1 DOF, rotation around Z only
//! - XCAFKinObjects_PairType_Prismatic        <F, F, F, T, F, F> - 1 DOF, translation along X only
//! - XCAFKinObjects_PairType_Cylindrical      <F, F, T, F, F, T> - 2 DOF, rotation and translation on same axis
//! - XCAFKinObjects_PairType_Universal        <T, F, T, F, F, F> - 2 DOF, rotation around X and Z (with skew angle)
//! - XCAFKinObjects_PairType_Homokinetic      <T, F, T, F, F, F> - 2 DOF, similar to Universal but with constant velocity
//! - XCAFKinObjects_PairType_SphericalWithPin <F, T, T, F, F, F> - 2 DOF, rotation around Y and Z
//! - XCAFKinObjects_PairType_Spherical        <T, T, T, F, F, F> - 3 DOF, rotation around all axes
//! - XCAFKinObjects_PairType_Planar           <T, F, F, T, T, F> - 3 DOF, rotation around X and translation along X and Y
//! - XCAFKinObjects_PairType_Unconstrained    <T, T, T, T, T, T> - 6 DOF, no constraints
// clang-format on
class XCAFKinObjects_LowOrderPairObject : public XCAFKinObjects_PairObject
{

public:
  //! Default constructor.
  //! Creates an empty low order pair object with all limits set to infinity (no constraints).
  Standard_EXPORT XCAFKinObjects_LowOrderPairObject();

  //! Copy constructor.
  //! @param theObj Source low order pair object to copy from
  Standard_EXPORT XCAFKinObjects_LowOrderPairObject(
    const Handle(XCAFKinObjects_LowOrderPairObject)& theObj);

  //! Sets the minimum rotation limit around X axis.
  //! @param theLimit Minimum rotation value in radians
  void SetMinRotationX(const Standard_Real theLimit)
  {
    myMinRotationX = theLimit;
    myIsRanged     = Standard_True;
  }

  //! Returns the minimum rotation limit around X axis.
  //! @return Minimum rotation value in radians
  Standard_Real MinRotationX() const { return myMinRotationX; }

  //! Sets the maximum rotation limit around X axis.
  //! @param theLimit Maximum rotation value in radians
  void SetMaxRotationX(const Standard_Real theLimit)
  {
    myMaxRotationX = theLimit;
    myIsRanged     = Standard_True;
  }

  //! Returns the maximum rotation limit around X axis.
  //! @return Maximum rotation value in radians
  Standard_Real MaxRotationX() const { return myMaxRotationX; }

  //! Sets the minimum rotation limit around Y axis.
  //! @param theLimit Minimum rotation value in radians
  void SetMinRotationY(const Standard_Real theLimit)
  {
    myMinRotationY = theLimit;
    myIsRanged     = Standard_True;
  }

  //! Returns the minimum rotation limit around Y axis.
  //! @return Minimum rotation value in radians
  Standard_Real MinRotationY() const { return myMinRotationY; }

  //! Sets the maximum rotation limit around Y axis.
  //! @param theLimit Maximum rotation value in radians
  void SetMaxRotationY(const Standard_Real theLimit)
  {
    myMaxRotationY = theLimit;
    myIsRanged     = Standard_True;
  }

  //! Returns the maximum rotation limit around Y axis.
  //! @return Maximum rotation value in radians
  Standard_Real MaxRotationY() const { return myMaxRotationY; }

  //! Sets the minimum rotation limit around Z axis.
  //! @param theLimit Minimum rotation value in radians
  void SetMinRotationZ(const Standard_Real theLimit)
  {
    myMinRotationZ = theLimit;
    myIsRanged     = Standard_True;
  }

  //! Returns the minimum rotation limit around Z axis.
  //! @return Minimum rotation value in radians
  Standard_Real MinRotationZ() const { return myMinRotationZ; }

  //! Sets the maximum rotation limit around Z axis.
  //! @param theLimit Maximum rotation value in radians
  void SetMaxRotationZ(const Standard_Real theLimit)
  {
    myMaxRotationZ = theLimit;
    myIsRanged     = Standard_True;
  }

  //! Returns the maximum rotation limit around Z axis.
  //! @return Maximum rotation value in radians
  Standard_Real MaxRotationZ() const { return myMaxRotationZ; }

  //! Sets the minimum translation limit along X axis.
  //! @param theLimit Minimum translation value
  void SetMinTranslationX(const Standard_Real theLimit)
  {
    myMinTranslationX = theLimit;
    myIsRanged        = Standard_True;
  }

  //! Returns the minimum translation limit along X axis.
  //! @return Minimum translation value
  Standard_Real MinTranslationX() const { return myMinTranslationX; }

  //! Sets the maximum translation limit along X axis.
  //! @param theLimit Maximum translation value
  void SetMaxTranslationX(const Standard_Real theLimit)
  {
    myMaxTranslationX = theLimit;
    myIsRanged        = Standard_True;
  }

  //! Returns the maximum translation limit along X axis.
  //! @return Maximum translation value
  Standard_Real MaxTranslationX() const { return myMaxTranslationX; }

  //! Sets the minimum translation limit along Y axis.
  //! @param theLimit Minimum translation value
  void SetMinTranslationY(const Standard_Real theLimit)
  {
    myMinTranslationY = theLimit;
    myIsRanged        = Standard_True;
  }

  //! Returns the minimum translation limit along Y axis.
  //! @return Minimum translation value
  Standard_Real MinTranslationY() const { return myMinTranslationY; }

  //! Sets the maximum translation limit along Y axis.
  //! @param theLimit Maximum translation value
  void SetMaxTranslationY(const Standard_Real theLimit)
  {
    myMaxTranslationY = theLimit;
    myIsRanged        = Standard_True;
  }

  //! Returns the maximum translation limit along Y axis.
  //! @return Maximum translation value
  Standard_Real MaxTranslationY() const { return myMaxTranslationY; }

  //! Sets the minimum translation limit along Z axis.
  //! @param theLimit Minimum translation value
  void SetMinTranslationZ(const Standard_Real theLimit)
  {
    myMinTranslationZ = theLimit;
    myIsRanged        = Standard_True;
  }

  //! Returns the minimum translation limit along Z axis.
  //! @return Minimum translation value
  Standard_Real MinTranslationZ() const { return myMinTranslationZ; }

  //! Sets the maximum translation limit along Z axis.
  //! @param theLimit Maximum translation value
  void SetMaxTranslationZ(const Standard_Real theLimit)
  {
    myMaxTranslationZ = theLimit;
    myIsRanged        = Standard_True;
  }

  //! Returns the maximum translation limit along Z axis.
  //! @return Maximum translation value
  Standard_Real MaxTranslationZ() const { return myMaxTranslationZ; }

  //! Sets the skew angle parameter for Universal or Homokinetic pairs.
  //! This angle defines the offset between the two rotation axes.
  //! Only applicable for XCAFKinObjects_PairType_Universal and XCAFKinObjects_PairType_Homokinetic.
  //! @param theAngle Skew angle value in radians
  void SetSkewAngle(const Standard_Real theAngle)
  {
    if (Type() == XCAFKinObjects_PairType_Universal
        || Type() == XCAFKinObjects_PairType_Homokinetic)
      mySkewAngle = theAngle;
  }

  //! Returns the skew angle parameter for Universal or Homokinetic pairs.
  //! @return Skew angle value in radians, or 0 if not applicable for this pair type
  Standard_Real SkewAngle() const
  {
    if (Type() == XCAFKinObjects_PairType_Universal
        || Type() == XCAFKinObjects_PairType_Homokinetic)
      return mySkewAngle;
    else
      return 0;
  }

  //! Creates and returns an array containing all limit values in a standard order.
  //! The array contains 12 values representing min/max values for rotation and translation
  //! around and along the X, Y and Z axes, ordered as:
  //! [minRotX, maxRotX, minRotY, maxRotY, minRotZ, maxRotZ,
  //!  minTransX, maxTransX, minTransY, maxTransY, minTransZ, maxTransZ]
  //! @return Array containing all limit values or NULL if no limits defined
  Standard_EXPORT Handle(TColStd_HArray1OfReal) GetAllLimits() const Standard_OVERRIDE;

  //! Checks if any limits are defined for this kinematic pair.
  //! @return true if at least one limit value has been defined, false otherwise
  Standard_EXPORT virtual Standard_Boolean HasLimits() const Standard_OVERRIDE;

  //! Sets all limit values from an array.
  //! The array must contain 12 values representing min/max values for rotation and translation
  //! around and along the X, Y and Z axes, ordered as:
  //! [minRotX, maxRotX, minRotY, maxRotY, minRotZ, maxRotZ,
  //!  minTransX, maxTransX, minTransY, maxTransY, minTransZ, maxTransZ]
  //! @param theLimits Array containing all limit values to set
  Standard_EXPORT void SetAllLimits(const Handle(TColStd_HArray1OfReal)& theLimits)
    Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(XCAFKinObjects_LowOrderPairObject, XCAFKinObjects_PairObject)

private:
  Standard_Real    myMinRotationX;    //!< Minimum limit for rotation around X axis (in radians)
  Standard_Real    myMaxRotationX;    //!< Maximum limit for rotation around X axis (in radians)
  Standard_Real    myMinRotationY;    //!< Minimum limit for rotation around Y axis (in radians)
  Standard_Real    myMaxRotationY;    //!< Maximum limit for rotation around Y axis (in radians)
  Standard_Real    myMinRotationZ;    //!< Minimum limit for rotation around Z axis (in radians)
  Standard_Real    myMaxRotationZ;    //!< Maximum limit for rotation around Z axis (in radians)
  Standard_Real    myMinTranslationX; //!< Minimum limit for translation along X axis
  Standard_Real    myMaxTranslationX; //!< Maximum limit for translation along X axis
  Standard_Real    myMinTranslationY; //!< Minimum limit for translation along Y axis
  Standard_Real    myMaxTranslationY; //!< Maximum limit for translation along Y axis
  Standard_Real    myMinTranslationZ; //!< Minimum limit for translation along Z axis
  Standard_Real    myMaxTranslationZ; //!< Maximum limit for translation along Z axis
  Standard_Real    mySkewAngle;       //!< Skew angle parameter for universal/homokinetic pairs
  Standard_Boolean myIsRanged;        //!< Flag indicating if any limits are defined
};

#endif // _XCAFKinObjects_LowOrderPairObject_HeaderFile
