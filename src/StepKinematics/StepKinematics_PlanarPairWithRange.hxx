// Created on : Sat May 02 12:41:15 2020 
// Created by: Irina KRYLOVA
// Generator:	Express (EXPRESS -> CASCADE/XSTEP Translator) V3.0
// Copyright (c) Open CASCADE 2020
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

#ifndef _StepKinematics_PlanarPairWithRange_HeaderFile_
#define _StepKinematics_PlanarPairWithRange_HeaderFile_

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <StepKinematics_PlanarPair.hxx>

#include <TCollection_HAsciiString.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepRepr_ItemDefinedTransformation.hxx>
#include <StepKinematics_KinematicJoint.hxx>

DEFINE_STANDARD_HANDLE(StepKinematics_PlanarPairWithRange, StepKinematics_PlanarPair)

//! Representation of STEP entity PlanarPairWithRange
class StepKinematics_PlanarPairWithRange : public StepKinematics_PlanarPair
{
public :

  //! default constructor
  Standard_EXPORT StepKinematics_PlanarPairWithRange();

  //! Initialize all fields (own and inherited)
 Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
                           const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Name,
                           const Standard_Boolean hasItemDefinedTransformation_Description,
                           const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Description,
                           const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem1,
                           const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem2,
                           const Handle(StepKinematics_KinematicJoint)& theKinematicPair_Joint,
                           const Standard_Boolean theLowOrderKinematicPair_TX,
                           const Standard_Boolean theLowOrderKinematicPair_TY,
                           const Standard_Boolean theLowOrderKinematicPair_TZ,
                           const Standard_Boolean theLowOrderKinematicPair_RX,
                           const Standard_Boolean theLowOrderKinematicPair_RY,
                           const Standard_Boolean theLowOrderKinematicPair_RZ,
                           const Standard_Boolean hasLowerLimitActualRotation,
                           const Standard_Real theLowerLimitActualRotation,
                           const Standard_Boolean hasUpperLimitActualRotation,
                           const Standard_Real theUpperLimitActualRotation,
                           const Standard_Boolean hasLowerLimitActualTranslationX,
                           const Standard_Real theLowerLimitActualTranslationX,
                           const Standard_Boolean hasUpperLimitActualTranslationX,
                           const Standard_Real theUpperLimitActualTranslationX,
                           const Standard_Boolean hasLowerLimitActualTranslationY,
                           const Standard_Real theLowerLimitActualTranslationY,
                           const Standard_Boolean hasUpperLimitActualTranslationY,
                           const Standard_Real theUpperLimitActualTranslationY);

  //! Returns field LowerLimitActualRotation
  Standard_EXPORT Standard_Real LowerLimitActualRotation() const;
  //! Sets field LowerLimitActualRotation
  Standard_EXPORT void SetLowerLimitActualRotation (const Standard_Real theLowerLimitActualRotation);
  //! Returns True if optional field LowerLimitActualRotation is defined
  Standard_EXPORT Standard_Boolean HasLowerLimitActualRotation() const;

  //! Returns field UpperLimitActualRotation
  Standard_EXPORT Standard_Real UpperLimitActualRotation() const;
  //! Sets field UpperLimitActualRotation
  Standard_EXPORT void SetUpperLimitActualRotation (const Standard_Real theUpperLimitActualRotation);
  //! Returns True if optional field UpperLimitActualRotation is defined
  Standard_EXPORT Standard_Boolean HasUpperLimitActualRotation() const;

  //! Returns field LowerLimitActualTranslationX
  Standard_EXPORT Standard_Real LowerLimitActualTranslationX() const;
  //! Sets field LowerLimitActualTranslationX
  Standard_EXPORT void SetLowerLimitActualTranslationX (const Standard_Real theLowerLimitActualTranslationX);
  //! Returns True if optional field LowerLimitActualTranslationX is defined
  Standard_EXPORT Standard_Boolean HasLowerLimitActualTranslationX() const;

  //! Returns field UpperLimitActualTranslationX
  Standard_EXPORT Standard_Real UpperLimitActualTranslationX() const;
  //! Sets field UpperLimitActualTranslationX
  Standard_EXPORT void SetUpperLimitActualTranslationX (const Standard_Real theUpperLimitActualTranslationX);
  //! Returns True if optional field UpperLimitActualTranslationX is defined
  Standard_EXPORT Standard_Boolean HasUpperLimitActualTranslationX() const;

  //! Returns field LowerLimitActualTranslationY
  Standard_EXPORT Standard_Real LowerLimitActualTranslationY() const;
  //! Sets field LowerLimitActualTranslationY
  Standard_EXPORT void SetLowerLimitActualTranslationY (const Standard_Real theLowerLimitActualTranslationY);
  //! Returns True if optional field LowerLimitActualTranslationY is defined
  Standard_EXPORT Standard_Boolean HasLowerLimitActualTranslationY() const;

  //! Returns field UpperLimitActualTranslationY
  Standard_EXPORT Standard_Real UpperLimitActualTranslationY() const;
  //! Sets field UpperLimitActualTranslationY
  Standard_EXPORT void SetUpperLimitActualTranslationY (const Standard_Real theUpperLimitActualTranslationY);
  //! Returns True if optional field UpperLimitActualTranslationY is defined
  Standard_EXPORT Standard_Boolean HasUpperLimitActualTranslationY() const;

DEFINE_STANDARD_RTTIEXT(StepKinematics_PlanarPairWithRange, StepKinematics_PlanarPair)

private:
  Standard_Real myLowerLimitActualRotation; //!< optional
  Standard_Real myUpperLimitActualRotation; //!< optional
  Standard_Real myLowerLimitActualTranslationX; //!< optional
  Standard_Real myUpperLimitActualTranslationX; //!< optional
  Standard_Real myLowerLimitActualTranslationY; //!< optional
  Standard_Real myUpperLimitActualTranslationY; //!< optional
  Standard_Boolean defLowerLimitActualRotation; //!< flag "is LowerLimitActualRotation defined"
  Standard_Boolean defUpperLimitActualRotation; //!< flag "is UpperLimitActualRotation defined"
  Standard_Boolean defLowerLimitActualTranslationX; //!< flag "is LowerLimitActualTranslationX defined"
  Standard_Boolean defUpperLimitActualTranslationX; //!< flag "is UpperLimitActualTranslationX defined"
  Standard_Boolean defLowerLimitActualTranslationY; //!< flag "is LowerLimitActualTranslationY defined"
  Standard_Boolean defUpperLimitActualTranslationY; //!< flag "is UpperLimitActualTranslationY defined"

};
#endif // _StepKinematics_PlanarPairWithRange_HeaderFile_
