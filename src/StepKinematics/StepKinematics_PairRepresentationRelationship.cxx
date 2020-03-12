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

#include <StepKinematics_PairRepresentationRelationship.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_PairRepresentationRelationship, StepGeom_GeometricRepresentationItem)

//=======================================================================
//function : StepKinematics_PairRepresentationRelationship
//purpose  :
//=======================================================================
StepKinematics_PairRepresentationRelationship::StepKinematics_PairRepresentationRelationship ()
{
}

//=======================================================================
//function : Init
//purpose  :
//=======================================================================
void StepKinematics_PairRepresentationRelationship::Init (const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
                                                          const Handle(TCollection_HAsciiString)& theRepresentationRelationship_Name,
                                                          const Standard_Boolean /*hasRepresentationRelationship_Description*/,
                                                          const Handle(TCollection_HAsciiString)& theRepresentationRelationship_Description,
                                                          const StepRepr_RepresentationOrRepresentationReference& theRepresentationRelationship_Rep1,
                                                          const StepRepr_RepresentationOrRepresentationReference& theRepresentationRelationship_Rep2,
                                                          const StepRepr_Transformation& theRepresentationRelationshipWithTransformation_TransformationOperator)
{
  StepGeom_GeometricRepresentationItem::Init(theRepresentationItem_Name);
  myRepresentationRelationshipWithTransformation = new StepRepr_RepresentationRelationshipWithTransformation;
  myRepresentationRelationshipWithTransformation->Init(theRepresentationRelationship_Name,
                                                        /*hasRepresentationRelationship_Description,*/
                                                        theRepresentationRelationship_Description,
                                                        theRepresentationRelationship_Rep1.Representation(),
                                                        theRepresentationRelationship_Rep2.Representation(),
                                                        theRepresentationRelationshipWithTransformation_TransformationOperator);
}

//=======================================================================
//function : RepresentationRelationshipWithTransformation
//purpose  :
//=======================================================================
Handle(StepRepr_RepresentationRelationshipWithTransformation) StepKinematics_PairRepresentationRelationship::RepresentationRelationshipWithTransformation () const
{
  return myRepresentationRelationshipWithTransformation;
}

//=======================================================================
//function : SetRepresentationRelationshipWithTransformation
//purpose  :
//=======================================================================
void StepKinematics_PairRepresentationRelationship::SetRepresentationRelationshipWithTransformation (const Handle(StepRepr_RepresentationRelationshipWithTransformation)& theRepresentationRelationshipWithTransformation)
{
  myRepresentationRelationshipWithTransformation = theRepresentationRelationshipWithTransformation;
}
