// Copyright (c) 1999-2012 OPEN CASCADE SAS
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


#include <RWStepShape_RWSweptFaceSolid.ixx>
#include <StepShape_FaceSurface.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepShape_SweptFaceSolid.hxx>


RWStepShape_RWSweptFaceSolid::RWStepShape_RWSweptFaceSolid () {}

void RWStepShape_RWSweptFaceSolid::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_SweptFaceSolid)& ent) const
{


  // --- Number of Parameter Control ---
  
  if (!data->CheckNbParams(num,2,ach,"swept_face_solid")) return;
  
  // --- inherited field : name ---
  
  Handle(TCollection_HAsciiString) aName;
  //szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
  data->ReadString (num,1,"name",ach,aName);
  
  // --- own field : sweptFace ---
  
  Handle(StepShape_FaceSurface) aSweptFace;
  //szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
  data->ReadEntity(num, 2,"swept_face", ach, STANDARD_TYPE(StepShape_FaceSurface), aSweptFace);
  
  //--- Initialisation of the read entity ---

  ent->Init(aName, aSweptFace);
}


void RWStepShape_RWSweptFaceSolid::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_SweptFaceSolid)& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());
	
  // --- own field : sweptFace ---
  
  SW.Send(ent->SweptFace());
}


void RWStepShape_RWSweptFaceSolid::Share(const Handle(StepShape_SweptFaceSolid)& ent, Interface_EntityIterator& iter) const
{

  iter.GetOneItem(ent->SweptFace());
}

