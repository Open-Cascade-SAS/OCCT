// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <STEPEdit_EditContext.ixx>
#include <Interface_TypedValue.hxx>
#include <TCollection_HAsciiString.hxx>

#include <StepData_StepModel.hxx>
#include <STEPConstruct_ContextTool.hxx>


STEPEdit_EditContext::STEPEdit_EditContext ()
: IFSelect_Editor (5)
{
  Handle(Interface_TypedValue) ac_val = new Interface_TypedValue("Application Context",Interface_ParamIdent);
  SetValue(1,ac_val,"AC",IFSelect_EditRead);

  Handle(Interface_TypedValue) ac_status = new Interface_TypedValue("AppliContext_Status");
  SetValue(2,ac_status,"AC_Status");
  Handle(Interface_TypedValue) ac_sche = new Interface_TypedValue("AppliContext_Schema");
  SetValue(3,ac_sche,"AC_Schema");
  Handle(Interface_TypedValue) ac_year = new Interface_TypedValue("AppliContext_Year",Interface_ParamInteger);
  SetValue(4,ac_year,"AC_Year");
  Handle(Interface_TypedValue) ac_name = new Interface_TypedValue("AppliContext_Name");
  SetValue(5,ac_name,"AC_Name");

//  Handle(Interface_TypedValue) prpc_val = new Interface_TypedValue("Product Related Product Category",Interface_ParamIdent);
//  SetValue(6,ac_val,"PRPC",IFSelect_EditRead);

//  Handle(Interface_TypedValue) prpc_name = new Interface_TypedValue("PRPC_Name");
//  SetValue(7,prpc_name,"PRPC_Name");
//  Handle(Interface_TypedValue) prpc_descr = new Interface_TypedValue("PRPC_Description");
//  SetValue(8,prpc_descr,"PRPC_Descr");
}

TCollection_AsciiString  STEPEdit_EditContext::Label () const
{  return TCollection_AsciiString ("STEP : Product Definition Context");  }

Standard_Boolean  STEPEdit_EditContext::Recognize
(const Handle(IFSelect_EditForm)& form) const
{
// il faut 17 parametres
  return Standard_True;
}

Handle(TCollection_HAsciiString)  STEPEdit_EditContext::StringValue
(const Handle(IFSelect_EditForm)& form, const Standard_Integer num) const
{
  Handle(TCollection_HAsciiString) str;
  switch (num) {
  case 2 : return new TCollection_HAsciiString("DIS");
  case 3 : return new TCollection_HAsciiString("automotive_design");
  case 4 : return new TCollection_HAsciiString("1998");
  case 5 : return new TCollection_HAsciiString("EUCLID");
  case 7 : return new TCollection_HAsciiString("Undefined Category");
  case 8 : return new TCollection_HAsciiString("Undefined Description");
    default : break;
  }
  return str;
}

Standard_Boolean  STEPEdit_EditContext::Load
(const Handle(IFSelect_EditForm)& form,
 const Handle(Standard_Transient)& ent,
 const Handle(Interface_InterfaceModel)& model) const
{
  Handle(StepData_StepModel) modl = 
    Handle(StepData_StepModel)::DownCast(model);
  if (modl.IsNull()) return Standard_False;

  STEPConstruct_ContextTool ctx (modl);

  form->LoadValue (1, modl->StringLabel(ctx.GetAPD()) );

  form->LoadValue (2, ctx.GetACstatus());
  form->LoadValue (3, ctx.GetACschemaName());
  form->LoadValue (4, new TCollection_HAsciiString(ctx.GetACyear()) );
  form->LoadValue (5, ctx.GetACname());

//  form->LoadValue (6, modl->StringLabel(ctx.GetPRPC()) );

//  form->LoadValue (7, ctx.GetPRPCName());
//  form->LoadValue (8, ctx.GetPRPCDescription());

  return Standard_True;
}

Standard_Boolean  STEPEdit_EditContext::Apply
(const Handle(IFSelect_EditForm)& form,
 const Handle(Standard_Transient)& ent,
 const Handle(Interface_InterfaceModel)& model) const
{
  Handle(StepData_StepModel) modl = 
    Handle(StepData_StepModel)::DownCast(model);
  if (modl.IsNull()) return Standard_False;

  STEPConstruct_ContextTool ctx (modl);

  ctx.AddAPD();  // on ne sait jamais
//  ctx.AddPRPC();

  if (form->IsModified(2)) ctx.SetACstatus        (form->EditedValue(2));
  if (form->IsModified(3)) ctx.SetACschemaName    (form->EditedValue(3));
  if (form->IsModified(4)) ctx.SetACyear(form->EditedValue(4)->IntegerValue());
  if (form->IsModified(5)) ctx.SetACname          (form->EditedValue(5));

//  if (form->IsModified(7)) ctx.SetPRPCName        (form->EditedValue(7));
//  if (form->IsModified(8)) ctx.SetPRPCDescription (form->EditedValue(8));

  return Standard_True;
}
