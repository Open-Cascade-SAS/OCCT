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

#include <StepKinematics_ContextDependentKinematicLinkRepresentation.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_ContextDependentKinematicLinkRepresentation, Standard_Transient)

//=======================================================================
//function : StepKinematics_ContextDependentKinematicLinkRepresentation
//purpose  :
//=======================================================================
StepKinematics_ContextDependentKinematicLinkRepresentation::StepKinematics_ContextDependentKinematicLinkRepresentation ()
{
}

//=======================================================================
//function : Init
//purpose  :
//=======================================================================
void StepKinematics_ContextDependentKinematicLinkRepresentation::Init (const Handle(StepKinematics_KinematicLinkRepresentationAssociation)& theRepresentationRelation,
                                                                       const Handle(StepKinematics_ProductDefinitionRelationshipKinematics)& theRepresentedProductRelation)
{

  myRepresentationRelation = theRepresentationRelation;

  myRepresentedProductRelation = theRepresentedProductRelation;
}

//=======================================================================
//function : RepresentationRelation
//purpose  :
//=======================================================================
Handle(StepKinematics_KinematicLinkRepresentationAssociation) StepKinematics_ContextDependentKinematicLinkRepresentation::RepresentationRelation () const
{
  return myRepresentationRelation;
}

//=======================================================================
//function : SetRepresentationRelation
//purpose  :
//=======================================================================
void StepKinematics_ContextDependentKinematicLinkRepresentation::SetRepresentationRelation (const Handle(StepKinematics_KinematicLinkRepresentationAssociation)& theRepresentationRelation)
{
  myRepresentationRelation = theRepresentationRelation;
}

//=======================================================================
//function : RepresentedProductRelation
//purpose  :
//=======================================================================
Handle(StepKinematics_ProductDefinitionRelationshipKinematics) StepKinematics_ContextDependentKinematicLinkRepresentation::RepresentedProductRelation () const
{
  return myRepresentedProductRelation;
}

//=======================================================================
//function : SetRepresentedProductRelation
//purpose  :
//=======================================================================
void StepKinematics_ContextDependentKinematicLinkRepresentation::SetRepresentedProductRelation (const Handle(StepKinematics_ProductDefinitionRelationshipKinematics)& theRepresentedProductRelation)
{
  myRepresentedProductRelation = theRepresentedProductRelation;
}
