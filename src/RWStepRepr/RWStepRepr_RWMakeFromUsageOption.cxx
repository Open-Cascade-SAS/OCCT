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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepRepr_RWMakeFromUsageOption.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepBasic_ProductDefinition.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_MakeFromUsageOption.hxx>

//=======================================================================
//function : RWStepRepr_RWMakeFromUsageOption
//purpose  : 
//=======================================================================
RWStepRepr_RWMakeFromUsageOption::RWStepRepr_RWMakeFromUsageOption ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWMakeFromUsageOption::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                 const Standard_Integer num,
                                                 Handle(Interface_Check)& ach,
                                                 const Handle(StepRepr_MakeFromUsageOption) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,8,ach,"make_from_usage_option") ) return;

  // Inherited fields of ProductDefinitionRelationship

  Handle(TCollection_HAsciiString) aProductDefinitionRelationship_Id;
  data->ReadString (num, 1, "product_definition_relationship.id", ach, aProductDefinitionRelationship_Id);

  Handle(TCollection_HAsciiString) aProductDefinitionRelationship_Name;
  data->ReadString (num, 2, "product_definition_relationship.name", ach, aProductDefinitionRelationship_Name);

  Handle(TCollection_HAsciiString) aProductDefinitionRelationship_Description;
  Standard_Boolean hasProductDefinitionRelationship_Description = Standard_True;
  if ( data->IsParamDefined (num,3) ) {
    data->ReadString (num, 3, "product_definition_relationship.description", ach, aProductDefinitionRelationship_Description);
  }
  else {
    hasProductDefinitionRelationship_Description = Standard_False;
  }

  StepBasic_ProductDefinitionOrReference aProductDefinitionRelationship_RelatingProductDefinition;
  data->ReadEntity (num, 4, "product_definition_relationship.relating_product_definition", ach, aProductDefinitionRelationship_RelatingProductDefinition);

  StepBasic_ProductDefinitionOrReference aProductDefinitionRelationship_RelatedProductDefinition;
  data->ReadEntity (num, 5, "product_definition_relationship.related_product_definition", ach, aProductDefinitionRelationship_RelatedProductDefinition);

  // Own fields of MakeFromUsageOption

  Standard_Integer aRanking;
  data->ReadInteger (num, 6, "ranking", ach, aRanking);

  Handle(TCollection_HAsciiString) aRankingRationale;
  data->ReadString (num, 7, "ranking_rationale", ach, aRankingRationale);

  Handle(StepBasic_MeasureWithUnit) aQuantity;
  data->ReadEntity (num, 8, "quantity", ach, STANDARD_TYPE(StepBasic_MeasureWithUnit), aQuantity);

  // Initialize entity
  ent->Init(aProductDefinitionRelationship_Id,
            aProductDefinitionRelationship_Name,
            hasProductDefinitionRelationship_Description,
            aProductDefinitionRelationship_Description,
            aProductDefinitionRelationship_RelatingProductDefinition,
            aProductDefinitionRelationship_RelatedProductDefinition,
            aRanking,
            aRankingRationale,
            aQuantity);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWMakeFromUsageOption::WriteStep (StepData_StepWriter& SW,
                                                  const Handle(StepRepr_MakeFromUsageOption) &ent) const
{

  // Inherited fields of ProductDefinitionRelationship

  SW.Send (ent->StepBasic_ProductDefinitionRelationship::Id());

  SW.Send (ent->StepBasic_ProductDefinitionRelationship::Name());

  if ( ent->StepBasic_ProductDefinitionRelationship::HasDescription() ) {
    SW.Send (ent->StepBasic_ProductDefinitionRelationship::Description());
  }
  else SW.SendUndef();

  SW.Send (ent->StepBasic_ProductDefinitionRelationship::RelatingProductDefinitionAP242().Value());

  SW.Send (ent->StepBasic_ProductDefinitionRelationship::RelatedProductDefinitionAP242().Value());

  // Own fields of MakeFromUsageOption

  SW.Send (ent->Ranking());

  SW.Send (ent->RankingRationale());

  SW.Send (ent->Quantity());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepRepr_RWMakeFromUsageOption::Share (const Handle(StepRepr_MakeFromUsageOption) &ent,
                                              Interface_EntityIterator& iter) const
{

  // Inherited fields of ProductDefinitionRelationship

  iter.AddItem (ent->StepBasic_ProductDefinitionRelationship::RelatingProductDefinitionAP242().Value());

  iter.AddItem (ent->StepBasic_ProductDefinitionRelationship::RelatedProductDefinitionAP242().Value());

  // Own fields of MakeFromUsageOption

  iter.AddItem (ent->Quantity());
}
