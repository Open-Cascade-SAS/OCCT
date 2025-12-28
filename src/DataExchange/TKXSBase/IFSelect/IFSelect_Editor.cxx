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
#include <IFSelect_Editor.hxx>
#include <IFSelect_ListEditor.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_MSG.hxx>
#include <Interface_TypedValue.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>

#include <string.h>
IMPLEMENT_STANDARD_RTTIEXT(IFSelect_Editor, Standard_Transient)

IFSelect_Editor::IFSelect_Editor(const int nbval)
    : thenbval(nbval),
      themaxsh(0),
      themaxco(0),
      themaxla(0),
      thevalues(1, nbval),
      theshorts(1, nbval),
      themodes(1, nbval),
      thelists(1, nbval)
{
  thelists.Init(-1);
}

void IFSelect_Editor::SetNbValues(const int nbval)
{
  if (nbval > thevalues.Upper())
    throw Standard_OutOfRange("IFSelect_Editor:SetNbValues");
  thenbval = nbval;
}

void IFSelect_Editor::SetValue(const int                                num,
                               const occ::handle<Interface_TypedValue>& typval,
                               const char*                              shortname,
                               const IFSelect_EditValue                 editmode)
{
  if (num < 1 || num > thenbval)
    return;
  TCollection_AsciiString shn(shortname);
  int                     lng = shn.Length();
  if (lng > 0)
    thenames.Bind(shortname, num);
  if (lng > themaxsh)
    themaxsh = lng;
  lng = (int)strlen(typval->Name());
  if (lng > themaxco)
    themaxco = lng;
  lng = (int)strlen(typval->Label());
  if (lng > themaxla)
    themaxla = lng;

  thenames.Bind(typval->Name(), num);
  int edm = (int)editmode;
  thevalues.SetValue(num, typval);
  theshorts.SetValue(num, shn);
  themodes.SetValue(num, edm);
}

void IFSelect_Editor::SetList(const int num, const int max)
{
  if (num < 1 || num > thenbval)
    return;
  thelists.SetValue(num, max);
}

int IFSelect_Editor::NbValues() const
{
  return thenbval;
}

occ::handle<Interface_TypedValue> IFSelect_Editor::TypedValue(const int num) const
{
  return occ::down_cast<Interface_TypedValue>(thevalues.Value(num));
}

bool IFSelect_Editor::IsList(const int num) const
{
  if (num < 1 || num > thenbval)
    return false;
  return (thelists.Value(num) >= 0);
}

int IFSelect_Editor::MaxList(const int num) const
{
  if (num < 1 || num > thenbval)
    return -1;
  return thelists.Value(num);
}

const char* IFSelect_Editor::Name(const int num, const bool isshort) const
{
  if (num < 1 || num > thenbval)
    return "";
  if (isshort)
    return theshorts.Value(num).ToCString();
  else
    return TypedValue(num)->Name();
}

IFSelect_EditValue IFSelect_Editor::EditMode(const int num) const
{
  if (num < 1 || num > thenbval)
    return IFSelect_EditDynamic;
  int edm = themodes.Value(num);
  return (IFSelect_EditValue)edm;
}

void IFSelect_Editor::PrintNames(Standard_OStream& S) const
{
  int i, nb = NbValues();
  S << "****    Editor : " << Label() << std::endl;
  S << "****    Nb Values = " << nb << "    ****    Names / Labels" << std::endl;
  S << " Num ";
  if (themaxsh > 0)
    S << "Short" << Interface_MSG::Blanks("Short", themaxsh) << " ";
  S << "Complete" << Interface_MSG::Blanks("Complete", themaxco) << "  Label" << std::endl;

  for (i = 1; i <= nb; i++)
  {
    occ::handle<Interface_TypedValue> tv = TypedValue(i);
    if (tv.IsNull())
      continue;
    S << Interface_MSG::Blanks(i, 3) << i << " ";
    if (themaxsh > 0)
    {
      const TCollection_AsciiString& sho = theshorts(i);
      S << sho << Interface_MSG::Blanks(sho.ToCString(), themaxsh) << " ";
    }
    S << tv->Name() << Interface_MSG::Blanks(tv->Name(), themaxco) << "  " << tv->Label()
      << std::endl;
  }
}

void IFSelect_Editor::PrintDefs(Standard_OStream& S, const bool labels) const
{
  int i, nb = NbValues();
  S << "****    Editor : " << Label() << std::endl;
  S << "****    Nb Values = " << nb << "    ****    " << (labels ? "Labels" : "Names")
    << "  /  Definitions" << std::endl;
  S << " Num ";
  if (labels)
    S << "Label" << Interface_MSG::Blanks("Label", themaxla);
  else
  {
    if (themaxsh > 0)
      S << "Short" << Interface_MSG::Blanks("Short", themaxsh + 1);
    S << "Complete" << Interface_MSG::Blanks("Complete", themaxco);
  }
  S << "  Edit Mode  &  Definition" << std::endl;

  for (i = 1; i <= nb; i++)
  {
    occ::handle<Interface_TypedValue> tv = TypedValue(i);
    if (tv.IsNull())
      continue;
    S << " " << Interface_MSG::Blanks(i, 3) << i << " ";
    if (labels)
      S << tv->Label() << Interface_MSG::Blanks(tv->Label(), themaxla);
    else
    {
      if (themaxsh > 0)
      {
        const TCollection_AsciiString& sho = theshorts(i);
        S << sho << Interface_MSG::Blanks(sho.ToCString(), themaxsh) << " ";
      }
      S << tv->Name() << Interface_MSG::Blanks(tv->Name(), themaxco);
    }

    S << " ";
    int maxls = MaxList(i);
    if (maxls == 0)
      S << " (List) ";
    else if (maxls > 0)
      S << " (List <= " << maxls << " Items) ";
    else
      S << " ";
    IFSelect_EditValue edm = EditMode(i);
    switch (edm)
    {
      case IFSelect_Optional:
        S << "Optional ";
        break;
      case IFSelect_Editable:
        S << "Editable ";
        break;
      case IFSelect_EditProtected:
        S << "Protected";
        break;
      case IFSelect_EditComputed:
        S << "Computed ";
        break;
      case IFSelect_EditRead:
        S << "ReadOnly ";
        break;
      case IFSelect_EditDynamic:
        S << "Dynamic  ";
        break;
      default:
        S << "?????????";
        break;
    }

    S << " " << tv->Definition() << std::endl;
  }
}

int IFSelect_Editor::MaxNameLength(const int what) const
{
  if (what == -1)
    return themaxsh;
  if (what == 0)
    return themaxco;
  if (what == 1)
    return themaxla;
  return 0;
}

int IFSelect_Editor::NameNumber(const char* name) const
{
  int res;
  if (thenames.Find(name, res))
    return res;
  res = atoi(name); // if it's an integer, we try it
  if (res < 1 || res > NbValues())
    res = 0;
  return res;
}

occ::handle<IFSelect_EditForm> IFSelect_Editor::Form(const bool readonly, const bool undoable) const
{
  return new IFSelect_EditForm(this, readonly, undoable, Label().ToCString());
}

occ::handle<IFSelect_ListEditor> IFSelect_Editor::ListEditor(const int num) const
{
  occ::handle<IFSelect_ListEditor> led;
  int                              max = MaxList(num);
  if (max < 0)
    return led;
  led = new IFSelect_ListEditor(TypedValue(num), max);
  return led;
}

occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> IFSelect_Editor::
  ListValue(const occ::handle<IFSelect_EditForm>& /*form*/, const int /*num*/) const
{
  occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> list;
  return list;
}

bool IFSelect_Editor::Update(const occ::handle<IFSelect_EditForm>& /*form*/,
                             const int /*num*/,
                             const occ::handle<TCollection_HAsciiString>& /*newval*/,
                             const bool /*enforce*/) const
{
  return true;
}

bool IFSelect_Editor::UpdateList(
  const occ::handle<IFSelect_EditForm>& /*form*/,
  const int /*num*/,
  const occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>>& /*newval*/,
  const bool /*enforce*/) const
{
  return true;
}
