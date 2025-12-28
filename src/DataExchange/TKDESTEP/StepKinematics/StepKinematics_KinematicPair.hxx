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

#ifndef _StepKinematics_KinematicPair_HeaderFile_
#define _StepKinematics_KinematicPair_HeaderFile_

#include <Standard.hxx>
#include <StepGeom_GeometricRepresentationItem.hxx>

#include <StepRepr_RepresentationItem.hxx>
#include <StepRepr_ItemDefinedTransformation.hxx>
#include <StepKinematics_KinematicJoint.hxx>

//! Representation of STEP entity KinematicPair
class StepKinematics_KinematicPair : public StepGeom_GeometricRepresentationItem
{
public:
  //! default constructor
  Standard_EXPORT StepKinematics_KinematicPair();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>&      theRepresentationItem_Name,
    const occ::handle<TCollection_HAsciiString>&      theItemDefinedTransformation_Name,
    const bool                       hasItemDefinedTransformation_Description,
    const occ::handle<TCollection_HAsciiString>&      theItemDefinedTransformation_Description,
    const occ::handle<StepRepr_RepresentationItem>&   theItemDefinedTransformation_TransformItem1,
    const occ::handle<StepRepr_RepresentationItem>&   theItemDefinedTransformation_TransformItem2,
    const occ::handle<StepKinematics_KinematicJoint>& theJoint);

  //! Returns data for supertype ItemDefinedTransformation
  Standard_EXPORT occ::handle<StepRepr_ItemDefinedTransformation> ItemDefinedTransformation() const;
  //! Sets data for supertype ItemDefinedTransformation
  Standard_EXPORT void SetItemDefinedTransformation(
    const occ::handle<StepRepr_ItemDefinedTransformation>& theItemDefinedTransformation);

  //! Returns field Joint
  Standard_EXPORT occ::handle<StepKinematics_KinematicJoint> Joint() const;
  //! Sets field Joint
  Standard_EXPORT void SetJoint(const occ::handle<StepKinematics_KinematicJoint>& theJoint);

  DEFINE_STANDARD_RTTIEXT(StepKinematics_KinematicPair, StepGeom_GeometricRepresentationItem)

private:
  occ::handle<StepRepr_ItemDefinedTransformation> myItemDefinedTransformation; //!< supertype
  occ::handle<StepKinematics_KinematicJoint>      myJoint;
};
#endif // _StepKinematics_KinematicPair_HeaderFile_
