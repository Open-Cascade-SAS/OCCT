// Created on : Sat May 02 12:41:16 2020 
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

#ifndef _StepKinematics_UniversalPairWithRange_HeaderFile_
#define _StepKinematics_UniversalPairWithRange_HeaderFile_

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <StepKinematics_UniversalPair.hxx>

#include <TCollection_HAsciiString.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepRepr_ItemDefinedTransformation.hxx>
#include <StepKinematics_KinematicJoint.hxx>

DEFINE_STANDARD_HANDLE(StepKinematics_UniversalPairWithRange, StepKinematics_UniversalPair)

//! Representation of STEP entity UniversalPairWithRange
class StepKinematics_UniversalPairWithRange : public StepKinematics_UniversalPair
{
public :

  //! default constructor
  Standard_EXPORT StepKinematics_UniversalPairWithRange();

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
                           const Standard_Boolean hasUniversalPair_InputSkewAngle,
                           const Standard_Real theUniversalPair_InputSkewAngle,
                           const Standard_Boolean hasLowerLimitFirstRotation,
                           const Standard_Real theLowerLimitFirstRotation,
                           const Standard_Boolean hasUpperLimitFirstRotation,
                           const Standard_Real theUpperLimitFirstRotation,
                           const Standard_Boolean hasLowerLimitSecondRotation,
                           const Standard_Real theLowerLimitSecondRotation,
                           const Standard_Boolean hasUpperLimitSecondRotation,
                           const Standard_Real theUpperLimitSecondRotation);

  //! Returns field LowerLimitFirstRotation
  Standard_EXPORT Standard_Real LowerLimitFirstRotation() const;
  //! Sets field LowerLimitFirstRotation
  Standard_EXPORT void SetLowerLimitFirstRotation (const Standard_Real theLowerLimitFirstRotation);
  //! Returns True if optional field LowerLimitFirstRotation is defined
  Standard_EXPORT Standard_Boolean HasLowerLimitFirstRotation() const;

  //! Returns field UpperLimitFirstRotation
  Standard_EXPORT Standard_Real UpperLimitFirstRotation() const;
  //! Sets field UpperLimitFirstRotation
  Standard_EXPORT void SetUpperLimitFirstRotation (const Standard_Real theUpperLimitFirstRotation);
  //! Returns True if optional field UpperLimitFirstRotation is defined
  Standard_EXPORT Standard_Boolean HasUpperLimitFirstRotation() const;

  //! Returns field LowerLimitSecondRotation
  Standard_EXPORT Standard_Real LowerLimitSecondRotation() const;
  //! Sets field LowerLimitSecondRotation
  Standard_EXPORT void SetLowerLimitSecondRotation (const Standard_Real theLowerLimitSecondRotation);
  //! Returns True if optional field LowerLimitSecondRotation is defined
  Standard_EXPORT Standard_Boolean HasLowerLimitSecondRotation() const;

  //! Returns field UpperLimitSecondRotation
  Standard_EXPORT Standard_Real UpperLimitSecondRotation() const;
  //! Sets field UpperLimitSecondRotation
  Standard_EXPORT void SetUpperLimitSecondRotation (const Standard_Real theUpperLimitSecondRotation);
  //! Returns True if optional field UpperLimitSecondRotation is defined
  Standard_EXPORT Standard_Boolean HasUpperLimitSecondRotation() const;

DEFINE_STANDARD_RTTIEXT(StepKinematics_UniversalPairWithRange, StepKinematics_UniversalPair)

private:
  Standard_Real myLowerLimitFirstRotation; //!< optional
  Standard_Real myUpperLimitFirstRotation; //!< optional
  Standard_Real myLowerLimitSecondRotation; //!< optional
  Standard_Real myUpperLimitSecondRotation; //!< optional
  Standard_Boolean defLowerLimitFirstRotation; //!< flag "is LowerLimitFirstRotation defined"
  Standard_Boolean defUpperLimitFirstRotation; //!< flag "is UpperLimitFirstRotation defined"
  Standard_Boolean defLowerLimitSecondRotation; //!< flag "is LowerLimitSecondRotation defined"
  Standard_Boolean defUpperLimitSecondRotation; //!< flag "is UpperLimitSecondRotation defined"

};
#endif // _StepKinematics_UniversalPairWithRange_HeaderFile_
