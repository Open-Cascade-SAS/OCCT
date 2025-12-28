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

#include <IFGraph_Cumulate.hxx>
#include <IFSelect_SelectCombine.hxx>
#include <IFSelect_Selection.hxx>
#include <IFSelect_SelectionIterator.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_SelectCombine, IFSelect_Selection)

IFSelect_SelectCombine::IFSelect_SelectCombine() = default;

//      {  thelist = new IFSelect_SequenceOfSelection();  }

int IFSelect_SelectCombine::NbInputs() const
{
  return thelist.Length();
}

occ::handle<IFSelect_Selection> IFSelect_SelectCombine::Input(const int num) const
{
  return thelist.Value(num);
}

int IFSelect_SelectCombine::InputRank(const occ::handle<IFSelect_Selection>& sel) const
{
  if (sel.IsNull())
    return 0;
  int i, nb = thelist.Length();
  for (i = 1; i <= nb; i++)
    if (sel == thelist.Value(i))
      return i;
  return 0;
}

void IFSelect_SelectCombine::Add(const occ::handle<IFSelect_Selection>& sel, const int atnum)
{
  if (atnum <= 0 || atnum > thelist.Length())
    thelist.Append(sel);
  else
    thelist.InsertBefore(atnum, sel);
}

bool IFSelect_SelectCombine::Remove(const occ::handle<IFSelect_Selection>& sel)
{
  return Remove(InputRank(sel));
}

bool IFSelect_SelectCombine::Remove(const int num)
{
  if (num <= 0 || num > thelist.Length())
    return false;
  thelist.Remove(num);
  return true;
}

bool IFSelect_SelectCombine::HasUniqueResult() const
{
  return true;
}

void IFSelect_SelectCombine::FillIterator(IFSelect_SelectionIterator& iter) const
{
  iter.AddList(thelist);
}
