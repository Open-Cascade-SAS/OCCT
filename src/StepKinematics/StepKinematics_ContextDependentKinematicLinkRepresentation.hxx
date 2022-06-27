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

#ifndef _StepKinematics_ContextDependentKinematicLinkRepresentation_HeaderFile_
#define _StepKinematics_ContextDependentKinematicLinkRepresentation_HeaderFile_

#include <Standard.hxx>
#include <Standard_Transient.hxx>

#include <StepKinematics_KinematicLinkRepresentationAssociation.hxx>
#include <StepKinematics_ProductDefinitionRelationshipKinematics.hxx>

DEFINE_STANDARD_HANDLE(StepKinematics_ContextDependentKinematicLinkRepresentation, Standard_Transient)

//! Representation of STEP entity ContextDependentKinematicLinkRepresentation
class StepKinematics_ContextDependentKinematicLinkRepresentation : public Standard_Transient
{
public :

  //! default constructor
  Standard_EXPORT StepKinematics_ContextDependentKinematicLinkRepresentation();

  //! Initialize all fields (own and inherited)
 Standard_EXPORT void Init(const Handle(StepKinematics_KinematicLinkRepresentationAssociation)& theRepresentationRelation,
                           const Handle(StepKinematics_ProductDefinitionRelationshipKinematics)& theRepresentedProductRelation);

  //! Returns field RepresentationRelation
  Standard_EXPORT Handle(StepKinematics_KinematicLinkRepresentationAssociation) RepresentationRelation() const;
  //! Sets field RepresentationRelation
  Standard_EXPORT void SetRepresentationRelation (const Handle(StepKinematics_KinematicLinkRepresentationAssociation)& theRepresentationRelation);

  //! Returns field RepresentedProductRelation
  Standard_EXPORT Handle(StepKinematics_ProductDefinitionRelationshipKinematics) RepresentedProductRelation() const;
  //! Sets field RepresentedProductRelation
  Standard_EXPORT void SetRepresentedProductRelation (const Handle(StepKinematics_ProductDefinitionRelationshipKinematics)& theRepresentedProductRelation);

DEFINE_STANDARD_RTTIEXT(StepKinematics_ContextDependentKinematicLinkRepresentation, Standard_Transient)

private:
  Handle(StepKinematics_KinematicLinkRepresentationAssociation) myRepresentationRelation;
  Handle(StepKinematics_ProductDefinitionRelationshipKinematics) myRepresentedProductRelation;

};
#endif // _StepKinematics_ContextDependentKinematicLinkRepresentation_HeaderFile_
