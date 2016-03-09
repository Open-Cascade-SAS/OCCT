// Created on: 1999-03-09
// Created by: data exchange team
// Copyright (c) 1999 Matra Datavision
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

#ifndef _StepAP214_DateAndTimeItem_HeaderFile
#define _StepAP214_DateAndTimeItem_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepAP214_ApprovalItem.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class StepBasic_ApprovalPersonOrganization;
class StepAP214_AppliedPersonAndOrganizationAssignment;
class StepAP214_AppliedOrganizationAssignment;


class StepAP214_DateAndTimeItem  : public StepAP214_ApprovalItem
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns a DateAndTimeItem SelectType
  Standard_EXPORT StepAP214_DateAndTimeItem();
  
  //! Recognizes a DateAndTimeItem Kind Entity that is :
  //! 1 -> ApprovalPersonOrganization
  //! 2 -> AppliedDateAndPersonAssignment
  //! 3 -> AppliedOrganizationAssignment
  //! 4 -> AssemblyComponentUsageSubstitute
  //! 5 -> DocumentFile
  //! 6 -> Effectivity
  //! 7 -> MaterialDesignation
  //! 8 -> MechanicalDesignGeometricPresentationRepresentation
  //! 9 -> PresentationArea
  //! 10 -> Product
  //! 11 -> ProductDefinition
  //! 12 -> ProductDefinitionFormation
  //! 13 -> ProductDefinitionRelationship
  //! 14 -> PropertyDefinition
  //! 15 -> ShapeRepresentation
  //! 16 -> SecurityClassification
  //! 0 else
  Standard_EXPORT virtual Standard_Integer CaseNum (const Handle(Standard_Transient)& ent) const Standard_OVERRIDE;
  
  //! returns Value as a ApprovalPersonOrganization (Null if another type)
  Standard_EXPORT Handle(StepBasic_ApprovalPersonOrganization) ApprovalPersonOrganization() const;
  
  //! returns Value as a AppliedDateAndPersonAssignment (Null if another type)
  Standard_EXPORT Handle(StepAP214_AppliedPersonAndOrganizationAssignment) AppliedPersonAndOrganizationAssignment() const;
  
  //! returns Value as a AppliedOrganizationAssignment (Null if another type)
  Standard_EXPORT Handle(StepAP214_AppliedOrganizationAssignment) AppliedOrganizationAssignment() const;

protected:





private:





};







#endif // _StepAP214_DateAndTimeItem_HeaderFile
