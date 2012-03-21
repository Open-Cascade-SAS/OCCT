// Created on: 2000-05-10
// Created by: Andrey BETENEV
// Copyright (c) 2000-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1

#include <RWStepBasic_RWGroupRelationship.ixx>

//=======================================================================
//function : RWStepBasic_RWGroupRelationship
//purpose  : 
//=======================================================================

RWStepBasic_RWGroupRelationship::RWStepBasic_RWGroupRelationship ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWGroupRelationship::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                const Standard_Integer num,
                                                Handle(Interface_Check)& ach,
                                                const Handle(StepBasic_GroupRelationship) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,4,ach,"group_relationship") ) return;

  // Own fields of GroupRelationship

  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num, 1, "name", ach, aName);

  Handle(TCollection_HAsciiString) aDescription;
  Standard_Boolean hasDescription = Standard_True;
  if ( data->IsParamDefined (num,2) ) {
    data->ReadString (num, 2, "description", ach, aDescription);
  }
  else {
    hasDescription = Standard_False;
  }

  Handle(StepBasic_Group) aRelatingGroup;
  data->ReadEntity (num, 3, "relating_group", ach, STANDARD_TYPE(StepBasic_Group), aRelatingGroup);

  Handle(StepBasic_Group) aRelatedGroup;
  data->ReadEntity (num, 4, "related_group", ach, STANDARD_TYPE(StepBasic_Group), aRelatedGroup);

  // Initialize entity
  ent->Init(aName,
            hasDescription,
            aDescription,
            aRelatingGroup,
            aRelatedGroup);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWGroupRelationship::WriteStep (StepData_StepWriter& SW,
                                                 const Handle(StepBasic_GroupRelationship) &ent) const
{

  // Own fields of GroupRelationship

  SW.Send (ent->Name());

  if ( ent->HasDescription() ) {
    SW.Send (ent->Description());
  }
  else SW.SendUndef();

  SW.Send (ent->RelatingGroup());

  SW.Send (ent->RelatedGroup());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWGroupRelationship::Share (const Handle(StepBasic_GroupRelationship) &ent,
                                             Interface_EntityIterator& iter) const
{

  // Own fields of GroupRelationship

  iter.AddItem (ent->RelatingGroup());

  iter.AddItem (ent->RelatedGroup());
}
