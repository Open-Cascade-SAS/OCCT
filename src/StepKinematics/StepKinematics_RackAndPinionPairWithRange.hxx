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

#ifndef _StepKinematics_RackAndPinionPairWithRange_HeaderFile_
#define _StepKinematics_RackAndPinionPairWithRange_HeaderFile_

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <StepKinematics_RackAndPinionPair.hxx>

#include <TCollection_HAsciiString.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepRepr_ItemDefinedTransformation.hxx>
#include <StepKinematics_KinematicJoint.hxx>

DEFINE_STANDARD_HANDLE(StepKinematics_RackAndPinionPairWithRange, StepKinematics_RackAndPinionPair)

//! Representation of STEP entity RackAndPinionPairWithRange
class StepKinematics_RackAndPinionPairWithRange : public StepKinematics_RackAndPinionPair
{
public :

  //! default constructor
  Standard_EXPORT StepKinematics_RackAndPinionPairWithRange();

  //! Initialize all fields (own and inherited)
 Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
                           const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Name,
                           const Standard_Boolean hasItemDefinedTransformation_Description,
                           const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Description,
                           const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem1,
                           const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem2,
                           const Handle(StepKinematics_KinematicJoint)& theKinematicPair_Joint,
                           const Standard_Real theRackAndPinionPair_PinionRadius,
                           const Standard_Boolean hasLowerLimitRackDisplacement,
                           const Standard_Real theLowerLimitRackDisplacement,
                           const Standard_Boolean hasUpperLimitRackDisplacement,
                           const Standard_Real theUpperLimitRackDisplacement);

  //! Returns field LowerLimitRackDisplacement
  Standard_EXPORT Standard_Real LowerLimitRackDisplacement() const;
  //! Sets field LowerLimitRackDisplacement
  Standard_EXPORT void SetLowerLimitRackDisplacement (const Standard_Real theLowerLimitRackDisplacement);
  //! Returns True if optional field LowerLimitRackDisplacement is defined
  Standard_EXPORT Standard_Boolean HasLowerLimitRackDisplacement() const;

  //! Returns field UpperLimitRackDisplacement
  Standard_EXPORT Standard_Real UpperLimitRackDisplacement() const;
  //! Sets field UpperLimitRackDisplacement
  Standard_EXPORT void SetUpperLimitRackDisplacement (const Standard_Real theUpperLimitRackDisplacement);
  //! Returns True if optional field UpperLimitRackDisplacement is defined
  Standard_EXPORT Standard_Boolean HasUpperLimitRackDisplacement() const;

DEFINE_STANDARD_RTTIEXT(StepKinematics_RackAndPinionPairWithRange, StepKinematics_RackAndPinionPair)

private:
  Standard_Real myLowerLimitRackDisplacement; //!< optional
  Standard_Real myUpperLimitRackDisplacement; //!< optional
  Standard_Boolean defLowerLimitRackDisplacement; //!< flag "is LowerLimitRackDisplacement defined"
  Standard_Boolean defUpperLimitRackDisplacement; //!< flag "is UpperLimitRackDisplacement defined"

};
#endif // _StepKinematics_RackAndPinionPairWithRange_HeaderFile_
