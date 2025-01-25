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

#ifndef _StepKinematics_PlanarPairValue_HeaderFile_
#define _StepKinematics_PlanarPairValue_HeaderFile_

#include <Standard.hxx>
#include <StepKinematics_PairValue.hxx>

#include <TCollection_HAsciiString.hxx>
#include <StepKinematics_KinematicPair.hxx>

DEFINE_STANDARD_HANDLE(StepKinematics_PlanarPairValue, StepKinematics_PairValue)

//! Representation of STEP entity PlanarPairValue
class StepKinematics_PlanarPairValue : public StepKinematics_PairValue
{
public:
  //! default constructor
  Standard_EXPORT StepKinematics_PlanarPairValue();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)&     theRepresentationItem_Name,
                            const Handle(StepKinematics_KinematicPair)& thePairValue_AppliesToPair,
                            const Standard_Real                         theActualRotation,
                            const Standard_Real                         theActualTranslationX,
                            const Standard_Real                         theActualTranslationY);

  //! Returns field ActualRotation
  Standard_EXPORT Standard_Real ActualRotation() const;
  //! Sets field ActualRotation
  Standard_EXPORT void SetActualRotation(const Standard_Real theActualRotation);

  //! Returns field ActualTranslationX
  Standard_EXPORT Standard_Real ActualTranslationX() const;
  //! Sets field ActualTranslationX
  Standard_EXPORT void SetActualTranslationX(const Standard_Real theActualTranslationX);

  //! Returns field ActualTranslationY
  Standard_EXPORT Standard_Real ActualTranslationY() const;
  //! Sets field ActualTranslationY
  Standard_EXPORT void SetActualTranslationY(const Standard_Real theActualTranslationY);

  DEFINE_STANDARD_RTTIEXT(StepKinematics_PlanarPairValue, StepKinematics_PairValue)

private:
  Standard_Real myActualRotation;
  Standard_Real myActualTranslationX;
  Standard_Real myActualTranslationY;
};
#endif // _StepKinematics_PlanarPairValue_HeaderFile_
