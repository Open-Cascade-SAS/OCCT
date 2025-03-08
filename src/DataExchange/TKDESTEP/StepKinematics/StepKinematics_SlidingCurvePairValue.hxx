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

#ifndef _StepKinematics_SlidingCurvePairValue_HeaderFile_
#define _StepKinematics_SlidingCurvePairValue_HeaderFile_

#include <Standard.hxx>
#include <StepKinematics_PairValue.hxx>

#include <TCollection_HAsciiString.hxx>
#include <StepKinematics_KinematicPair.hxx>
#include <StepGeom_PointOnCurve.hxx>

DEFINE_STANDARD_HANDLE(StepKinematics_SlidingCurvePairValue, StepKinematics_PairValue)

//! Representation of STEP entity SlidingCurvePairValue
class StepKinematics_SlidingCurvePairValue : public StepKinematics_PairValue
{
public:
  //! default constructor
  Standard_EXPORT StepKinematics_SlidingCurvePairValue();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)&     theRepresentationItem_Name,
                            const Handle(StepKinematics_KinematicPair)& thePairValue_AppliesToPair,
                            const Handle(StepGeom_PointOnCurve)&        theActualPointOnCurve1,
                            const Handle(StepGeom_PointOnCurve)&        theActualPointOnCurve2);

  //! Returns field ActualPointOnCurve1
  Standard_EXPORT Handle(StepGeom_PointOnCurve) ActualPointOnCurve1() const;
  //! Sets field ActualPointOnCurve1
  Standard_EXPORT void SetActualPointOnCurve1(
    const Handle(StepGeom_PointOnCurve)& theActualPointOnCurve1);

  //! Returns field ActualPointOnCurve2
  Standard_EXPORT Handle(StepGeom_PointOnCurve) ActualPointOnCurve2() const;
  //! Sets field ActualPointOnCurve2
  Standard_EXPORT void SetActualPointOnCurve2(
    const Handle(StepGeom_PointOnCurve)& theActualPointOnCurve2);

  DEFINE_STANDARD_RTTIEXT(StepKinematics_SlidingCurvePairValue, StepKinematics_PairValue)

private:
  Handle(StepGeom_PointOnCurve) myActualPointOnCurve1;
  Handle(StepGeom_PointOnCurve) myActualPointOnCurve2;
};
#endif // _StepKinematics_SlidingCurvePairValue_HeaderFile_
