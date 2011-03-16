
#include <RWHeaderSection_RWFileDescription.ixx>
#include <Interface_HArray1OfHAsciiString.hxx>


RWHeaderSection_RWFileDescription::RWHeaderSection_RWFileDescription () {}

void RWHeaderSection_RWFileDescription::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(HeaderSection_FileDescription)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"file_description has not 2 parameter(s)")) return;

	// --- own field : description ---

	Handle(Interface_HArray1OfHAsciiString) aDescription;
	Handle(TCollection_HAsciiString) aDescriptionItem;
	Standard_Integer nsub1;
	nsub1 = data->SubListNumber(num, 1, Standard_False);
	if (nsub1 !=0) {
	  Standard_Integer nb1 = data->NbParams(nsub1);
	  aDescription = new Interface_HArray1OfHAsciiString (1, nb1);
	  for (Standard_Integer i1 = 1; i1 <= nb1; i1 ++) {
	    Standard_Boolean stat1 = data->ReadString
	         (nsub1,i1,"description",ach,aDescriptionItem);
	    if (stat1) aDescription->SetValue(i1,aDescriptionItem);
	  }
	}
	else {
	  ach->AddFail("Parameter #1 (description) is not a LIST");
	}

	// --- own field : implementationLevel ---

	Handle(TCollection_HAsciiString) aImplementationLevel;
	Standard_Boolean stat2;
	stat2 = data->ReadString (num,2,"implementation_level",ach,aImplementationLevel);

	//--- Initialisation of the read entity ---


	if (!ach->HasFailed()) ent->Init(aDescription, aImplementationLevel);
}


void RWHeaderSection_RWFileDescription::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(HeaderSection_FileDescription)& ent) const
{

	// --- own field : description ---

	SW.OpenSub();
	for (Standard_Integer i1 = 1;  i1 <= ent->NbDescription();  i1 ++) {
	  SW.Send(ent->DescriptionValue(i1));
	}
	SW.CloseSub();

	// --- own field : implementationLevel ---

	SW.Send(ent->ImplementationLevel());
}
