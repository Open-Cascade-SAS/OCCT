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

#include <IFSelect_SelectSuite.ixx>
#include <IFSelect_SelectPointed.hxx>
#include <stdio.h>


IFSelect_SelectSuite::IFSelect_SelectSuite  ()    {  }

    Standard_Boolean  IFSelect_SelectSuite::AddInput
  (const Handle(IFSelect_Selection)& item)
{
  if (item.IsNull()) return Standard_False;
  Handle(IFSelect_Selection) input = Input();
  if (!input.IsNull()) return Standard_False;
  Handle(IFSelect_SelectDeduct) first = Handle(IFSelect_SelectDeduct)::DownCast(item);
  if (first.IsNull()) SetInput(item);
  else thesel.Prepend (item);
  return Standard_True;
}

    void  IFSelect_SelectSuite::AddPrevious
  (const Handle(IFSelect_SelectDeduct)& item)
      {  if (!item.IsNull()) thesel.Prepend (item);  }

    void  IFSelect_SelectSuite::AddNext
  (const Handle(IFSelect_SelectDeduct)& item)
      {  if (!item.IsNull()) thesel.Append (item);  }

    Standard_Integer  IFSelect_SelectSuite::NbItems () const
      {  return thesel.Length();  }

    Handle(IFSelect_SelectDeduct)  IFSelect_SelectSuite::Item
  (const Standard_Integer num) const
      {  return Handle(IFSelect_SelectDeduct)::DownCast(thesel.Value(num));  }

    void  IFSelect_SelectSuite::SetLabel (const Standard_CString lab)
      {  thelab.Clear();  thelab.AssignCat (lab);  }


    Interface_EntityIterator  IFSelect_SelectSuite::RootResult
  (const Interface_Graph& G) const
{
  Interface_EntityIterator iter;
  Standard_Boolean firstin = (HasInput() || HasAlternate());
  if (firstin) iter = InputResult(G);
//   Demarrage : on prend l Input/Alternate SI un des 2 est mis
//   Sinon, on demarre sur la definition de base de la premiere selection

  Standard_Integer i, nb = NbItems();
  for (i = 1; i <= nb; i ++) {
    Handle(IFSelect_SelectDeduct) anitem = Item(i);
    if (firstin) anitem->Alternate()->SetList (iter.Content());
    firstin = Standard_True;  // ensuite c est systematique
    iter = anitem->UniqueResult(G);
  }
  return iter;
}

    TCollection_AsciiString  IFSelect_SelectSuite::Label () const
{
  if (thelab.Length() > 0) return thelab;
  char txt[100];
  sprintf (txt,"Suite of %d Selections",NbItems());
  TCollection_AsciiString lab(txt);
  return lab;
}
