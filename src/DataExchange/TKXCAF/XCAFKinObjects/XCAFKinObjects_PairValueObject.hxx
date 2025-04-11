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

//! \class XCAFKinObjects_PairValueObject
//! \brief Object to store and process current position
//! of kinematic pair.
//! NoType - no values,
//!
//! FullyConstrained - no values,
//! Revolute - angle,
//! Prismatic - length,
//! Cylindrical - angle + length,
//! Universal - angle + angle,
//! Homokinetic - angle + angle,
//! SphericalWithPin - ypr,
//! Spherical - ypr,
//! Planar - angle + length + length,
//! Unconstrained - ax2,
//!
//! Screw - angle,
//! RackAndPinion - length,
//! Gear - angle,
//!
//! PointOnSurface pnt on surface + ypr,
//! SlidingSurface - angle + pnt on surface + pnt on surface,
//! RollingSurface - angle + pnt on surface,
//! PointOnPlanarCurve pnt on curve + ypr,
//! SlidingCurve - pnt on curve + pnt on curve,
//! RollingCurve - pnt on curve
//!
//! ypr - three angles: yaw, pitch, roll
//! point on surface - (u,v)
//! point on curve - (t)
//! ax2 - consists of location and two directions
class XCAFKinObjects_PairValueObject : public Standard_Transient
{

public:
  Standard_EXPORT XCAFKinObjects_PairValueObject();

  Standard_EXPORT XCAFKinObjects_PairValueObject(const Handle(XCAFKinObjects_PairValueObject)& theObj);

  Standard_EXPORT void SetType(const XCAFKinObjects_PairType theType);

  XCAFKinObjects_PairType Type() const { return myType; }

  void SetAllValues(const Handle(TColStd_HArray1OfReal)& theValues)
  {
    if (myValues->Length() == theValues->Length())
      myValues = theValues;
  };

  Handle(TColStd_HArray1OfReal) GetAllValues() const { return myValues; }

  // Rotations

  void SetRotation(const Standard_Real theRotation) { SetFirstRotation(theRotation); }

  Standard_Real GetRotation() const { return GetFirstRotation(); }

  Standard_EXPORT void SetFirstRotation(const Standard_Real theRotation);

  Standard_EXPORT Standard_Real GetFirstRotation() const;

  Standard_EXPORT void SetSecondRotation(const Standard_Real theRotation);

  Standard_EXPORT Standard_Real GetSecondRotation() const;

  // Translations

  void SetTranslation(const Standard_Real theTranslation) { SetFirstTranslation(theTranslation); }

  Standard_Real GetTranslation() const { return GetFirstTranslation(); }

  Standard_EXPORT void SetFirstTranslation(const Standard_Real theTranslation);

  Standard_EXPORT Standard_Real GetFirstTranslation() const;

  Standard_EXPORT void SetSecondTranslation(const Standard_Real theTranslation);

  Standard_EXPORT Standard_Real GetSecondTranslation() const;

  // Points on surface

  void SetPointOnSurface(const Standard_Real theU, const Standard_Real theV)
  {
    SetFirstPointOnSurface(theU, theV);
  }

  Standard_Boolean GetPointOnSurface(Standard_Real& theU, Standard_Real& theV) const
  {
    return GetFirstPointOnSurface(theU, theV);
  }

  Standard_EXPORT void SetFirstPointOnSurface(const Standard_Real theU, const Standard_Real theV);

  Standard_EXPORT Standard_Boolean GetFirstPointOnSurface(Standard_Real& theU,
                                                          Standard_Real& theV) const;

  Standard_EXPORT void SetSecondPointOnSurface(const Standard_Real theU, const Standard_Real theV);

  Standard_EXPORT Standard_Boolean GetSecondPointOnSurface(Standard_Real& theU,
                                                           Standard_Real& theV) const;

  // Points on curve

  void SetPointOnCurve(const Standard_Real theT) { SetFirstPointOnCurve(theT); }

  Standard_Real GetPointOnCurve() const { return GetFirstPointOnCurve(); }

  Standard_EXPORT void SetFirstPointOnCurve(const Standard_Real theT);

  Standard_EXPORT Standard_Real GetFirstPointOnCurve() const;

  Standard_EXPORT void SetSecondPointOnCurve(const Standard_Real theT);

  Standard_EXPORT Standard_Real GetSecondPointOnCurve() const;

  // YPR

  Standard_EXPORT void SetYPR(const Standard_Real theYaw,
                              const Standard_Real thePitch,
                              const Standard_Real theRoll);

  Standard_EXPORT Standard_Boolean GetYPR(Standard_Real& theYaw,
                                          Standard_Real& thePitch,
                                          Standard_Real& theRoll) const;

  // Transformation

  Standard_EXPORT void SetTransformation(const gp_Ax2& theTrsf);

  Standard_EXPORT gp_Ax2 GetTransformation();

  DEFINE_STANDARD_RTTIEXT(XCAFKinObjects_PairValueObject, Standard_Transient)

private:
  XCAFKinObjects_PairType           myType;   //!< type of kinematic pair
  Handle(TColStd_HArray1OfReal) myValues; //!< all values in a line
};

#endif // _XCAFKinObjects_PairValueObject_HeaderFile
