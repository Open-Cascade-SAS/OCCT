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


#include <IFGraph_Compare.hxx>
#include <IFGraph_SubPartsIterator.hxx>
#include <IFSelect_Dispatch.hxx>
#include <IFSelect_Selection.hxx>
#include <IFSelect_SelectionIterator.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Interface_InterfaceError.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_Dispatch,Standard_Transient)

void  IFSelect_Dispatch::SetRootName
  (const Handle(TCollection_HAsciiString)& name)
{
  thename = name;
}

    Standard_Boolean  IFSelect_Dispatch::HasRootName () const
      {  return (!thename.IsNull());  }

    const Handle(TCollection_HAsciiString)&  IFSelect_Dispatch::RootName () const
      {  return thename;  }

    void  IFSelect_Dispatch::SetFinalSelection
  (const Handle(IFSelect_Selection)& sel)
      {  thefinal = sel;  }

    Handle(IFSelect_Selection)  IFSelect_Dispatch::FinalSelection () const 
      {  return thefinal;  }

    IFSelect_SelectionIterator  IFSelect_Dispatch::Selections () const 
{
  IFSelect_SelectionIterator iter;
  iter.AddItem(thefinal);
  for(; iter.More(); iter.Next()) {
    iter.Value()->FillIterator(iter);    // Iterateur qui se court apres
  }
  return iter;
}


    Standard_Boolean  IFSelect_Dispatch::CanHaveRemainder () const 
      {  return Standard_False;  }

    Standard_Boolean  IFSelect_Dispatch::LimitedMax
  (const Standard_Integer , Standard_Integer& max) const 
      {  max = 0;  return Standard_False;  }

    Interface_EntityIterator  IFSelect_Dispatch::GetEntities
  (const Interface_Graph& G) const
      {  return thefinal->UniqueResult(G);  }

    Interface_EntityIterator  IFSelect_Dispatch::Packeted
  (const Interface_Graph& G) const
{
  Interface_EntityIterator total  = GetEntities(G);
  Interface_EntityIterator remain = Remainder(G);
  if (remain.NbEntities() == 0) return total;
//  sinon, faire la difference !
  IFGraph_Compare GC(G);
  GC.GetFromIter (total, Standard_True);
  GC.GetFromIter (remain,Standard_False);
  return GC.FirstOnly();
}

    Interface_EntityIterator  IFSelect_Dispatch::Remainder
  (const Interface_Graph& ) const
      {  Interface_EntityIterator iter;  return iter;  }    // par defaut vide
