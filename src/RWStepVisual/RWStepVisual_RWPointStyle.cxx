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


#include <RWStepVisual_RWPointStyle.ixx>
#include <StepVisual_MarkerSelect.hxx>
#include <StepBasic_SizeSelect.hxx>
#include <StepVisual_Colour.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepVisual_PointStyle.hxx>


RWStepVisual_RWPointStyle::RWStepVisual_RWPointStyle () {}

void RWStepVisual_RWPointStyle::ReadStep
(const Handle(StepData_StepReaderData)& data,
 const Standard_Integer num,
 Handle(Interface_Check)& ach,
 const Handle(StepVisual_PointStyle)& ent) const
{
  
  
  // --- Number of Parameter Control ---
  
  if (!data->CheckNbParams(num,4,ach,"point_style")) return;
  
  // --- own field : name ---
  
  Handle(TCollection_HAsciiString) aName;
  //szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
  data->ReadString (num,1,"name",ach,aName);
  
  // --- own field : marker ---
  // idem RWStepVisual_RWBooleanResult
  
  StepVisual_MarkerSelect aMarker;
  //szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
  data->ReadEntity(num,2,"marker",ach,aMarker);
  
  // --- own field : markerSize ---
  // idem RWStepVisual_RWBooleanResult
  
  StepBasic_SizeSelect aMarkerSize;
  //szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
  data->ReadEntity(num,3,"marker_size",ach,aMarkerSize);
  
  // --- own field : markerColour ---
  
  Handle(StepVisual_Colour) aMarkerColour;
  //szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
  data->ReadEntity(num, 4,"marker_colour", ach, STANDARD_TYPE(StepVisual_Colour), aMarkerColour);
  
  //--- Initialisation of the read entity ---
  
  
  ent->Init(aName, aMarker, aMarkerSize, aMarkerColour);
}


void RWStepVisual_RWPointStyle::WriteStep
(StepData_StepWriter& SW,
 const Handle(StepVisual_PointStyle)& ent) const
{
  
  // --- own field : name ---
  SW.Send(ent->Name());
  
  // --- own field : marker ---
  SW.Send(ent->Marker().Value());
  
  // --- own field : markerSize ---
  SW.Send(ent->MarkerSize().Value());
  
  // --- own field : markerColour ---
  SW.Send(ent->MarkerColour());
}


void RWStepVisual_RWPointStyle::Share(const Handle(StepVisual_PointStyle)& ent, Interface_EntityIterator& iter) const
{
  
  if (ent->Marker().CaseNumber() > 0)
    iter.GetOneItem(ent->Marker().Value());
  iter.GetOneItem(ent->MarkerColour());
}

