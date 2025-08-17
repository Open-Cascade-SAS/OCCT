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

#include <IFGraph_ExternalSources.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Standard_Transient.hxx>

// ExternalSources exploits the results stored in the Graph on Sharings
// Either the "Sharings" of entities noted by GetFromEntity and GetFromIter
// Those of the "Sharings" that were not already noted are ExternalSources
// The status:
// - The starting entities are at Status 0
// - The NEW Sharing entities (ExternalSources) are at Status 1
IFGraph_ExternalSources::IFGraph_ExternalSources(const Interface_Graph& agraph)
    : thegraph(agraph)
{
}

void IFGraph_ExternalSources::GetFromEntity(const Handle(Standard_Transient)& ent)
{
  thegraph.GetFromEntity(ent, Standard_True);
}

void IFGraph_ExternalSources::GetFromIter(const Interface_EntityIterator& iter)
{
  thegraph.GetFromIter(iter, 0);
}

void IFGraph_ExternalSources::ResetData()
{
  Reset();
  thegraph.Reset();
}

void IFGraph_ExternalSources::Evaluate()
{
  Reset();
  thegraph.RemoveStatus(1);
  Standard_Integer nb = thegraph.Size();
  for (Standard_Integer i = 1; i <= nb; i++)
  {
    if (thegraph.IsPresent(i) && thegraph.Status(i) == 0)
      thegraph.GetFromIter(thegraph.Sharings(thegraph.Entity(i)), 1);
  }
  GetFromGraph(thegraph, 1);
}

Standard_Boolean IFGraph_ExternalSources::IsEmpty()
{
  Evaluate();
  Standard_Integer nb = thegraph.Size();
  for (Standard_Integer i = 1; i <= nb; i++)
  {
    if (thegraph.IsPresent(i) || thegraph.Status(i) == 1)
      return Standard_False;
  }
  return Standard_True;
}
