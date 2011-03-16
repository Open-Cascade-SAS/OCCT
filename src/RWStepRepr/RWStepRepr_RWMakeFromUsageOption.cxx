// File:	RWStepRepr_RWMakeFromUsageOption.cxx
// Created:	Mon Jul  3 20:13:36 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <RWStepRepr_RWMakeFromUsageOption.ixx>

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

  Handle(StepBasic_ProductDefinition) aProductDefinitionRelationship_RelatingProductDefinition;
  data->ReadEntity (num, 4, "product_definition_relationship.relating_product_definition", ach, STANDARD_TYPE(StepBasic_ProductDefinition), aProductDefinitionRelationship_RelatingProductDefinition);

  Handle(StepBasic_ProductDefinition) aProductDefinitionRelationship_RelatedProductDefinition;
  data->ReadEntity (num, 5, "product_definition_relationship.related_product_definition", ach, STANDARD_TYPE(StepBasic_ProductDefinition), aProductDefinitionRelationship_RelatedProductDefinition);

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

  SW.Send (ent->StepBasic_ProductDefinitionRelationship::RelatingProductDefinition());

  SW.Send (ent->StepBasic_ProductDefinitionRelationship::RelatedProductDefinition());

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

  iter.AddItem (ent->StepBasic_ProductDefinitionRelationship::RelatingProductDefinition());

  iter.AddItem (ent->StepBasic_ProductDefinitionRelationship::RelatedProductDefinition());

  // Own fields of MakeFromUsageOption

  iter.AddItem (ent->Quantity());
}
