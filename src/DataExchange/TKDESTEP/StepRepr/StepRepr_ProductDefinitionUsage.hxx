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

#ifndef _StepRepr_ProductDefinitionUsage_HeaderFile
#define _StepRepr_ProductDefinitionUsage_HeaderFile

#include <Standard.hxx>

#include <StepBasic_ProductDefinitionRelationship.hxx>

class StepRepr_ProductDefinitionUsage;
DEFINE_STANDARD_HANDLE(StepRepr_ProductDefinitionUsage, StepBasic_ProductDefinitionRelationship)

//! Representation of STEP entity ProductDefinitionUsage
class StepRepr_ProductDefinitionUsage : public StepBasic_ProductDefinitionRelationship
{

public:
  //! Empty constructor
  Standard_EXPORT StepRepr_ProductDefinitionUsage();

  DEFINE_STANDARD_RTTIEXT(StepRepr_ProductDefinitionUsage, StepBasic_ProductDefinitionRelationship)

protected:
private:
};

#endif // _StepRepr_ProductDefinitionUsage_HeaderFile
