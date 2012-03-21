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


#include <RWStepGeom_RWCartesianTransformationOperator.ixx>
#include <StepGeom_Direction.hxx>
#include <StepGeom_CartesianPoint.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepGeom_CartesianTransformationOperator.hxx>


RWStepGeom_RWCartesianTransformationOperator::RWStepGeom_RWCartesianTransformationOperator () {}

void RWStepGeom_RWCartesianTransformationOperator::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepGeom_CartesianTransformationOperator)& ent) const
{

// 29 MAI 1997
// PATCH CKY : functionally_defined_transformation est aussi supertype, avec
//  deux champs STRING. Pour bien faire, les ajouter. Au minimum, les faire
//  sauter. On attend 7 champs au lieu de 5 et on commence au champ 3

	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,7,ach,"cartesian_transformation_operator")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,3,"name",ach,aName);

	// --- own field : axis1 ---

	Handle(StepGeom_Direction) aAxis1;
	Standard_Boolean hasAaxis1 = Standard_True;
	if (data->IsParamDefined(num,4)) {
	  //szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	  data->ReadEntity(num, 4,"axis1", ach, STANDARD_TYPE(StepGeom_Direction), aAxis1);
	}
	else {
	  hasAaxis1 = Standard_False;
	  aAxis1.Nullify();
	}

	// --- own field : axis2 ---

	Handle(StepGeom_Direction) aAxis2;
	Standard_Boolean hasAaxis2 = Standard_True;
	if (data->IsParamDefined(num,5)) {
	  //szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	  data->ReadEntity(num, 5,"axis2", ach, STANDARD_TYPE(StepGeom_Direction), aAxis2);
	}
	else {
	  hasAaxis2 = Standard_False;
	  aAxis2.Nullify();
	}

	// --- own field : localOrigin ---

	Handle(StepGeom_CartesianPoint) aLocalOrigin;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
	data->ReadEntity(num, 6,"local_origin", ach, STANDARD_TYPE(StepGeom_CartesianPoint), aLocalOrigin);

	// --- own field : scale ---

	Standard_Real aScale;
	Standard_Boolean hasAscale = Standard_True;
	if (data->IsParamDefined(num,7)) {
	  //szv#4:S4163:12Mar99 `Standard_Boolean stat5 =` not needed
	  data->ReadReal (num,7,"scale",ach,aScale);
	}
	else {
	  hasAscale = Standard_False;
	  aScale = 0.;
	}

	//--- Initialisation of the read entity ---


	ent->Init(aName, hasAaxis1, aAxis1, hasAaxis2, aAxis2, aLocalOrigin, hasAscale, aScale);
}


void RWStepGeom_RWCartesianTransformationOperator::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepGeom_CartesianTransformationOperator)& ent) const
{

	// --- inherited field name ---
// PATCH CKY : name depuis geometric_representation_item
//    et name + descr depuis functionally_defined_transformation

	SW.Send(ent->Name());
	SW.Send(ent->Name());
	SW.Send(ent->Name());

	// --- own field : axis1 ---

	Standard_Boolean hasAaxis1 = ent->HasAxis1();
	if (hasAaxis1) {
	  SW.Send(ent->Axis1());
	}
	else {
	  SW.SendUndef();
	}

	// --- own field : axis2 ---

	Standard_Boolean hasAaxis2 = ent->HasAxis2();
	if (hasAaxis2) {
	  SW.Send(ent->Axis2());
	}
	else {
	  SW.SendUndef();
	}

	// --- own field : localOrigin ---

	SW.Send(ent->LocalOrigin());

	// --- own field : scale ---

	Standard_Boolean hasAscale = ent->HasScale();
	if (hasAscale) {
	  SW.Send(ent->Scale());
	}
	else {
	  SW.SendUndef();
	}
}


void RWStepGeom_RWCartesianTransformationOperator::Share(const Handle(StepGeom_CartesianTransformationOperator)& ent, Interface_EntityIterator& iter) const
{
	if (ent->HasAxis1()) {
	  iter.GetOneItem(ent->Axis1());
	}


	if (ent->HasAxis2()) {
	  iter.GetOneItem(ent->Axis2());
	}



	iter.GetOneItem(ent->LocalOrigin());
}

