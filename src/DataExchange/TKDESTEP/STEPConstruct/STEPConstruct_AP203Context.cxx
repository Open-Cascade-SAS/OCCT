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

#include <TCollection_AsciiString.hxx>
#include <NCollection_Sequence.hxx>

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
#include <StepAP203_PersonOrganizationItem.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepAP203_ClassifiedItem.hxx>
#include <StepAP203_DateTimeItem.hxx>
#include <StepAP203_ApprovedItem.hxx>
#include <StepBasic_ProductCategory.hxx>

#if !defined(_WIN32) && !defined(__ANDROID__) && !defined(__EMSCRIPTEN__)
  #include <pwd.h>
#endif

//=================================================================================================

STEPConstruct_AP203Context::STEPConstruct_AP203Context()
{
  InitRoles();
}

//=================================================================================================

occ::handle<StepBasic_Approval> STEPConstruct_AP203Context::DefaultApproval()
{
  if (defApproval.IsNull())
  {
    occ::handle<StepBasic_ApprovalStatus> aStatus = new StepBasic_ApprovalStatus;
    occ::handle<TCollection_HAsciiString> aName = new TCollection_HAsciiString("not_yet_approved");
    aStatus->Init(aName);

    occ::handle<TCollection_HAsciiString> aLevel = new TCollection_HAsciiString("");
    defApproval                                  = new StepBasic_Approval;
    defApproval->Init(aStatus, aLevel);
  }
  return defApproval;
}

//=================================================================================================

void STEPConstruct_AP203Context::SetDefaultApproval(const occ::handle<StepBasic_Approval>& app)
{
  defApproval = app;
}

//=================================================================================================

occ::handle<StepBasic_DateAndTime> STEPConstruct_AP203Context::DefaultDateAndTime()
{
  if (defDateAndTime.IsNull())
  {
    OSD_Process   sys;
    Quantity_Date date = sys.SystemDate();

    occ::handle<StepBasic_CalendarDate> aDate = new StepBasic_CalendarDate;
    aDate->Init(date.Year(), date.Day(), date.Month());

    occ::handle<StepBasic_CoordinatedUniversalTimeOffset> zone =
      new StepBasic_CoordinatedUniversalTimeOffset;
#if defined(_MSC_VER) && _MSC_VER >= 1600
    long shift = 0;
    _get_timezone(&shift);
#else
    int shift = int(timezone);
#endif
    int                     shifth = abs(shift) / 3600;
    int                     shiftm = (abs(shift) - shifth * 3600) / 60;
    StepBasic_AheadOrBehind sense  = (shift > 0   ? StepBasic_aobBehind
                                      : shift < 0 ? StepBasic_aobAhead
                                                  : StepBasic_aobExact);
    zone->Init(shifth, (shiftm != 0), shiftm, sense);

    occ::handle<StepBasic_LocalTime> aTime = new StepBasic_LocalTime;
    aTime->Init(date.Hour(), true, date.Minute(), false, 0., zone);

    defDateAndTime = new StepBasic_DateAndTime;
    defDateAndTime->Init(aDate, aTime);
  }
  return defDateAndTime;
}

//=================================================================================================

void STEPConstruct_AP203Context::SetDefaultDateAndTime(const occ::handle<StepBasic_DateAndTime>& dt)
{
  defDateAndTime = dt;
}

//=================================================================================================

occ::handle<StepBasic_PersonAndOrganization> STEPConstruct_AP203Context::
  DefaultPersonAndOrganization()
{
  if (defPersonAndOrganization.IsNull())
  {
    // get IP address as a unique id of organization
    occ::handle<TCollection_HAsciiString> orgId = new TCollection_HAsciiString("IP");
    OSD_Host                              aHost;
    TCollection_AsciiString               anIP = aHost.InternetAddress();
    // cut off last number
    int aLastDotIndex = anIP.SearchFromEnd(".");
    if (aLastDotIndex > 0)
    {
      anIP.Trunc(aLastDotIndex - 1);
      orgId->AssignCat(anIP.ToCString());
    }

    // create organization
    occ::handle<StepBasic_Organization>   aOrg   = new StepBasic_Organization;
    occ::handle<TCollection_HAsciiString> oName  = new TCollection_HAsciiString("Unspecified");
    occ::handle<TCollection_HAsciiString> oDescr = new TCollection_HAsciiString("");
    aOrg->Init(true, orgId, oName, oDescr);

    // construct person`s name
    OSD_Process             sys;
    TCollection_AsciiString user(sys.UserName());
#if !defined(_WIN32) && !defined(__ANDROID__) && !defined(__EMSCRIPTEN__)
    if (!user.IsEmpty())
    {
      struct passwd* aPwd = getpwnam(user.ToCString());
      if (aPwd != nullptr)
      {
        user = aPwd->pw_gecos;
      }
    }
    else
    {
      user = "Unknown";
    }
#endif
    occ::handle<TCollection_HAsciiString> fname = new TCollection_HAsciiString("");
    occ::handle<TCollection_HAsciiString> lname = new TCollection_HAsciiString("");
    occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> mname;
    NCollection_Sequence<TCollection_AsciiString>                           names;
    int i; // svv Jan11 2000 : porting on DEC
    for (i = 1;; i++)
    {
      TCollection_AsciiString token = user.Token(" \t", i);
      if (!token.Length())
        break;
      names.Append(token);
    }
    if (names.Length() > 0)
      fname->AssignCat(names.Value(1).ToCString());
    if (names.Length() > 1)
      lname->AssignCat(names.Value(names.Length()).ToCString());
    if (names.Length() > 2)
    {
      mname = new NCollection_HArray1<occ::handle<TCollection_HAsciiString>>(1, names.Length() - 2);
      for (i = 2; i < names.Length(); i++)
        mname->SetValue(i - 1, new TCollection_HAsciiString(names.Value(i)));
    }

    // create a person
    occ::handle<StepBasic_Person>         aPerson = new StepBasic_Person;
    occ::handle<TCollection_HAsciiString> uid     = new TCollection_HAsciiString(orgId);
    uid->AssignCat(",");
    uid->AssignCat(sys.UserName().ToCString());
    occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> suffix, prefix;
    aPerson
      ->Init(uid, true, lname, true, fname, (!mname.IsNull()), mname, false, suffix, false, prefix);

    defPersonAndOrganization = new StepBasic_PersonAndOrganization;
    defPersonAndOrganization->Init(aPerson, aOrg);
  }
  return defPersonAndOrganization;
}

//=================================================================================================

void STEPConstruct_AP203Context::SetDefaultPersonAndOrganization(
  const occ::handle<StepBasic_PersonAndOrganization>& po)
{
  defPersonAndOrganization = po;
}

//=================================================================================================

occ::handle<StepBasic_SecurityClassificationLevel> STEPConstruct_AP203Context::
  DefaultSecurityClassificationLevel()
{
  if (defSecurityClassificationLevel.IsNull())
  {
    defSecurityClassificationLevel                = new StepBasic_SecurityClassificationLevel;
    occ::handle<TCollection_HAsciiString> levName = new TCollection_HAsciiString("unclassified");
    defSecurityClassificationLevel->Init(levName);
  }
  return defSecurityClassificationLevel;
}

//=================================================================================================

void STEPConstruct_AP203Context::SetDefaultSecurityClassificationLevel(
  const occ::handle<StepBasic_SecurityClassificationLevel>& scl)
{
  defSecurityClassificationLevel = scl;
}

//=================================================================================================

occ::handle<StepBasic_PersonAndOrganizationRole> STEPConstruct_AP203Context::RoleCreator() const
{
  return roleCreator;
}

//=================================================================================================

occ::handle<StepBasic_PersonAndOrganizationRole> STEPConstruct_AP203Context::RoleDesignOwner() const
{
  return roleDesignOwner;
}

//=================================================================================================

occ::handle<StepBasic_PersonAndOrganizationRole> STEPConstruct_AP203Context::RoleDesignSupplier()
  const
{
  return roleDesignSupplier;
}

//=================================================================================================

occ::handle<StepBasic_PersonAndOrganizationRole> STEPConstruct_AP203Context::
  RoleClassificationOfficer() const
{
  return roleClassificationOfficer;
}

//=================================================================================================

occ::handle<StepBasic_DateTimeRole> STEPConstruct_AP203Context::RoleCreationDate() const
{
  return roleCreationDate;
}

//=================================================================================================

occ::handle<StepBasic_DateTimeRole> STEPConstruct_AP203Context::RoleClassificationDate() const
{
  return roleClassificationDate;
}

//=================================================================================================

occ::handle<StepBasic_ApprovalRole> STEPConstruct_AP203Context::RoleApprover() const
{
  return roleApprover;
}

//=================================================================================================

void STEPConstruct_AP203Context::Init(
  const occ::handle<StepShape_ShapeDefinitionRepresentation>& sdr)
{
  Clear();
  STEPConstruct_Part SDRTool;
  SDRTool.ReadSDR(sdr);
  InitPart(SDRTool);
}

//=================================================================================================

void STEPConstruct_AP203Context::Init(const STEPConstruct_Part& SDRTool)
{
  Clear();
  InitPart(SDRTool);
}

//=================================================================================================

// void STEPConstruct_AP203Context::Init (const STEPConstruct_Part &SDRTool, const
// occ::handle<Interface_Model> &Model) {}

//=================================================================================================

void STEPConstruct_AP203Context::Init(const occ::handle<StepRepr_NextAssemblyUsageOccurrence>& NAUO)
{
  Clear();
  InitAssembly(NAUO);
}

//=================================================================================================

occ::handle<StepAP203_CcDesignPersonAndOrganizationAssignment> STEPConstruct_AP203Context::
  GetCreator() const
{
  return myCreator;
}

//=================================================================================================

occ::handle<StepAP203_CcDesignPersonAndOrganizationAssignment> STEPConstruct_AP203Context::
  GetDesignOwner() const
{
  return myDesignOwner;
}

//=================================================================================================

occ::handle<StepAP203_CcDesignPersonAndOrganizationAssignment> STEPConstruct_AP203Context::
  GetDesignSupplier() const
{
  return myDesignSupplier;
}

//=================================================================================================

occ::handle<StepAP203_CcDesignPersonAndOrganizationAssignment> STEPConstruct_AP203Context::
  GetClassificationOfficer() const
{
  return myClassificationOfficer;
}

//=================================================================================================

occ::handle<StepAP203_CcDesignSecurityClassification> STEPConstruct_AP203Context::GetSecurity()
  const
{
  return mySecurity;
}

//=================================================================================================

occ::handle<StepAP203_CcDesignDateAndTimeAssignment> STEPConstruct_AP203Context::GetCreationDate()
  const
{
  return myCreationDate;
}

//=================================================================================================

occ::handle<StepAP203_CcDesignDateAndTimeAssignment> STEPConstruct_AP203Context::
  GetClassificationDate() const
{
  return myClassificationDate;
}

//=================================================================================================

occ::handle<StepAP203_CcDesignApproval> STEPConstruct_AP203Context::GetApproval() const
{
  return myApproval;
}

//=================================================================================================

occ::handle<StepBasic_ApprovalPersonOrganization> STEPConstruct_AP203Context::GetApprover() const
{
  return myApprover;
}

//=================================================================================================

occ::handle<StepBasic_ApprovalDateTime> STEPConstruct_AP203Context::GetApprovalDateTime() const
{
  return myApprovalDateTime;
}

//=================================================================================================

occ::handle<StepBasic_ProductCategoryRelationship> STEPConstruct_AP203Context::
  GetProductCategoryRelationship() const
{
  return myProductCategoryRelationship;
}

//=================================================================================================

void STEPConstruct_AP203Context::Clear()
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

//=================================================================================================

void STEPConstruct_AP203Context::InitRoles()
{
  roleCreator               = new StepBasic_PersonAndOrganizationRole;
  roleDesignOwner           = new StepBasic_PersonAndOrganizationRole;
  roleDesignSupplier        = new StepBasic_PersonAndOrganizationRole;
  roleClassificationOfficer = new StepBasic_PersonAndOrganizationRole;
  roleCreationDate          = new StepBasic_DateTimeRole;
  roleClassificationDate    = new StepBasic_DateTimeRole;
  roleApprover              = new StepBasic_ApprovalRole;

  roleCreator->Init(new TCollection_HAsciiString("creator"));
  roleDesignOwner->Init(new TCollection_HAsciiString("design_owner"));
  roleDesignSupplier->Init(new TCollection_HAsciiString("design_supplier"));
  roleClassificationOfficer->Init(new TCollection_HAsciiString("classification_officer"));
  roleCreationDate->Init(new TCollection_HAsciiString("creation_date"));
  roleClassificationDate->Init(new TCollection_HAsciiString("classification_date"));
  roleApprover->Init(new TCollection_HAsciiString("approver"));
}

//=================================================================================================

void STEPConstruct_AP203Context::InitPart(const STEPConstruct_Part& SDRTool)
{
  if (myCreator.IsNull())
  {
    myCreator = new StepAP203_CcDesignPersonAndOrganizationAssignment;
    occ::handle<NCollection_HArray1<StepAP203_PersonOrganizationItem>> items =
      new NCollection_HArray1<StepAP203_PersonOrganizationItem>(1, 2);
    items->ChangeValue(1).SetValue(SDRTool.PDF());
    items->ChangeValue(2).SetValue(SDRTool.PD());
    myCreator->Init(DefaultPersonAndOrganization(), RoleCreator(), items);
  }

  if (myDesignOwner.IsNull())
  {
    myDesignOwner = new StepAP203_CcDesignPersonAndOrganizationAssignment;
    occ::handle<NCollection_HArray1<StepAP203_PersonOrganizationItem>> items =
      new NCollection_HArray1<StepAP203_PersonOrganizationItem>(1, 1);
    items->ChangeValue(1).SetValue(SDRTool.Product());
    myDesignOwner->Init(DefaultPersonAndOrganization(), RoleDesignOwner(), items);
  }

  if (myDesignSupplier.IsNull())
  {
    myDesignSupplier = new StepAP203_CcDesignPersonAndOrganizationAssignment;
    occ::handle<NCollection_HArray1<StepAP203_PersonOrganizationItem>> items =
      new NCollection_HArray1<StepAP203_PersonOrganizationItem>(1, 1);
    items->ChangeValue(1).SetValue(SDRTool.PDF());
    myDesignSupplier->Init(DefaultPersonAndOrganization(), RoleDesignSupplier(), items);
  }

  if (myCreationDate.IsNull())
  {
    myCreationDate = new StepAP203_CcDesignDateAndTimeAssignment;
    occ::handle<NCollection_HArray1<StepAP203_DateTimeItem>> items =
      new NCollection_HArray1<StepAP203_DateTimeItem>(1, 1);
    items->ChangeValue(1).SetValue(SDRTool.PD());
    myCreationDate->Init(DefaultDateAndTime(), RoleCreationDate(), items);
  }

  if (mySecurity.IsNull())
  {

    occ::handle<TCollection_HAsciiString>         aName    = new TCollection_HAsciiString("");
    occ::handle<TCollection_HAsciiString>         aPurpose = new TCollection_HAsciiString("");
    occ::handle<StepBasic_SecurityClassification> sc       = new StepBasic_SecurityClassification;
    sc->Init(aName, aPurpose, DefaultSecurityClassificationLevel());

    mySecurity = new StepAP203_CcDesignSecurityClassification;
    occ::handle<NCollection_HArray1<StepAP203_ClassifiedItem>> items =
      new NCollection_HArray1<StepAP203_ClassifiedItem>(1, 1);
    items->ChangeValue(1).SetValue(SDRTool.PDF());
    mySecurity->Init(sc, items);
  }
  InitSecurityRequisites();

  if (myApproval.IsNull())
  {
    myApproval = new StepAP203_CcDesignApproval;
    occ::handle<NCollection_HArray1<StepAP203_ApprovedItem>> items =
      new NCollection_HArray1<StepAP203_ApprovedItem>(1, 3);
    items->ChangeValue(1).SetValue(SDRTool.PDF());
    items->ChangeValue(2).SetValue(SDRTool.PD());
    items->ChangeValue(3).SetValue(mySecurity->AssignedSecurityClassification());
    myApproval->Init(DefaultApproval(), items);
  }
  InitApprovalRequisites();

  if (myProductCategoryRelationship.IsNull())
  {
    occ::handle<StepBasic_ProductCategory> PC     = new StepBasic_ProductCategory;
    occ::handle<TCollection_HAsciiString>  PCName = new TCollection_HAsciiString("part");
    PC->Init(PCName, false, nullptr);

    myProductCategoryRelationship                  = new StepBasic_ProductCategoryRelationship;
    occ::handle<TCollection_HAsciiString> PCRName  = new TCollection_HAsciiString("");
    occ::handle<TCollection_HAsciiString> PCRDescr = new TCollection_HAsciiString("");
    myProductCategoryRelationship->Init(PCRName, true, PCRDescr, PC, SDRTool.PRPC());
  }
}

//=================================================================================================

void STEPConstruct_AP203Context::InitAssembly(
  const occ::handle<StepRepr_NextAssemblyUsageOccurrence>& NAUO)
{
  if (mySecurity.IsNull())
  {

    occ::handle<TCollection_HAsciiString>         aName    = new TCollection_HAsciiString("");
    occ::handle<TCollection_HAsciiString>         aPurpose = new TCollection_HAsciiString("");
    occ::handle<StepBasic_SecurityClassification> sc       = new StepBasic_SecurityClassification;
    sc->Init(aName, aPurpose, DefaultSecurityClassificationLevel());

    mySecurity = new StepAP203_CcDesignSecurityClassification;
    occ::handle<NCollection_HArray1<StepAP203_ClassifiedItem>> items =
      new NCollection_HArray1<StepAP203_ClassifiedItem>(1, 1);
    items->ChangeValue(1).SetValue(NAUO);
    mySecurity->Init(sc, items);
  }
  InitSecurityRequisites();

  if (myApproval.IsNull())
  {
    myApproval = new StepAP203_CcDesignApproval;
    occ::handle<NCollection_HArray1<StepAP203_ApprovedItem>> items =
      new NCollection_HArray1<StepAP203_ApprovedItem>(1, 1);
    items->ChangeValue(1).SetValue(mySecurity->AssignedSecurityClassification());
    myApproval->Init(DefaultApproval(), items);
  }
  InitApprovalRequisites();
}

//=================================================================================================

void STEPConstruct_AP203Context::InitSecurityRequisites()
{
  if (myClassificationOfficer.IsNull()
      || myClassificationOfficer->Items()->Value(1).Value()
           != mySecurity->AssignedSecurityClassification())
  {
    myClassificationOfficer = new StepAP203_CcDesignPersonAndOrganizationAssignment;
    occ::handle<NCollection_HArray1<StepAP203_PersonOrganizationItem>> items =
      new NCollection_HArray1<StepAP203_PersonOrganizationItem>(1, 1);
    items->ChangeValue(1).SetValue(mySecurity->AssignedSecurityClassification());
    myClassificationOfficer->Init(DefaultPersonAndOrganization(),
                                  RoleClassificationOfficer(),
                                  items);
  }

  if (myClassificationDate.IsNull()
      || myClassificationDate->Items()->Value(1).Value()
           != mySecurity->AssignedSecurityClassification())
  {
    myClassificationDate = new StepAP203_CcDesignDateAndTimeAssignment;
    occ::handle<NCollection_HArray1<StepAP203_DateTimeItem>> items =
      new NCollection_HArray1<StepAP203_DateTimeItem>(1, 1);
    items->ChangeValue(1).SetValue(mySecurity->AssignedSecurityClassification());
    myClassificationDate->Init(DefaultDateAndTime(), RoleClassificationDate(), items);
  }
}

//=================================================================================================

void STEPConstruct_AP203Context::InitApprovalRequisites()
{
  if (myApprover.IsNull() || myApprover->AuthorizedApproval() != myApproval->AssignedApproval())
  {
    myApprover = new StepBasic_ApprovalPersonOrganization;
    StepBasic_PersonOrganizationSelect po;
    po.SetValue(DefaultPersonAndOrganization());
    myApprover->Init(po, myApproval->AssignedApproval(), RoleApprover());
  }

  if (myApprovalDateTime.IsNull()
      || myApprovalDateTime->DatedApproval() != myApproval->AssignedApproval())
  {
    myApprovalDateTime = new StepBasic_ApprovalDateTime;
    StepBasic_DateTimeSelect dt;
    dt.SetValue(DefaultDateAndTime());
    myApprovalDateTime->Init(dt, myApproval->AssignedApproval());
  }
}
