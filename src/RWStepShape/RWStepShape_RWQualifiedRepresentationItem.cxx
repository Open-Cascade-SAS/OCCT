#include <RWStepShape_RWQualifiedRepresentationItem.ixx>
#include <TCollection_HAsciiString.hxx>
#include <StepShape_ValueQualifier.hxx>
#include <StepShape_HArray1OfValueQualifier.hxx>


RWStepShape_RWQualifiedRepresentationItem::RWStepShape_RWQualifiedRepresentationItem () {}

void RWStepShape_RWQualifiedRepresentationItem::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_QualifiedRepresentationItem)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"qualified_representation_item")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : qualifiers ---

	Handle(StepShape_HArray1OfValueQualifier) quals;
	Standard_Integer nsub2;
	if (data->ReadSubList (num,2,"qualifiers",ach,nsub2)) {
	  Standard_Integer nb2 = data->NbParams(nsub2);
	  quals = new StepShape_HArray1OfValueQualifier (1,nb2);
	  for (Standard_Integer i2 = 1; i2 <= nb2; i2 ++) {
	    StepShape_ValueQualifier VQ;
	    if (data->ReadEntity (nsub2,i2,"qualifier",ach,VQ))
	      quals->SetValue (i2,VQ);
	  }
	}

	//--- Initialisation of the read entity ---

	ent->Init(aName, quals);
}


void RWStepShape_RWQualifiedRepresentationItem::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_QualifiedRepresentationItem)& ent) const
{
  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- own field : qualifiers ---
  Standard_Integer i, nbq = ent->NbQualifiers();
  SW.OpenSub();
  for (i = 1; i <= nbq; i ++) SW.Send (ent->QualifiersValue(i).Value());
  SW.CloseSub();
}


void RWStepShape_RWQualifiedRepresentationItem::Share(const Handle(StepShape_QualifiedRepresentationItem)& ent, Interface_EntityIterator& iter) const
{
  Standard_Integer i, nbq = ent->NbQualifiers();
  for (i = 1; i <= nbq; i ++) iter.AddItem (ent->QualifiersValue(i).Value());
}

