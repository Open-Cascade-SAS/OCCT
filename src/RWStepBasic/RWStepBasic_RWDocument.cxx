// File:	RWStepBasic_RWDocument.cxx
// Created:	Thu May 11 16:38:00 2000 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <RWStepBasic_RWDocument.ixx>

//=======================================================================
//function : RWStepBasic_RWDocument
//purpose  : 
//=======================================================================

RWStepBasic_RWDocument::RWStepBasic_RWDocument ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWDocument::ReadStep (const Handle(StepData_StepReaderData)& data,
                                       const Standard_Integer num,
                                       Handle(Interface_Check)& ach,
                                       const Handle(StepBasic_Document) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,4,ach,"document") ) return;

  // Own fields of Document

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

  Handle(StepBasic_DocumentType) aKind;
  data->ReadEntity (num, 4, "kind", ach, STANDARD_TYPE(StepBasic_DocumentType), aKind);

  // Initialize entity
  ent->Init(aId,
            aName,
            hasDescription,
            aDescription,
            aKind);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWDocument::WriteStep (StepData_StepWriter& SW,
                                        const Handle(StepBasic_Document) &ent) const
{

  // Own fields of Document

  SW.Send (ent->Id());

  SW.Send (ent->Name());

  if ( ent->HasDescription() ) {
    SW.Send (ent->Description());
  }
  else SW.SendUndef();

  SW.Send (ent->Kind());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWDocument::Share (const Handle(StepBasic_Document) &ent,
                                    Interface_EntityIterator& iter) const
{

  // Own fields of Document

  iter.AddItem (ent->Kind());
}
