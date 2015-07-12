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
#include <RWStepShape_RWExtrudedFaceSolid.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_Direction.hxx>
#include <StepShape_ExtrudedFaceSolid.hxx>
#include <StepShape_FaceSurface.hxx>

RWStepShape_RWExtrudedFaceSolid::RWStepShape_RWExtrudedFaceSolid () {}

void RWStepShape_RWExtrudedFaceSolid::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_ExtrudedFaceSolid)& ent) const
{


  // --- Number of Parameter Control ---
  
  if (!data->CheckNbParams(num,4,ach,"extruded_face_solid")) return;

  // --- inherited field : name ---

  Handle(TCollection_HAsciiString) aName;
  //szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
  data->ReadString (num,1,"name",ach,aName);
  
  // --- inherited field : sweptFace ---

  Handle(StepShape_FaceSurface) aSweptFace;
  //szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
  data->ReadEntity(num, 2,"swept_face", ach, STANDARD_TYPE(StepShape_FaceSurface), aSweptFace);

  // --- own field : extrudedDirection ---

  Handle(StepGeom_Direction) aExtrudedDirection;
  //szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
  data->ReadEntity(num, 3,"extruded_direction", ach, STANDARD_TYPE(StepGeom_Direction), aExtrudedDirection);

  // --- own field : depth ---

  Standard_Real aDepth;
  //szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
  data->ReadReal (num,4,"depth",ach,aDepth);

  //--- Initialisation of the read entity ---

  ent->Init(aName, aSweptFace, aExtrudedDirection, aDepth);
}


void RWStepShape_RWExtrudedFaceSolid::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_ExtrudedFaceSolid)& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());
  
  // --- inherited field sweptFace ---
  
  SW.Send(ent->SweptFace());
  
  // --- own field : extrudedDirection ---

  SW.Send(ent->ExtrudedDirection());

  // --- own field : depth ---

  SW.Send(ent->Depth());
}


void RWStepShape_RWExtrudedFaceSolid::Share(const Handle(StepShape_ExtrudedFaceSolid)& ent, Interface_EntityIterator& iter) const
{

  iter.GetOneItem(ent->SweptFace());
  iter.GetOneItem(ent->ExtrudedDirection());
}

