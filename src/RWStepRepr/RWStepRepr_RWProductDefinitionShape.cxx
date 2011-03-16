// File:	RWStepRepr_RWProductDefinitionShape.cxx
// Created:	Mon Jul  3 16:29:03 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <RWStepRepr_RWProductDefinitionShape.ixx>

//=======================================================================
//function : RWStepRepr_RWProductDefinitionShape
//purpose  : 
//=======================================================================

RWStepRepr_RWProductDefinitionShape::RWStepRepr_RWProductDefinitionShape ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWProductDefinitionShape::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                    const Standard_Integer num,
                                                    Handle(Interface_Check)& ach,
                                                    const Handle(StepRepr_ProductDefinitionShape) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"product_definition_shape") ) return;

  // Inherited fields of PropertyDefinition

  Handle(TCollection_HAsciiString) aPropertyDefinition_Name;
  data->ReadString (num, 1, "property_definition.name", ach, aPropertyDefinition_Name);

  Handle(TCollection_HAsciiString) aPropertyDefinition_Description;
  Standard_Boolean hasPropertyDefinition_Description = Standard_True;
  if ( data->IsParamDefined (num,2) ) {
    data->ReadString (num, 2, "property_definition.description", ach, aPropertyDefinition_Description);
  }
  else {
    hasPropertyDefinition_Description = Standard_False;
  }

  StepRepr_CharacterizedDefinition aPropertyDefinition_Definition;
  data->ReadEntity (num, 3, "property_definition.definition", ach, aPropertyDefinition_Definition);

  // Initialize entity
  ent->Init(aPropertyDefinition_Name,
            hasPropertyDefinition_Description,
            aPropertyDefinition_Description,
            aPropertyDefinition_Definition);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWProductDefinitionShape::WriteStep (StepData_StepWriter& SW,
                                                     const Handle(StepRepr_ProductDefinitionShape) &ent) const
{

  // Inherited fields of PropertyDefinition

  SW.Send (ent->StepRepr_PropertyDefinition::Name());

  if ( ent->StepRepr_PropertyDefinition::HasDescription() ) {
    SW.Send (ent->StepRepr_PropertyDefinition::Description());
  }
  else SW.SendUndef();

  SW.Send (ent->StepRepr_PropertyDefinition::Definition().Value());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepRepr_RWProductDefinitionShape::Share (const Handle(StepRepr_ProductDefinitionShape) &ent,
                                                 Interface_EntityIterator& iter) const
{

  // Inherited fields of PropertyDefinition

  iter.AddItem (ent->StepRepr_PropertyDefinition::Definition().Value());
}
