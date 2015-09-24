// Created on: 2015-07-10
// Created by: Irina KRYLOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _StepAP242_ItemIdentifiedRepresentationUsageDefinition_HeaderFile
#define _StepAP242_ItemIdentifiedRepresentationUsageDefinition_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class StepAP214_AppliedApprovalAssignment;
class StepAP214_AppliedDateAndTimeAssignment;
class StepAP214_AppliedDateAssignment;
class StepAP214_AppliedDocumentReference;
class StepAP214_AppliedExternalIdentificationAssignment;
class StepAP214_AppliedGroupAssignment;
class StepAP214_AppliedOrganizationAssignment;
class StepAP214_AppliedPersonAndOrganizationAssignment;
class StepAP214_AppliedSecurityClassificationAssignment;
class StepShape_DimensionalSize;
class StepBasic_GeneralProperty;
class StepDimTol_GeometricTolerance;
class StepBasic_ProductDefinitionRelationship;
class StepRepr_PropertyDefinition;
class StepRepr_PropertyDefinitionRelationship;
class StepRepr_ShapeAspect;
class StepRepr_ShapeAspectRelationship;

class StepAP242_ItemIdentifiedRepresentationUsageDefinition  : public StepData_SelectType
{
public:

  DEFINE_STANDARD_ALLOC
  
  //! Returns a ItemIdentifiedRepresentationUsageDefinition select type
  Standard_EXPORT StepAP242_ItemIdentifiedRepresentationUsageDefinition();
  
  //! Recognizes a ItemIdentifiedRepresentationUsageDefinition Kind Entity that is :
  //! 1 -> AppliedApprovalAssignment
  //! 2 -> AppliedDateAndTimeAssignment
  //! 3 -> AppliedDateAssignment
  //! 4 -> AppliedDocumentReference
  //! 5 -> AppliedExternalIdentificationAssignment
  //! 6 -> AppliedGroupAssignment
  //! 7 -> AppliedOrganizationAssignment
  //! 8 -> AppliedPersonAndOrganizationAssignment
  //! 9 -> AppliedSecurityClassificationAssignment
  //! 10 -> DimensionalSize
  //! 11 -> GeneralProperty
  //! 12 -> GeometricTolerance
  //! 13 -> ProductDefinitionRelationship
  //! 14 -> PropertyDefinition
  //! 15 -> PropertyDefinitionRelationship
  //! 16 -> ShapeAspect
  //! 17 -> ShapeAspectRelationship
  //! 0 else
  Standard_EXPORT Standard_Integer CaseNum (const Handle(Standard_Transient)& ent)  const;
  
  //! returns Value as a AppliedApprovalAssignment (Null if another type)
  Standard_EXPORT Handle(StepAP214_AppliedApprovalAssignment) AppliedApprovalAssignment()  const;
  
  //! returns Value as a AppliedDateAndTimeAssignment (Null if another type)
  Standard_EXPORT Handle(StepAP214_AppliedDateAndTimeAssignment) AppliedDateAndTimeAssignment()  const;
  
  //! returns Value as a AppliedDateAssignment (Null if another type)
  Standard_EXPORT Handle(StepAP214_AppliedDateAssignment) AppliedDateAssignment()  const;
  
  //! returns Value as a AppliedDocumentReference (Null if another type)
  Standard_EXPORT Handle(StepAP214_AppliedDocumentReference) AppliedDocumentReference()  const;
  
  //! returns Value as a AppliedExternalIdentificationAssignment (Null if another type)
  Standard_EXPORT Handle(StepAP214_AppliedExternalIdentificationAssignment) AppliedExternalIdentificationAssignment()  const;
  
  //! returns Value as a AppliedGroupAssignment (Null if another type)
  Standard_EXPORT Handle(StepAP214_AppliedGroupAssignment) AppliedGroupAssignment()  const;
  
  //! returns Value as a AppliedOrganizationAssignment (Null if another type)
  Standard_EXPORT Handle(StepAP214_AppliedOrganizationAssignment) AppliedOrganizationAssignment()  const;
  
  //! returns Value as a AppliedPersonAndOrganizationAssignment (Null if another type)
  Standard_EXPORT Handle(StepAP214_AppliedPersonAndOrganizationAssignment) AppliedPersonAndOrganizationAssignment()  const;
  
  //! returns Value as a AppliedSecurityClassificationAssignment (Null if another type)
  Standard_EXPORT Handle(StepAP214_AppliedSecurityClassificationAssignment) AppliedSecurityClassificationAssignment()  const;
  
  //! returns Value as a DimensionalSize (Null if another type)
  Standard_EXPORT Handle(StepShape_DimensionalSize) DimensionalSize()  const;
  
  //! returns Value as a GeneralProperty (Null if another type)
  Standard_EXPORT Handle(StepBasic_GeneralProperty) GeneralProperty()  const;
  
  //! returns Value as a GeometricTolerance (Null if another type)
  Standard_EXPORT Handle(StepDimTol_GeometricTolerance) GeometricTolerance()  const;
  
  //! returns Value as a ProductDefinitionRelationship (Null if another type)
  Standard_EXPORT Handle(StepBasic_ProductDefinitionRelationship) ProductDefinitionRelationship()  const;
  
  //! returns Value as a PropertyDefinition (Null if another type)
  Standard_EXPORT Handle(StepRepr_PropertyDefinition) PropertyDefinition()  const;
  
  //! returns Value as a PropertyDefinitionRelationship (Null if another type)
  Standard_EXPORT Handle(StepRepr_PropertyDefinitionRelationship) PropertyDefinitionRelationship()  const;
  
  //! returns Value as a ShapeAspect (Null if another type)
  Standard_EXPORT Handle(StepRepr_ShapeAspect) ShapeAspect()  const;
  
  //! returns Value as a ShapeAspectRelationship (Null if another type)
  Standard_EXPORT Handle(StepRepr_ShapeAspectRelationship) ShapeAspectRelationship()  const;

};
#endif // _StepAP242_ItemIdentifiedRepresentationUsageDefinition_HeaderFile
