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

#include <Interface_Check.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <Transfer_ResultFromTransient.hxx>
#include <Transfer_TransientProcess.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Transfer_ResultFromTransient, Standard_Transient)

static occ::handle<Interface_Check> voidcheck = new Interface_Check;

Transfer_ResultFromTransient::Transfer_ResultFromTransient() {}

void Transfer_ResultFromTransient::SetStart(const occ::handle<Standard_Transient>& start)
{
  thestart = start;
}

void Transfer_ResultFromTransient::SetBinder(const occ::handle<Transfer_Binder>& binder)
{
  thebinder = binder;
}

occ::handle<Standard_Transient> Transfer_ResultFromTransient::Start() const
{
  return thestart;
}

occ::handle<Transfer_Binder> Transfer_ResultFromTransient::Binder() const
{
  return thebinder;
}

bool Transfer_ResultFromTransient::HasResult() const
{
  return (thebinder.IsNull() ? false : thebinder->HasResult());
}

const occ::handle<Interface_Check> Transfer_ResultFromTransient::Check() const
{
  if (thebinder.IsNull())
    return voidcheck;
  return thebinder->Check();
}

Interface_CheckStatus Transfer_ResultFromTransient::CheckStatus() const
{
  if (thebinder.IsNull())
    return Interface_CheckOK;
  const occ::handle<Interface_Check> ach = thebinder->Check();
  return ach->Status();
}

occ::handle<Transfer_ResultFromTransient> Transfer_ResultFromTransient::ResultFromKey(
  const occ::handle<Standard_Transient>& key) const
{
  occ::handle<Transfer_ResultFromTransient> res;
  if (key == thestart)
    return this;
  int i, nb = NbSubResults();
  for (i = 1; i <= nb; i++)
  {
    res = SubResult(i)->ResultFromKey(key);
    if (!res.IsNull())
      return res;
  }
  return res;
}

void Transfer_ResultFromTransient::FillMap(NCollection_IndexedMap<occ::handle<Standard_Transient>>& map) const
{
  if (thesubs.IsNull())
    return;
  int i, nb = thesubs->Length();
  for (i = 1; i <= nb; i++)
    map.Add(thesubs->Value(i));
  for (i = 1; i <= nb; i++)
    SubResult(i)->FillMap(map);
}

//  #####   SUBS   #####

void Transfer_ResultFromTransient::ClearSubs()
{
  thesubs.Nullify();
}

void Transfer_ResultFromTransient::AddSubResult(const occ::handle<Transfer_ResultFromTransient>& sub)
{
  if (sub.IsNull())
    return;
  if (thesubs.IsNull())
    thesubs = new NCollection_HSequence<occ::handle<Standard_Transient>>();
  thesubs->Append(sub);
}

int Transfer_ResultFromTransient::NbSubResults() const
{
  return (thesubs.IsNull() ? 0 : thesubs->Length());
}

occ::handle<Transfer_ResultFromTransient> Transfer_ResultFromTransient::SubResult(
  const int num) const
{
  occ::handle<Transfer_ResultFromTransient> sub;
  if (thesubs.IsNull())
    return sub;
  if (num < 1 || num > thesubs->Length())
    return sub;
  return occ::down_cast<Transfer_ResultFromTransient>(thesubs->Value(num));
}

void Transfer_ResultFromTransient::Fill(const occ::handle<Transfer_TransientProcess>& /*TP*/)
{
  // abv: WARNING: to be removed (scopes)
  return;
}

void Transfer_ResultFromTransient::Strip()
{
  // abv: WARNING: to be removed (scopes)
}

void Transfer_ResultFromTransient::FillBack(const occ::handle<Transfer_TransientProcess>& /*TP*/) const
{
  // abv: WARNING: to be removed (scopes)
}
