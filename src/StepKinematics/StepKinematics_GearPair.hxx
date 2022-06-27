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

#ifndef _StepKinematics_GearPair_HeaderFile_
#define _StepKinematics_GearPair_HeaderFile_

#include <Standard.hxx>
#include <StepKinematics_LowOrderKinematicPairWithMotionCoupling.hxx>

#include <TCollection_HAsciiString.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepKinematics_KinematicJoint.hxx>

DEFINE_STANDARD_HANDLE(StepKinematics_GearPair, StepKinematics_LowOrderKinematicPairWithMotionCoupling)

//! Representation of STEP entity GearPair
class StepKinematics_GearPair : public StepKinematics_LowOrderKinematicPairWithMotionCoupling
{
public :

  //! default constructor
  Standard_EXPORT StepKinematics_GearPair();

  //! Initialize all fields (own and inherited)
 Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
                           const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Name,
                           const Standard_Boolean hasItemDefinedTransformation_Description,
                           const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Description,
                           const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem1,
                           const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem2,
                           const Handle(StepKinematics_KinematicJoint)& theKinematicPair_Joint,
                           const Standard_Real theRadiusFirstLink,
                           const Standard_Real theRadiusSecondLink,
                           const Standard_Real theBevel,
                           const Standard_Real theHelicalAngle,
                           const Standard_Real theGearRatio);

  //! Returns field RadiusFirstLink
  Standard_EXPORT Standard_Real RadiusFirstLink() const;
  //! Sets field RadiusFirstLink
  Standard_EXPORT void SetRadiusFirstLink (const Standard_Real theRadiusFirstLink);

  //! Returns field RadiusSecondLink
  Standard_EXPORT Standard_Real RadiusSecondLink() const;
  //! Sets field RadiusSecondLink
  Standard_EXPORT void SetRadiusSecondLink (const Standard_Real theRadiusSecondLink);

  //! Returns field Bevel
  Standard_EXPORT Standard_Real Bevel() const;
  //! Sets field Bevel
  Standard_EXPORT void SetBevel (const Standard_Real theBevel);

  //! Returns field HelicalAngle
  Standard_EXPORT Standard_Real HelicalAngle() const;
  //! Sets field HelicalAngle
  Standard_EXPORT void SetHelicalAngle (const Standard_Real theHelicalAngle);

  //! Returns field GearRatio
  Standard_EXPORT Standard_Real GearRatio() const;
  //! Sets field GearRatio
  Standard_EXPORT void SetGearRatio (const Standard_Real theGearRatio);

DEFINE_STANDARD_RTTIEXT(StepKinematics_GearPair, StepKinematics_LowOrderKinematicPairWithMotionCoupling)

private:
  Standard_Real myRadiusFirstLink;
  Standard_Real myRadiusSecondLink;
  Standard_Real myBevel;
  Standard_Real myHelicalAngle;
  Standard_Real myGearRatio;

};
#endif // _StepKinematics_GearPair_HeaderFile_
