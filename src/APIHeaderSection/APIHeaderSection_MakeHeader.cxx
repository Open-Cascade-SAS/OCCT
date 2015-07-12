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

//#58 rln 28.12.98 STEP header fields (NOTE: more parameterization is necessary)
//pdn 11.01.99 including <stdio.h> for compilation on NT

#include <APIHeaderSection_MakeHeader.hxx>
#include <HeaderSection_FileDescription.hxx>
#include <HeaderSection_FileName.hxx>
#include <HeaderSection_FileSchema.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_HArray1OfHAsciiString.hxx>
#include <Interface_Macros.hxx>
#include <Interface_MSG.hxx>
#include <Interface_Protocol.hxx>
#include <Interface_Version.hxx>
#include <StepData_Protocol.hxx>
#include <StepData_StepModel.hxx>
#include <TCollection_HAsciiString.hxx>

#include <stdio.h>
// This is a generic header for any STEP sheme
static Handle(TCollection_HAsciiString) nulstr;
static Handle(Interface_HArray1OfHAsciiString) nularr;

APIHeaderSection_MakeHeader::APIHeaderSection_MakeHeader
  (const Handle(StepData_StepModel)& model)
{
  done = Standard_True;
  if (model->HasHeaderEntity (STANDARD_TYPE(HeaderSection_FileName))) {
    fn = GetCasted(HeaderSection_FileName,
		   model->HeaderEntity(STANDARD_TYPE(HeaderSection_FileName)));
  }
  else done = Standard_False;
  if (model->HasHeaderEntity (STANDARD_TYPE(HeaderSection_FileSchema))) {
    fs = GetCasted(HeaderSection_FileSchema,
		   model->HeaderEntity(STANDARD_TYPE(HeaderSection_FileSchema)));
  }
  else done = Standard_False;
  if (model->HasHeaderEntity (STANDARD_TYPE(HeaderSection_FileDescription))) {
    fd = GetCasted(HeaderSection_FileDescription,
		   model->HeaderEntity(STANDARD_TYPE(HeaderSection_FileDescription)));
  }
  else done = Standard_False;
}

APIHeaderSection_MakeHeader::APIHeaderSection_MakeHeader
  (const Standard_Integer shapetype)
{
  switch(shapetype) {
    case 1 : Init ("Open CASCADE Facetted BRep Model");       break;
    case 2 : Init ("Open CASCADE Face Based Surface Model");  break;
    case 3 : Init ("Open CASCADE Shell Based Surface Model"); break;
    case 4 : Init ("Open CASCADE Manifold Solid Brep Model"); break;
    default: Init ("Open CASCADE Shape Model");               break;
  }
}

void  APIHeaderSection_MakeHeader::Init (const Standard_CString nameval)
{
  done = Standard_True;
  
  // - File Name
  char timestamp[50];
  
  if (fn.IsNull()) fn = new HeaderSection_FileName;
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString(nameval);
  fn->SetName(name);
  Interface_MSG::TDate (timestamp,0,0,0,0,0,1,"C:%4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d");  // actually
  Handle(TCollection_HAsciiString) tst = 
    new TCollection_HAsciiString(timestamp);
  fn->SetTimeStamp(tst);
  Handle(Interface_HArray1OfHAsciiString) authors = 
    new Interface_HArray1OfHAsciiString(1,1);
  Handle(TCollection_HAsciiString) a1 = 
    new TCollection_HAsciiString("Author");
  authors->SetValue(1,a1);
  fn->SetAuthor(authors);
  Handle(Interface_HArray1OfHAsciiString) org = 
    new Interface_HArray1OfHAsciiString(1,1);
  Handle(TCollection_HAsciiString) org1 = 
    new TCollection_HAsciiString("Open CASCADE");
  org->SetValue(1,org1);
  fn->SetOrganization(org);
  
  char procver[80];
  sprintf (procver, XSTEP_PROCESSOR_VERSION, "STEP");
  Handle(TCollection_HAsciiString) pv = new TCollection_HAsciiString (procver);
  //Handle(TCollection_HAsciiString) pv = 
  //new TCollection_HAsciiString(XSTEP_VERSION);
  fn->SetPreprocessorVersion(pv);
  
  Handle(TCollection_HAsciiString) sys = 
    new TCollection_HAsciiString(XSTEP_SYSTEM_VERSION);//#58 rln
  fn->SetOriginatingSystem(sys);
  Handle(TCollection_HAsciiString) auth = 
    new TCollection_HAsciiString("Unknown");
  fn->SetAuthorisation(auth);
  
  // - File Description 
  
  if (fd.IsNull()) fd = new HeaderSection_FileDescription;
  Handle(Interface_HArray1OfHAsciiString) descr =
    new Interface_HArray1OfHAsciiString(1,1);
  Handle(TCollection_HAsciiString) descr1 = 
    new TCollection_HAsciiString("Open CASCADE Model");
  descr->SetValue(1,descr1);
  fd->SetDescription(descr);
  Handle(TCollection_HAsciiString) il = 
    new TCollection_HAsciiString("2;1");
  fd->SetImplementationLevel(il);

  // - File Schema

  if (fs.IsNull()) fs  = new HeaderSection_FileSchema;
  Handle(Interface_HArray1OfHAsciiString) schid =
    new Interface_HArray1OfHAsciiString(1,1);
  Handle(TCollection_HAsciiString) schid1 = 
    new TCollection_HAsciiString("");
  schid->SetValue(1,schid1);
  fs->SetSchemaIdentifiers(schid);

}

Standard_Boolean APIHeaderSection_MakeHeader::IsDone() const
{
  return done;
}

void APIHeaderSection_MakeHeader::Apply
  (const Handle(StepData_StepModel)& model) const
{
  Interface_EntityIterator header = model->Header();
  if (HasFd() && !model->HasHeaderEntity (STANDARD_TYPE(HeaderSection_FileDescription)))
    header.AddItem(fd);
  if (HasFn() && !model->HasHeaderEntity (STANDARD_TYPE(HeaderSection_FileName)))
    header.AddItem(fn);
  if (HasFs() && !model->HasHeaderEntity (STANDARD_TYPE(HeaderSection_FileSchema))) {

// Schema defined? If not take it from the protocole
    Handle(TCollection_HAsciiString) sch;
    Handle(Interface_HArray1OfHAsciiString) schid = fs->SchemaIdentifiers();
    if (!schid.IsNull()) sch = schid->Value(1);
    else {
      schid = new Interface_HArray1OfHAsciiString(1,1);
      fs->SetSchemaIdentifiers(schid);
    }
    if (!sch.IsNull()) { if (sch->Length() < 2) sch.Nullify(); } // not defined
    if (sch.IsNull()) {
      Handle(StepData_Protocol) stepro = Handle(StepData_Protocol)::DownCast
	( model->Protocol());
      if (!stepro.IsNull()) sch = new TCollection_HAsciiString
	(stepro->SchemaName());
      if (!sch.IsNull()) schid->SetValue (1,sch);
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

Handle(StepData_StepModel)  APIHeaderSection_MakeHeader::NewModel
  (const Handle(Interface_Protocol)& protocol) const
{
  Handle(StepData_StepModel) stepmodel = new StepData_StepModel;
  stepmodel->SetProtocol (protocol);

      // - Make Header information

  Apply(stepmodel);
  return stepmodel;
}

//   ########        Individual Queries / Actions        ########

// ========
// FileName
// ========

Standard_Boolean APIHeaderSection_MakeHeader::HasFn() const
{  return (!fn.IsNull());  }

Handle(HeaderSection_FileName) APIHeaderSection_MakeHeader::FnValue() const
{
  return fn;
}

/*
void APIHeaderSection_MakeHeader::SetNameFromShapeType(const Standard_Integer shapetype)
{
  Handle(TCollection_HAsciiString) name;
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

void APIHeaderSection_MakeHeader::SetName(const Handle(TCollection_HAsciiString)& aName)
{
	if (!fn.IsNull()) fn->SetName(aName);
}

Handle(TCollection_HAsciiString) APIHeaderSection_MakeHeader::Name() const
{
	return (fn.IsNull() ? nulstr : fn->Name());
}

void APIHeaderSection_MakeHeader::SetTimeStamp(const Handle(TCollection_HAsciiString)& aTimeStamp)
{
	if (!fn.IsNull()) fn->SetTimeStamp(aTimeStamp);
}

Handle(TCollection_HAsciiString) APIHeaderSection_MakeHeader::TimeStamp() const
{
	return (fn.IsNull() ? nulstr : fn->TimeStamp());
}

void APIHeaderSection_MakeHeader::SetAuthor(const Handle(Interface_HArray1OfHAsciiString)& aAuthor)
{
	if (!fn.IsNull()) fn->SetAuthor(aAuthor);
}

void APIHeaderSection_MakeHeader::SetAuthorValue(const Standard_Integer num, const Handle(TCollection_HAsciiString)& aAuthor)
{
  if (fn.IsNull()) return;
  Handle(Interface_HArray1OfHAsciiString) li = fn->Author();
  if (num >= li->Lower() && num <= li->Upper()) li->SetValue(num,aAuthor);
}

Handle(Interface_HArray1OfHAsciiString) APIHeaderSection_MakeHeader::Author() const
{
	return (fn.IsNull() ? nularr : fn->Author());
}

Handle(TCollection_HAsciiString) APIHeaderSection_MakeHeader::AuthorValue(const Standard_Integer num) const
{
	return (fn.IsNull() ? nulstr : fn->AuthorValue(num));
}

Standard_Integer APIHeaderSection_MakeHeader::NbAuthor () const
{
	return (fn.IsNull() ? 0 : fn->NbAuthor());
}

void APIHeaderSection_MakeHeader::SetOrganization(const Handle(Interface_HArray1OfHAsciiString)& aOrganization)
{
	if (!fn.IsNull()) fn->SetOrganization(aOrganization);
}

void APIHeaderSection_MakeHeader::SetOrganizationValue(const Standard_Integer num, const Handle(TCollection_HAsciiString)& aOrgan)
{
  if (fn.IsNull()) return;
  Handle(Interface_HArray1OfHAsciiString) li = fn->Organization();
  if (num >= li->Lower() && num <= li->Upper()) li->SetValue(num,aOrgan);
}

Handle(Interface_HArray1OfHAsciiString) APIHeaderSection_MakeHeader::Organization() const
{
	return (fn.IsNull() ? nularr : fn->Organization());
}

Handle(TCollection_HAsciiString) APIHeaderSection_MakeHeader::OrganizationValue(const Standard_Integer num) const
{
	return (fn.IsNull() ? nulstr : fn->OrganizationValue(num));
}

Standard_Integer APIHeaderSection_MakeHeader::NbOrganization () const
{
	return (fn.IsNull() ? 0 : fn->NbOrganization());
}

void APIHeaderSection_MakeHeader::SetPreprocessorVersion(const Handle(TCollection_HAsciiString)& aPreprocessorVersion)
{
	if (!fn.IsNull()) fn->SetPreprocessorVersion(aPreprocessorVersion);
}

Handle(TCollection_HAsciiString) APIHeaderSection_MakeHeader::PreprocessorVersion() const
{
	return (fn.IsNull() ? nulstr : fn->PreprocessorVersion());
}

void APIHeaderSection_MakeHeader::SetOriginatingSystem(const Handle(TCollection_HAsciiString)& aOriginatingSystem)
{
	if (!fn.IsNull()) fn->SetOriginatingSystem(aOriginatingSystem);
}

Handle(TCollection_HAsciiString) APIHeaderSection_MakeHeader::OriginatingSystem() const
{
	return (fn.IsNull() ? nulstr : fn->OriginatingSystem());
}

void APIHeaderSection_MakeHeader::SetAuthorisation(const Handle(TCollection_HAsciiString)& aAuthorisation)
{
	if (!fn.IsNull()) fn->SetAuthorisation(aAuthorisation);
}

Handle(TCollection_HAsciiString) APIHeaderSection_MakeHeader::Authorisation() const
{
	return (fn.IsNull() ? nulstr : fn->Authorisation());
}

// ===========
// File Schema
// ===========

Standard_Boolean APIHeaderSection_MakeHeader::HasFs() const
{  return (!fs.IsNull());  }

Handle(HeaderSection_FileSchema) APIHeaderSection_MakeHeader::FsValue() const
{
  return fs;
}

void APIHeaderSection_MakeHeader::SetSchemaIdentifiers(const Handle(Interface_HArray1OfHAsciiString)& aSchemaIdentifiers)
{
	if (!fs.IsNull()) fs->SetSchemaIdentifiers(aSchemaIdentifiers);
}

void APIHeaderSection_MakeHeader::SetSchemaIdentifiersValue(const Standard_Integer num, const Handle(TCollection_HAsciiString)& aSchem)
{
  if (fs.IsNull()) return;
  Handle(Interface_HArray1OfHAsciiString) li = fs->SchemaIdentifiers();
  if (num >= li->Lower() && num <= li->Upper()) li->SetValue(num,aSchem);
}

Handle(Interface_HArray1OfHAsciiString) APIHeaderSection_MakeHeader::SchemaIdentifiers() const
{
	return (fs.IsNull() ? nularr : fs->SchemaIdentifiers());
}

Handle(TCollection_HAsciiString) APIHeaderSection_MakeHeader::SchemaIdentifiersValue(const Standard_Integer num) const
{
	return (fs.IsNull() ? nulstr : fs->SchemaIdentifiersValue(num));
}

Standard_Integer APIHeaderSection_MakeHeader::NbSchemaIdentifiers () const
{
	return (fs.IsNull() ? 0 : fs->NbSchemaIdentifiers());
}

//=======================================================================
//function : AddSchemaIdentifier
//purpose  : 
//=======================================================================

void APIHeaderSection_MakeHeader::AddSchemaIdentifier(const Handle(TCollection_HAsciiString)& aSchem)
{
  if ( fs.IsNull() ) fs = new HeaderSection_FileSchema;
  Handle(Interface_HArray1OfHAsciiString) idents = fs->SchemaIdentifiers();

  // check that requested subschema is already in the list
  Standard_Integer i;
  for ( i=1; ! idents.IsNull() && i <= idents->Length(); i++ ) {
    if ( aSchem->IsSameString ( idents->Value(i) ) ) return;
  }
  
  // add a subshema
  Handle(Interface_HArray1OfHAsciiString) ids = 
    new Interface_HArray1OfHAsciiString ( 1, ( idents.IsNull() ? 1 : idents->Length() + 1 ) );
  for ( i=1; ! idents.IsNull() && i <= idents->Length(); i++ ) {
    ids->SetValue ( i, idents->Value(i) );
  }
  ids->SetValue ( i, aSchem );
  
  fs->SetSchemaIdentifiers ( ids );
}

// ================
// File Description
// ================

Standard_Boolean APIHeaderSection_MakeHeader::HasFd() const
{  return (!fd.IsNull());  }

Handle(HeaderSection_FileDescription) APIHeaderSection_MakeHeader::FdValue() const
{
  return fd;
}

void APIHeaderSection_MakeHeader::SetDescription(const Handle(Interface_HArray1OfHAsciiString)& aDescription)
{
	if (!fs.IsNull()) fd->SetDescription(aDescription);
}

void APIHeaderSection_MakeHeader::SetDescriptionValue(const Standard_Integer num, const Handle(TCollection_HAsciiString)& aDescr)
{
  if (fd.IsNull()) return;
  Handle(Interface_HArray1OfHAsciiString) li = fd->Description();
  if (num >= li->Lower() && num <= li->Upper()) li->SetValue(num,aDescr);
}

Handle(Interface_HArray1OfHAsciiString) APIHeaderSection_MakeHeader::Description() const
{
	return (fd.IsNull() ? nularr : fd->Description());
}

Handle(TCollection_HAsciiString) APIHeaderSection_MakeHeader::DescriptionValue(const Standard_Integer num) const
{
	return (fd.IsNull() ? nulstr : fd->DescriptionValue(num));
}

Standard_Integer APIHeaderSection_MakeHeader::NbDescription () const
{
	return (fd.IsNull() ? 0 : fd->NbDescription());
}

void APIHeaderSection_MakeHeader::SetImplementationLevel(const Handle(TCollection_HAsciiString)& aImplementationLevel)
{
	if (!fd.IsNull()) fd->SetImplementationLevel(aImplementationLevel);
}

Handle(TCollection_HAsciiString) APIHeaderSection_MakeHeader::ImplementationLevel() const
{
	return (fd.IsNull() ? nulstr : fd->ImplementationLevel());
}
