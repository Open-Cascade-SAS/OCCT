// Created on: 2016-08-25
// Created by: Irina KRYLOVA
// Copyright (c) 2016 OPEN CASCADE SAS
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

#ifndef _StepVisual_StepVisual_CharacterizedObjAndRepresentationAndDraughtingModel_HeaderFile
#define _StepVisual_StepVisual_CharacterizedObjAndRepresentationAndDraughtingModel_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepVisual_DraughtingModel.hxx>

class StepVisual_CharacterizedObjAndRepresentationAndDraughtingModel;
DEFINE_STANDARD_HANDLE(StepVisual_CharacterizedObjAndRepresentationAndDraughtingModel,
                       StepVisual_DraughtingModel)

//! Added for Dimensional Tolerances
//! Complex STEP entity Characterized_Object & Characterized_Representation & Draughting_Model &
//! Representation
class StepVisual_CharacterizedObjAndRepresentationAndDraughtingModel
    : public StepVisual_DraughtingModel
{

public:
  Standard_EXPORT StepVisual_CharacterizedObjAndRepresentationAndDraughtingModel();

  DEFINE_STANDARD_RTTIEXT(StepVisual_CharacterizedObjAndRepresentationAndDraughtingModel,
                          StepVisual_DraughtingModel)
};
#endif // _StepVisual_CharacterizedObjAndRepresentationAndDraughtingModel_HeaderFile
