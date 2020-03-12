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

#ifndef _StepKinematics_PlanarCurvePairRange_HeaderFile_
#define _StepKinematics_PlanarCurvePairRange_HeaderFile_

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <StepKinematics_PlanarCurvePair.hxx>

#include <TCollection_HAsciiString.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepRepr_ItemDefinedTransformation.hxx>
#include <StepKinematics_KinematicJoint.hxx>
#include <StepGeom_Curve.hxx>
#include <StepGeom_TrimmedCurve.hxx>

DEFINE_STANDARD_HANDLE(StepKinematics_PlanarCurvePairRange, StepKinematics_PlanarCurvePair)

//! Representation of STEP entity PlanarCurvePairRange
class StepKinematics_PlanarCurvePairRange : public StepKinematics_PlanarCurvePair
{
public :

  //! default constructor
  Standard_EXPORT StepKinematics_PlanarCurvePairRange();

  //! Initialize all fields (own and inherited)
 Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
                           const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Name,
                           const Standard_Boolean hasItemDefinedTransformation_Description,
                           const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Description,
                           const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem1,
                           const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem2,
                           const Handle(StepKinematics_KinematicJoint)& theKinematicPair_Joint,
                           const Handle(StepGeom_Curve)& thePlanarCurvePair_Curve1,
                           const Handle(StepGeom_Curve)& thePlanarCurvePair_Curve2,
                           const Standard_Boolean thePlanarCurvePair_Orientation,
                           const Handle(StepGeom_TrimmedCurve)& theRangeOnCurve1,
                           const Handle(StepGeom_TrimmedCurve)& theRangeOnCurve2);

  //! Returns field RangeOnCurve1
  Standard_EXPORT Handle(StepGeom_TrimmedCurve) RangeOnCurve1() const;
  //! Sets field RangeOnCurve1
  Standard_EXPORT void SetRangeOnCurve1 (const Handle(StepGeom_TrimmedCurve)& theRangeOnCurve1);

  //! Returns field RangeOnCurve2
  Standard_EXPORT Handle(StepGeom_TrimmedCurve) RangeOnCurve2() const;
  //! Sets field RangeOnCurve2
  Standard_EXPORT void SetRangeOnCurve2 (const Handle(StepGeom_TrimmedCurve)& theRangeOnCurve2);

DEFINE_STANDARD_RTTIEXT(StepKinematics_PlanarCurvePairRange, StepKinematics_PlanarCurvePair)

private:
  Handle(StepGeom_TrimmedCurve) myRangeOnCurve1;
  Handle(StepGeom_TrimmedCurve) myRangeOnCurve2;

};
#endif // _StepKinematics_PlanarCurvePairRange_HeaderFile_
