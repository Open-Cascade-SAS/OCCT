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


#include <APIHeaderSection_EditHeader.hxx>
#include <APIHeaderSection_MakeHeader.hxx>
#include <IFSelect_EditForm.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_TypedValue.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <StepData_StepModel.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(APIHeaderSection_EditHeader,IFSelect_Editor)

static Standard_Boolean IsTimeStamp
  (const Handle(TCollection_HAsciiString)& val)
{
  if (val.IsNull()) return Standard_False;
  if (val->Length() != 19) return Standard_False;
//  On y va
  char dizmois = val->Value(6);
  char dizjour = val->Value(9);
  char dizheur = val->Value(12);
  for (Standard_Integer i = 1; i <= 19; i ++)
  {
    char uncar = val->Value(i);
    switch (i) {
    case  1 : if (uncar != '1' && uncar != '2') return Standard_False;
      break;
    case  2 :
    case  3 :
    case  4 : if (uncar < '0' || uncar > '9') return Standard_False;
      break;
    case  5 : if (uncar != '-') return Standard_False;
    case  6 : if (uncar != '0' && uncar != '1') return Standard_False;
      break;
    case  7 : if (uncar < '0' || uncar > '9') return Standard_False;
      if (dizmois == '1' && (uncar < '0' || uncar > '2')) return Standard_False;
      break;
    case  8 : if (uncar != '-') return Standard_False;
    case  9 : if (uncar < '0' || uncar > '3') return Standard_False;
      break;
    case 10 : if (uncar < '0' || uncar > '9') return Standard_False;
      if (dizjour == '3' && (uncar != '0' && uncar != '1')) return Standard_False;
      break;
    case 11 : if (uncar != 'T') return Standard_False;
    case 12 : if (uncar < '0' || uncar > '2') return Standard_False;
      break;
    case 13 : if (uncar < '0' || uncar > '9') return Standard_False;
      if (dizheur == '2' && (uncar < '0' || uncar > '3')) return Standard_False;
      break;
    case 14 : if (uncar != ':') return Standard_False;
    case 15 : if (uncar < '0' || uncar > '5') return Standard_False;
      break;
    case 16 : if (uncar < '0' || uncar > '9') return Standard_False;
      break;
    case 17 : if (uncar != ':') return Standard_False;
    case 18 : if (uncar < '0' || uncar > '5') return Standard_False;
      break;
    case 19 : if (uncar < '0' || uncar > '9') return Standard_False;
      break;
    default :
      break;
    }
  }
  return Standard_True;
}


    APIHeaderSection_EditHeader::APIHeaderSection_EditHeader  ()
    : IFSelect_Editor (10)
{
//  Definition
  Handle(Interface_TypedValue) fn_name = new Interface_TypedValue("fn_name");
  SetValue (1,fn_name,"name");
  Handle(Interface_TypedValue) fn_time = new Interface_TypedValue("fn_time_stamp");
  fn_time->SetSatisfies (IsTimeStamp,"IsTimeStamp");
  SetValue (2,fn_time,"time");
  Handle(Interface_TypedValue) fn_author = new Interface_TypedValue("fn_author");
  SetValue (3,fn_author,"author");  // 1 seul (1er de liste)
  Handle(Interface_TypedValue) fn_org = new Interface_TypedValue("fn_organization");
  SetValue (4,fn_org,"org");  // 1 seul (1er de liste)
  Handle(Interface_TypedValue) fn_preproc = new Interface_TypedValue("fn_preprocessor_version");
  SetValue (5,fn_preproc,"preproc");
  Handle(Interface_TypedValue) fn_orig = new Interface_TypedValue("fn_originating_system");
  SetValue (6,fn_orig,"orig");
  Handle(Interface_TypedValue) fn_autorize = new Interface_TypedValue("fn_authorization");
  SetValue (7,fn_autorize,"autorize");

  Handle(Interface_TypedValue) fs_schema = new Interface_TypedValue("fs_schema_identifiers");
  SetValue (8,fs_schema,"schema");  // 1 seul (1er de liste)

  Handle(Interface_TypedValue) fd_descr = new Interface_TypedValue("fd_description");
  SetValue (9,fd_descr,"descr");  // 1 seul (1er de liste)

  Handle(Interface_TypedValue) fd_level = new Interface_TypedValue("fd_implementation_level");
  SetValue (10,fd_level,"level");

}

    TCollection_AsciiString  APIHeaderSection_EditHeader::Label () const
      {  return TCollection_AsciiString ("Step Header");  }

    Standard_Boolean  APIHeaderSection_EditHeader::Recognize
  (const Handle(IFSelect_EditForm)& /*form*/) const
{  return Standard_True;  }  // ??

    Handle(TCollection_HAsciiString)  APIHeaderSection_EditHeader::StringValue
  (const Handle(IFSelect_EditForm)& /*form*/, const Standard_Integer num) const
{
//  Default Values
  return TypedValue(num)->HStringValue();
}

    Standard_Boolean  APIHeaderSection_EditHeader::Load
  (const Handle(IFSelect_EditForm)& form,
   const Handle(Standard_Transient)& /*ent*/,
   const Handle(Interface_InterfaceModel)& model) const
{
  Handle(StepData_StepModel) modl =
    Handle(StepData_StepModel)::DownCast(model);
  if (modl.IsNull()) return Standard_False;

  APIHeaderSection_MakeHeader mkh (modl);

  form->LoadValue (1 ,mkh.Name ());
  form->LoadValue (2 ,mkh.TimeStamp ());
  form->LoadValue (3 ,mkh.AuthorValue (1));
  form->LoadValue (4 ,mkh.OrganizationValue (1));
  form->LoadValue (5 ,mkh.PreprocessorVersion ());
  form->LoadValue (6 ,mkh.OriginatingSystem ());
  form->LoadValue (7 ,mkh.Authorisation ());

  form->LoadValue (8 ,mkh.SchemaIdentifiersValue (1));

  form->LoadValue (9 ,mkh.DescriptionValue (1));
  form->LoadValue (10,mkh.ImplementationLevel ());

  return Standard_True;
}

    Standard_Boolean  APIHeaderSection_EditHeader::Apply
  (const Handle(IFSelect_EditForm)& form,
   const Handle(Standard_Transient)& /*ent*/,
   const Handle(Interface_InterfaceModel)& model) const
{
  Handle(StepData_StepModel) modl =
    Handle(StepData_StepModel)::DownCast(model);
  if (modl.IsNull()) return Standard_False;

  APIHeaderSection_MakeHeader mkh (modl);

  if (form->IsModified(1))  mkh.SetName (form->EditedValue(1));
  if (form->IsModified(2))  mkh.SetTimeStamp (form->EditedValue(2));
  if (form->IsModified(3))  mkh.SetAuthorValue (1,form->EditedValue(3));
  if (form->IsModified(4))  mkh.SetOrganizationValue (1,form->EditedValue(4));
  if (form->IsModified(5))  mkh.SetPreprocessorVersion (form->EditedValue(5));
  if (form->IsModified(6))  mkh.SetOriginatingSystem (form->EditedValue(6));
  if (form->IsModified(7))  mkh.SetAuthorisation (form->EditedValue(7));

  if (form->IsModified(8))  mkh.SetSchemaIdentifiersValue (1,form->EditedValue(8));

  if (form->IsModified(9))  mkh.SetDescriptionValue (1,form->EditedValue(9));
  if (form->IsModified(10)) mkh.SetImplementationLevel (form->EditedValue(10));

  mkh.Apply( Handle(StepData_StepModel)::DownCast(model) );

  return Standard_True;
}
