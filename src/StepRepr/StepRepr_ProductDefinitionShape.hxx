// Created on: 2000-07-03
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _StepRepr_ProductDefinitionShape_HeaderFile
#define _StepRepr_ProductDefinitionShape_HeaderFile

#include <Standard.hxx>

#include <StepRepr_PropertyDefinition.hxx>

class StepRepr_ProductDefinitionShape;
DEFINE_STANDARD_HANDLE(StepRepr_ProductDefinitionShape, StepRepr_PropertyDefinition)

//! Representation of STEP entity ProductDefinitionShape
class StepRepr_ProductDefinitionShape : public StepRepr_PropertyDefinition
{

public:
  //! Empty constructor
  Standard_EXPORT StepRepr_ProductDefinitionShape();

  DEFINE_STANDARD_RTTIEXT(StepRepr_ProductDefinitionShape, StepRepr_PropertyDefinition)

protected:
private:
};

#endif // _StepRepr_ProductDefinitionShape_HeaderFile
