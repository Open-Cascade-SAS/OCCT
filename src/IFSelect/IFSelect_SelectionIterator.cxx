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


#include <IFSelect_Selection.hxx>
#include <IFSelect_SelectionIterator.hxx>
#include <Standard_NoSuchObject.hxx>

IFSelect_SelectionIterator::IFSelect_SelectionIterator ()
{
  thecurr = 1;
  thelist = new IFSelect_HSeqOfSelection();
}

    IFSelect_SelectionIterator::IFSelect_SelectionIterator
  (const Handle(IFSelect_Selection)& sel)
{
  thecurr = 1;
  thelist = new IFSelect_HSeqOfSelection();
  sel->FillIterator(*this);
}

    void  IFSelect_SelectionIterator::AddFromIter (IFSelect_SelectionIterator& iter)
      {  for (; iter.More(); iter.Next()) AddItem(iter.Value());  }

    void  IFSelect_SelectionIterator::AddItem
  (const Handle(IFSelect_Selection)& sel)
      {  if (!sel.IsNull()) thelist->Append(sel);  }

    void  IFSelect_SelectionIterator::AddList
  (const IFSelect_TSeqOfSelection& list)
{
  Standard_Integer nb = list.Length();  // <list> Pas Handle  <thelist> Handle
  for (Standard_Integer i = 1; i <= nb; i ++) thelist->Append(list.Value(i));
}

    Standard_Boolean  IFSelect_SelectionIterator::More () const 
      {  return (thecurr <= thelist->Length());  }

    void  IFSelect_SelectionIterator::Next () 
      {  thecurr ++;  }

    const Handle(IFSelect_Selection)&  IFSelect_SelectionIterator::Value () const 
      {  return thelist->Value(thecurr);  }
