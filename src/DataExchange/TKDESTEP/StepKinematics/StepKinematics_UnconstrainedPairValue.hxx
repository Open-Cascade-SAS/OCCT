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

#ifndef _StepKinematics_UnconstrainedPairValue_HeaderFile_
#define _StepKinematics_UnconstrainedPairValue_HeaderFile_

#include <Standard.hxx>
#include <StepKinematics_PairValue.hxx>

#include <TCollection_HAsciiString.hxx>
#include <StepKinematics_KinematicPair.hxx>
#include <StepGeom_Axis2Placement3d.hxx>

//! Representation of STEP entity UnconstrainedPairValue
class StepKinematics_UnconstrainedPairValue : public StepKinematics_PairValue
{
public:
  //! default constructor
  Standard_EXPORT StepKinematics_UnconstrainedPairValue();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>&     theRepresentationItem_Name,
    const occ::handle<StepKinematics_KinematicPair>& thePairValue_AppliesToPair,
    const occ::handle<StepGeom_Axis2Placement3d>&    theActualPlacement);

  //! Returns field ActualPlacement
  Standard_EXPORT occ::handle<StepGeom_Axis2Placement3d> ActualPlacement() const;
  //! Sets field ActualPlacement
  Standard_EXPORT void SetActualPlacement(
    const occ::handle<StepGeom_Axis2Placement3d>& theActualPlacement);

  DEFINE_STANDARD_RTTIEXT(StepKinematics_UnconstrainedPairValue, StepKinematics_PairValue)

private:
  occ::handle<StepGeom_Axis2Placement3d> myActualPlacement;
};
#endif // _StepKinematics_UnconstrainedPairValue_HeaderFile_
