// File:	RWStepBasic_RWConversionBasedUnitAndAreaUnit.cxx
// Created:	Tue Oct 12 14:20:03 1999
// Author:	data exchange team
//		<det@friendox>


#include <RWStepBasic_RWConversionBasedUnitAndAreaUnit.ixx>
#include <StepBasic_MeasureWithUnit.hxx>

RWStepBasic_RWConversionBasedUnitAndAreaUnit::RWStepBasic_RWConversionBasedUnitAndAreaUnit ()
{
}

void RWStepBasic_RWConversionBasedUnitAndAreaUnit::ReadStep(const Handle(StepData_StepReaderData)& data,
							    const Standard_Integer num0,
							    Handle(Interface_Check)& ach,
							    const Handle(StepBasic_ConversionBasedUnitAndAreaUnit)& ent) const
{
  Standard_Integer num = num0;
  //data->NamedForComplex("AREA_UNIT",num0,num,ach);
  if (!data->CheckNbParams(num,0,ach,"area_unit")) return;
  
  // --- Instance of plex componant ConversionBasedUnit ---
  num = data->NextForComplex(num);
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
  
  ent->Init(aDimensions,aName,aConversionFactor);
}

void RWStepBasic_RWConversionBasedUnitAndAreaUnit::WriteStep(StepData_StepWriter& SW,
							     const Handle(StepBasic_ConversionBasedUnitAndAreaUnit)& ent) const
{
  SW.StartEntity("AREA_UNIT");
  SW.StartEntity("CONVERSION_BASED_UNIT");
  SW.Send(ent->Name());
  SW.Send(ent->ConversionFactor());
  SW.StartEntity("NAMED_UNIT");
  SW.Send(ent->Dimensions());
}
  
void RWStepBasic_RWConversionBasedUnitAndAreaUnit::Share(const Handle(StepBasic_ConversionBasedUnitAndAreaUnit)& ent, 
							 Interface_EntityIterator& iter) const
{
  iter.GetOneItem(ent->Dimensions());
  iter.GetOneItem(ent->ConversionFactor());
}


  
