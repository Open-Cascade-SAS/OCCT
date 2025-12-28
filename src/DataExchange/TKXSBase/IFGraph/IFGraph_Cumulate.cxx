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

#include <IFGraph_AllShared.hxx>
#include <IFGraph_Cumulate.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Standard_Transient.hxx>

// Cumulation calculation
// Very simple, we note the requested entities, and at the end
// we have the cumulation itself, and as derived info, the duplicates and omissions
// Each overlap corresponds to an increase of ONE in the status
// Status starts at 2, thus when adding an entity, we distinguish well
// between new entities, linked to this call (temporary status 1) and the
// others (status greater than or equal to 2)
IFGraph_Cumulate::IFGraph_Cumulate(const Interface_Graph& agraph)
    : thegraph(agraph)
{
}

void IFGraph_Cumulate::GetFromEntity(const occ::handle<Standard_Transient>& ent)
{
  IFGraph_AllShared iter(thegraph.Model(), ent);
  GetFromIter(iter);
}

void IFGraph_Cumulate::ResetData()
{
  Reset();
  thegraph.Reset();
}

void IFGraph_Cumulate::GetFromIter(const Interface_EntityIterator& iter)
{
  thegraph.GetFromIter(iter, 1, 1, true);
  thegraph.ChangeStatus(1, 2); // once the calculation is done
}

void IFGraph_Cumulate::Evaluate()
{
  Reset();
  GetFromGraph(thegraph); // evaluation already done in the graph
}

Interface_EntityIterator IFGraph_Cumulate::Overlapped() const
{
  Interface_EntityIterator iter;
  int                      nb = thegraph.Size();
  for (int i = 1; i <= nb; i++)
  {
    if (thegraph.IsPresent(i) && thegraph.Status(i) > 2)
      iter.GetOneItem(thegraph.Entity(i));
  }
  return iter;
}

Interface_EntityIterator IFGraph_Cumulate::Forgotten() const
{
  Interface_EntityIterator iter;
  int                      nb = thegraph.Size();
  for (int i = 1; i <= nb; i++)
  {
    if (!thegraph.IsPresent(i))
      iter.GetOneItem(thegraph.Model()->Value(i));
  }
  return iter;
}

Interface_EntityIterator IFGraph_Cumulate::PerCount(const int count) const
{
  Interface_EntityIterator iter;
  int                      nb = thegraph.Size();
  for (int i = 1; i <= nb; i++)
  {
    if (thegraph.IsPresent(i) && thegraph.Status(i) == (count + 1))
      iter.GetOneItem(thegraph.Model()->Value(i));
  }
  return iter;
}

int IFGraph_Cumulate::NbTimes(const occ::handle<Standard_Transient>& ent) const
{
  int num = thegraph.EntityNumber(ent);
  if (num == 0)
    return 0;
  int stat = thegraph.Status(num);
  return stat - 1;
}

int IFGraph_Cumulate::HighestNbTimes() const
{
  int max = 0;
  int nb  = thegraph.Size();
  for (int i = 1; i <= nb; i++)
  {
    if (!thegraph.IsPresent(i))
      continue;
    int count = thegraph.Status(i) - 1;
    if (count > max)
      max = count;
  }
  return max;
}
