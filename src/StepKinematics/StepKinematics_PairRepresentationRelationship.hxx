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

#ifndef _StepKinematics_PairRepresentationRelationship_HeaderFile_
#define _StepKinematics_PairRepresentationRelationship_HeaderFile_

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <StepGeom_GeometricRepresentationItem.hxx>

#include <TCollection_HAsciiString.hxx>
#include <StepRepr_RepresentationOrRepresentationReference.hxx>
#include <StepRepr_Transformation.hxx>
#include <StepRepr_RepresentationRelationshipWithTransformation.hxx>

DEFINE_STANDARD_HANDLE(StepKinematics_PairRepresentationRelationship, StepGeom_GeometricRepresentationItem)

//! Representation of STEP entity PairRepresentationRelationship
class StepKinematics_PairRepresentationRelationship : public StepGeom_GeometricRepresentationItem
{
public :

  //! default constructor
  Standard_EXPORT StepKinematics_PairRepresentationRelationship();

  //! Initialize all fields (own and inherited)
 Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
                           const Handle(TCollection_HAsciiString)& theRepresentationRelationship_Name,
                           const Standard_Boolean hasRepresentationRelationship_Description,
                           const Handle(TCollection_HAsciiString)& theRepresentationRelationship_Description,
                           const StepRepr_RepresentationOrRepresentationReference& theRepresentationRelationship_Rep1,
                           const StepRepr_RepresentationOrRepresentationReference& theRepresentationRelationship_Rep2,
                           const StepRepr_Transformation& theRepresentationRelationshipWithTransformation_TransformationOperator);

  //! Returns data for supertype RepresentationRelationshipWithTransformation
  Standard_EXPORT Handle(StepRepr_RepresentationRelationshipWithTransformation) RepresentationRelationshipWithTransformation() const;
  //! Sets data for supertype RepresentationRelationshipWithTransformation
  Standard_EXPORT void SetRepresentationRelationshipWithTransformation (const Handle(StepRepr_RepresentationRelationshipWithTransformation)& theRepresentationRelationshipWithTransformation);

DEFINE_STANDARD_RTTIEXT(StepKinematics_PairRepresentationRelationship, StepGeom_GeometricRepresentationItem)

private:
  Handle(StepRepr_RepresentationRelationshipWithTransformation) myRepresentationRelationshipWithTransformation; //!< supertype

};
#endif // _StepKinematics_PairRepresentationRelationship_HeaderFile_
