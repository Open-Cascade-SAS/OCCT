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

#include <IFSelect_SelectPointed.hxx>
#include <IFSelect_Transformer.hxx>
#include <Interface_CopyControl.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <NCollection_Map.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_SelectPointed, IFSelect_SelectBase)

IFSelect_SelectPointed::IFSelect_SelectPointed()
    : theset(false)
{
}

void IFSelect_SelectPointed::Clear()
{
  theitems.Clear();
  theset = false;
}

bool IFSelect_SelectPointed::IsSet() const
{
  return theset;
}

void IFSelect_SelectPointed::SetEntity(const occ::handle<Standard_Transient>& ent)
{
  theitems.Clear();
  theset = true;
  if (ent.IsNull())
    return;
  theitems.Append(ent);
}

void IFSelect_SelectPointed::SetList(
  const occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>& list)
{
  theitems.Clear();
  theset = true;
  if (list.IsNull())
    return;
  int i, nb = list->Length();
  for (i = 1; i <= nb; i++)
    theitems.Append(list->Value(i));
}

//  ....    Editions

bool IFSelect_SelectPointed::Add(const occ::handle<Standard_Transient>& item)
{
  if (item.IsNull())
    return false;
  for (int i = theitems.Length(); i >= 1; i--)
    if (item == theitems.Value(i))
      return false;
  theitems.Append(item);
  theset = true;
  return true;
}

bool IFSelect_SelectPointed::Remove(const occ::handle<Standard_Transient>& item)
{
  if (item.IsNull())
    return false;
  for (int i = theitems.Length(); i >= 1; i--)
    if (item == theitems.Value(i))
    {
      theitems.Remove(i);
      return true;
    }
  return true;
}

bool IFSelect_SelectPointed::Toggle(const occ::handle<Standard_Transient>& item)
{
  if (item.IsNull())
    return false;
  int num = 0;
  for (int i = theitems.Length(); i >= 1; i--)
    if (item == theitems.Value(i))
      num = i;
  if (num == 0)
    theitems.Append(item);
  else
    theitems.Remove(num);
  return (num == 0);
}

bool IFSelect_SelectPointed::AddList(
  const occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>& list)
{
  //   Optimized with a Map
  bool res = false;
  if (list.IsNull())
    return res;
  int                                              i, nb = theitems.Length(), nl = list->Length();
  NCollection_Map<occ::handle<Standard_Transient>> deja(nb + nl + 1);
  for (i = 1; i <= nb; i++)
    deja.Add(theitems.Value(i));

  for (i = 1; i <= nl; i++)
  {
    if (!deja.Contains(list->Value(i)))
      theitems.Append(list->Value(i));
  }
  theset = true;
  return res;
}

bool IFSelect_SelectPointed::RemoveList(
  const occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>& list)
{
  bool res = false;
  if (list.IsNull())
    return res;
  int i, nb = list->Length();
  for (i = 1; i <= nb; i++)
    res |= Remove(list->Value(i));
  return res;
}

bool IFSelect_SelectPointed::ToggleList(
  const occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>& list)
{
  bool res = true;
  if (list.IsNull())
    return res;
  int i, nb = list->Length();
  for (i = 1; i <= nb; i++)
    res |= Toggle(list->Value(i));
  return res;
}

//  ....   Consultations

int IFSelect_SelectPointed::Rank(const occ::handle<Standard_Transient>& item) const
{
  if (item.IsNull())
    return 0;
  for (int i = theitems.Length(); i >= 1; i--)
    if (item == theitems.Value(i))
      return i;
  return 0;
}

int IFSelect_SelectPointed::NbItems() const
{
  return theitems.Length();
}

occ::handle<Standard_Transient> IFSelect_SelectPointed::Item(const int num) const
{
  occ::handle<Standard_Transient> item;
  if (num <= 0 || num > theitems.Length())
    return item;
  return theitems.Value(num);
}

void IFSelect_SelectPointed::Update(const occ::handle<Interface_CopyControl>& control)
{
  int nb = theitems.Length();
  for (int i = nb; i > 0; i--)
  {
    occ::handle<Standard_Transient> enfr, ento;
    enfr = theitems.Value(i);
    if (!control->Search(enfr, ento))
      theitems.Remove(i);
    else
      theitems.SetValue(i, ento);
  }
}

void IFSelect_SelectPointed::Update(const occ::handle<IFSelect_Transformer>& trf)
{
  int nb = theitems.Length();
  for (int i = nb; i > 0; i--)
  {
    occ::handle<Standard_Transient> enfr, ento;
    enfr = theitems.Value(i);
    if (!trf->Updated(enfr, ento))
      theitems.Remove(i);
    else
      theitems.SetValue(i, ento);
  }
}

//  ....  Actions Generales

Interface_EntityIterator IFSelect_SelectPointed::RootResult(const Interface_Graph& G) const
{
  Interface_EntityIterator result;
  int                      nb = theitems.Length();
  for (int i = 1; i <= nb; i++)
  {
    occ::handle<Standard_Transient> item = theitems.Value(i);
    if (G.EntityNumber(item) > 0)
      result.GetOneItem(item);
  }
  return result;
}

TCollection_AsciiString IFSelect_SelectPointed::Label() const
{
  return TCollection_AsciiString("Pointed Entities");
}
