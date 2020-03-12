// Created on : Sat May 02 12:41:15 2020 
// Created by: Irina KRYLOVA
// Generator:	Express (EXPRESS -> CASCADE/XSTEP Translator) V3.0
// Copyright (c) Open CASCADE 2020
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

#include <RWStepKinematics_RWPairRepresentationRelationship.hxx>

#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepKinematics_PairRepresentationRelationship.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepRepr_Representation.hxx>
#include <StepRepr_RepresentationRelationshipWithTransformation.hxx>
#include <StepRepr_RepresentationOrRepresentationReference.hxx>
#include <StepRepr_Transformation.hxx>

//=======================================================================
//function : RWStepKinematics_RWPairRepresentationRelationship
//purpose  :
//=======================================================================
RWStepKinematics_RWPairRepresentationRelationship::RWStepKinematics_RWPairRepresentationRelationship() {}


//=======================================================================
//function : ReadStep
//purpose  :
//=======================================================================
void RWStepKinematics_RWPairRepresentationRelationship::ReadStep (const Handle(StepData_StepReaderData)& theData,
                                                                  const Standard_Integer theNum,
                                                                  Handle(Interface_Check)& theArch,
                                                                  const Handle(StepKinematics_PairRepresentationRelationship)& theEnt) const
{
  // Check number of parameters
  if ( ! theData->CheckNbParams(theNum,6,theArch,"pair_representation_relationship") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  theData->ReadString (theNum, 1, "representation_item.name", theArch, aRepresentationItem_Name);

  // Inherited fields of RepresentationRelationship

  Handle(TCollection_HAsciiString) aRepresentationRelationship_Name;
  theData->ReadString (theNum, 2, "representation_relationship.name", theArch, aRepresentationRelationship_Name);

  Handle(TCollection_HAsciiString) aRepresentationRelationship_Description;
  Standard_Boolean hasRepresentationRelationship_Description = Standard_True;
  if ( theData->IsParamDefined (theNum,3) ) {
    theData->ReadString (theNum, 3, "representation_relationship.description", theArch, aRepresentationRelationship_Description);
  }
  else {
    hasRepresentationRelationship_Description = Standard_False;
    aRepresentationRelationship_Description.Nullify();
  }

  StepRepr_RepresentationOrRepresentationReference aRepresentationRelationship_Rep1;
  theData->ReadEntity (theNum, 4, "representation_relationship.rep1", theArch, aRepresentationRelationship_Rep1);

  StepRepr_RepresentationOrRepresentationReference aRepresentationRelationship_Rep2;
  theData->ReadEntity (theNum, 5, "representation_relationship.rep2", theArch, aRepresentationRelationship_Rep2);

  // Inherited fields of RepresentationRelationshipWithTransformation

  StepRepr_Transformation aRepresentationRelationshipWithTransformation_TransformationOperator;
  theData->ReadEntity (theNum, 6, "representation_relationship_with_transformation.transformation_operator", theArch, aRepresentationRelationshipWithTransformation_TransformationOperator);

  // Initialize entity
  theEnt->Init(aRepresentationItem_Name,
            aRepresentationRelationship_Name,
            hasRepresentationRelationship_Description,
            aRepresentationRelationship_Description,
            aRepresentationRelationship_Rep1,
            aRepresentationRelationship_Rep2,
            aRepresentationRelationshipWithTransformation_TransformationOperator);
}

//=======================================================================
//function : WriteStep
//purpose  :
//=======================================================================
void RWStepKinematics_RWPairRepresentationRelationship::WriteStep (StepData_StepWriter& theSW,
                                                                   const Handle(StepKinematics_PairRepresentationRelationship)& theEnt) const
{

  // Own fields of RepresentationItem

  theSW.Send (theEnt->Name());

  // Own fields of RepresentationRelationship

  theSW.Send (theEnt->Name());


  if (theEnt->RepresentationRelationshipWithTransformation()->HasDescription())
    theSW.Send (theEnt->RepresentationRelationshipWithTransformation()->Description());
  else theSW.SendUndef();

  theSW.Send (theEnt->RepresentationRelationshipWithTransformation()->Rep1());

  theSW.Send (theEnt->RepresentationRelationshipWithTransformation()->Rep2());

  // Inherited fields of RepresentationRelationshipWithTransformation

  theSW.Send (theEnt->RepresentationRelationshipWithTransformation()->TransformationOperator().Value());
}

//=======================================================================
//function : Share
//purpose  :
//=======================================================================
void RWStepKinematics_RWPairRepresentationRelationship::Share (const Handle(StepKinematics_PairRepresentationRelationship)& theEnt,
                                                               Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Inherited fields of RepresentationRelationship

  iter.AddItem (theEnt->RepresentationRelationshipWithTransformation()->Rep1());

  iter.AddItem (theEnt->RepresentationRelationshipWithTransformation()->Rep2());

  // Inherited fields of RepresentationRelationshipWithTransformation

  iter.AddItem (theEnt->RepresentationRelationshipWithTransformation()->TransformationOperator().Value());
}
