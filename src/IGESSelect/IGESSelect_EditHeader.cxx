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


#include <IFSelect_EditForm.hxx>
#include <IGESData_BasicEditor.hxx>
#include <IGESData_GlobalSection.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESData_Protocol.hxx>
#include <IGESSelect_EditHeader.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Static.hxx>
#include <Interface_TypedValue.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSelect_EditHeader,IFSelect_Editor)

static Standard_Boolean IsTimeStamp
  (const Handle(TCollection_HAsciiString)& val)
{
  if (val.IsNull()) return Standard_False;
//  La date peut etre sur 13 ou 15 caracteres (15 : bonjour l an 2000!)
//  forme [YY]YYMMDD.HHMMSS
  Standard_Integer lng = val->Length();
  if (lng != 13 && lng != 15) return Standard_False;
  lng -= 13;  // devient 0 ou 2 (offset siecle)

//  Cas du siecle present :
  if (lng == 2) {
    char uncar = val->Value(1);
    if (uncar != '1' && uncar != '2') return Standard_False;
    uncar = val->Value(2);
    if (uncar < '0' || uncar > '9') return Standard_False;
  }

//  On y va
  char dizmois = val->Value(lng+3);
  char dizjour = val->Value(lng+5);
  char dizheur = val->Value(lng+8);
  for (Standard_Integer i = 1; i <= 13; i ++) {
    char uncar = val->Value(i+lng);
    switch (i) {
    case  1 :
    case  2 : if (uncar < '0' || uncar > '9') return Standard_False; break;
    case  3 : if (uncar != '0' && uncar != '1') return Standard_False;  break;
    case  4 : if (uncar < '0' || uncar > '9') return Standard_False;
      if (dizmois == '1' && (uncar < '0' || uncar > '2')) return Standard_False;
      break;
    case  5 : if (uncar < '0' || uncar > '3') return Standard_False; break;
    case  6 : if (uncar < '0' || uncar > '9') return Standard_False;
      if (dizjour == '3' && (uncar != '0' && uncar != '1')) return Standard_False;
      break;
    case  7 : if (uncar != '.') return Standard_False;  break;
    case  8 : if (uncar < '0' || uncar > '2') return Standard_False; break;
    case  9 : if (uncar < '0' || uncar > '9') return Standard_False;
      if (dizheur == '2' && (uncar < '0' || uncar > '3')) return Standard_False; //szv#4:S4163:12Mar99 extra break
      break;
    case 10 : if (uncar < '0' || uncar > '5') return Standard_False; break;
    case 11 : if (uncar < '0' || uncar > '9') return Standard_False; break;
    case 12 : if (uncar < '0' || uncar > '5') return Standard_False; break;
    case 13 : if (uncar < '0' || uncar > '9') return Standard_False; break;
      default : break;
    }
  }
  return Standard_True;
}


    IGESSelect_EditHeader::IGESSelect_EditHeader  ()
    : IFSelect_Editor (30)
{
  Standard_Integer i,nb;
//  Definition
  Handle(Interface_TypedValue) start = new Interface_TypedValue("Start Section");
  start->SetMaxLength(72);
  SetValue (1,start,"Start");
  SetList  (1);

  Handle(Interface_TypedValue) sep = new Interface_TypedValue("Parameter Delimiter");
  sep->SetMaxLength(1);
  SetValue (2,sep,"G1:Separator",IFSelect_Optional);
  Handle(Interface_TypedValue) endmark = new Interface_TypedValue("Record Delimiter");
  endmark->SetMaxLength(1);
  SetValue (3,endmark,"G2:EndMark",IFSelect_Optional);

  Handle(Interface_TypedValue) sendname = new Interface_TypedValue("Sender Product Id");
  SetValue (4,sendname,"G3:SendName",IFSelect_Optional);

  Handle(Interface_TypedValue) filename = new Interface_TypedValue("File Name");
  SetValue (5,filename,"G4:FileName");

  Handle(Interface_TypedValue) systid = new Interface_TypedValue("Native System Id");
  SetValue (6,systid,"G5:SystemId");

  Handle(Interface_TypedValue) version = new Interface_TypedValue("Preprocessor Version");
  SetValue (7,version,"G6:Version");

  Handle(Interface_TypedValue) intbits = new Interface_TypedValue("Integer Binary Bits",Interface_ParamInteger);
  SetValue (8,intbits,"G7:IntBits");
  Handle(Interface_TypedValue) pow10s = new Interface_TypedValue("Single Precision Magnitude",Interface_ParamInteger);
  SetValue (9,pow10s,"G8:SingleMag");
  Handle(Interface_TypedValue) dig10s = new Interface_TypedValue("Single Precision Significance",Interface_ParamInteger);
  SetValue (10,dig10s,"G9:SingDigits");
  Handle(Interface_TypedValue) pow10d = new Interface_TypedValue("Double Precision Magnitude",Interface_ParamInteger);
  SetValue (11,pow10d,"G10:DoubleMag");
  Handle(Interface_TypedValue) dig10d = new Interface_TypedValue("Double Precision Significance",Interface_ParamInteger);
  SetValue (12,dig10d,"G11:DoubDigits");

  Handle(Interface_TypedValue) recname = new Interface_TypedValue("Receiver Product Id");
  SetValue (13,recname,"G12:Receiver",IFSelect_Optional);

  Handle(Interface_TypedValue) scale = new Interface_TypedValue("Model Space Scale",Interface_ParamReal);
  SetValue (14,scale,"G13:Scale",IFSelect_Optional);

  Handle(Interface_TypedValue) unitflag = new Interface_TypedValue("Units Flag",Interface_ParamInteger);
  unitflag->SetIntegerLimit(Standard_False,1);
  unitflag->SetIntegerLimit(Standard_True,11);
  SetValue (15,unitflag,"G14:UnitFlag",IFSelect_Optional);
//  On prend a la source ...  Mieux vaudrait "recopier" les definitions ...
  Handle(Interface_TypedValue) unitname = new Interface_TypedValue("Units Name",Interface_ParamEnum);
  unitname->StartEnum (1);
  for (i = 1; i <= 11; i ++)
    unitname->AddEnumValue (IGESData_BasicEditor::UnitFlagName(i),i);
//  similaire a Interface_Static::Static("XSTEP.iges.unit");
  SetValue (16,unitname,"G15:UnitName",IFSelect_Optional);
  Handle(Interface_TypedValue) unitval  = new Interface_TypedValue("Computed Unit Value",Interface_ParamReal);
  SetValue (17,unitval,"V15:UnitValue",IFSelect_EditDynamic);

  Handle(Interface_TypedValue) linwgr = new Interface_TypedValue("Max Line Weight Gradation",Interface_ParamInteger);
  SetValue (18,linwgr,"G16:LineWGrad",IFSelect_Optional);
  Handle(Interface_TypedValue) maxlw = new Interface_TypedValue("Width of Max Line Weight",Interface_ParamReal);
  SetValue (19,maxlw,"G17:MaxLineW");

  Handle(Interface_TypedValue) filedate = new Interface_TypedValue("Date of File Creation");
  filedate->SetSatisfies (IsTimeStamp,"IsIGESDate");
  SetValue (20,filedate,"G18:FileDate");

  Handle(Interface_TypedValue) resol = new Interface_TypedValue("Max Resolution",Interface_ParamReal);
  SetValue (21,resol,"G19:Resolution");
  Handle(Interface_TypedValue) coord = new Interface_TypedValue("Max Coordinates",Interface_ParamReal);
  SetValue (22,coord,"G20:MaxCoord",IFSelect_Optional);

  Handle(Interface_TypedValue) author = new Interface_TypedValue("Name of Author");
  SetValue (23,author,"G21:Author");
  Handle(Interface_TypedValue) company = new Interface_TypedValue("Author Organization");
  SetValue (24,company,"G22:Company");

  Handle(Interface_TypedValue) igesvers = new Interface_TypedValue("Version Flag",Interface_ParamInteger);
  nb = IGESData_BasicEditor::IGESVersionMax();
  igesvers->SetIntegerLimit(Standard_False,1);
  igesvers->SetIntegerLimit(Standard_True,nb);
  SetValue (25,igesvers,"G23:IGESVersion");
  Handle(Interface_TypedValue) versname = new Interface_TypedValue("IGES Version Name",Interface_ParamEnum);
  versname->StartEnum (0);
  for (i = 0; i <= IGESData_BasicEditor::IGESVersionMax(); i ++)
    versname->AddEnumValue (IGESData_BasicEditor::IGESVersionName(i),i);
  SetValue (26,versname,"V23:VersionName");

  Handle(Interface_TypedValue) draft = new Interface_TypedValue("Drafting Standard Flag",Interface_ParamInteger);
  nb = IGESData_BasicEditor::DraftingMax();
  draft->SetIntegerLimit(Standard_False,0);
  draft->SetIntegerLimit(Standard_True,nb);
  SetValue (27,draft,"G24:Drafting");
  Handle(Interface_TypedValue) draftname = new Interface_TypedValue("Drafting Standard Name",Interface_ParamEnum);
  draftname->StartEnum (0);
  for (i = 0; i <= nb; i ++)
    draftname->AddEnumValue (IGESData_BasicEditor::DraftingName(i),i);
  SetValue (28,draftname,"V24:DraftingName");

  Handle(Interface_TypedValue) changedate = new Interface_TypedValue("Date of Creation/Change");
  changedate->SetSatisfies (IsTimeStamp,"IsIGESDate");
  SetValue (29,changedate,"G25:ChangeDate",IFSelect_Optional);

  Handle(Interface_TypedValue) proto = new Interface_TypedValue("Application Protocol/Subset Id");
  SetValue (30,proto,"G26:Protocol",IFSelect_Optional);

}

    TCollection_AsciiString  IGESSelect_EditHeader::Label () const
      {  return TCollection_AsciiString ("IGES Header");  }

    Standard_Boolean  IGESSelect_EditHeader::Recognize
  (const Handle(IFSelect_EditForm)& /*form*/) const
{  return Standard_True;  }  // ??

    Handle(TCollection_HAsciiString)  IGESSelect_EditHeader::StringValue
  (const Handle(IFSelect_EditForm)& /*form*/, const Standard_Integer num) const
{
//  Default Values
  return TypedValue(num)->HStringValue();
}

    Standard_Boolean  IGESSelect_EditHeader::Load
  (const Handle(IFSelect_EditForm)& form,
   const Handle(Standard_Transient)& /*ent*/,
   const Handle(Interface_InterfaceModel)& model) const
{
  Handle(IGESData_IGESModel) modl =
    Handle(IGESData_IGESModel)::DownCast(model);
  if (modl.IsNull()) return Standard_False;

  IGESData_GlobalSection GS = modl->GlobalSection();

  form->LoadList  (1 ,modl->StartSection());
  form->LoadValue (2 ,new TCollection_HAsciiString(GS.Separator()) );
  form->LoadValue (3 ,new TCollection_HAsciiString(GS.EndMark()) );
  form->LoadValue (4 ,GS.SendName());
  form->LoadValue (5 ,GS.FileName());
  form->LoadValue (6 ,GS.SystemId());
  form->LoadValue (7 ,GS.InterfaceVersion());

  form->LoadValue (8 ,new TCollection_HAsciiString(GS.IntegerBits()) );
  form->LoadValue (9 ,new TCollection_HAsciiString(GS.MaxPower10Single()) );
  form->LoadValue (10 ,new TCollection_HAsciiString(GS.MaxDigitsSingle()) );
  form->LoadValue (11 ,new TCollection_HAsciiString(GS.MaxPower10Double()) );
  form->LoadValue (12 ,new TCollection_HAsciiString(GS.MaxDigitsDouble()) );

  form->LoadValue (13 ,GS.ReceiveName());
  form->LoadValue (14 ,new TCollection_HAsciiString(GS.Scale()) );

  form->LoadValue (15 ,new TCollection_HAsciiString(GS.UnitFlag()) );
  form->LoadValue (16 ,GS.UnitName());
  form->LoadValue (17 ,new TCollection_HAsciiString(GS.UnitValue()) );
  form->LoadValue (18 ,new TCollection_HAsciiString(GS.LineWeightGrad()) );
  form->LoadValue (19 ,new TCollection_HAsciiString(GS.MaxLineWeight()) );

  form->LoadValue (20 ,GS.Date());
  form->LoadValue (21 ,new TCollection_HAsciiString(GS.Resolution()) );
  if (GS.HasMaxCoord()) form->LoadValue (22 ,new TCollection_HAsciiString(GS.MaxCoord()) );

  form->LoadValue (23 ,GS.AuthorName());
  form->LoadValue (24 ,GS.CompanyName());
  form->LoadValue (25 ,new TCollection_HAsciiString(GS.IGESVersion()) );
  form->LoadValue (26 ,new TCollection_HAsciiString
		   (IGESData_BasicEditor::IGESVersionName(GS.IGESVersion()) ));
  form->LoadValue (27 ,new TCollection_HAsciiString(GS.DraftingStandard()) );
  form->LoadValue (28 ,new TCollection_HAsciiString
		   (IGESData_BasicEditor::DraftingName(GS.DraftingStandard()) ));
  form->LoadValue (29 ,GS.LastChangeDate());
  form->LoadValue (30 ,GS.ApplicationProtocol());

  return Standard_True;
}

    Standard_Boolean  IGESSelect_EditHeader::Update
  (const Handle(IFSelect_EditForm)& form,
   const Standard_Integer num,
   const Handle(TCollection_HAsciiString)& val,
   const Standard_Boolean enforce) const
{
  if (num == 15) {
    if (!enforce) return Standard_False;  // quand meme ...
//    Unit Flag : mettre a jour UnitName et UnitValue
    Standard_Integer unitflag = val->IntegerValue();
    Standard_CString unitname = IGESData_BasicEditor::UnitFlagName (unitflag);
    if (unitname[0] == '\0') return Standard_False;
    form->Touch (16,new TCollection_HAsciiString (unitname));
    form->Touch (17,new TCollection_HAsciiString
		 (IGESData_BasicEditor::UnitFlagValue(unitflag)) );
  }
  if (num == 16) {
    if (!enforce) return Standard_False;  // quand meme ...
//    Unit Name : mettre a jour UnitFlag et UnitValue
    Standard_Integer unitflag = IGESData_BasicEditor::UnitNameFlag
      (val->ToCString());
    if (unitflag == 0) return Standard_False;  // pas bon
    form->Touch (15,new TCollection_HAsciiString (unitflag));
    form->Touch (17,new TCollection_HAsciiString
		 (IGESData_BasicEditor::UnitFlagValue(unitflag)) );
  }

  if (num == 25) {
//    Unit Version : mettre a jour son nom
    Standard_Integer version = 3;  // par defaut ...
    if (!val.IsNull()) version = atoi(val->ToCString());
    Standard_CString versname = IGESData_BasicEditor::IGESVersionName(version);
    if (versname[0] == '\0') return Standard_False;
    form->Touch (26,new TCollection_HAsciiString (versname));
  }
  if (num == 27) {
//   Drafting : mettre a jour son nom
    Standard_Integer draft = 0;
    if (!val.IsNull()) draft = atoi(val->ToCString());
    Standard_CString draftname = IGESData_BasicEditor::IGESVersionName(draft);
    if (draftname[0] == '\0') return Standard_False;
    form->Touch (28,new TCollection_HAsciiString (draftname));
  }
  return Standard_True;
}

    Standard_Boolean  IGESSelect_EditHeader::Apply
  (const Handle(IFSelect_EditForm)& form,
   const Handle(Standard_Transient)& /*ent*/,
   const Handle(Interface_InterfaceModel)& model) const
{
  Handle(IGESData_IGESModel) modl =
    Handle(IGESData_IGESModel)::DownCast(model);
  if (modl.IsNull()) return Standard_False;

  IGESData_GlobalSection GS = modl->GlobalSection();

  Handle(TCollection_HAsciiString) str;

  if (form->IsModified(1))  modl->SetStartSection (form->EditedList(1));
  if (form->IsModified(2)) {
    str = form->EditedValue(2);
    if (!str.IsNull() && str->Length() >= 1) GS.SetSeparator (str->Value(1));
  }
  if (form->IsModified(3)) {
    str = form->EditedValue(3);
    if (!str.IsNull() && str->Length() >= 1) GS.SetEndMark (str->Value(1));
  }
  if (form->IsModified(4))  GS.SetSendName (form->EditedValue(4));
  if (form->IsModified(5))  GS.SetFileName (form->EditedValue(5));
  if (form->IsModified(6))  GS.SetSystemId (form->EditedValue(6));
  if (form->IsModified(7))  GS.SetInterfaceVersion (form->EditedValue(7));

  if (form->IsModified(8))  GS.SetIntegerBits (form->EditedValue(8)->IntegerValue());
  if (form->IsModified(9))  GS.SetMaxPower10Single (form->EditedValue(9)->IntegerValue());
  if (form->IsModified(10)) GS.SetMaxDigitsSingle (form->EditedValue(10)->IntegerValue());
  if (form->IsModified(11)) GS.SetMaxPower10Double (form->EditedValue(11)->IntegerValue());
  if (form->IsModified(12)) GS.SetMaxDigitsDouble (form->EditedValue(12)->IntegerValue());

  if (form->IsModified(13)) GS.SetReceiveName (form->EditedValue(13));
  if (form->IsModified(14)) GS.SetScale (form->EditedValue(14)->RealValue());
  if (form->IsModified(15)) GS.SetUnitFlag (form->EditedValue(15)->IntegerValue());
  if (form->IsModified(16)) GS.SetUnitName (form->EditedValue(16));

  if (form->IsModified(18)) GS.SetLineWeightGrad (form->EditedValue(18)->IntegerValue());
  if (form->IsModified(19)) GS.SetMaxLineWeight (form->EditedValue(19)->RealValue());

  if (form->IsModified(20)) GS.SetDate (form->EditedValue(20));
  if (form->IsModified(21)) GS.SetResolution (form->EditedValue(21)->RealValue());
  if (form->IsModified(22)) {
    str = form->EditedValue(22);
    if (str.IsNull()) GS.SetMaxCoord();
    else GS.SetMaxCoord (str->RealValue());
  }

  if (form->IsModified(23)) GS.SetAuthorName (form->EditedValue(23));
  if (form->IsModified(24)) GS.SetCompanyName (form->EditedValue(24));
  if (form->IsModified(25)) GS.SetIGESVersion (form->EditedValue(25)->IntegerValue());
  if (form->IsModified(27)) GS.SetDraftingStandard (form->EditedValue(27)->IntegerValue());
  if (form->IsModified(29)) GS.SetLastChangeDate (form->EditedValue(29));
  if (form->IsModified(30)) GS.SetApplicationProtocol (form->EditedValue(30));

  modl->SetGlobalSection (GS);

//  Pour l unite
  if (form->IsModified(15) || form->IsModified(16)) {
    IGESData_BasicEditor bed
      (modl,Handle(IGESData_Protocol)::DownCast(modl->Protocol()) );
    if (bed.SetUnitValue (GS.UnitValue()) ) return Standard_False;
    bed.ApplyUnit (Standard_True);
  }

  return Standard_True;
}
