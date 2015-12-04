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
#include <IFSelect_SelectionIterator.hxx>
#include <IFSelect_SelectPointed.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Interface_InterfaceError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_SelectDeduct,IFSelect_Selection)

void  IFSelect_SelectDeduct::SetInput (const Handle(IFSelect_Selection)& sel)
      {  thesel = sel;  }

    Handle(IFSelect_Selection)  IFSelect_SelectDeduct::Input () const 
      {  return thesel;  }

    Standard_Boolean  IFSelect_SelectDeduct::HasInput () const 
      {  return (!thesel.IsNull());  }

    Standard_Boolean  IFSelect_SelectDeduct::HasAlternate () const 
{  if (!thealt.IsNull()) return thealt->IsSet();  return Standard_False;  }

    Handle(IFSelect_SelectPointed)&  IFSelect_SelectDeduct::Alternate ()
{
  if (thealt.IsNull()) thealt = new IFSelect_SelectPointed;
  return thealt;
}


    Interface_EntityIterator  IFSelect_SelectDeduct::InputResult
  (const Interface_Graph& G) const
{
  Interface_EntityIterator res;
  if (!thealt.IsNull()) {
    if (thealt->IsSet()) {
      res = thealt->UniqueResult (G);
      thealt->Clear();
      return res;
    }
  }
  if (thesel.IsNull()) return res;
  return  thesel->UniqueResult(G);
}

    void  IFSelect_SelectDeduct::FillIterator
  (IFSelect_SelectionIterator& iter) const 
      {  iter.AddItem(thesel);  }
