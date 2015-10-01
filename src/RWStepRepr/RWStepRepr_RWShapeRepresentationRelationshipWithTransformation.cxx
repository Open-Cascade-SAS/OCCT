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
#include <RWStepRepr_RWShapeRepresentationRelationshipWithTransformation.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_Representation.hxx>
#include <StepRepr_ShapeRepresentationRelationshipWithTransformation.hxx>
#include <StepRepr_Transformation.hxx>

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
        data->NamedForComplex("REPRESENTATION_RELATIONSHIP", "RPRRLT",num0,num,ach);

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

        data->NamedForComplex("REPRESENTATION_RELATIONSHIP_WITH_TRANSFORMATION", "RRWT",num0,num,ach);
	if (!data->CheckNbParams(num,1,ach,"representation_relationship_with_transformation")) return;

	// --- own field : transformation_operator

	StepRepr_Transformation aTrans;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat5 =` not needed
	data->ReadEntity(num,1,"transformation_operator",ach,aTrans);

        // --- Instance of plex componant ShapeRepresentationRelationship ---

        data->NamedForComplex("SHAPE_REPRESENTATION_RELATIONSHIP", "SHRPRL",num0,num,ach);
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

