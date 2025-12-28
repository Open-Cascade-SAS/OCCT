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

#include <Interface_EntityIterator.hxx>
#include <Interface_IntVal.hxx>
#include <Standard_NoMoreObject.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_Transient.hxx>

//  Iterator for for writing, or while with Next at the end :
//  for (create iterator; iter.More(); iter.Next()) { val = iter.Value(); ... }
// .... Initial definitions : in particular those required for
//      graph tools (construction with graph, with a vertex)
Interface_EntityIterator::Interface_EntityIterator()
{
  //  thecurr = new Interface_IntVal;
  //  thecurr->CValue() = 0;
  //  thelist = new NCollection_HSequence<occ::handle<Standard_Transient>>();  // empty constructor
  //  thelist sera construit au premier Add (quelquefois, il nyena pas)
}

Interface_EntityIterator::Interface_EntityIterator(const occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>& list)
{
  thecurr           = new Interface_IntVal;
  thecurr->CValue() = 0;
  thelist           = list;
}

void Interface_EntityIterator::AddList(const occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>& list)
{
  if (thelist.IsNull())
    thelist = new NCollection_HSequence<occ::handle<Standard_Transient>>();
  if (thecurr.IsNull())
    thecurr = new Interface_IntVal;
  thecurr->CValue() = 0;
  thelist->Append(list);
}

void Interface_EntityIterator::AddItem(const occ::handle<Standard_Transient>& anentity)
{
  if (anentity.IsNull())
    return;
  if (thecurr.IsNull())
    thecurr = new Interface_IntVal;
  if (thelist.IsNull())
    thelist = new NCollection_HSequence<occ::handle<Standard_Transient>>();
  thecurr->CValue() = 0;
  thelist->Append(anentity);
}

void Interface_EntityIterator::GetOneItem(const occ::handle<Standard_Transient>& anentity)
{
  AddItem(anentity);
}

void Interface_EntityIterator::Reset()
{
  if (thecurr.IsNull())
    thecurr = new Interface_IntVal;
  thecurr->CValue() = 0;
  thelist           = new NCollection_HSequence<occ::handle<Standard_Transient>>();
}

// .... Pre-iteration sorting functionalities ....

//  "Dumb" way : remove terms that don't fit : slow !
//  Better to make another sequence alongside

void Interface_EntityIterator::SelectType(const occ::handle<Standard_Type>& atype,
                                          const bool       keep)
{
  if (thelist.IsNull())
    return;
  int                     i, n = thelist->Length();
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> nlist = new NCollection_HSequence<occ::handle<Standard_Transient>>();
  for (i = 1; i <= n; i++)
  {
    if (thelist->Value(i)->IsKind(atype) == keep)
      nlist->Append(thelist->Value(i));
  }
  thelist = nlist;
}

//  ....  Iteration proprement dite  ....

int Interface_EntityIterator::NbEntities() const
{
  if (thelist.IsNull())
    return 0;
  if (!thecurr.IsNull() && thecurr->Value() == 0)
    Start();
  return thelist->Length();
}

int Interface_EntityIterator::NbTyped(const occ::handle<Standard_Type>& atype) const
{
  int res = 0;
  if (thelist.IsNull())
    return res;
  int i, n = thelist->Length();
  for (i = 1; i <= n; i++)
  {
    if (thelist->Value(i)->IsKind(atype))
      res++;
  }
  return res;
}

Interface_EntityIterator Interface_EntityIterator::Typed(const occ::handle<Standard_Type>& atype) const
{
  Interface_EntityIterator res;
  if (thelist.IsNull())
    return res;
  int i, n = thelist->Length();
  for (i = 1; i <= n; i++)
  {
    if (thelist->Value(i)->IsKind(atype))
      res.AddItem(thelist->Value(i));
  }
  return res;
}

void Interface_EntityIterator::Start() const
{
  if (!thecurr.IsNull())
    thecurr->CValue() = 1;
} // can be redefined ...

bool Interface_EntityIterator::More() const
{
  if (thecurr.IsNull())
    return false;
  if (thecurr->Value() == 0)
    Start(); // iteration preparation
  if (thelist.IsNull())
    return false;
  return (thecurr->Value() <= thelist->Length());
}

void Interface_EntityIterator::Next() const
{
  thecurr->CValue()++;
} // Next ne verifie rien : soin laisse a Value

const occ::handle<Standard_Transient>& Interface_EntityIterator::Value() const
{
  //  NbEntity not const (we don't know how it is implemented after all)
  if (thelist.IsNull())
    throw Standard_NoSuchObject("Interface_EntityIterator");
  if (thecurr->Value() < 1 || thecurr->Value() > thelist->Length())
    throw Standard_NoSuchObject("Interface_EntityIterator");
  return thelist->Value(thecurr->Value());
}

occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> Interface_EntityIterator::Content() const
{
  if (!thecurr.IsNull() && thecurr->Value() == 0)
    Start();
  if (thelist.IsNull())
    return new NCollection_HSequence<occ::handle<Standard_Transient>>(); // empty
  return thelist;
}

void Interface_EntityIterator::Destroy()
{
  thecurr.Nullify();
} // becomes empty again !

Interface_EntityIterator::~Interface_EntityIterator()
{
  Destroy();
}
