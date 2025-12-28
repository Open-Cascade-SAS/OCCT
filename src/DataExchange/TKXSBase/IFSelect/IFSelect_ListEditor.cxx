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

#include <IFSelect_ListEditor.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_TypedValue.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_ListEditor, Standard_Transient)

IFSelect_ListEditor::IFSelect_ListEditor()
    : themax(0),
      thetouc(0)
{
}

IFSelect_ListEditor::IFSelect_ListEditor(const occ::handle<Interface_TypedValue>& def,
                                         const int              max)
    : themax(max),
      thedef(def),
      thetouc(0)
{
}

void IFSelect_ListEditor::LoadModel(const occ::handle<Interface_InterfaceModel>& model)
{
  themodl = model;
}

void IFSelect_ListEditor::LoadValues(const occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>>& vals)
{
  theorig = vals;
  ClearEdit();
}

void IFSelect_ListEditor::SetTouched()
{
  thetouc = 1;
}

void IFSelect_ListEditor::ClearEdit()
{
  theedit = new NCollection_HSequence<occ::handle<TCollection_HAsciiString>>();
  thestat = new NCollection_HSequence<int>();
  if (theorig.IsNull())
    return;
  int i, nb = theorig->Length();
  for (i = 1; i <= nb; i++)
  {
    theedit->Append(theorig->Value(i));
    thestat->Append(0);
  }
  thetouc = 0;
}

//  ########    CHECK    ########

static bool CheckValue(const occ::handle<TCollection_HAsciiString>& val,
                                   const occ::handle<Interface_InterfaceModel>& modl,
                                   const occ::handle<Interface_TypedValue>&     thedef)
{
  if (val.IsNull() || modl.IsNull() || thedef.IsNull())
    return true;

  Interface_ParamType pty = thedef->Type();
  if (!thedef->Satisfies(val))
    return false;
  if (pty == Interface_ParamIdent && !val.IsNull())
  {
    if (modl->NextNumberForLabel(val->ToCString(), 0) <= 0)
      return false;
  }
  return true;
}

//  ########    EDITION    ########

bool IFSelect_ListEditor::LoadEdited(
  const occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>>& list)
{
  if (list.IsNull())
    return false;
  int i, nb = list->Length();
  if (nb > themax)
    return false;

  //   check values
  if (!thedef.IsNull())
  {
    for (i = 1; i <= nb; i++)
    {
      occ::handle<TCollection_HAsciiString> newval = list->Value(i);
      if (!CheckValue(newval, themodl, thedef))
        return false;
    }
  }

  //  OK
  theedit = list;
  thestat = new NCollection_HSequence<int>();
  for (i = 1; i <= nb; i++)
    thestat->Append(1);
  thetouc = 1;

  return true;
}

bool IFSelect_ListEditor::SetValue(const int                  num,
                                               const occ::handle<TCollection_HAsciiString>& val)
{
  if (theedit.IsNull())
    return false;
  if (num < 1 || num > theedit->Length())
    return false;

  //   check value
  if (!CheckValue(val, themodl, thedef))
    return false;

  // OK
  theedit->SetValue(num, val);
  thestat->SetValue(num, 1);
  thetouc = 1;
  return true;
}

bool IFSelect_ListEditor::AddValue(const occ::handle<TCollection_HAsciiString>& val,
                                               const int                  atnum)
{
  if (theedit.IsNull())
    return false;
  if (themax > 0 && theedit->Length() >= themax)
    return false;
  if (!CheckValue(val, themodl, thedef))
    return false;
  if (atnum > 0)
  {
    theedit->InsertBefore(atnum, val);
    thestat->InsertBefore(atnum, 2);
  }
  else
  {
    theedit->Append(val);
    thestat->Append(2);
  }
  thetouc = 2;
  return true;
}

bool IFSelect_ListEditor::Remove(const int num,
                                             const int howmany)
{
  if (theedit.IsNull())
    return false;
  int nb = theedit->Length();
  if (num < 0)
    return false;
  if (num == 0)
    return Remove(nb - howmany, howmany);

  if ((num + howmany) > nb)
    return false;
  theedit->Remove(num, howmany);
  thestat->Remove(num, howmany);
  thetouc = 3;
  return true;
}

//  ########    QUERIES    ########

occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> IFSelect_ListEditor::OriginalValues() const
{
  return theorig;
}

occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> IFSelect_ListEditor::EditedValues() const
{
  return theedit;
}

int IFSelect_ListEditor::NbValues(const bool edited) const
{
  if (edited)
    return (theedit.IsNull() ? 0 : theedit->Length());
  return (theorig.IsNull() ? 0 : theorig->Length());
}

occ::handle<TCollection_HAsciiString> IFSelect_ListEditor::Value(const int num,
                                                            const bool edited) const
{
  occ::handle<TCollection_HAsciiString> val;
  if (edited)
  {
    if (theedit.IsNull())
      return val;
    if (num < 1 || num > theedit->Length())
      return val;
    val = theedit->Value(num);
  }
  else
  {
    if (theorig.IsNull())
      return val;
    if (num < 1 || num > theorig->Length())
      return val;
    val = theorig->Value(num);
  }
  return val;
}

bool IFSelect_ListEditor::IsChanged(const int num) const
{
  if (thestat.IsNull())
    return false;
  if (num < 1 || num > thestat->Length())
    return false;
  int stat = thestat->Value(num);
  return (stat != 0);
}

bool IFSelect_ListEditor::IsModified(const int num) const
{
  if (thestat.IsNull())
    return false;
  if (num < 1 || num > thestat->Length())
    return false;
  int stat = thestat->Value(num);
  return (stat == 1);
}

bool IFSelect_ListEditor::IsAdded(const int num) const
{
  if (thestat.IsNull())
    return false;
  if (num < 1 || num > thestat->Length())
    return false;
  int stat = thestat->Value(num);
  return (stat == 2);
}

bool IFSelect_ListEditor::IsTouched() const
{
  return (thetouc != 0);
}
