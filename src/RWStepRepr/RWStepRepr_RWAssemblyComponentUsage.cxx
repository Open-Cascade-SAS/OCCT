// File:	RWStepRepr_RWAssemblyComponentUsage.cxx
// Created:	Mon Jul  3 19:47:50 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <RWStepRepr_RWAssemblyComponentUsage.ixx>

//=======================================================================
//function : RWStepRepr_RWAssemblyComponentUsage
//purpose  : 
//=======================================================================

RWStepRepr_RWAssemblyComponentUsage::RWStepRepr_RWAssemblyComponentUsage ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWAssemblyComponentUsage::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                    const Standard_Integer num,
                                                    Handle(Interface_Check)& ach,
                                                    const Handle(StepRepr_AssemblyComponentUsage) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,6,ach,"assembly_component_usage") ) return;

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

  // Own fields of AssemblyComponentUsage

  Handle(TCollection_HAsciiString) aReferenceDesignator;
  Standard_Boolean hasReferenceDesignator = Standard_True;
  if ( data->IsParamDefined (num,6) ) {
    data->ReadString (num, 6, "reference_designator", ach, aReferenceDesignator);
  }
  else {
    hasReferenceDesignator = Standard_False;
  }

  // Initialize entity
  ent->Init(aProductDefinitionRelationship_Id,
            aProductDefinitionRelationship_Name,
            hasProductDefinitionRelationship_Description,
            aProductDefinitionRelationship_Description,
            aProductDefinitionRelationship_RelatingProductDefinition,
            aProductDefinitionRelationship_RelatedProductDefinition,
            hasReferenceDesignator,
            aReferenceDesignator);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWAssemblyComponentUsage::WriteStep (StepData_StepWriter& SW,
                                                     const Handle(StepRepr_AssemblyComponentUsage) &ent) const
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

  // Own fields of AssemblyComponentUsage

  if ( ent->HasReferenceDesignator() ) {
    SW.Send (ent->ReferenceDesignator());
  }
  else SW.SendUndef();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepRepr_RWAssemblyComponentUsage::Share (const Handle(StepRepr_AssemblyComponentUsage) &ent,
                                                 Interface_EntityIterator& iter) const
{

  // Inherited fields of ProductDefinitionRelationship

  iter.AddItem (ent->StepBasic_ProductDefinitionRelationship::RelatingProductDefinition());

  iter.AddItem (ent->StepBasic_ProductDefinitionRelationship::RelatedProductDefinition());

  // Own fields of AssemblyComponentUsage
}
