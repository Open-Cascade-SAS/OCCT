// Created on: 1999-11-18
// Created by: Andrey BETENEV
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

#ifndef _STEPConstruct_AP203Context_HeaderFile
#define _STEPConstruct_AP203Context_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class StepBasic_Approval;
class StepBasic_DateAndTime;
class StepBasic_PersonAndOrganization;
class StepBasic_SecurityClassificationLevel;
class StepBasic_PersonAndOrganizationRole;
class StepBasic_DateTimeRole;
class StepBasic_ApprovalRole;
class StepAP203_CcDesignPersonAndOrganizationAssignment;
class StepAP203_CcDesignSecurityClassification;
class StepAP203_CcDesignDateAndTimeAssignment;
class StepAP203_CcDesignApproval;
class StepBasic_ApprovalPersonOrganization;
class StepBasic_ApprovalDateTime;
class StepBasic_ProductCategoryRelationship;
class StepShape_ShapeDefinitionRepresentation;
class STEPConstruct_Part;
class StepRepr_NextAssemblyUsageOccurrence;

//! Maintains context specific for AP203 (required data and
//! management information such as persons, dates, approvals etc.)
//! It contains static entities (which can be shared), default
//! values for person and organisation, and also provides
//! tool for creating management entities around specific part (SDR).
class STEPConstruct_AP203Context
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates tool and fills constant fields
  Standard_EXPORT STEPConstruct_AP203Context();

  //! Returns default approval entity which
  //! is used when no other data are available
  Standard_EXPORT Handle(StepBasic_Approval) DefaultApproval();

  //! Sets default approval
  Standard_EXPORT void SetDefaultApproval(const Handle(StepBasic_Approval)& app);

  //! Returns default date_and_time entity which
  //! is used when no other data are available
  Standard_EXPORT Handle(StepBasic_DateAndTime) DefaultDateAndTime();

  //! Sets default date_and_time entity
  Standard_EXPORT void SetDefaultDateAndTime(const Handle(StepBasic_DateAndTime)& dt);

  //! Returns default person_and_organization entity which
  //! is used when no other data are available
  Standard_EXPORT Handle(StepBasic_PersonAndOrganization) DefaultPersonAndOrganization();

  //! Sets default person_and_organization entity
  Standard_EXPORT void SetDefaultPersonAndOrganization(
    const Handle(StepBasic_PersonAndOrganization)& po);

  //! Returns default security_classification_level entity which
  //! is used when no other data are available
  Standard_EXPORT Handle(StepBasic_SecurityClassificationLevel)
    DefaultSecurityClassificationLevel();

  //! Sets default security_classification_level
  Standard_EXPORT void SetDefaultSecurityClassificationLevel(
    const Handle(StepBasic_SecurityClassificationLevel)& sc);

  Standard_EXPORT Handle(StepBasic_PersonAndOrganizationRole) RoleCreator() const;

  Standard_EXPORT Handle(StepBasic_PersonAndOrganizationRole) RoleDesignOwner() const;

  Standard_EXPORT Handle(StepBasic_PersonAndOrganizationRole) RoleDesignSupplier() const;

  Standard_EXPORT Handle(StepBasic_PersonAndOrganizationRole) RoleClassificationOfficer() const;

  Standard_EXPORT Handle(StepBasic_DateTimeRole) RoleCreationDate() const;

  Standard_EXPORT Handle(StepBasic_DateTimeRole) RoleClassificationDate() const;

  //! Return predefined PersonAndOrganizationRole and DateTimeRole
  //! entities named 'creator', 'design owner', 'design supplier',
  //! 'classification officer', 'creation date', 'classification date',
  //! 'approver'
  Standard_EXPORT Handle(StepBasic_ApprovalRole) RoleApprover() const;

  //! Takes SDR (part) which brings all standard data around part
  //! (common for AP203 and AP214) and creates all the additional
  //! entities required for AP203
  Standard_EXPORT void Init(const Handle(StepShape_ShapeDefinitionRepresentation)& sdr);

  //! Takes tool which describes standard data around part
  //! (common for AP203 and AP214) and creates all the additional
  //! entities required for AP203
  //!
  //! The created entities can be obtained by calls to methods
  //! GetCreator(), GetDesignOwner(), GetDesignSupplier(),
  //! GetClassificationOfficer(), GetSecurity(), GetCreationDate(),
  //! GetClassificationDate(), GetApproval(),
  //! GetApprover(), GetApprovalDateTime(),
  //! GetProductCategoryRelationship()
  Standard_EXPORT void Init(const STEPConstruct_Part& SDRTool);

  //! Takes NAUO which describes assembly link to component
  //! and creates the security_classification entity associated to
  //! it as required by the AP203
  //!
  //! Instantiated (or existing previously) entities concerned
  //! can be obtained by calls to methods
  //! GetClassificationOfficer(), GetSecurity(),
  //! GetClassificationDate(), GetApproval(),
  //! GetApprover(), GetApprovalDateTime()
  //! Takes tool which describes standard data around part
  //! (common for AP203 and AP214) and takes from model (or creates
  //! if missing) all the additional entities required by AP203
  Standard_EXPORT void Init(const Handle(StepRepr_NextAssemblyUsageOccurrence)& nauo);

  Standard_EXPORT Handle(StepAP203_CcDesignPersonAndOrganizationAssignment) GetCreator() const;

  Standard_EXPORT Handle(StepAP203_CcDesignPersonAndOrganizationAssignment) GetDesignOwner() const;

  Standard_EXPORT Handle(StepAP203_CcDesignPersonAndOrganizationAssignment) GetDesignSupplier()
    const;

  Standard_EXPORT Handle(StepAP203_CcDesignPersonAndOrganizationAssignment)
    GetClassificationOfficer() const;

  Standard_EXPORT Handle(StepAP203_CcDesignSecurityClassification) GetSecurity() const;

  Standard_EXPORT Handle(StepAP203_CcDesignDateAndTimeAssignment) GetCreationDate() const;

  Standard_EXPORT Handle(StepAP203_CcDesignDateAndTimeAssignment) GetClassificationDate() const;

  Standard_EXPORT Handle(StepAP203_CcDesignApproval) GetApproval() const;

  Standard_EXPORT Handle(StepBasic_ApprovalPersonOrganization) GetApprover() const;

  Standard_EXPORT Handle(StepBasic_ApprovalDateTime) GetApprovalDateTime() const;

  //! Return entities (roots) instantiated for the part by method Init
  Standard_EXPORT Handle(StepBasic_ProductCategoryRelationship) GetProductCategoryRelationship()
    const;

  //! Clears all fields describing entities specific to each part
  Standard_EXPORT void Clear();

  //! Initializes constant fields (shared entities)
  Standard_EXPORT void InitRoles();

  //! Initializes all missing data which are required for assembly
  Standard_EXPORT void InitAssembly(const Handle(StepRepr_NextAssemblyUsageOccurrence)& nauo);

  //! Initializes ClassificationOfficer and ClassificationDate
  //! entities according to Security entity
  Standard_EXPORT void InitSecurityRequisites();

  //! Initializes Approver and ApprovalDateTime
  //! entities according to Approval entity
  Standard_EXPORT void InitApprovalRequisites();

protected:
private:
  //! Initializes all missing data which are required for part
  Standard_EXPORT void InitPart(const STEPConstruct_Part& SDRTool);

  Handle(StepBasic_Approval)                                defApproval;
  Handle(StepBasic_DateAndTime)                             defDateAndTime;
  Handle(StepBasic_PersonAndOrganization)                   defPersonAndOrganization;
  Handle(StepBasic_SecurityClassificationLevel)             defSecurityClassificationLevel;
  Handle(StepBasic_PersonAndOrganizationRole)               roleCreator;
  Handle(StepBasic_PersonAndOrganizationRole)               roleDesignOwner;
  Handle(StepBasic_PersonAndOrganizationRole)               roleDesignSupplier;
  Handle(StepBasic_PersonAndOrganizationRole)               roleClassificationOfficer;
  Handle(StepBasic_DateTimeRole)                            roleCreationDate;
  Handle(StepBasic_DateTimeRole)                            roleClassificationDate;
  Handle(StepBasic_ApprovalRole)                            roleApprover;
  Handle(StepAP203_CcDesignPersonAndOrganizationAssignment) myCreator;
  Handle(StepAP203_CcDesignPersonAndOrganizationAssignment) myDesignOwner;
  Handle(StepAP203_CcDesignPersonAndOrganizationAssignment) myDesignSupplier;
  Handle(StepAP203_CcDesignPersonAndOrganizationAssignment) myClassificationOfficer;
  Handle(StepAP203_CcDesignSecurityClassification)          mySecurity;
  Handle(StepAP203_CcDesignDateAndTimeAssignment)           myCreationDate;
  Handle(StepAP203_CcDesignDateAndTimeAssignment)           myClassificationDate;
  Handle(StepAP203_CcDesignApproval)                        myApproval;
  Handle(StepBasic_ApprovalPersonOrganization)              myApprover;
  Handle(StepBasic_ApprovalDateTime)                        myApprovalDateTime;
  Handle(StepBasic_ProductCategoryRelationship)             myProductCategoryRelationship;
};

#endif // _STEPConstruct_AP203Context_HeaderFile
