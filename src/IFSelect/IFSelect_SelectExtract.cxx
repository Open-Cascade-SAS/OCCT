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


#include <IFSelect_SelectExtract.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_SelectExtract,IFSelect_SelectDeduct)

IFSelect_SelectExtract::IFSelect_SelectExtract ()
      {  thesort = Standard_True;  }

    Standard_Boolean  IFSelect_SelectExtract::IsDirect () const 
      {  return thesort;  }

    void  IFSelect_SelectExtract::SetDirect (const Standard_Boolean direct)
      {  thesort = direct;  }


    Interface_EntityIterator  IFSelect_SelectExtract::RootResult
  (const Interface_Graph& G) const 
{
  Interface_EntityIterator iter;
  Interface_EntityIterator inputer = InputResult(G);  // tient compte de tout
  Handle(Interface_InterfaceModel) model = G.Model();
  Standard_Integer rank = 0;
  for (inputer.Start(); inputer.More(); inputer.Next()) {
    Handle(Standard_Transient) ent = inputer.Value();
    rank ++;
    if (SortInGraph(rank,ent,G) == thesort) iter.GetOneItem(ent);
  }
  return iter;
}


    Standard_Boolean  IFSelect_SelectExtract::SortInGraph
  (const Standard_Integer rank, const Handle(Standard_Transient)& ent,
   const Interface_Graph& G) const
      {  return Sort (rank, ent, G.Model());  }


    TCollection_AsciiString  IFSelect_SelectExtract::Label () const
{
  TCollection_AsciiString labl;
  if ( thesort) labl.AssignCat("Picked: ");
  if (!thesort) labl.AssignCat("Removed: ");
  labl.AssignCat(ExtractLabel());
  return labl;
}
