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
#include <IGESData_ColorEntity.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESData_LabelDisplayEntity.hxx>
#include <IGESData_LevelListEntity.hxx>
#include <IGESData_LineFontEntity.hxx>
#include <IGESData_TransfEntity.hxx>
#include <IGESData_ViewKindEntity.hxx>
#include <IGESSelect_EditDirPart.hxx>
#include <Interface_InterfaceModel.hxx>
#include <MoniTool_Macros.hxx>
#include <Interface_Static.hxx>
#include <Interface_TypedValue.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSelect_EditDirPart, IFSelect_Editor)

static occ::handle<Interface_TypedValue> NewDefType(const char* name)
{
  occ::handle<Interface_TypedValue> deftype = new Interface_TypedValue(name, Interface_ParamEnum);
  deftype->StartEnum(0);
  deftype->AddEnumValue("Void", 0);
  deftype->AddEnumValue("Value", 1);
  deftype->AddEnumValue("Entity", 2);
  return deftype;
}

static occ::handle<Interface_TypedValue> NewDefList(const char* name)
{
  occ::handle<Interface_TypedValue> deftype = new Interface_TypedValue(name, Interface_ParamEnum);
  deftype->StartEnum(0);
  deftype->AddEnumValue("None", 0);
  deftype->AddEnumValue("One", 1);
  deftype->AddEnumValue("List", 2);
  return deftype;
}

IGESSelect_EditDirPart::IGESSelect_EditDirPart()
    : IFSelect_Editor(23)
{
  // int i,nb; //szv#4:S4163:12Mar99 unused
  //   Definition
  occ::handle<Interface_TypedValue> typenum =
    new Interface_TypedValue("Type Number", Interface_ParamInteger);
  SetValue(1, typenum, "D1:Type", IFSelect_EditRead);
  occ::handle<Interface_TypedValue> formnum =
    new Interface_TypedValue("Form Number", Interface_ParamInteger);
  SetValue(2, formnum, "D15:Form", IFSelect_EditRead);

  occ::handle<Interface_TypedValue> stru = new Interface_TypedValue("Structure", Interface_ParamIdent);
  SetValue(3, stru, "D3:Struct", IFSelect_Optional);

  occ::handle<Interface_TypedValue> lftype = NewDefType("Line Font Pattern");
  SetValue(4, lftype, "D4:LineFont", IFSelect_EditDynamic);
  occ::handle<Interface_TypedValue> lfval =
    new Interface_TypedValue("Line Font Number", Interface_ParamInteger);
  lfval->SetIntegerLimit(false, 0);
  SetValue(5, lfval, "N4:LineFont", IFSelect_Optional);
  occ::handle<Interface_TypedValue> lfent =
    new Interface_TypedValue("Line Font Entity", Interface_ParamIdent);
  SetValue(6, lfent, "E4:LineFont", IFSelect_Optional);

  occ::handle<Interface_TypedValue> levlist = NewDefList("Level");
  SetValue(7, levlist, "D5:Level", IFSelect_EditDynamic);
  occ::handle<Interface_TypedValue> leval =
    new Interface_TypedValue("Level Number", Interface_ParamInteger);
  leval->SetIntegerLimit(false, 0);
  SetValue(8, leval, "N5:Level", IFSelect_Optional);
  occ::handle<Interface_TypedValue> levent =
    new Interface_TypedValue("Level List Entity", Interface_ParamIdent);
  SetValue(9, levent, "L5:Level", IFSelect_Optional);

  occ::handle<Interface_TypedValue> viewlist = NewDefList("View");
  SetValue(10, viewlist, "D6:View", IFSelect_EditDynamic);
  occ::handle<Interface_TypedValue> viewent =
    new Interface_TypedValue("View Entity", Interface_ParamIdent);
  SetValue(11, viewent, "E6:View", IFSelect_Optional);

  occ::handle<Interface_TypedValue> transf =
    new Interface_TypedValue("Transformation", Interface_ParamIdent);
  SetValue(12, transf, "D7:Transf", IFSelect_Optional);

  occ::handle<Interface_TypedValue> labdisp =
    new Interface_TypedValue("Label Display Associativity", Interface_ParamIdent);
  SetValue(13, labdisp, "D8:LabelDisp", IFSelect_Optional);

  occ::handle<Interface_TypedValue> blank =
    new Interface_TypedValue("Blank Status", Interface_ParamInteger);
  blank->SetIntegerLimit(false, 0);
  blank->SetIntegerLimit(true, 1);
  SetValue(14, blank, "D9-1:Blank");
  occ::handle<Interface_TypedValue> subord =
    new Interface_TypedValue("Subordinate Status", Interface_ParamInteger);
  subord->SetIntegerLimit(false, 0);
  subord->SetIntegerLimit(true, 3);
  SetValue(15, subord, "D9-2:Subordinate", IFSelect_EditProtected);
  occ::handle<Interface_TypedValue> useflg =
    new Interface_TypedValue("Use Flag", Interface_ParamInteger);
  useflg->SetIntegerLimit(false, 0);
  useflg->SetIntegerLimit(true, 6);
  SetValue(16, useflg, "D9-3:UseFlag");
  occ::handle<Interface_TypedValue> hier = new Interface_TypedValue("Hierarchy", Interface_ParamInteger);
  hier->SetIntegerLimit(false, 0);
  hier->SetIntegerLimit(true, 2);
  SetValue(17, hier, "D9-4:Hierarchy", IFSelect_EditProtected);

  occ::handle<Interface_TypedValue> lwnum =
    new Interface_TypedValue("Line Weight Number", Interface_ParamInteger);
  lwnum->SetIntegerLimit(false, 0);
  SetValue(18, lwnum, "D12:LineWeight");

  occ::handle<Interface_TypedValue> coltype = NewDefType("Color");
  SetValue(19, coltype, "D13:Color", IFSelect_EditDynamic);
  occ::handle<Interface_TypedValue> colval =
    new Interface_TypedValue("Color Number", Interface_ParamInteger);
  colval->SetIntegerLimit(false, 0);
  SetValue(20, colval, "N13:Color", IFSelect_Optional);
  occ::handle<Interface_TypedValue> colent =
    new Interface_TypedValue("Color Entity", Interface_ParamIdent);
  SetValue(21, colent, "E13:Color", IFSelect_Optional);

  occ::handle<Interface_TypedValue> entlab = new Interface_TypedValue("Entity Label");
  SetValue(22, entlab, "D18:Label");
  occ::handle<Interface_TypedValue> sublab =
    new Interface_TypedValue("Subscript Number", Interface_ParamInteger);
  SetValue(23, sublab, "D19:Subscript", IFSelect_Optional);
}

TCollection_AsciiString IGESSelect_EditDirPart::Label() const
{
  return TCollection_AsciiString("IGES Header");
}

bool IGESSelect_EditDirPart::Recognize(const occ::handle<IFSelect_EditForm>& /*form*/) const
{
  return true;
} // ??

occ::handle<TCollection_HAsciiString> IGESSelect_EditDirPart::StringValue(
  const occ::handle<IFSelect_EditForm>& /*form*/,
  const int num) const
{
  //  Default Values
  return TypedValue(num)->HStringValue();
}

occ::handle<TCollection_HAsciiString> DefTypeName(const IGESData_DefType& deftype)
{
  switch (deftype)
  {
    case IGESData_DefVoid:
      return new TCollection_HAsciiString("Void");
    case IGESData_DefValue:
      return new TCollection_HAsciiString("Value");
    case IGESData_DefReference:
      return new TCollection_HAsciiString("Entity");
    default:
      break;
  }
  return new TCollection_HAsciiString("???");
}

occ::handle<TCollection_HAsciiString> DefListName(const IGESData_DefList& deflist)
{
  switch (deflist)
  {
    case IGESData_DefNone:
      return new TCollection_HAsciiString("None");
    case IGESData_DefOne:
      return new TCollection_HAsciiString("One");
    case IGESData_DefSeveral:
      return new TCollection_HAsciiString("List");
    default:
      break;
  }
  return new TCollection_HAsciiString("???");
}

bool IGESSelect_EditDirPart::Load(const occ::handle<IFSelect_EditForm>&        form,
                                              const occ::handle<Standard_Transient>&       ent,
                                              const occ::handle<Interface_InterfaceModel>& model) const
{
  occ::handle<IGESData_IGESModel> modl = occ::down_cast<IGESData_IGESModel>(model);
  if (modl.IsNull())
    return false;
  occ::handle<IGESData_IGESEntity> iges = occ::down_cast<IGESData_IGESEntity>(ent);
  if (iges.IsNull())
    return false;

  form->LoadValue(1, new TCollection_HAsciiString(iges->TypeNumber()));
  form->LoadValue(2, new TCollection_HAsciiString(iges->FormNumber()));
  if (iges->HasStructure())
    form->LoadValue(3, modl->StringLabel(iges->Structure()));

  form->LoadValue(4, DefTypeName(iges->DefLineFont()));
  form->LoadValue(5, new TCollection_HAsciiString(iges->RankLineFont()));
  if (iges->DefLineFont() == IGESData_DefReference)
    form->LoadValue(6, modl->StringLabel(iges->LineFont()));

  form->LoadValue(7, DefListName(iges->DefLevel()));
  form->LoadValue(8, new TCollection_HAsciiString(iges->Level()));
  if (iges->DefLevel() == IGESData_DefSeveral)
    form->LoadValue(9, modl->StringLabel(iges->LevelList()));

  form->LoadValue(10, DefListName(iges->DefView()));
  if (iges->DefView() != IGESData_DefNone)
    form->LoadValue(11, modl->StringLabel(iges->View()));

  if (iges->HasTransf())
    form->LoadValue(12, modl->StringLabel(iges->Transf()));
  if (iges->HasLabelDisplay())
    form->LoadValue(13, modl->StringLabel(iges->LabelDisplay()));

  form->LoadValue(14, new TCollection_HAsciiString(iges->BlankStatus()));
  form->LoadValue(15, new TCollection_HAsciiString(iges->SubordinateStatus()));
  form->LoadValue(16, new TCollection_HAsciiString(iges->UseFlag()));
  form->LoadValue(17, new TCollection_HAsciiString(iges->HierarchyStatus()));

  form->LoadValue(18, new TCollection_HAsciiString(iges->LineWeightNumber()));

  form->LoadValue(19, DefTypeName(iges->DefColor()));
  form->LoadValue(20, new TCollection_HAsciiString(iges->RankColor()));
  if (iges->DefColor() == IGESData_DefReference)
    form->LoadValue(21, modl->StringLabel(iges->Color()));

  form->LoadValue(22, iges->ShortLabel());
  if (iges->HasSubScriptNumber())
    form->LoadValue(23, new TCollection_HAsciiString(iges->SubScriptNumber()));

  return true;
}

bool IGESSelect_EditDirPart::Update(const occ::handle<IFSelect_EditForm>&        form,
                                                const int                  num,
                                                const occ::handle<TCollection_HAsciiString>& val,
                                                const bool /*enforce*/) const
{
  occ::handle<TCollection_HAsciiString> nulstr;
  occ::handle<IGESData_IGESModel>       modl = occ::down_cast<IGESData_IGESModel>(form->Model());

  //    LineFont
  if (num == 5)
  {
    if (val.IsNull())
      form->Touch(4, DefTypeName(IGESData_DefVoid));
    else
      form->Touch(4, DefTypeName(IGESData_DefValue));
  }
  if (num == 6)
  {
    if (val.IsNull())
      form->Touch(4, DefTypeName(IGESData_DefVoid));
    else
      form->Touch(4, DefTypeName(IGESData_DefReference));
  }

  //    Level
  if (num == 8)
  {
    if (val.IsNull())
      form->Touch(7, DefListName(IGESData_DefNone));
    else
      form->Touch(7, DefListName(IGESData_DefOne));
  }
  if (num == 9)
  {
    if (val.IsNull())
      form->Touch(7, DefListName(IGESData_DefNone));
    else
      form->Touch(7, DefListName(IGESData_DefSeveral));
  }

  //    View
  if (num == 11)
  {
    if (val.IsNull())
      form->Touch(10, DefListName(IGESData_DefNone));
    else
      form->Touch(10, DefListName(IGESData_DefOne));
  }

  //    Color
  if (num == 20)
  {
    if (val.IsNull())
      form->Touch(19, DefTypeName(IGESData_DefVoid));
    else
      form->Touch(19, DefTypeName(IGESData_DefValue));
  }
  if (num == 21)
  {
    if (val.IsNull())
      form->Touch(19, DefTypeName(IGESData_DefVoid));
    else
      form->Touch(19, DefTypeName(IGESData_DefReference));
  }

  return true;
}

bool IGESSelect_EditDirPart::Apply(const occ::handle<IFSelect_EditForm>&        form,
                                               const occ::handle<Standard_Transient>&       ent,
                                               const occ::handle<Interface_InterfaceModel>& model) const
{
  occ::handle<IGESData_IGESModel> modl = occ::down_cast<IGESData_IGESModel>(model);
  if (modl.IsNull())
    return false;
  occ::handle<IGESData_IGESEntity> iges = occ::down_cast<IGESData_IGESEntity>(ent);
  if (iges.IsNull())
    return false;
  occ::handle<IGESData_IGESEntity>         sub;
  occ::handle<IGESData_LineFontEntity>     lfent;
  occ::handle<IGESData_LevelListEntity>    levlist;
  occ::handle<IGESData_ViewKindEntity>     view;
  occ::handle<IGESData_TransfEntity>       transf;
  occ::handle<IGESData_LabelDisplayEntity> labdisp;
  occ::handle<IGESData_ColorEntity>        color;

  occ::handle<TCollection_HAsciiString> str;
  int                 num;

  if (form->IsModified(3))
  {
    str = form->EditedValue(3);
    sub.Nullify();
    num = 0;
    if (!str.IsNull())
      num = modl->NextNumberForLabel(str->ToCString());
    else
      num = -1;
    if (num > 0)
      sub = modl->Entity(num);
    if (num != 0)
      iges->InitDirFieldEntity(3, sub);
  }

  if (form->IsModified(5))
  {
    str = form->EditedValue(5);
    lfent.Nullify();
    if (str.IsNull())
      num = 0;
    else
      num = str->IntegerValue();
    iges->InitLineFont(lfent, num);
  }
  if (form->IsModified(6))
  {
    str = form->EditedValue(6);
    lfent.Nullify();
    num = 0;
    if (str.IsNull())
      num = -1;
    else
      num = modl->NextNumberForLabel(str->ToCString());
    if (num > 0)
      lfent = GetCasted(IGESData_LineFontEntity, modl->Entity(num));
    if (num < 0 || !lfent.IsNull())
      iges->InitLineFont(lfent, 0);
  }

  if (form->IsModified(8))
  {
    str = form->EditedValue(8);
    levlist.Nullify();
    if (str.IsNull())
      num = 0;
    else
      num = str->IntegerValue();
    iges->InitLevel(levlist, num);
  }
  if (form->IsModified(9))
  {
    str = form->EditedValue(9);
    levlist.Nullify();
    num = 0;
    if (str.IsNull())
      num = -1;
    else
      num = modl->NextNumberForLabel(str->ToCString());
    if (num > 0)
      levlist = GetCasted(IGESData_LevelListEntity, modl->Entity(num));
    if (num < 0 || !levlist.IsNull())
      iges->InitLevel(levlist, 0);
  }

  if (form->IsModified(11))
  {
    str = form->EditedValue(11);
    view.Nullify();
    num = 0;
    if (!str.IsNull())
      num = modl->NextNumberForLabel(str->ToCString());
    else
      num = -1;
    if (num > 0)
      view = GetCasted(IGESData_ViewKindEntity, modl->Entity(num));
    if (num != 0 || !view.IsNull())
      iges->InitView(view);
  }

  if (form->IsModified(12))
  {
    str = form->EditedValue(12);
    transf.Nullify();
    num = 0;
    if (!str.IsNull())
      num = modl->NextNumberForLabel(str->ToCString());
    else
      num = -1;
    if (num > 0)
      transf = GetCasted(IGESData_TransfEntity, modl->Entity(num));
    if (num != 0 || !transf.IsNull())
      iges->InitTransf(transf);
  }

  if (form->IsModified(13))
  {
    str = form->EditedValue(13);
    labdisp.Nullify();
    num = 0;
    if (!str.IsNull())
      num = modl->NextNumberForLabel(str->ToCString());
    else
      num = -1;
    if (num > 0)
      labdisp = GetCasted(IGESData_LabelDisplayEntity, modl->Entity(num));
    if (num != 0 || !labdisp.IsNull())
      iges->InitDirFieldEntity(8, labdisp);
  }

  if (form->IsModified(14) || form->IsModified(15) || form->IsModified(16) || form->IsModified(17))
  {
    int n1, n2, n3, n4;
    n1 = iges->BlankStatus();
    n2 = iges->SubordinateStatus();
    n3 = iges->UseFlag();
    n4 = iges->HierarchyStatus();
    if (form->IsModified(14))
      n1 = form->EditedValue(14)->IntegerValue();
    if (form->IsModified(15))
      n2 = form->EditedValue(15)->IntegerValue();
    if (form->IsModified(16))
      n3 = form->EditedValue(16)->IntegerValue();
    if (form->IsModified(17))
      n4 = form->EditedValue(17)->IntegerValue();
    iges->InitStatus(n1, n2, n3, n4);
  }

  if (form->IsModified(18))
  {
    str = form->EditedValue(18);
    num = 0;
    if (!str.IsNull())
      num = str->IntegerValue();
    iges->InitMisc(iges->Structure(), iges->LabelDisplay(), num);
  }

  if (form->IsModified(20))
  {
    str = form->EditedValue(20);
    color.Nullify();
    if (str.IsNull())
      num = 0;
    else
      num = str->IntegerValue();
    iges->InitColor(color, num);
  }
  if (form->IsModified(21))
  {
    str = form->EditedValue(21);
    color.Nullify();
    num = 0;
    if (str.IsNull())
      num = -1;
    else
      num = modl->NextNumberForLabel(str->ToCString());
    if (num > 0)
      color = GetCasted(IGESData_ColorEntity, modl->Entity(num));
    if (num < 0 || !color.IsNull())
      iges->InitColor(color, 0);
  }

  if (form->IsModified(22) || form->IsModified(23))
  {
    num = -1;
    if (iges->HasSubScriptNumber())
      num = iges->SubScriptNumber();
    if (form->IsModified(23))
    {
      str = form->EditedValue(23);
      if (str.IsNull())
        num = -1;
      else
        num = str->IntegerValue();
    }
    str = iges->ShortLabel();
    if (form->IsModified(22))
      str = form->EditedValue(22);
    iges->SetLabel(str, num);
  }

  return true;
}
