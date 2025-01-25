// Created on: 1999-11-26
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _StepShape_ShapeDefinitionRepresentation_HeaderFile
#define _StepShape_ShapeDefinitionRepresentation_HeaderFile

#include <Standard.hxx>

#include <StepRepr_PropertyDefinitionRepresentation.hxx>

class StepShape_ShapeDefinitionRepresentation;
DEFINE_STANDARD_HANDLE(StepShape_ShapeDefinitionRepresentation,
                       StepRepr_PropertyDefinitionRepresentation)

//! Representation of STEP entity ShapeDefinitionRepresentation
class StepShape_ShapeDefinitionRepresentation : public StepRepr_PropertyDefinitionRepresentation
{

public:
  //! Empty constructor
  Standard_EXPORT StepShape_ShapeDefinitionRepresentation();

  DEFINE_STANDARD_RTTIEXT(StepShape_ShapeDefinitionRepresentation,
                          StepRepr_PropertyDefinitionRepresentation)

protected:
private:
};

#endif // _StepShape_ShapeDefinitionRepresentation_HeaderFile
