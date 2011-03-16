#include <RWStepShape_RWMeasureQualification.ixx>
#include <TCollection_HAsciiString.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepShape_ValueQualifier.hxx>
#include <StepShape_HArray1OfValueQualifier.hxx>


RWStepShape_RWMeasureQualification::RWStepShape_RWMeasureQualification () {}

void RWStepShape_RWMeasureQualification::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_MeasureQualification)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,4,ach,"measure_qualification")) return;

	// --- own field : name ---

	Handle(TCollection_HAsciiString) aName;
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : description ---

	Handle(TCollection_HAsciiString) aDescr;
	data->ReadString (num,2,"description",ach,aDescr);

	// --- own field : qualified_measure ---

	Handle(StepBasic_MeasureWithUnit) aQM;
	data->ReadEntity (num,3,"qualified_measure",ach,
			  STANDARD_TYPE(StepBasic_MeasureWithUnit),aQM);

	// --- own field : qualifiers ---

	Handle(StepShape_HArray1OfValueQualifier) quals;
	Standard_Integer nsub4;
	if (data->ReadSubList (num,4,"qualifiers",ach,nsub4)) {
	  Standard_Integer nb4 = data->NbParams(nsub4);
	  quals = new StepShape_HArray1OfValueQualifier (1,nb4);
	  for (Standard_Integer i4 = 1; i4 <= nb4; i4 ++) {
	    StepShape_ValueQualifier VQ;
	    if (data->ReadEntity (nsub4,i4,"qualifier",ach,VQ))
	      quals->SetValue (i4,VQ);
	  }
	}

	//--- Initialisation of the read entity ---

	ent->Init(aName, aDescr, aQM, quals);
}


void RWStepShape_RWMeasureQualification::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_MeasureQualification)& ent) const
{
  SW.Send(ent->Name());
  SW.Send(ent->Description());
  SW.Send(ent->QualifiedMeasure());
  Standard_Integer i, nbq = ent->NbQualifiers();
  SW.OpenSub();
  for (i = 1; i <= nbq; i ++) SW.Send (ent->QualifiersValue(i).Value());
  SW.CloseSub();
}


void RWStepShape_RWMeasureQualification::Share(const Handle(StepShape_MeasureQualification)& ent, Interface_EntityIterator& iter) const
{
  Standard_Integer i, nbq = ent->NbQualifiers();
  for (i = 1; i <= nbq; i ++) iter.AddItem (ent->QualifiersValue(i).Value());
}

