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


#include <RWStepRepr_RWShapeRepresentationRelationshipWithTransformation.ixx>
#include <StepRepr_Representation.hxx>
#include <StepRepr_Transformation.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepRepr_ShapeRepresentationRelationshipWithTransformation.hxx>


RWStepRepr_RWShapeRepresentationRelationshipWithTransformation::RWStepRepr_RWShapeRepresentationRelationshipWithTransformation () {}

void RWStepRepr_RWShapeRepresentationRelationshipWithTransformation::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num0,
	 Handle(Interface_Check)& ach,
	 const Handle(StepRepr_ShapeRepresentationRelationshipWithTransformation)& ent) const
{

//  Complex entity
//  REPR_RLTS,REPR_RLTS_WITH_TR,SHAPE_REPR_RLTS
//  But same fields as RepresentationRelationshipWithTransformation

        // --- Instance of plex componant RepresentationRelationship ---

        Standard_Integer num = 0;  // num0;
        data->NamedForComplex("REPRESENTATION_RELATIONSHIP RPRRLT",num0,num,ach);

	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,4,ach,"representation_relationship")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- inherited field : description ---

	Handle(TCollection_HAsciiString) aDescription;
	if (data->IsParamDefined (num,2)) { //abv 08.10.99 TRJ2
	  data->ReadString (num,2,"description",ach,aDescription);
	}

	// --- inherited field : rep1 ---

	Handle(StepRepr_Representation) aRep1;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadEntity(num, 3,"rep_1", ach, STANDARD_TYPE(StepRepr_Representation), aRep1);

	// --- inherited field : rep2 ---

	Handle(StepRepr_Representation) aRep2;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
	data->ReadEntity(num, 4,"rep_2", ach, STANDARD_TYPE(StepRepr_Representation), aRep2);

        // --- Instance of plex componant RepresentationRelationshipWithTransformation ---

        data->NamedForComplex("REPRESENTATION_RELATIONSHIP_WITH_TRANSFORMATION RRWT",num0,num,ach);
	if (!data->CheckNbParams(num,1,ach,"representation_relationship_with_transformation")) return;

	// --- own field : transformation_operator

	StepRepr_Transformation aTrans;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat5 =` not needed
	data->ReadEntity(num,1,"transformation_operator",ach,aTrans);

        // --- Instance of plex componant ShapeRepresentationRelationship ---

        data->NamedForComplex("SHAPE_REPRESENTATION_RELATIONSHIP SHRPRL",num0,num,ach);
	if (!data->CheckNbParams(num,0,ach,"shape_representation_relationship")) return;

	//--- Initialisation of the read entity ---


	ent->Init(aName, aDescription, aRep1, aRep2, aTrans);
}


void RWStepRepr_RWShapeRepresentationRelationshipWithTransformation::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepRepr_ShapeRepresentationRelationshipWithTransformation)& ent) const
{
        // --- Instance of plex componant RepresentationRelationship ---

  SW.StartEntity ("REPRESENTATION_RELATIONSHIP");

	// --- inherited field : name ---

	SW.Send(ent->Name());

	// --- inherited field : description ---

	SW.Send(ent->Description());

	// --- inherited field : rep1 ---

	SW.Send(ent->Rep1());

	// --- inherited field : rep2 ---

	SW.Send(ent->Rep2());


        // --- Instance of plex componant RepresentationRelationshipWithTransformation ---
  SW.StartEntity ("REPRESENTATION_RELATIONSHIP_WITH_TRANSFORMATION");

	SW.Send(ent->TransformationOperator().Value());

        // --- Instance of plex componant ShapeRepresentationRelationship ---

  SW.StartEntity ("SHAPE_REPRESENTATION_RELATIONSHIP");
}


void RWStepRepr_RWShapeRepresentationRelationshipWithTransformation::Share(const Handle(StepRepr_ShapeRepresentationRelationshipWithTransformation)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Rep1());

	iter.GetOneItem(ent->Rep2());

	iter.GetOneItem(ent->TransformationOperator().Value());
}

