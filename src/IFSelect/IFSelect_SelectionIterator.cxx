// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <IFSelect_SelectionIterator.ixx>


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
