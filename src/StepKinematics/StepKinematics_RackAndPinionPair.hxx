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

#ifndef _StepKinematics_RackAndPinionPair_HeaderFile_
#define _StepKinematics_RackAndPinionPair_HeaderFile_

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <StepKinematics_LowOrderKinematicPairWithMotionCoupling.hxx>

#include <TCollection_HAsciiString.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepRepr_ItemDefinedTransformation.hxx>
#include <StepKinematics_KinematicJoint.hxx>

DEFINE_STANDARD_HANDLE(StepKinematics_RackAndPinionPair, StepKinematics_LowOrderKinematicPairWithMotionCoupling)

//! Representation of STEP entity RackAndPinionPair
class StepKinematics_RackAndPinionPair : public StepKinematics_LowOrderKinematicPairWithMotionCoupling
{
public :

  //! default constructor
  Standard_EXPORT StepKinematics_RackAndPinionPair();

  //! Initialize all fields (own and inherited)
 Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
                           const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Name,
                           const Standard_Boolean hasItemDefinedTransformation_Description,
                           const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Description,
                           const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem1,
                           const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem2,
                           const Handle(StepKinematics_KinematicJoint)& theKinematicPair_Joint,
                           const Standard_Real thePinionRadius);

  //! Returns field PinionRadius
  Standard_EXPORT Standard_Real PinionRadius() const;
  //! Sets field PinionRadius
  Standard_EXPORT void SetPinionRadius (const Standard_Real thePinionRadius);

DEFINE_STANDARD_RTTIEXT(StepKinematics_RackAndPinionPair, StepKinematics_LowOrderKinematicPairWithMotionCoupling)

private:
  Standard_Real myPinionRadius;

};
#endif // _StepKinematics_RackAndPinionPair_HeaderFile_
