// Created on: 1999-10-12
// Created by: data exchange team
// Copyright (c) 1999-1999 Matra Datavision
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



#include <RWStepBasic_RWConversionBasedUnitAndVolumeUnit.ixx>

RWStepBasic_RWConversionBasedUnitAndVolumeUnit::RWStepBasic_RWConversionBasedUnitAndVolumeUnit ()
{
}

void RWStepBasic_RWConversionBasedUnitAndVolumeUnit::ReadStep(const Handle(StepData_StepReaderData)& data,
							      const Standard_Integer num0,
							      Handle(Interface_Check)& ach,
							      const Handle(StepBasic_ConversionBasedUnitAndVolumeUnit)& ent) const
{
  Standard_Integer num = num0;
  // --- Instance of plex componant ConversionBasedUnit ---
  if (!data->CheckNbParams(num,2,ach,"conversion_based_unit")) return;
  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num,1,"name",ach,aName);
  
  // --- field : conversionFactor ---
  Handle(StepBasic_MeasureWithUnit) aConversionFactor;
  data->ReadEntity(num, 2,"conversion_factor", ach, STANDARD_TYPE(StepBasic_MeasureWithUnit), aConversionFactor);
  
  num = data->NextForComplex(num);
  if (!data->CheckNbParams(num,1,ach,"named_unit")) return;
  Handle(StepBasic_DimensionalExponents) aDimensions;
  data->ReadEntity(num, 1,"dimensions", ach, STANDARD_TYPE(StepBasic_DimensionalExponents), aDimensions);
  
  data->NamedForComplex("VOLUME_UNIT",num0,num,ach);
  if (!data->CheckNbParams(num,0,ach,"volume_unit")) return;
  
  ent->Init(aDimensions,aName,aConversionFactor);
}

void RWStepBasic_RWConversionBasedUnitAndVolumeUnit::WriteStep(StepData_StepWriter& SW,
							     const Handle(StepBasic_ConversionBasedUnitAndVolumeUnit)& ent) const
{
  SW.StartEntity("CONVERSION_BASED_UNIT");
  SW.Send(ent->Name());
  SW.Send(ent->ConversionFactor());
  SW.StartEntity("NAMED_UNIT");
  SW.Send(ent->Dimensions());
  SW.StartEntity("VOLUME_UNIT");
}
  
void RWStepBasic_RWConversionBasedUnitAndVolumeUnit::Share(const Handle(StepBasic_ConversionBasedUnitAndVolumeUnit)& ent, 
							   Interface_EntityIterator& iter) const
{
  iter.GetOneItem(ent->Dimensions());
  iter.GetOneItem(ent->ConversionFactor());
}
