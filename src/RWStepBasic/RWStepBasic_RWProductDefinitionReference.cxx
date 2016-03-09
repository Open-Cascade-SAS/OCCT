// Created on: 2016-03-31
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

//gka 05.03.99 S4134 upgrade from CD to DIS

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepBasic_RWProductDefinitionReference.hxx>
#include <StepBasic_ExternalSource.hxx>
#include <StepBasic_ProductDefinitionReference.hxx>
#include <StepBasic_ProductDefinitionContext.hxx>
#include <StepBasic_ProductDefinitionFormation.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

//=======================================================================
//function : StepBasic_ProductDefinitionReference
//purpose  : 
//=======================================================================
RWStepBasic_RWProductDefinitionReference::RWStepBasic_RWProductDefinitionReference () {}

//=======================================================================
//function : StepBasic_ProductDefinitionReference
//purpose  : 
//=======================================================================
void RWStepBasic_RWProductDefinitionReference::ReadStep
  (const Handle(StepData_StepReaderData)& data,
   const Standard_Integer num,
   Handle(Interface_Check)& ach,
   const Handle(StepBasic_ProductDefinitionReference)& ent) const
{
  // Number of Parameter Control
  if (!data->CheckNbParams(num, 5, ach, "product_definition_reference")) return;

  // Own field source
  Handle(StepBasic_ExternalSource) aSource;
  data->ReadEntity(num, 1,"source", ach, STANDARD_TYPE(StepBasic_ExternalSource), aSource);

  // Own field : product_id
  Handle(TCollection_HAsciiString) aProductId;
  data->ReadString (num, 2, "product_id", ach, aProductId);

  // Own field : product_definition_formation_id
  Handle(TCollection_HAsciiString) aProductDefinitionFormationId;
  data->ReadString (num, 3, "product_definition_formation_id", ach, aProductDefinitionFormationId);

  // Own field : product_definition_id
  Handle(TCollection_HAsciiString) aProductDefinitionId;
  data->ReadString (num, 4, "product_definition_id", ach, aProductDefinitionId);

  // Own field : id_owning_organization_name
  Handle(TCollection_HAsciiString) aIdOwningOrganizationName;
  if (data->IsParamDefined (num, 5)) {
    data->ReadString (num, 5, "id_owning_organization_name", ach, aIdOwningOrganizationName);
  }

  //--- Initialisation of the read entity ---
  ent->Init(aSource, aProductId, aProductDefinitionFormationId, aProductDefinitionId, aIdOwningOrganizationName);
}


//=======================================================================
//function : StepBasic_ProductDefinitionReference
//purpose  : 
//=======================================================================
void RWStepBasic_RWProductDefinitionReference::WriteStep
  (StepData_StepWriter& SW,
   const Handle(StepBasic_ProductDefinitionReference)& ent) const
{

 // Own field : source
 SW.Send(ent->Source());
 
 // Own field : product_id
 SW.Send(ent->ProductId());
 
 // Own field : product_definition_formation_id
 SW.Send(ent->ProductDefinitionFormationId());
 
 // Own field : product_definition_id
 SW.Send(ent->ProductDefinitionId());
 
 // Own field : id_owning_organization_name
 if (ent->HasIdOwningOrganizationName())
   SW.Send(ent->IdOwningOrganizationName());
 else
   SW.SendUndef();
}


//=======================================================================
//function : StepBasic_ProductDefinitionReference
//purpose  : 
//=======================================================================
void RWStepBasic_RWProductDefinitionReference::Share(const Handle(StepBasic_ProductDefinitionReference)& ent, Interface_EntityIterator& iter) const
{
  iter.GetOneItem(ent->Source());
}

