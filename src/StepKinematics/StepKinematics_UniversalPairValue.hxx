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

#ifndef _StepKinematics_UniversalPairValue_HeaderFile_
#define _StepKinematics_UniversalPairValue_HeaderFile_

#include <Standard.hxx>
#include <StepKinematics_PairValue.hxx>

#include <TCollection_HAsciiString.hxx>
#include <StepKinematics_KinematicPair.hxx>

DEFINE_STANDARD_HANDLE(StepKinematics_UniversalPairValue, StepKinematics_PairValue)

//! Representation of STEP entity UniversalPairValue
class StepKinematics_UniversalPairValue : public StepKinematics_PairValue
{
public :

  //! default constructor
  Standard_EXPORT StepKinematics_UniversalPairValue();

  //! Initialize all fields (own and inherited)
 Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
                           const Handle(StepKinematics_KinematicPair)& thePairValue_AppliesToPair,
                           const Standard_Real theFirstRotationAngle,
                           const Standard_Real theSecondRotationAngle);

  //! Returns field FirstRotationAngle
  Standard_EXPORT Standard_Real FirstRotationAngle() const;
  //! Sets field FirstRotationAngle
  Standard_EXPORT void SetFirstRotationAngle (const Standard_Real theFirstRotationAngle);

  //! Returns field SecondRotationAngle
  Standard_EXPORT Standard_Real SecondRotationAngle() const;
  //! Sets field SecondRotationAngle
  Standard_EXPORT void SetSecondRotationAngle (const Standard_Real theSecondRotationAngle);

DEFINE_STANDARD_RTTIEXT(StepKinematics_UniversalPairValue, StepKinematics_PairValue)

private:
  Standard_Real myFirstRotationAngle;
  Standard_Real mySecondRotationAngle;

};
#endif // _StepKinematics_UniversalPairValue_HeaderFile_
