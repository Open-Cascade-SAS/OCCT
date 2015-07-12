// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepShape_RWRevolvedFaceSolid.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_Axis1Placement.hxx>
#include <StepShape_FaceSurface.hxx>
#include <StepShape_RevolvedFaceSolid.hxx>

RWStepShape_RWRevolvedFaceSolid::RWStepShape_RWRevolvedFaceSolid () {}

void RWStepShape_RWRevolvedFaceSolid::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_RevolvedFaceSolid)& ent) const
{


  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num,4,ach,"revolved_face_solid")) return;

  // --- inherited field : name ---

  Handle(TCollection_HAsciiString) aName;
  //szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
  data->ReadString (num,1,"name",ach,aName);
  
  // --- inherited field : sweptFace ---

  Handle(StepShape_FaceSurface) aSweptFace;
  //szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
  data->ReadEntity(num, 2,"swept_face", ach, STANDARD_TYPE(StepShape_FaceSurface), aSweptFace);

  // --- own field : axis ---

  Handle(StepGeom_Axis1Placement) aAxis;
  //szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
  data->ReadEntity(num, 3,"axis", ach, STANDARD_TYPE(StepGeom_Axis1Placement), aAxis);
  
  // --- own field : angle ---

  Standard_Real aAngle;
  //szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
  data->ReadReal (num,4,"angle",ach,aAngle);

  //--- Initialisation of the read entity ---

  ent->Init(aName, aSweptFace, aAxis, aAngle);
}


void RWStepShape_RWRevolvedFaceSolid::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_RevolvedFaceSolid)& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- inherited field sweptFace ---

  SW.Send(ent->SweptFace());

  // --- own field : axis ---

  SW.Send(ent->Axis());

  // --- own field : angle ---

  SW.Send(ent->Angle());
}


void RWStepShape_RWRevolvedFaceSolid::Share(const Handle(StepShape_RevolvedFaceSolid)& ent, Interface_EntityIterator& iter) const
{

  iter.GetOneItem(ent->SweptFace());
  iter.GetOneItem(ent->Axis());
}

