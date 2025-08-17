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

#include <IFSelect_SelectExplore.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_IndexedMapOfTransient.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(IFSelect_SelectExplore, IFSelect_SelectDeduct)

IFSelect_SelectExplore::IFSelect_SelectExplore(const Standard_Integer level)
    : thelevel(level)
{
}

Standard_Integer IFSelect_SelectExplore::Level() const
{
  return thelevel;
}

Interface_EntityIterator IFSelect_SelectExplore::RootResult(const Interface_Graph& G) const
{
  //  Attention, here's how we proceed
  //  We have an IndexedMapOfTransient as input (entities already processed/to process)
  //    It is initialized by InputResult
  //  And a map as output (results taken) -> the result will be unique
  //  As input, a cursor of current entity
  //  For each entity, we call Explore. 3 possible cases:
  //    return False, we skip
  //    return True and empty list, we take this entity without going further
  //    return True and non-empty list, we don't take this entity but we
  //      consider its result.
  //      If last level, we take it entirely. Otherwise, it feeds the input

  Standard_Integer              nb = G.Size();
  TColStd_IndexedMapOfTransient entrees(nb);
  TColStd_IndexedMapOfTransient result(nb);
  //  Initialisation
  Standard_Integer         i, j, level = 1, ilev = 0;
  Interface_EntityIterator input;
  input = InputResult(G);
  for (input.Start(); input.More(); input.Next())
    i = entrees.Add(input.Value());
  ilev = entrees.Extent();

  // Exploration
  for (i = 1; i <= nb; i++)
  {
    if (i > entrees.Extent())
      break;
    if (i > ilev)
    {
      level++;
      if (level > thelevel && thelevel > 0)
        break;
      ilev = entrees.Extent();
    }
    Handle(Standard_Transient) ent = entrees.FindKey(i);
    if (ent.IsNull())
      continue;
    Interface_EntityIterator exp;
    if (!Explore(level, ent, G, exp))
      continue;

    //  We take into account : entity to take directly ?
    //  take back input list (level not reached) or result (level reached)
    if (exp.NbEntities() == 0)
    {
      j = result.Add(ent);
      continue;
    }
    else if (level == thelevel)
    {
      for (exp.Start(); exp.More(); exp.Next())
        j = result.Add(exp.Value());
    }
    else
    {
      for (exp.Start(); exp.More(); exp.Next())
        j = entrees.Add(exp.Value());
    }
  }

  //  We collect the result
  Interface_EntityIterator res;
  nb = result.Extent();
  for (j = 1; j <= nb; j++)
    res.AddItem(result.FindKey(j));
  return res;
}

TCollection_AsciiString IFSelect_SelectExplore::Label() const
{
  TCollection_AsciiString labl;
  if (thelevel == 0)
    labl.AssignCat("(Recursive)");
  else if (thelevel > 0)
  {
    char lab[30];
    Sprintf(lab, "(Level %d)", thelevel);
    labl.AssignCat(lab);
  }
  labl.AssignCat(ExploreLabel());
  return labl;
}
