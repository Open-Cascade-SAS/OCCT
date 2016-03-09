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

#ifndef _StepAP214_PersonAndOrganizationItem_HeaderFile
#define _StepAP214_PersonAndOrganizationItem_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepAP214_ApprovalItem.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class StepAP214_AppliedOrganizationAssignment;
class StepAP214_AppliedSecurityClassificationAssignment;
class StepBasic_Approval;



class StepAP214_PersonAndOrganizationItem  : public StepAP214_ApprovalItem
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns a PersonAndOrganizationItem SelectType
  Standard_EXPORT StepAP214_PersonAndOrganizationItem();
  
  //! Recognizes a APersonAndOrganizationItem Kind Entity that is :
  //! 1 -> AppliedOrganizationAssignment
  //! 2 -> AssemblyComponentUsageSubstitute
  //! 3 -> DocumentFile
  //! 4 -> MaterialDesignation
  //! 5 -> MechanicalDesignGeometricPresentationRepresentation
  //! 6 -> PresentationArea
  //! 7 -> Product
  //! 8 -> ProductDefinition
  //! 9 -> ProductDefinitionFormation
  //! 10 -> ProductDefinitionRelationship
  //! 11 -> PropertyDefinition
  //! 12 -> ShapeRepresentation
  //! 13 -> SecurityClassification
  //! 14 -> AppliedSecurityClassificationAssignment
  //! 15 -> Approval
  //! 0 else
  Standard_EXPORT virtual Standard_Integer CaseNum (const Handle(Standard_Transient)& ent) const Standard_OVERRIDE;
  
  //! returns Value as a AppliedOrganizationAssignment (Null if another type)
  Standard_EXPORT Handle(StepAP214_AppliedOrganizationAssignment) AppliedOrganizationAssignment() const;

  //! returns Value as a AppliedSecurityClassificationAssignment (Null if another type)
  Standard_EXPORT Handle(StepAP214_AppliedSecurityClassificationAssignment) AppliedSecurityClassificationAssignment() const;

  //! returns Value as a Approval (Null if another type)
  Standard_EXPORT Handle(StepBasic_Approval) Approval() const;

protected:





private:





};







#endif // _StepAP214_PersonAndOrganizationItem_HeaderFile
