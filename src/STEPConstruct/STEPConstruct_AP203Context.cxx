// Created on: 1999-11-19
// Created by: Andrey BETENEV
// Copyright (c) 1999-1999 Matra Datavision
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


#include <StepAP203_CcDesignApproval.hxx>
#include <StepAP203_CcDesignDateAndTimeAssignment.hxx>
#include <StepAP203_CcDesignPersonAndOrganizationAssignment.hxx>
#include <StepAP203_CcDesignSecurityClassification.hxx>
#include <StepBasic_Approval.hxx>
#include <StepBasic_ApprovalDateTime.hxx>
#include <StepBasic_ApprovalPersonOrganization.hxx>
#include <StepBasic_ApprovalRole.hxx>
#include <StepBasic_DateAndTime.hxx>
#include <StepBasic_DateTimeRole.hxx>
#include <StepBasic_PersonAndOrganization.hxx>
#include <StepBasic_PersonAndOrganizationRole.hxx>
#include <StepBasic_ProductCategoryRelationship.hxx>
#include <StepBasic_SecurityClassificationLevel.hxx>
#include <STEPConstruct_AP203Context.hxx>
#include <STEPConstruct_Part.hxx>
#include <StepRepr_NextAssemblyUsageOccurrence.hxx>
#include <StepShape_ShapeDefinitionRepresentation.hxx>

#include <OSD_Host.hxx>
#include <OSD_Process.hxx>
#include <Quantity_Date.hxx>

#include <TCollection_HAsciiString.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>
#include <Interface_HArray1OfHAsciiString.hxx>

#include <StepBasic_CalendarDate.hxx>
#include <StepBasic_LocalTime.hxx>
#include <StepBasic_ApprovalStatus.hxx>
#include <StepBasic_CoordinatedUniversalTimeOffset.hxx>
#include <StepBasic_AheadOrBehind.hxx>
#include <StepBasic_Person.hxx>
#include <StepBasic_Organization.hxx>
#include <StepBasic_SecurityClassification.hxx>
#include <StepBasic_Product.hxx>
#include <StepBasic_ProductDefinition.hxx>
#include <StepBasic_ProductDefinitionFormation.hxx>
#include <StepBasic_ProductRelatedProductCategory.hxx>
#include <StepAP203_HArray1OfPersonOrganizationItem.hxx>
#include <StepAP203_HArray1OfClassifiedItem.hxx>
#include <StepAP203_HArray1OfDateTimeItem.hxx>
#include <StepAP203_HArray1OfApprovedItem.hxx>
#include <StepBasic_ProductCategory.hxx>

#ifndef _WIN32
# include <pwd.h>
#endif

//=======================================================================
//function : STEPConstruct_AP203Context
//purpose  : 
//=======================================================================

STEPConstruct_AP203Context::STEPConstruct_AP203Context ()
{
  InitRoles();
}
	    
//=======================================================================
//function : DefaultApproval
//purpose  : 
//=======================================================================

Handle(StepBasic_Approval) STEPConstruct_AP203Context::DefaultApproval ()
{
  if ( defApproval.IsNull() ) {
    Handle(StepBasic_ApprovalStatus) aStatus = new StepBasic_ApprovalStatus;
    Handle(TCollection_HAsciiString) aName = new TCollection_HAsciiString ("not_yet_approved");
    aStatus->Init ( aName );

    Handle(TCollection_HAsciiString) aLevel = new TCollection_HAsciiString ("");
    defApproval = new StepBasic_Approval;
    defApproval->Init ( aStatus, aLevel );
  }
  return defApproval;
}

//=======================================================================
//function : SetDefaultApproval
//purpose  : 
//=======================================================================

void STEPConstruct_AP203Context::SetDefaultApproval (const Handle(StepBasic_Approval) &app)
{
  defApproval = app;
}
    
//=======================================================================
//function : DefaultDateAndTime
//purpose  : 
//=======================================================================

Handle(StepBasic_DateAndTime) STEPConstruct_AP203Context::DefaultDateAndTime ()
{
  if ( defDateAndTime.IsNull() ) {
    OSD_Process sys;
    Quantity_Date date = sys.SystemDate ();

    Handle(StepBasic_CalendarDate) aDate = new StepBasic_CalendarDate;
    aDate->Init ( date.Year(), date.Day(), date.Month() );

    Handle(StepBasic_CoordinatedUniversalTimeOffset) zone = 
      new StepBasic_CoordinatedUniversalTimeOffset;
  #if defined(_MSC_VER) && _MSC_VER >= 1600
    long shift = 0;
    _get_timezone (&shift);
  #else
    Standard_Integer shift = Standard_Integer(timezone);
  #endif
    Standard_Integer shifth = abs ( shift ) / 3600;
    Standard_Integer shiftm = ( abs ( shift ) - shifth * 3600 ) / 60;
    StepBasic_AheadOrBehind sense = ( shift >0 ? StepBasic_aobBehind : 
				      shift <0 ? StepBasic_aobAhead : 
				                 StepBasic_aobExact );
    zone->Init ( shifth, ( shiftm != 0 ), shiftm, sense );
    
    Handle(StepBasic_LocalTime) aTime = new StepBasic_LocalTime;
    aTime->Init ( date.Hour(), Standard_True, date.Minute(), Standard_False, 0., zone );

    defDateAndTime = new StepBasic_DateAndTime;
    defDateAndTime->Init ( aDate, aTime );
  }
  return defDateAndTime;
}

//=======================================================================
//function : SetDefaultDateAndTime
//purpose  : 
//=======================================================================

void STEPConstruct_AP203Context::SetDefaultDateAndTime (const Handle(StepBasic_DateAndTime) &dt)
{
  defDateAndTime = dt;
}
    
//=======================================================================
//function : DefaultPersonAndOrganization
//purpose  : 
//=======================================================================

Handle(StepBasic_PersonAndOrganization) STEPConstruct_AP203Context::DefaultPersonAndOrganization ()
{
  if ( defPersonAndOrganization.IsNull() ) {
    // get IP address as a unique id of organization
    Handle(TCollection_HAsciiString) orgId = new TCollection_HAsciiString ( "IP" );
    OSD_Host aHost;
    TCollection_AsciiString anIP = aHost.InternetAddress();
    // cut off last number
    Standard_Integer aLastDotIndex = anIP.SearchFromEnd (".");
    if (aLastDotIndex >0)
    {
      anIP.Trunc (aLastDotIndex - 1);
      orgId->AssignCat (anIP.ToCString());
    }
    
    // create organization
    Handle(StepBasic_Organization) aOrg = new StepBasic_Organization;
    Handle(TCollection_HAsciiString) oName = new TCollection_HAsciiString ( "Unspecified" );
    Handle(TCollection_HAsciiString) oDescr = new TCollection_HAsciiString ( "" );
    aOrg->Init ( Standard_True, orgId, oName, oDescr );
    
    // construct person`s name
    OSD_Process sys;
    Standard_CString usr = sys.UserName().ToCString();
#if !defined(_WIN32) && !defined(__ANDROID__)
    if ( usr ) {
      struct passwd *pwd = getpwnam ( usr );
      if ( pwd ) usr = pwd->pw_gecos;
    }
    else usr = "Unknown";
#endif
    TCollection_AsciiString user ( usr );
    Handle(TCollection_HAsciiString) fname = new TCollection_HAsciiString ("");
    Handle(TCollection_HAsciiString) lname = new TCollection_HAsciiString ("");
    Handle(Interface_HArray1OfHAsciiString) mname;
    TColStd_SequenceOfAsciiString names;
    Standard_Integer i; // svv Jan11 2000 : porting on DEC
    for ( i=1; ; i++ ) {
      TCollection_AsciiString token = user.Token ( " \t", i );
      if ( ! token.Length() ) break;
      names.Append ( token );
    }
    if ( names.Length() >0 ) fname->AssignCat ( names.Value(1).ToCString() );
    if ( names.Length() >1 ) lname->AssignCat ( names.Value(names.Length()).ToCString() );
    if ( names.Length() >2 ) {
      mname = new Interface_HArray1OfHAsciiString ( 1, names.Length()-2 );
      for ( i=2; i < names.Length(); i++ ) 
	mname->SetValue ( i-1, new TCollection_HAsciiString ( names.Value(i) ) );
    }
    
    // create a person
    Handle(StepBasic_Person) aPerson = new StepBasic_Person;
    Handle(TCollection_HAsciiString) uid = new TCollection_HAsciiString ( orgId );
    uid->AssignCat ( "," );
    uid->AssignCat (sys.UserName().ToCString());
    Handle(Interface_HArray1OfHAsciiString) suffix, prefix;
    aPerson->Init ( uid, Standard_True, lname, Standard_True, fname, ( ! mname.IsNull() ),
		    mname, Standard_False, suffix, Standard_False, prefix );
 
    defPersonAndOrganization = new StepBasic_PersonAndOrganization;
    defPersonAndOrganization->Init ( aPerson, aOrg );
  }
  return defPersonAndOrganization;  
}

//=======================================================================
//function : SetDefaultPersonAndOrganization
//purpose  : 
//=======================================================================

void STEPConstruct_AP203Context::SetDefaultPersonAndOrganization (const Handle(StepBasic_PersonAndOrganization) &po)
{
  defPersonAndOrganization = po;
}
    
//=======================================================================
//function : DefaultSecurityClassificationLevel
//purpose  : 
//=======================================================================

Handle(StepBasic_SecurityClassificationLevel) STEPConstruct_AP203Context::DefaultSecurityClassificationLevel ()
{
  if ( defSecurityClassificationLevel.IsNull() ) {
    defSecurityClassificationLevel = new StepBasic_SecurityClassificationLevel;
    Handle(TCollection_HAsciiString) levName = new TCollection_HAsciiString ( "unclassified" );
    defSecurityClassificationLevel->Init ( levName );
  }
  return defSecurityClassificationLevel;  
}

//=======================================================================
//function : SetDefaultSecurityClassificationLevel
//purpose  : 
//=======================================================================

void STEPConstruct_AP203Context::SetDefaultSecurityClassificationLevel (const Handle(StepBasic_SecurityClassificationLevel) &scl)
{
  defSecurityClassificationLevel = scl;
}

//=======================================================================
//function : RoleCreator
//purpose  : 
//=======================================================================

Handle(StepBasic_PersonAndOrganizationRole) STEPConstruct_AP203Context::RoleCreator () const 
{
  return roleCreator;
}

//=======================================================================
//function : RoleDesignOwner
//purpose  : 
//=======================================================================

Handle(StepBasic_PersonAndOrganizationRole) STEPConstruct_AP203Context::RoleDesignOwner () const
{
  return roleDesignOwner;
}

//=======================================================================
//function : RoleDesignSupplier
//purpose  : 
//=======================================================================

Handle(StepBasic_PersonAndOrganizationRole) STEPConstruct_AP203Context::RoleDesignSupplier () const
{
  return roleDesignSupplier;
}

//=======================================================================
//function : RoleClassificationOfficer
//purpose  : 
//=======================================================================

Handle(StepBasic_PersonAndOrganizationRole) STEPConstruct_AP203Context::RoleClassificationOfficer () const
{
  return roleClassificationOfficer;
}

//=======================================================================
//function : RoleCreationDate
//purpose  : 
//=======================================================================

Handle(StepBasic_DateTimeRole) STEPConstruct_AP203Context::RoleCreationDate () const
{
  return roleCreationDate;
}

//=======================================================================
//function : RoleClassificationDate
//purpose  : 
//=======================================================================

Handle(StepBasic_DateTimeRole) STEPConstruct_AP203Context::RoleClassificationDate () const
{
  return roleClassificationDate;
}

//=======================================================================
//function : RoleApprover
//purpose  : 
//=======================================================================

Handle(StepBasic_ApprovalRole) STEPConstruct_AP203Context::RoleApprover () const
{
  return roleApprover;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void STEPConstruct_AP203Context::Init (const Handle(StepShape_ShapeDefinitionRepresentation) &sdr)
{
  Clear();
  STEPConstruct_Part SDRTool;
  SDRTool.ReadSDR ( sdr );
  InitPart ( SDRTool );
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void STEPConstruct_AP203Context::Init (const STEPConstruct_Part &SDRTool)
{
  Clear();
  InitPart ( SDRTool );
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

//void STEPConstruct_AP203Context::Init (const STEPConstruct_Part &SDRTool, const Handle(Interface_Model) &Model) {}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void STEPConstruct_AP203Context::Init (const Handle(StepRepr_NextAssemblyUsageOccurrence) &NAUO)
{
  Clear();
  InitAssembly ( NAUO );
}

//=======================================================================
//function : GetCreator
//purpose  : 
//=======================================================================

Handle(StepAP203_CcDesignPersonAndOrganizationAssignment) STEPConstruct_AP203Context::GetCreator () const
{
  return myCreator;
}

//=======================================================================
//function : GetDesignOwner
//purpose  : 
//=======================================================================

Handle(StepAP203_CcDesignPersonAndOrganizationAssignment) STEPConstruct_AP203Context::GetDesignOwner () const
{
  return myDesignOwner;
}

//=======================================================================
//function : GetDesignSupplier
//purpose  : 
//=======================================================================

Handle(StepAP203_CcDesignPersonAndOrganizationAssignment) STEPConstruct_AP203Context::GetDesignSupplier () const
{
  return myDesignSupplier;
}

//=======================================================================
//function : GetClassificationOfficer
//purpose  : 
//=======================================================================

Handle(StepAP203_CcDesignPersonAndOrganizationAssignment) STEPConstruct_AP203Context::GetClassificationOfficer () const
{
  return myClassificationOfficer;
}

//=======================================================================
//function : GetSecurity
//purpose  : 
//=======================================================================

Handle(StepAP203_CcDesignSecurityClassification) STEPConstruct_AP203Context::GetSecurity () const
{
  return mySecurity;
}

//=======================================================================
//function : GetCreationDate
//purpose  : 
//=======================================================================

Handle(StepAP203_CcDesignDateAndTimeAssignment) STEPConstruct_AP203Context::GetCreationDate () const
{
  return myCreationDate;
}

//=======================================================================
//function : GetClassificationDate
//purpose  : 
//=======================================================================

Handle(StepAP203_CcDesignDateAndTimeAssignment) STEPConstruct_AP203Context::GetClassificationDate () const
{
  return myClassificationDate;
}

//=======================================================================
//function : GetApproval
//purpose  : 
//=======================================================================

Handle(StepAP203_CcDesignApproval) STEPConstruct_AP203Context::GetApproval () const
{
  return myApproval;
}

//=======================================================================
//function : GetApprover
//purpose  : 
//=======================================================================

Handle(StepBasic_ApprovalPersonOrganization) STEPConstruct_AP203Context::GetApprover () const
{
  return myApprover;
}

//=======================================================================
//function : GetApprovalDateTime
//purpose  : 
//=======================================================================

Handle(StepBasic_ApprovalDateTime) STEPConstruct_AP203Context::GetApprovalDateTime () const
{
  return myApprovalDateTime;
}

//=======================================================================
//function : GetProductCategoryRelationship
//purpose  : 
//=======================================================================

Handle(StepBasic_ProductCategoryRelationship) STEPConstruct_AP203Context::GetProductCategoryRelationship () const
{
  return myProductCategoryRelationship;
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void STEPConstruct_AP203Context::Clear ()
{
  myCreator.Nullify();
  myDesignOwner.Nullify();
  myDesignSupplier.Nullify();
  myClassificationOfficer.Nullify();
  mySecurity.Nullify();
  myCreationDate.Nullify();
  myClassificationDate.Nullify();
  myApproval.Nullify();
  
//  myApprover.Nullify();
//  myApprovalDateTime.Nullify();
  
  myProductCategoryRelationship.Nullify();
}

//=======================================================================
//function : InitRoles
//purpose  : 
//=======================================================================

void STEPConstruct_AP203Context::InitRoles ()
{
  roleCreator = new StepBasic_PersonAndOrganizationRole;
  roleDesignOwner = new StepBasic_PersonAndOrganizationRole;
  roleDesignSupplier = new StepBasic_PersonAndOrganizationRole;
  roleClassificationOfficer = new StepBasic_PersonAndOrganizationRole;
  roleCreationDate = new StepBasic_DateTimeRole;
  roleClassificationDate = new StepBasic_DateTimeRole;
  roleApprover = new StepBasic_ApprovalRole;
  
  roleCreator->Init ( new TCollection_HAsciiString ( "creator" ) );
  roleDesignOwner->Init ( new TCollection_HAsciiString ( "design_owner" ) );
  roleDesignSupplier->Init ( new TCollection_HAsciiString ( "design_supplier" ) );
  roleClassificationOfficer->Init ( new TCollection_HAsciiString ( "classification_officer" ) );
  roleCreationDate->Init ( new TCollection_HAsciiString ( "creation_date" ) );
  roleClassificationDate->Init ( new TCollection_HAsciiString ( "classification_date" ) );
  roleApprover->Init ( new TCollection_HAsciiString ( "approver" ) );
}

//=======================================================================
//function : InitPart
//purpose  : 
//=======================================================================

void STEPConstruct_AP203Context::InitPart (const STEPConstruct_Part &SDRTool)
{
  if ( myCreator.IsNull() ) {
    myCreator = new StepAP203_CcDesignPersonAndOrganizationAssignment;
    Handle(StepAP203_HArray1OfPersonOrganizationItem) items = 
      new StepAP203_HArray1OfPersonOrganizationItem (1, 2);
    items->ChangeValue(1).SetValue ( SDRTool.PDF() );
    items->ChangeValue(2).SetValue ( SDRTool.PD() );
    myCreator->Init ( DefaultPersonAndOrganization(), RoleCreator(), items );
  }
  
  if ( myDesignOwner.IsNull() ) {
    myDesignOwner = new StepAP203_CcDesignPersonAndOrganizationAssignment;
    Handle(StepAP203_HArray1OfPersonOrganizationItem) items = 
      new StepAP203_HArray1OfPersonOrganizationItem (1, 1);
    items->ChangeValue(1).SetValue ( SDRTool.Product() );
    myDesignOwner->Init ( DefaultPersonAndOrganization(), RoleDesignOwner(), items );
  }
  
  if ( myDesignSupplier.IsNull() ) {
    myDesignSupplier = new StepAP203_CcDesignPersonAndOrganizationAssignment;
    Handle(StepAP203_HArray1OfPersonOrganizationItem) items = 
      new StepAP203_HArray1OfPersonOrganizationItem (1, 1);
    items->ChangeValue(1).SetValue ( SDRTool.PDF() );
    myDesignSupplier->Init ( DefaultPersonAndOrganization(), RoleDesignSupplier(), items );
  }
  
  if ( myCreationDate.IsNull() ) {
    myCreationDate = new StepAP203_CcDesignDateAndTimeAssignment;
    Handle(StepAP203_HArray1OfDateTimeItem) items = 
      new StepAP203_HArray1OfDateTimeItem (1, 1);
    items->ChangeValue(1).SetValue ( SDRTool.PD() );
    myCreationDate->Init ( DefaultDateAndTime(), RoleCreationDate(), items );
  }
  
  if ( mySecurity.IsNull() ) {
    
    Handle(TCollection_HAsciiString) aName = new TCollection_HAsciiString ( "" );
    Handle(TCollection_HAsciiString) aPurpose = new TCollection_HAsciiString ( "" );
    Handle(StepBasic_SecurityClassification) sc = new StepBasic_SecurityClassification;
    sc->Init ( aName, aPurpose, DefaultSecurityClassificationLevel() );

    mySecurity = new StepAP203_CcDesignSecurityClassification;
    Handle(StepAP203_HArray1OfClassifiedItem) items = 
      new StepAP203_HArray1OfClassifiedItem (1, 1);
    items->ChangeValue(1).SetValue ( SDRTool.PDF() );
    mySecurity->Init ( sc, items );
  }
  InitSecurityRequisites();
  
  if ( myApproval.IsNull() ) {
    myApproval = new StepAP203_CcDesignApproval;
    Handle(StepAP203_HArray1OfApprovedItem) items = 
      new StepAP203_HArray1OfApprovedItem (1, 3);
    items->ChangeValue(1).SetValue ( SDRTool.PDF() );
    items->ChangeValue(2).SetValue ( SDRTool.PD() );
    items->ChangeValue(3).SetValue ( mySecurity->AssignedSecurityClassification() );
    myApproval->Init ( DefaultApproval(), items );
  }
  InitApprovalRequisites();

  if ( myProductCategoryRelationship.IsNull() ) {
    Handle(StepBasic_ProductCategory) PC = new StepBasic_ProductCategory;
    Handle(TCollection_HAsciiString) PCName = new TCollection_HAsciiString ( "part" );
    PC->Init ( PCName, Standard_False, 0 );
    
    myProductCategoryRelationship = new StepBasic_ProductCategoryRelationship;
    Handle(TCollection_HAsciiString) PCRName = new TCollection_HAsciiString ( "" );
    Handle(TCollection_HAsciiString) PCRDescr = new TCollection_HAsciiString ( "" );
    myProductCategoryRelationship->Init ( PCRName, Standard_True, PCRDescr, PC, SDRTool.PRPC() );
  }
}

//=======================================================================
//function : InitAssembly
//purpose  : 
//=======================================================================

void STEPConstruct_AP203Context::InitAssembly (const Handle(StepRepr_NextAssemblyUsageOccurrence) &NAUO)
{
  if ( mySecurity.IsNull() ) {
    
    Handle(TCollection_HAsciiString) aName = new TCollection_HAsciiString ( "" );
    Handle(TCollection_HAsciiString) aPurpose = new TCollection_HAsciiString ( "" );
    Handle(StepBasic_SecurityClassification) sc = new StepBasic_SecurityClassification;
    sc->Init ( aName, aPurpose, DefaultSecurityClassificationLevel() );

    mySecurity = new StepAP203_CcDesignSecurityClassification;
    Handle(StepAP203_HArray1OfClassifiedItem) items = 
      new StepAP203_HArray1OfClassifiedItem (1, 1);
    items->ChangeValue(1).SetValue ( NAUO );
    mySecurity->Init ( sc, items );
  }
  InitSecurityRequisites();
  
  if ( myApproval.IsNull() ) {
    myApproval = new StepAP203_CcDesignApproval;
    Handle(StepAP203_HArray1OfApprovedItem) items = 
      new StepAP203_HArray1OfApprovedItem (1, 1);
    items->ChangeValue(1).SetValue ( mySecurity->AssignedSecurityClassification() );
    myApproval->Init ( DefaultApproval(), items );
  }  
  InitApprovalRequisites();
}

//=======================================================================
//function : InitSecurityRequisites
//purpose  : 
//=======================================================================

void STEPConstruct_AP203Context::InitSecurityRequisites ()
{
  if ( myClassificationOfficer.IsNull() ||
       myClassificationOfficer->Items()->Value(1).Value() != mySecurity->AssignedSecurityClassification() ) {
    myClassificationOfficer = new StepAP203_CcDesignPersonAndOrganizationAssignment;
    Handle(StepAP203_HArray1OfPersonOrganizationItem) items = 
      new StepAP203_HArray1OfPersonOrganizationItem (1, 1);
    items->ChangeValue(1).SetValue ( mySecurity->AssignedSecurityClassification() );
    myClassificationOfficer->Init ( DefaultPersonAndOrganization(), RoleClassificationOfficer(), items );
  }
  
  if ( myClassificationDate.IsNull() ||
       myClassificationDate->Items()->Value(1).Value() != mySecurity->AssignedSecurityClassification() ) {
    myClassificationDate = new StepAP203_CcDesignDateAndTimeAssignment;
    Handle(StepAP203_HArray1OfDateTimeItem) items = 
      new StepAP203_HArray1OfDateTimeItem (1, 1);
    items->ChangeValue(1).SetValue ( mySecurity->AssignedSecurityClassification() );
    myClassificationDate->Init ( DefaultDateAndTime(), RoleClassificationDate(), items );
  }
}
  
//=======================================================================
//function : InitApprovalRequisites
//purpose  : 
//=======================================================================

void STEPConstruct_AP203Context::InitApprovalRequisites ()
{
  if ( myApprover.IsNull() || 
       myApprover->AuthorizedApproval() != myApproval->AssignedApproval() ) {
    myApprover = new StepBasic_ApprovalPersonOrganization;
    StepBasic_PersonOrganizationSelect po;
    po.SetValue ( DefaultPersonAndOrganization() );
    myApprover->Init ( po, myApproval->AssignedApproval(), RoleApprover() );
  }
  
  if ( myApprovalDateTime.IsNull() || 
       myApprovalDateTime->DatedApproval() != myApproval->AssignedApproval() ) {
    myApprovalDateTime = new StepBasic_ApprovalDateTime;
    StepBasic_DateTimeSelect dt;
    dt.SetValue ( DefaultDateAndTime() );
    myApprovalDateTime->Init ( dt, myApproval->AssignedApproval() );
  }
}
