// Created on: 1992-02-03
// Created by: Christian CAILLET
// Copyright (c) 1992-1999 Matra Datavision
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

#include <Transfer_IteratorOfProcessForFinder.hxx>

#include <Standard_NoSuchObject.hxx>
#include <Transfer_Finder.hxx>
#include <Transfer_FindHasher.hxx>
#include <Transfer_ProcessForFinder.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_ActorOfProcessForFinder.hxx>

//=================================================================================================

Transfer_IteratorOfProcessForFinder::Transfer_IteratorOfProcessForFinder(const bool withstarts)
    : Transfer_TransferIterator()
{
  if (withstarts)
    thestarts = new NCollection_HSequence<occ::handle<Transfer_Finder>>();
}

//=================================================================================================

void Transfer_IteratorOfProcessForFinder::Add(const occ::handle<Transfer_Binder>& binder)
{
  if (!thestarts.IsNull())
    throw Standard_NoSuchObject(
      "Transfer_IteratorOfProcessForFinder : Add, Starting Object required not provided");
  AddItem(binder);
}

//=================================================================================================

void Transfer_IteratorOfProcessForFinder::Add(const occ::handle<Transfer_Binder>& binder,
                                              const occ::handle<Transfer_Finder>& start)
{
  AddItem(binder);
  if (!thestarts.IsNull())
    thestarts->Append(start);
}

//=================================================================================================

void Transfer_IteratorOfProcessForFinder::Filter(
  const occ::handle<NCollection_HSequence<occ::handle<Transfer_Finder>>>& list,
  const bool                                                              keep)
{
  if (list.IsNull() || thestarts.IsNull())
    return;
  int i, j, nb = thestarts->Length();
  if (nb == 0)
    return;
  occ::handle<Transfer_Binder> factice;
  NCollection_IndexedDataMap<occ::handle<Transfer_Finder>,
                             occ::handle<Transfer_Binder>,
                             Transfer_FindHasher>
    amap(nb);
  for (i = 1; i <= nb; i++)
  {
    j = amap.Add(thestarts->Value(i), factice);
    SelectItem(j, !keep);
  }

  //  Comparison
  nb = list->Length();
  for (i = 1; i <= nb; i++)
  {
    j = amap.FindIndex(list->Value(i));
    if (j > 0)
      SelectItem(j, keep);
  }
}

//=================================================================================================

bool Transfer_IteratorOfProcessForFinder::HasStarting() const
{
  return (!thestarts.IsNull());
}

//=================================================================================================

const occ::handle<Transfer_Finder>& Transfer_IteratorOfProcessForFinder::Starting() const
{
  if (thestarts.IsNull())
    throw Standard_NoSuchObject("TransferIterator : No Starting defined at all");
  return thestarts->Value(thecurr);
}
