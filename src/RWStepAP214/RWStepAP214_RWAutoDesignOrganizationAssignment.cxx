
#include <RWStepAP214_RWAutoDesignOrganizationAssignment.ixx>
#include <StepAP214_HArray1OfAutoDesignGeneralOrgItem.hxx>
#include <StepAP214_AutoDesignGeneralOrgItem.hxx>
#include <StepBasic_Organization.hxx>
#include <StepBasic_OrganizationRole.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepAP214_AutoDesignOrganizationAssignment.hxx>


RWStepAP214_RWAutoDesignOrganizationAssignment::RWStepAP214_RWAutoDesignOrganizationAssignment () {}

void RWStepAP214_RWAutoDesignOrganizationAssignment::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepAP214_AutoDesignOrganizationAssignment)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"auto_design_organization_assignment")) return;

	// --- inherited field : assignedOrganization ---

	Handle(StepBasic_Organization) aAssignedOrganization;
#ifdef DEB
	Standard_Boolean stat1 = 
#endif
	  data->ReadEntity(num, 1,"assigned_organization", ach, STANDARD_TYPE(StepBasic_Organization), aAssignedOrganization);

	// --- inherited field : role ---

	Handle(StepBasic_OrganizationRole) aRole;
#ifdef DEB
	Standard_Boolean stat2 = 
#endif
	  data->ReadEntity(num, 2,"role", ach, STANDARD_TYPE(StepBasic_OrganizationRole), aRole);

	// --- own field : items ---

	Handle(StepAP214_HArray1OfAutoDesignGeneralOrgItem) aItems;
	StepAP214_AutoDesignGeneralOrgItem aItemsItem;
	Standard_Integer nsub3;
	if (data->ReadSubList (num,3,"items",ach,nsub3)) {
	  Standard_Integer nb3 = data->NbParams(nsub3);
	  aItems = new StepAP214_HArray1OfAutoDesignGeneralOrgItem (1, nb3);
	  for (Standard_Integer i3 = 1; i3 <= nb3; i3 ++) {
	    Standard_Boolean stat3 = data->ReadEntity
	         (nsub3,i3,"items",ach,aItemsItem);
	    if (stat3) aItems->SetValue(i3,aItemsItem);
	  }
	}

	//--- Initialisation of the read entity ---


	ent->Init(aAssignedOrganization, aRole, aItems);
}


void RWStepAP214_RWAutoDesignOrganizationAssignment::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepAP214_AutoDesignOrganizationAssignment)& ent) const
{

	// --- inherited field assignedOrganization ---

	SW.Send(ent->AssignedOrganization());

	// --- inherited field role ---

	SW.Send(ent->Role());

	// --- own field : items ---

	SW.OpenSub();
	for (Standard_Integer i3 = 1;  i3 <= ent->NbItems();  i3 ++) {
	  SW.Send(ent->ItemsValue(i3).Value());
	}
	SW.CloseSub();
}


void RWStepAP214_RWAutoDesignOrganizationAssignment::Share(const Handle(StepAP214_AutoDesignOrganizationAssignment)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->AssignedOrganization());


	iter.GetOneItem(ent->Role());


	Standard_Integer nbElem3 = ent->NbItems();
	for (Standard_Integer is3=1; is3<=nbElem3; is3 ++) {
	  iter.GetOneItem(ent->ItemsValue(is3).Value());
	}

}

