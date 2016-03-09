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

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepBasic_RWProductDefinitionReferenceWithLocalRepresentation.hxx>
#include <StepBasic_ExternalSource.hxx>
#include <StepBasic_ProductDefinitionReferenceWithLocalRepresentation.hxx>
#include <StepBasic_ProductDefinitionContext.hxx>
#include <StepBasic_ProductDefinitionFormation.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

//=======================================================================
//function : RWStepBasic_RWProductDefinitionReferenceWithLocalRepresentation
//purpose  : 
//=======================================================================
RWStepBasic_RWProductDefinitionReferenceWithLocalRepresentation::
  RWStepBasic_RWProductDefinitionReferenceWithLocalRepresentation () {}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================
void RWStepBasic_RWProductDefinitionReferenceWithLocalRepresentation::ReadStep
  (const Handle(StepData_StepReaderData)& data,
   const Standard_Integer num,
   Handle(Interface_Check)& ach,
   const Handle(StepBasic_ProductDefinitionReferenceWithLocalRepresentation)& ent) const
{
  // Number of Parameter Control
  if (!data->CheckNbParams(num, 5, ach, "product_definition_reference_with_local_representation")) return;

  // Own field source
  Handle(StepBasic_ExternalSource) aSource;
  data->ReadEntity(num, 1,"source", ach, STANDARD_TYPE(StepBasic_ExternalSource), aSource);

  // Inherited field : id
  Handle(TCollection_HAsciiString) aId;
  data->ReadString (num, 2, "id", ach, aId);
  
  // Inherited field : description
  Handle(TCollection_HAsciiString) aDescription;
  if (data->IsParamDefined (num, 3)) {
    data->ReadString (num, 3, "description", ach, aDescription);
  }

  // Inherited field : formation
  Handle(StepBasic_ProductDefinitionFormation) aFormation;
  data->ReadEntity(num, 4, "formation", ach, STANDARD_TYPE(StepBasic_ProductDefinitionFormation), aFormation);
  
  // Inherited : frame_of_reference
  Handle(StepBasic_ProductDefinitionContext) aFrameOfReference;
  data->ReadEntity(num, 5,"frame_of_reference", ach, STANDARD_TYPE(StepBasic_ProductDefinitionContext), aFrameOfReference);
  
  //  Initialisation of the read entity
  ent->Init(aSource, aId, aDescription, aFormation, aFrameOfReference);
}


//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================
void RWStepBasic_RWProductDefinitionReferenceWithLocalRepresentation::WriteStep
  (StepData_StepWriter& SW,
   const Handle(StepBasic_ProductDefinitionReferenceWithLocalRepresentation)& ent) const
{
  // Own field : source
  SW.Send(ent->Source());

  // Inherited field : id
  SW.Send(ent->Id());
  
  // Inherited field : description
  SW.Send(ent->Description());
  
  // Inherited field : formation
  SW.Send(ent->Formation());
  
  // Inherited field : frame_of_reference
  SW.Send(ent->FrameOfReference());
}


//=======================================================================
//function : Share
//purpose  : 
//=======================================================================
void RWStepBasic_RWProductDefinitionReferenceWithLocalRepresentation::Share(
  const Handle(StepBasic_ProductDefinitionReferenceWithLocalRepresentation)& ent,
  Interface_EntityIterator& iter) const
{
  iter.GetOneItem(ent->Source());
  iter.GetOneItem(ent->Formation());
  iter.GetOneItem(ent->FrameOfReference());
}

