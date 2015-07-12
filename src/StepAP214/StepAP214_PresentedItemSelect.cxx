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


#include <Interface_Macros.hxx>
#include <Standard_Transient.hxx>
#include <StepAP214_PresentedItemSelect.hxx>
#include <StepBasic_ProductDefinition.hxx>
#include <StepBasic_ProductDefinitionRelationship.hxx>

StepAP214_PresentedItemSelect::StepAP214_PresentedItemSelect ()    {  }

Standard_Integer  StepAP214_PresentedItemSelect::CaseNum (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinition))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionRelationship))) return 2;
  return 0;
}

Handle(StepBasic_ProductDefinition)  StepAP214_PresentedItemSelect::ProductDefinition () const
{
  return GetCasted(StepBasic_ProductDefinition,Value());
}

Handle(StepBasic_ProductDefinitionRelationship)  StepAP214_PresentedItemSelect::ProductDefinitionRelationship () const
{
  return GetCasted(StepBasic_ProductDefinitionRelationship,Value());
}

