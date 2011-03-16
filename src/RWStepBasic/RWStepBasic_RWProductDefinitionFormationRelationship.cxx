// File:	RWStepBasic_RWProductDefinitionFormationRelationship.cxx
// Created:	Sun Dec 15 10:59:25 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <RWStepBasic_RWProductDefinitionFormationRelationship.ixx>

//=======================================================================
//function : RWStepBasic_RWProductDefinitionFormationRelationship
//purpose  : 
//=======================================================================

RWStepBasic_RWProductDefinitionFormationRelationship::RWStepBasic_RWProductDefinitionFormationRelationship ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWProductDefinitionFormationRelationship::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                                     const Standard_Integer num,
                                                                     Handle(Interface_Check)& ach,
                                                                     const Handle(StepBasic_ProductDefinitionFormationRelationship) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,5,ach,"product_definition_formation_relationship") ) return;

  // Own fields of ProductDefinitionFormationRelationship

  Handle(TCollection_HAsciiString) aId;
  data->ReadString (num, 1, "id", ach, aId);

  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num, 2, "name", ach, aName);

  Handle(TCollection_HAsciiString) aDescription;
  data->ReadString (num, 3, "description", ach, aDescription);

  Handle(StepBasic_ProductDefinitionFormation) aRelatingProductDefinitionFormation;
  data->ReadEntity (num, 4, "relating_product_definition_formation", ach, STANDARD_TYPE(StepBasic_ProductDefinitionFormation), aRelatingProductDefinitionFormation);

  Handle(StepBasic_ProductDefinitionFormation) aRelatedProductDefinitionFormation;
  data->ReadEntity (num, 5, "related_product_definition_formation", ach, STANDARD_TYPE(StepBasic_ProductDefinitionFormation), aRelatedProductDefinitionFormation);

  // Initialize entity
  ent->Init(aId,
            aName,
            aDescription,
            aRelatingProductDefinitionFormation,
            aRelatedProductDefinitionFormation);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWProductDefinitionFormationRelationship::WriteStep (StepData_StepWriter& SW,
                                                                      const Handle(StepBasic_ProductDefinitionFormationRelationship) &ent) const
{

  // Own fields of ProductDefinitionFormationRelationship

  SW.Send (ent->Id());

  SW.Send (ent->Name());

  SW.Send (ent->Description());

  SW.Send (ent->RelatingProductDefinitionFormation());

  SW.Send (ent->RelatedProductDefinitionFormation());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWProductDefinitionFormationRelationship::Share (const Handle(StepBasic_ProductDefinitionFormationRelationship) &ent,
                                                                  Interface_EntityIterator& iter) const
{

  // Own fields of ProductDefinitionFormationRelationship

  iter.AddItem (ent->RelatingProductDefinitionFormation());

  iter.AddItem (ent->RelatedProductDefinitionFormation());
}
