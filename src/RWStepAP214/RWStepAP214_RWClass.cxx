// File:	RWStepAP214_RWClass.cxx
// Created:	Wed May 10 15:09:06 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <RWStepAP214_RWClass.ixx>

//=======================================================================
//function : RWStepAP214_RWClass
//purpose  : 
//=======================================================================

RWStepAP214_RWClass::RWStepAP214_RWClass ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepAP214_RWClass::ReadStep (const Handle(StepData_StepReaderData)& data,
                                    const Standard_Integer num,
                                    Handle(Interface_Check)& ach,
                                    const Handle(StepAP214_Class) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"class") ) return;

  // Inherited fields of Group

  Handle(TCollection_HAsciiString) aGroup_Name;
  data->ReadString (num, 1, "group.name", ach, aGroup_Name);

  Handle(TCollection_HAsciiString) aGroup_Description;
  Standard_Boolean hasGroup_Description = Standard_True;
  if ( data->IsParamDefined (num,2) ) {
    data->ReadString (num, 2, "group.description", ach, aGroup_Description);
  }
  else {
    hasGroup_Description = Standard_False;
  }

  // Initialize entity
  ent->Init(aGroup_Name,
            hasGroup_Description,
            aGroup_Description);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepAP214_RWClass::WriteStep (StepData_StepWriter& SW,
                                     const Handle(StepAP214_Class) &ent) const
{

  // Inherited fields of Group

  SW.Send (ent->StepBasic_Group::Name());

  if ( ent->StepBasic_Group::HasDescription() ) {
    SW.Send (ent->StepBasic_Group::Description());
  }
  else SW.SendUndef();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepAP214_RWClass::Share (const Handle(StepAP214_Class) &ent,
                                 Interface_EntityIterator& iter) const
{

  // Inherited fields of Group
}
