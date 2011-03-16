// File:	RWStepBasic_RWProductDefinitionRelationship.cxx
// Created:	Mon Jul  3 19:47:51 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <RWStepBasic_RWProductDefinitionRelationship.ixx>

//=======================================================================
//function : RWStepBasic_RWProductDefinitionRelationship
//purpose  : 
//=======================================================================

RWStepBasic_RWProductDefinitionRelationship::RWStepBasic_RWProductDefinitionRelationship ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWProductDefinitionRelationship::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                            const Standard_Integer num,
                                                            Handle(Interface_Check)& ach,
                                                            const Handle(StepBasic_ProductDefinitionRelationship) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,5,ach,"product_definition_relationship") ) return;

  // Own fields of ProductDefinitionRelationship

  Handle(TCollection_HAsciiString) aId;
  data->ReadString (num, 1, "id", ach, aId);

  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num, 2, "name", ach, aName);

  Handle(TCollection_HAsciiString) aDescription;
  Standard_Boolean hasDescription = Standard_True;
  if ( data->IsParamDefined (num,3) ) {
    data->ReadString (num, 3, "description", ach, aDescription);
  }
  else {
    hasDescription = Standard_False;
  }

  Handle(StepBasic_ProductDefinition) aRelatingProductDefinition;
  data->ReadEntity (num, 4, "relating_product_definition", ach, STANDARD_TYPE(StepBasic_ProductDefinition), aRelatingProductDefinition);

  Handle(StepBasic_ProductDefinition) aRelatedProductDefinition;
  data->ReadEntity (num, 5, "related_product_definition", ach, STANDARD_TYPE(StepBasic_ProductDefinition), aRelatedProductDefinition);

  // Initialize entity
  ent->Init(aId,
            aName,
            hasDescription,
            aDescription,
            aRelatingProductDefinition,
            aRelatedProductDefinition);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWProductDefinitionRelationship::WriteStep (StepData_StepWriter& SW,
                                                             const Handle(StepBasic_ProductDefinitionRelationship) &ent) const
{

  // Own fields of ProductDefinitionRelationship

  SW.Send (ent->Id());

  SW.Send (ent->Name());

  if ( ent->HasDescription() ) {
    SW.Send (ent->Description());
  }
  else SW.SendUndef();

  SW.Send (ent->RelatingProductDefinition());

  SW.Send (ent->RelatedProductDefinition());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWProductDefinitionRelationship::Share (const Handle(StepBasic_ProductDefinitionRelationship) &ent,
                                                         Interface_EntityIterator& iter) const
{

  // Own fields of ProductDefinitionRelationship

  iter.AddItem (ent->RelatingProductDefinition());

  iter.AddItem (ent->RelatedProductDefinition());
}
