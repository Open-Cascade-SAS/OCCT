// Created on: 1993-08-12
// Created by: Frederic MAUPAS
// Copyright (c) 1993-1999 Matra Datavision
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

// #58 rln 28.12.98 STEP header fields (NOTE: more parameterization is necessary)
// pdn 11.01.99 including <stdio.h> for compilation on NT

#include <APIHeaderSection_MakeHeader.hxx>
#include <HeaderSection_FileDescription.hxx>
#include <HeaderSection_FileName.hxx>
#include <HeaderSection_FileSchema.hxx>
#include <Interface_EntityIterator.hxx>
#include <TCollection_HAsciiString.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <MoniTool_Macros.hxx>
#include <Interface_MSG.hxx>
#include <Interface_Version.hxx>
#include <StepData_Protocol.hxx>
#include <StepData_StepModel.hxx>

#include <cstdio>
// This is a generic header for any STEP scheme
static occ::handle<TCollection_HAsciiString>                                   nulstr;
static occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> nularr;

APIHeaderSection_MakeHeader::APIHeaderSection_MakeHeader(
  const occ::handle<StepData_StepModel>& model)
{
  done = true;
  if (model->HasHeaderEntity(STANDARD_TYPE(HeaderSection_FileName)))
  {
    fn =
      GetCasted(HeaderSection_FileName, model->HeaderEntity(STANDARD_TYPE(HeaderSection_FileName)));
  }
  else
    done = false;
  if (model->HasHeaderEntity(STANDARD_TYPE(HeaderSection_FileSchema)))
  {
    fs = GetCasted(HeaderSection_FileSchema,
                   model->HeaderEntity(STANDARD_TYPE(HeaderSection_FileSchema)));
  }
  else
    done = false;
  if (model->HasHeaderEntity(STANDARD_TYPE(HeaderSection_FileDescription)))
  {
    fd = GetCasted(HeaderSection_FileDescription,
                   model->HeaderEntity(STANDARD_TYPE(HeaderSection_FileDescription)));
  }
  else
    done = false;
}

APIHeaderSection_MakeHeader::APIHeaderSection_MakeHeader(const int shapetype)
{
  switch (shapetype)
  {
    case 1:
      Init("Open CASCADE Facetted BRep Model");
      break;
    case 2:
      Init("Open CASCADE Face Based Surface Model");
      break;
    case 3:
      Init("Open CASCADE Shell Based Surface Model");
      break;
    case 4:
      Init("Open CASCADE Manifold Solid Brep Model");
      break;
    default:
      Init("Open CASCADE Shape Model");
      break;
  }
}

void APIHeaderSection_MakeHeader::Init(const char* const nameval)
{
  done = true;

  // - File Name
  char timestamp[50];

  if (fn.IsNull())
    fn = new HeaderSection_FileName;
  occ::handle<TCollection_HAsciiString> name = new TCollection_HAsciiString(nameval);
  fn->SetName(name);
  // clang-format off
  Interface_MSG::TDate (timestamp,0,0,0,0,0,1,"C:%4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d");  // actually
  // clang-format on
  occ::handle<TCollection_HAsciiString> tst = new TCollection_HAsciiString(timestamp);
  fn->SetTimeStamp(tst);
  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> authors =
    new NCollection_HArray1<occ::handle<TCollection_HAsciiString>>(1, 1);
  occ::handle<TCollection_HAsciiString> a1 = new TCollection_HAsciiString("Author");
  authors->SetValue(1, a1);
  fn->SetAuthor(authors);
  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> org =
    new NCollection_HArray1<occ::handle<TCollection_HAsciiString>>(1, 1);
  occ::handle<TCollection_HAsciiString> org1 = new TCollection_HAsciiString("Open CASCADE");
  org->SetValue(1, org1);
  fn->SetOrganization(org);

  char procver[80];
  Sprintf(procver, XSTEP_PROCESSOR_VERSION, "STEP");
  occ::handle<TCollection_HAsciiString> pv = new TCollection_HAsciiString(procver);
  // occ::handle<TCollection_HAsciiString> pv =
  // new TCollection_HAsciiString(XSTEP_VERSION);
  fn->SetPreprocessorVersion(pv);

  occ::handle<TCollection_HAsciiString> sys =
    new TCollection_HAsciiString(XSTEP_SYSTEM_VERSION); // #58 rln
  fn->SetOriginatingSystem(sys);
  occ::handle<TCollection_HAsciiString> auth = new TCollection_HAsciiString("Unknown");
  fn->SetAuthorisation(auth);

  // - File Description

  if (fd.IsNull())
    fd = new HeaderSection_FileDescription;
  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> descr =
    new NCollection_HArray1<occ::handle<TCollection_HAsciiString>>(1, 1);
  occ::handle<TCollection_HAsciiString> descr1 = new TCollection_HAsciiString("Open CASCADE Model");
  descr->SetValue(1, descr1);
  fd->SetDescription(descr);
  occ::handle<TCollection_HAsciiString> il = new TCollection_HAsciiString("2;1");
  fd->SetImplementationLevel(il);

  // - File Schema

  if (fs.IsNull())
    fs = new HeaderSection_FileSchema;
  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> schid =
    new NCollection_HArray1<occ::handle<TCollection_HAsciiString>>(1, 1);
  occ::handle<TCollection_HAsciiString> schid1 = new TCollection_HAsciiString("");
  schid->SetValue(1, schid1);
  fs->SetSchemaIdentifiers(schid);
}

bool APIHeaderSection_MakeHeader::IsDone() const
{
  return done;
}

void APIHeaderSection_MakeHeader::Apply(const occ::handle<StepData_StepModel>& model) const
{
  Interface_EntityIterator header = model->Header();
  if (HasFd() && !model->HasHeaderEntity(STANDARD_TYPE(HeaderSection_FileDescription)))
    header.AddItem(fd);
  if (HasFn() && !model->HasHeaderEntity(STANDARD_TYPE(HeaderSection_FileName)))
    header.AddItem(fn);
  if (HasFs() && !model->HasHeaderEntity(STANDARD_TYPE(HeaderSection_FileSchema)))
  {

    // Schema defined? If not take it from the protocole
    occ::handle<TCollection_HAsciiString>                                   sch;
    occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> schid =
      fs->SchemaIdentifiers();
    if (!schid.IsNull())
      sch = schid->Value(1);
    else
    {
      schid = new NCollection_HArray1<occ::handle<TCollection_HAsciiString>>(1, 1);
      fs->SetSchemaIdentifiers(schid);
    }
    if (!sch.IsNull())
    {
      if (sch->Length() < 2)
        sch.Nullify();
    } // not defined
    if (sch.IsNull())
    {
      occ::handle<StepData_Protocol> stepro = occ::down_cast<StepData_Protocol>(model->Protocol());
      if (!stepro.IsNull())
        sch = new TCollection_HAsciiString(stepro->SchemaName(model));
      if (!sch.IsNull())
        schid->SetValue(1, sch);
    }
    header.AddItem(fs);
  }
  model->ClearHeader();
  for (header.Start(); header.More(); header.Next())
    model->AddHeaderEntity(header.Value());
}

// ========
// FileName
// ========

occ::handle<StepData_StepModel> APIHeaderSection_MakeHeader::NewModel(
  const occ::handle<Interface_Protocol>& protocol) const
{
  occ::handle<StepData_StepModel> stepmodel = new StepData_StepModel;
  stepmodel->SetProtocol(protocol);

  // - Make Header information

  Apply(stepmodel);
  return stepmodel;
}

//   ########        Individual Queries / Actions        ########

// ========
// FileName
// ========

bool APIHeaderSection_MakeHeader::HasFn() const
{
  return (!fn.IsNull());
}

occ::handle<HeaderSection_FileName> APIHeaderSection_MakeHeader::FnValue() const
{
  return fn;
}

/*
void APIHeaderSection_MakeHeader::SetNameFromShapeType(const int shapetype)
{
  occ::handle<TCollection_HAsciiString> name;
  switch(shapetype)
    {
    case 1: // face_based_surface_model
      name = new TCollection_HAsciiString
    ("Euclid Face Based Surface Model");
      break;
    case 2: // manifold_solid_brep
      name = new TCollection_HAsciiString
    ("Euclid Manifold Solid Brep Model");
      break;
    case 3: // facetted_brep
      name = new TCollection_HAsciiString
    ("Euclid Facetted Brep Model");
      break;
    default : // others ?
      name = new TCollection_HAsciiString
    ("Euclid Shape Model");
      break;
    }
  SetName(aName);
}
*/

void APIHeaderSection_MakeHeader::SetName(const occ::handle<TCollection_HAsciiString>& aName)
{
  if (!fn.IsNull())
    fn->SetName(aName);
}

occ::handle<TCollection_HAsciiString> APIHeaderSection_MakeHeader::Name() const
{
  return (fn.IsNull() ? nulstr : fn->Name());
}

void APIHeaderSection_MakeHeader::SetTimeStamp(
  const occ::handle<TCollection_HAsciiString>& aTimeStamp)
{
  if (!fn.IsNull())
    fn->SetTimeStamp(aTimeStamp);
}

occ::handle<TCollection_HAsciiString> APIHeaderSection_MakeHeader::TimeStamp() const
{
  return (fn.IsNull() ? nulstr : fn->TimeStamp());
}

void APIHeaderSection_MakeHeader::SetAuthor(
  const occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& aAuthor)
{
  if (!fn.IsNull())
    fn->SetAuthor(aAuthor);
}

void APIHeaderSection_MakeHeader::SetAuthorValue(
  const int                                    num,
  const occ::handle<TCollection_HAsciiString>& aAuthor)
{
  if (fn.IsNull())
    return;
  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> li = fn->Author();
  if (num >= li->Lower() && num <= li->Upper())
    li->SetValue(num, aAuthor);
}

occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>
  APIHeaderSection_MakeHeader::Author() const
{
  return (fn.IsNull() ? nularr : fn->Author());
}

occ::handle<TCollection_HAsciiString> APIHeaderSection_MakeHeader::AuthorValue(const int num) const
{
  return (fn.IsNull() ? nulstr : fn->AuthorValue(num));
}

int APIHeaderSection_MakeHeader::NbAuthor() const
{
  return (fn.IsNull() ? 0 : fn->NbAuthor());
}

void APIHeaderSection_MakeHeader::SetOrganization(
  const occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& aOrganization)
{
  if (!fn.IsNull())
    fn->SetOrganization(aOrganization);
}

void APIHeaderSection_MakeHeader::SetOrganizationValue(
  const int                                    num,
  const occ::handle<TCollection_HAsciiString>& aOrgan)
{
  if (fn.IsNull())
    return;
  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> li = fn->Organization();
  if (num >= li->Lower() && num <= li->Upper())
    li->SetValue(num, aOrgan);
}

occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>
  APIHeaderSection_MakeHeader::Organization() const
{
  return (fn.IsNull() ? nularr : fn->Organization());
}

occ::handle<TCollection_HAsciiString> APIHeaderSection_MakeHeader::OrganizationValue(
  const int num) const
{
  return (fn.IsNull() ? nulstr : fn->OrganizationValue(num));
}

int APIHeaderSection_MakeHeader::NbOrganization() const
{
  return (fn.IsNull() ? 0 : fn->NbOrganization());
}

void APIHeaderSection_MakeHeader::SetPreprocessorVersion(
  const occ::handle<TCollection_HAsciiString>& aPreprocessorVersion)
{
  if (!fn.IsNull())
    fn->SetPreprocessorVersion(aPreprocessorVersion);
}

occ::handle<TCollection_HAsciiString> APIHeaderSection_MakeHeader::PreprocessorVersion() const
{
  return (fn.IsNull() ? nulstr : fn->PreprocessorVersion());
}

void APIHeaderSection_MakeHeader::SetOriginatingSystem(
  const occ::handle<TCollection_HAsciiString>& aOriginatingSystem)
{
  if (!fn.IsNull())
    fn->SetOriginatingSystem(aOriginatingSystem);
}

occ::handle<TCollection_HAsciiString> APIHeaderSection_MakeHeader::OriginatingSystem() const
{
  return (fn.IsNull() ? nulstr : fn->OriginatingSystem());
}

void APIHeaderSection_MakeHeader::SetAuthorisation(
  const occ::handle<TCollection_HAsciiString>& aAuthorisation)
{
  if (!fn.IsNull())
    fn->SetAuthorisation(aAuthorisation);
}

occ::handle<TCollection_HAsciiString> APIHeaderSection_MakeHeader::Authorisation() const
{
  return (fn.IsNull() ? nulstr : fn->Authorisation());
}

// ===========
// File Schema
// ===========

bool APIHeaderSection_MakeHeader::HasFs() const
{
  return (!fs.IsNull());
}

occ::handle<HeaderSection_FileSchema> APIHeaderSection_MakeHeader::FsValue() const
{
  return fs;
}

void APIHeaderSection_MakeHeader::SetSchemaIdentifiers(
  const occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& aSchemaIdentifiers)
{
  if (!fs.IsNull())
    fs->SetSchemaIdentifiers(aSchemaIdentifiers);
}

void APIHeaderSection_MakeHeader::SetSchemaIdentifiersValue(
  const int                                    num,
  const occ::handle<TCollection_HAsciiString>& aSchem)
{
  if (fs.IsNull())
    return;
  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> li =
    fs->SchemaIdentifiers();
  if (num >= li->Lower() && num <= li->Upper())
    li->SetValue(num, aSchem);
}

occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>
  APIHeaderSection_MakeHeader::SchemaIdentifiers() const
{
  return (fs.IsNull() ? nularr : fs->SchemaIdentifiers());
}

occ::handle<TCollection_HAsciiString> APIHeaderSection_MakeHeader::SchemaIdentifiersValue(
  const int num) const
{
  return (fs.IsNull() ? nulstr : fs->SchemaIdentifiersValue(num));
}

int APIHeaderSection_MakeHeader::NbSchemaIdentifiers() const
{
  return (fs.IsNull() ? 0 : fs->NbSchemaIdentifiers());
}

//=================================================================================================

void APIHeaderSection_MakeHeader::AddSchemaIdentifier(
  const occ::handle<TCollection_HAsciiString>& aSchem)
{
  if (fs.IsNull())
    fs = new HeaderSection_FileSchema;
  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> idents =
    fs->SchemaIdentifiers();

  // check that requested subschema is already in the list
  int i;
  for (i = 1; !idents.IsNull() && i <= idents->Length(); i++)
  {
    if (aSchem->IsSameString(idents->Value(i)))
      return;
  }

  // add a subshema
  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> ids =
    new NCollection_HArray1<occ::handle<TCollection_HAsciiString>>(
      1,
      (idents.IsNull() ? 1 : idents->Length() + 1));
  for (i = 1; !idents.IsNull() && i <= idents->Length(); i++)
  {
    ids->SetValue(i, idents->Value(i));
  }
  ids->SetValue(i, aSchem);

  fs->SetSchemaIdentifiers(ids);
}

// ================
// File Description
// ================

bool APIHeaderSection_MakeHeader::HasFd() const
{
  return (!fd.IsNull());
}

occ::handle<HeaderSection_FileDescription> APIHeaderSection_MakeHeader::FdValue() const
{
  return fd;
}

void APIHeaderSection_MakeHeader::SetDescription(
  const occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& aDescription)
{
  if (!fs.IsNull())
    fd->SetDescription(aDescription);
}

void APIHeaderSection_MakeHeader::SetDescriptionValue(
  const int                                    num,
  const occ::handle<TCollection_HAsciiString>& aDescr)
{
  if (fd.IsNull())
    return;
  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> li = fd->Description();
  if (num >= li->Lower() && num <= li->Upper())
    li->SetValue(num, aDescr);
}

occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>
  APIHeaderSection_MakeHeader::Description() const
{
  return (fd.IsNull() ? nularr : fd->Description());
}

occ::handle<TCollection_HAsciiString> APIHeaderSection_MakeHeader::DescriptionValue(
  const int num) const
{
  return (fd.IsNull() ? nulstr : fd->DescriptionValue(num));
}

int APIHeaderSection_MakeHeader::NbDescription() const
{
  return (fd.IsNull() ? 0 : fd->NbDescription());
}

void APIHeaderSection_MakeHeader::SetImplementationLevel(
  const occ::handle<TCollection_HAsciiString>& aImplementationLevel)
{
  if (!fd.IsNull())
    fd->SetImplementationLevel(aImplementationLevel);
}

occ::handle<TCollection_HAsciiString> APIHeaderSection_MakeHeader::ImplementationLevel() const
{
  return (fd.IsNull() ? nulstr : fd->ImplementationLevel());
}
