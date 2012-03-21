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

#include <STEPEdit_EditSDR.ixx>
#include <Interface_TypedValue.hxx>
#include <TCollection_HAsciiString.hxx>

#include <StepData_StepModel.hxx>
#include <StepShape_ShapeDefinitionRepresentation.hxx>
#include <STEPConstruct_Part.hxx>


STEPEdit_EditSDR::STEPEdit_EditSDR ()
: IFSelect_Editor (11)
{
  Handle(Interface_TypedValue) pdc_name = new Interface_TypedValue("PDC_Name");
  SetValue (1,pdc_name,"PDC_Name");
  Handle(Interface_TypedValue) pdc_lfs  = new Interface_TypedValue("PDC_LifeCycleStage");
  SetValue(2,pdc_lfs,"PDC_Stage");
  Handle(Interface_TypedValue) pdc_descr = new Interface_TypedValue("PDC_Description");
  SetValue(3,pdc_descr,"PDC_Descr");

  Handle(Interface_TypedValue) pv_vid = new Interface_TypedValue("P_Version_Id");
  SetValue(4,pv_vid,"PV_Id");
  Handle(Interface_TypedValue) pv_descr = new Interface_TypedValue("P_Version_Description");
  SetValue(5,pv_descr,"PV_Descr");

  Handle(Interface_TypedValue) pid = new Interface_TypedValue("Product_Id");
  SetValue(6,pid,"P_Id");
  Handle(Interface_TypedValue) pname = new Interface_TypedValue("Product_Name");
  SetValue(7,pname,"P_Name");
  Handle(Interface_TypedValue) pdescr = new Interface_TypedValue("Product_Description");
  SetValue(8,pdescr,"P_Descr");

  Handle(Interface_TypedValue) pc_name = new Interface_TypedValue("P_Context_Name");
  SetValue(9,pc_name,"PC_Name");
  Handle(Interface_TypedValue) pc_disc = new Interface_TypedValue("P_Context_Discipline");
  SetValue(10,pc_disc,"PC_Disc");

  Handle(Interface_TypedValue) ac_ap = new Interface_TypedValue("Application");
  SetValue(11,ac_ap,"Appli");


//  ?? :
//Handle(TCollection_HAsciiString) StepPDR_SDRtool::PDSname() const
//Handle(TCollection_HAsciiString) StepPDR_SDRtool::PDSdescription() const
//Handle(TCollection_HAsciiString) StepPDR_SDRtool::PDdescription() const

}

TCollection_AsciiString  STEPEdit_EditSDR::Label () const
{  return TCollection_AsciiString ("STEP : Product Data (SDR)");  }

Standard_Boolean  STEPEdit_EditSDR::Recognize
(const Handle(IFSelect_EditForm)& form) const
{
// il faut 10 parametres
  return Standard_True;
}

Handle(TCollection_HAsciiString)  STEPEdit_EditSDR::StringValue
(const Handle(IFSelect_EditForm)& form, const Standard_Integer num) const
{
  Handle(TCollection_HAsciiString) str;
  switch (num) {
  case  1 : return new TCollection_HAsciiString("as proposed");
  case  2 : return new TCollection_HAsciiString("First_Design");
  case  3 : return new TCollection_HAsciiString("void");
  case  4 : return new TCollection_HAsciiString("ID");
  case  5 : return new TCollection_HAsciiString("void");
  case  8 : return new TCollection_HAsciiString("void");
  case  9 : return new TCollection_HAsciiString("Mechanical");
  case 10 : return new TCollection_HAsciiString("Assembly");
  case 11 : return new TCollection_HAsciiString("EUCLID");
    default : break;
  }
  return str;
}

Standard_Boolean  STEPEdit_EditSDR::Load
(const Handle(IFSelect_EditForm)& form,
 const Handle(Standard_Transient)& ent,
 const Handle(Interface_InterfaceModel)& model) const
{
  Handle(StepShape_ShapeDefinitionRepresentation) sdr =
    Handle(StepShape_ShapeDefinitionRepresentation)::DownCast (ent);
  Handle(StepData_StepModel) modl =
    Handle(StepData_StepModel)::DownCast(model);
  if (sdr.IsNull() || modl.IsNull()) return Standard_False;

  STEPConstruct_Part ctx;
  ctx.ReadSDR (sdr);

  form->LoadValue (1, ctx.PDCname());
  form->LoadValue (2, ctx.PDCstage());
  form->LoadValue (3, ctx.PDdescription());

  form->LoadValue (4, ctx.PDFid());
  form->LoadValue (5, ctx.PDFdescription());

  form->LoadValue (6, ctx.Pid());
  form->LoadValue (7, ctx.Pname());
  form->LoadValue (8, ctx.Pdescription());

  form->LoadValue (9, ctx.PCname());
  form->LoadValue (10, ctx.PCdisciplineType());

  form->LoadValue (11, ctx.ACapplication());

  return Standard_True;
}

Standard_Boolean  STEPEdit_EditSDR::Apply
(const Handle(IFSelect_EditForm)& form,
 const Handle(Standard_Transient)& ent,
 const Handle(Interface_InterfaceModel)& model) const
{
  Handle(StepShape_ShapeDefinitionRepresentation) sdr =
    Handle(StepShape_ShapeDefinitionRepresentation)::DownCast (ent);
  Handle(StepData_StepModel) modl =
    Handle(StepData_StepModel)::DownCast(model);
  if (sdr.IsNull() || modl.IsNull()) return Standard_False;

  STEPConstruct_Part ctx;
  ctx.ReadSDR (sdr);

  if (form->IsModified(1)) ctx.SetPDCname (form->EditedValue(1));
  if (form->IsModified(2)) ctx.SetPDCstage (form->EditedValue(2));
  if (form->IsModified(3)) ctx.SetPDdescription (form->EditedValue(3));

  if (form->IsModified(4)) ctx.SetPDFid (form->EditedValue(4));
  if (form->IsModified(5)) ctx.SetPDFdescription (form->EditedValue(5));

  if (form->IsModified(6)) ctx.SetPid (form->EditedValue(6));
  if (form->IsModified(7)) ctx.SetPname (form->EditedValue(7));
  if (form->IsModified(8)) ctx.SetPdescription (form->EditedValue(8));

  if (form->IsModified(9)) ctx.SetPCname (form->EditedValue(9));
  if (form->IsModified(10)) ctx.SetPCdisciplineType (form->EditedValue(10));

  if (form->IsModified(11)) ctx.SetACapplication (form->EditedValue(11));

  return Standard_True;
}
