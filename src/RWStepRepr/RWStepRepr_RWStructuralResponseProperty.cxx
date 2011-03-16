// File:	RWStepRepr_RWStructuralResponseProperty.cxx
// Created:	Sun Dec 15 10:59:25 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <RWStepRepr_RWStructuralResponseProperty.ixx>

//#include <StepRepr_RepresentedDefinition.hxx>

//=======================================================================
//function : RWStepRepr_RWStructuralResponseProperty
//purpose  : 
//=======================================================================

RWStepRepr_RWStructuralResponseProperty::RWStepRepr_RWStructuralResponseProperty ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWStructuralResponseProperty::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                        const Standard_Integer num,
                                                        Handle(Interface_Check)& ach,
                                                        const Handle(StepRepr_StructuralResponseProperty) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"structural_response_property") ) return;

  // Inherited fields of PropertyDefinition

  Handle(TCollection_HAsciiString) aPropertyDefinition_Name;
  data->ReadString (num, 1, "property_definition.name", ach, aPropertyDefinition_Name);

  Handle(TCollection_HAsciiString) aPropertyDefinition_Description;
  data->ReadString (num, 2, "property_definition.description", ach, aPropertyDefinition_Description);

  StepRepr_CharacterizedDefinition aPropertyDefinition_Definition;
  data->ReadEntity (num, 3, "property_definition.definition", ach, aPropertyDefinition_Definition);

  // Initialize entity
  ent->Init(aPropertyDefinition_Name, Standard_True,
            aPropertyDefinition_Description,
            aPropertyDefinition_Definition);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWStructuralResponseProperty::WriteStep (StepData_StepWriter& SW,
                                                         const Handle(StepRepr_StructuralResponseProperty) &ent) const
{

  // Inherited fields of PropertyDefinition

  SW.Send (ent->StepRepr_PropertyDefinition::Name());

  SW.Send (ent->StepRepr_PropertyDefinition::Description());

  SW.Send (ent->StepRepr_PropertyDefinition::Definition().Value());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepRepr_RWStructuralResponseProperty::Share (const Handle(StepRepr_StructuralResponseProperty) &ent,
                                                     Interface_EntityIterator& iter) const
{

  // Inherited fields of PropertyDefinition

  iter.AddItem (ent->StepRepr_PropertyDefinition::Definition().Value());
}
