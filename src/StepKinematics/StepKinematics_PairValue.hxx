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

#ifndef _StepKinematics_PairValue_HeaderFile_
#define _StepKinematics_PairValue_HeaderFile_

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <StepGeom_GeometricRepresentationItem.hxx>

#include <TCollection_HAsciiString.hxx>
#include <StepKinematics_KinematicPair.hxx>

DEFINE_STANDARD_HANDLE(StepKinematics_PairValue, StepGeom_GeometricRepresentationItem)

//! Representation of STEP entity PairValue
class StepKinematics_PairValue : public StepGeom_GeometricRepresentationItem
{
public :

  //! default constructor
  Standard_EXPORT StepKinematics_PairValue();

  //! Initialize all fields (own and inherited)
 Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
                           const Handle(StepKinematics_KinematicPair)& theAppliesToPair);

  //! Returns field AppliesToPair
  Standard_EXPORT Handle(StepKinematics_KinematicPair) AppliesToPair() const;
  //! Sets field AppliesToPair
  Standard_EXPORT void SetAppliesToPair (const Handle(StepKinematics_KinematicPair)& theAppliesToPair);

DEFINE_STANDARD_RTTIEXT(StepKinematics_PairValue, StepGeom_GeometricRepresentationItem)

private:
  Handle(StepKinematics_KinematicPair) myAppliesToPair;

};
#endif // _StepKinematics_PairValue_HeaderFile_
