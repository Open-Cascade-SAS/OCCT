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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0

#include <Standard_Transient.hxx>
#include <StepBasic_ProductDefinition.hxx>
#include <StepBasic_ProductDefinitionFormation.hxx>
#include <StepRepr_ConfigurationDesignItem.hxx>

//=======================================================================
//function : StepRepr_ConfigurationDesignItem
//purpose  : 
//=======================================================================
StepRepr_ConfigurationDesignItem::StepRepr_ConfigurationDesignItem ()
{
}

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepRepr_ConfigurationDesignItem::CaseNum (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinition))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionFormation))) return 2;
  return 0;
}

//=======================================================================
//function : ProductDefinition
//purpose  : 
//=======================================================================

Handle(StepBasic_ProductDefinition) StepRepr_ConfigurationDesignItem::ProductDefinition () const
{
  return Handle(StepBasic_ProductDefinition)::DownCast(Value());
}

//=======================================================================
//function : ProductDefinitionFormation
//purpose  : 
//=======================================================================

Handle(StepBasic_ProductDefinitionFormation) StepRepr_ConfigurationDesignItem::ProductDefinitionFormation () const
{
  return Handle(StepBasic_ProductDefinitionFormation)::DownCast(Value());
}
