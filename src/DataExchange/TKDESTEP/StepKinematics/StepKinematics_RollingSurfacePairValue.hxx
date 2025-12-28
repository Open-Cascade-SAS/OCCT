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

#ifndef _StepKinematics_RollingSurfacePairValue_HeaderFile_
#define _StepKinematics_RollingSurfacePairValue_HeaderFile_

#include <Standard.hxx>
#include <StepKinematics_PairValue.hxx>

#include <TCollection_HAsciiString.hxx>
#include <StepKinematics_KinematicPair.hxx>
#include <StepGeom_PointOnSurface.hxx>

//! Representation of STEP entity RollingSurfacePairValue
class StepKinematics_RollingSurfacePairValue : public StepKinematics_PairValue
{
public:
  //! default constructor
  Standard_EXPORT StepKinematics_RollingSurfacePairValue();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>&     theRepresentationItem_Name,
                            const occ::handle<StepKinematics_KinematicPair>& thePairValue_AppliesToPair,
                            const occ::handle<StepGeom_PointOnSurface>&      theActualPointOnSurface,
                            const double                         theActualRotation);

  //! Returns field ActualPointOnSurface
  Standard_EXPORT occ::handle<StepGeom_PointOnSurface> ActualPointOnSurface() const;
  //! Sets field ActualPointOnSurface
  Standard_EXPORT void SetActualPointOnSurface(
    const occ::handle<StepGeom_PointOnSurface>& theActualPointOnSurface);

  //! Returns field ActualRotation
  Standard_EXPORT double ActualRotation() const;
  //! Sets field ActualRotation
  Standard_EXPORT void SetActualRotation(const double theActualRotation);

  DEFINE_STANDARD_RTTIEXT(StepKinematics_RollingSurfacePairValue, StepKinematics_PairValue)

private:
  occ::handle<StepGeom_PointOnSurface> myActualPointOnSurface;
  double                   myActualRotation;
};
#endif // _StepKinematics_RollingSurfacePairValue_HeaderFile_
