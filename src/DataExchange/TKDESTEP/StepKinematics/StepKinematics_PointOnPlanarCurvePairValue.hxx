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

#ifndef _StepKinematics_PointOnPlanarCurvePairValue_HeaderFile_
#define _StepKinematics_PointOnPlanarCurvePairValue_HeaderFile_

#include <Standard.hxx>
#include <StepKinematics_PairValue.hxx>

#include <TCollection_HAsciiString.hxx>
#include <StepKinematics_KinematicPair.hxx>
#include <StepGeom_PointOnCurve.hxx>
#include <StepKinematics_SpatialRotation.hxx>

DEFINE_STANDARD_HANDLE(StepKinematics_PointOnPlanarCurvePairValue, StepKinematics_PairValue)

//! Representation of STEP entity PointOnPlanarCurvePairValue
class StepKinematics_PointOnPlanarCurvePairValue : public StepKinematics_PairValue
{
public:
  //! default constructor
  Standard_EXPORT StepKinematics_PointOnPlanarCurvePairValue();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)&     theRepresentationItem_Name,
                            const Handle(StepKinematics_KinematicPair)& thePairValue_AppliesToPair,
                            const Handle(StepGeom_PointOnCurve)&        theActualPointOnCurve,
                            const StepKinematics_SpatialRotation&       theInputOrientation);

  //! Returns field ActualPointOnCurve
  Standard_EXPORT Handle(StepGeom_PointOnCurve) ActualPointOnCurve() const;
  //! Sets field ActualPointOnCurve
  Standard_EXPORT void SetActualPointOnCurve(
    const Handle(StepGeom_PointOnCurve)& theActualPointOnCurve);

  //! Returns field InputOrientation
  Standard_EXPORT StepKinematics_SpatialRotation InputOrientation() const;
  //! Sets field InputOrientation
  Standard_EXPORT void SetInputOrientation(
    const StepKinematics_SpatialRotation& theInputOrientation);

  DEFINE_STANDARD_RTTIEXT(StepKinematics_PointOnPlanarCurvePairValue, StepKinematics_PairValue)

private:
  Handle(StepGeom_PointOnCurve)  myActualPointOnCurve;
  StepKinematics_SpatialRotation myInputOrientation;
};
#endif // _StepKinematics_PointOnPlanarCurvePairValue_HeaderFile_
