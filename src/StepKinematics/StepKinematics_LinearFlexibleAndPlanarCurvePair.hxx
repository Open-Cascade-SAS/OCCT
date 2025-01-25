// Created on : Fri May 08 19:02:07 2020
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

#ifndef _StepKinematics_LinearFlexibleAndPlanarCurvePair_HeaderFile_
#define _StepKinematics_LinearFlexibleAndPlanarCurvePair_HeaderFile_

#include <Standard.hxx>
#include <StepKinematics_HighOrderKinematicPair.hxx>

#include <TCollection_HAsciiString.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepKinematics_KinematicJoint.hxx>
#include <StepGeom_Curve.hxx>

DEFINE_STANDARD_HANDLE(StepKinematics_LinearFlexibleAndPlanarCurvePair,
                       StepKinematics_HighOrderKinematicPair)

//! Representation of STEP entity LinearFlexibleAndPlanarCurvePair
class StepKinematics_LinearFlexibleAndPlanarCurvePair : public StepKinematics_HighOrderKinematicPair
{
public:
  //! default constructor
  Standard_EXPORT StepKinematics_LinearFlexibleAndPlanarCurvePair();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const Handle(TCollection_HAsciiString)&      theRepresentationItem_Name,
    const Handle(TCollection_HAsciiString)&      theItemDefinedTransformation_Name,
    const Standard_Boolean                       hasItemDefinedTransformation_Description,
    const Handle(TCollection_HAsciiString)&      theItemDefinedTransformation_Description,
    const Handle(StepRepr_RepresentationItem)&   theItemDefinedTransformation_TransformItem1,
    const Handle(StepRepr_RepresentationItem)&   theItemDefinedTransformation_TransformItem2,
    const Handle(StepKinematics_KinematicJoint)& theKinematicPair_Joint,
    const Handle(StepGeom_Curve)&                thePairCurve,
    const Standard_Boolean                       theOrientation);

  //! Returns field PairCurve
  Standard_EXPORT Handle(StepGeom_Curve) PairCurve() const;
  //! Sets field PairCurve
  Standard_EXPORT void SetPairCurve(const Handle(StepGeom_Curve)& thePairCurve);

  //! Returns field Orientation
  Standard_EXPORT Standard_Boolean Orientation() const;
  //! Sets field Orientation
  Standard_EXPORT void SetOrientation(const Standard_Boolean theOrientation);

  DEFINE_STANDARD_RTTIEXT(StepKinematics_LinearFlexibleAndPlanarCurvePair,
                          StepKinematics_HighOrderKinematicPair)

private:
  Handle(StepGeom_Curve) myPairCurve;
  Standard_Boolean       myOrientation;
};
#endif // _StepKinematics_LinearFlexibleAndPlanarCurvePair_HeaderFile_
