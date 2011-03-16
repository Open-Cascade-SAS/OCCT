// File:	RWStepRepr_RWMaterialProperty.cxx
// Created:	Sat Dec 14 11:01:41 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <RWStepRepr_RWMaterialProperty.ixx>

//=======================================================================
//function : RWStepRepr_RWMaterialProperty
//purpose  : 
//=======================================================================

RWStepRepr_RWMaterialProperty::RWStepRepr_RWMaterialProperty ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWMaterialProperty::ReadStep (const Handle(StepData_StepReaderData)& data,
                                              const Standard_Integer num,
                                              Handle(Interface_Check)& ach,
                                              const Handle(StepRepr_MaterialProperty) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"material_property") ) return;

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

void RWStepRepr_RWMaterialProperty::WriteStep (StepData_StepWriter& SW,
                                               const Handle(StepRepr_MaterialProperty) &ent) const
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

void RWStepRepr_RWMaterialProperty::Share (const Handle(StepRepr_MaterialProperty) &ent,
                                           Interface_EntityIterator& iter) const
{

  // Inherited fields of PropertyDefinition

  iter.AddItem (ent->StepRepr_PropertyDefinition::Definition().Value());
}
