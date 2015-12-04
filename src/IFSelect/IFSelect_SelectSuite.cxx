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


#include <IFSelect_SelectDeduct.hxx>
#include <IFSelect_Selection.hxx>
#include <IFSelect_SelectPointed.hxx>
#include <IFSelect_SelectSuite.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Interface_InterfaceError.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(IFSelect_SelectSuite,IFSelect_SelectDeduct)

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
