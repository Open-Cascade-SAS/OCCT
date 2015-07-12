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
#include <Interface_Graph.hxx>
#include <Interface_GraphContent.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_IntList.hxx>
#include <Standard_Transient.hxx>

Interface_GraphContent::Interface_GraphContent ()    {  }

    Interface_GraphContent::Interface_GraphContent
  (const Interface_Graph& agraph)
      {  GetFromGraph(agraph);  }

    Interface_GraphContent::Interface_GraphContent
  (const Interface_Graph& agraph, const Standard_Integer stat)
      {  GetFromGraph(agraph,stat);  }

    Interface_GraphContent::Interface_GraphContent
  (const Interface_Graph& agraph, const Handle(Standard_Transient)& ent)
{
  Interface_EntityIterator list =  agraph.Shareds(ent);
  Standard_Integer nb = list.NbEntities();
  if (nb == 0) return;                             // Liste redefinie a VIDE
  for( ; list.More(); list.Next()) {
    Handle(Standard_Transient) curent = list.Value();
    if (agraph.IsPresent(agraph.EntityNumber(curent))) 
      GetOneItem (curent);
  }
}


    void  Interface_GraphContent::GetFromGraph (const Interface_Graph& agraph)
{
  Standard_Integer nb = agraph.Size();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if (agraph.IsPresent(i)) GetOneItem (agraph.Entity(i));
  }
}

    void  Interface_GraphContent::GetFromGraph
  (const Interface_Graph& agraph, const Standard_Integer stat)
{
  Standard_Integer nb = agraph.Size();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if (agraph.IsPresent(i) && agraph.Status(i) == stat)
      GetOneItem (agraph.Entity(i));
  }
}

    Interface_EntityIterator Interface_GraphContent::Result ()
{
  Interface_EntityIterator iter;    // On transvase ...
  for (Begin(); More(); Next()) iter.GetOneItem(Value());
  return iter;
}


    void Interface_GraphContent::Begin ()
{
  Evaluate();
  Interface_EntityIterator::Start();
}

    void Interface_GraphContent::Evaluate ()
{  }    // par defaut, Evaluate ne fait rien
