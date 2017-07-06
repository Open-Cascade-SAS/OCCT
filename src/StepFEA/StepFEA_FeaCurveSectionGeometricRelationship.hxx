// Created on: 2003-01-22
// Created by: data exchange team
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#ifndef _StepFEA_FeaCurveSectionGeometricRelationship_HeaderFile
#define _StepFEA_FeaCurveSectionGeometricRelationship_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class StepElement_CurveElementSectionDefinition;
class StepElement_AnalysisItemWithinRepresentation;


class StepFEA_FeaCurveSectionGeometricRelationship;
DEFINE_STANDARD_HANDLE(StepFEA_FeaCurveSectionGeometricRelationship, Standard_Transient)

//! Representation of STEP entity FeaCurveSectionGeometricRelationship
class StepFEA_FeaCurveSectionGeometricRelationship : public Standard_Transient
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepFEA_FeaCurveSectionGeometricRelationship();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init (const Handle(StepElement_CurveElementSectionDefinition)& aSectionRef, const Handle(StepElement_AnalysisItemWithinRepresentation)& aItem);
  
  //! Returns field SectionRef
  Standard_EXPORT Handle(StepElement_CurveElementSectionDefinition) SectionRef() const;
  
  //! Set field SectionRef
  Standard_EXPORT void SetSectionRef (const Handle(StepElement_CurveElementSectionDefinition)& SectionRef);
  
  //! Returns field Item
  Standard_EXPORT Handle(StepElement_AnalysisItemWithinRepresentation) Item() const;
  
  //! Set field Item
  Standard_EXPORT void SetItem (const Handle(StepElement_AnalysisItemWithinRepresentation)& Item);




  DEFINE_STANDARD_RTTIEXT(StepFEA_FeaCurveSectionGeometricRelationship,Standard_Transient)

protected:




private:


  Handle(StepElement_CurveElementSectionDefinition) theSectionRef;
  Handle(StepElement_AnalysisItemWithinRepresentation) theItem;


};







#endif // _StepFEA_FeaCurveSectionGeometricRelationship_HeaderFile
