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
#include <IFSelect_ParamEditor.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Static.hxx>
#include <Interface_TypedValue.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_ParamEditor, IFSelect_Editor)

IFSelect_ParamEditor::IFSelect_ParamEditor(const int nbmax,
                                           const char* label)
    : IFSelect_Editor(nbmax),
      thelabel(label)
{
  SetNbValues(0);
  if (thelabel.Length() == 0)
    thelabel.AssignCat("Param Editor");
}

void IFSelect_ParamEditor::AddValue(const occ::handle<Interface_TypedValue>& val,
                                    const char*              shortname)
{
  SetNbValues(NbValues() + 1);
  SetValue(NbValues(), val, shortname);
}

void IFSelect_ParamEditor::AddConstantText(const char* val,
                                           const char* shortname,
                                           const char* longname)
{
  occ::handle<Interface_TypedValue> tv =
    new Interface_TypedValue(longname[0] == '\0' ? shortname : longname);
  tv->SetCStringValue(val);
  SetNbValues(NbValues() + 1);
  SetValue(NbValues(), tv, shortname, IFSelect_EditRead);
}

TCollection_AsciiString IFSelect_ParamEditor::Label() const
{
  return thelabel;
}

bool IFSelect_ParamEditor::Recognize(const occ::handle<IFSelect_EditForm>& /*form*/) const
{
  return true;
} // no constraint

occ::handle<TCollection_HAsciiString> IFSelect_ParamEditor::StringValue(
  const occ::handle<IFSelect_EditForm>& /*form*/,
  const int num) const
{
  return TypedValue(num)->HStringValue();
}

bool IFSelect_ParamEditor::Load(const occ::handle<IFSelect_EditForm>& form,
                                            const occ::handle<Standard_Transient>& /*ent*/,
                                            const occ::handle<Interface_InterfaceModel>& /*model*/) const
{
  int i, nb = NbValues();
  for (i = 1; i <= nb; i++)
    form->LoadValue(i, TypedValue(i)->HStringValue());

  return true;
}

bool IFSelect_ParamEditor::Apply(
  const occ::handle<IFSelect_EditForm>& form,
  const occ::handle<Standard_Transient>& /*ent*/,
  const occ::handle<Interface_InterfaceModel>& /*model*/) const
{
  int i, nb = NbValues();
  for (i = 1; i <= nb; i++)
    if (form->IsModified(i))
      TypedValue(i)->SetHStringValue(form->EditedValue(i));

  return true;
}

occ::handle<IFSelect_ParamEditor> IFSelect_ParamEditor::StaticEditor(
  const occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>>& list,
  const char*                         label)
{
  occ::handle<IFSelect_ParamEditor> editor;
  if (list.IsNull())
    return editor;
  int i, nb = list->Length();
  //  if (nb == 0) return editor;
  editor = new IFSelect_ParamEditor(nb + 10, label);
  for (i = 1; i <= nb; i++)
  {
    occ::handle<Interface_Static> val = Interface_Static::Static(list->Value(i)->ToCString());
    if (!val.IsNull())
      editor->AddValue(val);
  }
  return editor;
}
