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

#include <IFGraph_Articulations.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Standard_Transient.hxx>

// Articulation Points of a Graph: these are the "required passages" of the graph
// Algorithm taken from Sedgewick, p 392
IFGraph_Articulations::IFGraph_Articulations(const Interface_Graph& agraph,
                                             const bool whole)
    : thegraph(agraph)
{
  if (whole)
    thegraph.GetFromModel();
}

void IFGraph_Articulations::GetFromEntity(const occ::handle<Standard_Transient>& ent)
{
  thegraph.GetFromEntity(ent, true);
}

void IFGraph_Articulations::GetFromIter(const Interface_EntityIterator& iter)
{
  thegraph.GetFromIter(iter, 0);
}

void IFGraph_Articulations::ResetData()
{
  Reset();
  thegraph.Reset();
  thelist = new NCollection_HSequence<int>();
}

void IFGraph_Articulations::Evaluate()
{
  //  Algorithm, see Sedgewick "Algorithms", p 392
  thelist = new NCollection_HSequence<int>();
  //  Use of Visit
  int nb = thegraph.Size();
  for (int i = 1; i <= nb; i++)
  {
    thenow = 0;
    if (thegraph.IsPresent(i))
      Visit(i);
  }
  //  Result in thelist
  Reset();
  int nbres = thelist->Length();
  for (int ires = 1; ires <= nbres; ires++)
  {
    int num = thelist->Value(ires);
    GetOneItem(thegraph.Model()->Value(num));
  }
}

int IFGraph_Articulations::Visit(const int num)
{
  thenow++;
  thegraph.SetStatus(num, thenow);
  int min = thenow;

  for (Interface_EntityIterator iter = thegraph.Shareds(thegraph.Entity(num)); iter.More();
       iter.Next())
  {
    const occ::handle<Standard_Transient>& ent    = iter.Value();
    int                  nument = thegraph.EntityNumber(ent);
    if (!thegraph.IsPresent(num))
    {
      thegraph.GetFromEntity(ent, false);
      nument = thegraph.EntityNumber(ent);
    }
    int statent = thegraph.Status(nument); // not reevaluated
    if (statent == 0)
    {
      int mm = Visit(nument);
      if (mm < min)
        min = mm;
      if (mm > thegraph.Status(num))
        thelist->Append(num); // WE HAVE ONE: num
    }
    else if (statent < min)
      min = statent;
  }
  return min;
}
