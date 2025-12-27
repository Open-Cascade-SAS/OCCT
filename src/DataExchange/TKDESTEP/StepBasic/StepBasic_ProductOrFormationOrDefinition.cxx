// Created on: 2003-01-28
// Created by: data exchange team
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <Standard_Transient.hxx>
#include <StepBasic_Product.hxx>
#include <StepBasic_ProductDefinition.hxx>
#include <StepBasic_ProductDefinitionFormation.hxx>
#include <StepBasic_ProductOrFormationOrDefinition.hxx>

//=================================================================================================

StepBasic_ProductOrFormationOrDefinition::StepBasic_ProductOrFormationOrDefinition() {}

//=================================================================================================

int StepBasic_ProductOrFormationOrDefinition::CaseNum(
  const occ::handle<Standard_Transient>& ent) const
{
  if (ent.IsNull())
    return 0;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_Product)))
    return 1;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionFormation)))
    return 2;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinition)))
    return 3;
  return 0;
}

//=================================================================================================

occ::handle<StepBasic_Product> StepBasic_ProductOrFormationOrDefinition::Product() const
{
  return occ::down_cast<StepBasic_Product>(Value());
}

//=================================================================================================

occ::handle<StepBasic_ProductDefinitionFormation> StepBasic_ProductOrFormationOrDefinition::
  ProductDefinitionFormation() const
{
  return occ::down_cast<StepBasic_ProductDefinitionFormation>(Value());
}

//=================================================================================================

occ::handle<StepBasic_ProductDefinition> StepBasic_ProductOrFormationOrDefinition::ProductDefinition()
  const
{
  return occ::down_cast<StepBasic_ProductDefinition>(Value());
}
